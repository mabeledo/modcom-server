/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: error.h
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#define	CANNOTOPENDIRECTORY 	"No se ha podido abrir el directorio"

/*
 * filemanager
 * */

#define CANNOTLOCATEDIR			"El directorio de los complementos no existe"
#define NOPLUGINFILESAVAILABLE	"No hay archivos de complemento disponibles"
#define CANNOTLOCATECONFIGFILE	"El archivo de configuracion no existe"

/* 
 * modulemanager 
 * */

#define	CANNOTLOADMODULES		"No se pueden cargar modulos dinamicamente"
#define	NOMODULESAVAILABLE		"No hay modulos disponibles"
#define NOMEMORYAVAILABLE		"No se ha podido reservar memoria"
#define INCORRECTMODULEFORMAT	"El archivo no tiene formato de modulo"
#define NOSYMBOLSAVAILABLE		"No es un plugin o no tiene una definicion estandar"
#define FREEQUEUEFAILED			"La cola no ha sido liberada correctamente"

/* 
 * threadmanager 
 * */

#define THREADSNOTSUPPORTED		"El sistema no soporta la creacion de hilos"
#define NOTENOUGHTHREADS		"No se ha creado ningun thread"
#define CANNOTCREATETHREAD		"No se ha podido crear el thread"
