/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugin.h
 * 	Version: 0.1
 *
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/
 
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
