/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: msg.h
 * 	Version: 1.0
 * 
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

/* Chunk size in bytes. */
#define CHUNKLEN		4092
 
typedef struct _Message
{
	const gchar* srcProto;
	const gchar* srcAddress;
	GAsyncQueue* qChunks;
} Message;
