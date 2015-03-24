/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM

*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/x/RCS/util.c,v 2.0.1.1 1995/09/15 09:57:25 pfeifer Exp $";
#endif

#define _C_UTIL

#define XWAIS
#include "xwais.h"

#include <X11/Xaw/TextP.h> /* for XawLF */

#ifdef XawLF
# define X11R5
#else
# ifdef X11R5
#  undef X11R5
# endif
#endif

int get_selected_item(w)
Widget w;
{
#ifdef MOTIF
  int *positions, size, i;

  if(XmListGetSelectedPos(w, &positions, &size) == TRUE)
    return (positions[0]-1);

  return NO_ITEM_SELECTED;
#else
  XawListReturnStruct *selected;

  if((selected = XawListShowCurrent(w)) == NULL)
    return NO_ITEM_SELECTED;
  else return selected->list_index;
#endif
}

int listlength(list)
List list;
{
  int num;
  List l;

  for(num = 0, l = list; l != NULL; num++, l = l->nextNode);

  return num;
}

int charlistlength(list)
char **list;
{
  int num;

  if(list) {
    for(num = 0; list[num] != NULL; num++);
    return num;
  }
  else 
    return 0;
}

void
PrintStatusW(str, w, replacep)
char * str;
Widget w;
boolean replacep;
{
  XEvent event;
  XawTextBlock text;
  static long lastlen = 0;
  XawTextPosition pos = XawTextGetInsertionPoint(w);

  if(w == NULL)
    fprintf(stderr, "Couldn't display status, invalid window.\n%s", str);
  else {
    text.length = strlen(str);
    text.ptr = str;
    text.firstPos = 0;
    text.format = FMT8BIT;

    XawTextReplace(w, (replacep?pos-lastlen:pos), pos, &text);

    pos += text.length;
    XawTextSetInsertionPoint(w, pos);
    lastlen = text.length;
    XawTextDisplay(w);
  }
  while((XtPending()&XtIMXEvent) != 0) {
    XtNextEvent(&event);
    XtDispatchEvent(&event);
  }
}

void PrintStatus
#ifdef ANSI_LIKE /* use ansi varargs */
(long logtype, long loglevel, char* format, ...)
#else /* k&r */
(va_alist)
va_dcl
#endif ANSI_LIKE
{
  va_list ap;			/* the variable arguments */
  char message[1000];
  static long last_type = STATUS_INFO;

#ifndef ANSI_LIKE
  long logtype, loglevel;
  char *format;

  va_start(ap);
  logtype = va_arg(ap, int);
  loglevel = va_arg(ap, int);
  format = va_arg(ap, char*);
#else
  va_start(ap, format);
#endif

  vsprintf(message, format, ap);

  PrintStatusW(message, messwidget, 
	       (last_type == STATUS_CONT && logtype == STATUS_CONT));
  last_type = logtype;

  if (logtype == STATUS_URGENT) sleep(2);
#ifndef ANSI_LIKE
  va_end(ap);
#endif
}

void
XwaisPrintf(str)
char * str;
{
  PrintStatusW(str, messwidget, false);
}

void
ReplaceText(w, str)
Widget w;
char * str;
{
#ifdef MOTIF
  int	lastpos;
  char *spaces = "";
  lastpos = XmTextGetLastPosition(w);
    
  XmTextReplace(w,0,lastpos,spaces);
  XmTextSetInsertionPosition(w,0);

  XmTextReplace(w,0,0,str);
  XmTextSetInsertionPosition(w,0);
#else
  ReplaceAsciiText(w, str);
#endif
}

void ReplaceAsciiText(w, str)
Widget w;
char* str;
{
  Arg args[ONE];

  XtSetArg(args[ZERO], XtNstring, str);
  XtSetValues(w, args, ONE);
}

Widget
MakeCommandButton(box, name, function, vlink, hlink, data)
Widget box, vlink, hlink;
char *name;
XtCallbackProc function;
caddr_t data;
{
  Widget w;
  Arg args[10];
  Cardinal numargs;

  numargs = 0;
  if(vlink != NULL) {
    XtSetArg(args[numargs], XtNfromVert, vlink); numargs++;
  }
  if(hlink != NULL) {
    XtSetArg(args[numargs], XtNfromHoriz, hlink); numargs++;
  }
  w = XtCreateManagedWidget(name, COMMANDBUTTON, box, args, numargs);
  if (function != NULL)
    XtAddCallback(w, COMMANDCALLBACK, function, data);
  return w;
}

Widget
MakeListWidget(parent, name, list, callback, vlink, hlink)
Widget parent, vlink, hlink;
String name;
char **list;
XtCallbackProc callback;
{
  Arg args[6];
  Cardinal numargs;
  Widget ListW, form;

  numargs = 0;
  if (vlink != NULL) {
    XtSetArg(args[numargs], XtNfromVert, vlink); numargs++;
  }
  if (hlink != NULL) {
    XtSetArg(args[numargs], XtNfromHoriz, hlink); numargs++;
  }
#ifdef MOTIF
  form = XtCreateManagedWidget (name, formWidgetClass,
				parent, args, numargs);
  numargs = 0;
  XtSetArg( args[numargs], XmNselectionPolicy, XmSINGLE_SELECT); numargs++;
  XtSetArg(args[numargs], XmNscrollBarDisplayPolicy, XmSTATIC); numargs++;
  ListW = XmCreateScrolledList(form, name, args, numargs);
  XtManageChild(ListW);
#else
  XtSetArg(args[numargs], XtNhorizDistance, 0); numargs++;
  XtSetArg(args[numargs], XtNlist, list); numargs++;
  XtSetArg(args[numargs], XtNforceColumns, True); numargs++;
  XtSetArg(args[numargs], XtNdefaultColumns, 1); numargs++;
#ifdef X11R5
  ListW = XtCreateManagedWidget(name, myListWidgetClass,
				parent, args, numargs);
#else
  ListW = XtCreateManagedWidget(name, listWidgetClass,
				parent, args, numargs);
#endif
#endif
  if (callback != NULL)
    XtAddCallback(ListW, 
#ifdef MOTIF
		  XmNdefaultActionCallback,
#else
		  XtNcallback,
#endif
		  callback, NULL);

  return ListW;
}  

#ifdef MOTIF
static void
 moveViewport(p, action)
Widget p;
long action;
{
  return;
}
#else
#include <X11/Xaw/ViewportP.h>

static void SetBar(w, top, length, total)
    Widget w;
    Position top;
    Dimension length, total;
{
    XawScrollbarSetThumb(w, (float)top/(float)total,
			 (float)length/(float)total);
}

#ifdef X11R5

static void
 moveViewport(p, action)
Widget p;
long action;
{
  if (action == LIST_TOP) {
    XawViewportSetLocation(p, 0.0, 0.0);
  }
  else if (action == LIST_BOTTOM) {
    XawViewportSetLocation(p, 0.0, 1.0);
  }
}

#else /* X11R4 is badly broken.  Lets see if we can fix it. */

static void
 moveViewport(p, action)
Widget p;
long action;
{
  ViewportWidget w = (ViewportWidget)p;
  Widget child, clip, scrollbar;

  if(w->composite.num_children >= 3) { 
    /* 3 widgets: clip, child, and scrollbar (I hope) */
    clip =  w->composite.children[0];
    child = w->composite.children[1];
    scrollbar = w->composite.children[2];

    if (action == LIST_TOP) {
      XtMoveWidget(child, 0, 0);
      SetBar(scrollbar, 0,
	     clip->core.height, child->core.height );
    }
    else if (action == LIST_BOTTOM) {
      XtMoveWidget(child, 0, -(child->core.height - clip->core.height));
      SetBar(scrollbar, -(child->core.y),
	     clip->core.height, child->core.height );
      return;
    }
  }
}

#endif /* X11R5 */
#endif /* MOTIF */

void RebuildListWidget(s, list, action)
ScrollList s;
String *list;
long action;
{
  Widget p = XtParent(s->ListWidget);

  if(s->ListWidget == NULL)
      s->ListWidget = MakeListWidget(p, s->name, list, s->callback, NULL, NULL);
  else {
#ifdef MOTIF
    XmString strn;

    if (s != NULL) {
      XmListDeleteAllItems( s->ListWidget );
      while(*list) {
	strn = XmStringCreateSimple( *list );
	XmListAddItem( s->ListWidget, strn, 0);
	XmStringFree( strn );
	list++;
      }
    }
#else
    if (list != NULL) XawListChange(s->ListWidget, list, 0, 0, True);
    moveViewport(p, action);
#endif
  }
}

ScrollList
MakeScrollList(parent, name, items, callback, vlink, hlink)
Widget parent, vlink, hlink;
XtCallbackProc callback;
String name, *items;
{
  Arg arglist[10];
  Cardinal num_args;
  static char names[STRINGSIZE];
  ScrollList result;

  result = (ScrollList) s_malloc(sizeof(_ScrollList));

  result->name = s_strdup(name);
  result->callback = callback;

  num_args = 0;
  if(vlink != NULL) {
    XtSetArg(arglist[num_args], XtNfromVert, vlink); num_args++;
  }
  if(hlink != NULL) {
    XtSetArg(arglist[num_args], XtNfromHoriz, hlink); num_args++;
  }
  XtSetArg(arglist[num_args], XtNorientation, XtorientVertical); num_args++;
  result->ListWidget = MakeListWidget(parent, name, items, 
				      callback, vlink, NULL);

  return(result);
}

Widget 
MakeStringBox(parent, name, string, vlink, hlink)
Widget parent, vlink, hlink;
String name, string;
{
  Arg args[5];
  Cardinal numargs = 0;
  Widget StringW;

#ifndef MOTIF
  XtSetArg(args[numargs], XtNeditType, XawtextEdit); numargs++;
#endif
  XtSetArg(args[numargs], NSTRING, string); numargs++;
  if(vlink != NULL) {
    XtSetArg(args[numargs], XtNfromVert, vlink); numargs++;
  }
  if(hlink != NULL) {
    XtSetArg(args[numargs], XtNfromHoriz, hlink); numargs++;
  }

  StringW = XtCreateManagedWidget(name, TEXTWIDGET, parent, args, numargs);
  return(StringW);  
}
 
#ifdef MOTIF
Widget 
MakeScrolledText(parent, name, string, vlink, hlink)
Widget parent, vlink, hlink;
String name, string;
{
  Arg args[5];
  Cardinal numargs = 0;
  Widget form, StringW;
  char *textWname[STRINGSIZE];

  if(vlink != NULL) {
    XtSetArg(args[numargs], XtNfromVert, vlink); numargs++;
  }
  if(hlink != NULL) {
    XtSetArg(args[numargs], XtNfromHoriz, hlink); numargs++;
  }

  form = XtCreateManagedWidget (name, formWidgetClass,
				parent, args, numargs);

  sprintf(textWname, "textW%s", name);
  numargs = 0;
  XtSetArg(args[numargs], XmNeditMode, XmMULTI_LINE_EDIT); numargs++;

  StringW = XmCreateScrolledText(form, textWname, args, numargs);
  XtManageChild(StringW);
  ReplaceText(StringW, string);
  return(StringW);  
}

Widget MakeSeparator(parent, name, vlink)
Widget parent, vlink;
String name;
{
  Arg args[5];
  Cardinal numargs = 0;
  Widget sep;

  if(vlink != NULL) {
    XtSetArg(args[numargs], XtNfromVert, vlink); numargs++;
  }

  sep = XtCreateManagedWidget(name, xmSeparatorWidgetClass, parent,
			      args, numargs);

  return(sep);  
}
#endif

/*	Function Name: GetString
 *	Description: retrieves the string from a asciiText widget.
 *	Arguments: w - the ascii text widget.
 *	Returns: the filename.
 */

String
GetString(w)
Widget w;
{
  String str;
  Arg args[1];
  
  XtSetArg(args[0], NSTRING, &str);
  XtGetValues( w, args, ONE);
  return(str);
}

String
GetAsciiString(w)
Widget w;
{
  String str;
  Arg args[1];
  
  XtSetArg(args[0], XtNstring, &str);
  XtGetValues( w, args, ONE);
  return(str);
}

Widget MakeLabel(parent, name, label, vlink, hlink)
Widget parent, vlink, hlink;
char *name, *label;
{
  Arg args[5];
  Cardinal numargs;
  Widget labelwid;
#ifdef MOTIF
  XmString strn = XmStringCreateSimple(label);
#else
  char* strn = label;
#endif

  numargs = 0;
  XtSetArg(args[numargs], NLABEL, strn); numargs++;
  if(vlink != NULL) {
    XtSetArg(args[numargs], XtNfromVert, vlink); numargs++;
  }
  if(hlink != NULL) {
    XtSetArg(args[numargs], XtNfromHoriz, hlink); numargs++;
  }
  /* XtSetArg(args[numargs], XtNborderWidth, 0); numargs++; Use default borderWidth dimension; i.e. 1 */
  labelwid = XtCreateManagedWidget(name,LABELWIDGET, parent, args, numargs);
#ifdef MOTIF
  XmStringFree( strn );
#endif
  return(labelwid);
}

#define BEFORE 1
#define DURING 2
#define QUOTE 5

/* ripped out of gmacs-ui.c */
int find_string_slot(source, key, value, value_size, delete_internal_quotes)
char *source, *key, *value;
long value_size;
boolean delete_internal_quotes;
{
  char ch;
  short state = BEFORE;
  long position = 0;  /* position in value */
  char *pos =(char*)strstr(source, key); /* address into source */

  value[0] = '\0';		/* initialize to nothing */

  if(NULL == pos)
    return(1);

  for(pos = pos + strlen(key); pos < source + strlen(source); pos++){
    ch = *pos;
    if((state == BEFORE) && (ch == '\"'))
      state = DURING;
    else if ((state == DURING) && (ch == '\\')){
      state = QUOTE;	
      if(!delete_internal_quotes){
	value[position] = ch;
	position++;
	if(position >= value_size){
	  value[value_size - 1] = '\0';
	  return(-1);
	}
      }
    }
    else if ((state == DURING) && (ch == '"')){	
      value[position] = '\0';
      return(0);
    }
    else if ((state == QUOTE) || (state == DURING)){
      if(state ==  QUOTE)
	state = DURING;
      value[position] = ch;
      position++;
      if(position >= value_size){
	value[value_size - 1] = '\0';
	return(-1);
      }
    }
    /* otherwise we are still before the start of the value */
  }
  value[position] = '\0';
  return(-1); /* error because we are in the middle of the string */
}

void find_value(source, key, value, value_size)
char *source, *key, *value;
int value_size;
{
  char ch;
  long position = 0;  /* position in value */
  char *pos =(char*)strstr(source, key); /* address into source */

  value[0] = '\0';		/* initialize to nothing */

  if(NULL == pos)
    return;

  pos = pos + strlen(key);
  ch = *pos;
  /* skip leading quotes and spaces */
  while((ch == '\"') || (ch == ' ')) {
    pos++; ch = *pos;
  }
  for(position = 0; pos < source + strlen(source); pos++){
    if((ch = *pos) == ' ') {
      value[position] = '\0';
      return;
    }
    value[position] = ch;
    position++;
    if(position >= value_size){
      value[value_size - 1] = '\0';
      return;
    }
  }
  value[position] = '\0';
}

void SortSources()
{
  Boolean Changed = TRUE;
  Source s;
  SList sl;

  while(Changed) {
    Changed = FALSE;
    for(sl = Sources; sl->nextSource != NULL; sl = sl->nextSource) {
      if(sl->nextSource->thisSource->name == NULL) {
	Changed = TRUE;
	sl->nextSource = sl->nextSource->nextSource;
      }
      else
	if(0 < strcmp(sl->thisSource->name, sl->nextSource->thisSource->name)) {
	  Changed = TRUE;
	  s = sl->thisSource;
	  sl->thisSource = sl->nextSource->thisSource;
	  sl->nextSource->thisSource = s;
	}
    }
  }
}

char *
get_filename(name)
char* name;
{
  char *result, *loc;
  long i;

  loc = (char*)strchr(name,' ');
  if (loc == NULL) return name;

  i = (long)loc - (long)name;
  
  result = s_malloc(i+1);
  strncpy(result, name, i);
  result[i] = 0;
  return result;
}

void
Feep()
{
  if (CurDpy != NULL)
    XBell(CurDpy, 0);
}

extern int alphasort();

char **
GetDirNames(directory)
char *directory;
{
  char **result;
  Direntry_t **list;
  int i, j;

  if ((j = scandir(directory, &list, NULL, alphasort)) < 0) {
    PrintStatus(STATUS_URGENT, STATUS_HIGH,
		"Error on open of source directory: %s.\n", directory);
    result = NULL;
  }
  else {
    result = (char**) s_malloc((j+1) * sizeof(char*));

    for (i = 0; i < j; i++) {
      result[i] = s_strdup(list[i]->d_name);
      s_free(list[i]);
    }
    s_free(list);
  }
  return result;
}

#include "xwaist.bit"

void SettIcon(parent)
Widget parent;
{
  Arg args[1];
  Pixmap icon_pixmap = None;

  XtSetArg (args[ZERO], XtNiconPixmap, &icon_pixmap);
  XtGetValues(parent, args, ONE);
  if (icon_pixmap == None) {
    XtSetArg(args[ZERO], XtNiconPixmap, 
	     XCreateBitmapFromData(XtDisplay(parent),
				   XtScreen(parent)->root,
				   xwaist_bits, xwaist_width, xwaist_height));
    XtSetValues (parent, args, ONE);
  }
}

static Widget helpwindow = NULL;

void EndHelp(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  XtPopdown(helpwindow);
}

#ifdef MOTIF
static insert_help(w, filename)
char *filename;
{
  FILE *hf;
  char buffer[10000];
  int loc;

  if((hf= fopen(filename, "r")) == NULL) {
    XwaisPrintf("Error opening help file");
  }
  loc = fread(buffer, 1, 10000, hf);
  buffer[loc] = 0;
  ReplaceText(w, buffer);

  fclose(hf);
}  
#endif

/* ARGSUSED */
void XwaisHelp(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  Widget textwindow, frame, button;
  Arg arglist[10];
  Cardinal num_args;
  static String items[] = {NULL};

  if (helpwindow == NULL) {
    num_args = 0;
    XtSetArg(arglist[num_args], XtNtitle, "X WAIS Help"); num_args++;
    XtSetArg(arglist[num_args], XtNiconName, "X WAIS Help"); num_args++;
    helpwindow = XtCreatePopupShell("textpopup", applicationShellWidgetClass,
				    top, arglist, num_args);
    frame =
      XtCreateManagedWidget("helppopupform", formWidgetClass,
			    helpwindow, NULL, ZERO);
#ifdef MOTIF
    textwindow =
      MakeScrolledText(frame, "textWindow", "", NULL, NULL);
    button = MakeCommandButton(frame, "tdone", EndHelp,
			       XtParent(XtParent(textwindow)), NULL);
    insert_help(textwindow, app_resources.helpFile);
#else
    num_args = 0;
    XtSetArg(arglist[num_args], XtNtype, XawAsciiFile); num_args++;
    XtSetArg(arglist[num_args], XtNstring, app_resources.helpFile); num_args++;
    XtSetArg(arglist[num_args], XtNeditType, XawtextRead); num_args++;
    textwindow =
      XtCreateManagedWidget("textWindow", asciiTextWidgetClass, frame, arglist, num_args);
    button = MakeCommandButton(frame, "tdone", EndHelp,
			       textwindow, NULL, NULL);
#endif
    SettIcon(helpwindow);
  }
  XtPopup(helpwindow, XtGrabNone);
}

/*
 * $Log: util.c,v $
 * Revision 2.0.1.1  1995/09/15 09:57:25  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:19  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.3  1994/12/23  16:46:16  pfeifer
 * Use default borderWidth dimension; i.e. 1
 *
 * Revision 1.2  1994/08/05  07:27:37  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.14  92/06/22  10:50:22  jonathan
 * Removed app_context from calls, using Xt calls instead.
 * 
 * Revision 1.13  92/06/15  13:46:55  jonathan
 * Modified PrintStatus to use variable length arguments, and added two new
 * args: logtype and loglevel (defined in ui/status.h).
 * 
 */
