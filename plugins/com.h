/********************************************************************
 *             Sistema modular de comunicación con un robot móvil.
 *                     Modulo de comunicacion puerto COM
 *
 *                        Manuel Angel Abeledo Garcia
 ********************************************************************/

#ifdef G_OS_UNIX
	#define io_channel_new_fd(fd) g_io_channel_unix_new(fd)
#elif
	#ifdef G_OS_WIN32
		#define io_channel_new_fd(fd) g_io_channel_win32_new_fd(fd)
	#endif
#endif

/* Funcion com_recibir.
 * Maneja mensajes entrantes (COM) y los envia a la cola.
 * Argumentos:
 *	- datos, un puntero a una estructura de tipo DATOS_MODULOS.
 * Devuelve:
 * 	- Un puntero a un mensaje de estado.
 * */

gpointer
com_recibir									(gpointer);

/* Funcion com_enviar.
 * Maneja mensajes salientes (COM) y los envia a su destino.
 * Argumentos:
 *	- datos, un puntero a una estructura de tipo MENSAJE_INT.
 * Devuelve:
 * 	- Un puntero a un mensaje de estado.
 * */

gpointer
com_enviar									(gpointer);

/*
 * VERSION 0.1
 * */
