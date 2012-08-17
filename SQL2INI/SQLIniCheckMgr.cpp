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

struct VALUE_INFO
{
	std::string strKey;
	std::string strValueFormat;
	int nValueIndex;
	VALUE_INFO() {
		nValueIndex = -1;
	}
};

// ============================================================================
// ==============================================================================

VALUE_INFO GetValueInfo(char *pszLine)
{
	//~~~~~~~~~~~~~~~
	VALUE_INFO infoRet;
	//~~~~~~~~~~~~~~~

	infoRet.strKey = pszLine;
	std::string::size_type sizePos = infoRet.strKey.find("=");
	if (sizePos == std::string::npos) {
		return infoRet;
	}

	infoRet.strValueFormat = infoRet.strKey.substr(sizePos + 1);
	infoRet.strKey = infoRet.strKey.substr(0, sizePos);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::string strTransPart = GetTransPart(pszLine);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (!strTransPart.empty()) {
		ReplaceStdString(infoRet.strValueFormat, strTransPart, "%s");
		MyTrim(infoRet.strValueFormat);
		sscanf_s(strTransPart.c_str() + 1, "%d", &infoRet.nValueIndex);
	}

	return infoRet;
}

// ============================================================================
// ==============================================================================
bool Rewrite(const SECTION_INFO &rInfoSection,
			 const VALUE_INFO &rInfoValue,
			 const std::vector<std::string> &rSQLRow,
			 const char *pszFile)
{
	//~~~~~~~~~~~~~~~~~~~~~~~
	char szSection[MAX_STRING];
	//~~~~~~~~~~~~~~~~~~~~~~~

	_snprintf_s(szSection, sizeof(szSection), rInfoSection.strFormat.c_str(), rSQLRow.at(rInfoSection.nIndex - 1).c_str());

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	const char *pszIniValue = CIniMgr::GetInstance().GetValue(pszFile, szSection, rInfoValue.strKey.c_str(), "");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (pszIniValue[0] == 0) {
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~
	char szRuleValue[MAX_STRING];
	//~~~~~~~~~~~~~~~~~~~~~~~~~

	_snprintf_s(szRuleValue, sizeof(szRuleValue), rInfoValue.strValueFormat.c_str(),
				rSQLRow.at(rInfoValue.nValueIndex - 1).c_str());
	if (ValueStrCmp(pszIniValue, szRuleValue)) {

		//~~~~~~~~~~~
		int nValue = 0;
		//~~~~~~~~~~~

		sscanf_s(szRuleValue, "%d", &nValue);
		_snprintf_s(szRuleValue, sizeof(szRuleValue), "%d", nValue);
		if (!WritePrivateProfileString(szSection, rInfoValue.strKey.c_str(), szRuleValue, pszFile)) {
			LogInfoIn("–¥»Î %s  ß∞‹", pszFile);
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
	const char *pszFile = "$FILE";
	const char *pszTable = "$TABLE";
	const char *pszField = "$FIELD";
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

		if (strstr(szLine, pszTable) == szLine) {
			stackTable.push(szLine + strlen(pszTable));
			continue;
		}

		if (strstr(szLine, pszField) == szLine) {
			vecRet = CMySQLMgr::GetInstance().QueryFields(szLine + strlen(pszField), stackTable.top());
			continue;
		}

		if (cFirst == '[') {
			infoSection = GetSectionInfo(szLine);
			continue;
		}

		if (strstr(szLine, "=")) {

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			VALUE_INFO infoValue = GetValueInfo(szLine);
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

					if (Rewrite(infoSection, infoValue, *itSQLRow, stackFile.top())) {
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
