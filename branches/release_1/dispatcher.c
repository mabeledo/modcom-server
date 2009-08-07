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

#define ROUTINGFILE				"routes.dat"
#define WAITPERIOD				1000
#define MODULENAME				"Dispatcher"


/* Error messages */
#define CANNOTLOCATEROUTEFILE	"Cannot locate routes file"
#define CANNOTOPENROUTINGFILE	"Cannot open routes file"
#define CANNOTWRITEDATA			"Imposible grabar datos en el fichero"
#define CANNOTLOADDISPATCHER	"Imposible enviar datos con el complemento"
#define	CANNOTCLOSEDISPATCHER	"Cannot close dispatcher properly"
#define PLUGINNOTFOUND			"Plugin not found"
#define BADENTRYFORMAT			"Bad configuration entry format"

/* Module type definitions. */
typedef struct _RoutingEntry
{
	gchar* srcProto;
	GPatternSpec* srcAddrPattern;
	gchar* destProto;
	gchar* destAddress;
	
} RoutingEntry;

/* Global variables. */
static RoutingEntry** routingTable;
static gint routingTableLength;

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
	gchar *routingFile, *buffer, **bufferEntry, **bufferDest;
	GIOChannel* routing;
	RoutingEntry* entry;
	GError* channelError;
	gint i, bufferLen;

	channelError = NULL;
	
	/* Checks for routing file. 
	 * */
	if ((routingFile = g_datalist_get_data(dispatchConfig, "routing")) == NULL)
	{
		routingFile = ROUTINGFILE;
	}
	
	if (!g_file_test(routingFile, G_FILE_TEST_EXISTS))
	{
		*error = g_strconcat(MODULENAME, " - ", CANNOTLOCATEROUTEFILE, NULL);
		return (FALSE);
	}
	
	/* Opens a GIOChannel to read whatever is into the file. */
	if ((routing = g_io_channel_new_file(routingFile, "r", &channelError)) == NULL)
	{
		if (channelError != NULL)
		{
			*error = g_strconcat(MODULENAME, " - ", CANNOTOPENROUTINGFILE, ": ", channelError->message, NULL);
			g_clear_error(&channelError);
		}
		else
		{
			*error = g_strconcat(MODULENAME, " - ", CANNOTOPENROUTINGFILE, ": ", NOERRORAVAILABLE, NULL);
		}
		
		return (FALSE);
	}

	routingTableLength = 0;
	routingTable = g_malloc(sizeof(RoutingEntry**));

	/* Read the routing file and create a routing table. */
	while (g_io_channel_read_line(routing, &buffer, NULL, NULL, &channelError) == G_IO_STATUS_NORMAL)
	{
		/* Ignore commented (# prefixed) and empty lines. */
		if (!(g_str_has_prefix(buffer, "#") || g_str_equal(buffer, "\n")))
		{
			bufferEntry = g_strsplit(buffer, " ", 4);
			if (g_strv_length(bufferEntry) == (guint)4)
			{
				/* Each destination address has its own entry in the routing table. */
				bufferDest = g_strsplit(bufferEntry[3], ",", 0);
				bufferLen = g_strv_length(bufferDest);
				
				for (i = 0; i < bufferLen; i++)
				{
					entry = g_slice_new0(RoutingEntry);
					
					entry->srcProto = g_strdup(bufferEntry[0]);
					entry->srcAddrPattern = g_pattern_spec_new(bufferEntry[1]);
					entry->destProto = g_strdup(bufferEntry[2]);				
					entry->destAddress = g_strdup(bufferDest[i]);

					routingTable[routingTableLength] = entry;
					routingTableLength++;
				}					
				
				g_strfreev(bufferEntry);
				g_strfreev(bufferDest);
			}
			else
			{
				g_warning("%s - %s", MODULENAME, BADENTRYFORMAT);
			}
		}
	}

	/* Close routing IO channel. */
	if (g_io_channel_shutdown(routing, FALSE, &channelError) == (G_IO_STATUS_ERROR | G_IO_STATUS_AGAIN))
	{
		*error = g_strconcat(MODULENAME, " - ", CANNOTOPENROUTINGFILE, ": ", channelError->message, NULL);
		g_error_free(channelError);
		return (FALSE);
	}
	
	/* TODO: manage error for empty routing table */
	
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
	
	gint i;
	gboolean found;
	gchar *funcError;
	
	etData = data;
	dPlugins = etData->dPlugins;
	qMessages = g_async_queue_ref((GAsyncQueue*)etData->tData->qMessages);
	exitFlag = etData->tData->exitFlag;
	
	/* Now the dispatcher is fully functional. */
	g_debug("%s - Up & running", MODULENAME);

	/* Keeps sending data */
	while (!g_atomic_int_get(exitFlag))
	{
		/* Gets a new message to dispatch.
		 * Trying to pop a message is actually more efficient than
		 * search for at least one element in the queue.
		 * */
		if ((msg = (Message*)g_async_queue_try_pop(qMessages)) != NULL)
		{
			/* Search for any route in the route table. */
			for (i = 0; i < routingTableLength; i++)
			{
				if (g_str_equal(routingTable[i]->srcProto, msg->srcProto) &&
					g_pattern_match_string(routingTable[i]->srcAddrPattern, msg->srcAddress))
				{
					found = TRUE;
					plugin = g_datalist_get_data(dPlugins, routingTable[i]->destProto);
					
					if (!plugin->pluginSend((gpointer)routingTable[i]->destAddress, (gpointer)msg, &funcError))
					{
						g_warning("%s", funcError);
						g_free((gpointer)funcError);
					}					
				}
			}
			
			g_free((gpointer)msg->srcProto);
			g_free((gpointer)msg->srcAddress);
			g_free(msg->chunk);
			g_free(msg);
			
			// TODO: better error handling
			if (!found)
			{
				//g_atomic_int_set(exitFlag, TRUE);
				g_warning("%s - %s", MODULENAME, PLUGINNOTFOUND);
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
	g_free(routingTable);
	
	/* TODO: free patterspec? */
	return (TRUE);
}
