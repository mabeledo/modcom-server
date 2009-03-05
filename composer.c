/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: composer.c
 * 	Version: 0.1
 *  
 *  Currently the composer module does not check for invalid protocols,
 *  i.e. protocols that are not supported by any plugin in the system.
 * 
 * 
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "general.h"
#include "msg.h"
#include "plugin.h"

#define CANNOTLOCATEMSGLOGFILE	"Imposible encontrar archivo de registro de mensajes"
#define CANNOTOPENMSGLOGFILE	"Imposible abrir el archivo de registro de mensajes"
#define CANNOTREADDATA			"Imposible leer datos del fichero"
#define CANNOTFORMATMSG			"Imposible formatear mensaje"

#define MSGLOGFILE			"msg.log"
#define DELIMITER			"||"

static GAsyncQueue* qMessages;
static GData** dPlugins;
static GIOChannel* msgLog;

/* Funcion initComposer
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
initComposer				(GData** composerConfig, gchar** error)
{	
	gchar* msgLogFile;
	GError* channelError;

	channelError = NULL;
	
	/* Checks for message logging file. */
	if ((msgLogFile = g_datalist_get_data(composerConfig, "msglog")) == NULL)
	{
		msgLogFile = MSGLOGFILE;
	}
	
	if (!g_file_test(msgLogFile, G_FILE_TEST_EXISTS))
	{
		*error = g_strdup(CANNOTLOCATEMSGLOGFILE);
		return (FALSE);
	}
	
	/* Opens a GIOChannel to read whatever is into the file. */
	if ((msgLog = g_io_channel_new_file(msgLogFile, "r", &channelError)) == NULL)
	{
		if (channelError != NULL)
		{
			*error = g_strconcat(CANNOTOPENMSGLOGFILE, ": ", channelError->message, NULL);
		}
		else
		{
			*error = g_strconcat(CANNOTOPENMSGLOGFILE, ": ", NOERRORAVAILABLE, NULL);
		}
		
		return (FALSE);
	}
	
	return (TRUE);
}

/* Funcion loadComposer
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
loadComposer				(GData** plugins, GAsyncQueue* msgs, gchar** error)
{
	dPlugins = plugins;
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
	Plugin* aux;
	
	aux = (Plugin*)g_datalist_get_data(dPlugins, proto);
	msg = g_malloc0(sizeof(Message));
	
	msg->proto = proto;
	msg->dest = dest;
	msg->src = aux->pluginAddress();
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
readMessage					(gchar** data, gchar** error)
{	
	gchar* line;
	gchar** split;
	GError* ioError;
	
	ioError = NULL;
	
	if (g_io_channel_read_line(msgLog, &line, NULL, NULL, &ioError) == 
								(G_IO_STATUS_ERROR | G_IO_STATUS_AGAIN))
	{
		if (ioError != NULL)
		{
			*error = g_strconcat(CANNOTREADDATA, ": ", ioError->message, NULL);
		}
		else
		{
			*error = g_strconcat(CANNOTREADDATA, ": ", NOERRORAVAILABLE, NULL);
		}
		
		return (FALSE);	
	}
	
	if (line != NULL)
	{
		split = g_strsplit(line, DELIMITER, 0);
		
		if ((*data 
				= g_strdup_printf("Protocol: %s Source: %s Message: %s", 
									split[0], split[1], split[2])) == NULL)
		{
			*error = g_strdup(CANNOTFORMATMSG);
			return (FALSE);
		}
	}
	
	return (TRUE);
}

/* Funcion readQueuedMessage
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
readQueuedMessage			(gchar** data, gchar** error)
{
	Message* msg;
	
	msg = g_async_queue_pop(qMessages);
	
	g_sprintf(*data, "%s  %s  %s  %s", msg->proto, msg->src, msg->dest, msg->data);
	
	return (TRUE);
}
