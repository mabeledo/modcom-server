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

static GAsyncQueue* qMessages;
static GData* addresses;

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
	gint i, length;
	Plugin* aux;
	
	g_datalist_init(&addresses);
	
	length = (gint)g_queue_get_length(qPlugins);
	
	for (i = 0 ; i < length; i++)
	{		
		aux = g_queue_peek_nth(qPlugins, i);
		g_datalist_set_data(&addresses, (gchar*)aux->pluginProto(), (gchar*)aux->pluginAddress());
	}

	qMessages = msgs;

	return (TRUE);
}


/* Funcion createMsgId
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
static gushort
createMsgId					()
{
	gushort id;
	
	id = 0;
	
	return (id);
}



/* Funcion writeMessage
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
writeMessage				(const gchar* proto, const gchar* dest, const gchar* data, gchar** error)
{
	Message* msg;
	const gchar* address;

	address = g_datalist_get_data(&addresses, proto);
	
	msg = g_malloc0(sizeof(Message));
	
	msg->proto = proto;
	msg->dest = dest;
	
	msg->src = g_datalist_get_data(&addresses, proto);
	msg->type = 0;

	msg->id = createMsgId();
	
	msg->part = 0;
	msg->data = g_strdup(data);
	msg->checksum = (const gchar*)g_compute_checksum_for_string
									(G_CHECKSUM_MD5,
							 		data,
							 		-1);
	
	g_async_queue_push(qMessages, msg);
	
	g_debug("Message pushed into the queue");
	
	/* Free data. */
	g_free(msg);
		
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
	
	g_sprintf(data, "%s  %s  %s  %s", msg->proto, msg->src, msg->dest, msg->data);
	
	return (TRUE);
}
