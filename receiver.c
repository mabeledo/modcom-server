/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: receivemanager.c
 * 	Version: 0.1
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
	GAsyncQueue* qMessages;
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
	GError* threadError = NULL;

	plugin = (Plugin*)data;
	fData = (ForeachData*)user_data;

	/* Creacion del hilo de recepcion, almacenamiento de la referencia en la cola */
	plugin->receiveThread = g_thread_create(plugin->pluginReceive, (gpointer)fData->qMessages, TRUE, &threadError);

	if (threadError != NULL)
	{
		g_warning("%s. Plugin: %s. Error: %s.", CANNOTCREATETHREAD,
												plugin->pluginName(),
												threadError->message);
		g_error_free(threadError);
		g_free(plugin->receiveThread);
	}
	else
	{
		g_ptr_array_add(fData->pThreads, (gpointer)plugin->receiveThread);
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
	ThreadData* tData;
	ForeachData* fData;
	GData** dPlugins;
	gint i;
	
	tData = data;
	dPlugins = tData->dPlugins;
	fData = g_new0(ForeachData, 1);
	fData->qMessages = tData->qMessages;
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
	
	g_debug("End receiving process");
	return (NULL);
}
