/*--------------------------------------------------------------------------
 * ABSTRACT:    wutil.h -- WAIS TOOLS header file
 *
 * AUTHOR
 *   M. Tracy Shen
 *
 *
 *------------------------------------------------------------------------*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#ifndef _H_WAIS_util_
#define _H_WAIS_util_

#include "cdialect.h"

#include "cutil.h"

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

void twais_format_req_apdu _AP(( boolean use_template, char* apdu_buff, long* len));
void twais_dsply_rsp_apdu _AP(( char* rsp_buff, long rsp_len));
void twais_free_apdu _AP((char* apdu_buff));

long twais_format_init_apdu _AP(( boolean use_template, char* apdu_buff));
long twais_format_typ3_srch_apdu _AP(( boolean use_template, char* apdu_buff));
long twais_format_typ1_srch_apdu _AP(( boolean use_template, char* apdu_buff));

void twais_dsply_init_rsp_apdu _AP(( char* buffer));
void twais_dsply_init_apdu _AP(( char* buffer));
void twais_dsply_srch_rsp_apdu _AP(( char* buffer));
void twais_dsply_srch_apdu _AP(( char* buffer));

void twais_tmplt_init_apdu _AP((char* buff, long* buff_len));
void twais_tmplt_init_rsp_apdu _AP((char* buff, long* buff_len));
void twais_tmplt_typ1_srch_apdu _AP(( char* buff, long* buff_len));
void twais_tmplt_typ3_srch_rsp_apdu _AP(( char* buff, long* buff_len));

#endif

#ifdef __cplusplus
	}
#endif /* def __cplusplus */
