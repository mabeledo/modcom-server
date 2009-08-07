# Makefile

# Global variables
# x86 GCC
CC=gcc
# ARM GCC
#CC=armv4tl-softfloat-linux-gnueabi-gcc
CFLAGS=-Wall -g -g3 -ggdb `pkg-config --cflags glib-2.0` -fPIC -c
LDFLAGS=-Wall -g -g3 -ggdb `pkg-config --libs glib-2.0` -lc -shared -Wl,-soname,

# Command format:
# target: dependencies
# [tab] system_command

null:
	$(CC) $(CFLAGS) null.c
	$(CC) $(LDFLAGS)libnull.so.1 -o null.so null.o 

file:
	$(CC) $(CFLAGS) file.c
	$(CC) $(LDFLAGS)libfile.so.1 -o file.so file.o 

com:
	$(CC) $(CFLAGS) com.c
	$(CC) $(LDFLAGS)libcom.so.1 -o com.so com.o 

tcpip:
	$(CC) $(CFLAGS) tcpip.c
	$(CC) $(LDFLAGS)libtcpip.so.1 -o tcpip.so tcpip.o 

bluetooth:
	$(CC) $(CFLAGS) -lbluetooth bluetooth.c
	$(CC) $(LDFLAGS)libbluetooth.so.1 -lbluetooth -o bluetooth.so bluetooth.o 
	
