/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: receivemanager.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "data.h"

#include "receivemanager.h"

/* Error messages */
#define THREADSNOTSUPPORTED		"El sistema no soporta la creacion de hilos"
#define NOTENOUGHTHREADS		"No se ha creado ningun thread"
#define CANNOTCREATETHREAD		"No se ha podido crear el thread"

/* Funcion initReceivers
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida: Puntero a mensaje de error.
 * Proceso: Comprueba que el sistema soporta hilos.
 * */
gboolean
initReceivers					(gchar** error)
{
	#ifdef G_THREADS_ENABLED
		if (!g_thread_supported())
		{
			g_thread_init(NULL);
		}

		return (TRUE);
	#else
		*error = g_strdup(THREADSNOTSUPPORTED);
		return (FALSE);
	#endif
}

/* Funcion loadReceiver
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Complemento cargado en memoria, cola de mensajes.
 * Salida: Puntero a un mensaje de error.
 * Proceso: Inicia los hilos necesarios para un complemento.
 * */
gboolean
loadReceiver					(Plugin* plugin, GAsyncQueue* qMessages, gchar** error)
{
	GError* threadError = NULL;

	/* Creacion del hilo de recepcion, almacenamiento de la referencia en la cola */
	plugin->receiveThread = g_thread_create(plugin->pluginReceive, (gpointer)qMessages, TRUE, &threadError);

	if (threadError != NULL)
	{
		*error = g_strconcat(CANNOTCREATETHREAD, ". Plugin: ", plugin->pluginName(),
							" Error: ", threadError->message, NULL);
		g_error_free(threadError);
		g_free(plugin->receiveThread);
		return (FALSE);
	}

	return (TRUE);
}

/* Funcion loadAllReceivers
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Cola con los complementos cargados en memoria, cola de mensajes.
 * Salida: Cola de hilos creados, mensaje de error
 * Proceso: Inicia los hilos necesarios para cada uno de los complementos cargados.
 * */
gpointer
loadAllReceivers				(gpointer data)
{
	Plugin* auxPlugin;
	ThreadData* tData;
	gchar* threadError;
	gint i;
	
	tData = data;

	/* Crea los hilos para las funciones de envío */
	for (i = 0; i < g_queue_get_length(tData->qPlugins); i++)
	{
		/* Rellena la estructura de datos para el hilo */
		if (!(loadReceiver((Plugin*)g_queue_peek_nth(tData->qPlugins, i), tData->qMessages, &threadError)))
		{
			return (threadError);
		}
	}
	
	/* Waits for every thread previously created */
	for (i = 0; i < g_queue_get_length(tData->qPlugins); i++)
	{
		auxPlugin = (Plugin*)g_queue_peek_nth(tData->qPlugins, i);
		g_thread_join(auxPlugin->receiveThread);
	}

	return (NULL);
}
