/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/interface.h
 * 	Version: 1.0
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

/* Default plugin interface.
 * */

const gchar*
pluginProto							();

const gchar*
pluginAddress						();

const gchar*
pluginName							();

const gchar*
pluginDesc							();

const gchar*
pluginVersion						();

gboolean
pluginInit							(gpointer, gchar**);

gboolean
pluginSend							(gpointer, gpointer, gchar**);

gpointer
pluginReceive						(gpointer);

gboolean
pluginExit							(gchar**);
