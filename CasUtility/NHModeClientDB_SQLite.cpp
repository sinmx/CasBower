#include "StdAfx.h"
#include "NHModeClientDB_SQLite.h"
#include <cassert>
#include <tchar.h>
#include "CppSQLite3.h"
#include "NHCommonAPI.h"
#include "NHLogAPI.h"

static wchar_t * ANSIToUnicode(const char *str);
static char * UnicodeToANSI(const wchar_t *str);
static wchar_t * UTF8ToUnicode(const char *str);
static char * UnicodeToUTF8(const wchar_t *str);

wchar_t * ANSIToUnicode(const char *str)
{
	// 支持(str==NULL)的情况

	int textlen(0);
	wchar_t *result(NULL);
	textlen = MultiByteToWideChar(CP_ACP, 0, str,-1, NULL, 0); 
	result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t)); 
	memset(result, 0, (textlen+1)*sizeof(wchar_t)); 
	MultiByteToWideChar(CP_ACP, 0, str, -1, (LPWSTR)result,textlen); 
	return result; 
}

char * UnicodeToANSI(const wchar_t *str)
{
	// 支持(str==NULL)的情况

	char* result(NULL);
	int textlen(0);
	textlen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	result =(char *)malloc((textlen+1)*sizeof(char));
	memset(result, 0, sizeof(char)*(textlen+1));
	WideCharToMultiByte(CP_ACP, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}

wchar_t * UTF8ToUnicode(const char *str)
{
	// 支持(str==NULL)的情况

	int textlen(0);
	wchar_t *result(NULL);
	textlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0); 
	result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t)); 
	memset(result, 0, (textlen+1)*sizeof(wchar_t)); 
	MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)result, textlen); 
	return result; 
}

char * UnicodeToUTF8(const wchar_t *str)
{
	// 支持(str==NULL)的情况

	char *result(NULL);
	int textlen(0);
	textlen = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	result = (char *)malloc((textlen+1)*sizeof(char));
	memset(result, 0, sizeof(char)*(textlen+1));
	WideCharToMultiByte(CP_UTF8, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}

// 打开SQLite数据库
// (in)sqlite3:sqlite3指针应用
// (return)错误代码:
// 0x00:正常，
// 非0x00:异常(x01:获取文件路径失败,0x02:执行打开数据库操作失败,......)
DWORD OpenSQLiteDB(sqlite3* &pSQLite3)
{
	DWORD dwReturn(0x00);

	wchar_t *pwchPath = new wchar_t[MAX_PATH]();
	ZeroMemory(pwchPath, sizeof(pwchPath));
	if (0x00 != CNHCommonAPI::GetFilePathEx(L"DB", L"CastleDB.db", pwchPath))
	{
		// 获取文件路径失败
		// 包括路径不存在、路径存在但文件不存在

		delete[] pwchPath;
		pwchPath = NULL;

		dwReturn = 0x01;
		return dwReturn;
	}

	// UNICODE TO UTF8
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, NULL, pwchPath, -1, NULL, 0, NULL, FALSE);
	char *pchPath = new char[dwNum];
	WideCharToMultiByte(CP_UTF8, NULL, pwchPath, -1, pchPath, dwNum, NULL, FALSE);

	// 打开数据库
	int nResult = sqlite3_open(pchPath, &pSQLite3);

	delete []pchPath;
	pchPath = NULL;
	delete[] pwchPath;
	pwchPath = NULL;

	if (SQLITE_OK != nResult)
	{
		// 打开数据库失败

		dwReturn = 0x02;
	}
	else
	{
		// 打开数据库成功

		dwReturn = 0x00;
	}

	return dwReturn;
}

// 关闭SQLite数据库
// (in)sqlite3:sqlite3指针应用
// (return)错误代码:
// 0x00:正常，
// 非0x00:异常(x01:执行关闭数据库操作失败,......)
DWORD CloseSQLiteDB(sqlite3 *pSQLite3)
{
	assert(NULL != pSQLite3);

	DWORD dwReturn(0x00);

	// 关闭数据库
	int nResult = sqlite3_close(pSQLite3);

	if (SQLITE_OK != nResult)
	{
		// 关闭数据库失败

		dwReturn = 0x01;
	}
	else
	{
		// 关闭数据库成功

		dwReturn = 0x00;
	}

	return dwReturn;
}

// 选择TestAmount表
// (in)pwchDepartNumber:检测线编号字符指针
// (in)st:时间结构引用
// (in)nAmount:数量
// (return)错误代码:
// 0x00:正常，
// 非0x00:异常(x01:打开数据库失败,0x02:执行SQL语句失败,......)
DWORD SelectTestAmount(const wchar_t * const pwchDepartNumber, const SYSTEMTIME &st, int &nRecordAmount, int &nAmount)
{
	// 0x00:正常
	// 0x01:打开数据库失败
	// 0x02:执行SQL语句失败
	DWORD dwReturn(0x00);

	sqlite3* pSQLite3(NULL);

	DWORD dwResult = OpenSQLiteDB(pSQLite3);
	if (0x00 != dwResult)
	{
		// 打开数据库失败

		dwReturn = 0x01;
		return dwReturn;
	}

	// SQLlite使用注意：
	// 经测试，SQL语句日期必须为YYYY-MM-DD，月日必须是2位，才能与数据库日期匹配
	// 例如：数据库为2010-1-11，'2010-01-11'才能匹配，'2010-1-11'不能匹配
	// By Cui

	wchar_t *pwchSQL = new wchar_t[MAX_PATH]();
	swprintf(pwchSQL, MAX_PATH, L"select DepartNumber,TestDate,AllAmount from TestAmount where DepartNumber=\'%s\' and TestDate=\'%04hd-%02hd-%02hd 00:00:00\'", pwchDepartNumber, st.wYear, st.wMonth, st.wDay);
	// UNICODE TO ANSI
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, pwchSQL, -1, NULL, 0, NULL, FALSE);
	char *pchSQL = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, pwchSQL, -1, pchSQL, dwNum, NULL, FALSE);

	// 获取检测数量
	char **dbResult;
	int nRow(0); 
	int nColumn(0);
	char *pchErrMsg(NULL);
	int nResult = sqlite3_get_table(pSQLite3, pchSQL, &dbResult, &nRow, &nColumn, &pchErrMsg);

	delete[] pchSQL;
	pchSQL = NULL;
	delete[] pwchSQL;
	pwchSQL = NULL;

	if (SQLITE_OK == nResult)
	{
		nRecordAmount = nRow;

		if (0 == nRow)
		{
			// 没有记录
			nAmount = -1;
		}
		else
		{
			// TestAmount表中
			// 每条检测线每天应该只有一个记录
			assert(1 == nRow);

			int nIndex(nColumn);
			for (int i=0; i<nRow; ++i)
			{
				for (int j=0; j<nColumn; ++j)
				{
					if (0 == strcmp(dbResult[j], "DepartNumber"))
					{}
					else if (0 == strcmp(dbResult[j], "TestDate"))
					{}
					else if (0 == strcmp(dbResult[j], "AllAmount"))
					{
						nAmount = atoi(dbResult[nIndex]);
					}
					++nIndex;
				}
			}
		}

		dwReturn = 0x00;
	}
	else
	{
		// 执行SQL语句失败

		dwReturn = 0x02;
	}

	sqlite3_free_table(dbResult);

	CloseSQLiteDB(pSQLite3);

	return dwReturn;
}

// 插入TestAmount表
// (in)pwchDepartNumber:检测线编号字符指针
// (in)st:时间结构引用
// (in)nAmount:数量
// (return)错误代码:
// 0x00:正常，
// 非0x00:异常(x01:打开数据库失败,0x02:执行SQL语句失败,......)
DWORD InsertTestAmount(const wchar_t * const pwchDepartNumber, const SYSTEMTIME &st, const int nAmount)
{
	// 0x00:正常
	// 0x01:打开数据库失败
	// 0x02:执行SQL语句失败
	DWORD dwReturn(0x00);

	sqlite3* pSQLite3(NULL);

	DWORD dwResult = OpenSQLiteDB(pSQLite3);
	if (0x00 != dwResult)
	{
		// 打开数据库失败

		dwReturn = 0x01;
		return dwReturn;
	}

	// SQLlite使用注意：
	// 经测试，SQL语句日期必须为YYYY-MM-DD，月日必须是2位，才能与数据库日期匹配
	// 例如：数据库为2010-1-11，'2010-01-11'才能匹配，'2010-1-11'不能匹配
	// By Cui

	wchar_t *pwchSQL = new wchar_t[MAX_PATH]();
	swprintf(pwchSQL, MAX_PATH, L"insert into TestAmount(DepartNumber,TestDate,AllAmount) values(\'%s',\'%04hd-%02hd-%02hd 00:00:00\',%d)", pwchDepartNumber, st.wYear, st.wMonth, st.wDay, nAmount);
	// UNICODE TO ANSI
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, pwchSQL, -1, NULL, 0, NULL, FALSE);
	char *pchSQL = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, pwchSQL, -1, pchSQL, dwNum, NULL, FALSE);

	char *pchErrMsg(NULL);
	sqlite3_exec(pSQLite3, "begin transaction;", 0, 0, &pchErrMsg);
	int nResult = sqlite3_exec(pSQLite3, pchSQL, 0, 0, &pchErrMsg);
	sqlite3_exec(pSQLite3, "commit transaction;", 0, 0, &pchErrMsg);

	delete[] pchSQL;
	pchSQL = NULL;
	delete[] pwchSQL;
	pwchSQL = NULL;

	if (SQLITE_OK != nResult)
	{
		// 执行SQL语句失败

		dwReturn = 0x02;
	}

	CloseSQLiteDB(pSQLite3);

	return dwReturn;
}

// 更新TestAmount表
// (in)pwchDepartNumber:检测线编号字符指针
// (in)st:时间结构引用
// (in)nAmount:数量
// (return)错误代码:
// 0x00:正常，
// 非0x00:异常(x01:打开数据库失败,0x02:执行SQL语句失败,......)
DWORD UpdateTestAmount(const wchar_t * const pwchDepartNumber, const SYSTEMTIME &st, const int nAmount)
{
	// 0x00:正常
	// 0x01:打开数据库失败
	// 0x02:执行SQL语句失败
	DWORD dwReturn(0x00);

	sqlite3* pSQLite3(NULL);

	DWORD dwResult = OpenSQLiteDB(pSQLite3);
	if (0x00 != dwResult)
	{
		// 打开数据库失败

		dwReturn = 0x01;
		return dwReturn;
	}

	// SQLlite使用注意：
	// 经测试，SQL语句日期必须为YYYY-MM-DD，月日必须是2位，才能与数据库日期匹配
	// 例如：数据库为2010-1-11，'2010-01-11'才能匹配，'2010-1-11'不能匹配
	// By Cui

	wchar_t *pwchSQL = new wchar_t[MAX_PATH]();
	swprintf(pwchSQL, MAX_PATH, L"update TestAmount set AllAmount=%d where DepartNumber=\'%s\' and TestDate=\'%04hd-%02hd-%02hd 00:00:00\'", nAmount, pwchDepartNumber, st.wYear, st.wMonth, st.wDay);
	// UNICODE TO ANSI
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, pwchSQL, -1, NULL, 0, NULL, FALSE);
	char *pchSQL = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, pwchSQL, -1, pchSQL, dwNum, NULL, FALSE);

	char *pchErrMsg(NULL);
	sqlite3_exec(pSQLite3, "begin transaction;", 0, 0, &pchErrMsg);
	int nResult = sqlite3_exec(pSQLite3, pchSQL, 0, 0, &pchErrMsg);
	sqlite3_exec(pSQLite3, "commit transaction;", 0, 0, &pchErrMsg);

	delete[] pchSQL;
	pchSQL = NULL;
	delete[] pwchSQL;
	pwchSQL = NULL;

	if (SQLITE_OK != nResult)
	{
		// 执行SQL语句失败

		dwReturn = 0x02;
	}

	CloseSQLiteDB(pSQLite3);

	return dwReturn;
}

// 写数据库出错信息处理
void WriteDatabaseError(CppSQLite3Exception &e)
{
	// TODO::输出错误信息到输出窗口和日志目录下的日志文件

	char *pchErrMsg = new char[MAX_PATH]();
	sprintf_s(pchErrMsg, MAX_PATH, "ErrorCode:%d ,ErrorMessage:%s\n", e.errorCode(), e.errorMessage());
	OutputDebugStringA(pchErrMsg);

	int nLen = MultiByteToWideChar(CP_ACP, 0, pchErrMsg, -1, NULL, 0);
	wchar_t *wchErrorMsg = new wchar_t[nLen+1]();
	MultiByteToWideChar(CP_ACP, 0, pchErrMsg, -1, wchErrorMsg, nLen);
	CNHLogAPI::WriteLog(LOG_EXP, L"CastleDB", wchErrorMsg);

	if (NULL != pchErrMsg)
	{
		delete[] pchErrMsg;
		pchErrMsg = NULL;
	}

	if (NULL != wchErrorMsg)
	{
		delete[] wchErrorMsg;
		wchErrorMsg = NULL;
	}
}

// 打开SQLite数据库
// (in&out)db:CppSQLite3DB对象引用
// (return)错误代码:
// 0x00:正常，
// 非0x00:异常(0xff:打开数据库操作失败,......)
DWORD OpenSQLiteDB(CppSQLite3DB &db)
{
	DWORD dwReturn(0x00);

	wchar_t wchPath[MAX_PATH];
	ZeroMemory(wchPath, sizeof(wchPath));
	CNHCommonAPI::GetFilePathEx(L"DB", L"CastleDB.db", wchPath);

	char *pchPath(NULL);
	pchPath = UnicodeToANSI(wchPath);

	try
	{
		db.open(pchPath);
	}
	catch (CppSQLite3Exception &e)
	{
		WriteDatabaseError(e);

		dwReturn =  0xFF;
	}

	if (NULL != pchPath)
	{
		free(pchPath);
		pchPath = NULL;
	}

	return dwReturn;
}

// 关闭SQLite数据库
// (in&out)db:CppSQLite3DB对象引用
// (return)错误代码:
// 0x00:正常，
// 非0x00:异常(0xff:关闭数据库操作失败,......)
DWORD CloseSQLiteDB(CppSQLite3DB &db)
{
	DWORD dwReturn(0x00);

	try
	{
		db.close();
	}
	catch (CppSQLite3Exception &e)
	{
		WriteDatabaseError(e);

		dwReturn =  0xFF;
	}

	return dwReturn;
}

// 获取字段值
// (in&out)cs3q:CppSQLite3Query对象引用
// (out)pwchFieldValue:字段值指针
// (in)nFieldValueLength:pwchFieldValue字符长度
// (in)pchFieldName:字段名称指针
// (return)错误代码:
// 0x00:正常，
// 非0x00:异常(0xff:获取字段值操作失败,......)
DWORD GetFieldValue(CppSQLite3Query & cs3q, wchar_t *const pwchFieldValue, const int nFieldValueLength, const char *const pchFieldName)
{
	assert(NULL != pwchFieldValue);
	assert(NULL != pchFieldName);

	DWORD dwReturn(0x00);

	wchar_t *pwch(NULL);

	try
	{
		pwch = UTF8ToUnicode(cs3q.fieldValue(pchFieldName));
		wcsncpy_s(pwchFieldValue, nFieldValueLength, pwch, nFieldValueLength-1);
	}
	catch (CppSQLite3Exception &e)
	{
		WriteDatabaseError(e);

		dwReturn =  0xFF;
	}

	if (NULL != pwch)
	{
		free(pwch);
		pwch = NULL;
	}

	return dwReturn;
}

CNHModeClientDB_SQLite::CNHModeClientDB_SQLite(void)
{
}

CNHModeClientDB_SQLite::~CNHModeClientDB_SQLite(void)
{
}


DWORD CNHModeClientDB_SQLite::GetDboCertificateOfDepartInfo(wchar_t const * pwchPath, wchar_t const * pwchDepartNumber, BYTE * const pbCertificateAuthentication, const UINT uiCALength, long& lDataSize)
{
	DWORD dwRet(0x00);

	char* pchPath(NULL);
	char* pchSql(NULL);

	try
	{
		CppSQLite3DB db;
		pchPath = UnicodeToANSI(pwchPath);
		db.open(pchPath);

		wchar_t wchSql[SQLMAX];
		swprintf_s(wchSql, SQLMAX, L"select CertificateAuthentication from DepartInfo where DepartNumber = '%s'", pwchDepartNumber);

		pchSql = UnicodeToANSI(wchSql);

		CppSQLite3Statement stat;
		stat=db.compileStatement(pchSql);

		CppSQLite3Query rs=stat.execQuery();

		if(!rs.eof())
		{
			int len(0);
			const unsigned char * pData=rs.getBlobField(0,len);
			lDataSize = len;

			if ((long)uiCALength >= len)
			{
				memcpy_s(pbCertificateAuthentication, uiCALength, pData, len);
				dwRet = 0x01;
			}
			else
			{
				dwRet = 0x02;
			}
		}
		else
		{
			dwRet = 0x00;
		}
		rs.finalize();
		stat.finalize();

		db.close();
	}
	catch (CppSQLite3Exception& e)
	{
		WriteDatabaseError(e);

		dwRet =  0xff;
	}

	if (NULL != pchPath)
	{
		free(pchPath);
		pchPath = NULL;
	}

	if (NULL != pchSql)
	{
		free(pchSql);
		pchSql = NULL;
	}

	return dwRet;
}

DWORD CNHModeClientDB_SQLite::SetDboCertificateOfDepartInfo(wchar_t const * pwchPath, wchar_t const * pwchDepartNumber, BYTE * const pbCertificateAuthentication, const UINT uiCALength)
{
	DWORD dwRet(0x00);

	char* pchPath(NULL);
	char* pchSql(NULL);
	try
	{
		CppSQLite3DB db;
		CppSQLite3Statement stat;

		pchPath = UnicodeToANSI(pwchPath);
		db.open(pchPath);

		wchar_t wchSql[SQLMAX];
		swprintf_s(wchSql, SQLMAX, L"update DepartInfo set CertificateAuthentication = ? where DepartNumber = '%s'", pwchDepartNumber);

		pchSql = UnicodeToANSI(wchSql);

		stat=db.compileStatement(pchSql);

		stat.bind(1,(const unsigned char *)pbCertificateAuthentication,uiCALength);

		int nRet = stat.execDML();

		if (0 == nRet)
		{
			dwRet = 0x01;
		}
		else
		{
			dwRet = 0x00;
		}

		stat.finalize();//释放分配的stat空间
		db.close();
	}
	catch (CppSQLite3Exception& e)
	{
		WriteDatabaseError(e);

		dwRet = 0xff;
	}

	if (NULL != pchPath)
	{
		free(pchPath);
		pchPath = NULL;
	}

	if (NULL != pchSql)
	{
		free(pchSql);
		pchSql = NULL;
	}

	return dwRet;
}

