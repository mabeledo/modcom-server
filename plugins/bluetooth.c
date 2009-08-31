/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/bluetooth.c
 * 	Version: 1.0
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

/* Plugin main attributes. */
#define PLUGINPROTO			"BLUETOOTH"
#define PLUGINNAME			"Bluetooth"
#define PLUGINDESC			"Bluetooth plugin"
#define PLUGINVERSION		"1.0"

#include "common.h"
#include "bluetooth.h"

#ifdef G_OS_WIN32

#endif

#ifdef LINUX
	#include <errno.h>
	#include <unistd.h>
	#include <sys/socket.h>
	#include <bluetooth/bluetooth.h>
	#include <bluetooth/rfcomm.h>
#endif

/* Error messages. */
#define NODEVICEAVAILABLE	"There is no device available"
#define OPENDEVICEFAILED	"Error opening device"
#define PORTNOTVALID		"Port number not valid, falling back to default: "
#define CONVERSIONERROR		"Error converting to bluetooth address"
#define OPENCHANNELERROR	"Error opening channel"
#define CONNECTERROR		"connect() error"
#define CHANOPTERROR		"Error setting up channel"
#define CHANSHUTDOWNERROR	"Error closing channel"
#define SENDERROR			"Error sending chunk"
#define FLUSHERROR			"Error flushing data"
#define BINDERROR			"bind() error"
#define LISTENERROR			"listen() error"
#define ACCEPTERROR			"accept() error"
#define SOCKETERROR			"socket() error"
#define READERROR			"Error reading data from channel"
#define FDSHUTDOWNERROR		"Error closing descriptor"

#define PORT				13

/* Global variables. */
static gint listenPort = PORT;

gboolean
pluginInit							(gpointer data, gchar** error)
{
	GData* blueConfig = (GData*)data;
	
	if (g_datalist_get_data(&blueConfig, "port") != NULL)
	{
		listenPort = (gint)g_strtod((gchar*)g_datalist_get_data(&blueConfig, "port"), NULL);
		
		if ((listenPort < 1) || (listenPort > 30))
		{
			listenPort = PORT;
			g_warning("%s %d", PORTNOTVALID, PORT);
		}
	}
	
	/* Free memory. */
	g_datalist_clear(&blueConfig);

	g_debug("%s - Plugin loaded and initialized", PLUGINNAME);
	return (TRUE);
}

gboolean
pluginSend							(gpointer dest, gpointer data, gchar** error)
{
	Message* msg = (Message*)data;
	gchar* destAddress = (gchar*)dest;
	
	GIOChannel* channel;
	GError* chanError;
	GTimer* timeElapsed = g_timer_new();

	#ifdef LINUX
		struct sockaddr_rc addr;
		gint clientSd, addrLen;
		
		addrLen = sizeof(struct sockaddr_rc);
			
		if ((clientSd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0)
		{
			*error = g_strconcat(PLUGINNAME, " - ", SOCKETERROR, ": ", g_strerror(errno), NULL);
			return (FALSE);
		}
			
		if ((channel = g_io_channel_unix_new(clientSd)) == NULL)
		{
			*error = g_strconcat(PLUGINNAME, " - ", OPENCHANNELERROR, NULL);
			return (FALSE);
		}

		bzero(&addr, addrLen);
		addr.rc_family = AF_BLUETOOTH;
		str2ba(destAddress, &addr.rc_bdaddr);
		addr.rc_channel = (guint8)listenPort;
		
		/* Connects to server. */
		if (connect(clientSd, (struct sockaddr*)&addr, addrLen) != 0)
		{
			*error = g_strconcat(PLUGINNAME, " - ", CONNECTERROR, ": ", g_strerror(errno), NULL);
			return (FALSE);
		}
		
		chanError = NULL;
		
		/* Set a NULL encoding to read raw data. */
		if (g_io_channel_set_encoding(channel, NULL, &chanError) != G_IO_STATUS_NORMAL)
		{
			*error = g_strconcat(PLUGINNAME, " - ", CHANOPTERROR, ": ", chanError->message, NULL);
			return (FALSE);
		}
		
		g_timer_start(timeElapsed);

		/* Writes data on channel. */
		if (g_io_channel_write_chars(channel, msg->chunk, msg->chunkLen, NULL, &chanError) != G_IO_STATUS_NORMAL)
		{
			*error = g_strconcat(PLUGINNAME, " - ", SENDERROR, ": ", chanError->message, NULL);
			return (FALSE);
		}

		/* Flushes channel. */
		if (g_io_channel_flush(channel, &chanError) != G_IO_STATUS_NORMAL)
		{
			*error = g_strconcat(PLUGINNAME, " - ", FLUSHERROR, ": ", chanError->message, NULL);
			return (FALSE);
		}

		if (g_io_channel_shutdown(channel, TRUE, &chanError) != G_IO_STATUS_NORMAL)
		{
			*error = g_strconcat(PLUGINNAME, " - ", CHANSHUTDOWNERROR, ": ", chanError->message, NULL);
			return (FALSE);
		}
		
		g_timer_stop(timeElapsed);
		g_debug("%s - %d bytes sent in %lf seconds", PLUGINNAME, msg->chunkLen, g_timer_elapsed(timeElapsed, NULL));
		g_timer_destroy(timeElapsed);
		
		g_io_channel_unref(channel);
	#endif
	
	return (TRUE);
}

gpointer
pluginReceive						(gpointer data)
{
	ThreadData* tData;
	Message* msg;
	
	GIOChannel* channel;
	GIOStatus devStatus;
	GError* chanError;
	gchar *error, *buffer;
	GTimer* timeElapsed = g_timer_new();
	
	tData = data;
	tData->qMessages = g_async_queue_ref(tData->qMessages);
	
	#ifdef LINUX
		gint serverSd, clientSd, addrLen;
		struct sockaddr_rc addr;
		
		addrLen = sizeof(struct sockaddr_rc);
		
		if ((serverSd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0)
		{
			error = g_strconcat(PLUGINNAME, " - ", SOCKETERROR, ": ", g_strerror(errno), NULL);
			g_warning("%s", error);
			return ((gpointer)error);
		}

		bzero(&addr, addrLen);
		addr.rc_family = AF_BLUETOOTH;
		addr.rc_bdaddr = *BDADDR_ANY;
		addr.rc_channel = (guint8)listenPort;
		
		/* Binds socket to address */
		if (bind(serverSd, (struct sockaddr *)&addr, addrLen) != 0)
		{
			error = g_strconcat(PLUGINNAME, " - ", BINDERROR, ": ", g_strerror(errno), NULL);
			g_warning("%s", error);
			return ((gpointer)error);
		}
		
		/* Sets a waiting socket with 10 wait slots */
		if (listen(serverSd, 10) != 0)
		{
			error = g_strconcat(PLUGINNAME, " - ", LISTENERROR, ": ", g_strerror(errno), NULL);
			g_warning("%s", error);
			return ((gpointer)error);
		}
		
		g_debug("%s - Accepting new connections", PLUGINNAME);
		
		/* Loop accepting incoming connections. */
		while (!g_atomic_int_get(tData->exitFlag))
		{
			/* Accepts client connections */
			clientSd = accept(serverSd, (struct sockaddr *)&addr, (guint32 *)&addrLen);

			if (clientSd <= 0)
			{
				error = g_strconcat(PLUGINNAME, " - ", ACCEPTERROR, NULL);
				return ((gpointer)error);
			}
			
			/* Initializes a new struct to hold all the data readed. */
			msg = g_new0(Message, 1);
			msg->srcProto = g_strdup(PLUGINPROTO);
			ba2str(&addr.rc_bdaddr, buffer);
			msg->srcAddress = g_strdup(buffer);
			msg->chunkLen = 0;
			
			if ((channel = g_io_channel_unix_new(clientSd)) == NULL)
			{
				error = g_strconcat(PLUGINNAME, " - ", OPENCHANNELERROR, NULL);
				return ((gpointer)error);
			}
			
			chanError = NULL;
			
			/* Set a NULL encoding to read raw data. */
			if (g_io_channel_set_encoding(channel, NULL, &chanError) != G_IO_STATUS_NORMAL)
			{
				error = g_strconcat(PLUGINNAME, " - ", CHANOPTERROR, ": ", chanError->message, NULL);
				g_error_free(chanError);
				return ((gpointer)error);
			}
			
			g_timer_start(timeElapsed);
			
			/* Reads data from channel and pushes each chunk into the chunk queue. */
			if ((devStatus = g_io_channel_read_to_end(channel, &msg->chunk, &msg->chunkLen, &chanError)) == G_IO_STATUS_NORMAL)
			{
				g_async_queue_push(tData->qMessages, msg);
			}
			
			if (chanError != NULL)
			{
				g_warning("%s - %s: %s", PLUGINNAME, READERROR, chanError->message);
				g_clear_error(&chanError);
			}
			
			if (g_io_channel_shutdown(channel, TRUE, &chanError) != G_IO_STATUS_NORMAL)
			{
				error = g_strconcat(PLUGINNAME, " - ", CHANSHUTDOWNERROR, ": ", chanError->message, NULL);
				return ((gpointer)error);
			}
			
			g_timer_stop(timeElapsed);
			g_debug("%s - %d bytes read in %lf seconds", PLUGINNAME, msg->chunkLen, g_timer_elapsed(timeElapsed, NULL));
			g_timer_reset(timeElapsed);

			g_io_channel_unref(channel);
		}
		
		if (shutdown(serverSd, SHUT_RDWR) != 0)
		{
			error = g_strconcat(PLUGINNAME, " - ", FDSHUTDOWNERROR, ": ", g_strerror(errno), NULL);
			return ((gpointer)error);
		}
	#endif
		
	/* Clean and exit. */
	g_async_queue_unref(tData->qMessages);
	g_debug("%s: Receiving thread terminated", PLUGINNAME);
	
	return (NULL);
}

gboolean
pluginExit							(gchar** error)
{
	
	return (TRUE);
}
