/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: util.h,v $
 * Revision 2.0.1.1  1995/09/15 09:54:16  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:05:32  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:19:27  pfeifer
 * Release beta 04
 *
 * Revision 1.1  93/06/23  20:02:40  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.2  92/03/17  14:32:44  jonathan
 * Prototyped and cleaned up.
 * 
 */

#ifndef _H_UTIL
#define _H_UTIL

#include <cdialect.h>

int charlistlength _AP((char** list));
void listlength _AP((List list));
void find_value _AP((char* source, char* key, char* value, int value_size));
#endif
