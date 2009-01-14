/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/null.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "null.h"

#define PLUGINTYPE		1
#define PLUGINNAME		"null"
#define PLUGINDESC		"Sin funcionalidad"
#define PLUGINVERSION	"0.1"

gushort
pluginType							()
{
	return ((gushort)PLUGINTYPE);
}

const gchar*
pluginName							()
{
	return (PLUGINNAME);
}

const gchar*
pluginDesc							()
{
	return (PLUGINDESC);
}

const gchar*
pluginVersion						()
{
	return (PLUGINVERSION);
}

gboolean
pluginInit							(gpointer data, gchar** error)
{
	g_debug("NULL plugin loaded and initialized");
	return (TRUE);
}

gboolean
pluginSend							(gpointer data, gchar** error)
{
	return (TRUE);
}

gpointer
pluginReceive						(gpointer data)
{
	return (NULL);
}

gboolean
pluginExit							(gchar** error)
{
	return (TRUE);
}

/*
 * VERSION 0.1
 * */
