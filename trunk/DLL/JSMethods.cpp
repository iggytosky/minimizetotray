#include "StdAfx.h"
#include "JSMethods.h"

#include "JSValue.h"

#include "ScriptableNPObject.h"

#include "ChromeTrayIcon.h"

#include "utils.h"

CScriptableNPObject* CJSMethods::m_pScriptableObject = NULL;

CJSMethods::CJSMethods(void)
{
}

CJSMethods::~CJSMethods(void)
{
}

void CJSMethods::RegisterMethods(CScriptableNPObject *pObject)
{
	m_pScriptableObject = pObject;

	//pObject->RegisterMethod("OnFocusChanged", OnFocusChanged);
	pObject->RegisterMethod("OnWindowCreated", OnWindowCreated);
	//pObject->RegisterMethod("OnWindowRemoved", OnWindowRemoved);
	pObject->RegisterMethod("OnOptionsChanged", OnOptionsChanged);

	pObject->RegisterMethod("SetIcon", SetIcon);
	pObject->RegisterMethod("PopupNotify", PopupNotify);
}
/*
bool CJSMethods::OnFocusChanged(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const uint32_t argCount, const NPVariant *args)
{
	DebugLog(_T("CJSMethods::OnFocusChanged"));

	g_ChromeTrayIcon.ChromeWindowFocusChanged();

	return true;
}
*/
bool CJSMethods::OnWindowCreated(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const uint32_t argCount, const NPVariant *args)
{
	/*
	DebugLog(_T("CJSMethods::OnWindowCreated"));

	if(argCount == 0)
	{
		return false;
	}

	g_ChromeTrayIcon.AddChromeWindow(args[0].value.intValue);
	*/

	return true;
}

/*
bool CJSMethods::OnWindowRemoved(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const uint32_t argCount, const NPVariant *args)
{
	DebugLog(_T("CJSMethods::OnWindowRemoved"));

	if(argCount == 0)
	{
		return false;
	}

	g_ChromeTrayIcon.RemoveChromeWindow(args[0].value.intValue);

	return true;
}
*/

bool CJSMethods::OnOptionsChanged(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const uint32_t argCount, const NPVariant *args)
{
	DebugLog(_T("CJSMethods::OnOptionsChanged"));

	g_ChromeTrayIcon.OptionsChanged();

	return true;
}

bool CJSMethods::SetIcon(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const uint32_t argCount, const NPVariant *args)
{
	if(argCount == 0)
	{
		return (g_ChromeTrayIcon.SetTrayIcon(NULL) ? true : false);
	}

	int nValueLength = args[0].value.stringValue.utf8length;

	if(nValueLength == 0)
	{
		return (g_ChromeTrayIcon.SetTrayIcon(NULL) ? true : false);
	}

	TCHAR *lpszIconPath = new TCHAR[nValueLength + 1];

	int nConverted = MultiByteToWideChar(CP_UTF8, 0, args[0].value.stringValue.utf8characters, nValueLength, lpszIconPath, nValueLength + 1);

	lpszIconPath[nConverted] = '\0';

	DebugLog(_T("JS_SetIcon called, path: %s"), lpszIconPath);

	bool bResult = (g_ChromeTrayIcon.SetTrayIcon(lpszIconPath) ? true : false);

	delete [] lpszIconPath;

	return bResult;
}

bool CJSMethods::PopupNotify(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const uint32_t argCount, const NPVariant *args)
{
	if(argCount != 2)
	{
		return false;
	}

	int nConverted;

	wstring strTitle;
	wstring strText;

	int nTitleLength = args[0].value.stringValue.utf8length;

	if(nTitleLength != 0)
	{
		TCHAR *lpszTitle = new TCHAR[nTitleLength + 1];

		nConverted = MultiByteToWideChar(CP_UTF8, 0, args[0].value.stringValue.utf8characters, nTitleLength, lpszTitle, nTitleLength + 1);

		lpszTitle[nConverted] = '\0';

		strTitle = lpszTitle;

		delete [] lpszTitle;
	}

	int nTextLength = args[1].value.stringValue.utf8length;

	if(nTextLength != 0)
	{
		TCHAR *lpszText = new TCHAR[nTextLength + 1];

		nConverted = MultiByteToWideChar(CP_UTF8, 0, args[1].value.stringValue.utf8characters, nTextLength, lpszText, nTextLength + 1);

		lpszText[nConverted] = '\0';

		strText = lpszText;

		delete [] lpszText;
	}

	g_ChromeTrayIcon.PopupNotify(strTitle.c_str(), strText.c_str());

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

bool CJSMethods::CallJSMethod(NPNetscapeFuncs *pBrowserFuncs, NPP pluginInstance, const char* lpszMethod, NPVariant *pArgs, const uint32_t argCount, NPVariant &varResult)
{
	if(pluginInstance == NULL || pBrowserFuncs == NULL)
	{
		return false;
	}

	NPObject* npWindow = NULL;

	pBrowserFuncs->getvalue(pluginInstance, NPNVWindowNPObject, &npWindow);

	if(npWindow == NULL)
	{
		return false;
	}

	NPVariant varDocument;

	NPIdentifier idDocument = pBrowserFuncs->getstringidentifier("document");

	pBrowserFuncs->getproperty(pluginInstance, npWindow, idDocument, &varDocument);

	NPObject* npDocument = NPVARIANT_TO_OBJECT(varDocument);

	if(npDocument == NULL)
	{
		pBrowserFuncs->releaseobject(npWindow);

		return false;
	}

	NPIdentifier idMethod = pBrowserFuncs->getstringidentifier(lpszMethod);

	bool bResult = pBrowserFuncs->invoke(pluginInstance, npWindow, idMethod, pArgs, argCount, &varResult);
	
	pBrowserFuncs->releasevariantvalue(&varDocument);
	pBrowserFuncs->releaseobject(npWindow);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

bool CJSMethods::GetWindowTabs(int nWindowId, vector<ChromeTab> &tabs)
{
	if(m_pScriptableObject == NULL)
	{
		return false;
	}

	NPP pluginInstance				= m_pScriptableObject->GetPluginInstance();
	NPNetscapeFuncs *pBrowserFuncs	= m_pScriptableObject->GetBrowserFuncs();

	if(pluginInstance == NULL || pBrowserFuncs == NULL)
	{
		return false;
	}

	NPVariant varResults;

	NPVariant varArgs;
	INT32_TO_NPVARIANT(nWindowId, varArgs);

	bool bResult = CallJSMethod(pBrowserFuncs, pluginInstance, "getWindowTabs", &varArgs, 1, varResults);

	if(bResult)
	{
		if(NPVARIANT_IS_OBJECT(varResults))
		{
			NPObject *pArray = NPVARIANT_TO_OBJECT(varResults);

			if(pArray != NULL)
			{
				int			nLength = 0;
				NPVariant	varTab;

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "length", nLength))
				{
					for(size_t i = 0; i < (size_t)nLength; ++i)
					{
						if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, i, varTab))
						{
							if(NPVARIANT_IS_OBJECT(varTab))
							{
								NPObject *pTab = NPVARIANT_TO_OBJECT(varTab);

								ChromeTab newTab;
								
								if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pTab, "title", newTab.strTitle) &&
								   CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pTab, "id", newTab.nId))
								{
									tabs.push_back(newTab);
								}
							}
						}

						pBrowserFuncs->releasevariantvalue(&varTab);
					}
				}
			}
			
		}
	}

	pBrowserFuncs->releasevariantvalue(&varArgs);
	pBrowserFuncs->releasevariantvalue(&varResults);

	return bResult;
}

bool CJSMethods::SelectWindowTab(int nTabId)
{
	if(m_pScriptableObject == NULL)
	{
		return false;
	}

	NPP pluginInstance				= m_pScriptableObject->GetPluginInstance();
	NPNetscapeFuncs *pBrowserFuncs	= m_pScriptableObject->GetBrowserFuncs();

	if(pluginInstance == NULL || pBrowserFuncs == NULL)
	{
		return false;
	}

	NPVariant varResults;

	NPVariant varArgs;
	INT32_TO_NPVARIANT(nTabId, varArgs);

	bool bResult = CallJSMethod(pBrowserFuncs, pluginInstance, "selectWindowTab", &varArgs, 1, varResults);

	pBrowserFuncs->releasevariantvalue(&varArgs);
	pBrowserFuncs->releasevariantvalue(&varResults);

	return bResult;
}

bool CJSMethods::ShowOptions()
{
	if(m_pScriptableObject == NULL)
	{
		return false;
	}

	NPP pluginInstance				= m_pScriptableObject->GetPluginInstance();
	NPNetscapeFuncs *pBrowserFuncs	= m_pScriptableObject->GetBrowserFuncs();

	if(pluginInstance == NULL || pBrowserFuncs == NULL)
	{
		return false;
	}

	NPVariant varResults;
	NPVariant varArgs;

	bool bResult = CallJSMethod(pBrowserFuncs, pluginInstance, "showOptions", &varArgs, 0, varResults);

	pBrowserFuncs->releasevariantvalue(&varArgs);
	pBrowserFuncs->releasevariantvalue(&varResults);

	return bResult;
}

bool CJSMethods::NewTab()
{
	if(m_pScriptableObject == NULL)
	{
		return false;
	}

	NPP pluginInstance				= m_pScriptableObject->GetPluginInstance();
	NPNetscapeFuncs *pBrowserFuncs	= m_pScriptableObject->GetBrowserFuncs();

	if(pluginInstance == NULL || pBrowserFuncs == NULL)
	{
		return false;
	}

	NPVariant varResults;
	NPVariant varArgs;

	bool bResult = CallJSMethod(pBrowserFuncs, pluginInstance, "newTab", &varArgs, 0, varResults);

	pBrowserFuncs->releasevariantvalue(&varArgs);
	pBrowserFuncs->releasevariantvalue(&varResults);

	return bResult;
}

bool CJSMethods::NewWindow()
{
	if(m_pScriptableObject == NULL)
	{
		return false;
	}

	NPP pluginInstance				= m_pScriptableObject->GetPluginInstance();
	NPNetscapeFuncs *pBrowserFuncs	= m_pScriptableObject->GetBrowserFuncs();

	if(pluginInstance == NULL || pBrowserFuncs == NULL)
	{
		return false;
	}

	NPVariant varResults;
	NPVariant varArgs;

	bool bResult = CallJSMethod(pBrowserFuncs, pluginInstance, "newWindow", &varArgs, 0, varResults);

	pBrowserFuncs->releasevariantvalue(&varArgs);
	pBrowserFuncs->releasevariantvalue(&varResults);

	return bResult;
}

bool CJSMethods::OpenUrl(wstring strUrl)
{
	if(m_pScriptableObject == NULL)
	{
		return false;
	}

	NPP pluginInstance				= m_pScriptableObject->GetPluginInstance();
	NPNetscapeFuncs *pBrowserFuncs	= m_pScriptableObject->GetBrowserFuncs();

	if(pluginInstance == NULL || pBrowserFuncs == NULL)
	{
		return false;
	}

	NPVariant varResults;

	char *pszUrl = new char[strUrl.size() + 4];
	int nConverted = WideCharToMultiByte(CP_ACP, 0, strUrl.c_str(), strUrl.size(), pszUrl, strUrl.size() + 1, NULL, NULL);
	pszUrl[nConverted] = '\0';
	
	NPVariant varArgs;
	STRINGN_TO_NPVARIANT(pszUrl, strlen(pszUrl), varArgs);

	bool bResult = CallJSMethod(pBrowserFuncs, pluginInstance, "openUrl", &varArgs, 1, varResults);

	// Dll will be crashed if you uncomment next line.
	// I don't know why, yet.
	//pBrowserFuncs->releasevariantvalue(&varArgs);

	pBrowserFuncs->releasevariantvalue(&varResults);

	delete [] pszUrl;

	return bResult;
}

bool CJSMethods::GetOptions(ChromeTrayIconOptions &options)
{
	if(m_pScriptableObject == NULL)
	{
		return false;
	}

	NPP pluginInstance				= m_pScriptableObject->GetPluginInstance();
	NPNetscapeFuncs *pBrowserFuncs	= m_pScriptableObject->GetBrowserFuncs();

	if(pluginInstance == NULL || pBrowserFuncs == NULL)
	{
		return false;
	}

	NPVariant varResults;
	NPVariant varArgs;

	bool bResult = CallJSMethod(pBrowserFuncs, pluginInstance, "getOptions", &varArgs, 0, varResults);

	if(bResult)
	{
		if(NPVARIANT_IS_OBJECT(varResults))
		{
			NPObject *pArray = NPVARIANT_TO_OBJECT(varResults);

			if(pArray != NULL)
			{
				wstring strValue;

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "minimizeOnLeftButton", strValue))
				{
					options.bMinimizeOnLeftButton = (strValue == _T("true"));
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "minimizeOnRightButton", strValue))
				{
					options.bMinimizeOnRightButton = (strValue == _T("true"));
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "trayHide", strValue))
				{
					options.bHideTray = (strValue == _T("true"));
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "actLClick", strValue))
				{
					options.actLClick = (TrayAction)_wtoi(strValue.c_str());
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "actRClick", strValue))
				{
					options.actRClick = (TrayAction)_wtoi(strValue.c_str());
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "actLDblClick", strValue))
				{
					options.actLDblClick = (TrayAction)_wtoi(strValue.c_str());
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "displayOptions", strValue))
				{
					options.bShowOptions = (strValue == _T("true"));
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "displayNewWindow", strValue))
				{
					options.bShowNewWindow = (strValue == _T("true"));
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "displayNewTab", strValue))
				{
					options.bShowNewTab = (strValue == _T("true"));
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "displayFavorites", strValue))
				{
					options.bShowFavorites = (strValue == _T("true"));
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "favorites", strValue))
				{
					options.strFavorites = strValue;
				}

				//////////////////////////////////////////////////////////////////////////
				// Boss key
				//////////////////////////////////////////////////////////////////////////

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "enableBossKey", strValue))
				{
					options.bEnableBossKey = (strValue == _T("true"));
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "bossMod1", strValue))
				{
					options.wBossModifier = StringToKey(strValue.c_str());
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "bossMod2", strValue))
				{
					options.wBossModifier |= StringToKey(strValue.c_str());
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "bossKey", strValue))
				{
					if(strValue.empty() == FALSE)
					{
						transform(strValue.begin(), strValue.end(), strValue.begin(), toupper);
						options.wBossKey = (WORD)strValue[0];
					}
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "bossHideTrayIcon", strValue))
				{
					options.bBossHideTrayIcon = (strValue == _T("true"));
				}
			}
		}
	}

	pBrowserFuncs->releasevariantvalue(&varArgs);
	pBrowserFuncs->releasevariantvalue(&varResults);

	return bResult;
}

bool CJSMethods::GetLanguage(ChromeTrayIconLanguage &language)
{
	if(m_pScriptableObject == NULL)
	{
		return false;
	}

	NPP pluginInstance				= m_pScriptableObject->GetPluginInstance();
	NPNetscapeFuncs *pBrowserFuncs	= m_pScriptableObject->GetBrowserFuncs();

	if(pluginInstance == NULL || pBrowserFuncs == NULL)
	{
		return false;
	}

	NPVariant varResults;
	NPVariant varArgs;

	bool bResult = CallJSMethod(pBrowserFuncs, pluginInstance, "getLanguage", &varArgs, 0, varResults);

	if(bResult)
	{
		if(NPVARIANT_IS_OBJECT(varResults))
		{
			NPObject *pArray = NPVARIANT_TO_OBJECT(varResults);

			if(pArray != NULL)
			{
				wstring strValue;

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "lngOptions", strValue))
				{
					language.strOptions = strValue;
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "lngNewTab", strValue))
				{
					language.strNewTab = strValue;
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "lngNewWindow", strValue))
				{
					language.strNewWindow = strValue;
				}

				if(CJSValue::GetProperty(pBrowserFuncs, pluginInstance, pArray, "lngFavorites", strValue))
				{
					language.strFavorites = strValue;
				}
			}
		}
	}

	pBrowserFuncs->releasevariantvalue(&varArgs);
	pBrowserFuncs->releasevariantvalue(&varResults);

	return bResult;
}