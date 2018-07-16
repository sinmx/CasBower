/*
* Copyright (c) 2010
* All rights reserved.
*
* 文件名称：NHModeClientDB_SQLite.h
* 文件标识：
* 描述：1）CastleDB数据库基本操作,返回值逻辑：除注明返回值逻辑的函数外，其它函数遵循如下返回规则：
*		Getxxx：执行成功返回0x01，无对应记录返回0x00，异常返回0xff；Setxxx：成功返回0x00，异常返回0xff
*
*
* 版本：1.0.0
* 日期：2010-12-31
* 作者：Chc
* 描述：正式发布使用
*/

#pragma once
#include "NHModeClientDB_Interface.h"

class CASUTILITY_EXPORT CNHModeClientDB_SQLite : public CNHModeClientDB_Interface
{
public:
	CNHModeClientDB_SQLite(void);
	virtual ~CNHModeClientDB_SQLite(void);

	// 禁用拷贝构造函数和拷贝赋值函数
private:
	CNHModeClientDB_SQLite(const CNHModeClientDB_SQLite &);
	CNHModeClientDB_SQLite & operator =(const CNHModeClientDB_SQLite &);


public:
	// return value
	// 0xff : Database exception
	// 0x00 : No record get
	// 0x01 : Get first record successful
	// 0x02 : Get CertificateAuthentication failed
	static DWORD GetDboCertificateOfDepartInfo(wchar_t const * pwchPath, wchar_t const * pwchDepartNumber, BYTE * const pbCertificateAuthentication, const UINT uiCALength, long& lDataSize);
	// return value
	// 0xff : Database exception
	// 0x00 : Update successful
	// 0x01 : DepartNumber error
	static DWORD SetDboCertificateOfDepartInfo(wchar_t const * pwchPath, wchar_t const * pwchDepartNumber, BYTE * const pbCertificateAuthentication, const UINT uiCALength);
};
