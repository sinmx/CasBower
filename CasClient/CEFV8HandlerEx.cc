#include "stdafx.h"
#include "CEFV8HandlerEx.h"
#include <strsafe.h>
#include "CEFWebkitBrowser.h"

#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "prettywriter.h"
#include "stringbuffer.h"
#include <Winsock2.h>
#include <time.h>
#pragma comment(lib,"ws2_32.lib")

CCEFV8HandlerEx::CCEFV8HandlerEx()
{

}

CCEFV8HandlerEx::~CCEFV8HandlerEx()
{

}

static const string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static inline bool is_base64(BYTE c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

string base64_encode(BYTE const* buf, unsigned int bufLen) {
	string ret;
	int i = 0;
	int j = 0;
	BYTE char_array_3[3];
	BYTE char_array_4[4];

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

vector<BYTE> base64_decode(string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	BYTE char_array_4[4], char_array_3[3];
	vector<BYTE> ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret.push_back(char_array_3[i]);
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
	}

	return ret;
}

std::string UTF8_To_string(const std::string & str) 
{ 
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0); 

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	memset(pwBuf, 0, nwLen * 2 + 2); 

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen); 

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL); 

	char * pBuf = new char[nLen + 1]; 
	memset(pBuf, 0, nLen + 1); 

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL); 

	std::string retStr = pBuf; 

	delete []pBuf; 
	delete []pwBuf; 

	pBuf = NULL; 
	pwBuf = NULL; 

	return retStr; 
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

string base64_utf8_decode(const CefString &para)
{
	vector<BYTE> ar = base64_decode(para);
	string strDoc(ar.begin(), ar.end());
	strDoc = UTF8_To_string(strDoc);
	return strDoc;
}

char * UnicodeToANSI(const wchar_t *str)
{
	char* result(NULL);
	int textlen(0);
	textlen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	result =(char *)malloc((textlen+1)*sizeof(char));
	memset(result, 0, sizeof(char)*(textlen+1));
	WideCharToMultiByte(CP_ACP, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}

string getIPAddress()
{
	WSADATA wsaData;  
	char name[155];             
	PHOSTENT hostinfo;   
	std::string ip;

	if (WSAStartup(MAKEWORD(2,0), &wsaData) == 0)   
	{  
		if(gethostname(name, sizeof(name)) == 0)   
		{  
			if((hostinfo = gethostbyname(name)) != NULL)   
			{ 
				ip = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list);                   
			}   
		}   
		WSACleanup();        
	}  

	return ip;
}

// 在JS里面调用 cef.onCallCpp("abc", "def");
bool CCEFV8HandlerEx::Execute(const CefString& name  /*JavaScript调用的C++方法名字*/
							  , CefRefPtr<CefV8Value> object /*JavaScript调用者对象*/
							  , const CefV8ValueList& arguments /*JavaScript传递的参数*/
							  , CefRefPtr<CefV8Value>& retval /*返回给JS的值设置给这个对象*/
							  , CefString& exception/*通知异常信息给JavaScript*/)
{
	if (name == _T("onCallCpp") && arguments.size() == 2)
	{
		//参数1方法名
		//参数2 是一个json 具体要解析
		CefString strParam1 = arguments.at(0)->GetStringValue();
		CefString strParam2 = arguments.at(1)->GetStringValue();

		if (strParam1 == "getLogin")
		{			
			getLogin(strParam2);			
			CEFWebkitBrowserWnd::instance()->setCanClose(true);		
		} 
		else if (strParam1 == "getControlOut")
		{
			getControlOut();
		}
		else if (strParam1 == "getInfo")
		{
			getInfo();
		}
		else if (strParam1 == "setControlOut")
		{
			setControlOut(strParam2);
		}
		else if (strParam1 == "getSql")
		{
			getSql(strParam2);
		}
		else if (strParam1 == "setSql")
		{
			setSql(strParam2);
		}
		else if (strParam1 == "newBill")
		{
			newBill(strParam2);
		}
		else if (strParam1 == "modifyBill")
		{
			modifyBill(strParam2);
		}

		//TCHAR szBuffer[512];
		//StringCbPrintf(szBuffer, sizeof(szBuffer), _T("jsInvokeCPlusPlus(%s,%s)"), strParam1.c_str(), strParam2.c_str());
		//::MessageBox(GetForegroundWindow(), szBuffer, _T("Castle1111111"), MB_OK);
		//retval = CefV8Value::CreateInt(0);
		return true;

	}
	// Function does not exist.
	return false;
}

void CCEFV8HandlerEx::cpp2web(string &cmd, string &value)
{
	string strJson = string_To_UTF8(value);
	strJson = "window."+ cmd + "({\"value\":\"" + base64_encode((const BYTE*)strJson.c_str(), strJson.length()) + "\"});";
	//strJson = "window.getLoginRsp({\"abc\":\"eyJlcnJvciI6IiIsInJvbGUiOjB9\"});";
	//执行js测试
	//CefString str(L"window.getLoginRsp(22);");
	CefString str;
	str.FromString(strJson);

	CefRefPtr<CefBrowser> pBrower = CEFWebkitBrowserWnd::instance()->m_pWKEWebkitCtrl->GetBrowserByID(1);
	if (pBrower)
	{
		pBrower->GetMainFrame().get()->ExecuteJavaScript(str, pBrower->GetMainFrame().get()->GetURL(), 0);
	}
}

void CCEFV8HandlerEx::getSql(const CefString &sql)
{
	string strDoc = base64_utf8_decode(sql);
	_RecordsetPtr pSet;

	rapidjson::Document	getObj;
	getObj.Parse(strDoc.c_str());

	if (getObj["cmd"].IsString() && getObj["sql"].IsString())
	{
		string strSql,strCmd;
		strCmd = getObj["cmd"].GetString();
		strSql = getObj["sql"].GetString();

		//strSql = "select * from UserInfo";
		int count(0);
		if (0x00 == CEFWebkitBrowserWnd::instance()->OpenQuery(pSet, strSql))
		{
			if (!pSet->adoEOF)
			{
				_bstr_t key;
				_variant_t value;
				//生成Json串
				rapidjson::Document setObj;    //生成一个dom元素Document
				rapidjson::Document::AllocatorType &allocator = setObj.GetAllocator(); //获取分配器
				rapidjson::Value val;
				setObj.SetObject();    //将当前的Document设置为一个object，也就是说，整个Document是一个Object类型的dom元素

				for (int i=0; i<pSet->Fields->Count; i++)
				{
					pSet->MoveFirst();
					key = pSet->Fields->GetItem(long(i))->GetName();
					//生成array
					rapidjson::Value letterArray(rapidjson::kArrayType);//创建一个Array类型的元素
					while (!pSet->adoEOF)
					{
						value.Clear();
						value = pSet->Fields->GetItem(long(i))->GetValue();
						if (value.vt == VT_NULL || value.vt>100)
						{
							letterArray.PushBack("", allocator);
						}
						else
						{
							letterArray.PushBack(val.SetString((const char*)(_bstr_t)value,allocator), allocator);
						}

						pSet->MoveNext();
					}
					setObj.AddMember(val.SetString(UnicodeToANSI(key.GetBSTR()),allocator), letterArray, allocator);
				}

				setObj.AddMember("cas_cmd", val.SetString(strCmd.c_str(), allocator), allocator);

				//生成字符串
				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				setObj.Accept(writer);

				string strJson = buffer.GetString();
				cpp2web(string("getSql"), strJson);
			}

		}
	}
}

void CCEFV8HandlerEx::setSql(const CefString &sql)
{
	string strSql = base64_utf8_decode(sql);
	CEFWebkitBrowserWnd::instance()->ExecuteDML(strSql);
}

void CCEFV8HandlerEx::newBill(const CefString &para)
{
	time_t rawtime ; 
	struct tm * timeinfo; 
	char s[256] = {0}; 
	char s1[256] = {0}; 
	time ( &rawtime ); 

	timeinfo = localtime ( &rawtime ); 
	time_t tick = mktime(timeinfo);
	strftime(s, sizeof(s), "%Y%m%d%H%M%S", timeinfo);
	strftime(s1, sizeof(s1), "%Y-%m-%d %H:%M:%S", timeinfo);
	char Temp[1024] = {0};
	string ip = getIPAddress();
	sprintf(Temp, "%s-%s", s, ip.c_str());
	string strRunningNumber = Temp;
	ZeroMemory(Temp, sizeof(Temp));
	string strManyNum = strRunningNumber.substr(2,4);
	
	sprintf(Temp, "select * from Many where manynum like '%s%%' and depart='%d'", strManyNum.c_str(), g_Part);
	string strSql = Temp;
	string strTemp;
	_RecordsetPtr pSet;
	if (0x00 == CEFWebkitBrowserWnd::instance()->OpenQuery(pSet, strSql))
	{
		if (pSet->adoEOF)
		{
			CEFWebkitBrowserWnd::instance()->CloseQuery(pSet);
			strManyNum += "00001";
		}
		else
		{
			CEFWebkitBrowserWnd::instance()->CloseQuery(pSet);
			ZeroMemory(Temp, sizeof(Temp));
			sprintf(Temp, "select max(manynum) maxnum from many where manynum like '%s%%' and depart='%d'", strManyNum.c_str(), g_Part);
			strSql = Temp;
			//m_strSQL.Format(L"select max(manynum) maxnum from many where manynum like '%%%s%%' and depart='%s'", m_strManyNum, m_strBigDepart);
			if (0x00 == CEFWebkitBrowserWnd::instance()->OpenQuery(pSet, strSql))
			{
				strTemp = CEFWebkitBrowserWnd::instance()->getField(pSet, "maxnum");
				sprintf(Temp, "%05d", atoi(strTemp.substr(4,5).c_str())+1);
				strManyNum += Temp;
			}
			CEFWebkitBrowserWnd::instance()->CloseQuery(pSet);
		}
	}
	CEFWebkitBrowserWnd::instance()->CloseQuery(pSet);
	ZeroMemory(Temp, sizeof(Temp));

	string strDoc = base64_utf8_decode(para);
	rapidjson::Document	getObj;
	getObj.Parse(strDoc.c_str());



	sprintf(Temp, "insert into Many(runningnumber, manynum, dater, scan, done, depart,tank,hltank) values('%s','%s','%s','0','0','%d','%s','%s')"
		, strRunningNumber.c_str(), strManyNum.c_str(), s1, g_Part, getObj["many"]["tank"].GetString(), getObj["many"]["hltank"].GetString());
	strSql = Temp;
	CEFWebkitBrowserWnd::instance()->ExecuteDML(strSql);

	//manyone


	UINT count = getObj["manyone"].Size();

	for (UINT i=0; i<count; i++)
	{
		if (getObj["manyone"][i]["dyenum"].IsString()
			&&getObj["manyone"][i]["dosage"].IsString())
		{
			ZeroMemory(Temp, sizeof(Temp));
			sprintf(Temp, "insert into ManyOne(processnum, dyenum, dosage, manynum, done, depart) values('%03d','%s',%.3f,'%s','0','%d')"
				, i+1, getObj["manyone"][i]["dyenum"].GetString(), atof(getObj["manyone"][i]["dosage"].GetString())
				, strRunningNumber.c_str(), g_Part);
			strSql = Temp;
			CEFWebkitBrowserWnd::instance()->ExecuteDML(strSql);

		}
	}

}

void CCEFV8HandlerEx::modifyBill(const CefString &para)
{
	char Temp[1024] = {0};
	string strSql;
	string strTemp;

	string strDoc = base64_utf8_decode(para);
	rapidjson::Document	getObj;
	getObj.Parse(strDoc.c_str());

	sprintf(Temp, "update Many set tank='%s', hltank='%s' where runningnumber='%s'"
		,getObj["many"]["tank"].GetString(), getObj["many"]["hltank"].GetString(),getObj["many"]["runningnumber"].GetString());
	strSql = Temp;
	CEFWebkitBrowserWnd::instance()->ExecuteDML(strSql);

	//manyone
	ZeroMemory(Temp, sizeof(Temp));
	sprintf(Temp, "delete from ManyOne where manynum='%s'",getObj["many"]["runningnumber"].GetString());
	strSql = Temp;
	CEFWebkitBrowserWnd::instance()->ExecuteDML(strSql);


	UINT count = getObj["manyone"].Size();

	for (UINT i=0; i<count; i++)
	{
		if (getObj["manyone"][i]["dyenum"].IsString()
			&&getObj["manyone"][i]["dosage"].IsString())
		{
			ZeroMemory(Temp, sizeof(Temp));
			sprintf(Temp, "insert into ManyOne(processnum, dyenum, dosage, manynum, done, depart) values('%03d','%s',%.3f,'%s','0','%d')"
				, i+1, getObj["manyone"][i]["dyenum"].GetString(), atof(getObj["manyone"][i]["dosage"].GetString())
				, getObj["many"]["runningnumber"].GetString(), g_Part);
			strSql = Temp;
			CEFWebkitBrowserWnd::instance()->ExecuteDML(strSql);

		}
	}
}

void CCEFV8HandlerEx::getLogin(const CefString &para)
{
	string strDoc = base64_utf8_decode(para);

	rapidjson::Document	getObj;
	getObj.Parse(strDoc.c_str());

	if (getObj["name"].IsString() && getObj["password"].IsString())
	{
		string strSql,strName,strPwd;
		strName = getObj["name"].GetString();
		strPwd = getObj["password"].GetString();

		//生成Json串
		rapidjson::Document setObj;    //生成一个dom元素Document
		rapidjson::Document::AllocatorType &allocator = setObj.GetAllocator(); //获取分配器
		setObj.SetObject();    //将当前的Document设置为一个object，也就是说，整个Document是一个Object类型的dom元素

		_RecordsetPtr pSet;

		strSql = "select * from UserInfo where Name='"+strName+"'";
		if (0x00 == CEFWebkitBrowserWnd::instance()->OpenQuery(pSet, strSql))
		{
			if(!pSet->adoEOF)
			{
				string dbPwd = CEFWebkitBrowserWnd::instance()->getField(pSet, "Password");
				if (dbPwd != strPwd)
				{
					CEFWebkitBrowserWnd::instance()->CloseQuery(pSet);
					setObj.AddMember("error", "密码错误！", allocator); 
					setObj.AddMember("role", -1, allocator);
				}
				else if ("0" == CEFWebkitBrowserWnd::instance()->getField(pSet, "Role"))
				{
					//m_bAdmin = true;
					setObj.AddMember("error", "", allocator); 
					setObj.AddMember("role", 0, allocator);
				}
			}
			else
			{
				CEFWebkitBrowserWnd::instance()->CloseQuery(pSet);
				setObj.AddMember("error", "账户不存在！", allocator); 
				setObj.AddMember("role", -1, allocator);
			}
		}
		else
		{
			CEFWebkitBrowserWnd::instance()->CloseQuery(pSet);
			setObj.AddMember("error", "数据打开失败！", allocator); 
			setObj.AddMember("role", -1, allocator);
		}
		CEFWebkitBrowserWnd::instance()->CloseQuery(pSet);

		//生成字符串
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		setObj.Accept(writer);

		string strJson = buffer.GetString();
		cpp2web(string("getLogin"), strJson);
		
	}


}

void CCEFV8HandlerEx::getInfo()
{
	char Temp[1024] = {0};
	string strPart;
	sprintf(Temp, "%d", g_Part);
	strPart = Temp;
	map<string,string> info;
	info["depart"] = strPart;

	setJson(string("getInfo"), info);

}

void CCEFV8HandlerEx::setJson(string &cmd, map<string,string> &info)
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

void CCEFV8HandlerEx::getControlOut()
{
	SControlOut out = CEFWebkitBrowserWnd::instance()->getControlOut();
	
	//生成Json串
	rapidjson::Document setObj;    //生成一个dom元素Document
	rapidjson::Document::AllocatorType &allocator = setObj.GetAllocator(); //获取分配器
	rapidjson::Value val;
	setObj.SetObject();    //将当前的Document设置为一个object，也就是说，整个Document是一个Object类型的dom元素
	
	setObj.AddMember("water_come", val.SetString(out.water_come.c_str(),allocator), allocator); 
	setObj.AddMember("water_put", val.SetString(out.water_put.c_str(),allocator), allocator); 
	setObj.AddMember("cy_come", val.SetString(out.cy_come.c_str(),allocator), allocator); 
	setObj.AddMember("gas_come", val.SetString(out.gas_come.c_str(),allocator), allocator); 

	setObj.AddMember("motor_come", val.SetString(out.motor_come.c_str(),allocator), allocator); 
	setObj.AddMember("normal_alarm", val.SetString(out.normal_alarm.c_str(),allocator), allocator); 
	setObj.AddMember("gaswater_alarm", val.SetString(out.gaswater_alarm.c_str(),allocator), allocator); 
	setObj.AddMember("freq_alarm", val.SetString(out.freq_alarm.c_str(),allocator), allocator); 

	setObj.AddMember("running_light", val.SetString(out.running_light.c_str(),allocator), allocator); 
	setObj.AddMember("reset", val.SetString(out.reset.c_str(),allocator), allocator); 
	setObj.AddMember("gas_put", val.SetString(out.gas_put.c_str(),allocator), allocator); 
	setObj.AddMember("water_put2", val.SetString(out.water_put2.c_str(),allocator), allocator); 

	//生成字符串
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	setObj.Accept(writer);

	string strJson = buffer.GetString();
	cpp2web(string("getControlOut"), strJson);
}

void CCEFV8HandlerEx::setControlOut(const CefString &para)
{
	string strDoc = base64_utf8_decode(para);

	rapidjson::Document	Obj;
	Obj.Parse(strDoc.c_str());

	SControlOut info;
	string str = Obj["water_come"].GetString();
	info.water_come = Obj["water_come"].GetString();
	info.water_put = Obj["water_put"].GetString();
	info.cy_come = Obj["cy_come"].GetString();
	info.gas_come = Obj["gas_come"].GetString();

	info.motor_come = Obj["motor_come"].GetString();
	info.normal_alarm = Obj["normal_alarm"].GetString();
	info.gaswater_alarm = Obj["gaswater_alarm"].GetString();
	info.running_light = Obj["running_light"].GetString();

	info.gas_put = Obj["gas_put"].GetString();
	info.reset = Obj["reset"].GetString();
	info.water_put2 = Obj["water_put2"].GetString();
	info.freq_alarm = Obj["freq_alarm"].GetString();

	CEFWebkitBrowserWnd::instance()->setControlOut(info);

}


