/*
* Copyright (c) 2009,
* All rights reserved.
*
* 文件名称：NHAppDataINI.h
* 文件标识：
* 摘    要：App_Data数据配置文件基本操作,数据结构格式定义
*
* 当前版本：1.0
* 作    者：
* 完成日期：2010-07-14
*/

#ifndef NHAPPDATAINI_H
#define NHAPPDATAINI_H

// 备注：各类型转换成wchar_t对照表
// int wchar_t[11] 有符号
// short wchar_t[6] 有符号
// time wchar_t[19] 精确到秒 2008-08-08 23:59:59

#include "CasDef.h"
#include <list>
#include <vector>
using std::list;
using std::vector;

// App_Data
// 用户信息 UserInfo.ini

extern "C" void __declspec(dllexport) GetIniUserInfo(PUSERINFO const pIniUserInfo);
extern "C" void __declspec(dllexport) SetIniUserInfo(const USERINFO& iniUserInfo);

//// ASM test result<ResultOfASM>
//extern "C" void __declspec(dllexport) GetIniResultOfASM(PRESULTOFASM const pIniResultOfASM);
//extern "C" void __declspec(dllexport) SetIniResultOfASM(const RESULTOFASM& iniResultOfASM);
//
//// ASM real time data<RealTimeDataOfASM>
//extern "C" void __declspec(dllexport) GetIniRealTimeDataOfASM(list<REALTIMEDATAOFASM>& listRealTimeDataOfASM);
//extern "C" void __declspec(dllexport) GetIniRealTimeDataOfASMVector(vector<REALTIMEDATAOFASM>& vRealTimeDataOfASM);

#endif