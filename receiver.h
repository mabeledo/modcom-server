/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: receivermanager.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#ifndef RECEIVER_H
#define RECEIVER_H

/* Funcion initReceivers
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida: 
 * Proceso: 
 * */
gboolean
initReceivers					(GData**, gchar**);

/* Funcion loadAllReceivers
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gpointer
loadAllReceivers				(gpointer);

/* Funcion closeReceivers
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
closeReceivers					(gchar**);

#endif

/*
 * VERSION 0.1
 * */
