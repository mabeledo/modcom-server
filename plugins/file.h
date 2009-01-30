/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/file.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/
#ifndef FILE_H
#define FILE_H

#include "interface.h"

/* Win32 file change notification support */
#ifdef G_OS_WIN32
	/* TODO */
#endif

#ifdef G_OS_UNIX

	/* Linux Inotify support */
	#if defined(__LINUX__) || defined(__linux__) || defined(__linux)
		#include <sys/types.h>
		#include <sys/inotify.h>
	#endif
#endif

#endif
/*
 * VERSION 0.1
 * */
