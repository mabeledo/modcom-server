## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(MODCOM_SERVER_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = modcom_server

modcom_server_SOURCES = \
	data.h         \
	error.h         \
	modcom-server.c         \
	msg.h         \
	pluginloader.c         \
	pluginloader.h         \
	threadmanager.c         \
	threadmanager.h

modcom_server_LDFLAGS = 

modcom_server_LDADD = $(MODCOM_SERVER_LIBS)

