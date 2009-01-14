/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: modulemanager.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "data.h"

#include "modulemanager.h"

/* Error messages */
#define	CANNOTLOADMODULES		"No se pueden cargar modulos dinamicamente"
#define	NOMODULESAVAILABLE		"No hay modulos disponibles"
#define NOMEMORYAVAILABLE		"No se ha podido reservar memoria"
#define INCORRECTMODULEFORMAT	"El archivo no tiene formato de modulo"
#define NOSYMBOLSAVAILABLE		"No es un plugin o no tiene una definicion estandar"
#define FREEQUEUEFAILED			"La cola no ha sido liberada correctamente"

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
static gboolean
loadModule			(Plugin* plugin, gchar** error)
{
	gushort (*pluginType) (void);
	const gchar* (*pluginName) (void);
	const gchar* (*pluginDesc) (void);
	const gchar* (*pluginVersion) (void);
	gboolean (*pluginInit) (gpointer data, gchar** error);
	gboolean (*pluginSend) (gpointer data, gchar** error);
	gpointer (*pluginReceive) (gpointer data);
	gboolean (*pluginExit) (gchar** error);
	
	plugin->module = g_module_open(plugin->filename, G_MODULE_BIND_LOCAL);
	
	if (plugin->module == NULL)
	{
		*error = g_strdup(INCORRECTMODULEFORMAT);
		return (FALSE);
	}
	
	if (!(g_module_symbol(plugin->module, "pluginType", (gpointer)&pluginType) &&
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
		return (FALSE);
	}
	
	plugin->pluginType = (gpointer)pluginType;
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
	gchar* moduleError;
	Plugin* aux;
	gint length;
	
	length = g_queue_get_length(qPlugins);
	i = 0;
	
	while (i < length)
	{
		aux = g_queue_peek_nth(qPlugins, i);
		
		if (!loadModule(aux, &moduleError))
		{
			aux = g_queue_pop_nth(qPlugins, i);
			g_queue_remove(qPlugins, aux);
			g_warning("Error loading module %s: %s", aux->filename, moduleError);
			g_free(aux);
			length--;
		}
		else
		{
			i++;
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
