/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugin.h
 *  Version: 1.0
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include <gmodule.h>

typedef struct _Plugin
{
        gchar*			filename;

        GModule*		module;
        const gchar*	(*pluginProto)		(void);
		const gchar*	(*pluginAddress)	(void);
        const gchar*	(*pluginName)		(void);
        const gchar*	(*pluginDesc)		(void);
        const gchar*	(*pluginVersion)	(void);
        gboolean		(*pluginInit)		(gpointer, gchar**);
        gboolean		(*pluginSend)		(gpointer, gpointer, gchar**);
        gpointer		(*pluginReceive)	(gpointer);
        gboolean		(*pluginExit)		(gchar**);
} Plugin;

