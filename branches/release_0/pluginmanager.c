/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: filemanager.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "error.h"
#include "data.h"
 
#ifdef G_OS_WIN32
	#define G_MODULE_SUFFIX "dll"
#endif

#ifdef G_OS_UNIX
	#define G_MODULE_SUFFIX "so"
#endif

/* Global variables
 * */
gchar* directory;

/* Funcion initPluginFiles
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initPluginFiles				(GData** pluginConfig, gchar** error)
{
	directory = g_datalist_get_data(pluginConfig, "directory");

	/* Comprueba que el directorio de los modulos existe */
	if (!g_file_test(directory, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)))
	{
		*error = g_strdup(CANNOTLOCATEDIR);
		return (FALSE);
	}

	return (TRUE);
}

/* Funcion loadAllPluginFiles
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
loadAllPluginFiles			(GQueue* qPlugins, GData** pluginSetConfig, gchar** error)
{
	GDir* pluginDir;
	gchar* dirEntry;
	gchar* configEntry;
	Plugin* aux;

	/* Se supondra que los modulos se encuentran en un directorio
	 * especifico.
	 * */
	if ((pluginDir = g_dir_open(directory, 0, NULL)) == NULL)
	{
		*error = g_strdup(CANNOTOPENDIRECTORY);
		return (FALSE);
	}
	
	/* Every directory entry is read */
	while ((dirEntry = (gchar*)g_dir_read_name(pluginDir)) != NULL)
	{
		if (g_str_has_suffix(dirEntry, G_MODULE_SUFFIX))
		{
			/* La funcion g_strconcat necesita terminar con NULL, para
			 * indicar que es el ultimo parametro de la lista variable
			 * */
			configEntry = g_strsplit(dirEntry, ".", 2)[0];
			dirEntry = g_strconcat(directory, "/", dirEntry, NULL);
			aux = g_new0(Plugin, 1);
			aux->filename = g_strdup(dirEntry);
			aux->config = g_datalist_get_data(pluginSetConfig, configEntry);
			g_queue_push_head(qPlugins, aux);
		}
	}
	
	if (g_queue_is_empty(qPlugins))
	{
		*error = g_strdup(NOPLUGINFILESAVAILABLE);
		return (FALSE);
	}

	g_dir_close(pluginDir);
	
	return (TRUE);
}

