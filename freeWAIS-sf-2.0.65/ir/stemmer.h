/* WIDE AREA INFORMATION SERVER SOFTWARE
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.  
  
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef STEMMER_H
#define STEMMER_H

#include "cdialect.h"
#include "cutil.h"


#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */


/* main stemmer routine */
char * stemmer _AP((char *word));


/* smart stemmer */
void smrt_stmr _AP((char *word));


/* plural stemmer */
void news_stmr _AP((char *word));


/* porter's stemmer */
void prtr_stmr _AP((char *word));


/* porter's alternative stemmer */
void alt_prtr_stmr _AP((char *word));


#ifdef __cplusplus
	}
#endif /* def __cplusplus */

#endif /* STEMMER_H */
