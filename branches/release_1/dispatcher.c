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

#include "composer.h"

#define ROUTINGFILE			"routes.dat"
#define MSGLOGFILE			"msg.log"
#define WAITPERIOD			1000
#define DELIMITER			"||"
#define EOL					"\n"

/* Error messages */
#define CANNOTLOCATEROUTEFILE	"Imposible encontrar archivo de rutas"
#define CANNOTOPENMSGLOGFILE	"Imposible abrir el archivo de registro de mensajes"
#define CANNOTOPENROUTINGFILE	"Imposible abrir el archivo de enrutado"
#define CANNOTSENDDATA			"Imposible enviar datos con el complemento"
#define CANNOTWRITEDATA			"Imposible grabar datos en el fichero"
#define CANNOTLOADDISPATCHER	"Imposible enviar datos con el complemento"
#define MSGWRITEONLOG			"Mensaje escrito en el registro"

/* Module type definitions. */
typedef struct _RoutingEntry
{
	gchar* msgProto;
	GPatternSpec* msgAddrPattern;
	gchar* destProto;
	gchar* destAddress;
	
} RoutingEntry;

/* Global variables. */
static GQueue* routingTable;
static GIOChannel* msgLog;

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
	gchar *routingFile, *msgLogFile, *buffer, **bufferSet;
	GIOChannel* routing;
	RoutingEntry* entry;
	GError* channelError;

	channelError = NULL;
	
	/* Checks for routing file. 
	 * */
	if ((routingFile = g_datalist_get_data(dispatchConfig, "routing")) == NULL)
	{
		routingFile = ROUTINGFILE;
	}
	
	if (!g_file_test(routingFile, G_FILE_TEST_EXISTS))
	{
		*error = g_strdup(CANNOTLOCATEROUTEFILE);
		return (FALSE);
	}
	
	/* Opens a GIOChannel to read whatever is into the file. */
	if ((routing = g_io_channel_new_file(routingFile, "r", &channelError)) == NULL)
	{
		if (channelError != NULL)
		{
			*error = g_strconcat(CANNOTOPENROUTINGFILE, ": ", channelError->message, NULL);
		}
		else
		{
			*error = g_strconcat(CANNOTOPENROUTINGFILE, ": ", NOERRORAVAILABLE, NULL);
		}
		
		return (FALSE);
	}
	
	/* Initialize routing table (queue). */
	routingTable = g_queue_new();

	while (g_io_channel_read_line(routing, &buffer, NULL, NULL, &channelError) == G_IO_STATUS_NORMAL)
	{
		/* Ignore commented (# prefixed) and empty lines. */
		if (!g_str_has_prefix(buffer, "#"))
		{
			bufferSet = g_strsplit(buffer, " ", 4);
			if (g_strv_length(bufferSet) == (guint)4)
			{
				entry = g_slice_new0(RoutingEntry);
				
				entry->msgProto = g_strdup(bufferSet[0]);
				entry->msgAddrPattern = g_pattern_spec_new(bufferSet[1]);
				entry->destProto = g_strdup(bufferSet[2]);
				entry->destAddress = g_strdup(bufferSet[3]);
				
				g_queue_push_tail(routingTable, entry);
			}
		}
	}
	
	/* TODO: warning message for previous error. */

	/* Close routing IO channel. */
	if (g_io_channel_shutdown(routing, FALSE, &channelError) == (G_IO_STATUS_ERROR | G_IO_STATUS_AGAIN))
	{
		*error = g_strconcat(CANNOTOPENROUTINGFILE, ": ", channelError->message, NULL);
		return (FALSE);
	}
	
	/* Check for message logging file. */
	if ((msgLogFile = g_datalist_get_data(dispatchConfig, "msglog")) == NULL)
	{
		msgLogFile = MSGLOGFILE;
	}
	
	if ((msgLog = g_io_channel_new_file(msgLogFile, "w+", &channelError)) == NULL)
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
	Plugin* plugin;
	RoutingEntry* entry;
	GData** dPlugins;
	GAsyncQueue* qMessages;
	gint tableLength, i;

	GError* ioError;
	gchar** funcError;
	
	tData = data;
	dPlugins = tData->dPlugins;
	qMessages = tData->qMessages;
	tableLength = g_queue_get_length(routingTable);
	
	/* Now the dispatcher is fully functional. */
	g_debug("Dispatcher up & running");

	/* Keeps sending data */
	while (dPlugins != NULL)
	{
		/* Gets a new message to dispatch.
		 * Trying to pop a message is actually more efficient than
		 * search for at least one element in the queue.
		 * */
		if ((msg = (Message*)g_async_queue_try_pop(qMessages)) != NULL)
		{	
			/* Chooses a default plugin using the original message protocol. */
			plugin = g_datalist_get_data(dPlugins, msg->proto);
			
			/* Checks if the message has already reached its destination.
			 * - If it has, it will be logged into a file.
			 * - If it has not reached its destination and there is not a
			 *   'route' defined for it, dispatch.
			 * - If it has not reached its destination and there is a
			 *   'route' defined for it, dispatch it through that way.
			 * */
			if (g_str_equal(msg->dest, plugin->pluginAddress()))
			{
				ioError = NULL;
				
				/* Write to disk cache. */
				if (!((g_io_channel_write_chars(msgLog,
						g_strconcat(msg->proto, DELIMITER, msg->src, DELIMITER, msg->data, EOL, NULL),
						-1, NULL,
						&ioError) == (G_IO_STATUS_ERROR | G_IO_STATUS_AGAIN)) &&
					(g_io_channel_flush(msgLog,
						&ioError) == (G_IO_STATUS_ERROR | G_IO_STATUS_AGAIN))))
				{
					if (ioError != NULL)
					{
						g_warning("%s: %s", CANNOTWRITEDATA, ioError->message);
					}
					else
					{
						g_warning("%s: %s", CANNOTWRITEDATA, NOERRORAVAILABLE);
					}
				}
				else
				{
					g_debug(MSGWRITEONLOG);
				}
			}
			else
			{	
				i = 0;
				
				/* Search for a route in the route table. */
				while (i < tableLength)
				{
					entry = g_queue_peek_nth(routingTable, i);
					
					if (g_str_equal(entry->msgProto, msg->proto) &&
						g_pattern_match_string(entry->msgAddrPattern, msg->dest))
					{
						plugin = g_datalist_get_data(dPlugins, entry->destProto);
						i = tableLength;	
					}
					else
					{
						i++;
					}
				}

				if (!plugin->pluginSend((gpointer)msg->dest, (gpointer)msg, funcError))
				{
					g_warning("%s: %s", CANNOTSENDDATA, *funcError);
				}		
			}
		}
		else
		{
			g_usleep(WAITPERIOD);
		}
	}

	g_debug("End dispatching process");
	return (NULL);
}
