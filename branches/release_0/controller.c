/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: controller.c
 * 	Version: 0.1
 *  
 *  Currently the controller module does not check for invalid protocols,
 *  i.e. protocols that are not supported by any plugin in the system.
 * 
 * 
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "general.h"
#include "msg.h"
#include "plugin.h"

/* This constant should not be here */

static GAsyncQueue* qMessages;
static GData* addresses;
static gchar* protocols[] = PROTOCOLS;

/* Funcion initController
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
initController				(GData** controlConfig, gchar** error)
{	
	return (TRUE);
}

/* Funcion loadController
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
loadController				(GQueue* qPlugins, GAsyncQueue* msgs, gchar** error)
{
	gint i, length, proto;
	gchar** protoAndSrc;
	Plugin* aux;
	
	g_datalist_init(&addresses);
	
	length = (gint)g_queue_get_length(qPlugins);
	
	for (i = 0 ; i < length; i++)
	{
		/* This is how it works.
		 * Each plugin is asked for its protocol code and local address.
		 * This data is packed in a bidimensional gchar array and inserted
		 * in a GData structure with a GQuark, created from the protocol
		 * name specified in the "protocols" array in msg.h.
		 * */
		aux = g_queue_peek_nth(qPlugins, i);
		proto = (gint)aux->pluginProto();
		g_sprintf(protoAndSrc[0], "%d", proto);
		protoAndSrc[1] = g_strdup(aux->pluginAddress());
		
		g_datalist_set_data(&addresses, protocols[proto], protoAndSrc);
	}

	qMessages = msgs;

	return (TRUE);
}

/* Funcion writeMessage
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
writeMessage				(gchar* proto, gchar* dest, gchar* data, gchar** error)
{
	Message* msg;
	gchar** protoAndSrc;
	
	protoAndSrc = g_datalist_get_data(&addresses, proto);

	msg = g_malloc0(sizeof(Message));
	msg->proto = (gushort)g_strtod(protoAndSrc[0], NULL);
	msg->dest = g_strdup(dest);
	msg->src = protoAndSrc[1];
	msg->type = 0;
	
	/* Should it be produced by an algoritm? */
	msg->id = 0;
	
	msg->part = 0;
	msg->data = g_strdup(data);

	g_async_queue_push(qMessages, msg);
		
	return (TRUE);
}

/* Funcion readMessage
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
readMessage					(gchar* data, gchar** error)
{
	Message* msg;
	
	msg = g_async_queue_pop(qMessages);
	
	g_sprintf(data, "%s  %s  %s  %s", protocols[(gint)msg->proto], msg->src, msg->dest, msg->data);
	
	return (TRUE);
}
