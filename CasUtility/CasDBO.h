/*
* Copyright (c) 2014
* All rights reserved.
*
* 文件名称：NHSQLServerDBO.h
* 文件标识：
* 描述：SQLServer数据库操作
*
* 版本：1.0
* 作者：Castle
* 日期：2011-03-16
*/

#ifndef NH_SQLSERVER_DBO_H
#define NH_SQLSERVER_DBO_H

#import "msado60_Backcompat_i386.tlb" no_namespace rename("EOF","adoEOF")


class CASUTILITY_EXPORT CNHSQLServerDBO
{
	// 禁用构造函数和析构函数(禁止生成对象)
private:
	CNHSQLServerDBO(void);
	virtual ~CNHSQLServerDBO(void);

	// 禁用拷贝构造函数和拷贝赋值函数
private:
	CNHSQLServerDBO(const CNHSQLServerDBO &);
	CNHSQLServerDBO & operator =(const CNHSQLServerDBO &);

public:
	// 打开数据库(默认连接信息)
	// (in&out)pConnection:连接指针引用
	// (return)错误代码:
	// 0x00:正常
	// 非0x00:异常
	static DWORD OpenDB(_ConnectionPtr &pConnection);
	// 打开数据库(指定连接信息)
	// (in&out)pConnection:连接指针引用
	// (in)pwchSource:数据源
	// (in)pwchCatalog:数据库名称
	// (in)pwchUserID:用户名
	// (in)pwchPassword:密码
	// (return)错误代码:
	// 0x00:正常
	// 非0x00:异常
	static DWORD OpenDB(_ConnectionPtr &pConnectionconst,
		const wchar_t *const pwchDataSource,
		const wchar_t *const pwchInitialCatalog,
		const wchar_t *const pwchUserID,
		const wchar_t *const pwchPassword);
	// 打开Oracle数据库(指定连接信息)
	// (in&out)pConnection:连接指针引用
	// (in)pwchSource:数据源
	// (in)pwchUserID:用户名
	// (in)pwchPassword:密码
	// (return)错误代码:
	// 0x00:正常
	// 非0x00:异常
	static DWORD OpenDBOfOrcale(_ConnectionPtr &pConnectionconst,
		const wchar_t *const pwchDataSource,
		const wchar_t *const pwchUserID,
		const wchar_t *const pwchPassword);
	// 关闭数据库
	// (in&out)pConnection:连接指针引用
	// (return)错误代码:
	// 0x00:正常
	// 非0x00:异常
	static DWORD CloseDB(_ConnectionPtr &pConnection);
	// 打开查询
	// (in)pConnection:连接指针引用
	// (in&out)pRecordset:记录集指针应用
	// (in)pwchSQL:查询语句字符串指针
	// (return)错误代码:
	// 0x00:正常
	// 非0x00:异常
	static DWORD OpenQuery(const _ConnectionPtr &pConnection, _RecordsetPtr &pRecordset, const wchar_t *const pwchSQL);
	// 关闭查询
	// (in&out)pRecordset:记录集指针应用
	// (return)错误代码:
	// 0x00:正常
	// 非0x00:异常
	static DWORD CloseQuery(_RecordsetPtr &pRecordset);
	// 获取字段值
	// (in&out)pRecordset:记录集指针应用
	// (out)vFieldValue:字段值引用
	// (in)pwchFieldName:字段名字符串指针
	// (return)错误代码:
	// 0x00:正常
	// 非0x00:异常
	static DWORD GetFieldValue(const _RecordsetPtr &pRecordset, _variant_t &vFieldValue, const wchar_t *const pwchFieldName);
	// 设置字段值
	// (in&out)pRecordset:记录集指针应用
	// (out)vFieldValue:字段值引用
	// (in)pwchFieldName:字段名字符串指针
	// (return)错误代码:
	// 0x00:正常
	// 非0x00:异常
	static DWORD PutFieldValue(const _RecordsetPtr &pRecordset, const _variant_t &vFieldValue, const wchar_t *const pwchFieldName);
	// 执行DML
	// (in&out)pConnection:连接指针引用
	// (in)pwchSQL:DML语句字符串指针
	// (out)pnRowsInvolved：受影响的行的数目指针，可以为NULL
	// (return)错误代码:
	// 0x00:正常
	// 非0x00:异常
	static DWORD ExecuteDML(const _ConnectionPtr &pConnection, const wchar_t *const pwchSQL, int *const pnRowsInvolved=NULL);
	// 输出数据库错误信息（输出到输出窗口和日志文件）
	// (in&out)e:异常对象
	// (return)无
	static void OutputDBErrMsg(const _com_error& e);
	// 设置日志文件路径
	// (in&out)e:异常对象
	// (return)无
	static void SetLogFilePath(const wchar_t *const pFilePath);

private:
	// 日志文件路径
	static wchar_t m_wchLogFilePath[MAX_PATH];
};

#endif