/********************************************************************
 *             Sistema modular de comunicación con un robot móvil.
 *                     Modulo de comunicacion por TCP/IP
 *
 *                        Manuel Angel Abeledo Garcia
 ********************************************************************/

#ifndef MYSOCKETS_H
	#define MYSOCKETS_H
	#ifdef POSH_OS_WIN32
		#include <winsock2.h>
	#else
		#include <sys/types.h>
		#include <sys/socket.h>
		#include <netinet/in.h>
		#include <arpa/inet.h>
		#include <netdb.h>
		#include <unistd.h>
	#endif
#endif

#include "general.h"
#include "modulos.h"
#include "mensaje.h"
#include "tcpip.h"

/* La implementacion con glib no esta clara.
 * Podria ser mejor escribir codigo para sockets BSD y utilizar
 * compilacion condicional para codigo windows
 */
gboolean
tcpip_recibir									(gpointer datos)
{
	SOCKET socket_s;
	struct sockaddr_in atrib_s;
	gchar buffer[TAM_MSJ];
	GIOChannel *canal_s;
	DATOS_MODULOS *dm = datos;
	GAsyncQueue *ref_com_hilos = g_async_queue_ref(dm->dc->com_hilos);

	if ((socket_s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_HANDLE)
	{
		g_critical("TCPIP : Fallo al crear el socket de servidor");
		return (FALSE);
	}

	canal_s = io_channel_new_for_socket(socket_s);

	atrib_s = memset(&atrib_s, 0, sizeof(destino));

	atrib_s.sin_family = AF_INET;
	atrib_s.sin_port = htons(PUERTO_S);
	atrib_s.sin_addr.s_addr = INADDR_ANY;

	if ((bind(socket_s, (struct sockaddr*) &atrib_s, sizeof(dir_s))) == -1)
	{
		g_critical("TCPIP: Fallo al enlazar el socket del servidor");
		return (FALSE);
	}

	if (listen(socket_s, 5) != 0)
	{
		g_critical("TCPIP: Fallo al establecer la escucha");
		return (FALSE);
	}

	g_debug("TCPIP: Esperando conexiones...");

	/* ¿Cuanto tiempo se pasa esperando conexiones? */
	while (g_str_equal((gconstpointer) g_async_queue_try_pop(ref_com_hilos), SALIR))
	{
		/* Solo se tienen en cuenta los estados de error, recurso ocupado y op normal
		* en teoria no deberia dar ningun EOF
		* */
		if (g_io_channel_read(canal_s, buffer, TAM_MSJ, NULL, NULL) != G_IO_STATUS_NORMAL)
		{
			g_debug("TCPIP: Mensaje descartado, no se incorpora a la cola");
		}
		else
		{
			g_debug("TCPIP: Mensaje enviado a la cola");
			g_queue_push_tail(dm->dc->cola, &(ext_int((MENSAJE_EXT)buffer)));
		}
	}

	g_debug("TCPIP: Recibida senal de fin. Terminando...")

	/* Limpieza */

	g_io_channel_shutdown(canal_s, TRUE, NULL);
	g_async_queue_unref(ref_com_hilos);
	CLOSESOCKET(socket_s);

	return (TRUE);
}

gboolean
tcpip_enviar									(gpointer datos)
{
	SOCKET socket_c;
	struct sockaddr_in atrib_s;
	gchar buffer[TAM_MSJ];
	GIOChannel *canal_c;
	MENSAJE_INT *msj_i = datos;
	MENSAJE_EXT msj_e;

	msj_e = g_strdup(int_ext(*msj_i));

	g_debug("TCPIP: Preparado para enviar mensaje:");
	g_debug("TCPIP: [%s]", msj_e);

	if ((socket_c = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_HANDLE)
	{
		g_critical("TCPIP : Fallo al crear el socket de cliente");
		return (FALSE);
	}

	canal_c = io_channel_new_for_socket(socket_c);

	/* bind */

	atrib_s = memset(&atrib_s, 0, sizeof(destino));
	atrib_s.sin_family = AF_INET;
	atrib_s.sin_port = htons(PUERTO_S);
	inet_aton(msj_i->destino, &atrib_s.sin_addr);

	if (connect(socket_c, &atrib_s, sizeof(atrib_s)) == 0)
	{
		g_critical("TCPIP: Fallo al establecer el envio de mensajes");
		return (FALSE);
	}

	g_debug("TCPIP: Esperando para enviar...");

	if (g_io_channel_write_chars(canal_c, (gchar*)msj_e, TAM_MSJ, NULL, NULL) != G_IO_STATUS_NORMAL)
	{
		g_debug("TCPIP: Mensaje no enviado");
	}
	else
	{
		g_debug("TCPIP: Mensaje enviado");
	}

	g_io_channel_shutdown(canal_c, TRUE, NULL);
	g_free(msj_e);
	CLOSESOCKET(socket_c);

	return (TRUE);
}

/*
 * VERSION 0.1
 * Solo compatible POSIX
 * Versiones posteriores incorporaran compat con Windows
 * */
