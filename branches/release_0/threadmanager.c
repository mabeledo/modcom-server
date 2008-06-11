/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: threadmanager.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "threadmanager.h"

/* Funcion threadInit
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida: Puntero a mensaje de error.
 * Proceso: Comprueba que el sistema soporta hilos.
 * */
void
threadInit						(gchar** error)
{
	#ifdef G_THREADS_ENABLED
		if (!g_thread_supported())
		{
			g_thread_init(NULL);
		}
	#else
		*error = g_strdup(THREADSNOTSUPPORTED);
		return;
	#endif
}

/* Funcion createAllThreads
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Cola con los complementos cargados en memoria, cola de mensajes.
 * Salida: Puntero a un mensaje de error, hilos creados.
 * Proceso: Inicia los hilos necesarios para cada uno de los complementos cargados.
 * */
GQueue*
createAllThreads					(GQueue* qPlugins, GQueue* qMessages, gchar** error)
{
	ThreadData* data;
	
	data->qMessages = qMessages;
	data->qPlugins = qPlugins;
	data->created = 0;
	
	/* Crea los hilos para las funciones de envío */
	g_queue_foreach(qPlugins, (GFunc)createThreads, (gpointer)data);
	
	if (data->created == 0)
	{
		*error = g_strdup(NOTENOUGHTHREADS);
		return (NULL);
	}
	
	
	
	
}

/* Funcion createThreads
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Complemento cargado en memoria, cola de mensajes.
 * Salida: Puntero a un mensaje de error.
 * Proceso: Inicia los hilos necesarios para un complemento.
 * */
void
createThreads						(Plugin* plugin, ThreadData* data)
{
	/* Inicializa el complemento.
	 * Habitualmente, la inicializacion consiste unicamente en un
	 * mensaje de debug.
	 * */
	plugin->pluginInit(data);
	
	/* Creacion del hilo de envio */
	g_thread_create(plugin->pluginSend, (gpointer)data->qMessages, NULL, data->error); 
	
	if (data->error )
	{
		
	}
	
	/* Creacion del hilo de recepcion */
	g_thread_create(plugin->pluginReceive, (gpointer)data->qMessages, NULL, data->error); 
	
	if (data->error )
	{
		
	}
	
}


/* Funcion destroyAllThreads
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gint
destroyAllThreads					(gchar** error)
{
	
	
}

/* Funcion destroyThread
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
void
destroyThread						(gchar** error)
{
	
	
}
