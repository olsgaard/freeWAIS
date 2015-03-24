/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef _H_STATUS
#define _H_STATUS

#define STATUS_INFO 1
#define STATUS_CONT 2
#define STATUS_URGENT 3

#define STATUS_LOW 0
#define STATUS_MEDIUM 1
#define STATUS_HIGH 2

void PrintStatus _AP((long logtype, long loglevel, char* format, ...));

#endif /* _H_STATUS */

/*
 *
 * $Log: status.h,v $
 * Revision 2.0.1.1  1995/09/15 09:54:00  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:05:23  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:18:43  pfeifer
 * Release beta 04
 *
 * Revision 1.1  93/06/23  20:02:39  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.1  92/06/14  19:00:05  jonathan
 * Initial revision
 * 
 */
