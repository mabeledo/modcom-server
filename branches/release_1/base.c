/*******************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: base.c
 * 	Version: 1.0
 *
 *  Application entry point. All operations are initialized and managed
 *  here, so base.c is a interface to the "view" side of the
 *  application.
 * 
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "general.h"
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
static GThread* dispatchThread;
static gpointer dispRetData;
static GThread* receiveThread;
static gpointer recvRetData;
static ExtThreadData* etData;

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
	GData* dPlugins;
	
	GData* dConfig;
	GData* baseConfig;
	GData* pluginConfig;
	GData* receiveConfig;
	GData* dispatchConfig;
	GData* pluginSetConfig;

	GError* threadError;
	gchar *behaviour, *closeError;
	gboolean exitFlag;
	
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
	if (!(initConfig(configFile, error) &&
	      loadConfig(configFile, &dConfig, error)))
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
	
	/* Free memory containing configuration patterns already used. */
	g_datalist_remove_data(&dConfig, "base");
	g_datalist_remove_data(&dConfig, "plugins");
	g_datalist_remove_data(&dConfig, "receive");
	g_datalist_remove_data(&dConfig, "dispatch");
	
	pluginSetConfig = dConfig;
	
	/* Inicializa la cola de plugins y de mensajes. */
	etData = g_new0(ExtThreadData, 1);
	
	g_datalist_init(&dPlugins);
	etData->dPlugins = &dPlugins;
	
	etData->tData = g_new0(ThreadData, 1);
	
	etData->tData->qMessages = g_async_queue_new();
	etData->tData->mutex = g_mutex_new();
	exitFlag = FALSE;
	etData->tData->exitFlag = &exitFlag;
	
	/* Se inicializan las estructuras de plugins.
	 *  - Explora el directorio de plugins y carga los ficheros.
	 *  - Carga las funciones en las estructuras de plugin correspondientes.
	 * */
	if (!(initPlugins(&pluginConfig, error) && 
		  loadAllPlugins(etData->dPlugins, &dConfig, error)))
	{
		return (FALSE);
	}

	/* Dispatcher manages the message dispatching process.
	 * Receiveer creates execution threads needed for asynchronous
	 * message reception.
	 * */
	if (!(initReceivers(&receiveConfig, error) &&
		  initDispatcher(&dispatchConfig, error)))
	{
		return (FALSE);
	}
	
	g_datalist_clear(&baseConfig);
	g_datalist_clear(&dConfig);
	
	if ((dispatchThread = g_thread_create((GThreadFunc)&loadDispatcher,
										  (gpointer)etData, TRUE, &threadError)) == NULL)
	{
		*error = (char*)g_strconcat(DISPATCHERROR, "\nReturned value: ",
											(gchar*)dispatchThread,
											"\nError message: ",
											(gchar*)threadError->message,
											NULL);
		return (FALSE);
	}
	
	if ((receiveThread = g_thread_create((GThreadFunc)&loadAllReceivers,
										  (gpointer)etData, TRUE, &threadError)) == NULL)
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
	if (g_str_equal(behaviour, "server"))
	{
		recvRetData = g_thread_join(receiveThread);
		dispRetData = g_thread_join(dispatchThread);
	}
	
	/* Clean all and exit. */
	if (!closeBaseSystem(error))
	{
		return (FALSE);
	}
	
	return (TRUE);
}

/* Funcion closeBaseSystem
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso:
 * */
gboolean
closeBaseSystem				(gchar** error)
{	
	/* Set the thread exit flag to TRUE. */
	g_atomic_int_set(etData->tData->exitFlag, TRUE);
	g_usleep(5000000);
	
	g_debug("End dispatching process");
	g_debug("End receiving process");
	
	/* Close all software modules in reverse order. */
	return (TRUE);
}

/*
 * VERSION 0.1
 * */
