/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: threadmanager.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#ifndef THREADMANAGER_H
#define THREADMANAGER_H

/* Funcion initThreads
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida: 
 * Proceso: 
 * */
gboolean
initThreads						(gchar*);

/* Funcion createAllThreads
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
createAllThreads				(GQueue*, GQueue*, gchar*);

/* Funcion createThread
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
createThread					(Plugin*, ThreadData*, gchar*);

#endif

/*
 * VERSION 0.1
 * */
