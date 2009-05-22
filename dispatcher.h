/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: dispatcher.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#ifndef DISPATCHER_H
#define DISPATCHER_H

/* Funcion initDispatcher
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initDispatcher					(GData**, gchar**);

/* Funcion loadDispatcher
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gpointer
loadDispatcher					(gpointer);

/* Funcion closeDispatcher
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
closeDispatcher					(gchar**);

#endif
