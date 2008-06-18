/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: data.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include <glib.h>
#include <gmodule.h>

/* Estructura de los complementos
 *  - filename: nombre del complemento
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
	gchar*			filename;
	GModule*		module;
	const gchar* 	(*pluginName) 		(void);
	const gchar* 	(*pluginDesc) 		(void);
	const gchar* 	(*pluginVersion) 	(void);
	const gchar* 	(*pluginProvides) 	(void);
	void			(*pluginInit)		(void);
	void 			(*pluginSend) 		(GQueue*);
	void 			(*pluginReceive) 	(GQueue*);
} Plugin;

typedef struct _ThreadData
{
	Plugin*				tPlugin;
	GQueue*				qMessages;
	GMutex*				mMessages;
} ThreadData;

typedef struct _Message
{
	
	
	
} Message;
