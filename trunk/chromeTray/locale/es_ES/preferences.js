
RegisterLang();

lang_es_ES = 
{
	lngLanguage: "Idioma",
		
	lngTrayIcon: "Icono de la bandeja del sistema",
	lngHideTrayIcon: "Esconder icono si no hay ventanas de Chrome minimizadas",
		
	lngMouseActions: "Acciones del Ratón",
	lngLeftButton: "Botón Izquierdo",
	lngDoubleClick: "Doble Clic",
	lngRightButton: "Botón Derecho",
	
	lngContextMenu: "Menú Contextual",	
	lngNewWindow: "Mostrar 'Ventana Nueva'",
	lngNewTab: "Mostrar 'Nueva Pestaña'",
		
	lngSave: "Guardar",
	lngExit: "Salir",
		
	lngDoNothing: "No hacer nada",
	lngShowContextMenu : "Mostrar Menú Contextual",
	lngRestoreAllWindows: "Restaurar todas las ventanas",
	lngCreateNewTab: "Crear nueva Pestaña",
	lngCreateNewWindow: "Crear nueva Ventana",
}

function RegisterLang()
{
	var ctrl = document.getElementById("language");
	
	if(ctrl != null)
	{
		ctrl.add(createOption("Español", "es_ES"));
	}
}