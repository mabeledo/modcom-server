/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: pluginloader.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "pluginloader.h"

#ifdef G_OS_WIN32
	#define G_MODULE_SUFFIX "dll"
#endif

#ifdef G_OS_UNIX
	#define G_MODULE_SUFFIX "so"
#endif

/* Funcion loadAllPlugins.
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Nombre del directorio de complementos, puntero a mensajes de error.
 * Salida: Cola con punteros a los complementos.
 * Proceso: Abre el directorio y lee las entradas del mismo,
 * cargando los complementos utilizando la funcion loadPlugins.
 * */
GQueue*
loadAllPlugins			(const gchar* dirname, gchar **error)
{
	GDir *pluginDir;
	GQueue* qPlugins;

	const gchar *dirEntry;
	gchar** pluginError;
	
	/* Comprueba que se pueden cargar modulos dinamicamente */
	if (!g_module_supported())
	{
		*error = g_strdup(CANNOTLOADMODULES);
		return (NULL);
	}
	
	/* Se supondra que los modulos se encuentran en el directorio
	 * "plugins/", tras el mismo directorio del ejecutable
	 * Devuelve -1 si el directorio no puede ser abierto.
	 * */
	if ((pluginDir = g_dir_open(dirname, 0, NULL)) == NULL)
	{
		*error = g_strdup(CANNOTOPENDIRECTORY);
		return (NULL);
	}
	
	/* Se leeran todas las entradas del directorio.
	 * Los errores al leer archivos que no correspondan a modulos
	 * cargables, seran reflejados como avisos de depuracion
	 * */
	while ((dirEntry = g_dir_read_name(pluginDir)) != NULL)
	{	
		dirEntry = g_strconcat(pluginDir, dirEntry);
		
		if (g_str_has_suffix(dirEntry, G_MODULE_SUFFIX))
		{
			g_queue_push_head(qPlugins, &(loadPlugin(dirEntry, &pluginError)));
			
			if (*pluginError)
			{
				/* Se asume que el puntero es nulo si ha habido un error,
				 * por lo que no se libera explicitamente la memoria.
				 * */
				g_queue_pop_head(qPlugins);
				g_debug("Error al cargar %s: %s", dirEntry, *error);
			}
			
			g_debug("%s cargado correctamente", dirEntry);
		}
		else
			g_debug("Archivo %s no puede ser cargado", dirEntry);	
	}
	
	/* Se libera la memoria temporal asignada */	
	g_dir_close(pluginDir);
	
	/* Se comprueba que existe al menos un modulo cargado.
	 * De no ser asi, devuelve un error numero -2
	 * */
	if (g_queue_is_empty(qPlugins))
	{
		*error = g_strdup(NOAVAILABLEPLUGINS);
		return (NULL);
	}
	
	return (qPlugins);
}

/* Funcion loadPlugin.
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Nombre de archivo.
 * Salida: Puntero a una estructura Plugin o mensaje de error.
 * Proceso: Abre el archivo, comprueba que se ajusta a la definicion
 * de Plugin y lo carga en memoria.
 * */
Plugin*
loadPlugin			(const gchar *filename, gchar **error)
{
	Plugin *plugin;
	gchar *pluginName;
	gchar *pluginDesc;
	gchar *pluginVersion;
	void (*pluginInit) (gchar **error);
	void (*pluginSend) (GQueue* qMsg, gchar **error);
	void (*pluginReceive) (GQueue* qMsg, gchar **error);
	
	plugin->module = g_module_open(filename, G_MODULE_BIND_LOCAL);
	if (plugin->module != NULL)
	{	
		if (g_module_symbol(plugin->module, "pluginName", &pluginName) ||
			g_module_symbol(plugin->module, "pluginDesc", &pluginDesc) ||
			g_module_symbol(plugin->module, "pluginVersion", &pluginVersion) ||
			g_module_symbol(plugin->module, "pluginInit", &pluginInit) ||
			g_module_symbol(plugin->module, "pluginSend", &pluginSend) ||
			g_module_symbol(plugin->module, "pluginReceive", &pluginReceive))
		{
			g_queue_push_tail(qPlugins, &auxModule);
			g_debug("Archivo %s cargado y registrado (%p)", dirEntry, &auxModule);
		}
		else
		{
			g_module_close(plugin->module);
			g_free(plugin);
			return NULL;
		}
	}
	else
		g_debug("Archivo %s parece un modulo, pero no lo es", dirEntry);
	}
		
	return (0);
}

/* Funcion unloadAllPlugins
 * 
 * */
gint
unloadPlugin				(GQueue* qPlugins, gchar **error)
{
	
	
}

/* Funcion unloadPlugin
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Estructura tipo Plugin.
 * Salida: Entero que indica como ha finalizado la funcion y 
 * mensaje de error si procede.
 * Proceso: Elimina el complemento y libera la memoria.
 * */ 
void
unloadPlugin				(Plugin* plugin, gchar **error)
{
	
	
}

/*
 * VERSION 0.1
 * */
