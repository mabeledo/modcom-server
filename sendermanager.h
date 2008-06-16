/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: sendermanager.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "data.h"
#include "error.h"

GQueue*
createAllThreads					(GQueue*, GQueue*, gchar*);


GThread*
createThread						(ThreadData*, gchar*);


gint
destroyAllThreads					(gchar*);


void
destroyThread						(gchar*);

