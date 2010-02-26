
RegisterLang();

lang_es_ES = 
{
	lngLanguage: "Idioma",
		
	lngMinimizeOn: "Hide to Tray On",
	lngMinimizeOnLeftButton: "Minimize on Left button down on minimize button",
	lngMinimizeOnRightButton: "Minimize on Right button down on minimize button",

	lngTrayIcon: "Icono de la bandeja del sistema",
	lngHideTrayIcon: "Esconder icono si no hay ventanas de Chrome minimizadas",
		
	lngMouseActions: "Acciones del Ratón",
	lngLeftButton: "Botón Izquierdo",
	lngDoubleClick: "Doble Clic",
	lngRightButton: "Botón Derecho",
	
	lngContextMenu: "Menú Contextual",	
	lngNewWindow: "Mostrar 'Ventana Nueva'",
	lngNewTab: "Mostrar 'Nueva Pestaña'",
	lngOptions: "Mostrar 'Opciones'",
	lngFavorites: "Mostrar 'Favoritos'",
		
	lngSave: "Guardar",
	lngExit: "Salir",
		
	lngDoNothing: "No hacer nada",
	lngShowContextMenu : "Mostrar Menú Contextual",
	lngRestoreAllWindows: "Restaurar todas las ventanas",
	lngCreateNewTab: "Crear nueva Pestaña",
	lngCreateNewWindow: "Crear nueva Ventana",
		
	lngBossKey: "Clave Maestra",
	lngEnableBossKey: "Esconder todas las ventanas visibles de Chrome",
	lngBossHideTrayIcon: "Esconder icono de la bandeja del sistema"
}

function RegisterLang()
{
	var ctrl = document.getElementById("language");

	ctrl.add(createOption("Español", "es_ES"));
}