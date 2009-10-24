/********************************************************************
 *  Project: modcom-server
 *  File: data.h
 *  Version: 1.0b
 *  
 *  Defines data structures.
 * 
 *  Author: Manuel Angel Abeledo Garcia
 *
 ************************ License ***********************************
 * 
 *  This file is part of modcom-server.
 *
 *  modcom-server is free software: you can redistribute it and/or 
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2 of
 *  the License, or (at your option) any later version.
 *
 *  modcom-server is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with modcom-server.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 ********************************************************************/

//#include "header.h"

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
} ThreadData;
	GAsyncQueue* qMessages;
