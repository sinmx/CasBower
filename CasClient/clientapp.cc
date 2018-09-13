#include "stdafx.h"
#include "clientapp.h"

CCefClientApp::CCefClientApp()
{
	m_v8Handler = new CCEFV8HandlerEx;

}

CCefClientApp::~CCefClientApp()
{

}


CefRefPtr<CefBrowserProcessHandler> CCefClientApp::GetBrowserProcessHandler()
{
	return this;
}

void CCefClientApp::OnBeforeCommandLineProcessing(const CefString & process_type, CefRefPtr<CefCommandLine> command_line)
{
	//加载flash插件
	command_line->AppendSwitchWithValue("--ppapi-flash-path", "ppflash/18_0_0_209/pepflashplayer32_18_0_0_209.dll");
	//manifest.json中的version
	command_line->AppendSwitchWithValue("--ppapi-flash-version", "18.0.0.209");
	command_line->AppendSwitch("--disable-extensions");
}


void CCefClientApp::OnContextInitialized()
{

}


//CefRefPtr<CefRenderProcessHandler> CCefClientApp::GetRenderProcessHandler()
//{
//	return this;	
//}

void CCefClientApp::OnWebKitInitialized()
{
	string app_code =
		//-----------------------------------
		//声明JavaScript里要调用的Cpp方法
		"var cef;"
		"if (!cef)"
		"  cef = {};"
		"(function() {"

		//  jsInvokeCPlusPlus 
		"  cef.onCallCpp = function(v1, v2) {"
		"    native function onCallCpp();"
		"    return onCallCpp(v1, v2);"
		"  };"

		"})();";


	// Register app extension module

	// JavaScript里调用app.jsInvokeCPlusPlus时，就会去通过CefRegisterExtension注册的CefV8Handler列表里查找
	// 找到"v8/app"对应的CCEFV8HandlerEx，就调用它的Execute方法
	// 假设m_v8Handler是CCefClientApp的一个成员变量
	//m_v8Handler = new CCEFV8HandlerEx();

	CefRegisterExtension("v8/cef", app_code, m_v8Handler);

}

void CCefClientApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	CefRefPtr<CefV8Value> object = context->GetGlobal();// 获取到window  
	CefRefPtr<CefV8Value> str = CefV8Value::CreateString("C++ created Value!");
	object->SetValue("jsValue", str, V8_PROPERTY_ATTRIBUTE_NONE);

	CefRefPtr<CefV8Accessor> accessor = new MyV8Accessor;
	CefRefPtr<CefV8Value> obj = CefV8Value::CreateObject(accessor);

	obj->SetValue("myval", V8_ACCESS_CONTROL_DEFAULT, V8_PROPERTY_ATTRIBUTE_NONE);

	object->SetValue("myobject", obj, V8_PROPERTY_ATTRIBUTE_NONE);

}


void CCefClientApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	m_v8Handler = NULL;
}
