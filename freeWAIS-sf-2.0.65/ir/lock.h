/* ----------------------------------------------------------------------------

	COPYRIGHT	1989 Thinking Machines Corporation
	AUTHOR		Gordon Linoff
	MODULE		lock.h -- #defines to use lock files
	COMMENTS
	    $Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/lock.h,v 2.0.1.1 1995/09/15 09:46:38 pfeifer Exp $
---------------------------------------------------------------------------- */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef _LOCK_H_INCLUDED
#define _LOCK_H_INCLUDED

#include "cdialect.h"

#define INVALID_LOCK          -1
#define LOCK_QUERY             0  /* processing a query */
#define LOCK_UPDATE            1  /* updating database files that
                                   * need exclu access , blocking
                                   * queries */
#define LOCK_DELETE_VERSION    2
#define LOCK_CHECK_POINT       3
#define LOCK_SERVER            4  /* entire server period */
#define LOCK_INDEX             5  /* entire indexing period */
#define NUM_LOCK_TYPES         6

extern long utlk_using_lock _AP((char *dbanme, long lock_type));
extern long utlk_unset_lock _AP((char *dbname, long lock_type));
extern long utlk_set_lock _AP((char *dbname, long lock_type));



#ifdef LOCK_STORAGE_MODULE

#define IN_LOCK_RANGE(l) ((l >= 0) && (l < NUM_LOCK_TYPES))

static char *lock_names[NUM_LOCK_TYPES] = {
    ".query.lock",
    ".update.lock",
    ".delete-version.lock",
    ".check-point.lock",
    ".server.lock",
    ".index.lock"
    };

 
#endif  

#endif /* #ifndef _LOCK_H_INCLUDED */
