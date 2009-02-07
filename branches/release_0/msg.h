/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: msg.h
 * 	Version: 0.1
 * 
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/
 
 #define END_OF_DATA				"^END"
 #define PROTOCOLS					{"", "NULL", "TCPIP", "FILE", "COM"}

/* Struct for receiving/sending messages. */
typedef struct _Message
{
	/*
	 * Available protocols:
	 *   NULL   - 0
	 *   TCPIP - 1
	 *   FILE   - 2
	 *   COM    - 3
	 * */
	gushort proto;
	
	/* Source address. */
	gchar* src;
	
	/* Destination address. */
	gchar* dest;
	
	/* Type.
	 * */
	gushort type;
	
	/* ID */
	gushort id;
	
	/* Part.
	 * Available values:
	 *   0 - Standalone message or end.
	 *   >0 - Part of a bigger message.
	 * */
	gushort part;
	
	/* Message contents. */
	gchar* data;
	
	/* MD5 checksum. */
	gchar checksum[32];
} Message;

