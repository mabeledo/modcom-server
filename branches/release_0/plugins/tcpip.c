/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/tcpip.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "tcpip.h"

#define PLUGINTYPE		2
#define PLUGINNAME		"tcpip"
#define PLUGINDESC		"Complemento de comunicaciones a traves de protocolo TCP/IP"
#define PLUGINVERSION	"0.1"

#define LISTENPORT		31337
#define BUFFERLEN		2048

/* Error messages. */
#define BINDERROR		"bind() error"
#define LISTENERROR		"listen() error"
#define ACCEPTERROR		"accept() error"
#define GETSERVERROR	"getservbyport() error"
#define SOCKETERROR		"socket() error"
#define ATONERROR		"inet_aton() error"
#define CONNECTERROR	"connect() error"
#define SENDERROR		"send() error"

/* Global variables.
 * These might be defined in the configuration file.
 * */
gint listenPort = LISTENPORT;

gushort
pluginType							()
{
	return ((gushort)PLUGINTYPE);
}

gchar*
pluginName							()
{
	return (PLUGINNAME);
}

gchar*
pluginDesc							()
{
	return (PLUGINDESC);
}

gchar*
pluginVersion						()
{
	return (PLUGINVERSION);
}

gboolean
pluginInit							(gpointer data, gchar** error)
{
	GData* tcpipConfig = (GData*)data;
	
	/* Load configuration parameters */
	if (g_datalist_get_data(&tcpipConfig, "port") != NULL)
	{
		listenPort = (gint)g_strtod((gchar*)g_datalist_get_data(&tcpipConfig, "port"), NULL);
	}
	
	g_debug("TCP/IP plugin loaded and initialized");
	return (TRUE);
}

gboolean
pluginSend							(gpointer data, gchar** error)
{
	Message* msg = (Message*)data;
	
	/* Better use a string instead a struct to send data over TCP. */
	gchar* msgStr = g_memdup(data, sizeof(Message));

	/* Winsocket support */
	#ifdef G_OS_WIN32
		/* TODO */
	#endif
	
	/* Posix socket support */
	#ifdef G_OS_UNIX
		gint clientSd, addrLen;
		struct servent *server;
		struct sockaddr_in addr;
		
		addrLen = sizeof(struct sockaddr_in);
		
		if ((clientSd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			*error = g_strdup(SOCKETERROR);
			return (FALSE);
		}
		
		/* Gets a struct servent for LISTENPORT port and TCP/IP protocol. */
		if ((server = getservbyport(listenPort, "tcp")) == NULL)
		{
			*error = g_strdup(GETSERVERROR);
			return (FALSE);
		}
		
		bzero(&addr, addrLen);
		addr.sin_family = AF_INET;
		addr.sin_port = server->s_port;
		
		if (inet_aton(msg->dest, &addr.sin_addr) == 0)
		{
			*error = g_strdup(ATONERROR);
			return (FALSE);
		}
		
		/* Connects to server. */
		if (connect(clientSd, (struct sockaddr*)&addr, addrLen) != 0)
		{
			*error = g_strdup(CONNECTERROR);
			return (FALSE);
		}
		
		/* Sends message.
		 * TODO: set 'errno' on error message.
		 * */
		if (send(clientSd, msgStr, strlen(msgStr), 0) <= 0)
		{
			*error = g_strdup(SENDERROR);
			return (FALSE);
		}
		
		close(clientSd);
	#endif

	return (TRUE);
}

gpointer
pluginReceive						(gpointer data)
{
	/* Increases the refence count of the queue by one. */
	GAsyncQueue* qMessages = g_async_queue_ref((GAsyncQueue*)data);
	gchar* error;
	
	/* Winsocket support */
	#ifdef G_OS_WIN32
		/* TODO */
	#endif
	
	/* Posix socket support */
	#ifdef G_OS_UNIX
		gint serverSd, clientSd, recvData, addrLen;
		gchar buffer[BUFFERLEN];
		struct sockaddr_in addr;
		
		addrLen = sizeof(struct sockaddr_in);
		
		if ((serverSd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			error = g_strdup(SOCKETERROR);
			return (FALSE);
		}

		bzero(&addr, addrLen);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(listenPort);
		addr.sin_addr.s_addr = INADDR_ANY;
		
		/* Binds socket to address */
		if (bind(serverSd, (struct sockaddr *)&addr, addrLen) != 0)
		{
			error = g_strdup(BINDERROR);
			return ((gpointer)error);
		}
		
		/* Sets a waiting socket with 10 wait slots */
		if (listen(serverSd, 10) != 0)
		{
			error = g_strdup(LISTENERROR);
			return ((gpointer)error);
		}
		
		g_debug("TCP/IP Plugin up, running and accepting new connections...");
		
		/* Endless loop accepting incoming connections. */
		while (TRUE)
		{
			/* Accepts client connections */
			clientSd = accept(serverSd, (struct sockaddr *)&addr, (guint32 *)&addrLen);

			if (clientSd <= 0)
			{
				error = g_strdup(ACCEPTERROR);
				return ((gpointer)error);
			}
			
			/* Gets data.
			 * */
			recvData = recv(clientSd, buffer, BUFFERLEN, 0);
			
			/* Allocate message at the end of the queue.
			 * */
			g_async_queue_push(qMessages, &buffer);
			
			close(clientSd);
		}
	#endif

	/* TODO
	 * Sockets are encapsulated in a GIOChannel structure, in order to
	 * be managed in a transparent way.
	 * */
	
	/* Clean and exit. */
	g_async_queue_unref(qMessages);
	return (NULL);
}

/*
 * VERSION 0.1
 * */
