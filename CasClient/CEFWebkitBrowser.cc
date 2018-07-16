#include "stdafx.h"
#include "CEFWebkitBrowser.h"

// WKEWebkitBrowser.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "CEFWebkitBrowser.h"
#include <strsafe.h>


#define 	E_GOFORWORD_TIMER  100
CEFWebkitBrowserWnd *CEFWebkitBrowserWnd::s_instance = NULL;

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

void CEFWebkitBrowserWnd::OnClick(TNotifyUI & msg)
{
	CDuiString strName = msg.pSender->GetName();

	if (_T("ui_btn_close") == strName)
	{
		if ((m_pWKEWebkitCtrl != NULL) && (!m_pWKEWebkitCtrl->IsClosed()))
		{
			m_pWKEWebkitCtrl->CloseAllPage();
		}

	}

	__super::OnClick(msg);

}


void CEFWebkitBrowserWnd::InitWindow()
{

}


void CEFWebkitBrowserWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("ui_btn_goback"))
		{
			COptionUI* pCurrentOpt = NULL;
			COptionTag* pTag = NULL;

			if (pCurrentOpt = GetActiveOption())
			{
				if (pTag = (COptionTag*)pCurrentOpt->GetTag())
				{

					m_pWKEWebkitCtrl->GoBack(pTag->nID_);
				}
			}
		}
		else if (msg.pSender->GetName() == _T("ui_btn_goforward"))
		{
			COptionUI* pCurrentOpt = NULL;
			COptionTag* pTag = NULL;

			if (pCurrentOpt = GetActiveOption())
			{
				if (pTag = (COptionTag*)pCurrentOpt->GetTag())
				{

					m_pWKEWebkitCtrl->GoForward(pTag->nID_);
				}
			}
		}
		else if (msg.pSender->GetName() == _T("ui_btn_refresh"))
		{
			COptionUI* pCurrentOpt = NULL;
			COptionTag* pTag = NULL;

			if (pCurrentOpt = GetActiveOption())
			{
				if (pTag = (COptionTag*)pCurrentOpt->GetTag())
				{
					m_pWKEWebkitCtrl->ReLoad(pTag->nID_);
				}
			}

		}
		else if (msg.pSender->GetName() == _T("ui_btn_home"))
		{
			m_pWKEWebkitCtrl->NewPage("http://web.yy.com/city_redpackge/task.html");
		}
		else if (msg.pSender->GetName() == _T("ui_btn_newpage"))
		{
			if (m_pWKEWebkitCtrl)
			{
				//	m_pWKEWebkitCtrl->NewPage(_T("about:black"));
				m_pWKEWebkitCtrl->NewPage(_T("about:blank"));
			}
		}


	}
	else if (msg.sType == _T("windowinit"))
	{
		OnInitComplate();
	}
	else if (msg.sType == DUI_MSGTYPE_TIMER)
	{
		if (msg.pSender == m_pWKEWebkitCtrl)
		{
			if (msg.wParam == E_GOFORWORD_TIMER)
			{
				SwitchUIState();
			}

		}

	}


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
	//db
	CNHSQLServerDBO::CloseDB(m_pConnection);
	if (0x00 != CNHSQLServerDBO::OpenDB(m_pConnection))
	{
		::MessageBoxW(GetForegroundWindow(), L"数据库打开失败!", TIP_INFO, MB_OK);
		return;
	}

	if (m_pWKEWebkitCtrl)
	{
		//CDuiString sFile = CPaintManagerUI::GetResourcePath() + _T("dist\\index.html");
		//m_pWKEWebkitCtrl->NewPage(sFile.GetData());

		m_pWKEWebkitCtrl->NewPage(_T("http://localhost:8080/#/login"));
	}

	//GetPaintManager()->SetTimer(m_pWKEWebkitCtrl, E_GOFORWORD_TIMER, 1000);
}

void CEFWebkitBrowserWnd::resetInI()
{

}

DWORD CEFWebkitBrowserWnd::OpenQuery(_RecordsetPtr &pRecordset, const CDuiString strSql)
{
	return CNHSQLServerDBO::OpenQuery(m_pConnection, pRecordset, strSql.GetData());
}

DWORD CEFWebkitBrowserWnd::CloseQuery(_RecordsetPtr &pRecordset)
{
	return CNHSQLServerDBO::CloseQuery(pRecordset);
}

DWORD CEFWebkitBrowserWnd::ExecuteDML(const CDuiString strSql, int *const pnRowsInvolved)
{
	return CNHSQLServerDBO::ExecuteDML(m_pConnection, strSql.GetData(), pnRowsInvolved);
}

CDuiString CEFWebkitBrowserWnd::getField(const _RecordsetPtr &pRecordset, const CDuiString strValue)
{
	CDuiString strResult(L"");
	if (!pRecordset->adoEOF)
	{
		m_vField.ChangeType(VT_NULL);
		CNHSQLServerDBO::GetFieldValue(pRecordset, m_vField, strValue.GetData());
		switch (m_vField.vt)
		{
		case VT_I4:
		case VT_I2:
			{
				strResult.Format(L"%d", (int)m_vField);
			}
			break;
		case VT_BSTR:
			{
				strResult.Format(L"%s", (const wchar_t*)(_bstr_t)m_vField);
			}
			break;
		case VT_DATE:
			{
				strResult.Format(L"%s", (const wchar_t*)(_bstr_t)m_vField);
			}
			break;
		case VT_DECIMAL:
			{
				strResult.Format(L"%s", (const wchar_t*)(_bstr_t)m_vField);
			}
			break;
		case VT_BOOL:
			{
				strResult.Format(L"%s", (bool)m_vField);
			}
			break;
		}
	}
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
