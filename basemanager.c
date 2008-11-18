/*******************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: basemanager.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "data.h"
#include "error.h"

#include "configmanager.h"
#include "pluginmanager.h"
#include "modulemanager.h"
#include "receivemanager.h"
#include "dispatchmanager.h"
#include "basemanager.h"

/* Funcion openComSystem
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
openComSystem				(const gchar* configFile, gchar* error)
{
	/*  - kConfig, contiene la transcripcion del fichero de configuracion
	 * 	  en memoria.
	 *  - pluginConfig, contiene la configuracion general de complementos.
	 *  - pluginSetConfig, contiene las configuraciones de cada uno de los
	 * 	  complementos.
	 * */
	GKeyFile* kConfig;
	GData* pluginConfig;
	GData* dispatchConfig;
	GData* pluginSetConfig;
	
	GQueue* qPlugins;
	GAsyncQueue* qMessages;
	
	/* *
	 * Primera fase (configuracion):
	 *  - Existe el ficheros de configuracion.
	 *  - Se puede cargar el fichero de configuracion.
	 * Devuelve el error del proceso que primero ha fallado.
	 * */
	kConfig = g_key_file_new();
	
	if (!(initConfigFile(configFile, error) &&
		  loadConfigFile(configFile, kConfig, error)))
	{
		return (FALSE);
	}
	
	/* Las configuraciones son volcadas a diccionarios para cada una
	 * de las partes de la aplicacion que necesita configuracion.
	 * */
	pluginConfig = getGroupConfig(kConfig, "Plugins", error);
	dispatchConfig = getGroupConfig(kConfig, "Dispatcher", error);
	pluginSetConfig = getGroupSetConfig(kConfig, "plugin", error);
	
	/* *
	 * Segunda fase (comprobacion y ejecucion de procesos):
	 *  - Existe el directorio de los complementos.
	 *  - El sistema puede cargar modulos dinamicamente.
	 *  - El sistema puede utilizar hilos de ejecucion.
	 *  - (Initdispatcher)
	 * */
	if (!(initPluginFiles(pluginConfig, error) &&
		  initModules(error) &&
		  initReceivers(error) &&
		  initDispatchers(error)))
	{
		return (FALSE);
	}
	
	/* Inicializa la cola de plugins y de mensajes. */
	qPlugins = g_queue_new();
	qMessages = g_async_queue_new();
	/* qMessages = g_new0(MsgQueue, 1);
	 * qMessages->messages = g_queue_new();
	 * qMessages->mutex = g_mutex_new();
	 * */
	
	/* Se inicializan las estructuras de plugins.
	 *  - Explora el directorio de plugins y carga los ficheros.
	 *  - Carga las funciones en las estructuras de plugin correspondientes.
	 * */
	if (!(loadAllPluginFiles(qPlugins, pluginConfig, pluginSetConfig, error) &&
		  loadAllModules(qPlugins, error)))
	{
		return (FALSE);
	}
	
	/* DispatchManager inicia el proceso de reparto de mensajes de la
	 * cola.
	 * Receivemanager crea los hilos de ejecucion necesarios para la
	 * recepcion asincrona de mensajes.
	 * */
	if (!loadAllDispatchers(qPlugins, qMessages, dispatchConfig, error))
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
closeComSystem				(gchar* error)
{
	return (TRUE);
}

/*
 * VERSION 0.1
 * */
