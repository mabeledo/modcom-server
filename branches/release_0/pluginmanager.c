/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: pluginmanager.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "pluginmanager.h"

#ifdef G_OS_WIN32
	#define G_MODULE_SUFFIX "dll"
#endif

#ifdef G_OS_UNIX
	#define G_MODULE_SUFFIX "so"
#endif

/* Funcion loadAllPlugins.
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Nombre del directorio de complementos.
 * Salida: Cola con punteros a los complementos, mensajes de error.
 * Proceso: Abre el directorio y lee las entradas del mismo,
 * cargando los complementos utilizando la funcion loadPlugins.
 * */
GQueue*
loadAllPlugins			(const gchar* dirname, gchar* error)
{
	GDir *pluginDir;
	GQueue* qPlugins;

	const gchar *dirEntry;
	gchar* pluginError;
	
	/* Comprueba que se pueden cargar modulos dinamicamente */
	if (!g_module_supported())
	{
		error = g_strdup(CANNOTLOADMODULES);
		return (NULL);
	}
	
	/* Se supondra que los modulos se encuentran en el directorio
	 * "plugins/", tras el mismo directorio del ejecutable
	 * Devuelve -1 si el directorio no puede ser abierto.
	 * */
	if ((pluginDir = g_dir_open(dirname, 0, NULL)) == NULL)
	{
		error = g_strdup(CANNOTOPENDIRECTORY);
		return (NULL);
	}
	
	/* Se leeran todas las entradas del directorio.
	 * Los errores al leer archivos que no correspondan a modulos
	 * cargables, seran reflejados como avisos de depuracion
	 * */
	while ((dirEntry = g_dir_read_name(pluginDir)) != NULL)
	{
		if (g_str_has_suffix(dirEntry, G_MODULE_SUFFIX))
		{
			/* La funcion g_strconcat necesita terminar con NULL, para
			 * indicar que es el ultimo parametro de la lista variable
			 * */
			dirEntry = g_strconcat(dirname, "/", dirEntry, NULL);
			error = NULL;
			
			g_queue_push_head(qPlugins, loadPlugin(dirEntry, pluginError));
			
			if (pluginError != NULL)
			{
				g_queue_pop_head(qPlugins);
				g_warning("Error al cargar %s: %s", dirEntry, error);
			}
			
			g_debug("%s cargado correctamente", dirEntry);
		}
		else
			g_warning("Archivo %s no puede ser cargado", dirEntry);	
	}
	
	/* Se libera la memoria temporal asignada */	
	g_dir_close(pluginDir);
	
	/* Se comprueba que existe al menos un modulo cargado.
	 * De no ser asi, devuelve un error numero -2
	 * */
	if (g_queue_is_empty(qPlugins))
	{
		error = g_strdup(NOPLUGINSAVAILABLE);
		return (NULL);
	}
	
	return (qPlugins);
}

/* Funcion loadPlugin.
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Nombre de archivo.
 * Salida: Puntero a una estructura Plugin, mensaje de error.
 * Proceso: Abre el archivo, comprueba que se ajusta a la definicion
 * de Plugin y lo carga en memoria.
 * */
Plugin*
loadPlugin			(const gchar *filename, gchar* error)
{
	Plugin *plugin;
	gchar* (*pluginName) (void);
	gchar* (*pluginDesc) (void);
	gchar* (*pluginVersion) (void);
	void (*pluginInit) (gchar* error);
	void (*pluginSend) (ThreadData* tData);
	void (*pluginReceive) (ThreadData* tData);
	
	plugin = g_new0(Plugin, 1);
	
	if (plugin == NULL)
	{
		error = g_strdup(NOMEMORYAVAILABLE);
		g_free(plugin);
		return(NULL);
	}
	
	plugin->module = g_module_open(filename, G_MODULE_BIND_LOCAL);
	
	if (plugin->module == NULL)
	{
		error = g_strdup(INCORRECTPLUGINFORMAT);
		g_free(plugin);
		return(NULL);
	}
	
	if (!(g_module_symbol(plugin->module, "pluginName", (gpointer)&pluginName) ||
		g_module_symbol(plugin->module, "pluginDesc", (gpointer)&pluginDesc) ||
		g_module_symbol(plugin->module, "pluginVersion", (gpointer)&pluginVersion) ||
		g_module_symbol(plugin->module, "pluginInit", (gpointer)&pluginInit) ||
		g_module_symbol(plugin->module, "pluginSend", (gpointer)&pluginSend) ||
		g_module_symbol(plugin->module, "pluginReceive", (gpointer)&pluginReceive)))
	{
		g_module_close(plugin->module);
		error = g_strdup(NOSYMBOLSAVAILABLE);
		g_free(plugin);
		return NULL;
	}
	
	plugin->pluginName = pluginName;
	plugin->pluginDesc = pluginDesc;
	plugin->pluginVersion = pluginVersion;
	plugin->pluginInit = pluginInit;
	plugin->pluginSend = pluginSend;
	plugin->pluginReceive = pluginReceive;
	plugin->filename = g_strdup(filename);
	error = NULL;
		
	return (plugin);
}

/* Funcion unloadAllPlugins
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Cola de estructuras tipo Plugin
 * Salida: Mensaje de error.
 * Proceso: Elimina todos los complementos.
 * */ 
void
unloadAllPlugins				(GQueue* qPlugins, gchar* error)
{
	/* Libera uno por uno los elementos de la cola.
	 * Esto se debe a la especial manera que tiene la estructura
	 * de "limpiarse".
	 * */
	g_queue_foreach(qPlugins, (GFunc)g_free, NULL);	
	
	/* Limpia la lista y libera la memoria */
	g_queue_clear(qPlugins);
	
	if (!g_queue_is_empty(qPlugins))
	{
		g_queue_free(qPlugins);
	}
	else
	{
		error = g_strdup(FREEQUEUEFAILED);
	}
	
	return;
}

/*
 * VERSION 0.1
 * */
