# Makefile

# Global variables
# x86 GCC
CC=gcc
# ARM GCC
# CC=armv4tl-softfloat-linux-gnueabi-gcc
CFLAGS=-Wall -g `pkg-config --cflags glib-2.0` -fPIC -c
LDFLAGS=-Wall -g `pkg-config --libs glib-2.0` -shared -lc -Wl,-soname,

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

# Linux only
bluetooth:
	$(CC) $(CFLAGS) -DLINUX bluetooth.c
	$(CC) -lbluetooth $(LDFLAGS)libbluetooth.so.1 -DLINUX -o bluetooth.so bluetooth.o 
	
