#include "stdafx.h"
#include "CEFWebkitBrowser.h"

// WKEWebkitBrowser.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "CEFWebkitBrowser.h"
#include <strsafe.h>

#include <MMSystem.h>
#pragma comment(lib, "Winmm.lib")


#define 	E_GOFORWORD_TIMER  100
CEFWebkitBrowserWnd *CEFWebkitBrowserWnd::s_instance = NULL;
UINT g_Part = 0;

wchar_t * ANSIToUnicode(const char *str)
{
	// 支持(str==NULL)的情况

	int textlen(0);
	wchar_t *result(NULL);
	textlen = MultiByteToWideChar(CP_ACP, 0, str,-1, NULL, 0); 
	result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t)); 
	memset(result, 0, (textlen+1)*sizeof(wchar_t)); 
	MultiByteToWideChar(CP_ACP, 0, str, -1, (LPWSTR)result,textlen); 
	return result;
}

DWORD GetHLDFilePath(const wchar_t *const pwchFileFolder, const wchar_t *const pwchFileName, wchar_t *const pwchFilePath)
{
	assert(NULL != pwchFileFolder);
	assert(NULL != pwchFileName);
	assert(NULL != pwchFilePath);


	GetModuleFileNameW(NULL, pwchFilePath, MAX_PATH);
	// 向上1层
	wchar_t *pwchDest = wcsrchr(pwchFilePath, L'\\');
	*pwchDest = L'\0';
	// 向上1层
	pwchDest = wcsrchr(pwchFilePath, L'\\');
	*pwchDest = L'\0';

	// 文件夹路径
	wcscat_s(pwchFilePath, MAX_PATH, L"\\");
	wcscat_s(pwchFilePath, MAX_PATH, pwchFileFolder);
	wchar_t wchFolderPath[MAX_PATH];
	ZeroMemory(wchFolderPath, sizeof(wchFolderPath));
	wcsncpy_s(wchFolderPath, _countof(wchFolderPath), pwchFilePath, _TRUNCATE);





	// 判断文件夹是否存在
	if (_waccess(wchFolderPath, 00))
	{
		// 文件夹不存在
		// 创建文件夹
		if (!CreateDirectory(wchFolderPath, NULL))
		{ 
			// 创建文件夹失败,返回
			return 0x01;
		}
	}

	// 文件路径
	wcscat_s(pwchFilePath, MAX_PATH, L"\\");
	wcscat_s(pwchFilePath, MAX_PATH, pwchFileName);

	// 判断文件是否存在
	if (_waccess(pwchFilePath, 00))
	{
		// 文件不存在
		return 0x02;
	}

	// 去除文件只读属性
	DWORD dwAttributes(0);
	dwAttributes = ::GetFileAttributes(pwchFilePath);
	dwAttributes &= ~FILE_ATTRIBUTE_READONLY;
	::SetFileAttributes(pwchFilePath, dwAttributes);

	return 0x00;
}

CEFWebkitBrowserWnd::CEFWebkitBrowserWnd()
{
	m_pWKEWebkitCtrl = NULL;
}

CEFWebkitBrowserWnd::~CEFWebkitBrowserWnd()
{

}


CControlUI* CEFWebkitBrowserWnd::CreateControl(LPCTSTR pstrClassName)
{
	if (_tcsicmp(pstrClassName, _T("CEFWebkitBrowser")) == 0)
	{
		return  (m_pWKEWebkitCtrl = new CCEFWebkitUI(GetSafeHwnd()));
	}

	return NULL;
}

void CEFWebkitBrowserWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

LRESULT CEFWebkitBrowserWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostQuitMessage(0L);
	return __super::OnDestroy(uMsg, wParam, lParam, bHandled);
}

//截取WM_CLOSE消息
LRESULT CEFWebkitBrowserWnd::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if ((m_pWKEWebkitCtrl != NULL) && (!m_pWKEWebkitCtrl->IsClosed()))
	{
		//m_pWKEWebkitCtrl->CloseAllPage();
		bHandled = TRUE;
		//  Cancel the close.

	}
	else
	{
		bHandled = TRUE;
	}

	return 0;
}

//标题改变
void CEFWebkitBrowserWnd::OnTitleChanged(int nID, const CefString str)
{
	nID;
	str;
	return;
}


void CEFWebkitBrowserWnd::OnAfterCreate(int nWebBrowserID)
{

}

void CEFWebkitBrowserWnd::OnBrowserClose(int nBrowserID)
{

}

void CEFWebkitBrowserWnd::OnWebLoadEnd(WPARAM wParam, LPARAM lParam)
{

}

void CEFWebkitBrowserWnd::OnWebLoadStart(WPARAM wParam, LPARAM lParam)
{

}

COptionUI * CEFWebkitBrowserWnd::GetActiveOption() const
{
	COptionUI* pCurrentOpt = NULL;
	return pCurrentOpt;
}

void CEFWebkitBrowserWnd::SwitchUIState()
{
	COptionUI* pCurrentOpt = NULL;
	COptionTag* pTag = NULL;
}


void CEFWebkitBrowserWnd::InitWindow()
{

}

void CEFWebkitBrowserWnd::Notify(TNotifyUI& msg)
{
	//if (msg.sType == DUI_MSGTYPE_CLICK)
	//{
	//	if (msg.pSender->GetName() == _T("ui_btn_goback"))
	//	{
	//		COptionUI* pCurrentOpt = NULL;
	//		COptionTag* pTag = NULL;

	//		if (pCurrentOpt = GetActiveOption())
	//		{
	//			if (pTag = (COptionTag*)pCurrentOpt->GetTag())
	//			{

	//				m_pWKEWebkitCtrl->GoBack(pTag->nID_);
	//			}
	//		}
	//	}
	//	else if (msg.pSender->GetName() == _T("ui_btn_goforward"))
	//	{
	//		COptionUI* pCurrentOpt = NULL;
	//		COptionTag* pTag = NULL;

	//		if (pCurrentOpt = GetActiveOption())
	//		{
	//			if (pTag = (COptionTag*)pCurrentOpt->GetTag())
	//			{

	//				m_pWKEWebkitCtrl->GoForward(pTag->nID_);
	//			}
	//		}
	//	}
	//	else if (msg.pSender->GetName() == _T("ui_btn_refresh"))
	//	{
	//		COptionUI* pCurrentOpt = NULL;
	//		COptionTag* pTag = NULL;

	//		if (pCurrentOpt = GetActiveOption())
	//		{
	//			if (pTag = (COptionTag*)pCurrentOpt->GetTag())
	//			{
	//				m_pWKEWebkitCtrl->ReLoad(pTag->nID_);
	//			}
	//		}

	//	}
	//	else if (msg.pSender->GetName() == _T("ui_btn_home"))
	//	{
	//		m_pWKEWebkitCtrl->NewPage("http://web.yy.com/city_redpackge/task.html");
	//	}
	//	else if (msg.pSender->GetName() == _T("ui_btn_newpage"))
	//	{
	//		if (m_pWKEWebkitCtrl)
	//		{
	//			//	m_pWKEWebkitCtrl->NewPage(_T("about:black"));
	//			m_pWKEWebkitCtrl->NewPage(_T("about:blank"));
	//		}
	//	}


	//}
	//else if (msg.sType == _T("windowinit"))
	//{
	//	OnInitComplate();
	//}
	//else if (msg.sType == DUI_MSGTYPE_TIMER)
	//{
	//	if (msg.pSender == m_pWKEWebkitCtrl)
	//	{
	//		if (msg.wParam == E_GOFORWORD_TIMER)
	//		{
	//			SwitchUIState();
	//		}

	//	}

	//}


	//__super::Notify(msg);
}

LRESULT CEFWebkitBrowserWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//TCHAR szBuf[256];

	switch (uMsg)
	{

	case UM_CEF_POSTQUITMESSAGE:
		bHandled = TRUE;
		//Sleep(2000);
		//	CefQuitMessageLoop();
		PostQuitMessage(0L);
		break;
	case UM_CEF_WEBLOADPOPUP:
		{

			CefString* pStrComplateURL = (CefString*)lParam;
			if (pStrComplateURL != NULL)
			{
				m_pWKEWebkitCtrl->NewPage(*pStrComplateURL);

				delete pStrComplateURL;
			}

		}
		break;
	case UM_CEF_WEBLOADEND:
		{
			OnWebLoadEnd(wParam, lParam);
		}
		break;

	case UM_CEF_WEBLOADSTART:
		{
			OnWebLoadStart(wParam, lParam);
		}
		break;
	case UM_CEF_WEBTITLECHANGE:
		{
			CefString* pStrComplateURL = (CefString*)lParam;
			if (pStrComplateURL != NULL)
			{
				OnTitleChanged(wParam, *pStrComplateURL);
				delete pStrComplateURL;
			}
		}
		break;
	case UM_CEF_AFTERCREATED:
		{
			OnAfterCreate(wParam);
		}
		break;
	case UM_CEF_BROWSERCLOSE:
		{
			OnBrowserClose(wParam);
		}
		break;
	default:
		break;
	}

	bHandled = FALSE;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//开头


void CEFWebkitBrowserWnd::OnClick(TNotifyUI & msg)
{
	CDuiString strName = msg.pSender->GetName();

	if (_T("ui_btn_close") == strName)
	{
		if (m_bCanCloseExe && (m_pWKEWebkitCtrl != NULL) && (!m_pWKEWebkitCtrl->IsClosed()))
		{
			m_pWKEWebkitCtrl->CloseAllPage();
		}
		else
		{
			MessageBoxW(GetForegroundWindow(), L"程序正在运行中，请先结束后再关闭!", TIP_INFO, MB_OK);
		}

	}

	__super::OnClick(msg);

}

void WINAPI CEFWebkitBrowserWnd::onTimeFunc(UINT wTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2)
{
	if (dwUser == 1)
	{
		wTimerID = 1;
		CEFWebkitBrowserWnd::s_instance->m_bCanCloseExe;
	} 
	else if (dwUser == 2)
	{
		wTimerID = 2;
	}
}

void CEFWebkitBrowserWnd::OnInitComplate()
{
	init();
}

CEFWebkitBrowserWnd* CEFWebkitBrowserWnd::instance()
{
	if (s_instance == NULL)
	{
		s_instance = new CEFWebkitBrowserWnd();
	}

	return s_instance;
}

void CEFWebkitBrowserWnd::destroy()
{
	//CNHSQLServerDBO::CloseDB(m_pConnection);
	DW_SAFE_DELETE(s_instance);
}

void CEFWebkitBrowserWnd::init()
{
	//init
	m_bCanCloseExe = true;
	
	//db
	CNHSQLServerDBO::CloseDB(m_pConnection);
	if (0x00 != CNHSQLServerDBO::OpenDB(m_pConnection))
	{
		::MessageBoxW(GetForegroundWindow(), L"数据库打开失败!", TIP_INFO, MB_OK);
		return;
	}

	if (m_pWKEWebkitCtrl)
	{
		//string sFile = CPaintManagerUI::GetResourcePath() + _T("dist\\index.html");
		//m_pWKEWebkitCtrl->NewPage(sFile.c_str());

		m_pWKEWebkitCtrl->NewPage(_T("http://localhost:8080/#/login"));
	}

	getPart();
	getDBOInfo();


	UINT timer = timeSetEvent(1000,1,(LPTIMECALLBACK)CEFWebkitBrowserWnd::onTimeFunc, 1, TIME_PERIODIC);
	timer = timeSetEvent(1000,1,(LPTIMECALLBACK)CEFWebkitBrowserWnd::onTimeFunc, 2, TIME_PERIODIC);

	//GetPaintManager()->SetTimer(m_pWKEWebkitCtrl, E_GOFORWORD_TIMER, 1000);
}

void CEFWebkitBrowserWnd::getPart()
{
	//ini
	wchar_t wchPath[MAX_PATH];
	ZeroMemory(wchPath, sizeof(wchPath));
	if (0x00 == GetHLDFilePath(L"Config", L"CasInfo.ini", wchPath))
	{
		// 获取数据库连接信息
		wchar_t wchTmp[MAX_PATH] = {0};
		GetPrivateProfileString(L"Database", L"Depart", L"", wchTmp, MAX_PATH, wchPath);
		g_Part = _wtoi(wchTmp);
	}
}

void CEFWebkitBrowserWnd::getDBOInfo()
{
	string strSql;
	char strTmp[256];
	_RecordsetPtr pSet;
	strSql = "select * from Dye";
	if (0x00 == OpenQuery(pSet, strSql))
	{
		m_mapDyeControl.clear();
		while(!pSet->adoEOF)
		{
			SDyeControl info;

			info.processnum = getField(pSet, "processnum");
			info.processname = getField(pSet, "processname");
			info.kindnum = getField(pSet, "kindnum");
			info.kindname = getField(pSet, "kindname");
			info.dyenum = getField(pSet, "num");
			info.dyename = getField(pSet, "name");
			info.water_high = getField(pSet, "water_high");
			info.water_low = getField(pSet, "water_low");
			info.cy_num = getField(pSet, "cy_num");
			info.cy = getField(pSet, "cy");
			info.cy_check = getField(pSet, "cy_check");
			info.unit = getField(pSet, "unit");	
			info.cy_num = getField(pSet, "cy_num");	
			info.beforetime = getField(pSet, "beforetime");
			sprintf(strTmp, "store%d", g_Part);
			info.store = getField(pSet, strTmp);	
			info.speed = getField(pSet, "speed");	
			m_mapDyeControl.insert(make_pair(info.dyenum, info));

			pSet->MoveNext();
		}
	}
	CloseQuery(pSet);

	//染缸信息
	strSql = "select * from VatsInfo";
	if (0x00 == OpenQuery(pSet, strSql))
	{
		while(!pSet->adoEOF)
		{
			SVatsInfo info;
			info.num = getField(pSet, "num");
			info.valve = getField(pSet, "valve");
			info.valve_check = getField(pSet, "valve_check");
			info.control_num = getField(pSet, "control_num");
			info.control = getField(pSet, "control");
			info.control_check = getField(pSet, "control_check");
			info.water_dosage = getField(pSet, "water_dosage");
			info.after_time = getField(pSet, "after_time");
			info.machine = atoi(getField(pSet, "machine").c_str()); 
			m_mapVatsInfo.insert(make_pair(info.num, info));
			pSet->MoveNext();
		}

	}
	CloseQuery(pSet);

	strSql = "select * from ControlInfo";
	if (0x00 == OpenQuery(pSet, strSql))
	{
		if (!pSet->adoEOF)
		{
			m_ControlIn.water_come = getField(pSet, "water_come1");
			m_ControlIn.water_put = getField(pSet, "water_put1");
			m_ControlIn.cy_come = getField(pSet, "cy_come1");
			m_ControlIn.gas_come = getField(pSet, "gas_come1");
			m_ControlIn.water_alarm = getField(pSet, "water_alarm1");
			m_ControlIn.gas_alarm = getField(pSet, "gas_alarm1");
			m_ControlIn.freq_alarm = getField(pSet, "freq_alarm1");
			m_ControlIn.gas_put = getField(pSet, "gas_put1");
			m_ControlIn.water_put2 = getField(pSet, "water_put2");
			m_ControlIn.motor_come = getField(pSet, "motor_come2");

			m_ControlOut.water_come = getField(pSet, "water_come");
			m_ControlOut.water_put = getField(pSet, "water_put");
			m_ControlOut.cy_come = getField(pSet, "cy_come");
			m_ControlOut.gas_come = getField(pSet, "gas_come");
			m_ControlOut.motor_come = getField(pSet, "motor_come");
			m_ControlOut.normal_alarm = getField(pSet, "normal_alarm");
			m_ControlOut.gaswater_alarm = getField(pSet, "gaswater_alarm");
			m_ControlOut.freq_alarm = getField(pSet, "freq_alarm");
			m_ControlOut.running_light = getField(pSet, "running_light");
			m_ControlOut.gas_put = getField(pSet, "gas_put");
			m_ControlOut.reset = getField(pSet, "reset");
			m_ControlOut.water_put2 = getField(pSet, "water_put3");

		}

	}
	CloseQuery(pSet);

	//base
	sprintf(strTmp, "select * from ClientInfo where id=%d", g_Part);
	strSql = strTmp;
	if (0x00 == OpenQuery(pSet, strSql))
	{
		if(!pSet->adoEOF)
		{
			m_BaseConfig.kind1 = getField(pSet, "kind1");
			//m_BaseConfig.machine = 0;
			m_BaseConfig.kind2 = getField(pSet, "kind2");
			m_BaseConfig.kind3 = getField(pSet, "kind3");
			m_BaseConfig.man = getField(pSet, "man");
			m_BaseConfig.water_time = getField(pSet, "water_time");
			m_BaseConfig.gas_time = getField(pSet, "gas_time");
			m_BaseConfig.cy_spit = getField(pSet, "cy_spit");
			m_BaseConfig.water_spit = getField(pSet, "water_spit");
			m_BaseConfig.dengdai_spit = getField(pSet, "dengdai_spit");
			m_BaseConfig.dengdai2_spit = getField(pSet, "dengdai2_spit");
			m_BaseConfig.dengdai3_spit = getField(pSet, "dengdai3_spit");
			m_BaseConfig.dengdai4_spit = getField(pSet, "dengdai4_spit");
			m_BaseConfig.super_auto = getField(pSet, "super_auto")=="1"?true:false;
		}

	}
	CloseQuery(pSet);
}

void CEFWebkitBrowserWnd::setBaseConfig(SBaseConfig &value)
{

}

DWORD CEFWebkitBrowserWnd::OpenQuery(_RecordsetPtr &pRecordset, const string strSql)
{
	return CNHSQLServerDBO::OpenQuery(m_pConnection, pRecordset, ANSIToUnicode(strSql.c_str()));
}

DWORD CEFWebkitBrowserWnd::CloseQuery(_RecordsetPtr &pRecordset)
{
	return CNHSQLServerDBO::CloseQuery(pRecordset);
}

DWORD CEFWebkitBrowserWnd::ExecuteDML(const string strSql, int *const pnRowsInvolved)
{
	return CNHSQLServerDBO::ExecuteDML(m_pConnection, ANSIToUnicode(strSql.c_str()), pnRowsInvolved);
}

string CEFWebkitBrowserWnd::getField(const _RecordsetPtr &pRecordset, const string strValue)
{
	string strResult("");
	char strTmp[1024] = {0};
	if (!pRecordset->adoEOF)
	{
		m_vField.ChangeType(VT_NULL);
		CNHSQLServerDBO::GetFieldValue(pRecordset, m_vField, ANSIToUnicode(strValue.c_str()));
		switch (m_vField.vt)
		{
		case VT_I4:
		case VT_I2:
			{
				sprintf(strTmp, "%d", (int)m_vField);				
			}
			break;
		case VT_BSTR:
			{
				sprintf(strTmp, "%s", (const char*)(_bstr_t)m_vField);
			}
			break;
		case VT_DATE:
			{
				sprintf(strTmp, "%s", (const char*)(_bstr_t)m_vField);
			}
			break;
		case VT_DECIMAL:
			{
				sprintf(strTmp, "%.3f", _wtof((LPCTSTR)(_bstr_t)m_vField)/1000.0f);
			}
			break;
		case VT_BOOL:
			{
				sprintf(strTmp, "%s", (bool)m_vField);
			}
			break;
		}
	}
	strResult = strTmp;
	return strResult;
}

int CEFWebkitBrowserWnd::setgetPLC(const char *pchSet, bool bSetOn, const char *pchGet)
{
	if (!m_SMachineInfo.pPLC)
	{
		return PLC_OPEN_ERROR;
	}

	if (m_SMachineInfo.pPLC->PLCSet(pchSet, bSetOn))
	{
		int ret = m_SMachineInfo.pPLC->PLCRead(pchGet);
		if (PLC_PROT_BIHE == ret)
		{
			return PLC_PROT_BIHE;
		}
		else if (PLC_PROT_DUAN == ret)
		{
			return PLC_PROT_DUAN;
		}
		else
		{
			return PLC_PROT_ERROR;
		}

	}

	return PLC_SET_WRONG;
}

int CEFWebkitBrowserWnd::getsetPLC(const char *pchGet, bool bGetOn, const char *pchSet, bool bSetOn)
{
	if (!m_SMachineInfo.pPLC)
	{
		return PLC_OPEN_ERROR;
	}

	int ret = m_SMachineInfo.pPLC->PLCRead(pchGet);
	if (PLC_PROT_ERROR == ret)
	{
		return PLC_PROT_ERROR;
	}

	if (bGetOn)
	{
		if (PLC_PROT_BIHE == ret)
		{

		}
		else if (PLC_PROT_DUAN == ret)
		{
			return PLC_PROT_DUAN;
		}
	} 
	else
	{
		if (PLC_PROT_DUAN == ret)
		{

		}
		else if (PLC_PROT_BIHE == ret)
		{
			return PLC_PROT_BIHE;
		}
	}


	if (m_SMachineInfo.pPLC->PLCSet(pchSet, bSetOn))
	{	
		return PLC_SET_OK;
	}
	else
	{
		return PLC_SET_WRONG;
	}
}


void CEFWebkitBrowserWnd::setControlIn(const SControlIn & info)
{
	//string strSql;
	//strSql.Format(L"update ControlInfo set water_come1='%s', water_put1='%s', cy_come1='%s', gas_come1='%s',\
	//				 water_alarm1='%s', gas_alarm1='%s', freq_alarm1='%s', gas_put1='%s', water_put2='%s', motor_come2='%s'",
	//				 info.water_come,info.water_put,info.cy_come,info.gas_come,info.water_alarm,
	//				 info.gas_alarm,info.freq_alarm,info.gas_put,info.water_put2,info.motor_come);
	//m_ControlIn = info;
	//ExecuteDML(strSql);
}

void CEFWebkitBrowserWnd::setControlOut(const SControlOut & info)
{
	string strSql;
	strSql = "update ControlInfo set water_come='";
	strSql += info.water_come;
	strSql += "', water_put='";
	strSql += info.water_put;
	
	strSql += "', cy_come='";
	strSql += info.cy_come;

	strSql += "', gas_come='";
	strSql += info.gas_come;

	strSql += "', motor_come='";
	strSql += info.motor_come;

	strSql += "', normal_alarm='";
	strSql += info.normal_alarm;

	strSql += "', gaswater_alarm='";
	strSql += info.gaswater_alarm;

	strSql += "', running_light='";
	strSql += info.running_light;

	strSql += "', gas_put='";
	strSql += info.gas_put;

	strSql += "', reset='";
	strSql += info.reset;

	strSql += "', water_put3='";
	strSql += info.water_put2;

	strSql += "', freq_alarm='";
	strSql += info.freq_alarm;
	strSql += "'";

	//strSql.Format(L"update ControlInfo set water_come='%s', water_put='%s', cy_come='%s', gas_come='%s',motor_come='%s', normal_alarm='%s',\
	//				 gaswater_alarm='%s', freq_alarm='%s', running_light='%s', reset='%s', gas_put='%s',water_put3='%s'",
	//				 info.water_come,info.water_put,info.cy_come,info.gas_come,
	//				 info.motor_come,info.normal_alarm,info.gaswater_alarm,
	//				 info.freq_alarm,info.running_light,info.reset,info.gas_put,info.water_put2);
	m_ControlOut = info;
	ExecuteDML(strSql);
}
