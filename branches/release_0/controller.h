/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: controller.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#ifndef CONTROLLER_H
#define CONTROLLER_H

/* Funcion initController
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
initController				(GData**, gchar**);

/* Funcion loadController
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
loadController				(GQueue*, GAsyncQueue*, gchar**);

/* Funcion writeMessage
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
writeMessage				(const gchar*, const gchar*, const gchar*, gchar**);

/* Funcion readMessage
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
readMessage					(gchar**, gchar**);

#endif
