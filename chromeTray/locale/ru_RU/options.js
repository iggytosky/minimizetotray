
RegisterLang();

lang_ru_RU = 
{
	lngLanguage: "Язык",

	lngMinimizeOn: "Прятать в Трей",
	lngMinimizeOnLeftButton: "При нажатии Левой клавиши мыши на кнопке сворачивания окна",
	lngMinimizeOnRightButton: "При нажатии Правой клавиши мыши на кнопке сворачивания окна",

	lngTrayIcon: "Иконка",
	lngHideTrayIcon: "Прятать иконку, если нет скрытых окон",
		
	lngMouseActions: "Клавиши мыши",
	lngLeftButton: "Левая клавиша",
	lngDoubleClick: "Двойной клик",
	lngRightButton: "Правая клавиша",
	
	lngContextMenu: "Контекстное меню",	
	lngNewWindow: "Пункт 'Новое окно'",
	lngNewTab: "Пункт 'Новая закладка'",
	lngOptions: "Пункт 'Настроить'",
	lngFavorites: "Пункт 'Избранное'",

	lngSave: "Сохранить",
	lngExit: "Выход",
		
	lngDoNothing: "Не делать ничего",
	lngShowContextMenu : "Контекстное меню",
	lngRestoreAllWindows: "Восстановить все скрытые окна",
	lngCreateNewTab: "Создать новую закладку",
	lngCreateNewWindow: "Создать новое окно",
		
	lngBossKey: "Boss key",
	lngEnableBossKey: "Прятать все видимые окна Chrome при нажатии",
	lngBossHideTrayIcon: "Прятать иконку"
}

function RegisterLang()
{
	var ctrl = document.getElementById("language");
	
	ctrl.add(createOption("Russian", "ru_RU"));
}