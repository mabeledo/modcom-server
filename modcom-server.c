/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: modcom-server.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include <glib.h>

#include "basemanager.h"

#define MODCOM_SERVER_VERSION	"0.1"

/* Funcion main
 * Precondiciones:
 * Postcondiciones:
 * Funcion de entrada del sistema de servidor
 * Recibe una serie de parametros genericos como (d)ebug, (v)erbose...
 * Pertenece al alto nivel
 * */
gint
main				(int argc, char *argv[])
{	
	/* Opciones a manejar:
	 *  - optVerbose activa la salida detallada del servidor
	 *  - optDebug activa la salida de informacion de depuracion del servidor
	 *  - optDaemon activa el modo de background
	 * */
	gboolean optVerbose = FALSE;
	gboolean optDaemon = FALSE;
	gboolean optVersion = FALSE;
	GError* error = NULL;
	gchar* returnError = "";
	gchar* optConfig = "modcom-server.cfg";
	gint i = 0;

	GOptionEntry options[] = 
	{
		{ "config", 'c', 0, G_OPTION_ARG_FILENAME, &optConfig, "Archivo de configuracion (por defecto modcom-server.cfg)", NULL },
		{ "verbose", 'V', 0, G_OPTION_ARG_NONE, &optVerbose, "Salida detallada", NULL },
		{ "version", 'v', 0, G_OPTION_ARG_NONE, &optVersion, "Version", NULL },
		{ "daemon", 'D', 0, G_OPTION_ARG_NONE, &optDaemon, "Modo Demonio", NULL },
		{ NULL }
	};
	
	GOptionContext *context;

	/* Manejo de opciones de la linea de comandos */
	context = g_option_context_new("- servidor del sistema ModCom");
	g_option_context_set_help_enabled (context, TRUE);
	g_option_context_set_description (context, "modcom-server es un servidor de comunicaciones modular que sirve de pasarela entre dispositivos");
	g_option_context_add_main_entries(context, options, "modcom-server");
	g_option_context_parse(context, &argc, &argv, &error);
	g_option_context_free(context);
	
	if (error)
	{
		g_print("Opcion desconocida.\nUtiliza --help para ver la ayuda\n");
		return(-1);
	}
	
	if (optVersion)
	{
		g_print("Version %s\n", MODCOM_SERVER_VERSION);
		return(0);
	}

	if (!g_str_equal(optConfig, "modcom-server.cfg"))
	{
		g_debug("Nuevo archivo de configuracion: %s", optConfig);
	}
	
	/* Todos los errores criticos resultan en una salida de la aplicacion */
	/* g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL); */
		 	
	/* Definir modo demonio */
	if (optDaemon)
	{
		/* Elimina el parametro -D */
		while (argv[i] != NULL)
		{
			if (g_str_equal(argv[i], "-D") == TRUE)
				g_stpcpy(argv[i], NULL);
			else
				i++;
		}
		/* Crea el nuevo proceso.
		 * Redirige la salida a /dev/null, por lo que no aparecera ningun
		 * mensaje de depuracion en pantalla.
		 * */
		if (!g_spawn_async(NULL, argv, NULL, (G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_STDOUT_TO_DEV_NULL),
						   NULL, NULL, NULL, &error))
		{
			g_critical("Imposible cargar el programa como demonio: %s", error->message);
			return (-2);
		}
		
		return (1);
	}
	
	/* Proceso */
	if (!initBaseSystem(optConfig, &returnError))
	{
		g_critical("%s", returnError);
		return (-3);	
	}
	
	g_print("Parent process done...\n");
	return(0);
}

/*
 * VERSION 0.1
 * */
