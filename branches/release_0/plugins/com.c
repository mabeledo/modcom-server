/********************************************************************
 *  Proyecto: Sistema modular de comunicacion con un robot movil
 *  Subproyecto: Servidor
 *  Archivo: plugins/com.c
 * 	Version: 0.1
 *
 *  Autor: Manuel Angel Abeledo Garcia
 ********************************************************************/

#include "com.h"

#define PLUGINPROTO		"COM"
#define PLUGINNAME		"COM Port"
#define PLUGINDESC		"Serial port plugin"
#define PLUGINVERSION	"0.1"

/* COM port attributes */
#define BAUDRATE		B38400
#define BUFFERLEN		2048			

/* Error messages. */
#define NODEVICEAVAILABLE	"There is no device available"
#define OPENDEVICEFAILED	"Error opening device"
#define INPUTERROR			"Input error: "
#define OUTPUTERROR			"Output error: "
#define SHUTDOWNCHANERROR	"Shutdown error: "

/* Global variables. */
static GIOChannel* devChan;

#ifdef G_OS_UNIX
	/* Previous serial port configuration. */
	static struct termios oldCfg;
	
	/* New serial port configuration. */
	static struct termios newCfg;
	
	/* Device file descriptor. */
	static gint fd;
	
	/* Device name */
	static gchar* device;
#endif

gushort
pluginProto							()
{
	return ((gushort)PLUGINPROTO);
}

const gchar*
pluginAddress						()
{
	return (device);
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
	GData* comConfig = (GData*)data;
	
	/* Load configuration parameters */
	if (g_datalist_get_data(&comConfig, "device") != NULL)
	{
		device = (gchar*)g_datalist_get_data(&comConfig, "device");
	}
	else
	{
		*error = g_strdup(NODEVICEAVAILABLE);
		return (FALSE);
	}
	
	/*
	 * Serial port configuration.
	 * */
	
	/* UNIX plugin implementation is a little bit special because it
	 * handles the serial port as a file and needs a bunch of
	 * parameters. Serial port is opened first and then configured, and
	 * it should stay opened until the process ends.
	 * */
	
	#ifdef G_OS_UNIX
		/* Open serial port only for reading and not as controlling
		 * tty, in order to avoid get killed with CTRL+C.
		 * */
		if ((fd = open(device, O_RDWR | O_NOCTTY)) < 0)
		{
			*error = g_strdup(OPENDEVICEFAILED);
			return (FALSE);
		}	
	
		/* Save current serial port settings and clear the structure
		 * to receive new.
		 * */
		tcgetattr(fd, &oldCfg);
		bzero(&newCfg, sizeof(newCfg));
		
		/* Set BAUDRATE (bps rate), CS8 (8 bit, no parity, 1 stopbit),
		 * CLOCAL (local reception, no modem control), CREAD
		 * (enable receiving characters).
		 * */
		newCfg.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
		
		/* Set IGNPAR (ignore bits with parity errors) and ICRNL (map CR,
		 * Windows end of line, to NL, UNIX end of line).
		 * */
		newCfg.c_iflag = IGNPAR | ICRNL;

		newCfg.c_oflag = 0;
		newCfg.c_lflag = ICANON;
		
		/* Initialize all control characters. */
		newCfg.c_cc[VINTR] = 0;
		newCfg.c_cc[VQUIT] = 0;
		newCfg.c_cc[VERASE] = 0;
		newCfg.c_cc[VKILL] = 0;
		newCfg.c_cc[VEOF] = 4;
		newCfg.c_cc[VTIME] = 0;
		newCfg.c_cc[VMIN] = 1;
		newCfg.c_cc[VSWTC] = 0;
		newCfg.c_cc[VSTART] = 0;
		newCfg.c_cc[VSTOP] = 0;
		newCfg.c_cc[VSUSP] = 0;
		newCfg.c_cc[VEOL] = 0;
		newCfg.c_cc[VREPRINT] = 0;
		newCfg.c_cc[VDISCARD] = 0;
		newCfg.c_cc[VWERASE] = 0;
		newCfg.c_cc[VLNEXT] = 0;
		newCfg.c_cc[VEOL2] = 0;
		
		/* Clean the modem line and activate the settings. */
		tcflush(fd, TCIFLUSH);
		tcsetattr(fd, TCSANOW, &newCfg);
		
		/* Only one port is managed. */
		devChan = g_io_channel_unix_new(fd);
	#endif

	g_debug("COM plugin loaded and initialized");
	return (TRUE);
}

gboolean
pluginSend							(gpointer dest, gpointer data, gchar** error)
{
	gchar* msgStr = g_memdup(data, sizeof(data));
	GError* returnError;

	if (g_io_channel_write_chars(devChan, msgStr, BUFFERLEN, NULL, &returnError) != G_IO_STATUS_NORMAL)
	{
		*error = g_strconcat(OUTPUTERROR, returnError->message, NULL);
		return (FALSE);
	}

	return (TRUE);
}

gpointer
pluginReceive						(gpointer data)
{
	GAsyncQueue* qMessages = g_async_queue_ref((GAsyncQueue*)data);
	
	gchar* buffer;
	Message* msg;
	gboolean receiving;
	GError* returnError;
	gchar* error;
	
	receiving = TRUE;
	
	while (receiving)
	{
		if (g_io_channel_read_line(devChan, &buffer, NULL, NULL, &returnError) != G_IO_STATUS_NORMAL)
		{
			error = g_strconcat(INPUTERROR, returnError->message, NULL);
			return((gpointer)error);
		}

		msg = (Message*)buffer;
		
		if ((msg->part == 0) && (g_ascii_strcasecmp(msg->data, END_OF_DATA) == 0))
		{
			receiving = FALSE;
		}
		else
		{
			g_async_queue_push(qMessages, &msg);	
		}
	}
	
	g_async_queue_unref(qMessages);
	return (NULL);
}

gboolean
pluginExit							(gchar** error)
{
	GError* returnError;
	
	if (g_io_channel_shutdown(devChan, FALSE, &returnError) != G_IO_STATUS_NORMAL)
	{
		*error = g_strconcat(SHUTDOWNCHANERROR, returnError->message, NULL);
		return (FALSE);
	}
	
	#ifdef G_OS_UNIX
		tcsetattr(fd, TCSANOW, &oldCfg);
		close(fd);
	#endif
	
	return (TRUE);
}

/*
 * VERSION 0.1
 * */
