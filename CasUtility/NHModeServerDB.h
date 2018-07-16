/*
* Copyright (c) 2010
* All rights reserved.
*
* 文件名称：NHModeServerDB.h
* 文件标识：
* 描述：NHModeServerDB数据库基本操作,返回值逻辑：除注明返回值逻辑的函数外，其它函数遵循如下返回规则：
*		Getxxx：执行成功返回0x01，无对应记录返回0x00，异常返回0xff；Setxxx：成功返回0x00，异常返回0xff
*
* 版本：1.0
* 作者：
* 日期：2010-07-14
*/


#ifndef NHMODESERVERDB_H
#define NHMODESERVERDB_H

// 由于是导出文件,所以要重新定义
#ifndef SQLMAX
#define SQLMAX 1024 // sql 语句最大输入字符数
#endif

#include "CasDef.h"
#include <list>
using std::list;

extern "C"
{
	// 用户信息<UserInfo> UserInfo表的所有操作

	// 获取用户信息
	// 返回：
	// 0x00：无对应记录
	// 0x01：获取成功
	// 0xff：数据库异常
	DWORD __declspec(dllexport) GetDboUserInfo(const wchar_t wchSql[SQLMAX], PUSERINFO const pDboUserInfo);
	// 设置用户信息
	// 返回：
	// 0x00：设置成功
	// 0xff：数据库异常
	DWORD __declspec(dllexport) SetDboUserInfo(const USERINFO &dboUserInfo);
	// 删除用户信息
	// 该操作未实现
	DWORD __declspec(dllexport) DelDboUserInfo(const wchar_t wchSql[SQLMAX]);
	// 获取用户权限
	// 返回：
	// 0x01：获取成功
	// 0x02：获取失败
	// 0xff：数据库异常
	DWORD __declspec(dllexport) GetDboUserPermission(const wchar_t *const pwchName, std::string &strPermission);
	// 写权限数据入数据库
	// 返回：
	// 0x00：写入成功
	// 0xff：数据库异常
	DWORD __declspec(dllexport) SetDboUserPermission(const wchar_t *const pwchName, const std::string &strPermission);
	// 车间认证信息<DepartInfo>
	// return value
	// 0xff : Database exception
	// 0x00 : No record get
	// 0x01 : Get first record successful
	// 0x02 : Get CertificateAuthentication failed
	DWORD __declspec(dllexport) GetDboCertificateOfDepartInfo(wchar_t const * pwchDepartNumber, BYTE * const pbCertificateAuthentication, const UINT uiCALength, long& lDataSize);
	// return value
	// 0xff : Database exception
	// 0x00 : Update successful
	// 0x01 : DepartNumber error
	DWORD __declspec(dllexport) SetDboCertificateOfDepartInfo(wchar_t const * pwchDepartNumber, BYTE * const pbCertificateAuthentication, const UINT uiCALength);
}
#endif