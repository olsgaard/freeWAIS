/* Send an UDP packet containing the program version and UID
   to a compiled-in address */
/* 
     Copyright (c) 1994 Harald T. Alvestrand

     The X Consortium, and any party obtaining a copy of these files from
     the X Consortium, directly or indirectly, is granted, free of charge, a
     full and unrestricted irrevocable, world-wide, paid up, royalty-free,
     nonexclusive right and license to deal in this software and
     documentation files (the "Software"), including without limitation the
     rights to use, copy, modify, merge, publish, distribute, sublicense,
     and/or sell copies of the Software, and to permit persons who receive
     copies from any such party to do so.  This license includes without
     limitation a license to do the foregoing actions under any patents of
     the party supplying this software to the X Consortium.

*/
#include "cdialect.h"
#ifndef DO_NOT_TELL_ABOUT_ME
/*
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
*/

#include "sockets.h"
#include <sys/utsname.h>

#ifndef UDP_PORT_NO
#define UDP_PORT_NO 29659
#endif
#ifndef UDP_HOST_NAME
#define UDP_HOST_NAME "ls6-www.informatik.uni-dortmund.de"
#endif
#ifndef UDP_HOST_NO
#define UDP_HOST_NO "129.217.20.172"
#endif

extern uid_t getuid();
extern int socket();
extern int sendto();
extern int close();

static int iamhere(progname)
     char *progname;
{
    char buffer[100];
    int s, err;
    struct sockaddr_in to;
    struct protoent *udp;
    struct utsname name;
    struct hostent *host;
    extern long tcp_port;

    /* This routine is made to leave on ANY error AT ALL */
    host = gethostbyname(UDP_HOST_NAME);
    /* Solaris returns > 0, others return = 0 */
    if (uname(&name) < 0) {
       sprintf(buffer, "[%d ???] %.60s", getuid(), progname);
    } else {
       /* Get machine, opsys and release from uname stuff */
       sprintf(buffer, "[uid=%d port=%d hw=%.10s os=%.10s osver=%.10s dns=%s cc=%s] %.40s",
            getuid(), tcp_port, 
            name.machine, name.sysname, name.release,
            host?"yes":"no",
            COMPILER_VERSION,
            progname);
    }
    udp = getprotobyname("udp");
    if (! udp) return -1;
    s = socket(PF_INET, SOCK_DGRAM, udp->p_proto);
    if (s == -1) return -1;
    to.sin_family = AF_INET;
    to.sin_port = htons(UDP_PORT_NO);
    if (host) {
       to.sin_addr.s_addr = *(u_long*)*(host->h_addr_list);
    } else {
#ifdef __DGUX__
       to.sin_addr = inet_addr(UDP_HOST_NO);
#else
       to.sin_addr.s_addr = inet_addr(UDP_HOST_NO);
#endif
    }
    if (to.sin_addr.s_addr == -1) return -1;
    err = sendto(s, buffer, strlen(buffer), 0, &to, sizeof(to));
    (void) close(s);
    if (err == -1) return -1;
    return 0;
}

int sayiamhere()
{
    int stat;
    /* Isolate the call in a new process */
    if (fork() != 0) {
       return 0;
    }
    stat = iamhere(VERSION_STRING);
    exit(stat!=0?1:0);
}

#endif
/* DO_NOT_TELL_ABOUT_ME */
