/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: dispatcher.c
 * 	Version: 1.0
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "general.h"
#include "thread.h"
#include "msg.h"
#include "plugin.h"

#define ROUTINGFILE			"routes.dat"
#define WAITPERIOD			100

/* Error messages */
#define CANNOTLOCATEROUTEFILE	"Imposible encontrar archivo de rutas"
#define CANNOTOPENROUTINGFILE	"Imposible abrir el archivo de enrutado"
#define CANNOTWRITEDATA			"Imposible grabar datos en el fichero"
#define CANNOTLOADDISPATCHER	"Imposible enviar datos con el complemento"
#define	CANNOTCLOSEDISPATCHER	"Cannot close dispatcher properly"

/* Module type definitions. */
typedef struct _RoutingEntry
{
	gchar* srcProto;
	GPatternSpec* srcAddrPattern;
	gchar* destProto;
	gchar* destAddress;
	
} RoutingEntry;

/* Global variables. */
static GQueue* routingTable;

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
	gchar *routingFile, *buffer, **bufferSet;
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
			g_clear_error(&channelError);
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
				
				entry->srcProto = g_strdup(bufferSet[0]);
				entry->srcAddrPattern = g_pattern_spec_new(bufferSet[1]);
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
		g_error_free(channelError);
		return (FALSE);
	}
	
	/* Free memory and exit. */
	g_datalist_clear(dispatchConfig);
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
	ExtThreadData* etData;
	GData** dPlugins;
	GAsyncQueue* qMessages;
	gboolean* exitFlag;
	
	Message* msg;
	Plugin* plugin;
	RoutingEntry* entry;
	
	gint tableLength, i;
	gchar *funcError;
	gboolean found;
	
	etData = data;
	dPlugins = etData->dPlugins;
	qMessages = g_async_queue_ref((GAsyncQueue*)etData->tData->qMessages);
	exitFlag = etData->tData->exitFlag;
	tableLength = g_queue_get_length(routingTable);
	
	/* Now the dispatcher is fully functional. */
	g_debug("Dispatcher up & running");

	/* Keeps sending data */
	while ((dPlugins != NULL) && (!g_atomic_int_get(exitFlag)))
	{
		/* Gets a new message to dispatch.
		 * Trying to pop a message is actually more efficient than
		 * search for at least one element in the queue.
		 * */
		if ((msg = (Message*)g_async_queue_try_pop(qMessages)) != NULL)
		{
			i = 0;
			found = FALSE;

			/* Search for a route in the route table. */
			while ((i < tableLength) && (found == FALSE))
			{
				entry = g_queue_peek_nth(routingTable, i);
				
				if (g_str_equal(entry->srcProto, msg->srcProto) &&
					g_pattern_match_string(entry->srcAddrPattern, msg->srcAddress))
				{
					plugin = g_datalist_get_data(dPlugins, entry->destProto);
					found = TRUE;
				}
				else
				{
					i++;
				}
			}
			
			// TODO: better error handling
			if (found == FALSE)
			{
				g_critical("PLUGIN not found!");
			}
			
			if (!plugin->pluginSend((gpointer)entry->destAddress, (gpointer)msg, &funcError))
			{
				g_warning("%s", funcError);
				g_free((gpointer)funcError);
			}
		}
		else
		{
			g_usleep(WAITPERIOD);
		}
	}

	/* Free memory. */	
	g_async_queue_unref(qMessages);

	return (NULL);
}

/* Funcion closeDispatcher
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
closeDispatcher					(gchar** error)
{
	g_queue_free(routingTable);
	
	/* TODO: free patterspec? */
	return (TRUE);
}
