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

/* Funcion initPluginFiles
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initPluginFiles				(GData* pluginConfig, gchar* error)
{
	gchar* directory;
	directory = (gchar *)g_datalist_get_data(&pluginConfig, "Directory");
	
	/* Comprueba que el directorio de los modulos existe */
	if (!g_file_test(directory, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)))
	{
		error = g_strdup(CANNOTLOCATEDIR);
		return (FALSE);
	}
	
	error = NULL;
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
loadAllPluginFiles			(GQueue* qPlugins, GData* pluginConfig, GData* pluginSetConfig, gchar* error)
{
	GDir* pluginDir;
	gchar* directory;
	gchar* dirEntry;
	Plugin* aux;
	
	directory = (gchar*)g_datalist_get_data(&pluginConfig, "Directory");
	
	/* Se supondra que los modulos se encuentran en un directorio
	 * especifico.
	 * */
	if ((pluginDir = g_dir_open(directory, 0, NULL)) == NULL)
	{
		error = g_strdup(CANNOTOPENDIRECTORY);
		return (FALSE);
	}
	
	/* Se leeran todas las entradas del directorio.
	 * */
	while ((dirEntry = (gchar*)g_dir_read_name(pluginDir)) != NULL)
	{
		if (g_str_has_suffix(dirEntry, G_MODULE_SUFFIX))
		{
			/* La funcion g_strconcat necesita terminar con NULL, para
			 * indicar que es el ultimo parametro de la lista variable
			 * */
			dirEntry = g_strconcat(directory, "/", dirEntry, NULL);
			aux = g_new0(Plugin, 1);
			aux->filename = g_strdup(dirEntry);
			/* aux->config = */
			g_queue_push_head(qPlugins, aux);
		}
	}
	
	if (g_queue_is_empty(qPlugins))
	{
		error = g_strdup(NOPLUGINFILESAVAILABLE);
		return (FALSE);
	}

	g_dir_close(pluginDir);
	
	error = NULL;
	return (TRUE);
}

