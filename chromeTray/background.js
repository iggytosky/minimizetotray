initPlugin();

/*
function notifyFocusChanged(windowId)
{
	var plugin = document.getElementById("minimize-to-tray-extension");
	plugin.OnFocusChanged(windowId);
}
*/

function notifyWindowCreated(windowId)
{
	var plugin = document.getElementById("minimize-to-tray-extension");
	plugin.OnWindowCreated(windowId);
}

/*
function notifyWindowRemoved(windowId)
{
	var plugin = document.getElementById("minimize-to-tray-extension");
	plugin.OnWindowRemoved(windowId);
}
*/

function notifyOptionsChanged(windowId)
{
	var plugin = document.getElementById("minimize-to-tray-extension");
	plugin.OnOptionsChanged();
}

function setIcon(path)
{
	var plugin = document.getElementById("minimize-to-tray-extension");
	plugin.SetIcon(path);
}

function popupNotify(title, text)
{
	var plugin = document.getElementById("minimize-to-tray-extension");
	plugin.PopupNotify(title, text);
}

function initPlugin()
{
	chrome.windows.getCurrent(function(wnd) {
		notifyWindowCreated(wnd.id);
	});
}

/////////////////////////////////////////////////////////////////////////////////

function getLanguage()
{
	var lang = readProperty("language");
	return eval("lang_" + lang);
}

function getWindowTabs(windowId)
{
	var tabsInfo = [];
	
	chrome.tabs.getAllInWindow(windowId, function enumTabs(tabs) {
		if(tabs != undefined && !(tabs.length == 1 && tabs[0].title == undefined))
		{
			for (var i = 0; i < tabs.length; i++) 
			{
				if(tabs[i].title == undefined)
				{
					continue;
				}
				tabsInfo.push({id: tabs[i].id, title: tabs[i].title});
			}
		}
	});
	
	return tabsInfo;
}

function selectWindowTab(tabId)
{
	chrome.tabs.update(tabId, {selected: true});
}

function showOptions()
{
	chrome.tabs.create({url:'options.html'});
}

function openUrl(url)
{
	chrome.tabs.create({selected: true, url: url});
}

function newTab()
{
	chrome.tabs.create({selected: true});
}

function newWindow()
{
	console.log();
	chrome.windows.create({});
}

function readProperty(property, defValue)
{
	if(localStorage == null)
	{
		return defValue;
	}
	
	if(localStorage[property] == null)
	{
		return defValue;
	}
	
	return "" + localStorage[property] + "";
}

function getOptions()
{
	return {
		minimizeOnLeftButton: readProperty("minimizeOnLeftButton", "true"),
		minimizeOnRightButton: readProperty("minimizeOnRightButton", "false"),

		trayHide: readProperty("trayHide", "false"),
		
		actLClick: readProperty("actLClick", "0"),
		actLDblClick: readProperty("actLDblClick", "2"),
		actRClick: readProperty("actRClick", "1"),
		
		displayNewWindow: readProperty("displayNewWindow", "false"),
		displayNewTab: readProperty("displayNewTab", "false"),
		displayOptions: readProperty("displayOptions", "true"),
		displayFavorites: readProperty("displayFavorites", "false"),
		favorites: readProperty("favorites", ""),
			
		enableBossKey: readProperty("enableBossKey", "false"),
		bossMod1: readProperty("bossMod1", "0"),
		bossMod2: readProperty("bossMod2", "0"),
		bossKey: readProperty("bossKey", "0"),
		bossHideTrayIcon: readProperty("bossHideTrayIcon", "false"),
			
			
		enableRestoreAllHotkey: readProperty("enableRestoreAllHotkey", "false"),
		restoreAllHotkeyMod1: readProperty("restoreAllHotkeyMod1", "0"),
		restoreAllHotkeyMod2: readProperty("restoreAllHotkeyMod2", "0"),
		restoreAllHotkeyKey: readProperty("restoreAllHotkeyKey", "0"),
	};
}

/////////////////////////////////////////////////////////////////////////////////

/*
chrome.windows.onFocusChanged.addListener(function(windowId) {
	notifyWindowCreated(windowId);
});

chrome.windows.onCreated.addListener(function(wnd) {
	notifyWindowCreated(wnd.id);
});

chrome.windows.onRemoved.addListener(function(windowId) {
	notifyWindowRemoved(windowId);
});
*/

chrome.extension.onConnect.addListener(function(port) {
	if(port.name == 'minimize-to-tray')
	{
		port.onMessage.addListener(function(data) {
			console.log("listener data=" + data);
			switch(data.command)
			{
				case 'OptionsChanged':
					notifyOptionsChanged();
					break;
			}
		});
	}
});

if(chrome.extension.onConnectExternal != null)
{
	chrome.extension.onConnectExternal.addListener(function(port) {
		if(port.name == 'minimize-to-tray')
		{
			port.onMessage.addListener(function(data) {
				console.log("listener data=" + data);
				switch(data.command)
				{
					case 'SetIcon':
						setIcon(data.param);
						break;
						
					case 'PopupNotify':
						popupNotify(data.title, data.text);
						break;
				}
			});
		}
	});
}