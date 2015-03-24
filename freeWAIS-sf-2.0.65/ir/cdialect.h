/*                               -*- Mode: C -*- 
 * 
 * cdialect.h -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Ulrich Pfeifer
 * Created On      : Tue Sep  5 15:27:25 1995
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Fri Jan 12 14:46:09 1996
 * Language        : C
 * Update Count    : 33
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 * $$
 * $Log: cdialect.h,v $
 * Revision 2.0.1.7  1996/01/14 17:25:42  pfeifer
 * patch61: Changed #if I_ALLOCA to #ifdef I_ALLOCA.
 *
 * Revision 2.0.1.6  1995/11/03  09:50:23  pfeifer
 * patch43: Moved the define for _AP up before the include of ctype.h.
 *
 * Revision 2.0.1.5  1995/10/06  17:21:17  pfeifer
 * patch24: Typo.
 *
 * Revision 2.0.1.5  1995/10/06  17:21:17  pfeifer
 * patch24: Typo.
 *
 * Revision 2.0.1.4  1995/09/15  09:39:37  pfeifer
 * patch7: Prototypes for getuser_name() and getuser_env_name().
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.3  1995/09/12  12:20:13  pfeifer
 * patch6: Added LITTLEENDIAN BIGENDIAN
 *
 * Revision 2.0.1.2  1995/09/11  12:43:23  pfeifer
 * patch4: Added alloca magics from autoconf. Added prototypes for
 * patch4: getuser_name() and getuser_env_name().
 *
 * Revision 2.0.1.1  1995/09/08  19:08:49  pfeifer
 * patch1: Added conditional #undef bcopy to overwrite confmagic.h
 * patch1: definitions when bcopy cannot do overlapping copies.
 *
 * Revision 2.0  1995/09/08  07:58:21  pfeifer
 * Metaconfig baseline
 *
 */

/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarentees or restrictions.  See the readme file for the full standard
   disclaimer.

   5.29.90	Harry Morris, morris@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#ifndef C_DIALECT
#define C_DIALECT

/*----------------------------------------------------------------------*/

/* AIX requires this to be the first thing in the file.  */
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not __GNUC__ */
#ifdef I_ALLOCA
#include <alloca.h>
#else /* not I_ALLOCA */
#ifdef _AIX
 #pragma alloca
#else /* not _AIX */
char *alloca ();
#endif /* not _AIX */
#endif /* not I_ALLOCA */
#endif /* not __GNUC__ */

#include "../config.h"
#include "../confmagic.h"

#ifdef I_STDLIB
#include <stdlib.h>
#endif /* I_STDLIB */

#ifdef I_STRING
#include <string.h>
#else /* not I_STRING */
#include <strings.h>
#endif /* not I_STRING */

#ifdef I_TIME
#include <time.h>
#endif /* I_TIME */
#ifdef I_SYS_TIME
#include <sys/time.h>
#endif /* I_SYS_TIME */

#ifdef I_SYS_TYPES
#include <sys/types.h> 
#endif /* I_SYS_TYPES */

#ifdef I_UNISTD
#include <unistd.h>
#endif /* I_UNISTD */

#ifdef I_DIRENT
#include <dirent.h>
#ifdef DIRNAMLEN
#define NLENGTH(dirent) ((dirent)->d_namlen)
#else /* not DIRNAMLEN */
#define NLENGTH(dirent) (strlen((dirent)->d_name))
#endif /* not DIRNAMLEN */
#ifdef I_SYS_NDIR
#include <sys/ndir.h>
#endif /* I_SYS_NDIR */


#ifdef I_NDIR
#include <ndir.h>
#endif /* I_NDIR */
#else
#ifdef I_SYS_DIR
#include <sys/dir.h>
#endif /* I_SYS_DIR */
#endif /* I_DIRENT */



#ifdef I_STRING
#include <string.h>
#else /* not I_STRING */
#include <strings.h>
#endif /* not I_STRING */

#ifdef I_MEMORY
#include <memory.h>
#endif /* I_MEMORY */

/* set up the function prototyping convention */
#ifdef CAN_PROTOTYPE
#define _AP(args) args
#define ANSI_LIKE
#else /* not CAN_PROTOTYPE */
#define K_AND_R
#define _AP(args) ()
#endif /* not CAN_PROTOTYPE */

#include <ctype.h>              /* local version redefines string functions */

#if 0 /* HAS_GETWD */
extern char* getcwd _AP((char *buf, size_t size));
#endif

extern char* getuser_name _((void));
extern char* getuser_env_name _((void));

/*
 * How about the memory copy routines ?
 */
#ifndef HAS_SAFE_BCOPY
#if HAS_SAFE_MEMCPY
/* ok: have memcpy() and it does overlapping copies */
#define bcopy(s, d, n) memcpy ((d), (s), (n))
#else /* not HAS_SAFE_MEMCPY */
/* Neither bcopy() nor memcpy() do overlapping copies */
#ifdef bcopy
#undef bcopy
#endif
#define bcopy(d, s, n) mybcopy ((d), (s), (n))
#define NEED_MYBCOPY
#endif /* not HAS_SAFE_MEMCPY */
#endif /* not HAS_SAFE_BCOPY */


#ifndef HAS_STRDUP
extern char * strdup _((char *s));
#endif /* not HAS_STRDUP */
#endif /* not C_DIALECT */

#if ((BYTEORDER==0x1234) || (BYTEORDER==0x12345678))
#define LITTLEENDIAN
#else
#define BIGENDIAN
#endif

#if (INTSIZE==4)
#define FOUR_BYTE int
#else
#if (LONGSIZE==4)
#define FOUR_BYTE long
#else
WhatTypeHasFourBytes
#endif
#endif

/* Comment this back in to figure out what the compiler thinks we are */
/*
#ifdef ANSI_LIKE
WeareAnsiLike
#endif
#ifdef PROTO_ANSI
WeareProtoAnsi
#endif
#ifdef K_AND_R
WeareKandR
#endif
Crash-and-Burn
*/
/* End of chunk to comment back in */

/*----------------------------------------------------------------------*/


