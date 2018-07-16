// NHSQLServerDBO.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "CasDBO.h"
#include <assert.h>

#include "NHCommonAPI.h"

#include "NHLogAPI.h"

wchar_t CNHSQLServerDBO::m_wchLogFilePath[MAX_PATH] = L"";

DWORD CNHSQLServerDBO::OpenDB(_ConnectionPtr &pConnection)
{
	DWORD dwReturn(0x00);

	// 读配置文件,获取数据库连接信息
	// 获取exe(dll)文件绝对路径
	wchar_t wchPath[MAX_PATH];
	ZeroMemory(wchPath, sizeof(wchPath));
	if (0x00 != CNHCommonAPI::GetHLDFilePath(L"Config", L"CasInfo.ini", wchPath))
	{
		return 0x01;
	}

	// 获取数据库连接信息
	wchar_t wchSource[MAX_PATH];
	ZeroMemory(wchSource, sizeof(wchSource));
	wchar_t wchCatalog[MAX_PATH];
	ZeroMemory(wchCatalog, sizeof(wchCatalog));
	wchar_t wchUserID[MAX_PATH];
	ZeroMemory(wchUserID, sizeof(wchUserID));
	wchar_t wchPassword[MAX_PATH];
	ZeroMemory(wchPassword, sizeof(wchPassword));
	GetPrivateProfileString(L"Database", L"Source", L"", wchSource, MAX_PATH, wchPath);
	GetPrivateProfileString(L"Database", L"Catalog", L"", wchCatalog, MAX_PATH, wchPath);
	GetPrivateProfileString(L"Database", L"UserID", L"", wchUserID, MAX_PATH, wchPath);
	GetPrivateProfileString(L"Database", L"Password", L"", wchPassword, MAX_PATH, wchPath);

	wchar_t wchSql[MAX_PATH];
	ZeroMemory(wchSql, sizeof(wchSql));

	// 加载COM组件
	CoInitialize(NULL);
	try
	{
		_snwprintf_s(wchSql, _countof(wchSql), _TRUNCATE, L"Provider=SQLOLEDB; Data Source=%s; Initial Catalog=%s; User ID=%s; Password=%s;",
			wchSource,
			wchCatalog,
			wchUserID,
			wchPassword);

		pConnection.CreateInstance(L"ADODB.Connection");
		//pConnection->ConnectionTimeout = 5;
		//pConnection->CommandTimeout = 5;
		pConnection->Open((_bstr_t)wchSql, L"", L"", adModeUnknown);
	}
	catch (_com_error &e)
	{
		//#ifdef _DEBUG
		const int nErrMsgLength(MAX_PATH);
		wchar_t *pwchErrMsg = new wchar_t[nErrMsgLength]();
		_snwprintf_s(pwchErrMsg, nErrMsgLength, _TRUNCATE, L"CNHSQLServerDBO::OpenDB发生错误（执行%s）。", wchSql);
		// 输出错误信息到输出窗口
		OutputDebugStringW(L"\t");
		OutputDebugStringW(pwchErrMsg);
		OutputDebugStringW(L"\n");
		// 输出错误信息到日志文件
		if (0 != wcscmp(m_wchLogFilePath, L""))
		{
			// 当日志文件路径不为空时，写日志
			CNHLogAPI::WriteLogEx(m_wchLogFilePath, LOG_ERR, L"NHSQLServerDBO", pwchErrMsg);
		}
		if (NULL != pwchErrMsg)
		{
			delete[] pwchErrMsg;
			pwchErrMsg = NULL;
		}
		OutputDBErrMsg(e);
		//#endif
		dwReturn = 0x01;
	}

	return dwReturn;
}

DWORD CNHSQLServerDBO::OpenDB(_ConnectionPtr &pConnection,
							  const wchar_t *const pwchDataSource,
							  const wchar_t *const pwchInitialCatalog,
							  const wchar_t *const pwchUserID,
							  const wchar_t *const pwchPassword)
{
	assert(NULL != pwchDataSource);
	assert(NULL != pwchInitialCatalog);
	assert(NULL != pwchUserID);
	assert(NULL != pwchPassword);

	DWORD dwReturn(0x00);

	wchar_t wchSql[MAX_PATH];
	ZeroMemory(wchSql, sizeof(wchSql));

	// 加载COM组件
	CoInitialize(NULL);
	try
	{
		_snwprintf_s(wchSql, _countof(wchSql), _TRUNCATE, L"Provider=SQLOLEDB; Data Source=%s; Initial Catalog=%s; User ID=%s; Password=%s;",
			pwchDataSource,
			pwchInitialCatalog,
			pwchUserID,
			pwchPassword);

		pConnection.CreateInstance(L"ADODB.Connection");
		//pConnection->ConnectionTimeout = 5;
		//pConnection->CommandTimeout = 5;
		pConnection->Open((_bstr_t)wchSql, L"", L"", adModeUnknown);
	}
	catch (_com_error &e)
	{
		//#ifdef _DEBUG
		const int nErrMsgLength(MAX_PATH);
		wchar_t *pwchErrMsg = new wchar_t[nErrMsgLength]();
		_snwprintf_s(pwchErrMsg, nErrMsgLength, _TRUNCATE, L"CNHSQLServerDBO::OpenDB发生错误（执行%s）。", wchSql);
		// 输出错误信息到输出窗口
		OutputDebugStringW(L"\t");
		OutputDebugStringW(pwchErrMsg);
		OutputDebugStringW(L"\n");
		// 输出错误信息到日志文件
		if (0 != wcscmp(m_wchLogFilePath, L""))
		{
			// 当日志文件路径不为空时，写日志
			CNHLogAPI::WriteLogEx(m_wchLogFilePath, LOG_ERR, L"NHSQLServerDBO", pwchErrMsg);
		}
		if (NULL != pwchErrMsg)
		{
			delete[] pwchErrMsg;
			pwchErrMsg = NULL;
		}
		OutputDBErrMsg(e);
		//#endif
		dwReturn = 0x01;
	}

	return dwReturn;
}

DWORD CNHSQLServerDBO::OpenDBOfOrcale(_ConnectionPtr &pConnection,
									  const wchar_t *const pwchDataSource,
									  const wchar_t *const pwchUserID,
									  const wchar_t *const pwchPassword)
{
	assert(NULL != pwchDataSource);
	assert(NULL != pwchUserID);
	assert(NULL != pwchPassword);

	DWORD dwReturn(0x00);

	wchar_t wchSql[MAX_PATH];
	ZeroMemory(wchSql, sizeof(wchSql));

	// 加载COM组件
	CoInitialize(NULL);
	try
	{
		_snwprintf_s(wchSql, _countof(wchSql), _TRUNCATE, L"Provider=OraOLEDB.Oracle; Data Source=%s; User ID=%s; Password=%s;",
			pwchDataSource,
			//pwchInitialCatalog,
			pwchUserID,
			pwchPassword);

		pConnection.CreateInstance(__uuidof(Connection));
		//pConnection->ConnectionTimeout = 5;
		//pConnection->CommandTimeout = 5;
		pConnection->Open((_bstr_t)wchSql, L"", L"", adModeUnknown);

	}
	catch (_com_error &e)
	{
		//#ifdef _DEBUG
		const int nErrMsgLength(MAX_PATH);
		wchar_t *pwchErrMsg = new wchar_t[nErrMsgLength]();
		_snwprintf_s(pwchErrMsg, nErrMsgLength, _TRUNCATE, L"CNHSQLServerDBO::OpenDBOfOrcale发生错误（执行%s）。", wchSql);
		// 输出错误信息到输出窗口
		OutputDebugStringW(L"\t");
		OutputDebugStringW(pwchErrMsg);
		OutputDebugStringW(L"\n");
		// 输出错误信息到日志文件
		if (0 != wcscmp(m_wchLogFilePath, L""))
		{
			// 当日志文件路径不为空时，写日志
			CNHLogAPI::WriteLogEx(m_wchLogFilePath, LOG_ERR, L"NHSQLServerDBO", pwchErrMsg);
		}
		if (NULL != pwchErrMsg)
		{
			delete[] pwchErrMsg;
			pwchErrMsg = NULL;
		}
		OutputDBErrMsg(e);
		//#endif
		dwReturn = 0x01;
	}

	return dwReturn;
}

DWORD CNHSQLServerDBO::CloseDB(_ConnectionPtr &pConnection)
{
	//assert(NULL != pConnection);

	DWORD dwReturn(0x00);

	try
	{
		//关闭连接
		if (NULL!=pConnection && adStateClosed!=pConnection->State)
		{
			pConnection->Close();
		}
		if (NULL != pConnection)
		{
			pConnection.Release();
			pConnection = NULL;
		}
	}
	catch (_com_error &e)
	{
		//#ifdef _DEBUG
		// 输出错误信息到输出窗口
		OutputDebugStringW(L"\t");
		OutputDebugStringW(L"CNHSQLServerDBO::CloseDB发生错误。");
		OutputDebugStringW(L"\n");
		// 输出错误信息到日志文件
		if (0 != wcscmp(m_wchLogFilePath, L""))
		{
			// 当日志文件路径不为空时，写日志
			CNHLogAPI::WriteLogEx(m_wchLogFilePath, LOG_ERR, L"NHSQLServerDBO", L"CNHSQLServerDBO::CloseDB发生错误。");
		}
		OutputDBErrMsg(e);
		//#endif
		dwReturn = 0x01;
	}

	// 卸载COM组件
	CoUninitialize();

	return dwReturn;
}

DWORD CNHSQLServerDBO::OpenQuery(const _ConnectionPtr &pConnection, _RecordsetPtr &pRecordset, const wchar_t *const pwchSQL)
{
	assert(NULL != pConnection);
	assert(NULL != pwchSQL);

	DWORD dwReturn(0x00);

	try
	{
		pRecordset.CreateInstance(L"ADODB.Recordset");
		pRecordset->Open((_bstr_t)pwchSQL,
			_variant_t((IDispatch*)pConnection, true),
			adOpenStatic,
			adLockOptimistic,
			adCmdText);
	}
	catch (_com_error &e)
	{
		//#ifdef _DEBUG
		const int nErrMsgLength(MAX_PATH);
		wchar_t *pwchErrMsg = new wchar_t[nErrMsgLength]();
		_snwprintf_s(pwchErrMsg, nErrMsgLength, _TRUNCATE, L"CNHSQLServerDBO::OpenQuery发生错误（执行%s）。", pwchSQL);
		// 输出错误信息到输出窗口
		OutputDebugStringW(L"\t");
		OutputDebugStringW(pwchErrMsg);
		OutputDebugStringW(L"\n");
		// 输出错误信息到日志文件
		if (0 != wcscmp(m_wchLogFilePath, L""))
		{
			// 当日志文件路径不为空时，写日志
			CNHLogAPI::WriteLogEx(m_wchLogFilePath, LOG_ERR, L"NHSQLServerDBO", pwchErrMsg);
		}
		if (NULL != pwchErrMsg)
		{
			delete[] pwchErrMsg;
			pwchErrMsg = NULL;
		}
		OutputDBErrMsg(e);
		//#endif
		dwReturn = 0x01;
	}

	return dwReturn;
}

DWORD CNHSQLServerDBO::CloseQuery(_RecordsetPtr &pRecordset)
{
	//assert(NULL != pRecordset);

	DWORD dwReturn(0x00);

	try
	{
		if (NULL!=pRecordset && adStateClosed!=pRecordset->State)
		{
			pRecordset->Close();
		}
		if (NULL != pRecordset)
		{
			pRecordset.Release();
			pRecordset = NULL;
		}
	}
	catch (_com_error &e)
	{
		//#ifdef _DEBUG
		// 输出错误信息到输出窗口
		OutputDebugStringW(L"\t");
		OutputDebugStringW(L"CNHSQLServerDBO::CloseQuery发生错误。");
		OutputDebugStringW(L"\n");
		// 输出错误信息到日志文件
		if (0 != wcscmp(m_wchLogFilePath, L""))
		{
			// 当日志文件路径不为空时，写日志
			CNHLogAPI::WriteLogEx(m_wchLogFilePath, LOG_ERR, L"NHSQLServerDBO", L"CNHSQLServerDBO::CloseQuery发生错误。");
		}
		OutputDBErrMsg(e);
		//#endif
		dwReturn = 0x01;
	}

	return dwReturn;
}

DWORD CNHSQLServerDBO::GetFieldValue(const _RecordsetPtr &pRecordset, _variant_t &vFieldValue, const wchar_t *const pwchFieldName)
{
	assert(NULL != pRecordset);
	assert(NULL != pwchFieldName);

	DWORD dwReturn(0x00);

	try
	{
		vFieldValue = pRecordset->GetCollect(_variant_t(pwchFieldName));
	}
	catch (_com_error &e)
	{
		//#ifdef _DEBUG
		const int nErrMsgLength(MAX_PATH);
		wchar_t *pwchErrMsg = new wchar_t[nErrMsgLength]();
		_snwprintf_s(pwchErrMsg, nErrMsgLength, _TRUNCATE, L"CNHSQLServerDBO::GetFieldValue发生错误（FieldName：%s）。", pwchFieldName);
		// 输出错误信息到输出窗口
		OutputDebugStringW(L"\t");
		OutputDebugStringW(pwchErrMsg);
		OutputDebugStringW(L"\n");
		// 输出错误信息到日志文件
		if (0 != wcscmp(m_wchLogFilePath, L""))
		{
			// 当日志文件路径不为空时，写日志
			CNHLogAPI::WriteLogEx(m_wchLogFilePath, LOG_ERR, L"NHSQLServerDBO", pwchErrMsg);
		}
		if (NULL != pwchErrMsg)
		{
			delete[] pwchErrMsg;
			pwchErrMsg = NULL;
		}
		OutputDBErrMsg(e);
		//#endif
		dwReturn = 0x01;
	}

	return dwReturn;
}

DWORD CNHSQLServerDBO::PutFieldValue(const _RecordsetPtr &pRecordset, const _variant_t &vFieldValue, const wchar_t *const pwchFieldName)
{
	assert(NULL != pRecordset);
	assert(NULL != pwchFieldName);

	DWORD dwReturn(0x00);

	try
	{
		pRecordset->PutCollect(_variant_t(pwchFieldName), vFieldValue);
	}
	catch (_com_error &e)
	{
		//#ifdef _DEBUG
		const int nErrMsgLength(MAX_PATH);
		wchar_t *pwchErrMsg = new wchar_t[nErrMsgLength]();
		_snwprintf_s(pwchErrMsg, nErrMsgLength, _TRUNCATE, L"CNHSQLServerDBO::PutFieldValue发生错误（FieldName：%s）。", pwchFieldName);
		// 输出错误信息到输出窗口
		OutputDebugStringW(L"\t");
		OutputDebugStringW(pwchErrMsg);
		OutputDebugStringW(L"\n");
		// 输出错误信息到日志文件
		if (0 != wcscmp(m_wchLogFilePath, L""))
		{
			// 当日志文件路径不为空时，写日志
			CNHLogAPI::WriteLogEx(m_wchLogFilePath, LOG_ERR, L"NHSQLServerDBO", pwchErrMsg);
		}
		if (NULL != pwchErrMsg)
		{
			delete[] pwchErrMsg;
			pwchErrMsg = NULL;
		}
		OutputDBErrMsg(e);
		//#endif
		dwReturn = 0x01;
	}

	return dwReturn;
}

DWORD CNHSQLServerDBO::ExecuteDML(const _ConnectionPtr &pConnection, const wchar_t *const pwchSQL, int *const pnRowsInvolved/*=NULL*/)
{	
	assert(NULL != pConnection);
	assert(NULL != pwchSQL);

	DWORD dwReturn(0x00);

	try
	{
		_variant_t vRA;
		pConnection->Execute((_bstr_t)pwchSQL, &vRA, adCmdText);
		if (NULL != pnRowsInvolved)
		{
			*pnRowsInvolved = static_cast<int>(vRA);
		}
	}
	catch (_com_error &e)
	{
		//#ifdef _DEBUG
		const int nErrMsgLength(MAX_PATH);
		wchar_t *pwchErrMsg = new wchar_t[nErrMsgLength]();
		_snwprintf_s(pwchErrMsg, nErrMsgLength, _TRUNCATE, L"CNHSQLServerDBO::ExecuteDML发生错误（执行%s）。", pwchSQL);
		// 输出错误信息到输出窗口
		OutputDebugStringW(L"\t");
		OutputDebugStringW(pwchErrMsg);
		OutputDebugStringW(L"\n");
		// 输出错误信息到日志文件
		if (0 != wcscmp(m_wchLogFilePath, L""))
		{
			// 当日志文件路径不为空时，写日志
			CNHLogAPI::WriteLogEx(m_wchLogFilePath, LOG_ERR, L"NHSQLServerDBO", pwchErrMsg);
		}
		if (NULL != pwchErrMsg)
		{
			delete[] pwchErrMsg;
			pwchErrMsg = NULL;
		}
		OutputDBErrMsg(e);
		//#endif
		dwReturn = 0x01;
	}

	return dwReturn;
}

void CNHSQLServerDBO::OutputDBErrMsg(const _com_error& e)
{
	const int nErrMsgLength(MAX_PATH);
	wchar_t *pwchErrMsg = new wchar_t[nErrMsgLength]();
	_snwprintf_s(pwchErrMsg, nErrMsgLength, _TRUNCATE, L"错误编号:%08lx  错误信息:%s  错误源:%s  错误描述:%s",
		e.Error(),
		e.ErrorMessage(),
		(LPCWSTR)e.Source(),
		(LPCWSTR)e.Description());

	// 输出错误信息到输出窗口
	OutputDebugStringW(L"\t");
	OutputDebugStringW(pwchErrMsg);
	OutputDebugStringW(L"\n");
	// 输出错误信息到日志文件
	if (0 != wcscmp(m_wchLogFilePath, L""))
	{
		// 当日志文件路径不为空时，写日志
		CNHLogAPI::WriteLogEx(m_wchLogFilePath, LOG_ERR, L"NHSQLServerDBO", pwchErrMsg);
	}

	if (NULL != pwchErrMsg)
	{
		delete[] pwchErrMsg;
		pwchErrMsg = NULL;
	}
}

void CNHSQLServerDBO::SetLogFilePath(const wchar_t *const pFilePath)
{
	assert(NULL != pFilePath);
	wcsncpy_s(m_wchLogFilePath, _countof(m_wchLogFilePath), pFilePath, _TRUNCATE);
}