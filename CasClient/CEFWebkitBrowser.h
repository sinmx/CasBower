#pragma once

#include "resource.h"
#include "CEFWebkit.h"
#include <map>

//���Ʋ���ȫ������
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
	//ˮλ
	string water_high;
	string water_low;
	//��Ͱ����
	string cy_num;
	string cy;
	string cy_check;
	string unit;
	string speed;
	//��ǰ��
	string beforetime;
	string store;
	SDyeControl():type(0){}
};

struct SControlOut
{
	string water_come;
	string water_put;
	string cy_come;
	//��������
	string gas_come;
	string motor_come;
	string normal_alarm;
	string gaswater_alarm;
	string freq_alarm;
	string running_light;
	//��������
	string gas_put;
	string reset;
	//��ˮ����
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
	//Ⱦ��
	string valve;
	string valve_check;
	//��������
	string control_num;
	string control;
	string control_check;
	//ˮϴ��
	string water_dosage;
	//ˮϴ��ʱ
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
	//0-δ�� 1-���ڳ� 2-�ѳ���
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
		WTType,//��ʱ�߼�
	};
	TYPEENUM type;
	bool bWaiting;//�Ƿ�ȴ�ģʽ
	bool bWaiting1;//�Ƿ�ȴ�ģʽ1
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
	FROMENUM from;//���ĸ��ط����õ�
	UINT step;//������һ�����õ�
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
	WASHING,//ˮϴ
	WASHEND,//ˮϴ���
};

enum EmTime
{
	//��ʱ��
	ETIME_CyStatus,
	ETIME_Collect,
	ETIME_Unloop,
	ETIME_preLoop,
	ETIME_Loop,
	ETIME_NextBilling,
	//ˮϴ�궨ʱ
	ETIME_AfterWater,
	//��ˮʱ�䶨ʱ
	ETIME_AfterPutWater,
	//����ʱ��
	ETIME_AfterPutGas,
	//��֮����
	ETIME_CYSpit,
	//ˮϴ���
	ETIME_WaterSpit,

	ETIME_ListBill,
};

enum EmZType {
	PLC_RO = 1,//��ȡ �˿�״̬��
	PLC_RC,//��ȡ �˿�״̬��
	PLC_WO,//д�� �˿ڴ�
	PLC_WC,//д�� �˿ڹر�
	FLW_R,//��ȡ ������״̬
	FKW_C,//�����Ƽ���
	SYS_WAIT,//�ȴ� ��λ����

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
	//�����������ı�
	void OnTitleChanged(int nWebBrowserID,const CefString str);
	//�����ҳ�����ɹ�
	void OnAfterCreate(int nWebBrowserID);
	//�����ҳ�ر�
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
//ҵ���߼�
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

//���̿���
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

	//��ǰ�Ĳ���
	float m_flowIn;
	float m_flowOut;

	//����
	EmSetp m_emStep;


	//��ʱ��
	UINT	m_timerCyStatus;
	UINT	m_timerCollect;
	UINT	m_timerUnloop;
	UINT	m_timerLoop;
	UINT	m_timerNextBilling;
	//ˮϴ�궨ʱ
	UINT	m_timerAfterWater;
	//��ˮʱ�䶨ʱ
	UINT	m_timerAfterPutWater;
	//����ʱ��
	UINT	m_timerAfterPutGas;
	//��֮����
	UINT	m_timerCYSpit;
	//ˮϴ���
	UINT	m_timerWaterSpit;
	UINT	m_timerListBill;
	UINT	m_timerDelayClose;

	vector<SWeightInfo> m_vecWeightInfo;
	//��ʼ����
	float m_fStart;
	//Ŀ������
	float m_fTarget;
	// ʣ������
	float m_fRemain;
	// ��ǰ�ѳ�����
	float m_fCurrent;
	//ˮϴ�� ����Ⱦ����
	float m_fWater;
	//��ǰ��
	float m_fBefore;
	//setSta
	float m_fSetSta;
	bool m_bBefore;
	//ˮϴ��ʱ�� ����Ⱦ����
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
	//����ȫ�Զ�������ȷ�Ͽ�ʼ
	bool m_bSuperAuto;
	bool m_bManualFirst;
	bool m_bManual;
	bool m_bManualModify;
	bool m_bNormalFirst;
	bool m_bSetRun;


	int m_nStep;//���ĸ���
	UINT m_nDetails;//����
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
