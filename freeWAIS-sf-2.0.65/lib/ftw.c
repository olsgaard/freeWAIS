
/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
**  FTW
**  Walk a directory hierarchy from a given point, calling a user-supplied
**  function at each thing we find.  If we go below a specified depth,
**  recycle file descriptors.
*/

#include <stdio.h>
/* #include <sys/types.h> */
#include "../ir/cdialect.h"
#include <sys/stat.h>

#define FTW_F		0	/* A normal file			*/
#define FTW_D		1	/* A directory				*/
#define FTW_DNR		2	/* Something opendir(3) failed on	*/
#define FTW_NS		3	/* Something stat(2) failed on		*/


#ifdef	RCSID
static char RCS[] = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/lib/RCS/ftw.c,v 2.0.1.1 1995/09/15 09:51:58 pfeifer Exp $";
#endif	/* RCSID */

#define EQ(a, b)	(strcmp((a), (b)) == 0)

/*
extern char		*malloc();
extern char		*strcpy();
*/
int
ftw(directory, funcptr, depth)
    char		 *directory;
    int			(*funcptr)();
    int			  depth;
{
    register DIR	 *dirp;
    Direntry_t		 *entp;
    struct stat		  stats;
    register char	 *p;
    register int	  i;
    long		  seekpoint;
    char		 *fullpath;

    /* If can't stat, tell the user so. */
    if (stat(directory, &stats) < 0)
	return (*funcptr)(directory, &stats, FTW_NS);

    /* If it's not a directory, call the user's function. */
    if ((stats.st_mode & S_IFMT) != S_IFDIR)
	/* Saying "FTW_F" here is lying; what if this is a symlink? */
	return (*funcptr)(directory, &stats, FTW_F);

    /* Open directory; if we can't, tell the user so. */
    dirp = opendir(directory);
    if (dirp == NULL)
	return (*funcptr)(directory, &stats, FTW_DNR);

    /* See if user wants to go further. */
    i = (*funcptr)(directory, &stats, FTW_D);
    if (i) {
	closedir(dirp);
	return i;
    }

    /* Get ready to hold the full paths. */
    i = strlen(directory);
    fullpath = (char *)malloc(i + 1 + MAXNAMLEN + 1);
    if (fullpath == NULL) {
	closedir(dirp);
	return -1;
    }
    (void)strcpy(fullpath, directory);
    p = &fullpath[i];
    if (i && p[-1] != '/')
	*p++ = '/';

    /* Read all entries in the directory.. */
    while (entp = readdir(dirp))
	if (!EQ(entp->d_name, ".") && !EQ(entp->d_name, "..")) {
	    if (depth <= 1) {
		/* Going too deep; checkpoint and close this directory. */
		seekpoint = telldir(dirp);
		closedir(dirp);
		dirp = NULL;
	    }

	    /* Process the file. */
	    (void)strcpy(p, entp->d_name);
	    i = ftw(fullpath, funcptr, depth - 1);
	    if (i) {
		/* User's finished; clean up. */
		free(fullpath);
		if (dirp)
		    closedir(dirp);
		return i;
	    }

	    /* Reopen the directory if necessary. */
	    if (dirp == NULL) {
		dirp = opendir(directory);
		if (dirp == NULL) {
		    free(fullpath);
		    return -1;
		}
		seekdir(dirp, seekpoint);
	    }
	}

    /* Clean up. */
    free(fullpath);
    closedir(dirp);
    return 0;
}
