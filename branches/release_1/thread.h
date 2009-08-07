/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: thread.h
 * 	Version: 0.1
 *
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

typedef struct _ThreadData
{
	GAsyncQueue* qMessages;
	
	/* This should be used only with g_atomic_int_set() 
	 * when it's modified in a thread. 
	 * */
	gboolean* exitFlag;
} ThreadData;

typedef struct _ExtThreadData
{
	GData** dPlugins;
	ThreadData* tData;
} ExtThreadData;
