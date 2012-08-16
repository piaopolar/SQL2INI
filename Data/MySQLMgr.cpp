#include "StdAfx.h"

#include "../BaseCode/BaseFunc.h"

#include "MySQLMgr.h"

// ============================================================================
// ==============================================================================

CMySQLMgr::CMySQLMgr(void)
{
	mysql_init(&m_mysql);
}

// ============================================================================
// ==============================================================================
CMySQLMgr::~CMySQLMgr(void)
{
}

// ============================================================================
// ==============================================================================
bool CMySQLMgr::Connect(const char *host, const char *user, const char *passwd, const char *db, unsigned int port)
{
	if (!mysql_real_connect(&m_mysql, host, user, passwd, db, port, 0, 0)) {
		LogInfoIn("数据库连接失败");
		return false;
	}

	LogInfoIn("数据库链接成功");

	return true;
}

// ============================================================================
// ==============================================================================
std::string CMySQLMgr::GetValue(const char *pszQueryField,
								const char *pszTable,
								const char *pszKeyField,
								const char *pszKeyValue)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~
	char szQueryCmd[MAX_STRING];
	//~~~~~~~~~~~~~~~~~~~~~~~~

	_snprintf(szQueryCmd, sizeof(szQueryCmd), "select %s from %s where %s = %s", pszQueryField, pszTable, pszKeyField,
			  pszKeyValue);

	//~~~~~~~~~~~
	MYSQL_RES *res;
	MYSQL_ROW row;
	//~~~~~~~~~~~

	if (mysql_query(&m_mysql, szQueryCmd)) {
		LogInfoIn("MySQL query failed : %s", szQueryCmd);
		return "";
	}

	if (!(res = mysql_store_result(&m_mysql))) {
		LogInfoIn("MySQL store result failed : %s", szQueryCmd);
		return "";
	}

	//~~~~~~~~~~~~~~~
	std::string strRet;
	//~~~~~~~~~~~~~~~

	while ((row = mysql_fetch_row(res))) {
		for (int i = 0; i < mysql_num_fields(res); i++) {
			strRet = row[i];
			MyTrim(strRet);
		}
	}

	mysql_free_result(res);
	return strRet;
}

// ============================================================================
// ==============================================================================
std::vector<std::vector<std::string> > CMySQLMgr::QueryFields(const char *pszQueryFields, const char *pszTable)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::vector<std::vector<std::string> > vecRet;
	char szQueryCmd[MAX_STRING];
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	_snprintf(szQueryCmd, sizeof(szQueryCmd), "select %s from %s", pszQueryFields, pszTable);

	//~~~~~~~~~~~
	MYSQL_RES *res;
	MYSQL_ROW row;
	//~~~~~~~~~~~

	if (mysql_query(&m_mysql, szQueryCmd)) {
		LogInfoIn("MySQL query failed (%s) : %s", mysql_error(&m_mysql), szQueryCmd);
		return vecRet;
	}

	if (!(res = mysql_store_result(&m_mysql))) {
		LogInfoIn("MySQL store result failed (%s) : %s", mysql_error(&m_mysql), szQueryCmd);
		return vecRet;
	}

	while ((row = mysql_fetch_row(res))) {

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		std::vector<std::string> vecRow;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		for (int i = 0; i < mysql_num_fields(res); i++) {
			std::string strRow = row[i];
			MyTrim(strRow);
			vecRow.push_back(strRow);
		}

		vecRet.push_back(vecRow);
	}

	mysql_free_result(res);
	return vecRet;
}
