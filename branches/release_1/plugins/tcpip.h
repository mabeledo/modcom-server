/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/tcpip.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/
#ifndef TCPIP_H
#define TCPIP_H

#include "interface.h"

/* Winsocket support */
#ifdef G_OS_WIN32
	#include <iostream.h>
	#include <winsock.h>
#endif

/* Posix socket support */
#ifdef G_OS_UNIX
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>


	#include <unistd.h>
	#include <sys/types.h> 
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <resolv.h>
	#include <net/if.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <string.h>
#endif

#endif
/*
 * VERSION 0.1
 * */
