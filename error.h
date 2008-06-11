/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: error.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#define	CANNOTOPENDIRECTORY 	"No se ha podido abrir el directorio"

/* moduleLoader.c */

/* Funcion loadModules
 * */
#define	CANNOTLOADMODULES		"No se pueden cargar modulos dinamicamente"
#define	NOAVAILABLEPLUGINS		"No hay plugins disponibles"

/* Funcion loadSymbols
 * */
#define NOAVAILABLESYMBOLS		"El plugin carece de funcionalidad"

/* Funcion initThreads
 * */
#define THREADSNOTSUPPORTED		"El sistema no soporta la creacion de hilos"

/* Funcion createAllThreads
 * */
#define NOTENOUGHTHREADS		"No se ha creado ningun thread"
