/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: msg.h
 * 	Version: 0.1
 * 
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/
 
 #define END_OF_DATA					"^END"

/* Struct for receiving/sending messages. */
typedef struct _Message
{
	/* Source protocol.
	 * Available protocols:
	 *   NULL   - 1
	 *   TCP/IP - 2
	 *   FILE   - 3
	 *   COM    - 4
	 * */
	gushort srcProto;
	
	/* Source address. */
	gchar src[15];
	
	/* Destination protocol. */
	gushort destProto;
	
	/* Destination address. */
	gchar dest[15];
	
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
	gchar data[1024];
	
	/* MD5 checksum. */
	gchar checksum[32];
} Message;
