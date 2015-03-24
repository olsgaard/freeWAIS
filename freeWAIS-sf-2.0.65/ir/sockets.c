/*                               -*- Mode: C -*- 
 * sockets.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Harry Morris, morris@think.com
 * Created On      : 5.29.90
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Fri Oct 20 17:32:41 1995
 * Language        : C
 * Update Count    : 33
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 * $$
 */

/* WIDE AREA INFORMATION SERVER SOFTWARE        
   No guarantees or restrictions.  See the readme file for the full standard 
   disclaimer.  
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#ifndef lint
static char    *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/sockets.c,v 2.0.1.5 1995/10/20 17:10:08 pfeifer Exp $";
#endif

/* Change log:
 * $Log: sockets.c,v $
 * Revision 2.0.1.5  1995/10/20 17:10:08  pfeifer
 * patch40: Fixed the sys_errlist stuff (for the last time?).
 *
 * Revision 2.0.1.4  1995/10/10  19:03:43  pfeifer
 * patch35: Not all compilers know elseif :-(
 *
 * Revision 2.0.1.4  1995/10/10  19:03:43  pfeifer
 * patch35: Not all compilers know elseif :-(
 *
 * Revision 2.0.1.3  1995/10/06  07:25:09  pfeifer
 * patch22: Added sys_errlist hack for BSD.
 *
 * Revision 2.0.1.2  1995/10/04  17:23:17  pfeifer
 * patch20: Tried a major cleanup. Removed all #ifdef __XXX__.
 *
 * Revision 2.0.1.1  1995/09/15  09:47:47  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:01:28  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.7  1994/12/13  18:52:45  pfeifer
 * chip@chinacat.unicom.com (Chip Rosenthal) patches.
 * Excluding the merge of libinv and libwais
 *
 * Revision 1.6  1994/11/29  10:44:03  pfeifer
 * "matthew (m.) holiday" <holiday@bnr.ca>:
 * I've been having some problems with server security, where the
 * server was refusing connections from an authorized host.  Lo and behold,
 * when the server accepted a connection from a client, the host_name and
 * host_address variables were left as empty strings (and so never matched the
 * entries in the DATA_SEC file)!  These need to be reset for each new client
 * connection, which is what the following patch does.
 *
 * Revision 1.5  1994/07/13  07:53:26  pfeifer
 * beta 02
 *
 * Revision 1.4  1994/05/20  12:57:23  pfeifer
 * beta
 *
 * Revision 1.3  1994/03/08  21:06:51  pfeifer
 * Patchlevel 04
 *
 * Revision 1.2  93/07/01  19:14:22  warnock
 * Fix for gethostname from francois and jonathan
 * 
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.23  92/05/06  17:34:41  jonathan
 * Added Mach to some compiler switches.
 * 
 * Revision 1.22  92/04/28  15:21:14  jonathan
 * Added decoding of IP address to DNS name in accept_client handler.
 * 
 * Revision 1.21  92/04/01  09:49:49  morris
 * declared clr_socket static to stop gcc from complaining
 * 
 * Revision 1.20  92/03/24  10:35:32  jonathan
 * Put a loop around connect in fd_connect_to_server to check if the connect
 * was interrupted by a system call (usually a timer).  Retries if errno is
 * EINTR.
 * 
 * Revision 1.19  92/02/16  12:38:22  jonathan
 * Changed bzero's to memset's.
 * 
 * Revision 1.18  92/02/16  12:34:11  jonathan
 * Removed code refering to NOINETNTOA, since we should use inet_ntoa.
 * 
 * Revision 1.17  92/02/12  13:48:21  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
 * 
 */

/*
   Added code in fd_accept_client_connection to print source Inet address to
   stderr. 
   - Jonny G Fri Apr 12 1991

 */

#define sockets_c

#include "sockets.h"

#ifdef NOTCPIP			/* we don't have TCPIP */

void 
open_server (port, socket, size)
     long            port;
     long           *socket;
     long            size;
{
}

void 
accept_client_connection (socket, file)
     long            socket;
     FILE          **file;
{
}

void 
close_client_connection (file)
     FILE           *file;
{
}

void 
close_server (socket)
     long            socket;
{
}

FILE           *
connect_to_server (host_name, port)
     char           *host_name;
     long            port;
{
  return (NULL);
}
void 
close_connection_to_server (file)
     FILE           *file;
{
}

#else /* there is TCPIP */

#include <errno.h>
#include "cdialect.h"
#include "panic.h"

#ifdef HAS_SYS_ERRLIST
#ifdef EXTERN_SYS_ERRLIST
extern char *sys_errlist[];
#endif
extern int      errno;
extern int      sys_nerr;
#endif

char            host_name[255], host_address[255];

/* XXX
   still need:
   non-blocking modes
   special send/recieve functions? (there are now some in ui.c)
   asynchronous calls?
 */

/* define the number of queued connections allowable on each port */
#define QUEUE_SIZE 3

/*---------------------------------------------------------------------------*/
/* Server functions                                                          */
/*---------------------------------------------------------------------------*/

static boolean clr_socket _AP ((struct sockaddr_in * address, long portnumber,
				long *sock));

static          boolean
clr_socket (address, portnumber, sock)
     struct sockaddr_in *address;
     long            portnumber;
     long           *sock;
{
  unsigned long  *S_addr = (unsigned long *) &(address->sin_addr);

  if (errno == EADDRINUSE) {
    /* Try connecting to it */
    if (connect (*sock, address, sizeof (struct sockaddr_in)) == 0) {
      close (*sock);
      waislog (WLOG_HIGH, WLOG_ERROR,
	       "Cannot bind port %ld: (Address already in use).",
	       portnumber);
      waislog (WLOG_HIGH, WLOG_ERROR,
	       "waisserver is already running on this system");
      panic ("Exiting");
    } else {
      /* Connection failed; probably socket in FIN_WAIT */
      int             one = 1;

      (void) close (*sock);
      if ((*sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	panic ("Open socket failed in trying to clear the port.");
      /*printf("Error binding port %d: (address already in use).\n\
         Attempting to clear stale socket...", portnumber); */
      if (setsockopt (*sock, SOL_SOCKET, SO_REUSEADDR,
		      &one, sizeof (one)) < 0) {
	/*printf("Warning: Setsockopt SO_REUSEADDR failed."); */
      }
      address->sin_family = AF_INET;
      *S_addr = INADDR_ANY;

      address->sin_port = (unsigned short int) htons (portnumber);

      if (bind (*sock, address, sizeof (*address)) == 0) {
	/*printf("Successfully cleared stale EADDRINUSE error"); */
      }
    }
  }
  return (true);
}

void
open_server (port, fd, size)
     long            port;
     long           *fd;
     long            size;
{
  struct sockaddr_in address;
  unsigned long  *S_addr = (unsigned long *) &address.sin_addr;
  memset (&address, 0, sizeof (address));

  /* open the fd */
  if ((*fd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
    panic ("can't get file descriptor for socket: %s", Strerror (errno));
  }
  address.sin_family = AF_INET;
  *S_addr = htonl (INADDR_ANY);
  address.sin_port = (unsigned short int) htons (port);
  if (bind (*fd, (struct sockaddr *) &address, sizeof (struct sockaddr)) < 0)
                    clr_socket (&address, port, fd);

  if (listen (*fd, QUEUE_SIZE) < 0)
    panic ("can't open server: %s", Strerror (errno));
}

/* This is a lower level function provided for use by the lisp version of
 * this library 
 * XXX should support non-blocking mode
 */

#ifdef I_ARPA_INET
#include <arpa/inet.h>
#endif

void
fd_accept_client_connection (socket, fd)
     long            socket;
     long           *fd;
{ /* accept an input connection, and open a file on it */
  struct sockaddr_in saddr;
  int                len;

  len = sizeof saddr;

  do {
    errno = 0;
    *fd = accept (socket, (struct sockaddr *)&saddr, &len);
  } while (*fd < 0 && errno == EINTR);

  if (saddr.sin_family == AF_INET) {
#ifdef HAS_GETHOSTENT
    struct hostent *gethostent();
    struct hostent *peer = NULL;

    host_name[0] = '\0';

    peer = gethostbyaddr ((char *) &saddr.sin_addr, 4, AF_INET);
    sprintf (host_address, "%s", inet_ntoa (saddr.sin_addr));
    if (peer != NULL) {
      sprintf (host_name, "%s", peer->h_name);

      waislog (WLOG_MEDIUM, WLOG_CONNECT,
	       "Accepted connection from: %s [%s]",
	       host_name[0] ? host_name : "?", host_address);
    } else {
      waislog (WLOG_MEDIUM, WLOG_CONNECT,
	       "Accepted connection from: [%s]", host_address);
    }
#else /* not HAS_GETHOSTENT */
    waislog (WLOG_MEDIUM, WLOG_CONNECT,
	       "Accepted connection from: [%s]", host_address);
#endif /* not HAS_GETHOSTENT */
  }
  if (*fd < 0)
    panic ("can't accept connection");
}

/* This is the prefered C function for accepting client requests */
void
accept_client_connection (socket, file)
     long            socket;
     FILE          **file;
{
  long            fd;			    /* file descriptor actually used */

  fd_accept_client_connection (socket, &fd);
  if ((*file = fdopen (fd, "r+")) == NULL)
    panic ("can't accept connection");
}

/* When a server wants to end the session with a client */
void
close_client_connection (file)
     FILE           *file;
{
  fclose (file);
}

/* when exiting the top level server process (not the forked
   server processes that come one per client).
   Maybe we need to do this once per client as well.
 */
void
close_server (socket)
     long            socket;
{
  close (socket);
}

/*---------------------------------------------------------------------------*/
/* Client functions                                                          */
/*---------------------------------------------------------------------------*/

/* This is a lower level function provided for use by the lisp version of
 * this library 
 * XXX should support non-blocking mode
 */

#define HOSTNAME_BUFFER_SIZE 120
#define MAX_RETRYS 10

boolean
fd_connect_to_server (hname, port, fd)
     char           *hname;
     long            port;
     long           *fd;
{
  char            hostnamebuf[80];
  long            rc, i;
  struct hostent *host;
  struct sockaddr_in name;
  /* Some systems define sin_addr as union, some dont. So lets assume we
     know what we are doing an copy a long. */
  unsigned long  *S_addr = (unsigned long *) &name.sin_addr;

  memset ((char *) &name, 0, sizeof (name));
  *S_addr = inet_addr (hname);

  if (*S_addr != -1) {
    name.sin_family = AF_INET;
    (void) strcpy (hostnamebuf, hname);
  } else {
    host = gethostbyname (hname);

    if (NULL == host) {
      return FALSE;
    }
    name.sin_family = host->h_addrtype;
#ifdef h_addr
    bcopy (host->h_addr_list[0],
	   (caddr_t) & name.sin_addr, host->h_length);
#endif
    (void) strcpy (hostnamebuf, host->h_name);
  }
  hname = hostnamebuf;

  name.sin_port = (unsigned short int) htons (port);

  *fd = socket (AF_INET, SOCK_STREAM, 0);
  for (i = 0; i < MAX_RETRYS; i++) {
    rc = connect (*fd, &name, sizeof (name));
    if (rc == 0)
      return TRUE;
    else if (errno == EINTR) {
      sleep (1);
    } else {
      perror ("Connect to socket did not work (1)");
      return FALSE;
    }
  }
  return FALSE;
}

/* This is the prefered C function for initiating client requests */
FILE           *
connect_to_server (hname, port)
     char           *hname;
     long            port;
{
  FILE           *file;
  long            fd;

  if (fd_connect_to_server (hname, port, &fd) == FALSE) {
    perror ("Connect to socket did not work (2)");
    return NULL;
  }
  if ((file = fdopen (fd, "r+")) == NULL) {
    perror ("Connect to socket did not work (3)");
    return NULL;
  }
  return file;
}

void
close_connection_to_server (file)
     FILE           *file;
{
  fclose (file);
}

/*---------------------------------------------------------------------------*/

#endif /* there is TCPIP */

/* Francois and Jonathan Goldman - hostname fix */
char           *
mygethostname (hostname, len)
     char           *hostname;
     long            len;
{
  char            name[255];
  struct hostent *h;

  gethostname (name, 254);
  strcpy (hostname, name);

  h = gethostbyname (name);

  if ((h = gethostbyname (name)) != NULL) {
    if ((strlen (h->h_name) >= len) && (len > 0)) {
      strncpy (hostname, h->h_name, len - 1);
    } else {
      strcpy (hostname, h->h_name);
    }
  } else {
    if ((strlen (name) >= len) && (len > 0)) {
      strncpy (hostname, name, len - 1);
    } else {
      strcpy (hostname, name);
    }
  }

  return (hostname);
}
