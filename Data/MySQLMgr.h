#pragma once

#include <string>
#include <vector>
#include "mysql.h"

#include "../BaseCode/Singleton.h"

class CMySQLMgr : public Singleton<CMySQLMgr>
{
public:
	CMySQLMgr(void);
	~CMySQLMgr(void);

	bool Connect(const char *host,
		const char *user,
		const char *passwd,
		const char *db,
		unsigned int port);
	std::string GetValue(const char* pszQueryField, const char *pszTable, const char *pszKeyField, const char *pszKeyValue);
	std::vector < std::vector < std::string > >  QueryFields(const char* pszQueryFields, const char *pszTable);

	MYSQL m_mysql;
};

