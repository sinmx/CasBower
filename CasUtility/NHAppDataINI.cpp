// NHAppDataINI.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "NHAppDataINI.h"
#include "assert.h"
#include <fstream>
#include <time.h>
#include <tchar.h>

//临时改变宏的含义“输出”为“输入”
#undef AFX_EXT_CLASS
#undef AFX_EXT_API
#undef AFX_EXT_DATA
#define AFX_EXT_CLASS AFX_CLASS_IMPORT
#define AFX_EXT_API AFX_API_IMPORT
#define AFX_EXT_DATA AFX_DATA_IMPORT

#include "ConfigFile.h"
#pragma comment(lib, "inilite.lib")
#include "NHCommonAPI.h"

//恢复宏的含义为输出
#undef AFX_EXT_CLASS
#undef AFX_EXT_API
#undef AFX_EXT_DATA
#define AFX_EXT_CLASS AFX_CLASS_EXPORT
#define AFX_EXT_API AFX_API_EXPORT
#define AFX_EXT_DATA AFX_DATA_EXPORT

void GetIniLiteString(IConfigFileA* const pIniFile, wchar_t* const wchSection, wchar_t* const wchKeyword, wchar_t* const wchDefaultValue, wchar_t* wchKeyvalue, const size_t &nKeyValueLen)
{
	char* chSection(CNHCommonAPI::UnicodeToANSI(wchSection));
	char* chKeyword(CNHCommonAPI::UnicodeToANSI(wchKeyword));
	char* chDefaultValue(CNHCommonAPI::UnicodeToANSI(wchDefaultValue));
	char chKeyvalue[MAX_PATH*10];

	pIniFile->GetString(chSection, chKeyword, chDefaultValue, chKeyvalue, nKeyValueLen*sizeof(wchar_t));

	wchar_t* wchNewKeyvalue(CNHCommonAPI::ANSIToUnicode(chKeyvalue));

	wcscpy_s(wchKeyvalue, nKeyValueLen, wchNewKeyvalue);

	if (NULL != chSection)
	{
		free(chSection);
		chSection = NULL;
	}

	if (NULL != chKeyword)
	{
		free(chKeyword);
		chKeyword = NULL;
	}

	if (NULL != chDefaultValue)
	{
		free(chDefaultValue);
		chDefaultValue = NULL;
	}

	if (NULL != wchNewKeyvalue)
	{
		free(wchNewKeyvalue);
		wchNewKeyvalue = NULL;
	}
}

void WriteIniLiteString(IConfigFileA* const pIniFile, wchar_t* const wchSection, wchar_t* const wchKeyword, const wchar_t* wchKeyvalue)
{
	char* chSection(CNHCommonAPI::UnicodeToANSI(wchSection));
	char* chKeyword(CNHCommonAPI::UnicodeToANSI(wchKeyword));
	char* chKeyvalue(CNHCommonAPI::UnicodeToANSI(wchKeyvalue));

	pIniFile->WriteString(chSection, chKeyword, chKeyvalue);

	if (NULL != chSection)
	{
		free(chSection);
		chSection = NULL;
	}

	if (NULL != chKeyword)
	{
		free(chKeyword);
		chKeyword = NULL;
	}

	if (NULL != chKeyvalue)
	{
		free(chKeyvalue);
		chKeyvalue = NULL;
	}
}

void GetIniUserInfo(PUSERINFO const pIniUserInfo)
{
	// 获取CasInfo.ini路径
	wchar_t wchPathUserInfoIni[MAX_PATH];
	if (0x00 != CNHCommonAPI::GetFilePathEx(L"Config", L"CasInfo.ini", wchPathUserInfoIni))
	{
		return;
	}
	IConfigFileA* pIniFile = OpenConfigFileW(wchPathUserInfoIni);

	// 获取CasInfo.ini信息
	GetIniLiteString(pIniFile, L"UserInfo", L"Name", L"", pIniUserInfo->wchName, 50);
	//GetIniLiteString(pIniFile, L"UserInfo", L"Password", L"", pIniUserInfo->wchPassword, 50);
	GetIniLiteString(pIniFile, L"UserInfo", L"LicenseSn", L"",pIniUserInfo->wchLicenseSn, 50);
	GetIniLiteString(pIniFile, L"UserInfo", L"LicenseStartDate", L"",pIniUserInfo->wchLicenseStartDate, NH_TIME_TO_WCHART);
	GetIniLiteString(pIniFile, L"UserInfo", L"LicenseEndDate", L"", pIniUserInfo->wchLicenseEndDate, NH_TIME_TO_WCHART);
	GetIniLiteString(pIniFile, L"UserInfo", L"Role", L"", pIniUserInfo->wchRole, NH_INT_TO_WCHART);

	ReleaseConfigFile(pIniFile);
}

void SetIniUserInfo(const USERINFO& iniUserInfo)
{
	// 获取CasInfo.ini路径
	wchar_t wchPathUserInfoIni[MAX_PATH];
	if (0x00 != CNHCommonAPI::GetFilePathEx(L"Config", L"CasInfo.ini", wchPathUserInfoIni, true))
	{
		return;
	}
	IConfigFileA* pIniFile = OpenConfigFileW(wchPathUserInfoIni);

	// 写CasInfo.ini配置文件
	WriteIniLiteString(pIniFile, L"UserInfo", L"Name", iniUserInfo.wchName);
	//WriteIniLiteString(pIniFile, L"UserInfo", L"Password", iniUserInfo.wchPassword);
	WriteIniLiteString(pIniFile, L"UserInfo", L"LicenseSn", iniUserInfo.wchLicenseSn);
	WriteIniLiteString(pIniFile, L"UserInfo", L"LicenseStartDate", iniUserInfo.wchLicenseStartDate);
	WriteIniLiteString(pIniFile, L"UserInfo", L"LicenseEndDate", iniUserInfo.wchLicenseEndDate);
	WriteIniLiteString(pIniFile, L"UserInfo", L"Role", iniUserInfo.wchRole);

	ReleaseConfigFile(pIniFile);
}

//void GetIniResultOfASM(PRESULTOFASM const pIniResultOfASM)
//{
//	// 获取ResultOfASM.ini路径
//	wchar_t wchPathResultOfASMIni[MAX_PATH];
//	if (0x00 != CNHCommonAPI::GetFilePathEx(L"Config", L"ResultOfASM.ini", wchPathResultOfASMIni))
//	{
//		return;
//	}
//	IConfigFileA* pIniFile = OpenConfigFileW(wchPathResultOfASMIni);
//
//	// 获取ResultOfASM.ini信息
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"AutoID", L"", pIniResultOfASM->wchAutoID, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"ReportNumber", L"", pIniResultOfASM->wchReportNumber, 50);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"FuelTypeCode", L"", pIniResultOfASM->wchFuelTypeCode, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"FuelTypeName", L"", pIniResultOfASM->wchFuelTypeName, 50);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfHC5025", L"", pIniResultOfASM->wchLimitOfHC5025, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfHC5025_EDP", L"", pIniResultOfASM->wchLimitOfHC5025_EDP, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfCO5025", L"", pIniResultOfASM->wchLimitOfCO5025, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfCO5025_EDP", L"", pIniResultOfASM->wchLimitOfCO5025_EDP, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfNO5025", L"", pIniResultOfASM->wchLimitOfNO5025, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfNO5025_EDP", L"", pIniResultOfASM->wchLimitOfNO5025_EDP, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfHC2540", L"", pIniResultOfASM->wchLimitOfHC2540, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfHC2540_EDP", L"", pIniResultOfASM->wchLimitOfHC2540_EDP, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfCO2540", L"", pIniResultOfASM->wchLimitOfCO2540, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfCO2540_EDP", L"", pIniResultOfASM->wchLimitOfCO2540_EDP, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfNO2540", L"", pIniResultOfASM->wchLimitOfNO2540, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfNO2540_EDP", L"", pIniResultOfASM->wchLimitOfNO2540_EDP, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"Rev5025", L"", pIniResultOfASM->wchRev5025, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"OilTemperature5025", L"", pIniResultOfASM->wchOilTemperature5025, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"Lambda5025", L"", pIniResultOfASM->wchLambda5025, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"Power5025", L"", pIniResultOfASM->wchPower5025, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"HC5025", L"", pIniResultOfASM->wchHC5025, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"CO5025", L"", pIniResultOfASM->wchCO5025, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"NO5025", L"", pIniResultOfASM->wchNO5025, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"CO25025", L"", pIniResultOfASM->wchCO25025, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"O25025", L"", pIniResultOfASM->wchO25025, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"PassOfHC5025", L"", pIniResultOfASM->wchPassOfHC5025, NH_BOOL_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"PassOfCO5025", L"", pIniResultOfASM->wchPassOfCO5025, NH_BOOL_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"PassOfNO5025", L"", pIniResultOfASM->wchPassOfNO5025, NH_BOOL_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"NeedTestMode2540", L"", pIniResultOfASM->wchNeedTestMode2540, NH_BOOL_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"Rev2540", L"", pIniResultOfASM->wchRev2540, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"OilTemperature2540", L"", pIniResultOfASM->wchOilTemperature2540, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"Lambda2540", L"", pIniResultOfASM->wchLambda2540, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"Power2540", L"", pIniResultOfASM->wchPower2540, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"HC2540", L"", pIniResultOfASM->wchHC2540, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"CO2540", L"", pIniResultOfASM->wchCO2540, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"NO2540", L"", pIniResultOfASM->wchNO2540, NH_INT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"CO22540", L"", pIniResultOfASM->wchCO22540, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"O22540", L"", pIniResultOfASM->wchO22540, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"PassOfHC2540", L"", pIniResultOfASM->wchPassOfHC2540, NH_BOOL_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"PassOfCO2540", L"", pIniResultOfASM->wchPassOfCO2540, NH_BOOL_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"PassOfNO2540", L"", pIniResultOfASM->wchPassOfNO2540, NH_BOOL_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"Pass", L"", pIniResultOfASM->wchPass, NH_BOOL_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"EnvironmentalTemperature", L"", pIniResultOfASM->wchEnvironmentalTemperature, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"RelativeHumidity", L"", pIniResultOfASM->wchRelativeHumidity, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"AtmosphericPressure", L"", pIniResultOfASM->wchAtmosphericPressure, NH_FLOAT_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"SkipRev", L"", pIniResultOfASM->wchSkipRev, NH_BOOL_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"SkipOilTemperature", L"", pIniResultOfASM->wchSkipOilTemperature, NH_BOOL_TO_WCHART);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"Remark", L"", pIniResultOfASM->wchRemark, 1024);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"Reserved1", L"", pIniResultOfASM->wchReserved1, 50);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"Reserved2", L"", pIniResultOfASM->wchReserved2, 50);
//	GetIniLiteString(pIniFile, L"ResultOfASM", L"Reserved3", L"", pIniResultOfASM->wchReserved3, 50);
//
//	ReleaseConfigFile(pIniFile);
//}
//
//void SetIniResultOfASM(const RESULTOFASM& iniResultOfASM)
//{
//	// 获取ResultOfASM.ini路径
//	wchar_t wchPathResultOfASMIni[MAX_PATH];
//	if (0x00 != CNHCommonAPI::GetFilePathEx(L"Config", L"ResultOfASM.ini", wchPathResultOfASMIni, true))
//	{
//		return;
//	}
//	IConfigFileA* pIniFile = OpenConfigFileW(wchPathResultOfASMIni);
//
//	// 获取ResultOfASM.ini信息
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"AutoID", iniResultOfASM.wchAutoID);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"ReportNumber", iniResultOfASM.wchReportNumber);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"FuelTypeCode", iniResultOfASM.wchFuelTypeCode);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"FuelTypeName", iniResultOfASM.wchFuelTypeName);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfHC5025", iniResultOfASM.wchLimitOfHC5025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfHC5025_EDP", iniResultOfASM.wchLimitOfHC5025_EDP);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfCO5025", iniResultOfASM.wchLimitOfCO5025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfCO5025_EDP", iniResultOfASM.wchLimitOfCO5025_EDP);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfNO5025", iniResultOfASM.wchLimitOfNO5025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfNO5025_EDP", iniResultOfASM.wchLimitOfNO5025_EDP);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfHC2540", iniResultOfASM.wchLimitOfHC2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfHC2540_EDP", iniResultOfASM.wchLimitOfHC2540_EDP);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfCO2540", iniResultOfASM.wchLimitOfCO2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfCO2540_EDP", iniResultOfASM.wchLimitOfCO2540_EDP);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfNO2540", iniResultOfASM.wchLimitOfNO2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"LimitOfNO2540_EDP", iniResultOfASM.wchLimitOfNO2540_EDP);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"Rev5025", iniResultOfASM.wchRev5025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"OilTemperature5025", iniResultOfASM.wchOilTemperature5025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"Lambda5025", iniResultOfASM.wchLambda5025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"Power5025", iniResultOfASM.wchPower5025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"HC5025", iniResultOfASM.wchHC5025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"CO5025", iniResultOfASM.wchCO5025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"NO5025", iniResultOfASM.wchNO5025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"CO25025", iniResultOfASM.wchCO25025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"O25025", iniResultOfASM.wchO25025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"PassOfHC5025", iniResultOfASM.wchPassOfHC5025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"PassOfCO5025", iniResultOfASM.wchPassOfCO5025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"PassOfNO5025", iniResultOfASM.wchPassOfNO5025);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"NeedTestMode2540", iniResultOfASM.wchNeedTestMode2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"Rev2540", iniResultOfASM.wchRev2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"OilTemperature2540", iniResultOfASM.wchOilTemperature2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"Lambda2540", iniResultOfASM.wchLambda2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"Power2540", iniResultOfASM.wchPower2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"HC2540", iniResultOfASM.wchHC2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"CO2540", iniResultOfASM.wchCO2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"NO2540", iniResultOfASM.wchNO2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"CO22540", iniResultOfASM.wchCO22540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"O22540", iniResultOfASM.wchO22540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"PassOfHC2540", iniResultOfASM.wchPassOfHC2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"PassOfCO2540", iniResultOfASM.wchPassOfCO2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"PassOfNO2540", iniResultOfASM.wchPassOfNO2540);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"Pass", iniResultOfASM.wchPass);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"EnvironmentalTemperature", iniResultOfASM.wchEnvironmentalTemperature);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"RelativeHumidity", iniResultOfASM.wchRelativeHumidity);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"AtmosphericPressure", iniResultOfASM.wchAtmosphericPressure);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"SkipRev", iniResultOfASM.wchSkipRev);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"SkipOilTemperature", iniResultOfASM.wchSkipOilTemperature);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"Remark", iniResultOfASM.wchRemark);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"Reserved1", iniResultOfASM.wchReserved1);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"Reserved2", iniResultOfASM.wchReserved2);
//	WriteIniLiteString(pIniFile, L"ResultOfASM", L"Reserved3", iniResultOfASM.wchReserved3);
//
//	ReleaseConfigFile(pIniFile);
//}
//
//void GetIniRealTimeDataOfASM(list<REALTIMEDATAOFASM>& listRealTimeDataOfASM)
//{
//	// 获取RealTimeDataOfASM.ini路径
//	wchar_t wchPathOfASMRealTimeData[MAX_PATH];
//	if (0x00 != CNHCommonAPI::GetFilePathEx(L"Config", L"RealTimeDataOfASM.ini", wchPathOfASMRealTimeData))
//	{
//		return;
//	}
//	IConfigFileA* pIniFile = OpenConfigFileW(wchPathOfASMRealTimeData);
//
//	// 读取总条数
//	wchar_t wchTotal[NH_INT_TO_WCHART];
//	GetIniLiteString(pIniFile, L"Sum", L"Total", L"0", wchTotal, NH_INT_TO_WCHART);
//	int nTotal = _wtoi(wchTotal);	
//	if(nTotal <= 0)
//	{
//		return;
//	}
//
//	RealTimeDataOfASM iniRealTimeDataOfASM;
//
//	for(int i=1; i<=nTotal; i++)
//	{
//		// 获取RealTimeDataOfASM.ini信息
//		wchar_t wchSection[NH_INT_TO_WCHART];
//		wsprintf(wchSection, L"%d", i);
//		GetIniLiteString(pIniFile, wchSection, L"AutoID", L"", iniRealTimeDataOfASM.wchAutoID, NH_INT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"ReportNumber", L"", iniRealTimeDataOfASM.wchReportNumber, 50);
//		GetIniLiteString(pIniFile, wchSection, L"FuelTypeCode", L"", iniRealTimeDataOfASM.wchFuelTypeCode, NH_INT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"FuelTypeName", L"", iniRealTimeDataOfASM.wchFuelTypeName, 50);
//		GetIniLiteString(pIniFile, wchSection, L"Time", L"", iniRealTimeDataOfASM.wchTime, NH_INT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"SamplingTime", L"", iniRealTimeDataOfASM.wchSamplingTime, 50);
//		GetIniLiteString(pIniFile, wchSection, L"Velocity", L"", iniRealTimeDataOfASM.wchVelocity, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"Power", L"", iniRealTimeDataOfASM.wchPower, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"Force", L"", iniRealTimeDataOfASM.wchForce, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"IHP", L"", iniRealTimeDataOfASM.wchIHP, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"PLHP", L"", iniRealTimeDataOfASM.wchPLHP, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"State", L"", iniRealTimeDataOfASM.wchState, 50);
//		GetIniLiteString(pIniFile, wchSection, L"HC", L"", iniRealTimeDataOfASM.wchHC, NH_INT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"CO", L"", iniRealTimeDataOfASM.wchCO, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"NO", L"", iniRealTimeDataOfASM.wchNO, NH_INT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"O2", L"", iniRealTimeDataOfASM.wchO2, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"CO2", L"", iniRealTimeDataOfASM.wchCO2, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"EngineRev", L"", iniRealTimeDataOfASM.wchEngineRev, NH_INT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"OilTemperature", L"", iniRealTimeDataOfASM.wchOilTemperature, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"Lambda", L"", iniRealTimeDataOfASM.wchLambda, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"EnvironmentalTemperature", L"", iniRealTimeDataOfASM.wchEnvironmentalTemperature, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"RelativeHumidity", L"", iniRealTimeDataOfASM.wchRelativeHumidity, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"AtmosphericPressure", L"", iniRealTimeDataOfASM.wchAtmosphericPressure, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"HumidityCorrectionFactor", L"", iniRealTimeDataOfASM.wchHumidityCorrectionFactor, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"DilutionCorrectionFactor", L"", iniRealTimeDataOfASM.wchDilutionCorrectionFactor, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"Remark", L"", iniRealTimeDataOfASM.wchRemark, 1024);
//		GetIniLiteString(pIniFile, wchSection, L"Reserved1", L"", iniRealTimeDataOfASM.wchReserved1, 50);
//		GetIniLiteString(pIniFile, wchSection, L"Reserved2", L"", iniRealTimeDataOfASM.wchReserved2, 50);
//		GetIniLiteString(pIniFile, wchSection, L"Reserved3", L"", iniRealTimeDataOfASM.wchReserved3, 50);
//
//		listRealTimeDataOfASM.push_back(iniRealTimeDataOfASM);
//	}
//
//	ReleaseConfigFile(pIniFile);
//}
//
//void GetIniRealTimeDataOfASMVector(vector<REALTIMEDATAOFASM>& vRealTimeDataOfASM)
//{
//	// 获取RealTimeDataOfASM.ini路径
//	wchar_t wchPathOfASMRealTimeData[MAX_PATH];
//	if (0x00 != CNHCommonAPI::GetFilePathEx(L"Config", L"RealTimeDataOfASM.ini", wchPathOfASMRealTimeData))
//	{
//		return;
//	}
//	IConfigFileA* pIniFile = OpenConfigFileW(wchPathOfASMRealTimeData);
//
//	// 读取总条数
//	wchar_t wchTotal[NH_INT_TO_WCHART];
//	GetIniLiteString(pIniFile, L"Sum", L"Total", L"0", wchTotal, NH_INT_TO_WCHART);
//	int nTotal = _wtoi(wchTotal);	
//	if(nTotal <= 0)
//	{
//		return;
//	}
//
//	RealTimeDataOfASM iniRealTimeDataOfASM;
//
//	for(int i=1; i<=nTotal; i++)
//	{
//		// 获取RealTimeDataOfASM.ini信息
//		wchar_t wchSection[NH_INT_TO_WCHART];
//		wsprintf(wchSection, L"%d", i);
//		GetIniLiteString(pIniFile, wchSection, L"AutoID", L"", iniRealTimeDataOfASM.wchAutoID, NH_INT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"ReportNumber", L"", iniRealTimeDataOfASM.wchReportNumber, 50);
//		GetIniLiteString(pIniFile, wchSection, L"FuelTypeCode", L"", iniRealTimeDataOfASM.wchFuelTypeCode, NH_INT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"FuelTypeName", L"", iniRealTimeDataOfASM.wchFuelTypeName, 50);
//		GetIniLiteString(pIniFile, wchSection, L"Time", L"", iniRealTimeDataOfASM.wchTime, NH_INT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"SamplingTime", L"", iniRealTimeDataOfASM.wchSamplingTime, 50);
//		GetIniLiteString(pIniFile, wchSection, L"Velocity", L"", iniRealTimeDataOfASM.wchVelocity, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"Power", L"", iniRealTimeDataOfASM.wchPower, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"Force", L"", iniRealTimeDataOfASM.wchForce, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"IHP", L"", iniRealTimeDataOfASM.wchIHP, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"PLHP", L"", iniRealTimeDataOfASM.wchPLHP, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"State", L"", iniRealTimeDataOfASM.wchState, 50);
//		GetIniLiteString(pIniFile, wchSection, L"HC", L"", iniRealTimeDataOfASM.wchHC, NH_INT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"CO", L"", iniRealTimeDataOfASM.wchCO, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"NO", L"", iniRealTimeDataOfASM.wchNO, NH_INT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"O2", L"", iniRealTimeDataOfASM.wchO2, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"CO2", L"", iniRealTimeDataOfASM.wchCO2, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"EngineRev", L"", iniRealTimeDataOfASM.wchEngineRev, NH_INT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"OilTemperature", L"", iniRealTimeDataOfASM.wchOilTemperature, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"Lambda", L"", iniRealTimeDataOfASM.wchLambda, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"EnvironmentalTemperature", L"", iniRealTimeDataOfASM.wchEnvironmentalTemperature, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"RelativeHumidity", L"", iniRealTimeDataOfASM.wchRelativeHumidity, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"AtmosphericPressure", L"", iniRealTimeDataOfASM.wchAtmosphericPressure, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"HumidityCorrectionFactor", L"", iniRealTimeDataOfASM.wchHumidityCorrectionFactor, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"DilutionCorrectionFactor", L"", iniRealTimeDataOfASM.wchDilutionCorrectionFactor, NH_FLOAT_TO_WCHART);
//		GetIniLiteString(pIniFile, wchSection, L"Remark", L"", iniRealTimeDataOfASM.wchRemark, 1024);
//		GetIniLiteString(pIniFile, wchSection, L"Reserved1", L"", iniRealTimeDataOfASM.wchReserved1, 50);
//		GetIniLiteString(pIniFile, wchSection, L"Reserved2", L"", iniRealTimeDataOfASM.wchReserved2, 50);
//		GetIniLiteString(pIniFile, wchSection, L"Reserved3", L"", iniRealTimeDataOfASM.wchReserved3, 50);
//
//		vRealTimeDataOfASM.push_back(iniRealTimeDataOfASM);
//	}
//
//	ReleaseConfigFile(pIniFile);
//}
