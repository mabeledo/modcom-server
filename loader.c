/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: loader.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "general.h"
#include "plugin.h"
 
#ifdef G_OS_WIN32
	#define G_MODULE_SUFFIX "dll"
#endif

#ifdef G_OS_UNIX
	#define G_MODULE_SUFFIX "so"
#endif

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

/* Funcion initPluginFiles
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initPlugins					(GData** pluginConfig, gchar** error)
{
	directory = g_datalist_get_data(pluginConfig, "directory");
	
	/* Checks for plugin directory. */
	if (!g_file_test(directory, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)))
	{
		*error = g_strdup(CANNOTLOCATEDIR);
		return (FALSE);
	}
	
	/* Checks for dynamically loaded modules support. */
	if (!g_module_supported())
	{
		*error = g_strdup(CANNOTLOADMODULES);
		return (FALSE);
	}

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
loadPlugin			(const gchar* fileName, GData* config, gchar** error)
{
	Plugin* plugin;
	
	gushort (*pluginProto) (void);
	const gchar* (*pluginAddress) (void);
	const gchar* (*pluginName) (void);
	const gchar* (*pluginDesc) (void);
	const gchar* (*pluginVersion) (void);
	gboolean (*pluginInit) (gpointer data, gchar** error);
	gboolean (*pluginSend) (gpointer dest, gpointer data, gchar** error);
	gpointer (*pluginReceive) (gpointer data);
	gboolean (*pluginExit) (gchar** error);
	
	plugin = g_new0(Plugin, 1);
	plugin->filename = g_strdup(fileName);
	plugin->config = g_memdup(config, sizeof(config));

	plugin->module = g_module_open(plugin->filename, G_MODULE_BIND_LOCAL);
	
	if (plugin->module == NULL)
	{
		*error = g_strdup(INCORRECTMODULEFORMAT);
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
	if ((plugin->pluginInit((gpointer)plugin->config, error)) == FALSE)
	{
		*error = g_strconcat(INITIALIZEPLUGIN, ": ", *error, NULL);
		return (NULL);
	}

	return (plugin);
}


/* Funcion loadAllPluginFiles
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
loadAllPlugins				(GQueue* qPlugins, GData** pluginSetConfig, gchar** error)
{
	GDir* pluginDir;
	gchar* dirEntry;
	gchar* configEntry;
	GData* pluginConfig;
	Plugin* plugin;

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
			pluginConfig = g_datalist_get_data(pluginSetConfig, configEntry);
			dirEntry = g_strconcat(directory, "/", dirEntry, NULL);
			
			if ((plugin = loadPlugin(dirEntry, pluginConfig, error)) == NULL)
			{
				g_debug("%s", g_strconcat(LOADPLUGINERROR, ": ", *error, NULL));
			}
			else
			{
				g_queue_push_head(qPlugins, plugin);
			}
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
