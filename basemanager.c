/*******************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: basemanager.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "data.h"

#include "configmanager.h"
#include "pluginmanager.h"
#include "modulemanager.h"
#include "receivemanager.h"
#include "dispatchmanager.h"
#include "basemanager.h"

/* Error messages */


/* Funcion openComSystem
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
openComSystem				(const gchar* configFile, gchar** error)
{
	/* Memory allocation is done here, to avoid local issues or
	 * automatic deallocation after function exit.
	 * */
	GData* dConfig;
	GData* pluginConfig;
	GData* moduleConfig;
	GData* dispatchConfig;
	GData* pluginSetConfig;
	GQueue* qPlugins;
	GAsyncQueue* qMessages;
	
	/* All data in the configuration file is copied into a
	 * GData structure, in order to handle configuration parameters
	 * easily.
	 * */
	if (!(initConfigFile(configFile, error) &&
	      loadConfigFile(configFile, &dConfig, error)))
	{
		return (FALSE);
	}
	
	/* Gets per module configuration settings. */
	pluginConfig = g_datalist_get_data(&dConfig, "plugins");
	dispatchConfig = g_datalist_get_data(&dConfig, "dispatch");
	
	/* *
	 * Segunda fase (comprobacion y ejecucion de procesos):
	 *  - Existe el directorio de los complementos.
	 *  - El sistema puede cargar modulos dinamicamente.
	 *  - El sistema puede utilizar hilos de ejecucion.
	 *  - (Initdispatcher)
	 * */
	if (!(initPluginFiles(&pluginConfig, error) &&
		  initModules(error) &&
		  initReceivers(error) &&
		  initDispatcher(&dispatchConfig, error)))
	{
		return (FALSE);
	}
	
	/* Free memory containing configuration patterns already used. */
	g_datalist_remove_data(&dConfig, "plugins");
	pluginSetConfig = dConfig;
	
	/* Inicializa la cola de plugins y de mensajes. */
	qPlugins = g_queue_new();
	qMessages = g_async_queue_new();
	
	/* Se inicializan las estructuras de plugins.
	 *  - Explora el directorio de plugins y carga los ficheros.
	 *  - Carga las funciones en las estructuras de plugin correspondientes.
	 * */
	if (!(loadAllPluginFiles(qPlugins, &dConfig, error) &&
		  loadAllModules(qPlugins, error)))
	{
		return (FALSE);
	}
	
	/* DispatchManager inicia el proceso de reparto de mensajes de la
	 * cola.
	 * Receivemanager crea los hilos de ejecucion	 necesarios para la
	 * recepcion asincrona de mensajes.
	 * */
	if (!loadDispatcher(qPlugins, qMessages, error))
	{
		return (FALSE);
	}
	
	if (!loadAllReceivers(qPlugins, qMessages, error))
	{
		return (FALSE);
	}
	
	return (TRUE);
}

/* Funcion closeComSystem
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * NOT IMPLEMENTED
 * */
gboolean
closeComSystem				(gchar** error)
{
	return (TRUE);
}

/*
 * VERSION 0.1
 * */
