/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: data.h
 * 	Version: 0.1
 * 
 *  Defines data structures.
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "header.h"

/* Estructura de los complementos
 *  - filename: nombre del complemento.
 *  - config: configuracion del complemento.
 *  - module: puntero al modulo cargado.
 *  - pluginName
 * 	- pluginDesc
 *  - pluginVersion
 *  - pluginSend
 *  - pluginReceive
 *  - pluginInit
 * */
typedef struct _Plugin
{
	/* Manejados por pluginmanager */
	gchar*			filename;
	GData*			config;
	
	/* Manejados por modulemanager */
	GModule*		module;
	gushort			(*pluginType) 		(void);
	const gchar* 	(*pluginName) 		(void);
	const gchar* 	(*pluginDesc) 		(void);
	const gchar* 	(*pluginVersion) 	(void);
	gboolean		(*pluginInit)		(gpointer, gchar**);
	gboolean 		(*pluginSend) 		(gpointer, gchar**);
	gpointer		(*pluginReceive) 	(gpointer);
	gboolean 		(*pluginExit) 		(gchar**);
	
	/* Manejados por threadmanager */
	GThread*		receiveThread;
} Plugin;


#define END_OF_DATA					"^END"

/* Struct for receiving/sending messages. */
typedef struct _Message
{
	/* Source protocol.
	 * Available protocols:
	 *   NULL   - 1
	 *   TCP/IP - 2
	 *   FILE   - 3
	 *   COM    - 4
	 * */
	gushort srcProto;
	
	/* Source address. */
	gchar src[15];
	
	/* Destination protocol. */
	gushort destProto;
	
	/* Destination address. */
	gchar dest[15];
	
	/* Type.
	 * */
	gushort type;
	
	/* ID */
	gushort id;
	
	/* Part.
	 * Available values:
	 *   0 - Standalone message or end.
	 *   >0 - Part of a bigger message.
	 * */
	gushort part;
	
	/* Message contents. */
	gchar data[1024];
	
	/* MD5 checksum. */
	gchar checksum[32];
} Message;


/*
 * Struct containing all data needed for creating receive or
 * dispatch threads. 
 * */
typedef struct _ThreadData
{
	GQueue* qPlugins;
	GAsyncQueue* qMessages;
} ThreadData;
