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

/* Funcion loadAllModules
 * */
#define	CANNOTLOADMODULES		"No se pueden cargar modulos dinamicamente"
#define	NOPLUGINSAVAILABLE		"No hay plugins disponibles"

/* Funcion loadModule
 * */
#define NOMEMORYAVAILABLE		"No se ha podido reservar memoria"
#define INCORRECTPLUGINFORMAT	"El archivo no tiene formato de plugin"
#define NOSYMBOLSAVAILABLE		"No es un plugin o no tiene una definicion estandar"

/* Funcion unloadAllModules
 * */
#define FREEQUEUEFAILED			"La cola no ha sido liberada correctamente"

/* Funcion initThreads
 * */
#define THREADSNOTSUPPORTED		"El sistema no soporta la creacion de hilos"

/* Funcion createAllThreads
 * */
#define NOTENOUGHTHREADS		"No se ha creado ningun thread"

/* Funcion createThread
 * */
#define CANNOTCREATETHREAD		"No se ha podido crear el thread"
