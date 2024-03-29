/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugin.h
 *      Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

typedef struct _Plugin
{
        gchar*              filename;
        GData*              config;

        GModule*            module;
        const gchar*       (*pluginProto)          (void);
		const gchar*    	(*pluginAddress)        (void);
        const gchar*    	(*pluginName)           (void);
        const gchar*    	(*pluginDesc)           (void);
        const gchar*    	(*pluginVersion)        (void);
        gboolean           (*pluginInit)           (gpointer, gchar**);
        gboolean           (*pluginSend)           (gpointer, gpointer, gchar**);
        gpointer           (*pluginReceive)        (gpointer);
        gboolean           (*pluginExit)           (gchar**);

        GThread*            receiveThread;
} Plugin;

