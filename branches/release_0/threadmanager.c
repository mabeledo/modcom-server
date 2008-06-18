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
threadInit						(gchar* error)
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
 * Salida: Cola de hilos creados, mensaje de error
 * Proceso: Inicia los hilos necesarios para cada uno de los complementos cargados.
 * */
GQueue*
createAllThreads					(GQueue* qPlugins, GQueue* qMessages, gchar* error)
{
	ThreadData* data;
	GQueue* qThreads;
	GMutex* mutex;
	gchar* threadError;
	gint i;

	mutex = g_mutex_new();
	
	/* Crea los hilos para las funciones de envío */
	for (i = 0; i < g_queue_get_length(qPlugins); i++)
	{
		/* Rellena la estructura de datos para el hilo */
		data = g_new0(ThreadData, 1);
		data->tPlugin = (Plugin*)g_queue_peek_nth(qPlugins, i);
		data->qMessages = qMessages;
		data->mMessages = mutex;

		g_queue_push_head(qThreads, createThread(data, threadError));
		
		if (threadError != NULL)
		{
			g_queue_pop_head(qThreads);
			g_warning("Error al crear hilo para %s: %s", data->tPlugin->pluginName(), error);
		}
	}
	
	if (g_queue_is_empty(qThreads))
	{
		error = g_strdup(NOTENOUGHTHREADS);
		return (NULL);
	}

	return(qThreads);
}

/* Funcion createThread
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Complemento cargado en memoria, cola de mensajes.
 * Salida: Puntero a un mensaje de error.
 * Proceso: Inicia los hilos necesarios para un complemento.
 * */
GThread*
createThread						(ThreadData* data, gchar* error)
{
	GThread* aux;
	Plugin* plugin;
	void (*pluginReceive) (ThreadData* tData);
	const gchar* (*pluginName) (void);
	GError* threadError;
	
	plugin = data->tPlugin;
	
	pluginReceive = (gpointer)plugin->pluginReceive;
	pluginName = (gpointer)plugin->pluginName;
	
	/* Creacion del hilo de recepcion, almacenamiento de la referencia en la cola */
	aux = g_thread_create((gpointer)&pluginReceive, (gpointer)data, FALSE, &threadError);

	if (threadError != NULL)
	{
		error = g_strconcat(CANNOTCREATETHREAD, " para el plugin ", pluginName(), ": ", threadError->message, NULL);
		g_error_free(threadError);
		g_free(aux);
		return(NULL);
	}
	
	return(aux);
}


/* Funcion destroyAllThreads
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gint
destroyAllThreads					(gchar* error)
{
	return (0);	
}

/* Funcion destroyThread
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
void
destroyThread						(gchar* error)
{
	return;
}
