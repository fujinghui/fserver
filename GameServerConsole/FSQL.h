#pragma once
#include <windows.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
class FSQL {
private:
	SQLHENV env;
	SQLHDBC dbc;
	SQLHSTMT stmt;
	SQLRETURN ret;

	SQLCHAR sno[64];
	SQLINTEGER grade;
	SQLINTEGER cbSno, cbGrade;
public:
	FSQL();
	~FSQL();
	bool execute(const char sql_str[]);
};

class CSQL {
private:
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt;
	SQLRETURN retcode;
public:
	CSQL();
	~CSQL();
	bool execute(const char sql_str[]);
};