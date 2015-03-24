/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* Header file for panic.c */

#ifndef PANIC_H
#define PANIC_H

#include "cdialect.h"

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

#ifdef ANSI_LIKE	/* use ansi */
void	panic _AP((char* format,...)); 
#else /* use K & R */
void	panic _AP(()); 
#endif

#ifdef __cplusplus
	}
#endif /* def __cplusplus */
#endif /* ndef PANIC_H */
