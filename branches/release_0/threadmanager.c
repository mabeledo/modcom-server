/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: threadmanager.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "data.h"
#include "error.h"

#include "threadmanager.h"

/* Funcion initThreads
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida: Puntero a mensaje de error.
 * Proceso: Comprueba que el sistema soporta hilos.
 * */
gboolean
initThreads						(gchar* error)
{
	#ifdef G_THREADS_ENABLED
		if (!g_thread_supported())
		{
			g_thread_init(NULL);
		}
		
		error = NULL;
		return (TRUE);
	#else
		error = g_strdup(THREADSNOTSUPPORTED);
		return (FALSE);
	#endif
}

/* Funcion createThread
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Complemento cargado en memoria, cola de mensajes.
 * Salida: Puntero a un mensaje de error.
 * Proceso: Inicia los hilos necesarios para un complemento.
 * */
gboolean
createThread						(Plugin* plugin, ThreadData* data, gchar* error)
{
	void (*pluginReceive) (ThreadData* tData);
	const gchar* (*pluginName) (void);
	GError* threadError;
	
	pluginReceive = (gpointer)plugin->pluginReceive;
	pluginName = (gpointer)plugin->pluginName;
	
	/* Creacion del hilo de recepcion, almacenamiento de la referencia en la cola */
	plugin->receiveThread = g_thread_create((gpointer)&pluginReceive, (gpointer)data, FALSE, &threadError);

	if (threadError != NULL)
	{
		error = g_strconcat(CANNOTCREATETHREAD, " para el plugin ", pluginName(), ": ", threadError->message, NULL);
		g_error_free(threadError);
		g_free(plugin->receiveThread);
		return (FALSE);
	}
	
	error = NULL;
	return (TRUE);
}

/* Funcion createAllThreads
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Cola con los complementos cargados en memoria, cola de mensajes.
 * Salida: Cola de hilos creados, mensaje de error
 * Proceso: Inicia los hilos necesarios para cada uno de los complementos cargados.
 * */
gboolean
createAllThreads					(GQueue* qPlugins, GQueue* qMessages, gchar* error)
{
	ThreadData* data;
	gchar* threadError;
	gint i;

	data = g_new0(ThreadData, 1);
	data->qMessages = qMessages;
	data->mMessages = g_mutex_new();
	
	/* Crea los hilos para las funciones de envío */
	for (i = 0; i < g_queue_get_length(qPlugins); i++)
	{
		/* Rellena la estructura de datos para el hilo */
		if (!(createThread((Plugin*)g_queue_peek_nth(qPlugins, i), data, threadError)))
		{
			error = g_strdup(threadError);
			return (FALSE);
		}
	}
	
	error = NULL;
	return (TRUE);
}
