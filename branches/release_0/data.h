/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: data.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

/* Estructura de los plugins
 *  - filename: nombre del plugin
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



typedef struct _Message
{
	
	
	
} Message;
