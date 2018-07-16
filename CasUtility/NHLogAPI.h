/*
* Copyright (c)
* All rights reserved.
*
* 文件名称：NHLogAPI.h
* 文件标识：
* 描述：日志记录
*
* 版本：1.0
* 作者：Castle
* 日期：2010-05-24

*/

#pragma once


#define LOG_ERR		L"错误"
#define LOG_EXP		L"异常"
#define LOG_MSG		L"信息"


class CASUTILITY_EXPORT CNHLogAPI
{

private:
	// 禁用拷贝构造函数和拷贝赋值函数
	CNHLogAPI(void);
	virtual ~CNHLogAPI(void);
	CNHLogAPI(const CNHLogAPI &);
	CNHLogAPI & operator =(const CNHLogAPI &);

	// 打开互斥对象句柄
	friend static DWORD OpenMutexHandle(void);
	DWORD static OpenMutexHandle(void);
	// 关闭互斥对象句柄
	friend static DWORD CloseMutexHandle(void);
	DWORD static CloseMutexHandle(void);

public:
	// 写入日志
	// wchType 日志类型，已定义常用宏LOG_ERR/LOG_EXP/LOG_MSG
	// wchContent 日志内容 
	static DWORD WriteLog(const wchar_t wchType[], const wchar_t wchModule[], const wchar_t wchContent[]);
	// 比WriteLog多一个路径参数
	static DWORD WriteLogEx(const wchar_t wchFilePath[], const wchar_t wchType[], const wchar_t wchModule[], const wchar_t wchContent[]);
	// 根据日志文件的最近修改时间，删除某时刻之前（含该时刻）的日志文件
	static DWORD DeleteLog(const SYSTEMTIME &st);
	// 根据日志文件的最近修改时间，删除某时刻之前（含该时刻）的日志文件, 需指定日志文件所在文件夹，如：“D:\LOG”
	static DWORD DeleteLogEx(const wchar_t wchFileFolder[], const SYSTEMTIME &st);

private:
	static HANDLE m_hWriteMutex;
};

