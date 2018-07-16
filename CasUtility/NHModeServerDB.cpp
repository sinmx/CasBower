// NHModeServerDB.cpp : 定义 DLL 应用程序的入口点。
//
#include "stdafx.h"
#include "NHModeServerDB.h"
#include "assert.h"
#include <fstream>
#include <time.h>
#include <tchar.h>

// 数据库操作
#import "msado60_Backcompat_i386.tlb" no_namespace rename("EOF","adoEOF")

#include "NHLogAPI.h"
#include "NHCommonAPI.h"

using std::wstring;

// 写日志文件
void WriteLogString(const wchar_t wchErrorLog[ERROR_MAX])
{
	assert(NULL != wchErrorLog);

	// 获取exe(dll)文件绝对路径
	wchar_t wchPathDatabaseLog[MAX_PATH];
	if (0x00 != CNHCommonAPI::GetFilePathEx(L"Log", L"Database.log", wchPathDatabaseLog, true))
	{
		return;
	}

	// 写入日志文件
	SYSTEMTIME sysTm;
	::GetLocalTime(&sysTm);

	// 添加时间日期
	wchar_t wchWriteTime[100];
	wsprintf(wchWriteTime, 
		L"%d年%d月%d日%d时%d分%d秒",
		sysTm.wYear,
		sysTm.wMonth,
		sysTm.wDay,
		sysTm.wHour,
		sysTm.wMinute,
		sysTm.wSecond);

	try
	{
		FILE* filePtr(NULL);
		_tfopen_s(&filePtr, wchPathDatabaseLog, L"a+b");  // 追加形式打开文件
		if (NULL == filePtr) 
		{
			return;
		}

		_ftprintf(filePtr, L"\r\n[%s] %s\r\n", wchWriteTime, wchErrorLog);  // 打开文件写入

		fflush(filePtr);
		fclose(filePtr);
	}
	catch(...)
	{
	}
}

// 写数据库出错信息处理
void WriteDatabaseError(const _com_error& e)
{
	wchar_t wchComError[ERROR_MAX];
	wsprintf(wchComError, L"错误编号:%08lx  错误信息:%s  错误源:%s  错误描述:%s",
		e.Error(),
		e.ErrorMessage(),
		(LPCTSTR) e.Source(),
		(LPCTSTR) e.Description());
	//WriteLogString(wchComError);
	CNHLogAPI::WriteLog(LOG_EXP, L"NHModeServerDB", wchComError);
}

char * UnicodeToANSIEx(const wchar_t * wchSource)
{
	// 只支持小于1024长度的字符转换
	char chResult[1024];
	char *pch(NULL);
	pch = CNHCommonAPI::UnicodeToANSI(wchSource);
	strcpy_s(chResult, pch);
	if (NULL != pch)
	{
		free(pch);
		pch = NULL;
	}

	return chResult;
}

// 连接数据库
DWORD ConnectDatabase(_ConnectionPtr& pConnection)
{
	// 返回值
	DWORD dwReturn(0x00);

	// 读配置文件,获取数据库连接信息
	// 获取exe(dll)文件绝对路径
	wchar_t wchPathNHModeServerDB[MAX_PATH];
	if( 0x00 != CNHCommonAPI::GetFilePathEx(L"Config", L"CasInfo.ini", wchPathNHModeServerDB))
	{
		return 0x01;
	}

	// 获取数据库连接信息
	wchar_t wchSource[MAX_PATH];
	wchar_t wchCatalog[MAX_PATH];
	wchar_t wchUserID[MAX_PATH];
	wchar_t wchPassword[MAX_PATH];
	GetPrivateProfileString(L"Database", L"Source", L"", wchSource, MAX_PATH, wchPathNHModeServerDB);
	GetPrivateProfileString(L"Database", L"Catalog", L"", wchCatalog, MAX_PATH, wchPathNHModeServerDB);
	GetPrivateProfileString(L"Database", L"UserID", L"", wchUserID, MAX_PATH, wchPathNHModeServerDB);
	GetPrivateProfileString(L"Database", L"Password", L"", wchPassword, MAX_PATH, wchPathNHModeServerDB);

	// 加载COM组件
	CoInitialize(NULL);
	try
	{
		wchar_t wchSql[SQLMAX];
		wsprintf(wchSql, L"Provider=SQLOLEDB; Data Source=%s; Initial Catalog=%s; User ID=%s; Password=%s;",
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
		WriteDatabaseError(e);
		dwReturn = 0x01;
	}

	return dwReturn;
}

// 关闭数据库连接
DWORD DisconnectDatabase(_ConnectionPtr& pConnection)
{
	assert(NULL != pConnection);

	// 返回值
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
		WriteDatabaseError(e);
		dwReturn = 0x01;
	}

	// 卸载COM组件
	CoUninitialize();

	return dwReturn;
}

DWORD GetFieldData(const _RecordsetPtr &rs, const wchar_t wchFieldName[MAX_PATH], _variant_t &vFieldData)
{
	assert(NULL != rs);
	assert(NULL != wchFieldName);

	// 返回值
	DWORD dwReturn(0x00);

	try
	{
		vFieldData = rs->GetCollect(_variant_t(wchFieldName));
		dwReturn = 0x00;
	}
	catch (_com_error &e)
	{
		WriteDatabaseError(e);
		dwReturn = 0x01;
	}

	return dwReturn;
}

DWORD PutFieldData(const _RecordsetPtr &rs, const wchar_t wchFieldName[MAX_PATH], _variant_t &vFieldData)
{
	assert(NULL != rs);
	assert(NULL != wchFieldName);

	// 返回值
	DWORD dwReturn(0x00);

	try
	{
		rs->PutCollect(_variant_t(wchFieldName), vFieldData);
		dwReturn = 0x00;
	}
	catch (_com_error &e)
	{
		WriteDatabaseError(e);
		dwReturn = 0x01;
	}

	return dwReturn;
}

DWORD SetStringToDBImage(_RecordsetPtr pRecordset, const std::string &str, const wchar_t* pwchFieldName)
{
	assert(pRecordset != NULL);
	assert(pwchFieldName != NULL);

	DWORD dwRet(0x00);
	if (str.empty())
	{
		_variant_t v;
		v.ChangeType(VT_NULL);
		dwRet = PutFieldData(pRecordset, pwchFieldName, v);
	}
	else
	{
		int nSize = str.size();
		SAFEARRAYBOUND Bound[1];
		Bound[0].lLbound = 0;
		Bound[0].cElements = nSize;
		SAFEARRAY *psa = SafeArrayCreate(VT_UI1, 1, Bound); // char 数组
		assert(psa != NULL);
		const char *pchStrBuf = str.c_str();
		for (long l=0; l<nSize; ++l)
		{
			SafeArrayPutElement(psa, &l, (void *)(pchStrBuf+l));
		}
		VARIANT var;
		var.vt = VT_ARRAY|VT_UI1;
		var.parray = psa;
		try
		{
			pRecordset->GetFields()->GetItem(pwchFieldName)->AppendChunk(var);
			dwRet = 0x01;
		}
		catch (_com_error &e)
		{
			WriteDatabaseError(e);
			dwRet = 0xFFFFFFFF;
		}
		VariantClear(&var);
		// 会异常退出程序
		//SafeArrayDestroy(psa);
	}
	return dwRet;
}

DWORD GetStringFromDBImage(_RecordsetPtr pRecordset, std::string &str, const wchar_t* pwchFieldName)
{
	assert(pRecordset != NULL);
	assert(pwchFieldName != NULL);
	DWORD dwRet(0x00);
	try
	{
		const long lSize = pRecordset->GetFields()->GetItem(pwchFieldName)->ActualSize;
		if (0 == lSize)
		{
			str = "";
		}
		else
		{
			_variant_t var;
			var = pRecordset->GetFields()->GetItem(pwchFieldName)->GetChunk(lSize);
			if (var.vt == (VT_ARRAY|VT_UI1))
			{
				char *pBuf(NULL);
				SafeArrayAccessData(var.parray, (void **)&pBuf);
				assert(pBuf != NULL);
				str.assign(pBuf, lSize);
				SafeArrayUnaccessData(var.parray);
			}
			dwRet = 0x01;
		}
	}
	catch (_com_error &e)
	{
		WriteDatabaseError(e);
		dwRet = 0xFFFFFFFF;
	}
	return dwRet;
}

bool SplitString(std::string &str, wchar_t *pwch, const char * pch)
{
	std::string::size_type pos;
	int size = str.size();

	if (size <= 1)
	{
		return false;
	}

	pos = str.find(pch, 0);
	if(pos < size)
	{
		std::string s = str.substr(0, pos);
		wchar_t * pwchTmp(NULL);
		pwchTmp = CNHCommonAPI::ANSIToUnicode(s.c_str());
		wcscpy_s(pwch, 50, pwchTmp);
		if (NULL != pwchTmp)
		{
			free(pwchTmp);
			pwchTmp = NULL;
		}
		//wcscpy_s(pwch, CNHCommonAPI::ANSIToUnicode(s.c_str()));
		str = str.substr(pos+1, str.length());
	}

	return true;

}


DWORD GetDboUserInfo(const wchar_t wchSql[SQLMAX], PUSERINFO const pDboUserInfo)
{
	assert(NULL != wchSql);
	assert(NULL != pDboUserInfo);

	DWORD dwReturn(0x00);

	_ConnectionPtr pConnection(NULL);

	if (0x00 == ConnectDatabase(pConnection))
	{
		_RecordsetPtr pRecordset;

		try
		{
			pRecordset.CreateInstance(L"ADODB.Recordset");
			pRecordset->Open((_bstr_t)wchSql,
				_variant_t((IDispatch*)pConnection, true),
				adOpenStatic,
				adLockOptimistic,
				adCmdText);

			if(!pRecordset->adoEOF)
			{
				_variant_t v;
				wstring s;

				v.ChangeType(VT_NULL);
				GetFieldData(pRecordset, L"AutoID", v);
				if(VT_NULL != v.vt)
				{
					swprintf_s(pDboUserInfo->wchAutoID, NH_INT_TO_WCHART, L"%d", (int)v);  
				}
				else
				{
					_tcscpy_s(pDboUserInfo->wchAutoID,L"");
				}


				v.ChangeType(VT_NULL);
				GetFieldData(pRecordset, L"Name", v);
				if(VT_NULL != v.vt)
				{
					s = (wchar_t*)(_bstr_t)v;
					wcscpy_s(pDboUserInfo->wchName, s.c_str());
				}
				else
				{
					_tcscpy_s(pDboUserInfo->wchName,L"");
				}

				v.ChangeType(VT_NULL);
				GetFieldData(pRecordset, L"Password", v);
				if(VT_NULL != v.vt)
				{
					s = (wchar_t*)(_bstr_t)v;
					wcscpy_s(pDboUserInfo->wchPassword, s.c_str());
				}
				else
				{
					_tcscpy_s(pDboUserInfo->wchPassword,L"");
				}

				v.ChangeType(VT_NULL);
				GetFieldData(pRecordset, L"LicenseSn", v);
				if(VT_NULL != v.vt)
				{
					s = (wchar_t*)(_bstr_t)v;
					wcscpy_s(pDboUserInfo->wchLicenseSn, s.c_str());
				}
				else
				{
					_tcscpy_s(pDboUserInfo->wchLicenseSn,L"");
				}

				v.ChangeType(VT_NULL);
				GetFieldData(pRecordset, L"LicenseStartDate", v);
				if(VT_NULL != v.vt)
				{
					/*s = (wchar_t*)(_bstr_t)v;
					wcscpy_s(pDboUserInfo->wchLicenseStartDate, s.c_str());*/

					wchar_t wch[50] = {0};
					SYSTEMTIME dt;
					VariantTimeToSystemTime(v, &dt);
					swprintf(wch, 30, L"%04d-%02d-%02d",dt.wYear, dt.wMonth, dt.wDay);
					wcscpy_s(pDboUserInfo->wchLicenseStartDate, wch);
				}
				else
				{
					_tcscpy_s(pDboUserInfo->wchLicenseStartDate,L"");
				}

				v.ChangeType(VT_NULL);
				GetFieldData(pRecordset, L"LicenseEndDate", v);
				if(VT_NULL != v.vt)
				{
					/*s = (wchar_t*)(_bstr_t)v;
					wcscpy_s(pDboUserInfo->wchLicenseEndDate, s.c_str());*/

					wchar_t wch[50] = {0};
					SYSTEMTIME dt;
					VariantTimeToSystemTime(v, &dt);
					swprintf(wch, 30, L"%04d-%02d-%02d",dt.wYear, dt.wMonth, dt.wDay);
					wcscpy_s(pDboUserInfo->wchLicenseEndDate, wch);
				}
				else
				{
					_tcscpy_s(pDboUserInfo->wchLicenseEndDate,L"");
				}

				v.ChangeType(VT_NULL);
				GetFieldData(pRecordset, L"Role", v);
				if(VT_NULL != v.vt)
				{
					swprintf_s(pDboUserInfo->wchRole, NH_INT_TO_WCHART, L"%d", (short)v);  
				}
				else
				{
					_tcscpy_s(pDboUserInfo->wchRole,L"");
				}

				v.ChangeType(VT_NULL);
				GetFieldData(pRecordset, L"Remark", v);
				if(VT_NULL != v.vt)
				{
					s = (wchar_t*)(_bstr_t)v;
					wcscpy_s(pDboUserInfo->wchRemark, s.c_str());
				}
				else
				{
					_tcscpy_s(pDboUserInfo->wchRemark,L"");
				}

				v.ChangeType(VT_NULL);
				GetFieldData(pRecordset, L"Reserved1", v);
				if(VT_NULL != v.vt)
				{
					s = (wchar_t*)(_bstr_t)v;
					wcscpy_s(pDboUserInfo->wchReserved1, s.c_str());
				}
				else
				{
					_tcscpy_s(pDboUserInfo->wchReserved1,L"");
				}

				v.ChangeType(VT_NULL);
				GetFieldData(pRecordset, L"Reserved2", v);
				if(VT_NULL != v.vt)
				{
					s = (wchar_t*)(_bstr_t)v;
					wcscpy_s(pDboUserInfo->wchReserved2, s.c_str());
				}
				else
				{
					_tcscpy_s(pDboUserInfo->wchReserved2,L"");
				}

				v.ChangeType(VT_NULL);
				GetFieldData(pRecordset, L"Reserved3", v);
				if(VT_NULL != v.vt)
				{
					s = (wchar_t*)(_bstr_t)v;
					wcscpy_s(pDboUserInfo->wchReserved3, s.c_str());
				}
				else
				{
					_tcscpy_s(pDboUserInfo->wchReserved3,L"");
				}

				dwReturn = 0x01;
			}
			else
			{
				dwReturn = 0x00;
			}
		}
		catch (_com_error &e)
		{
			WriteDatabaseError(e);
			dwReturn = 0xff;
		}

		try
		{
			//关闭记录集
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
			WriteDatabaseError(e);
			dwReturn = 0xff;
		}
	}
	else
	{
		dwReturn = 0xff;
	}

	if (0x00 != DisconnectDatabase(pConnection))
	{
		dwReturn = 0xff;
	}

	return dwReturn;
}

DWORD SetDboUserInfo(const USERINFO &dboUserInfo)
{
	return 0xff;
}

DWORD DelDboUserInfo(const wchar_t wchSql[SQLMAX])
{
	return 0xff;
}

DWORD GetDboUserPermission(const wchar_t *const pwchName, std::string &strPermission)
{
	assert(NULL != pwchName);

	DWORD dwReturn(0x00);

	_ConnectionPtr pConnection(NULL);

	if (0x00 == ConnectDatabase(pConnection))
	{
		wchar_t wchSql[SQLMAX];
		ZeroMemory(wchSql, sizeof(wchSql));
		_snwprintf_s(wchSql, _countof(wchSql), _TRUNCATE, L"select * from UserInfo where Name = '%s'", pwchName);

		_RecordsetPtr pRecordset;

		try
		{
			pRecordset.CreateInstance(L"ADODB.Recordset");
			pRecordset->Open((_bstr_t)wchSql,
				_variant_t((IDispatch*)pConnection, true),
				adOpenStatic,
				adLockOptimistic,
				adCmdText);

			if(!pRecordset->adoEOF)
			{
				wchar_t wchFieldName[20];
				ZeroMemory(wchFieldName, sizeof(wchFieldName));
				wcscpy_s(wchFieldName, L"Permission");
				const long lActualSize = pRecordset->GetFields()->GetItem(wchFieldName)->ActualSize;
				if (0 == lActualSize)
				{
					strPermission = "";
					dwReturn = 0x02;
				}
				else
				{
					_variant_t varBLOB;
					varBLOB = pRecordset->GetFields()->GetItem(wchFieldName)->GetChunk(lActualSize);
					if (varBLOB.vt == (VT_ARRAY|VT_UI1))
					{
						char *pBuf(NULL);
						SafeArrayAccessData(varBLOB.parray, (void **)&pBuf);
						assert(pBuf != NULL);
						strPermission.assign(pBuf, lActualSize);
						SafeArrayUnaccessData(varBLOB.parray);
					}
					dwReturn = 0x01;
				}
			}
			else
			{
				dwReturn = 0x02;
			}
		}
		catch (_com_error &e)
		{
			WriteDatabaseError(e);
			dwReturn = 0xff;
		}

		try
		{
			//关闭记录集
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
			WriteDatabaseError(e);
			dwReturn = 0xff;
		}
	}
	else
	{
		dwReturn = 0xff;
	}

	if (0x00 != DisconnectDatabase(pConnection))
	{
		dwReturn = 0xff;
	}

	return dwReturn;
}

DWORD SetDboUserPermission(const wchar_t *const pwchName, const std::string &strPermission)
{
	assert(NULL != pwchName);

	DWORD dwReturn(0x00);

	_ConnectionPtr pConnection(NULL);

	if (0x00 == ConnectDatabase(pConnection))
	{
		wchar_t wchSql[SQLMAX];
		ZeroMemory(wchSql, sizeof(wchSql));
		_snwprintf_s(wchSql, _countof(wchSql), _TRUNCATE, L"select * from UserInfo where Name = '%s'", pwchName);

		_RecordsetPtr pRecordset;

		try
		{
			pRecordset.CreateInstance(L"ADODB.Recordset");
			pRecordset->Open((_bstr_t)wchSql,
				_variant_t((IDispatch*)pConnection, true),
				adOpenStatic,
				adLockOptimistic,
				adCmdText);

			wchar_t wchFieldName[20];
			ZeroMemory(wchFieldName, sizeof(wchFieldName));
			wcscpy_s(wchFieldName, L"Permission");

			if (pRecordset->adoEOF) 
			{
				// 添加新记录
				//pRecordset->AddNew();
			}
			else
			{
				if (strPermission.empty())
				{
					_variant_t v;
					v.ChangeType(VT_NULL);
					PutFieldData(pRecordset, wchFieldName, v);
				}
				else
				{
					int nSize = strPermission.size();
					SAFEARRAYBOUND Bound[1];
					Bound[0].lLbound = 0;
					Bound[0].cElements = nSize;
					SAFEARRAY *psa = SafeArrayCreate(VT_UI1, 1, Bound); // char 数组
					assert(psa != NULL);
					const char *pchStrBuf = strPermission.c_str();
					for (long l=0; l<nSize; ++l)
					{
						SafeArrayPutElement(psa, &l, (void *)(pchStrBuf+l));
					}
					VARIANT var;
					var.vt = VT_ARRAY|VT_UI1;
					var.parray = psa;
					pRecordset->GetFields()->GetItem(wchFieldName)->AppendChunk(var);
					VariantClear(&var);
					// 会异常退出程序
					//SafeArrayDestroy(psa);
				}
				// 保存到库中
				pRecordset->Update();
			}
		}
		catch (_com_error &e)
		{
			WriteDatabaseError(e);
			dwReturn = 0xff;
		}

		try
		{
			//关闭记录集
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
			WriteDatabaseError(e);
			dwReturn = 0xff;
		}
	}
	else
	{
		dwReturn = 0xff;
	}

	if (0x00 != DisconnectDatabase(pConnection))
	{
		dwReturn = 0xff;
	}

	return dwReturn;
}

DWORD GetDboCertificateOfDepartInfo(wchar_t const * pwchDepartNumber, BYTE * const pbCertificateAuthentication, const UINT uiCALength, long& lDataSize)
{
	assert(NULL != pwchDepartNumber);
	assert(NULL != pbCertificateAuthentication);
	assert(uiCALength > 0);

	DWORD dwReturn(0x00);

	_ConnectionPtr pConnection(NULL);

	if (0x00 == ConnectDatabase(pConnection))
	{
		_RecordsetPtr pRecordset;

		wchar_t wchSql[SQLMAX];
		swprintf_s(wchSql, SQLMAX, L"select CertificateAuthentication from DepartInfo where DepartNumber = '%s'", pwchDepartNumber);

		try
		{
			pRecordset.CreateInstance(L"ADODB.Recordset");
			pRecordset->Open((_bstr_t)wchSql,
				_variant_t((IDispatch*)pConnection, true),
				adOpenStatic,
				adLockOptimistic,
				adCmdText);

			if(!pRecordset->adoEOF)
			{
				lDataSize = pRecordset->GetFields()->GetItem(L"CertificateAuthentication")->ActualSize; 

				if(lDataSize > 0 && lDataSize <= (long)uiCALength) 
				{ 
					_variant_t varBLOB; 
					varBLOB = pRecordset->GetFields()->GetItem(L"CertificateAuthentication")->GetChunk(lDataSize); 
					if(varBLOB.vt == (VT_ARRAY | VT_UI1)) 
					{ 
						for(long index=0;index<lDataSize;index++) 
						{
							SafeArrayGetElement(varBLOB.parray, &index, &pbCertificateAuthentication[index]);
						}
					} 
					dwReturn = 0x01;
				}
				else
				{
					dwReturn = 0x02;
				}

			}
			else
			{
				dwReturn = 0x00;
			}
		}
		catch (_com_error &e)
		{
			WriteDatabaseError(e);
			dwReturn = 0xff;
		}

		try
		{
			//关闭记录集
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
			WriteDatabaseError(e);
			dwReturn = 0xff;
		}
	}
	else
	{
		dwReturn = 0xff;
	}

	if (0x00 != DisconnectDatabase(pConnection))
	{
		dwReturn = 0xff;
	}

	return dwReturn;
}

DWORD SetDboCertificateOfDepartInfo(wchar_t const * pwchDepartNumber, BYTE * const pbCertificateAuthentication, const UINT uiCALength)
{
	assert(NULL != pwchDepartNumber);
	assert(NULL != pbCertificateAuthentication);
	assert(uiCALength > 0);

	//返回值
	DWORD dwReturn(0x00);	

	_ConnectionPtr pConnection(NULL);

	if (0x00 == ConnectDatabase(pConnection))
	{
		_RecordsetPtr pRecordset;

		try
		{
			wchar_t wchSql[SQLMAX];
			swprintf_s(wchSql, SQLMAX, L"select CertificateAuthentication from DepartInfo where DepartNumber = '%s'", pwchDepartNumber);

			pRecordset.CreateInstance(L"ADODB.Recordset");
			pRecordset->Open((_bstr_t)wchSql, 
				_variant_t((IDispatch*)pConnection, true), 
				adOpenStatic, 
				adLockOptimistic, 
				adCmdText);

			// 有，update
			// 无，返回
			if(!pRecordset->adoEOF) 
			{
				VARIANT varBLOB; 
				SAFEARRAY *psa; 
				SAFEARRAYBOUND rgsabound[1]; 
				rgsabound[0].lLbound = 0; 
				rgsabound[0].cElements = uiCALength;
				psa = SafeArrayCreate(VT_UI1, 1, rgsabound); 

				for (long i = 0; i < (long)uiCALength; i++) 
				{
					SafeArrayPutElement (psa, &i, &pbCertificateAuthentication[i]); 
				}
				varBLOB.vt = VT_ARRAY | VT_UI1; 
				varBLOB.parray = psa; 
				pRecordset->GetFields()->GetItem(L"CertificateAuthentication")->AppendChunk(varBLOB); 

				pRecordset->Update();  // 保存到库中
			}
			else
			{
				dwReturn = 0x01;
			}

			//关闭记录集
			if (NULL != pRecordset && adStateClosed != pRecordset->State)
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
			WriteDatabaseError(e);
			dwReturn = 0xff;
		}

	}  // if (0x00 == ConnectDatabase(pConnection))

	if (0x00 != DisconnectDatabase(pConnection))
	{
		dwReturn = 0xff;
	}

	return dwReturn;	
}