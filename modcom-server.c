/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: modcom-server.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include <stdio.h>
#include <glib.h>
#include <gmodule.h>

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
	/* Almacenamiento de la salida de funciones */
	gint resultFunc;
	
	/* Colas de recepcion, envio y modulos */
	GQueue *qReception;
	GQueue *qSend;
	GQueue *qModules;
	
	/* Opciones a manejar:
	 *  - optVerbose activa la salida detallada del servidor
	 *  - optDebug activa la salida de informacion de depuracion del servidor
	 *  - optDaemon activa el modo de background
	 * */
	gboolean optVerbose = FALSE;
	gboolean optDebug = FALSE;
	gboolean optDaemon = FALSE;
	gchar *optConfig = "modcom-server.cfg";

	GOptionEntry options[] = 
	{
		{ "config", 'c', 0, G_OPTION_ARG_FILENAME, &optConfig, "Archivo de configuracion", NULL },
		{ "verbose", 'v', 0, G_OPTION_ARG_NONE, &optVerbose, "Salida detallada", NULL },
		{ "debug", 'd', 0, G_OPTION_ARG_NONE, &optDebug, "Modo de depuracion", NULL },
		{ "daemon", 'D', 0, G_OPTION_ARG_NONE, &optDaemon, "Modo Demonio", NULL },
		{ NULL }
	};
	
	GOptionContext *context;

	/* Manejo de opciones de la linea de comandos */
	context = g_option_context_new("- servidor del sistema ModCom");
	g_option_context_set_help_enabled (context, TRUE);
	g_option_context_set_description (context, "modcom-server es un servidor de comunicaciones modular que sirve de pasarela entre dispositivos");
	g_option_context_add_main_entries(context, options, "modcom-server");
	g_option_context_parse(context, &argc, &argv, NULL);
	g_option_context_free(context);

	/* Todos los errores criticos resultan en una salida de la aplicacion */
	g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);
	
	/* Definir el modo detallado */
	if (optDebug)
	{
		g_debug("Modo detallado activado");
	}
	else
	{
		
	}
	 	
	/* Definir el modo de depuracion */
	
	
	/* Lectura del archivo de configuracion */
	/* TODO
	 * */
	
	/* Inicializar colas */
	g_debug("Inicializando colas de mensajes");
	qSend = g_queue_new();
	qPlugins = g_queue_new();
	
	/* Cargar los modulos en memoria */
	if ((resultFunc = loadAllPlugins ((qPlugins)) != 0)
		g_critical("No se pudieron cargar los modulos, error %d", resultFunc);
	
	/* Inicializacion de los modos de envio y recepcion simultaneamente */
	if ((resultFunc = loadSymbols(qModules, qSend, qReception)) != 0)
		g_critical("No se cargaron correctamente los simbolos, error %d", resultFunc);
	
	return(0);
}

/*
 * VERSION 0.1
 * */
