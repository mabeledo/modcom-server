/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: thread.h
 * 	Version: 0.1
 *
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

/*
 * Struct containing all data needed for creating receive or
 * dispatch threads. 
 * */
typedef struct _ThreadData
{
	GQueue* qPlugins;
	GAsyncQueue* qMessages;
} ThreadData;
