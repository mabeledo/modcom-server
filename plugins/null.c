/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/null.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "null.h"

#define PLUGINPROTO		1
#define PLUGINADDRESS	"null"
#define PLUGINNAME		"null"
#define PLUGINDESC		"Sin funcionalidad"
#define PLUGINVERSION	"0.1"

gushort
pluginProto							()
{
	return ((gushort)PLUGINPROTO);
}

const gchar*
pluginAddress						()
{
	return (PLUGINADDRESS);
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
pluginSend							(gpointer dest, gpointer data, gchar** error)
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
