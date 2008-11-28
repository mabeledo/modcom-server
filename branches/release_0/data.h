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

#include <glib.h>
#include <gmodule.h>

#define MODCOM_SERVER_VERSION		"0.1"

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
	const gchar* 	(*pluginName) 		(void);
	const gchar* 	(*pluginDesc) 		(void);
	const gchar* 	(*pluginVersion) 	(void);
	const gchar* 	(*pluginProvides) 	(void);
	void			(*pluginInit)		(gpointer, gchar**);
	gpointer 		(*pluginSend) 		(gpointer, gchar**);
	gpointer		(*pluginReceive) 	(gpointer);
	
	/* Manejados por threadmanager */
	GThread*		receiveThread;
} Plugin;


/* Struct for receiving/sending messages. */
typedef struct _Message
{
	/* Source protocol.
	 * Available protocols:
	 *   NULL   - 00000
	 *   TCP/IP - 00001
	 *   COM    - 00002
	 * */
	gushort srcProto;
	
	/* Source address. */
	gchar src[15];
	
	/* Destination protocol. */
	gushort destProto;
	
	/* Destination address. */
	gchar dest[15];
	
	/* Message contents. */
	gchar msg[952];
	
	/* MD5 checksum. */
	gchar checksum[32];
} Message;
