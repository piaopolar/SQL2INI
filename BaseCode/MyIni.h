#pragma once

#include <map>
#include <string>

class CMyIni
{
public:
	CMyIni(void);
	CMyIni(const char *pszFile);
	~ CMyIni(void);


	bool Open(const char *pszFileName);
	const char *GetValue(const char *pszApp, const char *pszKey, const char *pszDefault) const;
private:
	std::map<std::string, std::map<std::string, std::string> > m_mapValue;
};
