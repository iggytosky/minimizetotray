#pragma once

#include "types.h"

class CScriptableNPObject;

class CJSMethods
{
public:
	CJSMethods(void);
	~CJSMethods(void);

public:
	static void RegisterMethods(CScriptableNPObject *pObject);

protected:
	static bool CallJSMethod(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const char* lpszMethod, NPVariant *pArgs, const uint32_t argCount, NPVariant &varResult);

public:
	static bool GetWindowTabs(int nWindowId, vector<ChromeTab> &tabs);
	static bool SelectWindowTab(int nTabId);
	static bool ShowOptions();
	static bool NewTab();
	static bool NewWindow();
	static bool OpenUrl(wstring strUrl);
	static bool GetOptions(ChromeTrayIconOptions &options);
	static bool GetLanguage(ChromeTrayIconLanguage &language);

protected:
	//static bool OnFocusChanged(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const uint32_t argCount, const NPVariant *args);
	//static bool OnWindowCreated(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const uint32_t argCount, const NPVariant *args);
	//static bool OnWindowRemoved(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const uint32_t argCount, const NPVariant *args);
	static bool OnOptionsChanged(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const uint32_t argCount, const NPVariant *args);

	static bool SetIcon(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const uint32_t argCount, const NPVariant *args);
	static bool PopupNotify(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const uint32_t argCount, const NPVariant *args);

protected:
	static CScriptableNPObject	*m_pScriptableObject;
};
