/*******************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: base.c
 * 	Version: 0.1
 *
 *  Application entry point. All operations are initialized and managed
 *  here, so base.c is a interface to the "controller" side of the
 *  application.
 * 
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "general.h"
#include "msg.h"
#include "plugin.h"
#include "thread.h"

#include "config.h"
#include "loader.h"
#include "receiver.h"
#include "dispatcher.h"

/* Error messages */
#define THREADSNOTSUPPORTED	"Threads not supported"
#define DISPATCHERROR		"Unable to initialize the dispatcher"
#define RECEIVEERROR		"Unable to initialize all receivers"

/* Global variables. */
static GQueue* qPlugins;
static GAsyncQueue* qMessages;
static GThread* dispatchThread;
static GThread* receiveThread;
static ThreadData* tData;

/* Funcion initBaseSystem
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
initBaseSystem				(const gchar* configFile, gchar** error)
{
	/* Memory allocation is done here to avoid local issues or
	 * automatic deallocation after function exit.
	 * */
	GData* dConfig;
	GData* baseConfig;
	GData* pluginConfig;
	GData* receiveConfig;
	GData* dispatchConfig;
	GData* pluginSetConfig;

	GError* threadError;
	
	gchar* behaviour;
	
	#ifdef G_THREADS_ENABLED
		if (!g_thread_supported())
		{
			g_thread_init(NULL);
		}
	#else
		*error = g_strdup(THREADSNOTSUPPORTED);
		return (FALSE);
	#endif

	/* All data in the configuration file is copied into a
	 * GData structure, in order to handle configuration parameters
	 * easily.
	 * */
	if (!(initConfigFile(configFile, error) &&
	      loadConfigFile(configFile, &dConfig, error)))
	{
		return (FALSE);
	}
	
	/* Gets per module configuration settings. */
	baseConfig = g_datalist_get_data(&dConfig, "base");
	pluginConfig = g_datalist_get_data(&dConfig, "plugins");
	receiveConfig = g_datalist_get_data(&dConfig, "receive");
	dispatchConfig = g_datalist_get_data(&dConfig, "dispatch");
	
	/* Fill base module configure options. */
	behaviour = (gchar*)g_datalist_get_data(&baseConfig, "behaviour");
	
	/* *
	 * Segunda fase (comprobacion y ejecucion de procesos):
	 *  - Existe el directorio de los complementos.
	 *  - El sistema puede cargar modulos dinamicamente.
	 *  - El sistema puede utilizar hilos de ejecucion.
	 *  - (Initdispatcher)
	 * */
	if (!(initPlugins(&pluginConfig, error) &&
		  initReceivers(&receiveConfig, error) &&
		  initDispatcher(&dispatchConfig, error)))
	{
		return (FALSE);
	}
	
	/* Free memory containing configuration patterns already used. */
	g_datalist_remove_data(&dConfig, "plugins");
	pluginSetConfig = dConfig;
	
	/* Inicializa la cola de plugins y de mensajes. */
	qPlugins = g_queue_new();
	qMessages = g_async_queue_new();
	tData = g_new0(ThreadData, 1);
	tData->qPlugins = qPlugins;
	tData->qMessages = qMessages;
	
	/* Se inicializan las estructuras de plugins.
	 *  - Explora el directorio de plugins y carga los ficheros.
	 *  - Carga las funciones en las estructuras de plugin correspondientes.
	 * */
	if (!loadAllPlugins(qPlugins, &dConfig, error))
	{
		return (FALSE);
	}
	
	/* DispatchManager manages the message dispatching process.
	 * ReceiveManager creates execution threads needed for asynchronous
	 * message reception.
	 * */
	
	if ((dispatchThread = g_thread_create((GThreadFunc)&loadDispatcher,
										  (gpointer)tData, TRUE, &threadError)) == NULL)
	{
		*error = (char*)g_strconcat(DISPATCHERROR, "\nReturned value: ",
											(gchar*)dispatchThread,
											"\nError message: ",
											(gchar*)threadError->message,
											NULL);
		return (FALSE);
	}
	
	if ((receiveThread = g_thread_create((GThreadFunc)&loadAllReceivers,
										  (gpointer)tData, TRUE, &threadError)) == NULL)
	{
		*error = (char*)g_strconcat(RECEIVEERROR, "\nReturned value: ",
											(gchar*)receiveThread,
											"\nError message: ",
											(gchar*)threadError->message,
											NULL);
		return (FALSE);
	}
	
	/* Waits for both threads if the process was initialized by a server,
	 * not a client.
	 * Currently, returned values are ignored.
	 * */
	if (g_str_equal(behaviour, "server") == 0)
	{
		g_thread_join(receiveThread);
		g_thread_join(dispatchThread);
	}
	
	return (TRUE);
}

gboolean
writeMessage				(gchar* dest, gchar* proto, gchar* msg, gchar** error)
{
	Message* message;
	const gchar* (*pluginProto) ();
	
	/*message = g_malloc0(sizeof(Message));
	message->proto = proto;
	message->dest = dest;*/
	
	/* TODO: source */
	
	return (TRUE);
}

gboolean
readMessage					(gchar** msg, gchar** error)
{
	return (TRUE);
}

/* Funcion closeComSystem
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * NOT IMPLEMENTED
 * */
gboolean
closeBaseSystem				(gchar** error)
{
	return (TRUE);
}

/*
 * VERSION 0.1
 * */
