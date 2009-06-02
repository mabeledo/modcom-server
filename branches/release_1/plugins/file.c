/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/file.c
 * 	Version: 1.0
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/


/* Plugin main attributes. */
#define PLUGINPROTO		"FILE"
#define PLUGINNAME		"file"
#define PLUGINDESC		"File writer/reader"
#define PLUGINVERSION	"1.0"

#include "common.h"
#include "file.h"
#include "../msg.h"

/* File alteration monitor support. */
#ifdef G_OS_WIN32
	/* TODO */
#endif

#ifdef G_OS_UNIX
	/* Linux Inotify mechanism. */
	#if defined(__LINUX__) || defined(__linux__) ||
		defined(__linux) || defined(linux)
		#include <sys/inotify.h>
	#endif
#endif

#define MAXFILES		3

/* Error messages. */
#define INOTIFYINITERROR	"Inotify inicialization error"
#define OPENCHANNELERROR	"Error opening channel"
#define MAXFILESWARN		"Maximum number of files exceeded, cutting to 3..."
#define NOFILESAVAILABLE	"There is no files available in the configuration file"
#define CANNOTOPENANYFILE	"Cannot open at least one file to read/write"

/* Global variables. */
static gboolean deleteContent;
static GIOChannel** fileChannel;
static gint numFileChannel;

gboolean
pluginInit							(gpointer data, gchar** error)
{
	GData* fileConfig = (GData*)data;
	GError* chanError;
	gchar** files;
	gint length;
	gint i;
	
	/* Load configuration parameters */
	if (g_datalist_get_data(&fileConfig, "files") != NULL)
	{
		/* Returns a NULL terminated array of strings. */
		files = g_strsplit(g_datalist_get_data(&fileConfig, "files"), ",", 0);
		length = g_strv_length(files);
		numFileChannel = 0;
		
		if (length > MAXFILES)
		{
			g_warning("%s - %s", PLUGINNAME, MAXFILESWARN);
		}

		for (i = 0; i < MAXFILES; i++)
		{
			if ((fileChannel[i] = g_io_channel_new_file(files[i], "a+", &chanError)) == NULL)
			{
				g_warning("%s - %s: %s", PLUGINNAME, OPENCHANNELERROR, chanError->message);
			}
			else
			{
				numFileChannel++;
			}
		}
		
		if (numFileChannel < 1)
		{
			*error = g_strconcat(PLUGINNAME, " - ", CANNOTOPENANYFILE, NULL);
			return (FALSE);
		}
	}
	else
	{
		/* Needs at least one file to watch for */
		*error = g_strdup(NOFILESAVAILABLE);
		return (FALSE);
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
	gchar* error;
	
	
	
	/* Inotify support */
	#if defined(__LINUX__) || defined(__linux__) || defined(__linux) || defined(linux)
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
	g_strfreev(files);
	return (TRUE);
}
