/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: configmanager.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "general.h"
#include "data.h"

/* Error messages */
#define CANNOTINITIALIZEPARSER	"No se puede inicializar el analizador de configuracion"
#define CANNOTLOCATECONFIGFILE	"El archivo de configuracion no existe"
#define NOGROUPAVAILABLE		"El grupo no esta disponible"
#define NOGROUPSAVAILABLE		"No hay grupos disponibles"
#define NOGROUPSETAVAILABLE		"No hay grupos que contengan el patron: "

/* Funcion initConfigFile
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initConfigFile				(const gchar* filename, gchar** error)
{
	if (!g_file_test(filename, G_FILE_TEST_EXISTS))
	{
		*error = g_strdup(CANNOTLOCATECONFIGFILE);
		return (FALSE);
	}

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
loadConfigFile				(const gchar* filename, GData** dConfig, gchar** error)
{
	GKeyFile* kConfig;
	GData* auxConfig;
	gchar** groups;
	gint length;
	
	gchar** keys;
	gint i;
	gint j;
	
	kConfig = g_key_file_new();
	g_datalist_init(dConfig);
	
	if (!g_key_file_load_from_file(kConfig, filename, G_KEY_FILE_NONE, NULL))
	{
		*error = g_strdup(CANNOTINITIALIZEPARSER);
		return (FALSE);
	}

	groups = g_key_file_get_groups(kConfig, NULL);
	length = g_strv_length(groups);
	
	if (!(length > 0))
	{
		*error = g_strdup(NOGROUPSAVAILABLE);
		return (FALSE);
	}

	/* El bucle exterior recorre todos los grupos disponibles y
	 * procesa unicamente aquellos cuyo sufijo coincida con el patron.
	 * */
	for (i = 0; i < length; i++)
	{
		g_datalist_init(&auxConfig);
		keys = g_key_file_get_keys(kConfig, groups[i], NULL, NULL);
		
		/* El bucle interior toma los pares clave/valor de cada grupo
		 * y construye una estructura GData.
		 * */
		for (j = 0; j < g_strv_length(keys); j++)
		{
			g_datalist_set_data(&auxConfig, keys[j],
								g_key_file_get_value(kConfig, groups[i], keys[j], NULL));
		}
		
		g_datalist_set_data(dConfig, groups[i], auxConfig);
	}
	
	return (TRUE);
}
