#include "StdAfx.h"
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
