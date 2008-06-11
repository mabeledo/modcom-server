/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: threadmanager.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "data.h"
#include "error.h"


/* Estructura ThreadData
 * Se utiliza para pasar argumentos para la inicializacion de hilos
 * a partir de estructuras de tipo Plugin.
 * */
typedef struct _ThreadData
{
	GQueue*				qMessages;
	GQueue*				qThreads;
	gint				created;
	GError**			error;
} ThreadData;

/* Funcion threadInit
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida: Puntero a mensaje de error.
 * Proceso: Comprueba que el sistema soporta hilos.
 * */
void
threadInit						(gchar**);

/* Funcion createAllThreads
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Cola con los complementos cargados en memoria, cola de mensajes.
 * Salida: Puntero a un mensaje de error, hilos creados.
 * Proceso: Inicia los hilos necesarios para cada uno de los complementos cargados.
 * */
GQueue*
createAllThreads					(GQueue*, GQueue*, gchar**);

/* Funcion createThread
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Complemento cargado en memoria, cola de mensajes.
 * Salida: Puntero a un mensaje de error.
 * Proceso: Inicia los hilos necesarios para un complemento.
 * */
void
createThreads						(GQueue*, gchar**);

/* Funcion destroyAllThreads
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gint
destroyAllThreads					(gchar**);

/* Funcion destroyThread
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
void
destroyThread						(gchar**);
