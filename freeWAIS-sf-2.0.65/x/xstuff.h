/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   Version 0.8
   Fri Feb  1 1991

   jonathan@Think.COM

*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef _H_XSTUFF
#define _H_XSTUFF

#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>

#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Dialog.h>

#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Sme.h>
#include <X11/Xaw/SmeBSB.h>

#include "MyList.h"

#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/List.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/Text.h>
# define COMMANDBUTTON xmPushButtonWidgetClass
# define COMMANDCALLBACK XmNactivateCallback
# define TEXTWIDGET xmTextWidgetClass
# define NSTRING XmNvalue
# define LABELWIDGET xmLabelWidgetClass
# define NLABEL XmNlabelString
# define VERTVAL False
# define SENSVAL True
#else
# define COMMANDBUTTON commandWidgetClass
# define COMMANDCALLBACK XtNcallback
# define TEXTWIDGET asciiTextWidgetClass
# define NSTRING XtNstring
# define LABELWIDGET labelWidgetClass
# define NLABEL XtNlabel
# define VERTVAL True
# define SENSVAL False
#endif

#endif
