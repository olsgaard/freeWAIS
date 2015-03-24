/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* $Log: util.h,v $
 * Revision 2.0.1.1  1995/09/15 09:57:28  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:21  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:27:41  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.9  92/06/22  10:49:36  jonathan
 * ?
 * 
 * Revision 1.8  92/06/15  13:49:09  jonathan
 * Removed define for PrintStatus - that's in ui/status.h now.
 * 
 * Revision 1.7  92/06/03  17:07:42  jonathan
 * Added action to RebuildListWidget, along with defines for the actions.
 * Modifed RebuildListWidget to scroll the viewport and not kill the
 * listwidget.  Mucho cleanero.
 * 
 * Revision 1.6  92/04/28  15:32:11  jonathan
 * Modified prototypes for new functionality.
 * 
 * Revision 1.5  92/03/23  16:10:14  jonathan
 * Nothing much.
 * 
 * Revision 1.4  92/03/17  14:19:26  jonathan
 * fixed header
 * 
 */

#ifndef _H_UTIL
#define _H_UTIL

#define LIST_TOP 1
#define LIST_BOTTOM 2
#define LIST_NONE 0

int get_selected_item _AP((Widget w));
int listlength _AP((List list));
int charlistlength _AP((char **list));
void PrintStatusW _AP((char * str,Widget w, boolean replacep));
void XwaisPrintf _AP((char * str));
void ReplaceText _AP((Widget w,char * str));
void ReplaceAsciiText _AP((Widget w,char * str));
Widget MakeCommandButton _AP((Widget box,char* name,XtCallbackProc function,
			      Widget vlink,Widget hlink,caddr_t data));
Widget MakeListWidget _AP((Widget parent,String name,char **list,
			   XtCallbackProc callback,Widget vlink,Widget hlink));
void RebuildListWidget _AP((ScrollList s,String *list, long action));
ScrollList MakeScrollList _AP((Widget parent,String name,String* items,
			       XtCallbackProc callback, 
			       Widget vlink,Widget hlink));
Widget MakeStringBox _AP((Widget parent,String name,String string,
			  Widget vlink,Widget hlink));
#ifdef MOTIF
Widget MakeScrolledText _AP((Widget parent,String name,String string,
			  Widget vlink,Widget hlink));
#endif
String GetString _AP((Widget w));
Widget MakeLabel _AP((Widget parent,char* name,char* label,
		      Widget vlink,Widget hlink));
int find_string_slot _AP((char* source,char* key,char* value,
			  long value_size, boolean delete_internal_quotes));
void find_value _AP((char* source,char* key,char* value, int value_size));
void SortSources _AP((void));
char* get_filename _AP((char* name));
void Feep _AP((void));
char** GetDirNames _AP((char* directory));
void EndHelp _AP((Widget w, XtPointer closure,XtPointer call_data));
void XwaisHelp _AP((Widget w, XtPointer closure,XtPointer call_data));
#endif
