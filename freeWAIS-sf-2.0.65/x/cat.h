/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: cat.h,v $
 * Revision 2.0.1.2  1995/09/15 09:56:22  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.1  1995/09/10  13:00:40  pfeifer
 * patch3: Removed duplicate RCS log.
 *
 * Revision 2.0  1995/09/08  08:06:19  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.3  1994/08/05  07:23:04  pfeifer
 * Release beta 04
 *
 * Revision 1.2  93/07/19  16:33:03  warnock
 * Added catalog patches from Wayne Allen
 *
 * Revision 1.1  92/06/23  14:09:41  jonathan
 * Initial revision
 * 
 * 
 */

typedef struct catbuff {
  Widget	shell;
  SourceID	source;
  ScrollList	List;
  char*		database;
  String*	Items;
  String* 	headlines;
  String*	Docids;
  String*       Types;		/* einet */
  Widget	status;
  long          numDocs;	/* einet */
  void*         nextCat;	/* einet */
} _Catbuff, *Catbuff;

Catbuff build_cat _AP((char* catalog, SourceID source));
Widget MakeCatPopup _AP((Widget parent,Catbuff cat,char *name));

