/********************************************************************
 *  Project: modcom-server
 *  File: receiver.c
 *  Version: 1.0b
 * 
 *  Author: Manuel Angel Abeledo Garcia
 *
 ************************ License ***********************************
 * 
 *  This file is part of modcom-server.
 *
 *  modcom-server is free software: you can redistribute it and/or 
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2 of
 *  the License, or (at your option) any later version.
 *
 *  modcom-server is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with modcom-server.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 ********************************************************************/

#include "general.h"
#include "thread.h"
#include "plugin.h"

#define MODULENAME				"Receiver"

/* Error messages */
#define NOTENOUGHTHREADS		"No se ha creado ningun thread"
#define CANNOTCREATETHREAD		"No se ha podido crear el thread"

/* Local data definitions. */
typedef struct 
_ForeachData
{
	ThreadData* tData;
	GPtrArray* pThreads;
	gint counter;
} ForeachData;

/* Funcion initReceivers
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initReceivers					(GData** receiveConfig, gchar** error)
{
	
	/* Free memory and exit. */
	g_datalist_clear(receiveConfig);
	return (TRUE);
}

/* Funcion loadReceiver
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Complemento cargado en memoria, cola de mensajes.
 * Salida: Puntero a un mensaje de error.
 * Proceso: Inicia los hilos necesarios para un complemento.
 * */
static void
loadReceiver					(GQuark key, gpointer data, gpointer user_data)
{
	Plugin* plugin;
	ForeachData* fData;
	GThread* receiveThread;
	GError* threadError;
	
	plugin = (Plugin*)data;
	fData = (ForeachData*)user_data;
	threadError = NULL;

	/* Creacion del hilo de recepcion, almacenamiento de la referencia en la cola */
	receiveThread = g_thread_create(plugin->pluginReceive, (gpointer)fData->tData, TRUE, &threadError);

	if (threadError != NULL)
	{
		g_warning("%s - %s. Plugin: %s. Error: %s.", MODULENAME, 
													CANNOTCREATETHREAD,
													plugin->pluginName(),
													threadError->message);
		g_error_free(threadError);
		g_free(receiveThread);
	}
	else
	{
		g_ptr_array_add(fData->pThreads, (gpointer)receiveThread);
		fData->counter++;
	}
	
	return;
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
	ExtThreadData* etData;
	ForeachData* fData;
	GData** dPlugins;
	gint i;

	etData = data;
	dPlugins = etData->dPlugins;
	fData = g_new0(ForeachData, 1);
	fData->tData = etData->tData;
	fData->pThreads = g_ptr_array_new();
	fData->counter = 0;
	
	/* Crea los hilos para las funciones de envío */
	g_datalist_foreach(dPlugins, loadReceiver, (gpointer)fData);
		
	g_debug("%s - Up & running", MODULENAME);
	
	/* Waits for every thread previously created */
	if (fData->counter > 0)
	{
		for (i = 0; i < fData->counter; i++)
		{
			g_thread_join((GThread*)g_ptr_array_index(fData->pThreads, i));
		}
	}
	else
	{
		g_critical("%s - %s", MODULENAME, NOTENOUGHTHREADS);
	}

	return (NULL);
}

/* Funcion closeReceivers
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
closeReceivers					(gchar** error)
{

	return (TRUE);
}
