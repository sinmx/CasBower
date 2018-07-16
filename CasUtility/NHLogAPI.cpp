#include "StdAfx.h"
#include "NHCommonAPI.h"
#include "NHLogAPI.h"

#include <io.h>
#include <stdio.h>
#include <time.h>
//#include "afxmt.h"

HANDLE CNHLogAPI::m_hWriteMutex = NULL;
//CSemaphore g_shWriteMutex;
CRITICAL_SECTION g_CS;

CNHLogAPI::CNHLogAPI(void)
{
	InitializeCriticalSection(&g_CS);
}

CNHLogAPI::~CNHLogAPI(void)
{
	DeleteCriticalSection(&g_CS);
}

DWORD CNHLogAPI::OpenMutexHandle()
{
	m_hWriteMutex = CreateMutex( NULL, FALSE, NULL );
	if (NULL == m_hWriteMutex)
	{
		return 0xff;
	}
	else
	{
		return 0x00;
	}
}

DWORD CNHLogAPI::CloseMutexHandle()
{
	if (NULL == m_hWriteMutex)
	{
		return 0x00;
	}
	WaitForSingleObject(m_hWriteMutex, 1000L);
	ReleaseMutex(m_hWriteMutex);	
	if (CloseHandle(m_hWriteMutex))
	{
		m_hWriteMutex = NULL;
		return 0x00;
	}
	else
	{
		return 0xff;
	}
}

DWORD CNHLogAPI::WriteLog(const wchar_t wchType[], const wchar_t wchModule[], const wchar_t wchContent[])
{
	__time64_t now;
	struct tm  when;
	time(&now);
	_localtime64_s( &when, &now );
	wchar_t wchFileName[51] = {0};
	wcsftime(wchFileName, 50, L"%Y-%m-%d.log", &when);

	wchar_t wchFilePath[MAX_PATH] = {0};
	// 等待其它线程释放互斥对象
	//WaitForSingleObject(m_hWriteMutex, INFINITE);
	EnterCriticalSection(&g_CS);
	// 构造文件路径
	// 可能会对文件进行新建操作
	CNHCommonAPI::GetFilePathEx(L"LOG", wchFileName, wchFilePath, true);
	// 释放互斥对象
	//ReleaseMutex(m_hWriteMutex);
	LeaveCriticalSection(&g_CS);
	return WriteLogEx(wchFilePath, wchType, wchModule, wchContent);
}

DWORD CNHLogAPI::WriteLogEx(const wchar_t wchFilePath[], const wchar_t wchType[], const wchar_t wchModule[], const wchar_t wchContent[])
{
	// 等待其它线程释放互斥对象
	//WaitForSingleObject(m_hWriteMutex, INFINITE);
	EnterCriticalSection(&g_CS);

	// 获取文件夹路径
	wchar_t wchFolderPath[MAX_PATH] = {0};
	wcsncpy_s(wchFolderPath, wcslen(wchFolderPath), wchFilePath, _TRUNCATE);
	// 向上1层
	wchar_t *pwchDest = wcsrchr(wchFolderPath, L'\\');
	*pwchDest = L'\0';

	// 判断文件是否存在
	if (_waccess(wchFilePath, 00))
	{
		// 文件不存在

		// 判断文件夹是否存在
		if (_waccess(wchFolderPath, 00))
		{
			// 文件夹不存在
			// 创建文件夹
			if (!CreateDirectory(wchFolderPath, NULL))
			{ 
				// 创建文件夹失败
				//ReleaseMutex(m_hWriteMutex); // 释放互斥对象
				LeaveCriticalSection(&g_CS);
				return 0x01;
			}
		}

		// 创建Unicode文件
		if (0x00 != CNHCommonAPI::CreateUnicodeFile(wchFilePath))
		{
			// 创建文件失败
			//ReleaseMutex(m_hWriteMutex); // 释放互斥对象
			LeaveCriticalSection(&g_CS);
			return 0x02;
		}
	}

	// 去除文件只读属性
	DWORD dwAttributes(0);
	dwAttributes = ::GetFileAttributes(wchFilePath);
	dwAttributes &= ~FILE_ATTRIBUTE_READONLY;
	::SetFileAttributes(wchFilePath, dwAttributes);

	__time64_t now;
	struct tm when;
	time(&now);
	_localtime64_s( &when, &now );
	wchar_t wchTime[51] = {0};
	wcsftime(wchTime, 50, L"%H:%M:%S", &when);

	FILE *fp = NULL;
	_wfopen_s(&fp, wchFilePath, L"a+,ccs=UNICODE");
	if (NULL == fp)
	{
		// 释放互斥对象
		//ReleaseMutex(m_hWriteMutex);
		LeaveCriticalSection(&g_CS);
		// 写入失败
		return 0x03;
	}
	fwprintf(fp, L"[%s]\t%s\t%s\t%s\n", wchTime, wchType, wchModule, wchContent);
	fclose(fp);

	// 释放互斥对象
	//ReleaseMutex(m_hWriteMutex);
	LeaveCriticalSection(&g_CS);
	return 0x00;
}

DWORD CNHLogAPI::DeleteLog(const SYSTEMTIME &st)
{
	// 构造文件路径
	wchar_t wchFilePath[MAX_PATH] = {0};
	// 不会对文件操作，不用互斥保护
	CNHCommonAPI::GetFilePathEx(L"LOG", L"", wchFilePath);
	return DeleteLogEx(wchFilePath, st);
}

DWORD CNHLogAPI::DeleteLogEx(const wchar_t wchFileFolder[], const SYSTEMTIME &st)
{
	wchar_t wchFilePath[MAX_PATH] = {0};
	// 文件路径
	wcsncpy_s(wchFilePath, wcslen(wchFilePath), wchFileFolder, _TRUNCATE);
	wcscat_s(wchFilePath, wcslen(wchFilePath), L"\\*.log");
	
	// 找所有日志文件
	WIN32_FIND_DATAW fd;
	HANDLE hFindFile = FindFirstFile(wchFilePath, &fd);
	if (INVALID_HANDLE_VALUE == hFindFile)
	{
		return 0xff;
	}

	FILETIME ft;
	ZeroMemory(&ft, sizeof(FILETIME));
	SystemTimeToFileTime(&st, &ft);

	// 等待其它线程释放互斥对象
	//WaitForSingleObject(m_hWriteMutex, INFINITE);
	EnterCriticalSection(&g_CS);
	
	// 对比第1个文件名，早于或者等于设定时刻的删除
	FILETIME ftLocalLastWriteTime;
	ZeroMemory(&ftLocalLastWriteTime, sizeof(FILETIME));
	// 将最近修改时间转换成北京时间（东八区）
	// 用最近修改时间，不使用创建时间（创建时间在复制后会改变）。
	FileTimeToLocalFileTime(&(fd.ftLastWriteTime), &ftLocalLastWriteTime);
	if (1 == CompareFileTime(&ft, &ftLocalLastWriteTime)) // ft 迟于 ftLocalLastWriteTime
	{
		ZeroMemory(wchFilePath, sizeof(wchFilePath));
		wcsncpy_s(wchFilePath, wcslen(wchFilePath), wchFileFolder, _TRUNCATE);
		wcscat_s(wchFilePath, wcslen(wchFilePath), fd.cFileName);
		DeleteFile(wchFilePath);
	}

	// 找到，指向下一个文件
	while (FindNextFile(hFindFile, &fd))
	{
		// 对比文件名，早于或者等于设定时刻的删除
		ZeroMemory(&ftLocalLastWriteTime, sizeof(FILETIME));
		// 将最近修改时间转换成北京时间（东八区）
		// 用最近修改时间，不使用创建时间（创建时间在复制后会改变）。
		FileTimeToLocalFileTime(&(fd.ftLastWriteTime), &ftLocalLastWriteTime);
		if (1 == CompareFileTime(&ft, &ftLocalLastWriteTime)) // ft 迟于 ftLocalLastWriteTime
		{
			ZeroMemory(wchFilePath, sizeof(wchFilePath));
			wcsncpy_s(wchFilePath, wcslen(wchFilePath), wchFileFolder, _TRUNCATE);
			wcscat_s(wchFilePath, wcslen(wchFilePath), fd.cFileName);
			DeleteFile(wchFilePath);
		}
	}
	
	// 释放互斥对象
	//ReleaseMutex(m_hWriteMutex);
	LeaveCriticalSection(&g_CS);

	// 关闭句柄
	FindClose(hFindFile);
	return 0x00;
}