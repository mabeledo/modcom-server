/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: dispatcher.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "general.h"
#include "thread.h"
#include "msg.h"
#include "plugin.h"

/* Error messages */
#define CANNOTSENDDATA			"Imposible enviar datos con el complemento"
#define CANNOTLOADDISPATCHER	"Imposible enviar datos con el complemento"

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
gpointer
loadDispatcher					(gpointer data)
{
	ThreadData* tData;
	Message* msg;
	Plugin* aux;
	gchar** funcError;
	GData* dPlugins;
	
	tData = data;
	
	/* Now the dispatcher is fully functional. */
	g_debug("Dispatcher up & running");

	/* Keeps sending data */
	while ((tData->dPlugins != NULL) || (g_async_queue_length(tData->qMessages) > 0))
	{
		if (g_async_queue_length(tData->qMessages) > 0)
		{
			msg = g_async_queue_pop(tData->qMessages);
			
			aux = g_datalist_get_data(tData->dPlugins, msg->proto);
			
			if (!aux->pluginSend((gpointer)msg->dest, (gpointer)msg, funcError))
			{
				g_warning("%s: %s", CANNOTSENDDATA, *funcError);
			}
		}	
	}

	return (NULL);
}
