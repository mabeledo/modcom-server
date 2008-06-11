/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: pluginloader.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include <glib.h>
#include <gmodule.h>
#include "data.h"
#include "error.h"

/* Funcion loadAllPlugins.
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Nombre del directorio de complementos, puntero a mensajes de error.
 * Salida: Cola con punteros a los complementos.
 * Proceso: Abre el directorio y lee las entradas del mismo,
 * cargando los complementos utilizando la funcion loadPlugins.
 * */
GQueue*
loadAllPlugins			(const gchar*, gchar **);

/* Funcion loadPlugin.
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Nombre de archivo.
 * Salida: Puntero a una estructura Plugin o mensaje de error.
 * Proceso: Abre el archivo, comprueba que se ajusta a la definicion
 * de Plugin y lo carga en memoria.
 * */
Plugin*
loadPlugin				(const gchar *, gchar **);

/* Funcion unloadPlugin
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Estructura tipo Plugin.
 * Salida: Entero que indica como ha finalizado la funcion y 
 * mensaje de error si procede.
 * Proceso: Elimina el complemento y libera la memoria.
 * */ 
gint
unloadPlugin			(Plugin *, gchar **);

/*
 * VERSION 0.1
 * */
