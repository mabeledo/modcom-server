/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: filemanager.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

/* Funcion initPluginFiles
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initPluginFiles				(GData*, gchar*);

/* Funcion loadAllPluginFiles
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
loadAllPluginFiles			(GQueue*, GData*, GData*, gchar*);

#endif
