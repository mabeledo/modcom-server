/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/file.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "file.h"

#define PLUGINPROTO		3
#define PLUGINADDRESS	"file"
#define PLUGINNAME		"file"
#define PLUGINDESC		"File reader plugin"
#define PLUGINVERSION	"0.1"

#define MAXFILES		3

/* Error messages. */
#define INOTIFYINITERROR	"Inotify inicialization error"
#define MAXFILESERROR		"Maximum number of files exceeded"
#define NOFILESAVAILABLE	"There is no files available"

/* Global variables. */
static gchar** files;
static gboolean deleteContent;

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
	GData* fileConfig = (GData*)data;
	
	/* Load configuration parameters */
	if (g_datalist_get_data(&fileConfig, "files") != NULL)
	{
		/* Returns a NULL terminated array of strings. */
		files = g_strsplit(g_datalist_get_data(&fileConfig, "files"), ",", 0);

		if (g_strv_length(files) > MAXFILES)
		{
			*error = g_strdup(MAXFILESERROR);
			return (FALSE);
		}
	}
	else
	{
		/* Needs at least one file to watch for */
		*error = g_strdup(NOFILESAVAILABLE);
		return (FALSE);
	}

	if (g_datalist_get_data(&fileConfig, "delete") != NULL)
	{
		files = (gboolean)g_datalist_get_data(&fileConfig, "delete");
	}
	
	g_debug("FILE plugin loaded and initialized");
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
	
	
	
	
	/* Inotify support */
	#if defined(__LINUX__) || defined(__linux__) || defined(__linux)
		/* "id" stands for "inotify descriptor". "dd" stands for
		 * "directory descriptor".
		 * */
		gint id, filesLength;
		
		if ((id = inotify_init()) < 0)
		{
			return ((gpointer)INOTIFYINITERROR);
		}
		
		
		filesLength = g_strv_length(files);
		
	#endif
	
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
