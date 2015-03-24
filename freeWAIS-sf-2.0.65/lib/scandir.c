#include "../config.h"
#ifndef HAS_SCANDIR
/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
**  SCANDIR
**  Scan a directory, collecting all (selected) items into a an array.
*/

#include "../ir/cdialect.h"
/* #include <sys/types.h> */
#include <stdio.h>

#ifdef	RCSID
static char RCS[] = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/lib/RCS/scandir.c,v 2.0.1.3 1995/09/15 09:52:17 pfeifer Exp $";
#endif	/* RCSID */

/* Initial guess at directory size. */
#define INITIAL_SIZE	20

/*
extern char		*malloc();
extern char		*realloc();
extern char		*strcpy();
*/
int
scandir(name, list, selector, sorter)
    char		  *name;
    Direntry_t		***list;
    int			 (*selector)();
    int			 (*sorter)();
{
    register Direntry_t	  **names;
    register Direntry_t	  *entp;
    register DIR	  *dirp;
    register int	   i;
    register int	   size;

    /* Get initial list space and open directory. */
    size = INITIAL_SIZE;
    names = (Direntry_t **)malloc(size * sizeof names[0]);
    if (names == NULL)
	return -1;
    dirp = opendir(name);
    if (dirp == NULL)
	return -1;

    /* Read entries in the directory. */
    for (i = 0; entp = readdir(dirp); )
	if (selector == NULL || (*selector)(entp)) {
	    /* User wants them all, or he wants this one. */
	    if (++i >= size) {
		size <<= 1;
		names = (Direntry_t **)
		    realloc((char *)names, size * sizeof names[0]);
		if (names == NULL) {
		    closedir(dirp);
		    return -1;
		}
	    }

	    /* Copy the entry. */
	    names[i - 1] = (Direntry_t *)malloc(sizeof(Direntry_t) 
					        + NLENGTH(entp)+1);
	    if (names[i - 1] == NULL) {
		closedir(dirp);
		return -1;
	    }
	    names[i - 1]->d_ino = entp->d_ino;
	    names[i - 1]->d_reclen = entp->d_reclen;
#ifdef DIRNAMLEN
	    names[i - 1]->d_namlen = entp->d_namlen;
#endif
	    (void)strcpy(names[i - 1]->d_name, entp->d_name);
	}

    /* Close things off. */
    names[i] = NULL;
    *list = names;
    closedir(dirp);

    /* Sort? */
    if (i && sorter)
	qsort((char *)names, i, sizeof names[0], sorter);

    return i;
}
#else
static int make_ar_happy = 0;
#endif /* HAS_SCANDIR */
