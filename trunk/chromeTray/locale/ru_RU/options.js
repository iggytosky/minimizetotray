
RegisterLang();

lang_ru_RU = 
{
	lngLanguage: "Язык",
		
	lngTrayIcon: "Иконка",
	lngHideTrayIcon: "Прятать иконку, если нет скрытых окон",
		
	lngMouseActions: "Клавиши мыши",
	lngLeftButton: "Левая клавиша",
	lngDoubleClick: "Двойной клик",
	lngRightButton: "Правая клавиша",
	
	lngContextMenu: "Контекстное меню",	
	lngNewWindow: "Пункт 'Новое окно'",
	lngNewTab: "Пункт 'Новая закладка'",
		
	lngSave: "Сохранить",
	lngExit: "Выход",
		
	lngDoNothing: "Не делать ничего",
	lngShowContextMenu : "Контекстное меню",
	lngRestoreAllWindows: "Восстановить все скрытые окна",
	lngCreateNewTab: "Создать новую закладку",
	lngCreateNewWindow: "Создать новое окно"
}

function RegisterLang()
{
	var ctrl = document.getElementById("language");
	
	ctrl.add(createOption("Russian", "ru_RU"));
}