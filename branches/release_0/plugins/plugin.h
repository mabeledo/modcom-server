/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/plugin.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include <glib.h>
#include "../data.h"

/* Default plugin interface.
 * */

gushort
pluginType							();

gchar*
pluginName							();

gchar*
pluginDesc							();

gchar*
pluginVersion						();

gboolean
pluginInit							(gpointer, gchar**);

gboolean
pluginSend							(gpointer, gchar**);

gpointer
pluginReceive						(gpointer);

/*
 * VERSION 0.1
 * */
