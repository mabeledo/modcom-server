/********************************************************************
 *  Project: modcom-server
 *  File: config.c
 *  Version: 1.0b
 * 
 *  Author: Manuel Angel Abeledo Garcia
 *
 ************************ License ***********************************
 * 
 *  This file is part of modcom-server.
 *
 *  modcom-server is free software: you can redistribute it and/or 
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2 of
 *  the License, or (at your option) any later version.
 *
 *  modcom-server is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with modcom-server.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 ********************************************************************/

#include "general.h"

#define MODULENAME				"Config"

/* Error messages */
#define CANNOTINITIALIZEPARSER	"No se puede inicializar el analizador de configuracion"
#define CANNOTLOCATECONFIGFILE	"El archivo de configuracion no existe"
#define NOGROUPAVAILABLE		"El grupo no esta disponible"
#define NOGROUPSAVAILABLE		"No hay grupos disponibles"
#define NOGROUPSETAVAILABLE		"No hay grupos que contengan el patron: "

/* Funcion initConfig
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
initConfig					(const gchar* filename, gchar** error)
{
	if (!g_file_test(filename, G_FILE_TEST_EXISTS))
	{
		*error = g_strconcat(MODULENAME, " - ", CANNOTLOCATECONFIGFILE, NULL);
		return (FALSE);
	}

	return (TRUE);
}

/* Funcion loadConfig
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
loadConfig					(const gchar* filename, GData** dConfig, gchar** error)
{
	GKeyFile* kConfig;
	GData* auxConfig;
	gchar** groups;
	
	gchar** keys;
	guint i, j, length;
	
	kConfig = g_key_file_new();
	g_datalist_init(dConfig);
	
	if (!g_key_file_load_from_file(kConfig, filename, G_KEY_FILE_NONE, NULL))
	{
		*error = g_strconcat(MODULENAME, " - ", CANNOTINITIALIZEPARSER, NULL);
		return (FALSE);
	}

	groups = g_key_file_get_groups(kConfig, NULL);
	length = g_strv_length(groups);
	
	if (!(length > 0))
	{
		*error = g_strconcat(MODULENAME, " - ", NOGROUPSAVAILABLE, NULL);
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
	
	g_debug("%s - Configuration loaded", MODULENAME);
	return (TRUE);
}

/* Funcion closeConfig
 * Precondiciones:
 * Postcondiciones:
 * Entrada:
 * Salida:
 * Proceso:
 * */
gboolean
closeConfig					()
{
	
	return (TRUE);
}
