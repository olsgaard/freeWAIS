/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.	
  
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/lock.c,v 2.0.1.1 1995/09/15 09:46:35 pfeifer Exp $";
#endif

/* Change log:
 * $Log: lock.c,v $
 * Revision 2.0.1.1  1995/09/15 09:46:35  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:00:50  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.4  1994/12/13  18:52:45  pfeifer
 * chip@chinacat.unicom.com (Chip Rosenthal) patches.
 * Excluding the merge of libinv and libwais
 *
 * Revision 1.3  1994/05/20  12:56:50  pfeifer
 * beta
 *
 * Revision 1.2  1994/03/08  21:06:33  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.4  92/05/06  17:33:25  jonathan
 * ?
 * 
 * Revision 1.3  92/04/28  17:05:23  jonathan
 * K&R style definitions.
 * 
 */

/* ----------------------------------------------------------------------------

	COPYRIGHT	1992 Thinking Machines Corporation
	AUTHOR		M. Tracy Shen
                        Modified version of Gordon Linoff's lock.c
	MODULE		lock.c -- handle lock files all in one place
	INTERFACES
            utlk_using_lock()   - check the existence of a lock file
            utlk_set_lock()     - set a lock file if one doesn't exist
            utlk_unset_lock()   - unset a lock file if one exists
	INTERNAL ROUTINES
	COMMENTS
---------------------------------------------------------------------------- */

#include "cdialect.h"
#include <stdio.h> 
#include <sys/param.h>
#include <errno.h>		/* errno, EWOULDBLOCK, etc. */

#ifdef Mach
extern int errno;
#endif

#define SIGNOP 0		/* for using kill() to check if process is
				   alive */ 

#define FALSE 0
#define TRUE  1


#define LOCK_STORAGE_MODULE
#include "lock.h"
#undef LOCK_STORSGE_MODULE

#ifndef MAXPATHLEN
# define MAXPATHLEN 1024
#endif

/******************************************************************************
 *
 *	Function Name:	utlk_using_lock_and_get_pid
 *
 *
 *	Purpose:	check if update or query server is running
 *
 *	Returns:	return TRUE if process running, FALSE if not running
 *			NOT_RUNNING.
 *                      If pid is not NULL, sets it to the process id of
 *                      locking process.
 *
 *	Algorithm:	Find a lock file in /tmp for update or query, read
 *			pid from file, be sure process is running with that
 *			pid.
 *	End Algorithm
 */

long
    utlk_using_lock_and_get_pid (dbname, lock_type,pid_ptr)
char *dbname;
long    lock_type;
unsigned *pid_ptr;
{
    /* local variables */
    char lockfile_name[MAXPATHLEN];	/* absolute pathname of update lock */
    FILE *file_ptr;			/* for opening lock files */
    long pid;				/* pid of SC or update process */
    long status;

    if (! IN_LOCK_RANGE(lock_type)) {
	/* Out of range locks should never happen, but just in case . . . */
	return(FALSE);
        }

    sprintf( lockfile_name, "%s%s",dbname, lock_names[lock_type]);

    file_ptr = fopen(lockfile_name, "r"); /* look for an existing lock file */

    if (file_ptr == NULL) { 	/* didn't find lock file! */
	return(FALSE);
       }

    /* read pid from the lock file */
    status = fscanf(file_ptr, "%d", &pid);
    fclose(file_ptr);
    if (status == 0) {  /* is an empty lock file */
	return(FALSE);
        }

    /* Try a dummy kill; if ESRCH, process is not found.  Otherwise, assume
     * it's there. */
    status = kill(pid, SIGNOP);
    if ((status != 0) && (errno == ESRCH)) {
	return(FALSE);
    }
    if (pid_ptr != NULL) *pid_ptr = pid;
    return(TRUE);

    }  /* end utlk_using_lock_and_get_pid */

/******************************************************************************
 *
 *	Function Name:	utlk_using_lock
 *
 *
 *	Purpose:	check if update or query server is running
 *
 *	Returns:	return TRUE if process running, FALSE if not running
 *			NOT_RUNNING.
 *
 *	Algorithm:	Find a lock file in /tmp for update or query, read
 *			pid from file, be sure process is running with that
 *			pid.
 *	End Algorithm
 */

long
    utlk_using_lock (dbname, lock_type)
char *dbname;
long lock_type;
{
    return(utlk_using_lock_and_get_pid(dbname, lock_type, NULL));
}  /* end utlk_using_lock */



/******************************************************************************
 *
 *	Function Name:	utlk_unset_lock
 *
 *
 *	Purpose:	free lock
 *
 *	Returns:	Return TRUE is successful, FALSE if lock cannot be unset
 *
 *	Algorithm:	
 *	End Algorithm
 */

long
    utlk_unset_lock (dbname, lock_type)
char *dbname;
long lock_type;
{
    /* local variables */
    char lockfile_name[MAXPATHLEN];	/* absolute pathname of update lock */
    
    /* begin executable unset_lock */
    
    if (! IN_LOCK_RANGE(lock_type)) {
	/* Out of range locks should never happen, but just in case . . . */
	return(TRUE);
    }
    
    sprintf( lockfile_name, "%s%s",dbname, lock_names[lock_type]);
    
    return(unlink(lockfile_name) == 0);
}  /* end utlk_unset_lock */



/******************************************************************************
 *
 *	Function Name:	utlk_set_lock_with_pid
 *
 *
 *	Purpose:	set lock, process id is argument
 *
 *	Returns:	Return TRUE is successful, FALSE if lock cannot be set
 *
 *	Algorithm:	
 *	End Algorithm
 */

long
    utlk_set_lock_with_pid (dbname, lock_type, pid)
char *dbname;
long lock_type;			/* LOCK_UPDATE, etc. */
long pid;			/* pid of SC or update process */
{
    /* local variables */
    char lockfile_name[MAXPATHLEN];	/* absolute pathname of update lock */
    FILE *file_ptr;			/* for opening lock files */

    /* begin executable set_lock */
    
    if (! IN_LOCK_RANGE(lock_type)) {
	/* Out of range locks should never happen, but just in case . . . */
	return(FALSE);
	}
    
    sprintf( lockfile_name, "%s%s",dbname, lock_names[lock_type]);

    /* Don't look for an existing file -- just write over it */
    file_ptr = fopen(lockfile_name, "w");
    if (file_ptr == NULL) {	/* no write permission to db  */
	return(FALSE);
    }
    fprintf(file_ptr, "%d", pid);
    fclose(file_ptr);
    return(TRUE);
}  /* end utlk_set_lock */


/******************************************************************************
 *
 *	Function Name:	utlk_set_lock
 *
 *
 *	Purpose:	set lock
 *
 *	Returns:	Return TRUE is successful, FALSE if lock cannot be set
 *
 *	Algorithm:	
 *	End Algorithm
 */

long
    utlk_set_lock (dbname, lock_type)
char *dbname;
long lock_type;
{
    return(utlk_set_lock_with_pid(dbname, lock_type, getpid()));
}  /* end utlk_set_lock */
