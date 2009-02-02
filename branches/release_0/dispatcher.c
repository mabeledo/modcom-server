/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: dispatchmanager.c
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
	GData* dispatchers;
	ThreadData* tData;
	Message* msg;
	Plugin* aux;
	gint length, i;
	
	gboolean (*sendFunc) (gpointer data, gchar** error);
	gchar** funcError;
	
	tData = data;
	
	g_datalist_init(&dispatchers);
	length = (gint)g_queue_get_length(tData->qPlugins);
	
	/* Fill the GData structure with plugin descriptions and pointers. */
	for (i = 0; i < length; i++)
	{
		aux = (Plugin*)g_queue_peek_nth(tData->qPlugins, i);
		g_datalist_id_set_data(&dispatchers, (GQuark)aux->pluginType(), aux->pluginSend);
	}
	
	/* Now the dispatcher is fully functional. */
	g_debug("Dispatcher up & running");
	
	/* Keeps sending data */
	while ((g_queue_get_length(tData->qPlugins) > 0) || (g_async_queue_length(tData->qMessages) > 0))
	{
		msg = g_async_queue_pop(tData->qMessages);

		sendFunc = g_datalist_id_get_data(&dispatchers, (GQuark)msg->destProto);
		
		if (!sendFunc((gpointer)msg, funcError))
		{
			g_warning("%s: %s", CANNOTSENDDATA, *funcError);
		}
	}

	return (NULL);
}
