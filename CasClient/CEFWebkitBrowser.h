#pragma once

#include "resource.h"
#include "CEFWebkit.h"
#include <map>
using namespace std;

struct SBillingInfo
{
	string runningnumber;
	string vats_num;
	string manynum;
	string done;
	string clientname;
	string projectname;
	string colorname;
	string clientcolor;
	string kind;
	string createtime;
	UINT cur_client;
};

struct SDyeControl
{
	UINT type;
	string processnum;
	string processname;
	string kindnum;
	string kindname;
	string dyenum;
	string dyename;
	//水位
	string water_high;
	string water_low;
	//料桶阀门
	string cy_num;
	string cy;
	string cy_check;
	string unit;
	string speed;
	//提前量
	string beforetime;
	string store;
	SDyeControl():type(0){}
};

struct SControlOut
{
	string water_come;
	string water_put;
	string cy_come;
	//进气阀门
	string gas_come;
	string motor_come;
	string normal_alarm;
	string gaswater_alarm;
	string freq_alarm;
	string running_light;
	//放气阀门
	string gas_put;
	string reset;
	//放水阀门
	string water_put2;
};

struct SControlIn
{
	string water_come;
	string water_put;
	string cy_come;
	string gas_come;
	string water_alarm;
	string gas_alarm;
	string freq_alarm;

	string gas_put;
	string water_put2;
	string motor_come;
};

extern UINT g_Part;
extern bool g_test;

struct SBaseConfig
{
	UINT machine;
	string id;
	string kind1;
	string kind2;
	string kind3;
	string water_time;
	string gas_time;
	string cy_spit;
	string water_spit;
	string dengdai_spit;
	string dengdai2_spit;
	string dengdai3_spit;
	string dengdai4_spit;
	bool super_auto;
	string man;
	SBaseConfig():super_auto(false),machine(0){}
};

struct SVatsInfo
{
	string num;
	//染缸
	string valve;
	string valve_check;
	//分流阀门
	string control_num;
	string control;
	string control_check;
	//水洗量
	string water_dosage;
	//水洗后定时
	string after_time;
	UINT machine;
};

struct SMachineInfo
{
	UINT plcPort, plcBps, flowPort, flowBps;
	CasT *pPLC;	
	CasT* pFlow;
	bool bUse;
	SMachineInfo():pPLC(NULL),pFlow(NULL),bUse(false){}
};

extern UINT g_Part;
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

private:		
	class COptionTag
	{
	public:
		int nID_;
		COptionTag::COptionTag(int nID) :nID_(nID)
		{

		};
	};

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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//业务逻辑
public:
	static CEFWebkitBrowserWnd* instance(); 
	static void destroy();
	//init
	void init();
	void getPart();
	void getDBOInfo();
	//ini
	SBaseConfig getBaseConfig() {return m_BaseConfig;}
	void setBaseConfig(SBaseConfig &value);
	//db
	_ConnectionPtr &getConn() {return m_pConnection;}
	string getField(const _RecordsetPtr &pRecordset, const string strValue);
	DWORD OpenQuery(_RecordsetPtr &pRecordset, const string strSql);
	DWORD CloseQuery(_RecordsetPtr &pRecordset);
	DWORD ExecuteDML(const string strSql, int *const pnRowsInvolved=NULL);
	//com
	CasT* getPLC() {return m_SMachineInfo.pPLC;}
	CasT* getFlow() {return m_SMachineInfo.pFlow;}
	int setgetPLC(const char *pchSet, bool bSetOn, const char *pchGet);
	int getsetPLC(const char *pchGet, bool bGetOn, const char *pchSet, bool bSetOn);

	//control
	SControlIn getControlIn() {return m_ControlIn;}
	void setControlIn(const SControlIn & info);
	SControlOut getControlOut() {return m_ControlOut;}
	void setControlOut(const SControlOut & info);

	void setCanClose(bool b) {m_bCanCloseExe = b;}
	bool getCanClose() {return m_bCanCloseExe;}

	void static WINAPI onTimeFunc(UINT wTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2);
	
private:		
	bool m_bCanCloseExe;
	wstring m_webHost;

	static CEFWebkitBrowserWnd	*s_instance;
	_ConnectionPtr			m_pConnection;
	_variant_t m_vField;
	//dye plc
	map<string, SDyeControl> m_mapDyeControl;
	map<string, SVatsInfo> m_mapVatsInfo;
	SControlIn m_ControlIn;
	SControlOut m_ControlOut;
	SBillingInfo m_BillingInfo;
	SBaseConfig m_BaseConfig;
	SMachineInfo m_SMachineInfo;
	bool m_bRun;

	//当前的参数
	float m_flowIn;
	float m_flowOut;

};
