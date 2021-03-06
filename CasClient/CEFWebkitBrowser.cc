#include "stdafx.h"
#include "CEFWebkitBrowser.h"

// WKEWebkitBrowser.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "CEFWebkitBrowser.h"
#include <strsafe.h>

#include <MMSystem.h>
#pragma comment(lib, "Winmm.lib")

#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "prettywriter.h"
#include "stringbuffer.h"


#define 	E_GOFORWORD_TIMER		100
#define		KUnloopTimer			2000
CEFWebkitBrowserWnd *CEFWebkitBrowserWnd::s_instance = NULL;
UINT g_Part = 0;
bool g_Saline = true;

void CEFWebkitBrowserWnd::startTimer(UINT &timer, UINT uDelay,WORD dwUser,UINT fuEvent)
{
	if (timer != 0)
	{
		killTimer(timer);
	}

	timer = timeSetEvent(uDelay, 1, (LPTIMECALLBACK)CEFWebkitBrowserWnd::onTimeFunc, dwUser, fuEvent);
}

void CEFWebkitBrowserWnd::killTimer(UINT &timer)
{
	UINT ret = timeKillEvent(timer);
	if (ret == 0)
	{
		timer = 0;
	}
}

//MMRESULT timeSetEvent（ UINT uDelay, UINT uResolution,  LPTIMECALLBACK lpTimeProc, WORD dwUser, UINT fuEvent ）
//其中: uDelay：以毫秒指定事件的周期。
//Uresolution：以毫秒指定延时的精度，数值越小定时器事件分辨率越高。缺省值为1ms。
//LpTimeProc：指向一个回调函数。
//DwUser：存放用户提供的回调数据。
//FuEvent：指定定时器事件类型：TIME_ONESHOT：uDelay毫秒后只产生一次事件 TIME_PERIODIC ：每隔uDelay毫秒周期性地产生事件。
void WINAPI CEFWebkitBrowserWnd::onTimeFunc(UINT wTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2)
{
	if (dwUser == ETIME_ListBill)
	{
		wTimerID = 1;
		CEFWebkitBrowserWnd::s_instance->m_bCanCloseExe;
	} 
	else if (dwUser == ETIME_WaterSpit)
	{
		wTimerID = 2;
	}
	else if (dwUser == ETIME_Unloop)
	{
		CEFWebkitBrowserWnd::s_instance->unloopProcess();
	}
	else if (dwUser == ETIME_preLoop)
	{
		CEFWebkitBrowserWnd::s_instance->preLoop();
	}
	else if (dwUser == ETIME_Loop)
	{
		CEFWebkitBrowserWnd::s_instance->loopProcess();
	}
}

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

	m_timerCyStatus = 0;
	m_timerCollect = 0;
	m_timerUnloop = 0;
	m_timerNextBilling = 0;
	//水洗完定时
	m_timerAfterWater = 0;
	//放水时间定时
	m_timerAfterPutWater = 0;
	//放气时间
	m_timerAfterPutGas = 0;
	//料之间间隔
	m_timerCYSpit = 0;
	//水洗间隔
	m_timerWaterSpit = 0;
	m_timerListBill = 0;
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
	if (msg.sType == _T("windowinit"))
	{
		OnInitComplate();
	}	
	//else if (msg.sType == DUI_MSGTYPE_CLICK)
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


	__super::Notify(msg);
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

	getPart();

	if (m_pWKEWebkitCtrl)
	{
		//string sFile = CPaintManagerUI::GetResourcePath() + _T("dist\\index.html");
		//m_pWKEWebkitCtrl->NewPage(sFile.c_str());

		m_pWKEWebkitCtrl->NewPage(m_webHost);
	}

	//m_timerCyStatus = timeSetEvent(1000,1,(LPTIMECALLBACK)CEFWebkitBrowserWnd::onTimeFunc, 1, TIME_PERIODIC);
	startTimer(m_timerCyStatus,1000,ETIME_ListBill,TIME_PERIODIC);
	killTimer(m_timerCyStatus);

	
	getDBOInfo();

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
		GetPrivateProfileString(L"Database", L"Web", L"", wchTmp, MAX_PATH, wchPath);
		m_webHost = wchTmp;
		GetPrivateProfileString(L"Database", L"Saline", L"", wchTmp, MAX_PATH, wchPath);
		g_Saline = _wtoi(wchTmp)==1?true:false;
	}
}

SDyeControl CEFWebkitBrowserWnd::getDyeControlByNum(const string & dyenum)
{
	map<string, SDyeControl>::iterator it = m_mapDyeControl.find(dyenum);
	if (it != m_mapDyeControl.end())
	{
		return it->second;
	}

	return SDyeControl();
}

SVatsInfo CEFWebkitBrowserWnd::getVatsInfoByNum(const string & num)
{
	map<string, SVatsInfo>::iterator it = m_mapVatsInfo.find(num);
	if (it != m_mapVatsInfo.end())
	{
		return it->second;
	}

	return SVatsInfo();
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

string base64_encode(BYTE const* buf, unsigned int bufLen) {
	string ret;
	int i = 0;
	int j = 0;
	BYTE char_array_3[3];
	BYTE char_array_4[4];
	string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	while (bufLen--) {
		char_array_3[i++] = *(buf++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';
	}

	return ret;
}

string string_To_UTF8(const string & str) 
{ 
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0); 

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	ZeroMemory(pwBuf, nwLen * 2 + 2); 

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen); 

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL); 

	char * pBuf = new char[nLen + 1]; 
	ZeroMemory(pBuf, nLen + 1); 

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL); 

	string retStr(pBuf); 

	delete []pwBuf; 
	delete []pBuf; 

	pwBuf = NULL; 
	pBuf  = NULL; 

	return retStr; 
}

void CEFWebkitBrowserWnd::setJson(string &cmd, map<string,string> &info)
{
	//生成Json串
	rapidjson::Document setObj;
	rapidjson::Document::AllocatorType &allocator = setObj.GetAllocator();
	rapidjson::Value key;
	rapidjson::Value value;
	setObj.SetObject();

	for (map<string,string>::iterator it=info.begin(); it!=info.end(); ++it)
	{
		setObj.AddMember(key.SetString(it->first.c_str(), allocator), value.SetString(it->second.c_str(), allocator), allocator);
	}

	//生成字符串
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	setObj.Accept(writer);

	string strJson = buffer.GetString();
	cpp2web(cmd, strJson);

}

void CEFWebkitBrowserWnd::cpp2web(string &cmd, string &value)
{
	string strJson = string_To_UTF8(value);
	strJson = "window."+ cmd + "({\"value\":\"" + base64_encode((const BYTE*)strJson.c_str(), strJson.length()) + "\"});";
	CefString str;
	str.FromString(strJson);
	CefRefPtr<CefBrowser> pBrower = m_pWKEWebkitCtrl->GetBrowserByID(1);
	if (pBrower)
	{
		pBrower->GetMainFrame().get()->ExecuteJavaScript(str, pBrower->GetMainFrame().get()->GetURL(), 0);
	}
}

void CEFWebkitBrowserWnd::startWeight()
{
	m_emStep = READY_WEIGHT;
	m_bBefore = true;
	killTimer(m_timerDelayClose);
	m_bDelayClose = false;
	SDyeControl info = getDyeControlByNum(m_strDyeNum);
	SVatsInfo vats = getVatsInfoByNum(m_strVatsNum);
	SControlIn input = getControlIn();
	SControlOut output = getControlOut();
	m_strPortAlarm = output.normal_alarm;
	m_strWGAlarm = output.gaswater_alarm;
	m_strFreqAlarm = output.freq_alarm;
	//m_fBefore = info.beforetime.toFloat();
	//m_fWater = vats.water_dosage.toFloat();
	//m_nAfterWater = vats.after_time.toUInt();
	//if (info.dyenum.isEmpty())
	//{
	//	setExcuseInfo("助剂端口信息读取错误！", true);
	//	return;
	//}

	//if (vats.num.isEmpty())
	//{
	//	setExcuseInfo("染缸信息读取错误！", true);
	//	return;
	//}


	//if (!CasFrame::instance()->getPLC())
	//{
	//	setExcuseInfo("PLC端口打开失败！", true);
	//	return;
	//}

	//quint32 step(0);
	//m_Read.from = SReadingStatus::FROM_STARTWEIGHT;

	//SBaseConfig base = CasFrame::instance()->getBaseConfig();

	//if (m_bNormalFirst)
	//{		
	//	if (!setSta(SReadingStatus::WRType, step, true, false,
	//		QString("运行灯"), output.running_light,
	//		output.running_light, m_strPortAlarm))
	//	{
	//		return;
	//	}
	//	m_bNormalFirst = false;
	//}


	////if (setSta(SReadingStatus::RType, step, false, false,
	////	QString("%1料桶水位").arg(info.dyename),"",
	////	info.water_low,m_strPortAlarm))

	//if (info.store.toFloat() >= m_vecWeightInfo[m_nStep].dosage*1.5f)
	//{
	//	if (!setSta(SReadingStatus::WRType, step, true, false,
	//		QString("%1染缸").arg(vats.num),vats.valve,
	//		vats.valve_check, m_strPortAlarm))
	//	{
	//		return;
	//	}

	//	if (vats.control_num.toUInt() != 0)
	//	{
	//		if (!setSta(SReadingStatus::WRType, step, true, false,
	//			QString("%1分流阀门").arg(vats.control_num),vats.control,
	//			vats.control_check, m_strPortAlarm))
	//		{
	//			return;
	//		}
	//	}

	//	if (!setSta(SReadingStatus::RType, step, false, true,
	//		QString("变频端口"),"",
	//		input.freq_alarm, m_strFreqAlarm))
	//	{
	//		return;
	//	}

	//	if (!setSta(SReadingStatus::RFType, step, false, true,
	//		QString("流量计"),"",
	//		"", m_strFreqAlarm))
	//	{
	//		//重置PLC
	//		resetPLC();
	//		return;
	//	}
	//	else
	//	{
	//		m_fStart = m_fSetSta;
	//	}

	//	if (m_fTarget>3.0f)
	//	{
	//		if (!setSta(SReadingStatus::WRType, step, true, false,
	//			QString("%1料桶阀门").arg(info.dyename),info.cy,
	//			info.cy_check, m_strPortAlarm))
	//		{
	//			return;
	//		}
	//		if (!setSta(SReadingStatus::WRType, step, true, false,
	//			QString("进料阀门"), output.cy_come,
	//			input.cy_come, m_strPortAlarm))
	//		{
	//			return;
	//		}


	//		if (!setSta(SReadingStatus::WRType, step, true, false,
	//			QString("进料电机"), output.motor_come,
	//			input.motor_come, m_strPortAlarm))
	//		{
	//			return;
	//		}
	//		else
	//		{
	//			m_emStep = WEIGHTING;
	//			m_timerCollect.start();
	//		}

	//	}
	//	else if (m_fTarget<=3.0f)

	//	{
	//		if (!setSta(SReadingStatus::WRType, step, true, false,
	//			QString("进料电机"), output.motor_come,
	//			input.motor_come, m_strPortAlarm))
	//		{
	//			return;
	//		}

	//		if (!setSta(SReadingStatus::WRType, step, true, false,
	//			QString("进料阀门"), output.cy_come,
	//			input.cy_come, m_strPortAlarm))
	//		{
	//			return;
	//		}
	//		//延时打开料桶阀门
	//		if (m_Read.bWaiting1)
	//		{
	//			m_Read.step++;
	//			m_Read.bWaiting1 = false;
	//			m_Read.type = SReadingStatus::WTType;
	//			//m_Read.nWaitTime = 10;
	//			m_Read.nWaitTime = base.dengdai4_spit.toUInt();
	//			m_timerUnloop.start();
	//			return;
	//		}

	//		m_Read.bWaiting1 = true;

	//		if (!setSta(SReadingStatus::WRType, step, true, false,
	//			QString("%1料桶阀门").arg(info.dyename),info.cy,
	//			info.cy_check, m_strPortAlarm))
	//		{
	//			return;
	//		}

	//		else
	//		{
	//			m_emStep = WEIGHTING;
	//			m_timerCollect.start();
	//		}

	//	}


	//}
	//else
	//{
	//	setExcuseInfo(QString("%1当前量不足，请加料！").arg(info.dyename));
	//}
}

bool CEFWebkitBrowserWnd::setSta(const SReadingStatus::TYPEENUM type, UINT &step, bool bOpen, bool bRDuan, const string text, const string setP, const string getP, const string alarm)
{
	//step++;
	//QString strError;
	//if (type == SReadingStatus::RType)
	//{
	//	int ret = bRDuan ? PLC_PROT_DUAN : PLC_PROT_BIHE;
	//	int result = bRDuan ? PLC_PROT_DUAN : PLC_PROT_BIHE;

	//	if (m_Read.step <= step || !m_Read.bWaiting)
	//	{
	//		ret = CasFrame::instance()->getPLC()->PLCRead(getP.toStdString().c_str());
	//		m_Read.type = type;
	//		m_Read.getPort = getP;
	//		m_Read.status = result;			
	//		m_Read.step = step;
	//		m_Read.bWaiting = false;
	//		m_Read.alarmPort = alarm;
	//	}
	//	if (result == ret)
	//	{
	//		setExcuseInfo(QString("%1正常！").arg(text));
	//		return true;
	//	} 
	//	else
	//	{
	//		if (PLC_PROT_ERROR == ret)
	//		{
	//			strError = QString("%1通讯延时！").arg(text);
	//		}
	//		else
	//		{
	//			strError = QString("%1报警！").arg(text);
	//		}
	//		setExcuseInfo(strError, true);
	//		setErrorLog(strError);
	//		CasFrame::instance()->getPLC()->PLCSet(alarm.toStdString().c_str(), true);
	//		m_timerUnloop.start();
	//		return false;
	//	}

	//}		
	//else if(type == SReadingStatus::WRType)
	//{
	//	int ret = bOpen ? PLC_PROT_BIHE : PLC_PROT_DUAN;
	//	int result = bOpen ? PLC_PROT_BIHE : PLC_PROT_DUAN;
	//	if (m_Read.step <= step || !m_Read.bWaiting)
	//	{
	//		ret = CasFrame::instance()->setgetPLC(setP.toStdString().c_str(), bOpen, getP.toStdString().c_str());
	//		m_Read.type = type;
	//		m_Read.setPort = setP;
	//		m_Read.bOpen = bOpen;
	//		m_Read.getPort = getP;
	//		m_Read.status = result;
	//		m_Read.step = step;
	//		m_Read.bWaiting = false;
	//		m_Read.alarmPort = alarm;

	//	}
	//	if (result == ret)
	//	{
	//		setExcuseInfo(QString("%1%2成功！").arg(text).arg(bOpen?"打开":"关闭"));

	//		return true;
	//	} 
	//	else
	//	{
	//		if (PLC_PROT_ERROR == ret)
	//		{
	//			strError = QString("%1通讯延时！").arg(text);
	//		}
	//		else
	//		{
	//			strError = QString("%1%2失败！").arg(text).arg(bOpen?"打开":"关闭");

	//		}
	//		setExcuseInfo(strError, true);
	//		setErrorLog(strError);
	//		CasFrame::instance()->getPLC()->PLCSet(alarm.toStdString().c_str(), true);
	//		m_timerUnloop.start();
	//		return false;
	//	}
	//}
	//else if (type == SReadingStatus::RFType)
	//{
	//	float f(0.0f);
	//	bool bRet(true);
	//	//if (m_Read.step <= step || !m_Read.bWaiting)
	//	{
	//		bRet = CasFrame::instance()->getFlow()->GetFlowAllRateData(&f);
	//		m_Read.type = type;
	//		m_Read.step = step;
	//		m_Read.bWaiting = false;
	//		m_Read.alarmPort = alarm;
	//	}
	//	if(bRet)
	//	{
	//		m_fSetSta = f;
	//		setExcuseInfo(QString("%1流量计读取成功！").arg(text));
	//		return true;
	//	}
	//	else
	//	{
	//		setExcuseInfo(QString("%1流量计读取失败！").arg(text), true);
	//		setErrorLog(QString("%1流量计读取失败！").arg(text));
	//		//变频器报警
	//		CasFrame::instance()->getPLC()->PLCSet(alarm.toStdString().c_str(), true);
	//		return false;
	//	}
	//}			

	return false;
}

void CEFWebkitBrowserWnd::unloopProcess()
{
	////判断系统类型
	//if (g_Saline)
	//{
	//} 
	//else
	//{
	//}

	//read the para from zuheDB or zuheProgram
	m_ProcessStep = 0;
	//已经排好顺序
	for (vector<SZuhe>::iterator it=m_Additive.begin(); it!=m_Additive.end(); ++it)
	{
		if (it->is_done)
		{
			continue;
		}
		
		switch (it->code)
		{
		case PLC_RO:
			{
				int ret = m_AdditivePLC->PLCRead(it->para.c_str());
				if (PLC_PROT_DUAN != ret)
				{
					//开启定时器
					startTimer(m_timerUnloop,KUnloopTimer,ETIME_Unloop,TIME_ONESHOT);
					return ;
				}
			}
			break;
		case PLC_RC:
			{
				int ret = m_AdditivePLC->PLCRead(it->para.c_str());
				if (PLC_PROT_BIHE != ret)
				{
					startTimer(m_timerUnloop,KUnloopTimer,ETIME_Unloop,TIME_ONESHOT);
					return ;
				}
			}
			break;
		case PLC_WO:
			{
				bool ret = m_AdditivePLC->PLCSet(it->para.c_str(), true);
				if (!ret)
				{
					startTimer(m_timerUnloop,KUnloopTimer,ETIME_Unloop,TIME_ONESHOT);
					return;
				}
			}
			break;
		case PLC_WC:
			{
				bool ret = m_AdditivePLC->PLCSet(it->para.c_str(), false);
				if (!ret)
				{
					startTimer(m_timerUnloop,KUnloopTimer,ETIME_Unloop,TIME_ONESHOT);
					return;
				}
			}
			break;
		case FLW_R:
			{
				float f(0.0f);
				bool ret = m_AdditiveFlow->GetFlowAllRateData(&f);
				if (!ret)
				{
					startTimer(m_timerUnloop,KUnloopTimer,ETIME_Unloop,TIME_ONESHOT);
					return;
				}
			}
			break;
		case FKW_C:
			{
				float f(0.0f);
				bool ret = m_SMachineInfo.pFlow->GetFlowAllRateData(&f);
				if (!ret)
				{
					startTimer(m_timerUnloop,KUnloopTimer,ETIME_Unloop,TIME_ONESHOT);
					return;
				}
			}
			break;
		case SYS_WAIT:
			{
				//开启定时器
				startTimer(m_timerUnloop,atoi(it->para.c_str())*1000,ETIME_Unloop,TIME_ONESHOT);
				return;
			}
			break;
		}

		it->is_done = true;
		//成功之后
		if (it->next_is_loop)
		{
			startTimer(m_timerUnloop,100,ETIME_preLoop,TIME_ONESHOT);
			return;
		} 
		
	}

	
}

void CEFWebkitBrowserWnd::preLoop()
{
	m_ZuheLoop.clear();
	bool end(false);
	for (vector<SZuhe>::iterator it=m_Additive.begin(); it!=m_Additive.end(); ++it)
	{
		//没有执行且不是循环		
		if (!it->is_done && !it->is_loop)
		{
			startTimer(m_timerUnloop,100,ETIME_Loop,TIME_ONESHOT);
			break;
		}
		else if (!it->is_done && it->is_loop)
		{
			m_ZuheLoop.push_back(*it);
			it->is_done = true;
		}
	}

}

void CEFWebkitBrowserWnd::loopProcess()
{
	//获取循环表格参数

	for (vector<SZuhe>::iterator it=m_Additive.begin(); it!=m_Additive.end(); ++it)
	{
		if (it->is_done)
		{
			continue;
		}

		switch (it->code)
		{
		case PLC_RO:
			{
				int ret = m_AdditivePLC->PLCRead(it->para.c_str());
				if (PLC_PROT_DUAN != ret)
				{
					//开启定时器
					startTimer(m_timerUnloop,KUnloopTimer,ETIME_Unloop,TIME_ONESHOT);
					return ;
				}
			}
			break;
		case PLC_RC:
			{
				int ret = m_AdditivePLC->PLCRead(it->para.c_str());
				if (PLC_PROT_BIHE != ret)
				{
					startTimer(m_timerUnloop,KUnloopTimer,ETIME_Unloop,TIME_ONESHOT);
					return ;
				}
			}
			break;
		case PLC_WO:
			{
				bool ret = m_AdditivePLC->PLCSet(it->para.c_str(), true);
				if (!ret)
				{
					startTimer(m_timerUnloop,KUnloopTimer,ETIME_Unloop,TIME_ONESHOT);
					return;
				}
			}
			break;
		case PLC_WC:
			{
				bool ret = m_AdditivePLC->PLCSet(it->para.c_str(), false);
				if (!ret)
				{
					startTimer(m_timerUnloop,KUnloopTimer,ETIME_Unloop,TIME_ONESHOT);
					return;
				}
			}
			break;
		case FLW_R:
			{
				float f(0.0f);
				bool ret = m_AdditiveFlow->GetFlowAllRateData(&f);
				if (!ret)
				{
					startTimer(m_timerUnloop,KUnloopTimer,ETIME_Unloop,TIME_ONESHOT);
					return;
				}
			}
			break;
		case FKW_C:
			{
				float f(0.0f);
				bool ret = m_SMachineInfo.pFlow->GetFlowAllRateData(&f);
				if (!ret)
				{
					startTimer(m_timerUnloop,KUnloopTimer,ETIME_Unloop,TIME_ONESHOT);
					return;
				}
			}
			break;
		case SYS_WAIT:
			{
				//开启定时器
				startTimer(m_timerUnloop,atoi(it->para.c_str())*1000,ETIME_Unloop,TIME_ONESHOT);
				return;
			}
			break;
		}

		it->is_done = true;
		//成功之后
		if (it->next_is_loop)
		{
			startTimer(m_timerUnloop,100,ETIME_Loop,TIME_ONESHOT);
			return;
		} 

	}

}
