/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/com.c
 * 	Version: 1.0
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

/* Plugin main attributes. */
#define PLUGINPROTO			"COM"
#define PLUGINNAME			"COM"
#define PLUGINDESC			"Serial port plugin"
#define PLUGINVERSION		"1.0"

#define BAUDRATE			B115200
#define COM_EOF				"^EOF"

#include "common.h"
#include "com.h"
#include <glib/gprintf.h>
#include <string.h>

#ifdef G_OS_WIN32

#endif

#ifdef G_OS_UNIX
	#define _GNU_SOURCE
	#include <sys/select.h>
	#include <sys/time.h>
	#include <strings.h>
	#include <unistd.h>
	#include <termios.h>
	#include <stdio.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
#endif

/* Error messages. */
#define NODEVICEAVAILABLE	"There is no device available"
#define OPENDEVICEFAILED	"Error opening device"
#define SETRAWONCHANNEL		"Error setting raw mode on channel."
#define INPUTERROR			"Input error: "
#define CHANOPTERROR		"Error setting up channel"
#define CHANCODIFERROR		"Error setting channel codification"
#define CHANSHUTDOWNERROR	"Error closing channel"
#define CHANOPENERROR		"Error opening channel"
#define READERROR			"Error reading data from channel"
#define SENDERROR			"Error sending data over the channel"
#define FLUSHERROR			"Error flushing data"

/* Global variables. */

GMutex * mutex;

#ifdef G_OS_UNIX
	/* Previous serial port configuration. */
	static struct termios* oldCfg;
	
	/* New serial port configuration. */
	static struct termios* newCfg;
	
	/* Device file descriptor and name. */
	static gint* fd;
	static gchar** devices;
	
	/* Number of descriptors to manage. */
	static gint fdLength;
	
	/* Highest-numbered file descriptor plus 1. */
	static gint maxFd;
	
	/* File descriptor set for select() */
	static fd_set* set;
#endif

gboolean
pluginInit							(gpointer data, gchar** error)
{
	GData* comConfig = (GData*)data;
	gint i;
	
	mutex = g_mutex_new();
	
	/* Load configuration parameters */
	if (g_datalist_get_data(&comConfig, "devices") != NULL)
	{
		devices = g_strsplit((gchar*)g_datalist_get_data(&comConfig, "devices"), ",", 0);
	}
	else
	{
		*error = g_strconcat(PLUGINNAME, " - ", NODEVICEAVAILABLE, NULL);
		return (FALSE);
	}
	
	/*
	 * Serial port configuration.
	 * */
	
	#ifdef G_OS_UNIX
		/* UNIX plugin implementation is a little bit tricky because it
		 * handles the serial port as a file and needs a bunch of
		 * parameters. Serial port is opened first and then configured, and
		 * it should stay opened until the process ends.
		 * */
		
		fdLength = g_strv_length(devices);
		
		maxFd = 0;
		fd = g_new0(gint, fdLength);
		oldCfg = g_new0(struct termios, fdLength);
		newCfg = g_new0(struct termios, fdLength);
		set = g_new0(fd_set, 1);
		FD_ZERO(set);
	
		for (i = 0; i < fdLength; i++)
		{
			/* Open serial port only for reading and not as controlling
			 * tty, in order to avoid get killed with CTRL+C.
			 * */
			if ((fd[i] = open(devices[i], O_RDWR | O_NOCTTY | O_SYNC)) < 0)
			{
				*error = g_strconcat(PLUGINNAME, " - ", OPENDEVICEFAILED, NULL);
				return (FALSE);
			}
			
			if (fd[i] > maxFd)
			{
				maxFd = fd[i];
			}
			
			/* Save current serial port settings and clear the structure
			 * to receive new.
			 * */
			tcgetattr(fd[i], &oldCfg[i]);
			bzero(&newCfg[i], sizeof(newCfg));
			
			/* Set BAUDRATE (bps rate), CS8 (8 bit, no parity, 1 stopbit),
			 * CLOCAL (local reception, no modem control), CREAD
			 * (enable receiving characters).
			 * */
			newCfg[i].c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
			
			/* Set IGNPAR (ignore bits with parity errors),
			 * non canonical processing.
			 * */
			newCfg[i].c_iflag = IGNPAR;
			newCfg[i].c_oflag = 0;
			//newCfg[i].c_lflag = ~ICANON;
			newCfg[i].c_lflag = 0;
			
			/* Clean inter-character timer. Dont block.
			 * */
			newCfg[i].c_cc[VTIME] = 0;
			newCfg[i].c_cc[VMIN] = 0;
			
			//newCfg[i].c_cc[VINTR] = 0;
			//newCfg[i].c_cc[VQUIT] = 0;
			//newCfg[i].c_cc[VERASE] = 0;
			//newCfg[i].c_cc[VKILL] = 0;
			//newCfg[i].c_cc[VEOF] = 4;
			//newCfg[i].c_cc[VTIME] = 0;
			//newCfg[i].c_cc[VSWTC] = 0;
			//newCfg[i].c_cc[VSTART] = 0;
			//newCfg[i].c_cc[VSTOP] = 0;
			//newCfg[i].c_cc[VSUSP] = 0;
			//newCfg[i].c_cc[VEOL] = 0;
			//newCfg[i].c_cc[VREPRINT] = 0;
			//newCfg[i].c_cc[VDISCARD] = 0;
			//newCfg[i].c_cc[VWERASE] = 0;
			//newCfg[i].c_cc[VLNEXT] = 0;
			//newCfg[i].c_cc[VEOL2] = 0;
			
			/* Clean the modem line and activate the settings. */
			tcflush(fd[i], TCIOFLUSH);
			tcsetattr(fd[i], TCSANOW, &newCfg[i]);
			
			FD_SET(fd[i], set);
		}
		
		maxFd++;
	#endif
	
	/* Free memory. */
	g_datalist_clear(&comConfig);

	g_debug("%s - Plugin loaded and initialized", PLUGINNAME);
	return (TRUE);
}

gboolean
pluginSend							(gpointer dest, gpointer data, gchar** error)
{
	Message* msg = (Message*)data;
	gchar* destAddress = (gchar*)dest;
	
	GIOChannel* channel;
	GError* chanError;
	gint i;
	GTimer* timeElapsed = g_timer_new();
	
	#ifdef G_OS_UNIX
		/* Seeks for the device ready to send data.
		 * Previously, comparison was done with g_str_equal, but this
		 * function seems to crash in ARM.	
		 * */
		for (i = 0; (g_strcmp0(destAddress, devices[i]) == 0) && (i < fdLength); i++);
		
		/* Create a new io channel to read data. */
		if ((channel = g_io_channel_unix_new(fd[i])) == NULL)
		{
			*error = g_strconcat(PLUGINNAME, " - ", CHANOPENERROR, NULL);
			return (FALSE);
		}
		
		chanError = NULL;
		
		/* Set a NULL encoding to read raw data. */
		if (g_io_channel_set_encoding(channel, NULL, &chanError) == G_IO_STATUS_ERROR)
		{
			*error = g_strconcat(PLUGINNAME, " - ", CHANOPTERROR, ": ", chanError->message, NULL);
			return (FALSE);
		}
		
		g_timer_start(timeElapsed);
		
		// ADDED
		tcflush(fd[i], TCOFLUSH);
		
		/* Writes data on channel. */
		if (g_io_channel_write_chars(channel, msg->chunk, msg->chunkLen, NULL, &chanError) == G_IO_STATUS_ERROR)
		{
			*error = g_strconcat(PLUGINNAME, " - ", SENDERROR, ": ", chanError->message, NULL);
			return (FALSE);
		}
		
		/* Flushes channel.
		 * GLib function is broken in ARM.
		 * */
		/* if (g_io_channel_flush(channel, &chanError) == G_IO_STATUS_ERROR)
		{
			*error = g_strconcat(PLUGINNAME, " - ", FLUSHERROR, NULL);
			return (FALSE);
		}*/
		/*if (tcflush(fd[i], TCIOFLUSH) != 0)
		{
			*error = g_strconcat(PLUGINNAME, " - ", FLUSHERROR, ": ", chanError->message, NULL);
			return (FALSE);
		}*/
		
		/* Closes channel. */
		if (g_io_channel_shutdown(channel, TRUE, &chanError) == G_IO_STATUS_NORMAL)
		{
			g_mutex_lock(mutex);
			
			/* Workaround to flush data on serial port.
			 * By unknown reason, g_io_channel_flush()
			 * breaks processing with a segmentation fault.
			 * Closing and re-opening flushes channel and maintains
			 * serial port active.
			 * */
			g_io_channel_unref(channel);
			if ((fd[i] = open(devices[i], O_RDWR | O_NOCTTY | O_SYNC)) < 0)
			{
				*error = g_strconcat(PLUGINNAME, " - ", OPENDEVICEFAILED, NULL);
				return (FALSE);
			}
			
			g_mutex_unlock(mutex);
		}
		else
		{
			*error = g_strconcat(PLUGINNAME, " - ", CHANSHUTDOWNERROR, ": ", chanError->message, NULL);
			return (FALSE);              
		}
		
		g_timer_stop(timeElapsed);
		g_debug("%s - %d bytes sent in %lf seconds", PLUGINNAME, msg->chunkLen, g_timer_elapsed(timeElapsed, NULL));
		g_timer_destroy(timeElapsed);

	#endif
	
	return (TRUE);
}

gpointer
pluginReceive						(gpointer data)
{
	ThreadData* tData;
	Message* msg;
	
	GIOChannel* channel;
	GError* chanError;
	gchar* error;
	GTimer* timeElapsed = g_timer_new();
	
	gsize dataLength, bufferLength;
	gchar* buffer;
	GString* temp;

	tData = data;
	tData->qMessages = g_async_queue_ref(tData->qMessages);
	
	#ifdef G_OS_UNIX
		gint i, readyFd;
		
		g_debug("%s - Accepting new connections", PLUGINNAME);
	
		/* Main loop. */
		while (!g_atomic_int_get(tData->exitFlag))
		{
			
			/* Waits for file descriptors ready to be read.
			 * There is no time limit, nor other sets to be watched
			 * but a read set.
			 *  */
			readyFd = select(maxFd, set, NULL, NULL, NULL);

			for (i = 0; (i < fdLength) && (readyFd > 0); i++)
			{
				/* There is at least one descriptor waiting to be read. */
				if (FD_ISSET(fd[i], set))
				{
					/* Initializes a new struct to hold all the data readed. */
					msg = g_new0(Message, 1);
					msg->srcProto = g_strdup(PLUGINPROTO);
					msg->srcAddress = g_strdup(devices[i]);
					msg->chunkLen = 0;
					

					/* Create a new io channel to read data. */
					if ((channel = g_io_channel_unix_new(fd[i])) == NULL)
					{
						error = g_strconcat(PLUGINNAME, " - ", CHANOPENERROR, NULL);
						return ((gpointer)error);
					}
					
					/* Workaround. g_io_channel_set_encoding() fails many times. */
					chanError = NULL;
					
					/* Set a NULL encoding to read raw data. */
					if (g_io_channel_set_encoding(channel, NULL, &chanError) == G_IO_STATUS_ERROR)
					{
						error = g_strconcat(PLUGINNAME, " - ", CHANCODIFERROR, NULL);
						return ((gpointer)error);
					}
					
					g_timer_start(timeElapsed);

					/* Read the data length. */
					if (g_io_channel_read_line(channel, &buffer, &dataLength, NULL, &chanError) == G_IO_STATUS_ERROR)
					{
						error = g_strconcat(PLUGINNAME, " - ", READERROR, NULL);
						return ((gpointer)error);
					}

					tcflush(fd[i], TCIFLUSH);
					dataLength = g_strtod(buffer, NULL);
					g_free(buffer);		

					if ((dataLength > 0) && (dataLength < MAXCHUNKLEN))
					{
						temp = g_string_new("");
						
						while (msg->chunkLen < dataLength)
						{
							if (g_io_channel_read_to_end(channel, &buffer, &bufferLength, &chanError) == G_IO_STATUS_ERROR)
							{
								error = g_strconcat(PLUGINNAME, " - ", READERROR, NULL);
								return ((gpointer)error);
							}
														
							
							
							temp = g_string_append(temp, buffer);
							msg->chunkLen += bufferLength;
							g_free(buffer);
						}
						
						/* Truncate to the declared size. */
						if (msg->chunkLen != dataLength)
						{
							temp = g_string_truncate(temp, dataLength);
							msg->chunkLen = dataLength;
						}

						msg->chunk = g_string_free(temp, FALSE);
						g_async_queue_push(tData->qMessages, msg);

						if (chanError != NULL)
						{
							g_warning("%s - %s: %s", PLUGINNAME, READERROR, chanError->message);
							g_clear_error(&chanError);
						}
						
						g_timer_stop(timeElapsed);
						g_debug("%s - %d bytes read in %lf seconds", PLUGINNAME, msg->chunkLen, g_timer_elapsed(timeElapsed, NULL));
						g_timer_reset(timeElapsed);
					}
					
					tcflush(fd[i], TCIFLUSH);
					
					/* Clean opened file descriptor. */
					if (g_io_channel_shutdown(channel, TRUE, &chanError) == G_IO_STATUS_NORMAL)
					{
						g_mutex_lock(mutex);
						
						/* Workaround to flush data on serial port.
						 * By unknown reason, g_io_channel_flush()
						 * breaks processing with a segmentation fault.
						 * Closing and re-opening flushes channel and maintains
						 * serial port active.
						 * */
						g_io_channel_unref(channel);
						
						if ((fd[i] = open(devices[i], O_RDWR | O_NOCTTY | O_SYNC)) < 0)
						{
							error = g_strconcat(PLUGINNAME, " - ", OPENDEVICEFAILED, NULL);
							return ((gpointer)error);
						}
						
						g_mutex_unlock(mutex);
					}
					else
					{
						error = g_strconcat(PLUGINNAME, " - ", CHANSHUTDOWNERROR, ": ", chanError->message, NULL);
						return ((gpointer)error);              
					}				
				}
			}
			
			if (readyFd < 0)
			{
				error = g_strconcat(PLUGINNAME, " - ", NODEVICEAVAILABLE, NULL);
				return ((gpointer)error);
			}
			
		}
	#endif

	/* Clean and exit. */
	g_async_queue_unref(tData->qMessages);
	g_debug("%s: Receiving thread terminated", PLUGINNAME);
	
	return (NULL);	
}

gboolean
pluginExit							(gchar** error)
{
	gint i;

	#ifdef G_OS_UNIX
		for (i = 0; i < fdLength; i++)
		{
			tcsetattr(fd[i], TCSANOW, &oldCfg[i]);
			close(fd[i]);
		}
	#endif
	
	/* Free memory. */
	g_strfreev(devices);
	g_free(newCfg);
	g_free(oldCfg);
	
	return (TRUE);
}
