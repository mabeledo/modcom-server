/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: controller.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#ifndef COMPOSER_H
#define COMPOSER_H

/* Funcion initComposer
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
initComposer				(GData**, gchar**);

/* Funcion loadComposer
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
loadComposer				(GData**, GAsyncQueue*, gchar**);

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
