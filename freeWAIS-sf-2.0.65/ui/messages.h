/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* some typical error messages */

#define DIAG_MESSAGE \
 "\nCode: %s, %s"

#define NOSELECT_MESSAGE \
 "\nNo selected item.  Select one and try again."

#define NODOC_MESSAGE \
 "\nUnable to find document.  This should not happen."

#define BADCONNECT_MESSAGE \
 "\nWarning: no information returned.  Possibly a bad connection"

#define BADALLOC_MESSAGE \
 "\nUnable to allocate space."

#define BUFFOVER_MESSAGE \
 "\nBuffer overflow!"

#define SEARCH_MESSAGE \
 "Searching %s..."

#define NOSEARCH_MESSAGE \
 "\nCan't search %s: Init Failure."

#define INITSOURCE_MESSAGE \
 "Initializing connection..."

#define FOUND_MESSAGE \
 "Found %d items."

#define BADWAIS_MESSAGE \
 "Failed to send WAIS request to source"

#define INITFAIL_MESSAGE \
 ": connection failed!"

#define NOSOURCE_MESSAGE \
 "\nCould not find Source: %s. "

#define VIEWSOURCE_MESSAGE \
 "\nViewing source: %s"

#define GETDOC_MESSAGE \
 "Getting \"%s\" from %s..."

#define BADNEXT_MESSAGE \
"\nCould not get next document."

#define BADPREV_MESSAGE \
 "\nCould not get previous document."

#define NOGETDOC_MESSAGE \
 "\nUnable to retrieve \"%s\" from %s."

#define RECEIVE_MESSAGE \
 "\nReceived %d bytes from %s..."

#define NODATA_MESSAGE \
 "\nNo data returned"

#define BADFOPEN_MESSAGE \
 "\nError opening file: %s."

#define ENTERFNAME_MESSAGE \
 "\nYou have to enter a filename."

#define UNKNOWNTYPE_MESSAGE \
 "\nDocument is of type: %s, which is unknown. Using Save routine."

#define FILTER_MESSAGE \
 "\nRunning '%s %s'"

#define ABORT_MESSAGE \
 "\nTransaction Aborted."

#define DONE_MESSAGE \
 "done."

/*
 * $Log: messages.h,v $
 * Revision 2.0.1.1  1995/09/15 09:53:39  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:05:13  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:30:09  pfeifer
 * Release beta 04
 *
 * Revision 1.1  93/06/23  20:02:35  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.1  92/06/21  12:40:01  jonathan
 * Initial revision
 * 
 * 
 */
