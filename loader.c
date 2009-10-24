/********************************************************************
 *  Project: modcom-server
 *  File: loader.c
 *  Version: 1.0b
 * 
 *  Author: Manuel Angel Abeledo Garcia
 *
 ************************ License ***********************************
 * 
 *  This file is part of modcom-server.
 *
 *  modcom-server is free software: you can redistribute it and/or 
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2 of
 *  the License, or (at your option) any later version.
 *
 *  modcom-server is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with modcom-server.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 ********************************************************************/
#include "general.h"
#include "plugin.h"
 
#ifdef G_OS_WIN32
	#define G_MODULE_SUFFIX "dll"
#endif

#ifdef G_OS_UNIX
	#define G_MODULE_SUFFIX "so"
#endif

#define PLUGINDIR				"./plugins"
#define MODULENAME				"Loader"

/* Error messages */
#define LOADPLUGINERROR			"El complemento no ha sido cargado"
#define INITIALIZEPLUGIN		"El complemento no ha podido ser cargado"
#define CANNOTLOCATEDIR			"El directorio de los complementos no existe"
#define NOPLUGINFILESAVAILABLE	"No hay archivos de complemento disponibles"
#define CANNOTOPENCONFIGFILE	"El archivo de configuracion no pudo ser abierto"
#define CANNOTOPENDIRECTORY		"Imposible abrir el directorio de complementos"
#define	CANNOTLOADMODULES		"No se pueden cargar modulos dinamicamente"
#define NOMEMORYAVAILABLE		"No se ha podido reservar memoria"
#define INCORRECTMODULEFORMAT	"El archivo no tiene formato de modulo"
#define NOSYMBOLSAVAILABLE		"No es un plugin o no tiene una definicion estandar"
#define FREEQUEUEFAILED			"La cola no ha sido liberada correctamente"

/* Global variables
 * */
static gchar* directory;

/* Funcion initPlugins
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initPlugins					(GData** pluginConfig, gchar** error)
{
	if ((directory = g_datalist_get_data(pluginConfig, "directory")) == NULL)
	{
		directory = PLUGINDIR;
	}
	
	/* Checks for plugin directory. */
	if (!g_file_test(directory, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)))
	{
		*error = g_strconcat(MODULENAME, " - ", CANNOTLOCATEDIR, NULL);
		return (FALSE);
	}
	
	/* Checks for dynamically loaded modules support. */
	if (!g_module_supported())
	{
		*error = g_strconcat(MODULENAME, " - ", CANNOTLOADMODULES, NULL);
		return (FALSE);
	}

	/* Free memory and exit. */
	g_datalist_clear(pluginConfig);
	return (TRUE);
}

/* Funcion loadPlugin.
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
static Plugin*
loadPlugin			(const gchar* fileName, GData** config, gchar** error)
{
	Plugin* plugin;
	
	const gchar* (*pluginProto) (void);
	const gchar* (*pluginAddress) (void);
	const gchar* (*pluginName) (void);
	const gchar* (*pluginDesc) (void);
	const gchar* (*pluginVersion) (void);
	gboolean (*pluginInit) (gpointer data, gchar** error);
	gboolean (*pluginSend) (gpointer dest, gpointer data, gchar** error);
	gpointer (*pluginReceive) (gpointer data);
	gboolean (*pluginExit) (gchar** error);
	
	plugin = g_slice_new0(Plugin);
	plugin->filename = g_strdup(fileName);

	plugin->module = g_module_open(plugin->filename, G_MODULE_BIND_LOCAL);
	
	if (plugin->module == NULL)
	{
		*error = g_strdup(INCORRECTMODULEFORMAT);
		g_slice_free(Plugin, plugin);
		return (NULL);
	}
	
	if (!(g_module_symbol(plugin->module, "pluginProto", (gpointer)&pluginProto) &&
		g_module_symbol(plugin->module, "pluginAddress", (gpointer)&pluginAddress) &&
	    g_module_symbol(plugin->module, "pluginName", (gpointer)&pluginName) &&
		g_module_symbol(plugin->module, "pluginDesc", (gpointer)&pluginDesc) &&
		g_module_symbol(plugin->module, "pluginVersion", (gpointer)&pluginVersion) &&
		g_module_symbol(plugin->module, "pluginInit", (gpointer)&pluginInit) &&
		g_module_symbol(plugin->module, "pluginSend", (gpointer)&pluginSend) &&
		g_module_symbol(plugin->module, "pluginReceive", (gpointer)&pluginReceive) &&
		g_module_symbol(plugin->module, "pluginExit", (gpointer)&pluginExit)))
	{
		g_module_close(plugin->module);
		*error = g_strdup(NOSYMBOLSAVAILABLE);
		return (NULL);
	}
	
	plugin->pluginProto = (gpointer)pluginProto;
	plugin->pluginAddress = (gpointer)pluginAddress;
	plugin->pluginName = (gpointer)pluginName;
	plugin->pluginDesc = (gpointer)pluginDesc;
	plugin->pluginVersion = (gpointer)pluginVersion;
	plugin->pluginInit = (gpointer)pluginInit;
	plugin->pluginSend = (gpointer)pluginSend;
	plugin->pluginReceive = (gpointer)pluginReceive;
	plugin->pluginExit = (gpointer)pluginExit;
	
	/* Initialize module.
	 * The first parameter is a GData filled with the plugin configuration
	 * options.
	 * */	
	if (!plugin->pluginInit((gpointer)config, error))
	{
		return (NULL);
	}

	return (plugin);
}


/* Funcion loadAllPlugins
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
loadAllPlugins				(GData** dPlugins, GData** pluginSetConfig, gchar** error)
{
	GDir* pluginDir;
	gchar* dirEntry;
	gchar* configEntry;
	GData** pluginConfig;
	Plugin* plugin;
	gint counter;

	/* Se supondra que los modulos se encuentran en un directorio
	 * especifico.
	 * */
	if ((pluginDir = g_dir_open(directory, 0, NULL)) == NULL)
	{
		*error = g_strconcat(MODULENAME, " - ", CANNOTOPENDIRECTORY, NULL);
		return (FALSE);
	}
	
	counter = 0;
	
	/* Every directory entry is read */
	while ((dirEntry = (gchar*)g_dir_read_name(pluginDir)) != NULL)
	{
		if (g_str_has_suffix(dirEntry, G_MODULE_SUFFIX))
		{
			/* La funcion g_strconcat necesita terminar con NULL, para
			 * indicar que es el ultimo parametro de la lista variable
			 * */
			configEntry = g_strsplit(dirEntry, ".", 2)[0];
			pluginConfig = g_datalist_get_data(pluginSetConfig, configEntry);
			dirEntry = g_strconcat(directory, "/", dirEntry, NULL);
			
			if ((plugin = loadPlugin(dirEntry, pluginConfig, error)) == NULL)
			{
				g_warning("%s", g_strconcat(MODULENAME, " - ", LOADPLUGINERROR, ": ", *error, NULL));
			}
			else
			{
				g_datalist_set_data(dPlugins, plugin->pluginProto(), plugin);
				counter++;
			}
		}
	}
	
	if (counter <= 0)
	{
		*error = g_strconcat(MODULENAME, " - ", NOPLUGINFILESAVAILABLE, NULL);
		return (FALSE);
	}

	g_dir_close(pluginDir);
	g_free(directory);
	
	return (TRUE);
}

/* Funcion closePlugin
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
static void
closePlugin					(GQuark key, gpointer data, gpointer user_data)
{
	Plugin* plugin = (Plugin*)data;
	g_slice_free(Plugin, plugin);
}


/* Funcion closeAllPlugins
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
closeAllPlugins				(GData** dPlugins, gchar** error)
{
	g_datalist_foreach(dPlugins, closePlugin, NULL);
	g_datalist_clear(dPlugins);
	
	return (TRUE);
}
