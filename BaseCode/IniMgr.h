#pragma once

#include <map>
#include <string>

#include "Singleton.h"
#include "MyIni.h"

class CIniMgr : public Singleton<CIniMgr>
{
public:
	CIniMgr(void);
	~CIniMgr(void);

	const char* GetValue(const char *pszFile, const char *pszSection, const char *pszKey, const char *pszDefault);
private:
	std::map<std::string, CMyIni> m_mapIni;
};

