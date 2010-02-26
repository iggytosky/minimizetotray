
RegisterLang();

lang_en_US = 
{
	lngLanguage: "Language",
	
	lngMinimizeOn: "Hide to Tray On",
	lngMinimizeOnLeftButton: "Minimize on Left button down on minimize button",
	lngMinimizeOnRightButton: "Minimize on Right button down on minimize button",
		
	lngTrayIcon: "Tray Icon",
	lngHideTrayIcon: "Hide tray icon if no minimized Chrome windows",
		
	lngMouseActions: "Mouse Actions",
	lngLeftButton: "Left Button",
	lngDoubleClick: "Double Click",
	lngRightButton: "Right Button",
	
	lngContextMenu: "Context Menu",	
	lngNewWindow: "Display 'New Window'",
	lngNewTab: "Display 'New Tab'",
	lngOptions: "Display 'Options'",
	lngFavorites: "Display 'Favorites'",
		
	lngSave: "Save",
	lngExit: "Exit",
		
	lngDoNothing: "Do nothing",
	lngShowContextMenu : "Show context menu",
	lngRestoreAllWindows: "Restore all minimized windows",
	lngCreateNewTab: "Create a new Tab",
	lngCreateNewWindow: "Create a new Window",
		
	lngBossKey: "Boss key",
	lngEnableBossKey: "Hide all visible Chrome windows",
	lngBossHideTrayIcon: "Hide tray icon"
}

function RegisterLang()
{
	var ctrl = document.getElementById("language");
	
	if(ctrl != null)
	{
		ctrl.add(createOption("English", "en_US"));
	}
}