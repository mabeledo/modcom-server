/********************************************************************
 *  Project: modcom-server
 *  File: modcom-server.c
 *  Version: 1.0b
 * 
 *  Author: Manuel Angel Abeledo Garcia
 *
 ************************ License ***********************************
 * 
 *  This file is part of modcom-server.
 *
 *  modcom-server is free software: you can redistribute it and/or 
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2 of
 *  the License, or (at your option) any later version.
 *
 *  modcom-server is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with modcom-server.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 ********************************************************************/

#include <glib.h>
#include <glib/gprintf.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


#include "base.h"

#define MODCOM_SERVER_VERSION	"1.0"
#define MODCOM_CFG				"modcom.cfg"
#define MODCOM_STATS			"modcom-stats.dat"

static gboolean statisticInfo = FALSE;

/* Funcion endProcess
 * Precondiciones:
 * Postcondiciones:
 * Funcion de entrada del sistema de servidor
 * Recibe una serie de parametros genericos como (d)ebug, (v)erbose...
 * Pertenece al alto nivel
 * */
static void
endProcess			(gint sig)
{
	gchar* errorMsg;

	#ifdef G_OS_UNIX
		pid_t pid;
		gchar * procFile, * contents;
	
		if (statisticInfo)
		{
			pid = getpid();
			g_sprintf(procFile, "/proc/%d/status", pid);
			
			if ((g_file_get_contents(procFile, &contents, NULL, NULL )) &&
				(g_file_set_contents(MODCOM_STATS, contents, -1, NULL)))
			{
				g_free(contents);
				g_debug("Statistic data saved");

			}
			else
			{
				g_warning("Statistic data not saved");
			}
		}
	#endif
	
	if (!closeBaseSystem(&errorMsg))
	{
		g_print("\nParent process terminated with signal %d\nErrors reported: %s\n", sig, errorMsg);
	}
	else
	{
		g_print("\nParent process terminated with signal %d.\nNo errors.\n", sig);
	}

	exit(sig);
}

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
	gboolean optStats = FALSE;
	GOptionContext *context;
	GError* error = NULL;
	gchar* returnError = "";
	gchar* defaultConfig = g_strconcat(g_get_current_dir(), "/", MODCOM_CFG, NULL);
	gchar* optConfig;
	gint i = 0;
	
	optConfig = g_strdup(defaultConfig);

	GOptionEntry options[] = 
	{
		{ "config", 'c', 0, G_OPTION_ARG_FILENAME, &optConfig, "Archivo de configuracion (por defecto modcom-server.cfg)", NULL },
		{ "verbose", 'V', 0, G_OPTION_ARG_NONE, &optVerbose, "Salida detallada", NULL },
		{ "version", 'v', 0, G_OPTION_ARG_NONE, &optVersion, "Version", NULL },
		{ "daemon", 'D', 0, G_OPTION_ARG_NONE, &optDaemon, "Modo Demonio", NULL },
		
	#ifdef G_OS_UNIX	
		{ "stats", 's', 0, G_OPTION_ARG_NONE, &optStats, "Recoge estadisticas en un archivo al salir (solo en GNU/Linux)", NULL },
	#endif
		{ NULL }
	};
	
	/* Check for GLib dependencies. */
	if (!GLIB_CHECK_VERSION(2, 12, 0))
	{
		g_critical("Version de GLib incorrecta. Se necesita al menos la version 2.12.");
		return (-1);
	}

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
		return(-2);
	}
	
	#ifdef G_OS_UNIX
		statisticInfo = optStats;
	#endif
	
	if (optVersion)
	{
		g_print("Version %s\n", MODCOM_SERVER_VERSION);
		return(0);
	}

	if (!g_str_equal(optConfig, defaultConfig))
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
			{
				g_stpcpy(argv[i], NULL);
			}
			else
			{
				i++;
			}
		}
		/* Crea el nuevo proceso.
		 * Redirige la salida a /dev/null, por lo que no aparecera ningun
		 * mensaje de depuracion en pantalla.
		 * */
		if (!g_spawn_async(NULL, argv, NULL, (G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_STDOUT_TO_DEV_NULL),
						   NULL, NULL, NULL, &error))
		{
			if (error != NULL)
			{
				g_critical("Imposible cargar el programa como demonio: %s", error->message);
			}
			else
			{
				g_critical("Imposible cargar el programa como demonio. Razon desconocida.");
			}
			
			return (-3);
		}
		
		return (1);
	}
	
	/* Catch termination signals. */
	signal(SIGINT, endProcess);
	signal(SIGTERM, endProcess);
	signal(SIGKILL, endProcess);

	if (!initBaseSystem(optConfig, &returnError))
	{
		g_critical("%s", returnError);
		return (-4);	
	}

	return(0);
}
