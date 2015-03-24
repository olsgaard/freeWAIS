/* this is the header for transprt.c
 * -brewster
 * revise log:
 *  11/14/90   -Tracy Shen
 *   add the followinf functions:
 *   transportCodeH, transportDecodeH, transportCodeI, transportDecodeI
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef TRANSPRT_H
#define TRANSPRT_H

#include "cdialect.h"

#include "cutil.h"

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

boolean transportCode _AP((long encoding,char* data,long* len));
boolean transportDecode _AP((long encoding,char* data,long* len));

void transportCodeH _AP((char* data,long* len));
void transportDecodeH _AP((char* data,long* len));

void transportCodeI _AP((char* data,long* len));
void transportDecodeI _AP((char* data,long* len));

#ifdef __cplusplus
	}
#endif /* def __cplusplus */
#endif /* ndef TRANSPRT_H */
