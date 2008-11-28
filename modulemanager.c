/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: modulemanager.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "data.h"
#include "error.h"

#include "modulemanager.h"

/* Funcion initModules
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initModules				(gchar** error)
{	
	/* Comprueba que se pueden cargar modulos dinamicamente */
	if (!g_module_supported())
	{
		*error = g_strdup(CANNOTLOADMODULES);
		return (FALSE);
	}
	
	return (TRUE);
}

/* Funcion loadModule.
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
loadModule			(Plugin* plugin, gchar** error)
{
	gchar* (*pluginName) (void);
	gchar* (*pluginDesc) (void);
	gchar* (*pluginVersion) (void);
	gboolean (*pluginInit) (gpointer data, gchar** error);
	gboolean (*pluginSend) (gpointer data, gchar** error);
	gpointer (*pluginReceive) (gpointer data);
	
	plugin->module = g_module_open(plugin->filename, G_MODULE_BIND_LOCAL);
	
	if (plugin->module == NULL)
	{
		*error = g_strdup(INCORRECTMODULEFORMAT);
		return (FALSE);
	}
	
	if (!(g_module_symbol(plugin->module, "pluginName", (gpointer)&pluginName) &&
		g_module_symbol(plugin->module, "pluginDesc", (gpointer)&pluginDesc) &&
		g_module_symbol(plugin->module, "pluginVersion", (gpointer)&pluginVersion) &&
		g_module_symbol(plugin->module, "pluginInit", (gpointer)&pluginInit) &&
		g_module_symbol(plugin->module, "pluginSend", (gpointer)&pluginSend) &&
		g_module_symbol(plugin->module, "pluginReceive", (gpointer)&pluginReceive)))
	{
		g_module_close(plugin->module);
		*error = g_strdup(NOSYMBOLSAVAILABLE);
		return (FALSE);
	}
	
	plugin->pluginName = (gpointer)pluginName;
	plugin->pluginDesc = (gpointer)pluginDesc;
	plugin->pluginVersion = (gpointer)pluginVersion;
	plugin->pluginInit = (gpointer)pluginInit;
	plugin->pluginSend = (gpointer)pluginSend;
	plugin->pluginReceive = (gpointer)pluginReceive;
	
	/* Initialize module.
	 * The first parameter is a GData filled with the plugin configuration
	 * options.
	 * */
	plugin->pluginInit((gpointer)plugin->config, error);

	return (TRUE);
}

/* Funcion loadAllModules.
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Nombre del directorio de complementos.
 * Salida: Cola con punteros a los complementos, mensajes de error.
 * Proceso: Abre el directorio y lee las entradas del mismo,
 * cargando los complementos utilizando la funcion loadModules.
 * */
gboolean
loadAllModules			(GQueue* qPlugins, gchar** error)
{
	gint i;
	gchar* pluginError;
	Plugin* aux;
	
	for (i = 0; i < g_queue_get_length(qPlugins); i++)
	{
		aux = g_queue_peek_nth(qPlugins, i);
		if (!loadModule(aux, error))
		{
			/* Si no es posible cargar el modulo, la estructura Plugin
			 * es retirada de la cola */
			aux = g_queue_pop_nth(qPlugins, i);
			g_warning("Error al cargar modulo %s: %s", aux->filename, pluginError);
			g_free(aux);
		}
		else
		{
			g_debug("%s cargado correctamente", aux->filename);
		}
	}

	/* Se comprueba que existe al menos un modulo cargado.
	 * */
	if (g_queue_is_empty(qPlugins))
	{
		*error = g_strdup(NOMODULESAVAILABLE);
		return (FALSE);
	}	

	return (TRUE);
}

/* Funcion unloadAllModules
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Cola de estructuras tipo Plugin
 * Salida: Mensaje de error.
 * Proceso: Elimina todos los modulos cargados.
 * */ 
gboolean
unloadAllModules				(GQueue* qPlugins, gchar** error)
{
	/* Libera uno por uno los modulos de la cola.
	 * */
	return (TRUE);
}

/*
 * VERSION 0.1
 * */
