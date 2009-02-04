/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/tcpip.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "tcpip.h"

#define PLUGINPROTO		2
#define PLUGINNAME		"tcpip"
#define PLUGINDESC		"Complemento de comunicaciones a traves de protocolo TCP/IP"
#define PLUGINVERSION	"0.1"

#define LISTENPORT		31337
#define INTERFACE		"wlan0"
#define ADDRESS			"127.0.0.1"
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
static gint listenPort = LISTENPORT;
static gchar* interface = INTERFACE;
static gchar* address = ADDRESS;

gushort
pluginProto							()
{
	return ((gushort)PLUGINPROTO);
}

const gchar*
pluginAddress						()
{
	return (address);
}

const gchar*
pluginName							()
{
	return (PLUGINNAME);
}

const gchar*
pluginDesc							()
{
	return (PLUGINDESC);
}

const gchar*
pluginVersion						()
{
	return (PLUGINVERSION);
}

gboolean
pluginInit							(gpointer data, gchar** error)
{
	GData* tcpipConfig = (GData*)data;
	
	/* Address search. */
	
	/* Winsocket support */
	#ifdef G_OS_WIN32
		/* TODO */
	#endif
	
	/* Posix socket support */
	#ifdef G_OS_UNIX
		gint localSd;
		struct ifconf ifaces;
		struct ifreq* ifAttr;
		struct in_addr* localAddr;
		
		gint i, ifreqSize;
		
		localSd = socket(AF_INET, SOCK_STREAM, 0);
		ifaces.ifc_buf = NULL;
		ifreqSize = sizeof(struct ifreq);
		
		/* Sets a 30-unit buffer for interface data. */
		ifaces.ifc_len = ifreqSize * 300;
		ifaces.ifc_buf = realloc(ifaces.ifc_buf, ifaces.ifc_len);

		/* Needs at least one interface in the list.
		 * This code is a little mess but works...
		 * */
		if (ioctl(localSd, SIOCGIFCONF, &ifaces) >= 0)
		{
			ifAttr = ifaces.ifc_req;
			
			for (i = 0; i < ifaces.ifc_len; i += ifreqSize)
			{				
				localAddr = (struct in_addr*)((ifAttr->ifr_ifru.ifru_addr.sa_data) + 2);

				if (g_ascii_strcasecmp(ifAttr->ifr_ifrn.ifrn_name, interface) == 0)
				{
					address = inet_ntoa(*localAddr);
				}
				ifAttr++;
			}
		}
	#endif	
	
	/* Load configuration parameters */
	if (g_datalist_get_data(&tcpipConfig, "port") != NULL)
	{
		listenPort = (gint)g_strtod((gchar*)g_datalist_get_data(&tcpipConfig, "port"), NULL);
	}
	
	if (g_datalist_get_data(&tcpipConfig, "interface") != NULL)
	{
		interface = (gchar*)g_datalist_get_data(&tcpipConfig, "interface");
	}
	
	g_debug("TCP/IP plugin loaded and initialized");
	return (TRUE);
}

gboolean
pluginSend							(gpointer dest, gpointer data, gchar** error)
{	
	/* Better use a string instead a struct to send data over TCP. */
	gchar* msgStr = g_memdup(data, sizeof(Message));
	gchar* destAddress = (gchar*)dest;

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
		
		if (inet_aton(destAddress, &addr.sin_addr) == 0)
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
		gboolean receiving;
		
		addrLen = sizeof(struct sockaddr_in);
		receiving = TRUE;
		
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
		
		/* Loop accepting incoming connections. */
		while (receiving)
		{
			/* Accepts client connections */
			clientSd = accept(serverSd, (struct sockaddr *)&addr, (guint32 *)&addrLen);

			if (clientSd <= 0)
			{
				error = g_strdup(ACCEPTERROR);
				return ((gpointer)error);
			}
			
			/* Gets data. */
			recvData = recv(clientSd, buffer, BUFFERLEN, 0);
			g_async_queue_push(qMessages, &buffer);
		}
		
		close(clientSd);
	#endif

	/* TODO
	 * Sockets are encapsulated in a GIOChannel structure, in order to
	 * be managed in a transparent way.
	 * */
	
	/* Clean and exit. */
	g_async_queue_unref(qMessages);
	return (NULL);
}

gboolean
pluginExit							(gchar** error)
{
	return (TRUE);
}

/*
 * VERSION 0.1
 * */
