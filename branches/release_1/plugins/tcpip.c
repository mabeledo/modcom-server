/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/tcpip.c
 * 	Version: 1.0
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

/* Plugin main attributes. */
#define PLUGINPROTO		"TCPIP"
#define PLUGINNAME		"TCP/IP"
#define PLUGINDESC		"Complemento de comunicaciones a traves de protocolo TCP/IP"
#define PLUGINVERSION	"1.0"

#include "common.h"
#include "tcpip.h"

/* Winsocket support */
#ifdef G_OS_WIN32
	#include <iostream.h>
	#include <winsock.h>
#endif

/* Posix socket support */
#ifdef G_OS_UNIX
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <errno.h>
	#include <unistd.h>
	#include <sys/types.h> 
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <resolv.h>
	#include <net/if.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <string.h>
#endif


#ifdef G_OS_UNIX
	#define INTERFACE		"eth0"
#endif

/* Error messages. */
#define NODEVICEAVAILABLE	"There is no device available"
#define NOPORTSPECIFIED		"No port in configuration file"
#define BINDERROR			"bind() error"
#define LISTENERROR			"listen() error"
#define ACCEPTERROR			"accept() error"
#define GETSERVERROR		"getservbyport() error"
#define SOCKETERROR			"socket() error"
#define OPENCHANNELERROR	"Error opening channel"
#define READERROR			"Error reading data from channel"
#define ATONERROR			"inet_aton() error"
#define CONNECTERROR		"connect() error"
#define SENDERROR			"Error sending chunk"
#define CHANOPTERROR		"Error setting up channel"
#define CHANSHUTDOWNERROR	"Error closing channel"
#define FDSHUTDOWNERROR		"Error closing descriptor"
#define FLUSHERROR			"Error flushing data"


/* Global variables.
 * These might be defined in the configuration file.
 * */
static gint listenPort;
static gchar* interface;

gboolean
pluginInit							(gpointer data, gchar** error)
{
	GData* tcpipConfig = (GData*)data;
	
	/* Load configuration parameters */
	#ifdef G_OS_UNIX
		if (g_datalist_get_data(&tcpipConfig, "interface") != NULL)
		{
			interface = (gchar*)g_datalist_get_data(&tcpipConfig, "interface");
		}
		else
		{
			*error = g_strconcat(PLUGINNAME, " - ", NODEVICEAVAILABLE, NULL);
			return (FALSE);
		}
	#endif
	
	if (g_datalist_get_data(&tcpipConfig, "port") != NULL)
	{
		listenPort = (gint)g_strtod((gchar*)g_datalist_get_data(&tcpipConfig, "port"), NULL);
	}
	else
	{
		*error = g_strconcat(PLUGINNAME, " - ", NOPORTSPECIFIED, NULL);
		return (FALSE);
	}
	
	/* Free memory. */
	g_datalist_clear(&tcpipConfig);
	
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

	/* Winsocket support */
	#ifdef G_OS_WIN32
		/* TODO */
	#endif
	
	/* Posix socket support */
	#ifdef G_OS_UNIX
		gint clientSd, addrLen;
		struct sockaddr_in addr;
		
		addrLen = sizeof(struct sockaddr_in);
		
		if ((clientSd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			*error = g_strconcat(PLUGINNAME, " - ", SOCKETERROR, NULL);
			return (FALSE);
		}
		
		if ((channel = g_io_channel_unix_new(clientSd)) == NULL)
		{
			*error = g_strconcat(PLUGINNAME, " - ", OPENCHANNELERROR, NULL);
			return (FALSE);
		}

		bzero(&addr, addrLen);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(listenPort);
		
		if (inet_aton(destAddress, &addr.sin_addr) == 0)
		{
			*error = g_strconcat(PLUGINNAME, " - ", ATONERROR, NULL);
			return (FALSE);
		}
		
		/* Connects to server. */
		if (connect(clientSd, (struct sockaddr*)&addr, addrLen) != 0)
		{
			*error = g_strconcat(PLUGINNAME, " - ", CONNECTERROR, ": ", strerror(errno), NULL);
			return (FALSE);
		}
		
		chanError = NULL;
		
		/* Set a NULL encoding to read raw data. */
		if (g_io_channel_set_encoding(channel, NULL, &chanError) != G_IO_STATUS_NORMAL)
		{
			*error = g_strconcat(PLUGINNAME, " - ", CHANOPTERROR, ": ", chanError->message, NULL);
			return (FALSE);
		}

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
	gchar* error;
	
	tData = data;
	tData->qMessages = g_async_queue_ref(tData->qMessages);

	/* Winsocket support */
	#ifdef G_OS_WIN32
		/* TODO */
	#endif
	
	/* Posix socket support */
	#ifdef G_OS_UNIX
		gint serverSd, clientSd, addrLen;
		struct sockaddr_in addr;
		
		addrLen = sizeof(struct sockaddr_in);
		
		if ((serverSd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			error = g_strconcat(PLUGINNAME, " - ", SOCKETERROR, ": ", strerror(errno), NULL);
			g_warning("%s", error);
			return ((gpointer)error);
		}

		bzero(&addr, addrLen);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(listenPort);
		addr.sin_addr.s_addr = INADDR_ANY;
		
		/* Binds socket to address */
		if (bind(serverSd, (struct sockaddr *)&addr, addrLen) != 0)
		{
			error = g_strconcat(PLUGINNAME, " - ", BINDERROR, ": ", strerror(errno), NULL);
			g_warning("%s", error);
			return ((gpointer)error);
		}
		
		/* Sets a waiting socket with 10 wait slots */
		if (listen(serverSd, 10) != 0)
		{
			error = g_strconcat(PLUGINNAME, " - ", LISTENERROR, ": ", strerror(errno), NULL);
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
			
			/* Get IP address converting the original sockaddr struct into a
			 * sockaddr_in, and the IP in network byte order from that struct into
			 * human readable notation.
			 * */
			msg->srcAddress = inet_ntoa(addr.sin_addr);
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
				g_warning("%s", error);
				return ((gpointer)error);
			}

			g_io_channel_unref(channel);
		}
		
		if (shutdown(serverSd, SHUT_RDWR) != 0)
		{
			error = g_strconcat(PLUGINNAME, " - ", FDSHUTDOWNERROR, ": ", strerror(errno), NULL);
			g_warning("%s", error);
			return ((gpointer)error);
		}
	#endif
		
	/* Clean and exit. */
	g_async_queue_unref(tData->qMessages);
	g_debug("%s: Receiving thread terminated", PLUGINNAME);
	
	return (NULL);
}

gboolean
pluginExit						(gchar** error)
{
	g_free((gpointer)interface);
	return (TRUE);
}
