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
	Plugin*				tPlugin;
	GQueue*				qMessages;
	GMutex*				mMessages;

} ThreadData;

void
threadInit						(gchar*);


GQueue*
createAllThreads					(GQueue*, GQueue*, gchar*);


GThread*
createThread						(ThreadData*, gchar*);


gint
destroyAllThreads					(gchar*);


void
destroyThread						(gchar*);
