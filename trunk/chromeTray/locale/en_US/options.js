
RegisterLang();

lang_en_US = 
{
	lngLanguage: "Language",
		
	lngTrayIcon: "Tray Icon",
	lngHideTrayIcon: "Hide tray icon if no minimized Chrome windows",
		
	lngMouseActions: "Mouse Actions",
	lngLeftButton: "Left Button",
	lngDoubleClick: "Double Click",
	lngRightButton: "Right Button",
	
	lngContextMenu: "Context Menu",	
	lngNewWindow: "Display 'New Window'",
	lngNewTab: "Display 'New Tab'",
		
	lngSave: "Save",
	lngExit: "Exit",
		
	lngDoNothing: "Do nothing",
	lngShowContextMenu : "Show context menu",
	lngRestoreAllWindows: "Restore all minimized windows",
	lngCreateNewTab: "Create a new Tab",
	lngCreateNewWindow: "Create a new Window"
}

function RegisterLang()
{
	var ctrl = document.getElementById("language");
	
	if(ctrl != null)
	{
		ctrl.add(createOption("English", "en_US"));
	}
}