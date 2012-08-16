#include "StdAfx.h"

#include <cstdio>
#include "../BaseCode/BaseFunc.h"

#include "MyIni.h"

// ============================================================================
// ==============================================================================

CMyIni::CMyIni(void)
{
}

// ============================================================================
// ==============================================================================
CMyIni::CMyIni(const char *pszFile)
{
	this->Open(pszFile);
}

// ============================================================================
// ==============================================================================
CMyIni::~CMyIni(void)
{
}

// ============================================================================
// ==============================================================================
bool CMyIni::Open(const char *pszFileName)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(pszFileName, "r");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		LogInfoIn("打开文件 %s 失败", pszFileName);
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~
	char szLine[MAX_STRING];
	std::string strSection;
	//~~~~~~~~~~~~~~~~~~~~

	while (fgets(szLine, sizeof(szLine), pFile)) {

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

		if (cFirst == '[') {

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			char *pszEnd = strstr(szLine, "]");
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			if (pszEnd) {
				*pszEnd = 0;
				strSection = szLine + 1;
			}

			continue;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		char *pPos = strstr(szLine, "=");
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (pPos) {
			*pPos = 0;

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			std::string strValue = pPos + 1;
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			ReplaceStdString(strValue, "\n", "");
			ReplaceStdString(strValue, "\r", "");
			m_mapValue[strSection][szLine] = strValue;
		}
	}

	fclose(pFile);
	return true;
}

// ============================================================================
// ==============================================================================
const char *CMyIni::GetValue(const char *pszApp, const char *pszKey, const char *pszDefault) const
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<std::string, std::map<std::string, std::string> >::const_iterator it = m_mapValue.find(pszApp);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (it == m_mapValue.end()) {
		return pszDefault;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<std::string, std::string>::const_iterator itIn = it->second.find(pszKey);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (itIn == it->second.end()) {
		return pszDefault;
	}

	return itIn->second.c_str();
}
