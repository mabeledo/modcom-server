/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: pluginmanager.h
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
 * Entrada: Nombre del directorio de complementos.
 * Salida: Cola con punteros a los complementos, mensajes de error.
 * Proceso: Abre el directorio y lee las entradas del mismo,
 * cargando los complementos utilizando la funcion loadPlugins.
 * */
GQueue*
loadAllPlugins			(const gchar*, gchar *);

/* Funcion loadPlugin.
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Nombre de archivo.
 * Salida: Puntero a una estructura Plugin, mensaje de error.
 * Proceso: Abre el archivo, comprueba que se ajusta a la definicion
 * de Plugin y lo carga en memoria.
 * */
Plugin*
loadPlugin				(const gchar*, gchar*);

/* Funcion unloadAllPlugins
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Cola de estructuras tipo Plugin
 * Salida: Mensaje de error.
 * Proceso: Elimina todos los complementos.
 * */ 
void
unloadAllPlugins			(GQueue*, gchar*);

/*
 * VERSION 0.1
 * */
