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

/* Funcion initBaseSystem
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
initBaseSystem				(const gchar*, gchar**);

/* Funcion writeMessage
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
writeMessage				(gchar*, gchar*, gchar*, gchar**);

/* Funcion readMessage
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
readMessage					(gchar**, gchar**);

/* Funcion closeBaseSystem
 * Precondiciones:
 * Postcondiciones:
 * Entrada: 
 * Salida: 
 * Proceso: 
 * */
gboolean
closeBaseSystem				(gchar**);

#endif

/*
 * VERSION 0.1
 * */
