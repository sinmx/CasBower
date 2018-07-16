#pragma once

#include "resource.h"
#include "CEFWebkit.h"

struct SMachineInfo
{
	UINT plcPort, plcBps, flowPort, flowBps;
	CasT *pPLC;	
	CasT* pFlow;
	bool bUse;
	SMachineInfo():pPLC(NULL),pFlow(NULL),bUse(false){}
};

class CEFWebkitBrowserWnd : public WindowImplBase
{

public:
    CEFWebkitBrowserWnd();
    ~CEFWebkitBrowserWnd();


    virtual LPCTSTR GetWindowClassName() const      { return _T("CEFWebkitBrowserWnd"); }
    virtual CDuiString GetSkinFile()                { return _T("root.xml"); }
    virtual CDuiString GetSkinFolder()              { return _T("cas"); }
    virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void OnFinalMessage(HWND hWnd);
	virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual void OnClick(TNotifyUI& msg) override;
    virtual void InitWindow();
    virtual void Notify(TNotifyUI& msg);
    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	void OnInitComplate();
	//标题栏发生改变
	void OnTitleChanged(int nWebBrowserID,const CefString str);
	//浏览器页创建成功
	void OnAfterCreate(int nWebBrowserID);
	//浏览器页关闭
	void OnBrowserClose(int nBrowserID);

	void OnWebLoadEnd(WPARAM wparam,LPARAM lparam);
	void OnWebLoadStart(WPARAM wParam, LPARAM lParam);
	COptionUI* GetActiveOption() const;

	void SwitchUIState();

public:
    
    wstring m_strURL;
    wstring m_strTitle;
    CCEFWebkitUI* m_pWKEWebkitCtrl; 

public:
	static CEFWebkitBrowserWnd* instance(); 
	static void destroy();
	//ini
	void init();
	void resetInI();
	//db
	_ConnectionPtr &getConn() {return m_pConnection;}
	CDuiString getField(const _RecordsetPtr &pRecordset, const CDuiString strValue);
	DWORD OpenQuery(_RecordsetPtr &pRecordset, const CDuiString strSql);
	DWORD CloseQuery(_RecordsetPtr &pRecordset);
	DWORD ExecuteDML(const CDuiString strSql, int *const pnRowsInvolved=NULL);
	//com
	CasT* getPLC() {return m_SMachineInfo.pPLC;}
	CasT* getFlow() {return m_SMachineInfo.pFlow;}
	int setgetPLC(const char *pchSet, bool bSetOn, const char *pchGet);
	int getsetPLC(const char *pchGet, bool bGetOn, const char *pchSet, bool bSetOn);
private:		
	class COptionTag
	{
	public:
		int nID_;
		COptionTag::COptionTag(int nID) :nID_(nID)
		{

		};
	};

	static CEFWebkitBrowserWnd	*s_instance;
	_ConnectionPtr			m_pConnection;
	_variant_t m_vField;
	SMachineInfo m_SMachineInfo;

};
