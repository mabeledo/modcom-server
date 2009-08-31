/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/common.h
 * 	Version: 1.0
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

/* Maximum size of each chunk, in bytes. */
#define MAXCHUNKLEN					30000

#include <glib.h>
#include "../msg.h"
#include "../thread.h"

static gchar* address;

const gchar*
pluginProto							()
{
	return (PLUGINPROTO);
}

const gchar*
pluginAddress						()
{
	return (address);
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
