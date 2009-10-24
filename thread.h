/********************************************************************
 *  Project: modcom-server
 *  File: thread.h
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

typedef struct _ThreadData
{
	GAsyncQueue* qMessages;
	
	/* This should be used only with g_atomic_int_set() 
	 * when it's modified in a thread. 
	 * */
	gboolean* exitFlag;
} ThreadData;

typedef struct _ExtThreadData
{
	GData** dPlugins;
	ThreadData* tData;
} ExtThreadData;
