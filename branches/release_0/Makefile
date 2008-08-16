# Makefile

# Global variables
CC=gcc
CFLAGS=`pkg-config --cflags glib-2.0` `pkg-config --cflags gmodule-2.0` `pkg-config --cflags gthread-2.0` -Wall -c
LDFLAGS=`pkg-config --libs glib-2.0` `pkg-config --libs gmodule-2.0` `pkg-config --libs gthread-2.0`
SOURCES=basemanager.c threadmanager.c configmanager.c modulemanager.c pluginmanager.c modcom-server.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=modcom-server

# Command format:
# target: dependencies
# [tab] system_command

all: $(SOURCES) $(EXECUTABLE)
			
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	
.c.o:
	$(CC) $(LDFLAGS) $(CFLAGS) $< -g -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
