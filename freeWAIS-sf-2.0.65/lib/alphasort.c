#include "../config.h"

#ifndef HAS_ALPHASORT
/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
**  ALPHASORT
**  Trivial sorting predicate for scandir; puts entries in alphabetical order.
*/

#include "../ir/cdialect.h"
#ifndef MAXNAMLEN
#define MAXNAMLEN 1024
#endif

#ifdef	RCSID
static char RCS[] = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/lib/RCS/alphasort.c,v 2.0.1.2 1995/09/15 09:51:45 pfeifer Exp $";
#endif	/* RCSID */

int
alphasort(d1, d2)
    Direntry_t **d1;
    Direntry_t **d2;
{
    return strcmp(d1[0]->d_name, d2[0]->d_name);
}

#else
static int make_ar_happy = 0;
#endif /* HAS_ALPHASORT */
