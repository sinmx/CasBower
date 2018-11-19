#pragma once
#include "stdafx.h"
#include <cef_v8.h>
#include <cef_base.h>
//数据处理全在这里
class CCEFV8HandlerEx : public CefV8Handler {
public:
	CCEFV8HandlerEx();
	~CCEFV8HandlerEx();
public:
	virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) override;

	//void setJson(string &cmd, map<string,string> &info);

private:
	// Map of message callbacks.
	typedef map<pair<string, int>, pair<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value> > >CallbackMap;
	CallbackMap callback_map_;

	void getSql(const CefString &sql);
	void setSql(const CefString &sql);
	void newBill(const CefString &para);
	void modifyBill(const CefString &para);

	//control
	void setCon(const CefString &para);

	//db
	void getLogin(const CefString &para);


	//web interface
	void getInfo();
	void getControlOut();
	void setControlOut(const CefString &para);

	//common
	
	//void cpp2web(string &cmd, string &value);

public:
	IMPLEMENT_REFCOUNTING(CCEFV8HandlerEx);
};

