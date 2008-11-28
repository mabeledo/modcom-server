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

/* Funcion getGroupConfig
 * Obtiene la configuracion de un grupo dentro de una estructura tipo
 * GKeyFile y le da formato de diccionario en una estructura GData, esto
 * es, un conjunto de pares clave/valor.
 *  - Entrada: puntero a estructura GKeyFile, nombre del grupo.
 *  - Salida: puntero a estructura GData con pares clave/valor.
 * */
GData*
getGroupConfig				(GKeyFile* kConfig, const gchar* group, gchar* error)
{	
	GData* gConfig;
	gchar** keys;
	gint i;
	
	g_datalist_init(&gConfig);
	
	if (!g_key_file_has_group(kConfig, group))
	{
		error = g_strconcat(NOGROUPAVAILABLE, group, NULL);
		return (NULL);
	}
	
	keys = g_key_file_get_keys(kConfig, group, NULL, NULL);
	
	for (i = 0; i < g_strv_length(keys); i++)
	{
		g_datalist_set_data(&gConfig, keys[i],
							g_key_file_get_value(kConfig, group, keys[i], NULL));
	}
	
	error = NULL;
	return (gConfig);
}

/* Funcion getGroupSetConfig
 * Obtiene un diccionario GData con pares clave/valor constituidos por
 * nombres cuyo sufijo sea el parametro de entrada "pattern", y
 * estructuras GData formadas a partir de los grupos cuyo nombre sea
 * la clave mas el patron. Ej. null-plugin/GData (grupo null-plugin)
 *  - Entrada: estructura GKeyFile con la configuracion de un fichero,
 * 			   cadena de texto con un patron.
 *  - Salida: Diccionario de diccionarios (Superestructura GData que
 * 			  contiene estructuras GData).
 * */
GData*
getGroupSetConfig				(GKeyFile* kConfig, const gchar* pattern, gchar* error)
{
	GData* sConfig;
	GData* auxConfig;
	gchar** groups;
	gint length;
	
	gchar** keys;
	gint i;
	gint j;
	
	g_datalist_init(&sConfig);
	groups = g_key_file_get_groups(kConfig, NULL);
	length = g_strv_length(groups);
	
	if (!(length > 0))
	{
		error = g_strconcat(NOGROUPSAVAILABLE, pattern, NULL);
		return (NULL);
	}
	
	/* El bucle exterior recorre todos los grupos disponibles y
	 * procesa unicamente aquellos cuyo sufijo coincida con el patron.
	 * */
	for (i = 0; i < length; i++)
	{
		if (g_str_has_suffix(groups[i], pattern))
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

			/* Las estructuras GData construidas en el bucle interior
			 * son integradas en una superestructura del mismo tipo,
			 * donde la clave es el nombre del grupo que coincide con
			 * el patron dado y el valor es una estructura GData.
			 * */
			g_datalist_set_data(&sConfig, g_strsplit(groups[i], "-", 2)[0], auxConfig);
		}
	}
	
	error = NULL;
	return (sConfig);
}
