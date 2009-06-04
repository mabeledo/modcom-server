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

#include "common.h"
#include "com.h"
#include "../msg.h"
#include "../thread.h"

#ifdef G_OS_WIN32

#endif

#ifdef G_OS_UNIX
	#include <sys/select.h>
	#include <sys/time.h>
	#include <strings.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <termios.h>
	#include <stdio.h>
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
			if ((fd[i] = open(devices[i], O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
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
			newCfg[i].c_cflag = B115200 | CS8 | CLOCAL | CREAD;
			
			/* Set IGNPAR (ignore bits with parity errors) and ICRNL (map CR,
			 * Windows end of line, to NL, UNIX end of line).
			 * */
			newCfg[i].c_iflag = IGNPAR | ICRNL;
			newCfg[i].c_oflag = 0;
			newCfg[i].c_lflag = ICANON;
			
			/* Initialize all control characters. */
			newCfg[i].c_cc[VINTR] = 0;
			newCfg[i].c_cc[VQUIT] = 0;
			newCfg[i].c_cc[VERASE] = 0;
			newCfg[i].c_cc[VKILL] = 0;
			newCfg[i].c_cc[VEOF] = 4;
			newCfg[i].c_cc[VTIME] = 0;
			newCfg[i].c_cc[VMIN] = 1;
			newCfg[i].c_cc[VSWTC] = 0;
			newCfg[i].c_cc[VSTART] = 0;
			newCfg[i].c_cc[VSTOP] = 0;
			newCfg[i].c_cc[VSUSP] = 0;
			newCfg[i].c_cc[VEOL] = 0;
			newCfg[i].c_cc[VREPRINT] = 0;
			newCfg[i].c_cc[VDISCARD] = 0;
			newCfg[i].c_cc[VWERASE] = 0;
			newCfg[i].c_cc[VLNEXT] = 0;
			newCfg[i].c_cc[VEOL2] = 0;
			
			/* Clean the modem line and activate the settings. */
			tcflush(fd[i], TCIFLUSH);
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
	
	#ifdef G_OS_UNIX
		/* Seeks for the device ready to send data. */
		for (i = 0; !g_str_equal(destAddress, devices[i]) && (i < fdLength); i++);
		
		/* Create a new io channel to read data. */
		if ((channel = g_io_channel_unix_new(fd[i])) == NULL)
		{
			*error = g_strconcat(PLUGINNAME, " - ", CHANOPENERROR, NULL);
			return (FALSE);
		}
		
		chanError = NULL;
		
		/* Set a NULL encoding to read raw data. */
		if (g_io_channel_set_encoding(channel, NULL, &chanError) != G_IO_STATUS_NORMAL)
		{
			*error = g_strconcat(PLUGINNAME, " - ", CHANOPTERROR, ": ", chanError->message, NULL);
			return (FALSE);
		}
						
		/* Writes data on channel. */
		if (g_io_channel_write_chars(channel, msg->chunk, msg->chunkLen, NULL, &chanError) != G_IO_STATUS_NORMAL)
		{
			*error = g_strconcat(PLUGINNAME, " - ", SENDERROR, ": ", chanError->message, NULL);
			return (FALSE);
		}
		
		/* Flushes channel. */
		if (g_io_channel_flush(channel, &chanError) != G_IO_STATUS_NORMAL)
		{
			*error = g_strconcat(PLUGINNAME, " - ", FLUSHERROR, ": ", chanError->message, NULL);
			return (FALSE);
		}
		
		/* Closes channel. 
		if (g_io_channel_shutdown(channel, TRUE, &chanError) != G_IO_STATUS_NORMAL)
		{
			*error = g_strconcat(PLUGINNAME, " - ", CHANSHUTDOWNERROR, ": ", chanError->message, NULL);
			return (FALSE);
		}
		
		g_io_channel_unref(channel);*/
	#endif
	
	return (TRUE);
}

gpointer
pluginReceive						(gpointer data)
{
	ThreadData* tData;
	Message* msg;
	
	GIOChannel* channel;
	GIOStatus devStatus;
	GError* chanError;
	gchar* error;
	
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
						g_warning("%s", error);
						return ((gpointer)error);
					}
					
					/* Workaround. g_io_channel_set_encoding() fails many times. */
					chanError = NULL;
					
					/* Set a NULL encoding to read raw data. */
					if (g_io_channel_set_encoding(channel, NULL, &chanError) != G_IO_STATUS_NORMAL)
					{
						error = g_strconcat(PLUGINNAME, " - ", CHANCODIFERROR, NULL);
						g_warning("%s", error);
						return ((gpointer)error);
					}

					/* Reads *lines* from channel and pushes each chunk into the chunk queue. */
					if ((devStatus = g_io_channel_read_line(channel, &msg->chunk, &msg->chunkLen, NULL, &chanError)) == G_IO_STATUS_NORMAL)
					{
						g_warning("chunk: %s", msg->chunk);
						g_async_queue_push(tData->qMessages, msg);
					}
					
					if (chanError != NULL)
					{
						g_warning("%s - %s: %s", PLUGINNAME, READERROR, chanError->message);
						g_clear_error(&chanError);
					}
				}
			}

			if (readyFd < 0)
			{
				error = g_strconcat(PLUGINNAME, " - ", NODEVICEAVAILABLE, NULL);
				g_warning("%s", error);
				return ((gpointer)error);
			}
			
		}
		
		/* Clean opened file descriptor.
		if (g_io_channel_shutdown(channel, TRUE, &chanError) != G_IO_STATUS_NORMAL)
		{
			error = g_strconcat(PLUGINNAME, " - ", CHANSHUTDOWNERROR, ": ", chanError->message, NULL);
			g_warning("%s", error);
			return ((gpointer)error);
		} */

		/*g_io_channel_unref(channel);*/
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
	
	return (TRUE);
}
