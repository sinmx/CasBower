#include "stdafx.h"
#include "CEFV8HandlerEx.h"
#include <strsafe.h>
#include "CEFWebkitBrowser.h"

#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "prettywriter.h"
#include "stringbuffer.h"

CCEFV8HandlerEx::CCEFV8HandlerEx()
{

}

CCEFV8HandlerEx::~CCEFV8HandlerEx()
{

}

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static inline bool is_base64(BYTE c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(BYTE const* buf, unsigned int bufLen) {
	std::string ret;
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

std::vector<BYTE> base64_decode(std::string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	BYTE char_array_4[4], char_array_3[3];
	std::vector<BYTE> ret;

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

std::string string_To_UTF8(const std::string & str) 
{ 
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0); 

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	ZeroMemory(pwBuf, nwLen * 2 + 2); 

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen); 

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL); 

	char * pBuf = new char[nLen + 1]; 
	ZeroMemory(pBuf, nLen + 1); 

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL); 

	std::string retStr(pBuf); 

	delete []pwBuf; 
	delete []pBuf; 

	pwBuf = NULL; 
	pBuf  = NULL; 

	return retStr; 
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
		CefString strParam1 = arguments.at(0)->GetStringValue();
		CefString strParam2 = arguments.at(1)->GetStringValue();

		if (strParam1 == "getLogin")
		{
			getLogin(strParam2);
		} 
		else if (strParam1 == "getSql")
		{
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

void CCEFV8HandlerEx::getLogin(const CefString &para)
{
	std::vector<BYTE> ar = base64_decode(para);
	std::string strDoc(ar.begin(), ar.end());

	rapidjson::Document	getObj;
	getObj.Parse(strDoc.c_str());

	if (getObj["name"].IsString() && getObj["password"].IsString())
	{
		CDuiString strSql,strName,strPwd;
		strName = getObj["name"].GetString();
		strPwd = getObj["password"].GetString();

		//生成Json串
		rapidjson::Document setObj;    //生成一个dom元素Document
		rapidjson::Document::AllocatorType &allocator = setObj.GetAllocator(); //获取分配器
		setObj.SetObject();    //将当前的Document设置为一个object，也就是说，整个Document是一个Object类型的dom元素

		_RecordsetPtr pSet;

		strSql.Format(L"select * from UserInfo where Name='%s'", strName);
		if (0x00 == CEFWebkitBrowserWnd::instance()->OpenQuery(pSet, strSql))
		{
			if(!pSet->adoEOF)
			{
				CDuiString dbPwd = CEFWebkitBrowserWnd::instance()->getField(pSet, L"Password");
				if (dbPwd != strPwd)
				{
					CEFWebkitBrowserWnd::instance()->CloseQuery(pSet);
					setObj.AddMember("error", "密码错误！", allocator); 
					setObj.AddMember("role", -1, allocator);
				}
				else if (L"0" == CEFWebkitBrowserWnd::instance()->getField(pSet, L"Role"))
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

		std::string strJson = buffer.GetString();
		strJson = string_To_UTF8(strJson);
		strJson = "window.getLogin({\"value\":\"" + base64_encode((const BYTE*)strJson.c_str(), strJson.length()) + "\"});";
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


}
