function init()
{
	setSelectedValue("language", readProperty("language", "en_US"));
	localize();
		
	fillActionSelect('actLClick');
	fillActionSelect('actLDblClick');
	fillActionSelect('actRClick');
	
	doDataExchange(false);
}

function getSelectedLanguage()
{
	var lang = getSelectedValue("language");
	return eval("lang_" + lang);
}

function onLanguageSelected()
{
	doDataExchange(true);
	init();
}

function localize()
{
	var lang  = getSelectedLanguage();
	
	for(var ctrlId in lang)
	{
   		var value = lang[ctrlId];
   		
   		var ctrl = document.getElementById(ctrlId);
   		
   		if(ctrl == null)
   		{
   			continue;
   		}
   		
   		if(ctrl.tagName == "SPAN")
   		{
   			ctrl.innerText = value;
   		}
   		else if(ctrl.tagName == "INPUT")
   		{
   			ctrl.value = value;
   		}
	}
}

function fillActionSelect(selectId)
{
	var lang  = getSelectedLanguage();

	var ctrl = document.getElementById(selectId);
	
	while(ctrl.options.length != 0)
	{      
		ctrl.removeChild(ctrl.options[0]);
	}
	
	ctrl.add(createOption(lang.lngDoNothing, "0"));
	ctrl.add(createOption(lang.lngShowContextMenu, "1"));
	ctrl.add(createOption(lang.lngRestoreAllWindows, "2"));
	ctrl.add(createOption(lang.lngCreateNewTab, "3"));
	ctrl.add(createOption(lang.lngCreateNewWindow, "4"));
}

function createOption(text, value)
{
	var opt = document.createElement('option');
	
	opt.text = text;
	opt.value = value;
	
	return opt;
}

function getSelectedValue(selectId)
{
	var ctrl = document.getElementById(selectId);
	return ctrl.options[ctrl.selectedIndex].value;
}

function setSelectedValue(selectId, value)
{
	var ctrl = document.getElementById(selectId);
	
	for (var i = 0; i < ctrl.options.length; ++i)
	{
		if (ctrl.options[i].value == value)
		{
			ctrl.options[i].selected = true;
			break;
		}
	}
}

function getCheckState(ctrlId)
{
	return document.getElementById(ctrlId).checked;
}

function setCheckState(ctrlId, checked)
{
	document.getElementById(ctrlId).checked = (checked == "true");
}

function readProperty(property, defValue)
{
	if(window.localStorage == null)
	{
		return defValue;
	}
	
	if(window.localStorage[property] == null)
	{
		return defValue;
	}
	
	return window.localStorage[property];
}

function doDataExchange(save)
{
	if(save)
	{
		window.localStorage["language"] = getSelectedValue("language");
		
		window.localStorage["trayHide"] = getCheckState("trayHide");
		
		window.localStorage["actLClick"] = getSelectedValue("actLClick");
		window.localStorage["actLDblClick"] = getSelectedValue("actLDblClick");
		window.localStorage["actRClick"] = getSelectedValue("actRClick");
		
		window.localStorage["displayNewWindow"] = getCheckState("displayNewWindow");
		window.localStorage["displayNewTab"] = getCheckState("displayNewTab");
	}
	else
	{
		setSelectedValue("language", readProperty("language", "en_US"));
		
		setCheckState("trayHide", readProperty("trayHide", "false"));
		
		setSelectedValue("actLClick", readProperty("actLClick", "0"));
		setSelectedValue("actLDblClick", readProperty("actLDblClick", "2"));
		setSelectedValue("actRClick", readProperty("actRClick", "1"));
		
		setCheckState("displayNewWindow", readProperty("displayNewWindow", "false"));
		setCheckState("displayNewTab", readProperty("displayNewTab", "false"));
	}
}

function onSave()
{
	doDataExchange(true);
	
	var port = chrome.extension.connect({name: 'minimize-to-tray'});
	port.postMessage({command: 'OptionsChanged'});
	
	window.close();
}

function onExit()
{
	window.close();
}