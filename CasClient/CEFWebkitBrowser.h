#pragma once

#include "resource.h"
#include "CEFWebkit.h"
#include <map>

//控制部分全在这里
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
extern bool g_Saline;

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

struct SWeightInfo
{
	UINT index;
	string processnum;
	string processname;
	string dyenum;
	string dyename;
	string id;
	string manynum;
	float dosage;
	string unit;
	string waterLevel;
	vector<string> line;
	//0-未称 1-正在称 2-已称完
	UINT type;
	SWeightInfo():type(0),dosage(0.0f),id(""){}
};

struct SReadingStatus
{
	enum TYPEENUM
	{
		NType = 0,
		RType,
		WType,
		WRType,
		RWType,
		RFType,
		WTType,//延时逻辑
	};
	TYPEENUM type;
	bool bWaiting;//是否等待模式
	bool bWaiting1;//是否等待模式1
	bool bOpen;//W use
	string getPort;
	string setPort;
	string alarmPort;
	enum FROMENUM
	{
		FROM_STARTWEIGHT = 0,
		FROM_WEIGHTCOLLECT,
		FROM_WASHWEIGHT,
		FROM_WASHCOLLECT,
		FROM_WASHAFTER,
		FROM_PUTWATER,
		FROM_PUTGAS,
	};
	FROMENUM from;//在哪个地方调用的
	UINT step;//具体哪一步调用的
	int status;
	UINT nWaitTime;
	SReadingStatus():type(NType),bOpen(true),getPort(""),setPort(""),alarmPort(""),from(FROM_STARTWEIGHT),step(0),status(-1),bWaiting(false),nWaitTime(0){}

	void clear()
	{
		type = NType;
		bOpen = true;
		bWaiting = false;
		getPort = "";
		setPort = "";
		bWaiting1 = true;
	}
};

enum EmSetp
{
	NO_WEIGHT,
	READY_WEIGHT,
	WEIGHTING,
	FINISH_WEIGHT,
	WASHING,//水洗
	WASHEND,//水洗完成
};

enum EmTime
{
	//定时器
	ETIME_CyStatus,
	ETIME_Collect,
	ETIME_Unloop,
	ETIME_preLoop,
	ETIME_Loop,
	ETIME_NextBilling,
	//水洗完定时
	ETIME_AfterWater,
	//放水时间定时
	ETIME_AfterPutWater,
	//放气时间
	ETIME_AfterPutGas,
	//料之间间隔
	ETIME_CYSpit,
	//水洗间隔
	ETIME_WaterSpit,

	ETIME_ListBill,
};

enum EmZType {
	PLC_RO = 1,//读取 端口状态开
	PLC_RC,//读取 端口状态关
	PLC_WO,//写入 端口打开
	PLC_WC,//写入 端口关闭
	FLW_R,//读取 流量计状态
	FKW_C,//流量计计数
	SYS_WAIT,//等待 单位毫秒

};

struct SZuhe {
	
	int system;
	bool is_start;
	EmZType code;
	string para_name;
	string para;
	string para_unit;
	bool is_loop;	
	string loop_table;
	int next;
	int next_is_loop;
	bool is_end;
	bool is_done;

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
	void setJson(string &cmd, map<string,string> &info);
	void cpp2web(string &cmd, string &value);

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

//流程控制
public:
	void startWeight();
	bool setSta(const SReadingStatus::TYPEENUM type,
		UINT &step,
		bool bOpen,
		bool bRDuan,
		const string text,
		const string setP,
		const string getP,
		const string alarm);

	SDyeControl getDyeControlByNum(const string & dyenum);
	SVatsInfo getVatsInfoByNum(const string & num);

	void unloopProcess();
	void preLoop();
	void loopProcess();
	
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

	//步骤
	EmSetp m_emStep;


	//定时器
	UINT	m_timerCyStatus;
	UINT	m_timerCollect;
	UINT	m_timerUnloop;
	UINT	m_timerLoop;
	UINT	m_timerNextBilling;
	//水洗完定时
	UINT	m_timerAfterWater;
	//放水时间定时
	UINT	m_timerAfterPutWater;
	//放气时间
	UINT	m_timerAfterPutGas;
	//料之间间隔
	UINT	m_timerCYSpit;
	//水洗间隔
	UINT	m_timerWaterSpit;
	UINT	m_timerListBill;
	UINT	m_timerDelayClose;

	vector<SWeightInfo> m_vecWeightInfo;
	//开始重量
	float m_fStart;
	//目标重量
	float m_fTarget;
	// 剩余重量
	float m_fRemain;
	// 当前已称重量
	float m_fCurrent;
	//水洗量 根据染缸来
	float m_fWater;
	//提前量
	float m_fBefore;
	//setSta
	float m_fSetSta;
	bool m_bBefore;
	//水洗后时间 按照染缸来
	UINT m_nAfterWater;
	string m_strPos, m_strPlcPos;
	string m_strManyNum;
	string m_strVatsNum;
	string m_strDyeNum;
	string m_strMany_RunningNumber;
	string m_strBill_RunningNumber;

	string m_strclientname;
	string m_strprojectname;
	string m_strcolorname;
	string m_strclientcolor;
	string m_strkind;
	string m_strPortAlarm;
	string m_strWGAlarm;
	string m_strFreqAlarm;
	bool m_bAuto;
	//超级全自动，不用确认开始
	bool m_bSuperAuto;
	bool m_bManualFirst;
	bool m_bManual;
	bool m_bManualModify;
	bool m_bNormalFirst;
	bool m_bSetRun;


	int m_nStep;//到哪个料
	UINT m_nDetails;//详情
	SReadingStatus m_Read;
	map<UINT,	void (CEFWebkitBrowserWnd::*)()> funcmap;
	
	bool m_bDelayClose;

	vector<SZuhe> m_Additive;
	vector<SZuhe> m_Saline;
	vector<SZuhe> m_ZuheLoop;
	int m_ProcessStep;

	CasT *m_AdditivePLC;	
	CasT* m_AdditiveFlow;


private:
	void startTimer(UINT &timer, UINT uDelay,WORD dwUser,UINT fuEvent);
	void killTimer(UINT &timer);

};
