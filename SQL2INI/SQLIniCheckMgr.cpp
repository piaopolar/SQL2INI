#include "StdAfx.h"

#include <map>
#include <stack>
#include <string>
#include "../BaseCode/BaseFunc.h"
#include "../BaseCode/IniMgr.h"
#include "../Data/MySQLMgr.h"

#include "SQLIniCheckMgr.h"

namespace
{
struct SECTION_INFO
{
	int nIndex;
	std::string strFormat;
	SECTION_INFO() {
		nIndex = -1;
	}
};

struct REPLACE_INFO
{
	REPLACE_INFO(const char *pszSrc, const char *pszDst) {
		strSrc = pszSrc ? pszSrc : "";
		strDst = pszDst ? pszDst : "";
	}

	std::string strSrc;
	std::string strDst;
};

// ============================================================================
// ==============================================================================

std::string GetTransPart(char *pszLine)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::string strPart = pszLine;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~

	std::string::size_type sizePos = strPart.find('$');
	if (sizePos == std::string::npos) {
		return "";
	}

	strPart = strPart.substr(sizePos);

	//~~~~~~~~~~~
	unsigned i = 0;
	//~~~~~~~~~~~

	for (i = 1; i < strPart.length(); ++i) {
		if (strPart[i] < '0' || strPart[i] > '9') {
			break;
		}
	}

	strPart = strPart.substr(0, i);
	return strPart;
}

// ============================================================================
// ==============================================================================
SECTION_INFO GetSectionInfo(char *pszLine)
{
	//~~~~~~~~~~~~~~~~~
	SECTION_INFO infoRet;
	//~~~~~~~~~~~~~~~~~

	infoRet.strFormat = pszLine;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::string strTransPart = GetTransPart(pszLine);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (!strTransPart.empty()) {
		ReplaceStdString(infoRet.strFormat, "[", "");
		ReplaceStdString(infoRet.strFormat, "]", "");
		ReplaceStdString(infoRet.strFormat, strTransPart, "%s");
		MyTrim(infoRet.strFormat);
		sscanf_s(strTransPart.c_str() + 1, "%d", &infoRet.nIndex);
	}

	return infoRet;
}

struct VALUE_FORMAT
{
	std::string strKeyFormat;
	std::string strValueFormat;
};

// ============================================================================
// ==============================================================================

VALUE_FORMAT GetValueFormat(char *pszLine)
{
	//~~~~~~~~~~~~~~~~~
	VALUE_FORMAT infoRet;
	//~~~~~~~~~~~~~~~~~

	infoRet.strKeyFormat = pszLine;
	std::string::size_type sizePos = infoRet.strKeyFormat.find("=");
	if (sizePos == std::string::npos) {
		return infoRet;
	}

	infoRet.strValueFormat = infoRet.strKeyFormat.substr(sizePos + 1);
	infoRet.strKeyFormat = infoRet.strKeyFormat.substr(0, sizePos);

	return infoRet;
}

// ============================================================================
// ==============================================================================
std::string FormatValueStr(std::string str)
{
	std::string::size_type sizePos = str.find_first_not_of('0');
	if (sizePos == std::string::npos) {
		str = "0";
	} else {
		str = str.substr(sizePos);
	}

	return str;
}

// ============================================================================
// ==============================================================================
std::string GetValue(std::string strFormat, const std::vector<std::string> &rVec)
{
	for (unsigned i = rVec.size(); i > 0; --i) {

		//~~~~~~~~~~~~~~~~~~~~~~~
		char szReplace[MAX_STRING];
		//~~~~~~~~~~~~~~~~~~~~~~~

		_snprintf_s(szReplace, sizeof(szReplace), "$%d", i);
		ReplaceStdString(strFormat, szReplace, FormatValueStr(rVec.at(i - 1)));
	}

	return strFormat;
}

// ============================================================================
// ==============================================================================
bool Rewrite(const SECTION_INFO &rInfoSection,
			 const VALUE_FORMAT &rInfoValue,
			 const std::vector<std::string> &rSQLRow,
			 const char *pszFile,
			 BOOL bForce)
{
	//~~~~~~~~~~~~~~~~~~~~~~~
	char szSection[MAX_STRING];
	//~~~~~~~~~~~~~~~~~~~~~~~

	_snprintf_s(szSection, sizeof(szSection), rInfoSection.strFormat.c_str(), rSQLRow.at(rInfoSection.nIndex - 1).c_str());

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::string strKey = GetValue(rInfoValue.strKeyFormat, rSQLRow);
	const char *pszIniValue = CIniMgr::GetInstance().GetValue(pszFile, szSection, strKey.c_str(), "");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (pszIniValue[0] == 0 && !bForce) {
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::string strRuleValue = GetValue(rInfoValue.strValueFormat, rSQLRow);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (ValueStrCmp(strRuleValue.c_str(), pszIniValue)) {
		if (!WritePrivateProfileString(szSection, strKey.c_str(), strRuleValue.c_str(), pszFile)) {
			LogInfoIn("		写入 %s 失败", pszFile);
		} else {
			LogInfoIn("		修改成功： Section :%s  Key:%s Value %s -> %s", szSection, strKey.c_str(), pszIniValue,
					  strRuleValue.c_str());
		}

		return true;
	}

	return false;
}

bool Gen(const char *pszFormat, 
		 const std::vector<std::vector<std::string> >& rSQLdata,
		 const std::vector<REPLACE_INFO> vecReplace,
		 const char *pszFileGen,
		 BOOL bNew)
{
	FILE* pFileGen = NULL;
	fopen_s(&pFileGen , pszFileGen, bNew ? "w" : "a");

	if (NULL == pFileGen) {
		LogInfoIn("		打开生成文件 %s 失败", pszFileGen);
		return false;
	}

	for (std::vector<std::vector<std::string> >::const_iterator it(rSQLdata.begin()); it != rSQLdata.end(); ++it) {
		std::string strValue = GetValue(pszFormat, *it);
		for (std::vector<REPLACE_INFO>::const_iterator itReplace(vecReplace.begin()); itReplace != vecReplace.end(); ++itReplace) {
			ReplaceStdString(strValue, itReplace->strSrc, itReplace->strDst);
		}

		fprintf_s(pFileGen, "%s\n", strValue.c_str());
	}
	
	fclose(pFileGen);
	LogInfoIn("		成功%s%s，写入%d行数据", bNew ? "生成" : "添加", pszFileGen, rSQLdata.size());
	return true;
}
}

// ============================================================================
// ==============================================================================
CSQLIniCheckMgr::CSQLIniCheckMgr(void)
{
}

// ============================================================================
// ==============================================================================
CSQLIniCheckMgr::~CSQLIniCheckMgr(void)
{
}

// ============================================================================
// ==============================================================================
int CSQLIniCheckMgr::Process(const char *pszRuleFile)
{
	//~~~~~~~~~~~~~~~~~~~~
	char szLine[MAX_STRING];
	FILE *pFileRule = NULL;
	//~~~~~~~~~~~~~~~~~~~~

	fopen_s(&pFileRule, pszRuleFile, "r");
	if (NULL == pFileRule || pszRuleFile[0] == 0) {
		return 0;
	}

	LogInfoIn("	%s processing ...", pszRuleFile);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nCountGen = 0;
	int nCountRewrite = 0;
	BOOL bForce = FALSE;
	BOOL bGenNew = TRUE;
	const char *pszFile = "$FILE";
	const char *pszTable = "$TABLE";
	const char *pszField = "$FIELD";
	const char *pszForce = "$FORCE";
	const char *pszSQL = "$SQL";
	const char *pszGen = "$GEN";
	const char *pszReplace = "$REPLACE";
	std::stack<CString> stackFile;
	std::stack<CString> stackTable;
	std::stack<CMyIni> stackIni;
	std::string strFileListGen;
	std::string strFileListRewrite;
	std::map<std::string, int> mapFilesGen;
	std::map<std::string, int> mapFilesRewrite;
	std::vector<std::vector<std::string> > vecRet;
	std::vector<REPLACE_INFO> vecReplace;
	SECTION_INFO infoSection;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	vecReplace.push_back(REPLACE_INFO("\\r", "\r"));
	vecReplace.push_back(REPLACE_INFO("\\n", "\n"));

	while (fgets(szLine, sizeof(szLine), pFileRule)) {

		//~~~~~~~~~~~~~~~~~~~~
		char cFirst = szLine[0];
		//~~~~~~~~~~~~~~~~~~~~

		switch (cFirst) {
		case 0:
		case '\r':
		case '\n':
		case ';':
			continue;
			break;
		default:
			break;
		}

		MyTrim(szLine);

		if (strstr(szLine, pszFile) == szLine) {
			stackFile.push(szLine + strlen(pszField));
			continue;
		}

		/* $FILE 前的文字均作为注释 */
		if (stackFile.empty()) {
			continue;
		}

		if (strstr(szLine, pszTable) == szLine) {
			stackTable.push(szLine + strlen(pszTable));
			continue;
		}

		if (strstr(szLine, pszField) == szLine) {
			vecRet = CMySQLMgr::GetInstance().QueryFields(szLine + strlen(pszField), stackTable.top());
			continue;
		}

		if (strstr(szLine, pszSQL) == szLine) {
			vecRet = CMySQLMgr::GetInstance().Query(szLine + strlen(pszSQL));
			continue;
		}

		if (strstr(szLine, pszForce) == szLine) {
			sscanf_s(szLine, "%*s%d", &bForce);
		}

		if (strstr(szLine, pszReplace) == szLine) {
			char szSrc[MAX_STRING];
			char szDst[MAX_STRING];
			if (2 == sscanf_s(szLine, "%*s%s%s", szSrc,  _countof(szSrc), szDst,  _countof(szDst))) {
				vecReplace.push_back(REPLACE_INFO(szSrc, szDst));
			}
		}

		if (cFirst == '[') {
			infoSection = GetSectionInfo(szLine);
			continue;
		}

		if (strstr(szLine, pszGen) == szLine) {
			
			std::string strFile = stackFile.top();
			if (strFile.empty()) {
				continue;
			}

			if (mapFilesGen.empty()) {
				strFileListGen += strFile;
				mapFilesGen[strFile] = 1;
			} else if (mapFilesGen.find(strFile) == mapFilesGen.end()) {
				strFileListGen += ",";
				strFileListGen += strFile;
				mapFilesGen[strFile] = 1;
			}
			
			if (!Gen(szLine + strlen(pszGen), vecRet, vecReplace, strFile.c_str(), bGenNew)) {
				continue;
			}

			nCountGen += vecRet.size();
		} else if (strstr(szLine, "=")) {

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			VALUE_FORMAT infoValue = GetValueFormat(szLine);
			std::vector<std::vector<std::string> >::const_iterator itSQLRow;
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			for (itSQLRow = vecRet.begin(); itSQLRow != vecRet.end(); ++itSQLRow) {
				if (stackFile.top().GetLength()) {

					//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
					std::string strFile = stackFile.top();
					//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

					if (mapFilesRewrite.empty()) {
						strFileListRewrite += strFile;
						mapFilesRewrite[strFile] = 1;
					} else if (mapFilesRewrite.find(strFile) == mapFilesRewrite.end()) {
						strFileListRewrite += ",";
						strFileListRewrite += strFile;
						mapFilesRewrite[strFile] = 1;
					}

					if (Rewrite(infoSection, infoValue, *itSQLRow, stackFile.top(), bForce)) {
						++nCountRewrite;
					}
				}
			}
		}
	}

	fclose(pFileRule);
	CString cstrInfo;
	cstrInfo.Format("	%s done with ", pszRuleFile);
	if (nCountGen) {
		CString cstrGen;
		cstrGen.Format("gen %d value(s) of %s", nCountGen, strFileListGen.c_str());
		cstrInfo += cstrGen;
	}

	if (nCountRewrite) {
		CString cstrRewrite;
		cstrRewrite.Format("rewrite %d value(s) of %s", nCountRewrite, strFileListRewrite.c_str());
		cstrInfo += cstrRewrite;
	}

	if (!nCountGen && !nCountRewrite) {
		cstrInfo += "no change";
	}
	
	LogInfoIn(cstrInfo);

	return nCountRewrite;
}

// ============================================================================
// ==============================================================================
bool CSQLIniCheckMgr::Connect(const char *host, const char *user, const char *passwd, const char *db, unsigned int port)
{
	return CMySQLMgr::GetInstance().Connect(host, user, passwd, db, port);
}
