/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef _H_WAIS
#define _H_WAIS

/* usefull definitions */

#define STRINGSIZE	256

/*
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
*/

/* from the IR directory */

#include <cutil.h>
#include <ui.h>
#include <irfileio.h>
#include <sockets.h>

#ifdef _IBMR2
#include <string.h>
#endif /* IBMR2 */

/* for this application */

#include "list.h"
#include "source.h"
#include "document.h"
#include "question.h"
#include "wais-ui.h"
#include "status.h"
#include "messages.h"
#endif

/*
 * $Log: wais.h,v $
 * Revision 2.0.1.1  1995/09/15 09:55:43  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:05:44  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:20:08  pfeifer
 * Release beta 04
 *
 * Revision 1.1  93/06/23  20:02:43  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.10  92/06/03  17:17:54  jonathan
 * Include string.h (for prototypes) for IBMR2. From mycroft@gnu.ai.mit.edu.
 * 
 * Revision 1.9  92/04/30  12:23:55  jonathan
 * Removed redundant includes.
 * 
 * Revision 1.8  92/03/17  14:31:44  jonathan
 * Generally cleaned up.
 * 
 * Revision 1.7  92/03/06  14:51:43  jonathan
 * Added sdir and cdir for source loading.
 * 
 * Revision 1.6  92/02/15  19:50:24  jonathan
 * Removed old cruft.  Added $Log for RCS
 * 
*/
