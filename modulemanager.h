/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: modulemanager.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

/* Funcion initModules
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida: Puntero a mensaje de error.
 * Proceso: Comprueba que el sistema soporta hilos.
 * */
gboolean
initModules				(gchar**);

/* Funcion loadAllModules.
 * Precondiciones:
 * Postcondiciones:
 * Entrada: Nombre del directorio de complementos.
 * Salida: Cola con punteros a los complementos, mensajes de error.
 * Proceso: Abre el directorio y lee las entradas del mismo,
 * cargando los complementos utilizando la funcion loadModules.
 * */
gboolean
loadAllModules			(GQueue*, gchar**);

/* Funcion unloadAllModules
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */ 
gboolean
unloadAllModules		(GQueue*, gchar**);

#endif

/*
 * VERSION 0.1
 * */
