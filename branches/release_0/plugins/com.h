/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/com.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/
#ifndef COM_H
#define COM_H

#include "plugin.h"

#ifdef G_OS_WIN32

#endif

#ifdef G_OS_UNIX
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <strings.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <termios.h>
	#include <stdio.h>
#endif

#endif
/*
 * VERSION 0.1
 * */


