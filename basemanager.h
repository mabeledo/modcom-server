/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: basemanager.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#define CONFIGDIRECTORY		"config_dir"

#ifndef BASEMANAGER_H
#define BASEMANAGER_H

/* Funcion initComSystem
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
openComSystem				(const gchar*, gchar*);

/* Funcion closeComSystem
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
closeComSystem				(gchar*);

#endif

/*
 * VERSION 0.1
 * */
