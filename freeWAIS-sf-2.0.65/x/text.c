/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/*
 * $Log: text.c,v $
 * Revision 2.0.1.1  1995/09/15 09:57:19  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:15  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:27:19  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.1  92/06/15  13:44:05  jonathan
 * Initial revision
 * 
 */

#include "xwais.h"

TextList
NewText()
{
  TextList result;

  if(NULL != (result = (TextList) s_malloc(sizeof(_TextList)))) {
    if(NULL == (result->thisText = (Textbuff) s_malloc(sizeof(_Textbuff)))) {
      s_free(result);
      result = NULL;
    }
    else {
      result->thisText->shell = NULL;
      result->thisText->textwindow = NULL;
      result->thisText->status = NULL;
      result->thisText->docid = NULL;
      result->thisText->text = NULL;
      result->thisText->type = NULL;
      result->thisText->size = 0;
      result->nextText = allText;
      allText = result;
    }
  }
  return(result);
}

void
KillText(t)
Textbuff t;
{
  TextList a_tList, tmp;

  if(t != NULL) {
    if(t->shell != NULL) XtDestroyWidget(t->shell);
    if(t->text != NULL) s_free(t->text);
    if(t->type != NULL) s_free(t->type);
    for(a_tList = allText; a_tList != NULL; a_tList = a_tList->nextText)
      if(a_tList->thisText == t) {
	if(a_tList->nextText != NULL) {
	  a_tList->thisText = a_tList->nextText->thisText;
	  tmp = a_tList->nextText;
	  a_tList->nextText = a_tList->nextText->nextText;
	  s_free(tmp);
	}
	else {
	  a_tList->thisText = NULL;
	  if(a_tList == allText) {
	    s_free(a_tList);
	    allText = NULL;
	  }
	}
	break;
      }
    s_free(t);
  }
}

Textbuff
findText(w)
Widget w;
{
  TextList a_tList;
  static Widget shell = NULL;

  if (w != NULL) 
    if((shell = XtParent(w)) != NULL)
      shell = XtParent(shell);
  if (shell != NULL) {
    for(a_tList = allText; a_tList != NULL; a_tList = a_tList->nextText)
      if(a_tList->thisText->shell != NULL)
	  if (a_tList->thisText->shell == shell)
	    return a_tList->thisText;
  }
  return NULL;
}

Textbuff
findTextDoc(doc, type)
DocumentID doc;
char *type;
{
  TextList a_tList;

  if (doc != NULL) { 
    for(a_tList = allText; a_tList != NULL; a_tList = a_tList->nextText)
      if(a_tList->thisText != NULL &&
	 a_tList->thisText->docid == doc) {
	if(a_tList->thisText->type != NULL &&
	   type != NULL  &&
	   !strcmp(a_tList->thisText->type, type))
	  return (a_tList->thisText);
      }
  }
  return NULL;
}

