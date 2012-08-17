#include "StdAfx.h"

#include "BaseFunc.h"

#include "IniMgr.h"

// ============================================================================
// ==============================================================================

CIniMgr::CIniMgr(void)
{
}

// ============================================================================
// ==============================================================================
CIniMgr::~CIniMgr(void)
{
}

// ============================================================================
// ==============================================================================
const char *CIniMgr::GetValue(const char *pszFile, const char *pszSection, const char *pszKey, const char *pszDefault)
{
	if (m_mapIni.find(pszFile) == m_mapIni.end()) {
		m_mapIni[pszFile] = CMyIni(pszFile);
	}

	return m_mapIni[pszFile].GetValue(pszSection, pszKey, pszDefault);
}

// ============================================================================
// ==============================================================================
int CIniMgr::GetValue(const char *pszFile, const char *pszSection, const char *pszKey, int nDefault)
{
	//~~~~~~~~~~~~~~~~~~~~~~~
	char szDefault[MAX_STRING];
	//~~~~~~~~~~~~~~~~~~~~~~~

	_snprintf_s(szDefault, sizeof(szDefault), "%d", nDefault);
	sscanf_s(this->GetValue(pszFile, pszSection, pszKey, szDefault), "%d", &nDefault);
	return nDefault;
}
