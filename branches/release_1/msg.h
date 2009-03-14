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

/* Struct for receiving/sending messages. */
typedef struct _Message
{
	/* Protocol. */
	const gchar* proto;
	
	/* Source address. */
	const gchar* src;
	
	/* Destination address. */
	const gchar* dest;
	
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
	const gchar* data;
	
	/* MD5 checksum. */
	const gchar* checksum;
} Message;

