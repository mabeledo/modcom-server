/*******************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: basemanager.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "general.h"
#include "thread.h"

#include "configmanager.h"
#include "pluginmanager.h"
#include "receivemanager.h"
#include "dispatchmanager.h"
#include "basemanager.h"

/* Error messages */
#define THREADSNOTSUPPORTED	"Threads not supported"
#define DISPATCHERROR		"Unable to initialize the dispatcher"
#define RECEIVEERROR		"Unable to initialize all receivers"

/* Funcion openComSystem
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
	GData* pluginConfig;
	GData* moduleConfig;
	GData* dispatchConfig;
	GData* pluginSetConfig;
	
	GQueue* qPlugins;
	GAsyncQueue* qMessages;
	ThreadData* tData;
	
	GThread* dispatchThread;
	GThread* receiveThread;
	GError* threadError;
	
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
	pluginConfig = g_datalist_get_data(&dConfig, "plugins");
	dispatchConfig = g_datalist_get_data(&dConfig, "dispatch");
	
	/* *
	 * Segunda fase (comprobacion y ejecucion de procesos):
	 *  - Existe el directorio de los complementos.
	 *  - El sistema puede cargar modulos dinamicamente.
	 *  - El sistema puede utilizar hilos de ejecucion.
	 *  - (Initdispatcher)
	 * */
	if (!(initPlugins(&pluginConfig, error) &&
		  initReceivers(error) &&
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
	
	/* Waits for both threads.
	 * Currently, returned values are ignored.
	 * */
	g_thread_join(receiveThread);
	g_thread_join(dispatchThread);
	
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
