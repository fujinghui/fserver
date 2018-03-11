#include "FSQL.h"
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

FSQL::FSQL() {
	this->ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &env);
	this->ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	this->ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
	
	char base_info[512];
	sprintf_s(base_info, "DRIVER{SQL Server};SERVER=SQLServerODBC;DATABASE=IMDBS;UID=sa;PWD=root");
	std::cout << base_info<<std::endl;
	SQLCHAR outstr[1024];
	SQLSMALLINT outstrlen;
	ret = ::SQLDriverConnect(dbc, NULL, (SQLCHAR*)base_info, SQL_NTS, outstr, sizeof(outstr), &outstrlen, SQL_DRIVER_NOPROMPT);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		MessageBox(NULL, "success", "title", MB_OK);
	}
	int intTemp = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt))
	{
		MessageBox(NULL, "faild", "title", MB_OK);
	}
	else {
		MessageBox(NULL, "success", "title", MB_OK);
	}
} 
//执行sql
bool FSQL::execute(const char sql_str[]) {
	ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
	ret = SQLSetStmtAttr(stmt, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)SQL_BIND_BY_COLUMN, SQL_IS_INTEGER);
	ret = SQLExecDirect(stmt, (SQLCHAR*)(sql_str), SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		return true;
	}
	else
		return false;
}
FSQL::~FSQL() {
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	SQLDisconnect(dbc);
	SQLFreeHandle(SQL_HANDLE_DBC, dbc);
	SQLFreeHandle(SQL_HANDLE_ENV, env);
}

CSQL::CSQL() {
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				//std::cout << "SUCCESS" << std::endl;
				retcode = SQLConnect(hdbc, (SQLCHAR*)"SQLServerODBC", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"root", SQL_NTS);
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

					std::cout << "数据库连接成功！" << std::endl;
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						//std::cout << "success" << std::endl;
					}
				}
			}
		}
	}
}
CSQL::~CSQL() {
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
}
bool CSQL::execute(const char sql_str[]) {
	SQLPrepare(hstmt, (SQLCHAR*)sql_str, strlen(sql_str));
	retcode = SQLExecute(hstmt);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		return true;
	}
	return false;
}
