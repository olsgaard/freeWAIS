/*                               -*- Mode: C -*- 
 * 
 * ustubs.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Ulrich Pfeifer
 * Created On      : Mon Sep 11 09:47:54 1995
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Wed Jan 31 15:19:37 1996
 * Language        : C
 * Update Count    : 12
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 */

/* WIDE AREA INFORMATION SERVER SOFTWARE	
   No guarantees or restrictions.  See the readme file for the full standard 
   disclaimer.  
   4.14.90	Harry Morris, morris@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/ustubs.c,v 2.0.1.3 1996/01/31 14:51:52 pfeifer Exp $";
#endif

/* Change log:
 * $Log: ustubs.c,v $
 * Revision 2.0.1.3  1996/01/31 14:51:52  pfeifer
 * patch63: Use getcwd() is present.
 *
 * Revision 2.0.1.2  1995/09/15  09:49:25  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.1  1995/09/11  12:55:04  pfeifer
 * patch4: Simplified getwd(), getcwd() and memmove() stuff. Added
 * patch4: getuser_name() and getuser_env_name().
 *
 * Revision 2.0  1995/09/08  08:02:24  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.10  1994/12/13  18:52:45  pfeifer
 * chip@chinacat.unicom.com (Chip Rosenthal) patches.
 * Excluding the merge of libinv and libwais
 *
 * Revision 1.9  1994/09/07  13:29:22  pfeifer
 * ctype is now included from cdialect.h after inclusion of string.h.
 * Since ctype.h (the local version defines strcmp and friends, inclusion o
 * of string.h after that caused probems
 *
 * Revision 1.8  1994/09/07  12:43:38  pfeifer
 * added const str2 to memmove definitions
 *
 * Revision 1.7  1994/08/08  07:32:53  pfeifer
 * Moved wais_log_file_name and waislogfile to cutil.[ch]
 *
 * Revision 1.6  1994/08/05  07:12:52  pfeifer
 * Release beta 04
 *
 * Revision 1.4  1994/07/13  07:53:26  pfeifer
 * beta 02
 *
 * Revision 1.3  1994/04/29  23:23:23  pfeifer
 * ifdef'ed memmove/remove  for gcc 2.4.3
 * ,
 *
 * Revision 1.2  1994/03/08  21:07:09  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.4  92/05/06  17:35:13  jonathan
 * Modified some #if's for NeXT and Mach.
 * 
 * Revision 1.3  92/03/06  11:08:49  jonathan
 * fixed incompatible return type error in HP version of getwd.  Thanks to
 * cshotton@oac.hsc.uth.tmc.edu.
 * 
 * Revision 1.2  92/02/12  13:54:29  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
 * 
*/

/*----------------------------------------------------------------------*/
/* stubs for silly non-ansi c compilers                                 */
/*----------------------------------------------------------------------*/

#include "ustubs.h"
#include "cutil.h" /* for substrcmp and NULL */

#ifndef MAXPATHLEN              /* Should be defined in sys/param.h */
#define MAXPATHLEN 1024
#endif

#ifndef HAS_GETCWD
char           *
getwd (pathname)
     char           *pathname;
{
  return ((char *) getcwd (pathname, MAXPATHLEN));
}
#else

static  char        getcwdbuf[MAXPATHLEN];

char           *
getcwd (buf, size)
     char           *buf;
     size_t          size;
{
  char           *p;

#ifdef HAS_GETCWD
  if (!getcwd (getcwdbuf, MAXPATHLEN))
#else
  if (!getwd (getcwdbuf))
#endif
    return NULL;
  getcwdbuf[MAXPATHLEN - 1] = '\0';
  strncpy (buf, getcwdbuf, MAXPATHLEN);
  return getcwdbuf;
}
#endif

#ifdef I_PWD
#include <pwd.h> 
#endif

static char usernamebuffer[9];

char           *
getuser_name ()
{
#ifdef HAS_GETPWUID
  struct passwd  *p;
  Uid_t           uid;

  uid = getuid ();
  if (p = getpwuid (uid)) {
    strncpy (usernamebuffer, p->pw_name, 9);
    usernamebuffer[8] = '\0';
    return usernamebuffer;
  }
#endif
  return getuser_env_name ();
}

char           *
getuser_env_name ()
{
  if (getenv ("USER"))
    return getenv ("USER");
  if (getenv ("LOGNAME"))
    return getenv ("LOGNAME");
  return "nobody";
}


/*----------------------------------------------------------------------*/

#ifndef HAS_MEMMOVE

void           *
memmove (str1, str2, n)
     void           *str1;
     const void     *str2;
     size_t          n;
{
  bcopy ((char *) str2, (char *) str1, (long) n);
  return (str1);
}
#endif

/*----------------------------------------------------------------------*/

#ifndef ANSI_LIKE  

/* atoi is not defined k&r. copied from the book */
long
atol (s)
     char           *s;
{
  long            i, n, sign;

  /* skip white space */
  for (i = 0; s[i] == ' ' || s[i] == 'n' || s[i] == 't'; i++);
  sign = 1;
  if (s[i] == '+' || s[i] == '-')
    sign = (s[i++] == '+') ? 1 : -1;
  for (n = 0; s[i] >= '0' && s[i] <= '9'; i++)
    n = 10 * n + s[i] - '0';
  return (sign * n);
}

#if 0
char *strstr(src,sub)
char *src;
char *sub;
{
  /* this is a poor implementation until the ANSI version catches on */
  char *ptr;
  for(ptr = src; (long)ptr <= (long)src + strlen(src) - strlen(sub); ptr++){
    if(substrcmp(ptr, sub))
      return(ptr);
  }
  return(NULL);
}
#endif

#ifndef HAS_REMOVE
indent:Standard input:5: Error: Unmatched #endif
int remove 
_AP ((char *filename))
{
  return (unlink (filename));
}
#endif

#endif /* ndef ANSI_LIKE */


