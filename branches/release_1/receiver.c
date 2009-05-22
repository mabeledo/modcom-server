/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: receivemanager.c
 * 	Version: 1.0
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "general.h"
#include "thread.h"
#include "plugin.h"

/* Error messages */
#define NOTENOUGHTHREADS		"No se ha creado ningun thread"
#define CANNOTCREATETHREAD		"No se ha podido crear el thread"

/* Local data definitions. */
typedef struct 
_ForeachData
{
	ThreadData* tData;
	GPtrArray* pThreads;
	gint counter;
} ForeachData;

/* Funcion initReceivers
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initReceivers					(GData** receiveConfig, gchar** error)
{
	
	/* Free memory and exit. */
	g_datalist_clear(receiveConfig);
	return (TRUE);
}

/* Funcion loadReceiver
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Complemento cargado en memoria, cola de mensajes.
 * Salida: Puntero a un mensaje de error.
 * Proceso: Inicia los hilos necesarios para un complemento.
 * */
static void
loadReceiver					(GQuark key, gpointer data, gpointer user_data)
{
	Plugin* plugin;
	ForeachData* fData;
	GThread* receiveThread;
	GError* threadError = NULL;

	plugin = (Plugin*)data;
	fData = (ForeachData*)user_data;

	/* Creacion del hilo de recepcion, almacenamiento de la referencia en la cola */
	receiveThread = g_thread_create(plugin->pluginReceive, (gpointer)fData->tData, TRUE, &threadError);

	if (threadError != NULL)
	{
		g_warning("%s. Plugin: %s. Error: %s.", CANNOTCREATETHREAD,
												plugin->pluginName(),
												threadError->message);
		g_error_free(threadError);
		g_free(receiveThread);
	}
	else
	{
		g_ptr_array_add(fData->pThreads, (gpointer)receiveThread);
		fData->counter++;
	}
	
	return;
}

/* Funcion loadAllReceivers
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Cola con los complementos cargados en memoria, cola de mensajes.
 * Salida: Cola de hilos creados, mensaje de error
 * Proceso: Inicia los hilos necesarios para cada uno de los complementos cargados.
 * */
gpointer
loadAllReceivers				(gpointer data)
{
	ExtThreadData* etData;
	ForeachData* fData;
	GData** dPlugins;
	gint i;

	etData = data;
	dPlugins = etData->dPlugins;
	fData = g_new0(ForeachData, 1);
	fData->tData = etData->tData;
	fData->pThreads = g_ptr_array_new();
	fData->counter = 0;
	
	/* Crea los hilos para las funciones de envío */
	g_datalist_foreach(dPlugins, loadReceiver, (gpointer)fData);
		
	g_debug("Receiver up & running");
	
	/* Waits for every thread previously created */
	if (fData->counter > 0)
	{
		for (i = 0; i < fData->counter; i++)
		{
			g_thread_join((GThread*)g_ptr_array_index(fData->pThreads, i));
		}
	}
	else
	{
		g_critical("%s", NOTENOUGHTHREADS);
	}

	return (NULL);
}

/* Funcion closeReceivers
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
closeReceivers					(gchar** error)
{
	return (TRUE);
}
