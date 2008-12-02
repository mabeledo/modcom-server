/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: configmanager.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

/* Funcion initConfigFile
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initConfigFile				(const gchar*, gchar**);

/* Funcion getGroupSetLength
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gint
getGroupSetLength			(const gchar*, gchar**);

/* Funcion loadConfigFile
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
loadConfigFile				(const gchar*, GData**, gchar**);

#endif
