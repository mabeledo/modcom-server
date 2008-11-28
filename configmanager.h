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

/* Funcion getGroupConfig
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
GData*
getGroupConfig				(GKeyFile*, const gchar*, gchar**);

/* Funcion getGroupSetConfig
 * Obtiene un diccionario GData con pares clave/valor constituidos por
 * nombres cuyo sufijo sea el parametro de entrada "pattern", y
 * estructuras GData formadas a partir de los grupos cuyo nombre sea
 * la clave mas el patron. Ej. null-plugin/GData (grupo null-plugin)
 *  - Entrada: estructura GKeyFile con la configuracion de un fichero,
 * 			   cadena de texto con un patron.
 *  - Salida: Diccionario de diccionarios (Superestructura GData que
 * 			  contiene estructuras GData).
 * */
GData*
getGroupSetConfig				(GKeyFile*, const gchar*, gchar**);

#endif
