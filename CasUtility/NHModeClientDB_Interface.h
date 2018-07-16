/*
* Copyright (c) 2010
* All rights reserved.
*
* 文件名称：NHModeClientDB_Interface.h
* 文件标识：
* 描述：xxxxxxx
*
* 版本：1.0.0alpha
* 日期：2010-xx-xx
* 作者：Chc
* 描述：正式开发
*/

#pragma once

// 由于是导出文件,所以要重新定义
#ifndef SQLMAX
#define SQLMAX 1024 // sql 语句最大输入字符数
#endif

#include "CasDef.h"
#include <list>

//class CNHModeClientDB_Interface
class CASUTILITY_EXPORT CNHModeClientDB_Interface
{
public:
	CNHModeClientDB_Interface(void);
	virtual ~CNHModeClientDB_Interface(void);

	// 禁用拷贝构造函数和拷贝赋值函数
private:
	CNHModeClientDB_Interface(const CNHModeClientDB_Interface &);
	CNHModeClientDB_Interface & operator =(const CNHModeClientDB_Interface &);

public:
	// return value
	// 0xff : Database exception
	// 0x00 : No record get
	// 0x01 : Get first record successful
	// 0x02 : Get CertificateAuthentication failed
	DWORD GetDboCertificateOfDepartInfo(wchar_t const * pwchDepartNumber, BYTE * const pbCertificateAuthentication, const UINT uiCALength, long& lDataSize);
	// return value
	// 0xff : Database exception
	// 0x00 : Update successful
	// 0x01 : DepartNumber error
	DWORD SetDboCertificateOfDepartInfo(wchar_t const * pwchDepartNumber, BYTE * const pbCertificateAuthentication, const UINT uiCALength);
};
