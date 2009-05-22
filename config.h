/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: config.h
 * 	Version: 1.0
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

/* Funcion initConfig
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initConfig					(const gchar*, gchar**);

/* Funcion loadConfig
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
loadConfig					(const gchar*, GData**, gchar**);

/* Funcion endConfig
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
endConfig					();

#endif
