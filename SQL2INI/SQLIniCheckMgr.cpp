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
int CSQLIniCheckMgr::Fix(const char *pszRuleFile)
{
	//~~~~~~~~~~~~~~~~~~~~
	char szLine[MAX_STRING];
	FILE *pFileRule = NULL;
	//~~~~~~~~~~~~~~~~~~~~

	fopen_s(&pFileRule, pszRuleFile, "r");
	if (NULL == pFileRule || pszRuleFile[0] == 0) {
		return 0;
	}

	LogInfoIn("	%s checking ...", pszRuleFile);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nCount = 0;
	BOOL bForce = FALSE;
	const char *pszFile = "$FILE";
	const char *pszTable = "$TABLE";
	const char *pszField = "$FIELD";
	const char *pszForce = "$FORCE";
	const char *pszSQL = "$SQL";
	std::stack<CString> stackFile;
	std::stack<CString> stackTable;
	std::stack<CMyIni> stackIni;
	std::string strFileList;
	std::map<std::string, int> mapFiles;
	std::vector<std::vector<std::string> > vecRet;
	SECTION_INFO infoSection;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

		if (cFirst == '[') {
			infoSection = GetSectionInfo(szLine);
			continue;
		}

		if (strstr(szLine, "=")) {

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			VALUE_FORMAT infoValue = GetValueFormat(szLine);
			std::vector<std::vector<std::string> >::const_iterator itSQLRow;
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			for (itSQLRow = vecRet.begin(); itSQLRow != vecRet.end(); ++itSQLRow) {
				if (stackFile.top().GetLength()) {

					//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
					std::string strFile = stackFile.top();
					//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

					if (mapFiles.empty()) {
						strFileList += strFile;
						mapFiles[strFile] = 1;
					} else if (mapFiles.find(strFile) == mapFiles.end()) {
						strFileList += ",";
						strFileList += strFile;
						mapFiles[strFile] = 1;
					}

					if (Rewrite(infoSection, infoValue, *itSQLRow, stackFile.top(), bForce)) {
						++nCount;
					}
				}
			}
		}
	}

	fclose(pFileRule);
	if (nCount) {
		LogInfoIn("	%s done with rewrite %d value(s) of %s", pszRuleFile, nCount, strFileList.c_str());
	} else {
		LogInfoIn("	%s done with no change", pszRuleFile);
	}

	return nCount;
}

// ============================================================================
// ==============================================================================
bool CSQLIniCheckMgr::Connect(const char *host, const char *user, const char *passwd, const char *db, unsigned int port)
{
	return CMySQLMgr::GetInstance().Connect(host, user, passwd, db, port);
}
