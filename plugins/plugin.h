/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/plugin.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "../general.h"
#include "../msg.h"

/* Default plugin interface.
 * */

gushort
pluginType							();

const gchar*
pluginName							();

const gchar*
pluginDesc							();

const gchar*
pluginVersion						();

gboolean
pluginInit							(gpointer, gchar**);

gboolean
pluginSend							(gpointer, gchar**);

gpointer
pluginReceive						(gpointer);

gboolean
pluginExit							(gchar**);

/*
 * VERSION 0.1
 * */
