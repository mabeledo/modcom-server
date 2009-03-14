# Makefile

# Global variables
CC=gcc -Wall -ggdb
CFLAGS=`pkg-config --cflags glib-2.0` -fPIC -g -c
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

file:
	$(CC) $(CFLAGS) file.c
	$(CC) $(LDFLAGS)libfile.so.1 -o file.so file.o 

com:
	$(CC) $(CFLAGS) com.c
	$(CC) $(LDFLAGS)libcom.so.1 -o com.so com.o 
	
