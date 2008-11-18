/********************************************************************
 *             Sistema modular de comunicación con un robot móvil.
 *                       Modulo de comunicacion TCP/IP
 *
 *                        Manuel Angel Abeledo Garcia
 ********************************************************************/

#define PUERTO_S								9000
#define PUERTO_C								9001

#if defined POSH_OS_WIN32
	#define CLOSESOCKET(s) closesocket(s)
#else
	#define INVALID_HANDLE -1
	#define INVALID_SOCKET -1
	#define CLOSESOCKET(s) close(s)
	typedef gint SOCKET;
#endif

/* Funcion tcpip_recibir.
 * Maneja mensajes entrantes (TCP/IP) y los envia a la cola.
 * Argumentos:
 *	- datos, un puntero a una estructura de tipo DATOS_MODULOS.
 * Devuelve:
 * 	- Un puntero a un mensaje de estado.
 * */

gboolean
tcpip_recibir									(gpointer);

/* Funcion tcpip_enviar.
 * Maneja mensajes salientes (TCP/IP) y los envia a su destino.
 * Argumentos:
 *	- datos, un puntero a una estructura de tipo MENSAJE_INT.
 * Devuelve:
 * 	- Un puntero a un mensaje de estado.
 * */

gboolean
tcpip_enviar									(gpointer);

/*
 * VERSION 0.1
 * */
