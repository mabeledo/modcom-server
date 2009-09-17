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

#include "base.h"
#include "config.h"
#include "loader.h"
#include "receiver.h"
#include "dispatcher.h"

#define MODULENAME			"Base"

/* Error messages */
#define THREADSNOTSUPPORTED	"Threads not supported"
#define DISPATCHERROR		"Unable to initialize the dispatcher"
#define RECEIVEERROR		"Unable to initialize all receivers"

/* Global variables. */
static GData* dPlugins;
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
	GData* dConfig;
	GData* baseConfig;
	GData* pluginConfig;
	GData* receiveConfig;
	GData* dispatchConfig;
	GData* pluginSetConfig;

	GError* threadError;
	gchar *behaviour;
	gboolean exitFlag;
	
	#ifdef G_THREADS_ENABLED
		if (!g_thread_supported())
		{
			g_thread_init(NULL);
		}
	#else
		*error = g_strconcat(MODULENAME, " - ", THREADSNOTSUPPORTED, NULL);
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
	
	/* Gets per module configuration settings and
	 * Free memory containing configuration patterns already used.
	 * */
	baseConfig = g_datalist_get_data(&dConfig, "base");
	g_datalist_remove_data(&dConfig, "base");
	pluginConfig = g_datalist_get_data(&dConfig, "plugins");
	g_datalist_remove_data(&dConfig, "plugins");
	receiveConfig = g_datalist_get_data(&dConfig, "receive");
	g_datalist_remove_data(&dConfig, "receive");
	dispatchConfig = g_datalist_get_data(&dConfig, "dispatch");
	g_datalist_remove_data(&dConfig, "dispatch");
	
	/* Fill base module configure options. */
	behaviour = (gchar*)g_datalist_get_data(&baseConfig, "behaviour");
	
	pluginSetConfig = dConfig;
	
	/* Inicializa la cola de plugins y de mensajes. */
	etData = g_new0(ExtThreadData, 1);
	
	g_datalist_init(&dPlugins);
	etData->dPlugins = &dPlugins;
	
	etData->tData = g_new0(ThreadData, 1);
	
	etData->tData->qMessages = g_async_queue_new();
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
	if (!(initDispatcher(&dispatchConfig, error) &&
		initReceivers(&receiveConfig, error)))
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
		g_error_free(threadError);
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
		g_error_free(threadError);
		return (FALSE);
	}

	/* Waits for both threads if the process was initialized by a server,
	 * not a client.
	 * */
	recvRetData = NULL;
	dispRetData = NULL;
	
	if (g_str_equal(behaviour, "server"))
	{
		if (((dispRetData = g_thread_join(dispatchThread)) != NULL) ||
			((recvRetData = g_thread_join(receiveThread)) != NULL))
		{
			g_debug("Dispatcher or receiver thread exited with errors");
			*error = g_strconcat("Dispatcher return message: ", (gchar*)dispRetData, "\n",
								 "Receiver return message: ", (gchar*)recvRetData, "\n" ,NULL);
			return (FALSE);
		}
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
	gchar* funcError;
	
	/* Set the thread exit flag to TRUE. */
	g_atomic_int_set(etData->tData->exitFlag, TRUE);
	g_debug("Waiting for threads to exit");
	g_usleep(500000);
	
	*error = NULL;

	if (!closeDispatcher(&funcError))
	{
		g_debug("Dispatch module closed with errors");
		*error = g_strconcat("Dispatch module: ", funcError, "\n", NULL);
	}
	else
	{
		g_debug("Dispatch module closed properly");
	}
	
	if (!closeReceivers(&funcError))
	{
		g_debug("Receive module closed with errors");
		*error = g_strconcat("Receive module: ", funcError, "\n", NULL);
	}
	else
	{
		g_debug("Receive module closed properly");
	}
	
	if (!closeAllPlugins(&dPlugins, &funcError))
	{
		g_debug("Plugins unloaded with errors");
		*error = g_strconcat("Load module: ", funcError, "\n", NULL);
	}
	else
	{
		g_debug("Plugins unloaded properly");
	}
	
	if (*error == NULL)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}
