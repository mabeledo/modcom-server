/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: dispatchmanager.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "data.h"

#include "dispatchmanager.h"

/* Error messages */
#define CANNOTSENDDATA			"Imposible enviar datos con el complemento"

/* Funcion initDispatcher
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initDispatcher					(GData** dispatchConfig, gchar** error)
{
	return (TRUE);
}

/* Funcion loadDispatcher
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
loadDispatcher					(GQueue* qPlugins, GAsyncQueue* qMessages, gchar** error)
{
	GData* dispatchers;
	GSList* threads;
	Message* data;
	Plugin* aux;
	gint length, i;
	
	gboolean (*sendFunc) (gpointer data, gchar** error);
	gchar** funcError;
	
	g_datalist_init(&dispatchers);
	threads = NULL;
	length = (gint)g_queue_get_length(qPlugins);
	
	/* Fill the GData structure with plugin descriptions and pointers. */
	for (i = 0; i < length; i++)
	{
		aux = (Plugin*)g_queue_peek_nth(qPlugins, i);
		g_datalist_id_set_data(&dispatchers, (GQuark)aux->pluginType(), aux->pluginSend);
		threads = g_slist_append(threads, aux->receiveThread);
	}
	
	/* Keeps sending data while threads are active or queue is not empty. */
	while ((g_slist_length(threads) > 0) || (g_async_queue_length(qMessages) > 0))
	{
		data = g_async_queue_pop(qMessages);
		sendFunc = g_datalist_id_get_data(&dispatchers, (GQuark)data->destProto);
		
		if (!sendFunc((gpointer)data, funcError))
		{
			g_warning("%s: %s", CANNOTSENDDATA, *funcError);
		}
	}
	
	return (TRUE);
}
