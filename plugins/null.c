/********************************************************************
 *  Project: modcom-server
 *  File: plugins/null.c
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

#include "null.h"

#define PLUGINPROTO		"NULL"
#define PLUGINADDRESS	"null"
#define PLUGINNAME		"Null"
#define PLUGINDESC		"Sin funcionalidad"
#define PLUGINVERSION	"0.1"

const gchar*
pluginProto							()
{
	return (PLUGINPROTO);
}

const gchar*
pluginAddress						()
{
	return (PLUGINADDRESS);
}

const gchar*
pluginName							()
{
	return (PLUGINNAME);
}

const gchar*
pluginDesc							()
{
	return (PLUGINDESC);
}

const gchar*
pluginVersion						()
{
	return (PLUGINVERSION);
}

gboolean
pluginInit							(gpointer data, gchar** error)
{
	g_debug("NULL plugin loaded and initialized");
	return (TRUE);
}

gboolean
pluginSend							(gpointer dest, gpointer data, gchar** error)
{
	return (TRUE);
}

gpointer
pluginReceive						(gpointer data)
{
	return (NULL);
}

gboolean
pluginExit							(gchar** error)
{
	return (TRUE);
}

/*
 * VERSION 0.1
 * */
