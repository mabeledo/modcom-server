/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: configmanager.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "data.h"
#include "error.h"

/* Funcion initConfigFile
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initConfigFile				(const gchar* filename, gchar* error)
{
	if (!g_file_test(filename, G_FILE_TEST_EXISTS))
	{
		error = g_strdup(CANNOTLOCATECONFIGFILE);
		return (FALSE);
	}
	
	error = NULL;
	return (TRUE);
}

/* Funcion loadConfigFile
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
loadConfigFile				(const gchar* filename, GKeyFile* kConfig, gchar* error)
{
	if (!g_key_file_load_from_file(kConfig, filename, G_KEY_FILE_NONE, NULL))
	{
		error = g_strdup(CANNOTINITIALIZEPARSER);
		return (FALSE);
	}
	return (TRUE);
}
