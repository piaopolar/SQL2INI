#pragma once
#include "../BaseCode/Singleton.h"

class CSQLIniCheckMgr : public Singleton<CSQLIniCheckMgr>
{
public:
	CSQLIniCheckMgr(void);
	~ CSQLIniCheckMgr(void);

	int Process(const char *pszRuleFile);
	bool Connect(const char *host, const char *user, const char *passwd, const char *db, unsigned int port);
};
