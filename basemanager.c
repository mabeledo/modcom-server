/********************************************************************
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
#include "threadmanager.h"
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
	GKeyFile* kConfig;
	GQueue* qPlugins;
	GQueue* qMessages;
	
	gchar* pluginDir;
	
	/* Inicializa las estructuras de datos */
	kConfig = g_key_file_new();
	
	/* Primera comprobacion:
	 *  - Existe el ficheros de configuracion.
	 *  - Se puede cargar el fichero de configuracion.
	 * Devuelve el error del proceso que primero ha fallado.
	 * */
	if (!(initConfigFile(configFile, error) &&
		  loadConfigFile(configFile, kConfig, error)))
	{
		return (FALSE);
	}
	
	pluginDir = (gchar*)g_key_file_get_value(kConfig, "Plugins", "Directory", NULL);
	
	/* Segunda comprobacion:
	 *  - Existe el directorio de los complementos.
	 *  - El sistema puede cargar modulos dinamicamente.
	 *  - El sistema puede utilizar hilos de ejecucion.
	 * */
	if (!(initPluginFiles(pluginDir, error) &&
		  initModules(error) &&
		  initThreads(error)))
	{
		return (FALSE);
	}
	
	/* Comienzo del proceso real.
	 * 
	 * */
	
	
	
	return (TRUE);
}

/* Funcion closeComSystem
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
closeComSystem				(gchar* error)
{
	
	return (TRUE);
	
}

/*
 * VERSION 0.1
 * */
