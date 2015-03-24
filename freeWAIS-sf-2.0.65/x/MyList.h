/*
 * MyList.h - public declarations
 *
 * Subclass of the Athena List widget to provide notification on
 * de-selection of list items.
 *
 * Written by Garrett A. Wollman, Garrett.Wollman@uvm.edu.
 *
 * Copyright 1992, University of Vermont and State Agricultural College.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of U.V.M. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  U.V.M. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * U.V.M. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL U.V.M.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* $Id: MyList.h,v 2.0.1.1 1995/09/15 09:56:08 pfeifer Exp $ */

#ifndef _h_MyList		/* use programmer's namespace */
#define _h_MyList

/* Resources:
 * All inherited from parent.
 */

typedef struct _MyListClassRec *MyListWidgetClass;
typedef struct _MyListRec *MyListWidget;

/* The class constant */
extern WidgetClass myListWidgetClass;

#endif /* defined( _h_MyList ) */
