# Makefile

# Global variables
CC=gcc
CFLAGS=`pkg-config --cflags glib-2.0` -fPIC -g -Wall -c
LDFLAGS=`pkg-config --libs glib-2.0` -lc -shared -Wl,-soname,

# Command format:
# target: dependencies
# [tab] system_command

null:
	$(CC) $(CFLAGS) null.c
	$(CC) $(LDFLAGS)libnull.so.1 -o null.so null.o 

tcpip:
	$(CC) $(CFLAGS) tcpip.c
	$(CC) $(LDFLAGS)libtcpip.so.1 -o tcpip.so tcpip.o 
	
