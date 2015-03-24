/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   5.29.90	Harry Morris, morris@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef sockets_h
#define sockets_h

#include "cdialect.h"
#include "cutil.h"

#ifndef THINK_C
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif /* THINK_C */

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

extern char host_name[255], host_address[255];

void open_server _AP((long port,long* socket,long size));
void accept_client_connection _AP((long socket,FILE** file));
void close_client_connection _AP((FILE* file));
void close_server _AP((long socket));
FILE *connect_to_server _AP((char* hname,long port));
void close_connection_to_server _AP((FILE* file));
char *mygethostname _AP((char *hostname, long len));

#ifdef __cplusplus
	}
#endif /* def __cplusplus */

/*---------------------------------------------------------------------------*/

#endif




