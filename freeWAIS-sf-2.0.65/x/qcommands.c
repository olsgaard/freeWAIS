/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* this file contains X specific code - it is an integral part of XWAIS */
/* einet begin MAC */
/* This was previously '#ifndef lint' */
#ifdef RCSID    /* einet */
/* einet end */
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/x/RCS/qcommands.c,v 2.0.1.1 1995/09/15 09:56:57 pfeifer Exp $";
/*
 * was:
 * static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/x/RCS/qcommands.c,v 2.0.1.1 1995/09/15 09:56:57 pfeifer Exp $";
 *
 *
 * $Log: qcommands.c,v $
 * Revision 2.0.1.1  1995/09/15 09:56:57  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:06:58  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.8  1995/01/10  17:05:55  pfeifer
 * Bug wich causes coredump when submitting the same query more than once
 * No completely salvaged
 *
 * Revision 1.7  1994/11/24  10:04:45  pfeifer
 * XtMalloc for string was too short
 *
 * Revision 1.6  1994/08/05  07:25:53  pfeifer
 * Release beta 04
 *
 * Revision 1.5  1994/03/25  10:21:02  pfeifer
 * now will try to make 8-bit chars work
 *
 * Revision 1.4  93/07/19  16:32:27  warnock
 * Added catalog patches from Wayne Allen
 * 
 * Revision 1.5  93/07/18  warnock
 * Merged in Wayne Allen's mods for catalog widget, removed call to prep_retrieval
 *
 * Revision 1.4  1993/06/22  20:02:30  hardin
 * Plugged a MAJOR memory leak in SetupSearch.
 *
 * Revision 1.3  1993/01/08  21:31:22  wa
 * added call to prep_retrieval. This checks annoy and crash dates.
 *
 * Revision 1.2  1992/09/29  19:03:20  hardin
 * Changed "#ifndef lint" to "#ifdef RCSID"
 *
 * Revision 1.1  1992/09/23  10:09:22  wa
 * Initial revision
 *
 * Revision 1.28  92/06/22  13:58:29  jonathan
 * Added finish_search for loadSource failure in SetupSearch.
 *
 * Revision 1.27  92/06/22  10:51:44  jonathan
 * Removed references to myAsciiSink and myAsciiSrc.
 * Added function to return seed_words_used.
 *
 * Revision 1.26  92/06/15  13:54:46  jonathan
 * Lots of things.  Cleaned up a lot of the functions, moved things around.
 * Added stuff for Xt handling of IO.  No more alarms.
 *
 * Revision 1.25  92/06/03  17:09:50  jonathan
 * Modified calls to RebuildListWidget to position list widget appropriately.
 *
 * Revision 1.24  92/05/07  14:51:22  jonathan
 * Changed use of setitimer to alarm.  Thanks to
 * steinkel@carlisle-emh2.army.mil.
 *
 * Revision 1.23  92/04/30  12:22:11  jonathan
 * Modified to understand "quit no save" and desensitivize it when necessary.
 *
 * Revision 1.22  92/04/28  15:28:24  jonathan
 * Cleaned up use of scrollists.  Fixed bugs in double clicking.  Added
 * ability to view selected sources and relevant documents.
 *
 * Revision 1.21  92/03/23  16:08:43  jonathan
 * Added file requester, save as button, changed some sensitivity, that kind
 * of stuff.
 *
 * Revision 1.20  92/03/17  14:24:17  jonathan
 * Added Cursor code, timers, etc.  Really a lot of stuff.
 *
 * Revision 1.19  92/03/06  14:47:16  jonathan
 * New and Improved source loading!
 *
 * Revision 1.18  92/03/02  14:34:20  jonathan
 * Added -f to csh call, per blaze@think.com.
 *
 * Revision 1.17  92/03/01  14:02:08  jonathan
 * Modified all calls to PrintStatus to use PrintStatusW for compatibility
 * with ui PrintStatus.
 *
 * Revision 1.16  92/02/17  17:48:27  jonathan
 * Moved last_doc to a global, so it can be reset on a new search.
 *
 *
 * Revision 1.15  92/02/17  12:55:38  jonathan
 * Added WCAT type to text display.  Added RCSid and $Log too.
 *
 *
 */
#endif

#define _C_QCOMMANDS

#include "xwais.h"
#include "cat.h"
#include <X11/Xaw/TextP.h>
#include <setjmp.h>

#define WORLD_INTERVAL 100
#define IN_HEADER 1
#define IN_BODY 2

static jmp_buf jbuf;
static XtIntervalId world_timer = NULL;
static SourceList current_sourcelist;
#ifdef ORIGINAL	/* einet */
static DocList resdocs, last_resdocs;
#else			/* einet begin 22jun93 */
/* initialize these values! */
static DocList resdocs = NULL, last_resdocs = NULL;
#endif			/* einet end */
static boolean savep = false;
static Sbuff sb = NULL;

static Boolean editting_new_question,
  busy = FALSE, searching = FALSE;

static int last_doc = NO_ITEM_SELECTED,
  last_qdoc = NO_ITEM_SELECTED,
  last_source = NO_ITEM_SELECTED,
  last_qsource = NO_ITEM_SELECTED;

static XQuestion helpquestion = NULL;
static Textbuff current_text;

static int s_dsb = SENSVAL, s_adb = SENSVAL, s_ddb = SENSVAL;

/* macros */
#define get_selected_type() \
  get_selected_item(typewindow->ListWidget)

#define get_selected_qsource(question) \
  get_selected_item((question)->window->Sources->ListWidget)

#define get_selected_qdoc(question) \
  get_selected_item((question)->window->RelevantDocuments->ListWidget)

#define get_selected_response(question) \
  get_selected_item((question)->window->ResultDocuments->ListWidget)

#define get_question_response(questionwindow) \
  get_selected_item((questionwindow)->ResultDocuments->ListWidget)


/* forward declarations */

static void SetupSearch _AP((Question q));
static void SetupRetrieve _AP((Textbuff textstruct,long page,long size));

static void HandleDoc _AP((Sbuff sb));
static void DoTSaveCB _AP((Widget w,
			   XtPointer closure,
			   XtPointer call_data));

/* Private functions */

static long
GetLineFromPos(text, p)
char *text;
XawTextPosition p;
{
  long i, lines;

  for(lines=0, i=0; (i < p) && (*text != 0); i++, text++)
    if(*text == '\n') lines++;

  return lines;
}

static XawTextPosition
GetPosFromLine(text, line)
char *text;
long line;
{
  long i;
  XawTextPosition pos;

  for(pos=0, i=0; (i < line) && (*text != 0); pos++, text++)
    if(*text == '\n') i++;

  return pos;
}

#define SetCursor(widget, cursor) \
 XDefineCursor(CurDpy, XtWindow((the_Question->window)->widget), cursor)

#define unSetCursor(widget) \
 XUndefineCursor(CurDpy, XtWindow((the_Question->window)->widget))

static void
SetCursors(cursor)
Cursor cursor;
{
  static Cursor xterm_cursor = NULL;

  if(xterm_cursor == NULL)
    xterm_cursor = XCreateFontCursor(CurDpy, XC_xterm);

  if(cursor != NULL) {
    SetCursor(keywordwid, cursor);
    SetCursor(shell, cursor);
    SetCursor(Sources->ListWidget, cursor);
    SetCursor(RelevantDocuments->ListWidget, cursor);
    SetCursor(ResultDocuments->ListWidget, cursor);
    SetCursor(StatusWindow, cursor);
  }
  else {
    SetCursor(keywordwid, xterm_cursor);
    unSetCursor(shell);
    unSetCursor(Sources->ListWidget);
    unSetCursor(RelevantDocuments->ListWidget);
    unSetCursor(ResultDocuments->ListWidget);
    unSetCursor(StatusWindow);
  }
}

static void
world(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  static int cursor = 0;

  SetCursors(wais_cursors[cursor]);
  cursor = (cursor+1)%NUM_CURSORS;
  world_timer = XtAddTimeOut(WORLD_INTERVAL, world, (Opaque) world);
}

#ifdef MOTIF
static void
highlight_words(w)
Widget w;
{
  long b, e, offset;
  char* text = GetString(w);

  e = strlen(text);

  for (offset = 0, e = strlen(text);
       (b = findKeyword(text, &e)) < 999999;
       offset+=e, text+=e, e = strlen(text)) {
    XmTextSetHighlight(w, b+offset, b+offset+e, XmHIGHLIGHT_SELECTED);
  }
}
#endif

static void
cleanup()
{
  Arg args[TWO];
#ifdef MOTIF
  XmString strn = XmStringCreateSimple(" View  ");
#else
  char* strn = " View  ";
#endif

  busy = FALSE;
  fuzzButtons(false);
  XtSetArg(args[ZERO], NLABEL, strn);
  XtSetValues(viewbutton, args, ONE);
#ifdef MOTIF
  highlight_words(the_Question->window->keywordwid, GetKeywordsUsed());
#else
  XtSetSensitive(viewbutton, False);
  XtSetSensitive(saveAsButton, False);
  XtCallActionProc(searchButton, "unset", NULL, NULL, 0);
  XawTextDisplay(the_Question->window->keywordwid);
#endif

  SetCursors(NULL);
  if(world_timer != NULL) {
    XtRemoveTimeOut(world_timer);
    world_timer = NULL;
  }
}

static void
truncate_document_list(dl, n)
DocList dl;
long n;
{
  while(--n > 0)
    if(dl == NULL ||
       (dl = dl->nextDoc) == NULL)
      return;

  freeDocList(dl->nextDoc);
  dl->nextDoc = NULL;
  return;
}

static void
finish_search(sb)
Sbuff sb;
{ /* Done with message, let's interpret it */
  Question q = the_Question->q;
  QuestionWindow qw = the_Question->window;
  char message[255];
  diagnosticRecord **diag;
  WAISSearchResponse *response;

  if(sb != NULL &&
     sb->buffer != NULL) {
    readSearchResponseAPDU(&q->query_response,
			   sb->buffer + HEADER_LENGTH);
    if (q->query_response != NULL)
      if ((response = (WAISSearchResponse *)
	   q->query_response->DatabaseDiagnosticRecords) != NULL) {
	if ((diag = response->Diagnostics)
	    != NULL)
	  showDiags(diag);
	if (q->ResultDocuments != NULL) {
	  char* keys_used = NULL;
	  DocList last = findLast(q->ResultDocuments);

	  last->nextDoc = build_response_list(q->query_response,
					      current_sourcelist->thisSource,
					      &keys_used);
	  if (keys_used != NULL) {
	    if (keys_used[0] != 0) {
	      if(q->keywords_used == NULL)
		q->keywords_used = keys_used;
	      else {
		if(strcmp(q->keywords_used, keys_used) != 0) {
		  q->keywords_used =
		    (char*)s_realloc(q->keywords_used,
				     strlen(q->keywords_used) + strlen(keys_used) +1);
		  strcat(q->keywords_used, " ");
		  strcat(q->keywords_used, keys_used);
		}
	      }
	      s_free(keys_used);
	    }
	  }
	}
	else
	  q->ResultDocuments =
	    build_response_list(q->query_response,
				current_sourcelist->thisSource,
				&q->keywords_used);
	freeWAISSearchResponse(response);
      }
  }
  else q->ResultDocuments = last_resdocs;

  sort_document_list(q->ResultDocuments);

  truncate_document_list(q->ResultDocuments, q->maxresdocs);

  current_sourcelist = current_sourcelist->nextSource;

  if(current_sourcelist == NULL) {
    q->numresdocs =  listlength((List)q->ResultDocuments);
    the_Question->Result_Items = buildDocumentItemList(q->ResultDocuments, TRUE);
    RebuildListWidget(qw->ResultDocuments, the_Question->Result_Items, LIST_TOP);
    if(sb != NULL)
      PrintStatus(STATUS_INFO, STATUS_MEDIUM, FOUND_MESSAGE, q->numresdocs);
    cleanup();
  }
  else
    SetupSearch(q);
}

static void
finish_retrieval(sb)
Sbuff sb;
{
  WAISDocumentText *text;
  diagnosticRecord **diag;
  Question q = the_Question->q;
  char* viewtext;
  long size, numChars, start_byte, end_byte;
  WAISSearchResponse *response;

  if(sb != NULL) {
    numChars = (sb->page+1) * sb->chars_per_page;

    if (sb->textstruct->text == NULL)
      sb->textstruct->text = (char*)s_malloc(sb->chars_per_page);
    else
      sb->textstruct->text = s_realloc(sb->textstruct->text, numChars);

    if(sb->textstruct->text == NULL) {
      PrintStatus(STATUS_URGENT, STATUS_HIGH, BADALLOC_MESSAGE);
      return;
    }

    start_byte = sb->page * sb->chars_per_page;
    end_byte = (sb->doc_size < 0 ?
		(sb->page + 1) * sb->chars_per_page :
		MINIMUM((sb->page + 1) * sb->chars_per_page, sb->doc_size));

    readSearchResponseAPDU(&q->retrieval_response,
			   sb->buffer + HEADER_LENGTH);

    if (q->retrieval_response != NULL)  {
      if ((response = (WAISSearchResponse *)
	   q->retrieval_response->DatabaseDiagnosticRecords) != NULL) {
	diag = response->Diagnostics;

	if(NULL == response->Text) {
	  if (diag != NULL)
	    showDiags(diag);

	  PrintStatus(STATUS_INFO, STATUS_HIGH, DONE_MESSAGE);
	  HandleDoc(sb);
	  return;
	}
	else {
	  text = response->Text[0];
	  if((sb->textstruct->type == NULL) || (strcmp(sb->textstruct->type, "TEXT") == 0)) {
	    long length = text->DocumentText->size;;

	    delete_seeker_codes(text->DocumentText->bytes, &length);
	    text->DocumentText->size = length;
	    replacecontrolM(text->DocumentText->bytes, &length);
	    text->DocumentText->size = length;
	  }

	  if(text->DocumentText->size > (end_byte - start_byte))
	    text->DocumentText->size = (end_byte - start_byte);

	  size = sb->textstruct->size;
	  viewtext = sb->textstruct->text+size;
	  size+=text->DocumentText->size;
	  sb->textstruct->size = size;
	  if (size <= numChars) { /* got less than we asked for */
	    memcpy(viewtext, text->DocumentText->bytes, text->DocumentText->size);
	    PrintStatus(STATUS_CONT, STATUS_HIGH, RECEIVE_MESSAGE,
			size, sb->s->name);

	    if(diag &&
	       diag[0] &&
	       diag[0]->ADDINFO != NULL &&
	       !strcmp(diag[0]->DIAG, D_PresentRequestOutOfRange)) {
	      PrintStatus(STATUS_INFO, STATUS_HIGH, DONE_MESSAGE);
	      sb->textstruct->size = size;
	      HandleDoc(sb);
	      freeWAISSearchResponse(response);
	      return;
	    }
	    if((sb->textstruct->type != NULL) &&
	       (strcmp(sb->textstruct->type, "TEXT") != 0) &&
	       (text->DocumentText->size != sb->chars_per_page)) {
	      PrintStatus(STATUS_INFO, STATUS_HIGH, DONE_MESSAGE);
	      sb->textstruct->size = size;
	      HandleDoc(sb);
	      freeWAISSearchResponse(response);
	      return;
	    }
	  }
	  else {
	    PrintStatus(STATUS_URGENT, STATUS_HIGH, BUFFOVER_MESSAGE);
	    sb->textstruct->size = size;
	    HandleDoc(sb);
	    freeWAISSearchResponse(response);
	    return;
	  }
	}

	if (diag != NULL)
	  showDiags(diag);

	freeWAISSearchResponse(response);
      }
    }
    sb->textstruct->size = size;
    if (end_byte == sb->doc_size) {
      PrintStatus(STATUS_INFO, STATUS_HIGH, DONE_MESSAGE);
      HandleDoc(sb);
    }
    else {
      sb->page++;
      SetupRetrieve(sb->textstruct, sb->page, sb->doc_size);
    }
  }
  else HandleDoc(NULL);
}


static void
GetData(data, fid, id)
XtPointer data;
int *fid;
XtInputId *id;
{
  long nbytes;
  Sbuff sb = (Sbuff)data;
  char* b;
  WAISMessage header;

  switch(sb->read_state) {
  case IN_HEADER:
    b = sb->buffer+sb->buffer_offset;
    if((nbytes = read(*fid, b, (HEADER_LENGTH-sb->buffer_offset))) == -1) {
      /* error! unregister XtInput */
      if(sb->xid != NULL)
	XtRemoveInput(sb->xid);
      sb->xid = NULL;
      return;
    }
    sb->buffer_offset+=nbytes;
    if (sb->buffer_offset == HEADER_LENGTH) { /* Done with header, let's interpret it */
      char length_array[11];
      readWAISPacketHeader(sb->buffer, &header);
      strncpy(length_array, header.msg_len, 10);
      length_array[10] = '\0';
      sb->read_state = IN_BODY;
      sb->toRead = atol(length_array);
      sb->buffer_offset = 0;
    }
    break;
  case IN_BODY:
    b = sb->buffer+sb->buffer_offset+HEADER_LENGTH;
    if((nbytes = read(*fid, b, (sb->toRead - sb->buffer_offset))) == -1) {
      /* error! unregister XtInput */
      if(sb->xid != NULL) {
	XtRemoveInput(sb->xid);
	sb->xid = NULL;
      }
      return;
    }
    sb->buffer_offset+=nbytes;
    if (sb->buffer_offset == sb->toRead) {
      sb->read_state = IN_HEADER;
      sb->toRead = HEADER_LENGTH;
      sb->buffer_offset = 0;
      if(sb->xid != NULL) {
	XtRemoveInput(sb->xid);
	sb->xid = NULL;
      }
      if(searching) finish_search(sb);
      else finish_retrieval(sb);
    }
  }
  return;
}

static boolean
  SendWaisMessage(request_message, request_length, connection)
char* request_message;
long request_length;
FILE* connection;
{
  writeWAISPacketHeader(request_message,
			request_length,
			(long)'z', /* Z39.50 */
			"wais      ", /* server name */
			(long)NO_COMPRESSION, /* no compression */
			(long)NO_ENCODING,(long)HEADER_VERSION);

  if( request_length + HEADER_LENGTH
     != fwrite (request_message, 1L, request_length + HEADER_LENGTH, connection))
    return false;

  fflush(connection);
  return true;
}

static void
 makeSBuff(source, page)
Source source;
long page;
{
  if(sb == NULL) sb = (Sbuff) s_malloc(sizeof(_Sbuff));

  sb->s = source;
  if(page == 0) {
    if (sb->buffer != NULL) s_free(sb->buffer);
    sb->buffer = s_malloc(source->buffer_length);
  }
  sb->page = page;
  sb->read_state = IN_HEADER;
  sb->toRead = HEADER_LENGTH;
  sb->buffer_offset = 0;
  if(source->connection != NULL)
    sb->xid = XtAddInput(fileno(source->connection),
			 (XtPointer)XtInputReadMask,
			 (XtInputCallbackProc)GetData, (XtPointer)sb);
  else sb->xid = NULL;
}

static void SetupSearch(q)
Question q;
{
  Source source;
  long request_buffer_length, numdocs;
  DocList dl;
  DocObj **Doc;
  char message[255];

  if(current_sourcelist != NULL) {
    if (last_resdocs) {
      freeDocList(last_resdocs);
      last_resdocs = NULL;
    } else {
      if(q->ResultDocuments != NULL)
        last_resdocs = q->ResultDocuments;
      else last_resdocs = resdocs;
    }
    /* build DocObjs */
    Doc = (DocObj**)s_malloc((q->numdocs+1) * sizeof(char*));

    for(numdocs=0, dl = q->RelevantDocuments;
	dl != NULL;
	dl = dl->nextDoc, numdocs++) {
      if(dl->thisDoc->doc != NULL) {
	char* tmptype = s_strdup((dl->thisDoc->doc->type) ?
				 dl->thisDoc->doc->type[0] : "TEXT");
	if(dl->thisDoc->doc->id != NULL)
	  if(dl->thisDoc->start >= 0)
	    Doc[numdocs] =
	      makeDocObjUsingLines(anyFromDocID(dl->thisDoc->doc->id),
				   tmptype, dl->thisDoc->start, dl->thisDoc->end);
	  else
	    Doc[numdocs] =
	      makeDocObjUsingWholeDocument(anyFromDocID(dl->thisDoc->doc->id),
					   tmptype);

      }
    }
    Doc[numdocs] = NULL;

    if(current_sourcelist == NULL ||
       current_sourcelist->thisSource == NULL) {
      current_sourcelist == NULL;
    }
    else {
      if((source =
	  findsource(current_sourcelist->thisSource->filename,
		     the_Question->q->sourcepath))
	  == NULL) {
	PrintStatus(STATUS_URGENT, STATUS_HIGH, NOSOURCE_MESSAGE,
		    current_sourcelist->thisSource->filename);
	finish_search(NULL);
      }
      else {
	PrintStatus(STATUS_INFO, STATUS_HIGH, "\n");

	if(source->initp != TRUE) {
	  PrintStatus(STATUS_INFO, STATUS_HIGH, INITSOURCE_MESSAGE);
	  init_for_source(source, q->request_message, MAX_MESSAGE_LEN,
			  q->response_message);
	}
	if(source->initp != FALSE) {
	  PrintStatus(STATUS_INFO, STATUS_HIGH, SEARCH_MESSAGE, source->name);
	  request_buffer_length = source->buffer_length;
	  if(NULL ==
	     generate_search_apdu(q->request_message + HEADER_LENGTH,
				  &request_buffer_length,
				  q->keywords,
				  (source->database[0]?source->database:NULL),
				  Doc, q->maxresdocs)) {
	    PrintStatus(STATUS_URGENT, STATUS_HIGH, BUFFOVER_MESSAGE);
	  }

	  makeSBuff(source, 0);
	  if(source->connection != NULL) {
	    if(SendWaisMessage(q->request_message,
			       (source->buffer_length -
				request_buffer_length),
			       source->connection)) {
	    }
	    else {
	      PrintStatus(STATUS_URGENT, STATUS_HIGH, BADWAIS_MESSAGE);
	    }
	  }
	  else {
	    if(interpret_message(q->request_message,
				 (source->buffer_length -
				  request_buffer_length),
				 sb->buffer,
				 source->buffer_length,
				 source->connection,
				 false
				 ) == 0) {
	      PrintStatus(STATUS_URGENT, STATUS_HIGH, BADCONNECT_MESSAGE);
	      close_source(source);
	    }
	    else {
	      finish_search(sb);
	    }
	  }
	}
	else {
	  PrintStatus(STATUS_INFO, STATUS_HIGH, NOSEARCH_MESSAGE, source->name);
	  if (sb != NULL && sb->buffer != NULL) {
	    s_free(sb->buffer);
	    sb->buffer = NULL;
	  }
	  finish_search(sb);
	}
      }
    }
    if (Doc != NULL) {
      doList((void**)Doc,freeDocObj);
      s_free(Doc);
    }
  }
}

static void SetupRetrieve(textstruct, page, size)
Textbuff textstruct;
long page;
long size;
{
  long request_length;
  any* docany;
  Source source = NULL;
  DocumentID doc = textstruct->docid;
  char* type = textstruct->type;

  if(doc != NULL &&
     doc->doc != NULL &&
     doc->doc->sourceID != NULL &&
     doc->doc->sourceID->filename != NULL &&
     (source = findsource(doc->doc->sourceID->filename,
			  the_Question->q->sourcepath))
     != NULL) {

    if(page == 0)
      PrintStatus(STATUS_INFO, STATUS_HIGH, "\n");

    if(source->initp == FALSE) {
      PrintStatus(STATUS_INFO, STATUS_HIGH, INITSOURCE_MESSAGE);
      init_for_source(source, the_Question->q->request_message, MAX_MESSAGE_LEN,
		      the_Question->q->response_message);

    }
    if(source->initp == FALSE) {
      PrintStatus(STATUS_INFO, STATUS_HIGH, NOGETDOC_MESSAGE,
		  textstruct->docid->doc->headline, source->name);
      finish_retrieval(NULL);
    }
    else {
/*      prep_retrieval(); */
      makeSBuff(source, page);
      if(page == 0) {
	PrintStatus(STATUS_INFO, STATUS_HIGH, GETDOC_MESSAGE,
		    textstruct->docid->doc->headline, source->name);
	sb->chars_per_page = source->buffer_length - HEADER_LENGTH - 1000; /* ? */
	sb->textstruct = textstruct;
	if(size == 0) sb->doc_size = textstruct->docid->doc->numChars;
	else sb->doc_size = size;
      }
      request_length = source->buffer_length;
      docany = anyFromDocID(doc->doc->id);

      if(NULL ==
	 generate_retrieval_apdu(the_Question->q->request_message + HEADER_LENGTH,
				 &request_length, docany, CT_byte,
				 page * sb->chars_per_page,
				 (sb->doc_size < 0 ?
				  (page + 1) * sb->chars_per_page :
				  MINIMUM((page + 1) * sb->chars_per_page, sb->doc_size)),
				 s_strdup((type ? type : "TEXT")),
				 (source->database[0] ?
				  source->database : NULL))) {
	PrintStatus(STATUS_URGENT, STATUS_HIGH, BUFFOVER_MESSAGE);
      }
      else {
	if(source->connection != NULL) {
	  if(SendWaisMessage(the_Question->q->request_message,
			     (source->buffer_length -
			      request_length),
			     source->connection)) {
	  }
	  else {
	    PrintStatus(STATUS_URGENT, STATUS_HIGH, BADWAIS_MESSAGE);
	  }
	}
	else {
	  if (interpret_message(the_Question->q->request_message,
				(source->buffer_length -
				 request_length),
				sb->buffer,
				source->buffer_length,
				source->connection,
				false)
	      == 0) {
	    PrintStatus(STATUS_URGENT, STATUS_HIGH, BADCONNECT_MESSAGE);
	    close_source(source);
	  }
	  else {
	    finish_retrieval(sb);
	  }
	}
      }
      freeAny(docany);
    }
  }
  else PrintStatus(STATUS_URGENT, STATUS_HIGH, NOSOURCE_MESSAGE,
		   doc->doc->sourceID->filename);
}

static void
SetPosition(w, shell, centerp)
Widget w;
Widget shell;
boolean centerp;
{
  Arg		args[2];
  Position	x, y;
  Dimension	width, height;

  XtSetArg(args[ZERO], XtNwidth, &width);
  XtSetArg(args[ONE], XtNheight, &height);
  XtGetValues(w, args, TWO);
  XtTranslateCoords(w,
		    (Position)(centerp?width/2:0),
		    (Position)(centerp?height/2:height),
		    &x, &y);

  XtSetArg(args[ZERO], XtNx, x);
  XtSetArg(args[ONE], XtNy, y);

  XtSetValues(shell, args, TWO);
}

static char *
findFilter(type)
char *type;
{
  char *p, *i1, *i2, t[MAX_FILE_NAME_LEN+1];
  static char result[MAX_FILE_NAME_LEN+1];

  /* filters are of the form TYPE,FILTER;... */

  memset(result, 0, MAX_FILE_NAME_LEN+1);
  p = app_resources.filters;

  while (*p != 0) {
    if((i1 = (char*)strchr(p, ',')) == NULL) break;
    strncpy(t, p, MINIMUM(i1-p, MAX_FILE_NAME_LEN));
    if(!strncmp(t, type, MINIMUM(i1-p, MAX_FILE_NAME_LEN))) {
      if((i2 = (char*)strchr(i1, ';')) != NULL) {
	strncpy(result, i1+1, MINIMUM(i2-i1-1, MAX_FILE_NAME_LEN));
	result[i2-i1] = 0;
      }
      else strcpy(result, i1+1);
      return result;
    }
    if((p = (char*)strchr(i1, ';')) == NULL) break;
    p++;
  }
  return NULL;
}

static Boolean
tryFilter(t, type, filename)
Textbuff t;
char *type;
char *filename;
{
  char fname[STRINGSIZE], command[STRINGSIZE], *text, *viewer;
  FILE *fp;
  long i;

  if((viewer = findFilter(type)) == NULL) return FALSE;

  sprintf(fname, "%s%s",
	  app_resources.documentDirectory,
	  get_filename(t->docid->doc->headline));

  if((fp = fopen(fname, "w")) == NULL) {
    PrintStatus(STATUS_URGENT, STATUS_HIGH, BADFOPEN_MESSAGE, fname);
    return;
  }

  dumptext(fp, t->text, t->size);
  fclose(fp);

  KillText(t);

  PrintStatus(STATUS_INFO, STATUS_HIGH, FILTER_MESSAGE,
	      viewer, fname);
  sprintf(command, "csh -fc '%s %s;/bin/rm %s' &", viewer, fname, fname);
  system(command);

  return TRUE;
}

static void
DoSource(t)
Textbuff t;
{
  char f[STRINGSIZE], message[STRINGSIZE];
  FILE *fp;

  sprintf(f, "/tmp/src%d", getpid());
  if((fp = fopen(f, "w")) == NULL) {
    PrintStatus(STATUS_URGENT, STATUS_HIGH, BADFOPEN_MESSAGE, f);
  }
  else {
    fprintf(fp, t->text);

    fclose(fp);
    if((fp = fopen(f, "r")) == NULL) {
      PrintStatus(STATUS_URGENT, STATUS_HIGH, BADFOPEN_MESSAGE, f);
    }
    else {
      memset(the_Source, 0, sizeof(_Source));

      ReadSource(the_Source, fp);
      fclose(fp);

      if (the_Source->name != NULL) s_free(the_Source->name);
      the_Source->name = s_strdup(get_filename(t->docid->doc->headline));

      unlink(f);

      KillText(t);
      PopupSource(the_Source);
    }
  }
}

static void
do_other_thing(t, type)
Textbuff t;
char *type;
{
  char message[STRINGSIZE];

  if (type != NULL && type[0] != 0) {
    PrintStatus(STATUS_URGENT, STATUS_HIGH, UNKNOWNTYPE_MESSAGE, type);
  }

  if(savelist == NULL)
    savelist = MakeSaveRequester(top);

  SetPosition(the_Question->window->shell, savereq, true);

  current_text = t;

  ReplaceText(filenamewidget, "");
  ReplaceText(dirnamewidget, app_resources.documentDirectory);

  XtPopup(savereq, XtGrabNone);
  SetDir(NULL, NULL, NULL);
  SetReqButtons(false);

  XtRemoveAllCallbacks(savebutton, COMMANDCALLBACK);
  XtAddCallback(savebutton, COMMANDCALLBACK, DoTSaveCB, t);
}

static boolean
GetDoc(textstruct, size)
Textbuff textstruct;
long size;
{
  busy = TRUE;
  searching = FALSE;
  fuzzButtons(true);

  world_timer = XtAddTimeOut(WORLD_INTERVAL, world, (Opaque) world);
  SetupRetrieve(textstruct, 0, size);
}

static void
HandleDoc(sb)
Sbuff sb;
{
#ifndef MOTIF
  XtCallActionProc(viewbutton, "unset", NULL, NULL, 0);
#endif
  if(world_timer != NULL) {
    XtRemoveTimeOut(world_timer);
    world_timer = NULL;
  }
  fuzzButtons(false);
  SetCursors(NULL);
  busy = false;

  if(sb != NULL) {
    if(sb->textstruct->size == 0) {
      KillText(sb->textstruct);
      PrintStatus(STATUS_URGENT, STATUS_HIGH, NODATA_MESSAGE);
      return;
    }

    if(savep)
      do_other_thing(sb->textstruct, NULL);
    else if(tryFilter(sb->textstruct, sb->textstruct->type))
      return;

    else if (substrcmp(sb->textstruct->docid->doc->headline, 
		       "Catalog for database:") ||
	     substrcmp(sb->textstruct->docid->doc->headline, ">") || 
	     strcmp(sb->textstruct->type,"WCAT") == 0 ) {
      Catbuff cat = build_cat(sb->textstruct->text,
			      sb->textstruct->docid->doc->sourceID);

      if(cat != NULL) {
	sb->textstruct->textwindow =
	  MakeCatPopup(top, cat, sb->textstruct->docid->doc->headline);
	XtPopup(sb->textstruct->textwindow, XtGrabNone);
      }
    }
    else if (sb->textstruct->type == NULL ||
	     substrcmp(sb->textstruct->type, "TEXT") ||
	     !strcmp(sb->textstruct->type, "WCAT")) {
      Arg args[2];

      sb->textstruct->textwindow =
	MakeTextPopup(top, sb->textstruct, sb->textstruct->docid->doc->headline);

      XtSetArg(args[ZERO], XtNtype, XawAsciiString);
      XtSetArg(args[ONE], XtNstring, sb->textstruct->text);
      XtSetValues(sb->textstruct->textwindow, args, TWO);

      if (sb->textstruct->docid->doc->best > 0) {
	XawTextPosition pos = GetPosFromLine(sb->textstruct->text,
					     sb->textstruct->docid->doc->best);
	XawTextSetInsertionPoint(sb->textstruct->textwindow, pos);
	XtSetArg(args[ZERO], XtNdisplayPosition, pos);
	XtSetValues(sb->textstruct->textwindow, args, ONE);
      }
    }
    else if (!strcmp(sb->textstruct->type, "WSRC")) {
      DoSource(sb->textstruct);
    }
    else do_other_thing(sb->textstruct, sb->textstruct->type);
  }
}

static void PopupTypeMenu(w, doc)
Widget w;
DocumentID doc;
{
  char** types = doc->doc->type;
  int i;

  SetPosition(w, typeshell, false);

  for(i = 0; types[i] != NULL; i++)
    Type_items[i] = types[i];

  RebuildListWidget(typewindow, Type_items, LIST_TOP);

  XtPopup(typeshell, XtGrabExclusive);
}

XawTextPosition
findstring(text, string, casesensitive)
char *text, *string;
Boolean casesensitive;
{
  char *t, *t2, *t3;

  if (casesensitive) {
    for (t = text; *t != 0; t++) {
      if (*t == *string) {
	t2 = t;
	t3 = string;
	do {
	  t2++;
	  t3++;
	  if(*t3 == 0) return((XawTextPosition)(t-text));
	}
	while(*t2 == *t3);
      }
    }
    return -1;
  }
  else {
    for (t = text; *t != 0; t++) {
      if (tolower(*t) == tolower(*string)) {
	t2 = t;
	t3 = string;
	do {
	  t2++;
	  t3++;
	  if(*t3 == 0) return((XawTextPosition)(t-text));
	}
	while(tolower(*t2) == tolower(*t3));
      }
    }
    return -1;
  }
}

/* ARGSUSED */
void
showKeyword(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  Textbuff t;
  static char msg[STRINGSIZE], str[STRINGSIZE], minstr[STRINGSIZE], *keys;
  Widget tw;
  XawTextPosition cpos, minpos;

  if((t = findText(w)) == NULL) {
    XwaisPrintf("couldn't find text.\n");
    return;
  }

  tw = t->textwindow;

  messwidget = t->status;
  PrintStatus(STATUS_CONT, STATUS_MEDIUM, "\nSearching for next keyword...");
  PrintStatusW(msg, t->status,false);

  cpos = XawTextGetInsertionPoint(tw);
  minpos = findKeyword(t->text+cpos, NULL);
  if (minpos == 999999) {
    PrintStatus(STATUS_INFO, STATUS_MEDIUM, "\nCould not find any more keywords.");
  }
  else {
    Arg args[ONE];

    minpos += cpos+1;
    XawTextSetInsertionPoint(tw, minpos + 1);
    /*XtSetArg(args[ZERO], XtNdisplayPosition, minpos);
    XtSetValues(tw, args, ONE);*/
    PrintStatus(STATUS_CONT, STATUS_MEDIUM, "\nSearching for next keyword...done");
  }
  messwidget = the_Question->window->StatusWindow;
}

static void
DoTSaveCB(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  FILE *fp;
  char message[STRINGSIZE], filename[STRINGSIZE];

  Textbuff t = (Textbuff) closure;

  strncpy(filename, GetString(filenamewidget), STRINGSIZE);

  if(filename[0]==0) {
    PrintStatus(STATUS_INFO, STATUS_HIGH, ENTERFNAME_MESSAGE);
  }
  else {
    XtPopdown(savereq);
    SetReqButtons(false);

    if (filename[0] != '/') {
      if(t->type != NULL &&
	 !strcmp(t->type, "WSRC"))
	sprintf(message, "%s%s", app_resources.userSourceDirectory, filename);
      else
	sprintf(message, "%s%s", GetString(dirnamewidget), filename);
      strcpy(filename, message);
    }

    if((fp = fopen(filename, "w")) == NULL) {
      PrintStatus(STATUS_URGENT, STATUS_HIGH, BADFOPEN_MESSAGE, filename);
    }
    else {
      dumptext(fp, t->text, t->size);
      fclose(fp);
    }
    if(t->type != NULL && strcmp(t->type, "TEXT") != 0)
      KillText(t);
  }
}

/* Public Functions */

void
fuzzButtons(fuzz)
boolean fuzz;
{
  if(fuzz) {
    s_dsb = XtIsSensitive(delSourceButton);
    s_adb = XtIsSensitive(addDocButton);
    s_ddb = XtIsSensitive(delDocButton);
  }

#ifndef MOTIF
  if(searching) {
    XtSetSensitive(viewbutton, !fuzz);
    XtCallActionProc(searchButton, (fuzz?"set":"unset"), NULL, NULL, 0);
  }
  else {
    XtSetSensitive(viewbutton, !fuzz);
    XtCallActionProc(viewbutton, (fuzz?"set":"unset"), NULL, NULL, 0);
  }
#endif

  XtSetSensitive(viewbutton, !fuzz);
  XtSetSensitive(saveAsButton, !fuzz);
  XtSetSensitive(addSourceButton, !fuzz);
  XtSetSensitive(delSourceButton, (fuzz?False:s_dsb));
  XtSetSensitive(addDocButton, (fuzz?False:s_adb));
  XtSetSensitive(delDocButton, (fuzz?False:s_ddb));

  XtSetSensitive(abortButton, fuzz);
  XtSetSensitive(helpButton, !fuzz);
  XtSetSensitive(doneButton, !fuzz);
}

void
Prefs(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  Arg arglist[TWO];
  char maxdocstring[STRINGSIZE];

  if(prefpopup == NULL)
    prefpopup = MakePrefPopup(top);

  XtPopup(prefpopup, XtGrabNone);
  XtSetArg(arglist[ZERO], XtNtitle, "Xwais Preferences");
  XtSetArg(arglist[ONE], XtNiconName, "Xwais Preferences");
  XtSetValues(prefpopup, arglist, TWO);
  sprintf(maxdocstring, "%d", the_Question->q->maxresdocs);
  ReplaceText(maxdocwid, maxdocstring);
  ReplaceText(sourcepathwid, the_Question->q->sourcepath);
  ReplaceText(filterwid, app_resources.filters);
}

void
DoPSave(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  the_Question->q->maxresdocs = atoi(GetString(maxdocwid));
  if(the_Question->q->sourcepath != NULL) s_free(the_Question->q->sourcepath);
  the_Question->q->sourcepath = s_strdup(GetString(sourcepathwid));
  if(app_resources.filters != NULL) s_free(app_resources.filters);
  app_resources.filters = s_strdup(GetString(filterwid));
  XtPopdown(prefpopup);
}

void
DontPSave(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  XtPopdown(prefpopup);
}

void
Abort(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  if(busy) {
    fuzzButtons(false);
    busy = FALSE;
    SetCursors(NULL);

    if (sb != NULL && sb->xid != NULL) {
      XtRemoveInput(sb->xid);
      sb->xid = NULL;
    }
    if(world_timer != NULL) {
      XtRemoveTimeOut(world_timer);
      world_timer = NULL;
    }

    PrintStatus(STATUS_URGENT, STATUS_HIGH, ABORT_MESSAGE);
    if(searching) {
      if(the_Question->q->ResultDocuments != NULL)
	freeDocList(the_Question->q->ResultDocuments);
      the_Question->q->ResultDocuments = resdocs;
      the_Question->q->numresdocs =
	listlength((List)the_Question->q->ResultDocuments);
      the_Question->Result_Items =
	buildDocumentItemList(the_Question->q->ResultDocuments, TRUE);
      RebuildListWidget(the_Question->window->ResultDocuments,
			the_Question->Result_Items, LIST_TOP);
#ifndef MOTIF
      XtCallActionProc(searchButton, "unset", NULL, NULL, 0);
#endif
    }
    else {
      freeDocList(resdocs);
#ifndef MOTIF
      XtCallActionProc(viewbutton, "unset", NULL, NULL, 0);
#endif
      if(sb != NULL && sb->textstruct != NULL) {
	KillText(sb->textstruct);
	sb->textstruct = NULL;
      }
      sb->page = 0; sb->chars_per_page = 0;
    }
    /* close out the connection to clear buffers */

    if(the_Question->q->Sources != NULL) {
      SourceList slist;
      Source source;
      for(slist = the_Question->q->Sources;
	  slist != NULL;
	  slist = slist->nextSource) {
	if((source = findsource(slist->thisSource->filename,
				the_Question->q->sourcepath)) != NULL)
	  close_source(source);
      }
    }
  }
}

/* these are the commands used in the question widget */


/* ARGSUSED */
void
DoSearch(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  double_click = FALSE;
  LastClicked = w;
  last_doc = -1;

  if(!busy) {
    /* update the info */

    strncpy(the_Question->q->keywords,
	    GetString(the_Question->window->keywordwid), STRINGSIZE);

    busy = TRUE;
    searching = TRUE;

    fuzzButtons(true);
    if((current_sourcelist = the_Question->q->Sources) == NULL) {
      SourceID sid;

      sid = (SourceID)s_malloc(sizeof(_SourceID));
      sid->filename = s_strdup("directory-of-servers.src");
      the_Question->q->Sources = makeSourceList(sid, NULL);
      current_sourcelist = the_Question->q->Sources;
      the_Question->Source_Items =
	buildSourceItemList(the_Question->q->Sources);
      the_Question->q->numsources =
	charlistlength(the_Question->Source_Items);

      RebuildListWidget(the_Question->window->Sources,
			the_Question->Source_Items, LIST_BOTTOM);
    }
    world_timer = XtAddTimeOut(WORLD_INTERVAL, world, (Opaque) world);
    resdocs = the_Question->q->ResultDocuments;
    the_Question->q->ResultDocuments = NULL;
    SetupSearch(the_Question->q);
  }
}

/* ARGSUSED */
void
DoSave(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
#ifndef MOTIF
  XtCallActionProc(savebutton, "set", NULL, NULL, 0);
  XtCallActionProc(savebutton, "notify", NULL, NULL, 0);
  XtCallActionProc(savebutton, "unset", NULL, NULL, 0);
#endif
}

/* ARGSUSED */
static void
DoSaveQ(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  FILE *fp;
  char message[STRINGSIZE], filename[STRINGSIZE];

  XtPopdown(savereq);
  SetReqButtons(false);

  strncpy(the_Question->q->name, GetString(filenamewidget), STRINGSIZE);
  sprintf(filename, "%s/%s",
	  GetString(dirnamewidget), the_Question->q->name);

  WriteQuestion(filename, the_Question->q);
  exit(0);
}

/* ARGSUSED */
static void
DontSave(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  exit(0);
}

static void
SaveQuestion()
{
  Arg args[ONE];
  if(savelist == NULL)
    savelist = MakeSaveRequester(top);

  SetPosition(the_Question->window->shell, savereq, true);

  XtPopup(savereq, XtGrabNone);
  ReplaceText(dirnamewidget, app_resources.questionDirectory);
  ReplaceText(filenamewidget, the_Question->q->name);
  SetDir(NULL, NULL, NULL);

  SetReqButtons(true);

  XtAddCallback(savebutton, COMMANDCALLBACK, DoSaveQ, NULL);
  XtAddCallback(quitbutton, COMMANDCALLBACK, DontSave, NULL);

}

/* ARGSUSED */
void CloseQuestionEdit(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  char filename[STRINGSIZE];

  strncpy(the_Question->q->keywords,
	  GetString(the_Question->window->keywordwid),
	  STRINGSIZE);

  if(strcmp(the_Question->q->name, "New Question") == 0) {
    SaveQuestion(NULL, NULL, NULL);
  }
  else {
    sprintf(filename, "%s%s", app_resources.questionDirectory,
	    the_Question->q->name);
    WriteQuestion(filename, the_Question->q);
    exit(0);
  }
}

/* ARGSUSED */
void
AddResponseToQuestion(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  long document_number;
  DocList this, last;

  double_click = FALSE;
  LastClicked = w;

  document_number = get_question_response(the_Question->window);

  if(document_number != NO_ITEM_SELECTED) {
    XtSetSensitive(delDocButton, False);
    last_qdoc = NO_ITEM_SELECTED;
    /* find and add document to question's relevant documents */
    last = findLast(the_Question->q->RelevantDocuments);

    for(this = the_Question->q->ResultDocuments;document_number--;)
      this = this->nextDoc;

    if(last != NULL)
      last->nextDoc = makeDocList(copy_docid(this->thisDoc), NULL);
    else the_Question->q->RelevantDocuments = makeDocList(copy_docid(this->thisDoc), NULL);

    if(the_Question->Relevant_Items != NULL)
      freeItemList(the_Question->Relevant_Items);

    the_Question->Relevant_Items =
      buildDocumentItemList(the_Question->q->RelevantDocuments, FALSE);

    the_Question->q->numdocs = charlistlength(the_Question->Relevant_Items);

    RebuildListWidget(the_Question->window->RelevantDocuments,
		      the_Question->Relevant_Items, LIST_BOTTOM);

    the_Question->q->modified = TRUE;
  }
}

/* ARGSUSED */
void
DeleteQuestionDoc(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  int SelectedDoc;
  DocList doc, last;

  if((SelectedDoc = get_selected_qdoc(the_Question)) == NO_ITEM_SELECTED) {
    double_click = FALSE;
    LastClicked = w;
    PrintStatus(STATUS_URGENT, STATUS_HIGH, NOSELECT_MESSAGE);
  }
  else {
    /* rip out the bugger */

    XtSetSensitive(delDocButton, False);
    last_qdoc = NO_ITEM_SELECTED;

    the_Question->q->modified = TRUE;

    double_click = FALSE;
    LastClicked = NULL;
    if (SelectedDoc == 0) {
      last = the_Question->q->RelevantDocuments;
      the_Question->q->RelevantDocuments = the_Question->q->RelevantDocuments->nextDoc;
      last->nextDoc = NULL;
      freeDocList(last);
    }
    else {
      for (doc = the_Question->q->RelevantDocuments;--SelectedDoc;) {
	doc = doc->nextDoc;
      }
      if(doc->nextDoc != NULL) {
	last = doc->nextDoc;
	doc->nextDoc = doc->nextDoc->nextDoc;
	last->nextDoc = NULL;
	freeDocList(last);
      }
    }
    if(the_Question->Relevant_Items != NULL) freeItemList(the_Question->Relevant_Items);
    the_Question->Relevant_Items = buildDocumentItemList(the_Question->q->RelevantDocuments, FALSE);

    the_Question->q->numdocs--;
    RebuildListWidget(the_Question->window->RelevantDocuments,
		      the_Question->Relevant_Items, LIST_NONE);
  }
}

/* ARGSUSED */
void
PopupSourceMenu(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  SetPosition(w, sshell, false);
#ifndef MOTIF
  XawListUnhighlight(sourcewindow->ListWidget);
#endif
  XtPopup(sshell, XtGrabExclusive);
}

/* ARGSUSED */
void
AddSourceToQuestion(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  long snum = get_selected_source();

  if(snum == NO_ITEM_SELECTED) XtPopdown(sshell);
  else {
#ifdef MOTIF
    last_source = snum;
#endif
    if(last_source != snum) last_source = snum;
    else {
      SourceID sid;
      SourceList slist, source = NULL;
      char *name = Source_items[snum];

      XtSetSensitive(delSourceButton, SENSVAL);
      last_qsource = NO_ITEM_SELECTED;
      XtPopdown(sshell);
      /* append it to the current sourcelist */

      for(source = the_Question->q->Sources;
	  source != NULL && source->nextSource != NULL;
	  source = source->nextSource)

	if(strcmp(source->thisSource->filename, name) == 0)
	  break;

      if(source == NULL || source->nextSource == NULL) {
	sid = (SourceID)s_malloc(sizeof(_SourceID));
	sid->filename = s_strdup(name);
	slist = makeSourceList(sid, NULL);

	if (source != NULL) source->nextSource = slist;
	else the_Question->q->Sources = slist;

	the_Question->Source_Items =
	  buildSourceItemList(the_Question->q->Sources);
	the_Question->q->numsources =
	  charlistlength(the_Question->Source_Items);

	RebuildListWidget(the_Question->window->Sources,
			  the_Question->Source_Items, LIST_BOTTOM);
	the_Question->q->modified = TRUE;
      }
    }
  }
}

/* ARGSUSED */
void
EditQuestionSource(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  int CurrentSource;
  Source edit_source = NULL;

  double_click = FALSE;
  LastClicked = w;

  if ((CurrentSource = get_selected_qsource(the_Question))
      == NO_ITEM_SELECTED) {
    last_qsource = NO_ITEM_SELECTED;
  }
  else {
#ifdef MOTIF
    last_qsource = CurrentSource;
#endif
    if (last_qsource == CurrentSource) {
      if((edit_source =
	  findsource(the_Question->Source_Items[CurrentSource],
		     the_Question->q->sourcepath)) == NULL) {
	PrintStatus(STATUS_URGENT, STATUS_HIGH, NOSOURCE_MESSAGE,
		    the_Question->Source_Items[CurrentSource]);
      }
      else {
	PrintStatus(STATUS_INFO, STATUS_MEDIUM, VIEWSOURCE_MESSAGE,
		    edit_source->name);
	PopupSource(edit_source);
      }
    }
    else last_qsource = CurrentSource;
  }
}

/* ARGSUSED */
void
DeleteQuestionSource(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  int SelectedSource;
  SourceList source, last;

  if((SelectedSource = get_selected_qsource(the_Question)) == NO_ITEM_SELECTED) {
    double_click = FALSE;
    LastClicked = w;
    PrintStatus(STATUS_URGENT, STATUS_HIGH, NOSELECT_MESSAGE);
  }
  else {
    /* rip out the bugger */
    XtSetSensitive(delSourceButton, SENSVAL);
    last_qsource = NO_ITEM_SELECTED;
    the_Question->q->modified = TRUE;

    double_click = FALSE;
    LastClicked = NULL;
    if (SelectedSource == 0) {
      last = the_Question->q->Sources;
      the_Question->q->Sources = the_Question->q->Sources->nextSource;
      s_free(last);
    }
    else {
      for (source = the_Question->q->Sources; --SelectedSource;) {
	source = source->nextSource;
      }
      if(source->nextSource != NULL) {
	last = source->nextSource;
	source->nextSource = source->nextSource->nextSource;
	s_free(last);
      }
    }
    the_Question->Source_Items =
      buildSourceItemList(the_Question->q->Sources);

    the_Question->q->numsources--;
    RebuildListWidget(the_Question->window->Sources,
		      the_Question->Source_Items, LIST_NONE);
  }
}

/* ARGSUSED */
void doType(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  long tnum, dnum;
  DocumentID doc;

  XtPopdown(typeshell);
  if((tnum = get_selected_type()) != NO_ITEM_SELECTED) {
    dnum = get_selected_response(the_Question);
    if((doc = findDoc(the_Question->q->ResultDocuments, dnum)) != NULL) {
      messwidget = the_Question->window->StatusWindow;
      ViewDoc(doc, Type_items[tnum], -1, false);
    }
  }
}

void SensitizeDelSource(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  XawListReturnStruct *data = (XawListReturnStruct *)call_data;

  XtSetSensitive(delSourceButton,!!data->string);
}

void SensitizeAddDoc(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  XawListReturnStruct *data = (XawListReturnStruct *)call_data;
  XtSetSensitive(addDocButton, !!data->string);
  XtSetSensitive(viewbutton, !!data->string);
  XtSetSensitive(saveAsButton, !!data->string);
}

void SensitizeDelDoc(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  XawListReturnStruct *data = (XawListReturnStruct *)call_data;

  XtSetSensitive(delDocButton, !!data->string);
}

void
  ViewDoc(doc, type, size, saveit)
DocumentID doc;
char *type;
long size;
Boolean saveit;
{
  Textbuff textstruct;
  TextList thisText;

  savep = saveit;

  if((textstruct = findTextDoc(doc, type)) != NULL) {
    if(textstruct->shell != NULL) {
      XtPopdown(textstruct->shell);
      XtPopup(textstruct->shell, XtGrabNone);
    }
    return;
  }

  if(busy) return;

  if(NULL == (thisText = NewText())) {
    PrintStatus(STATUS_URGENT, STATUS_HIGH, BADALLOC_MESSAGE);
    return;
  }

  textstruct = thisText->thisText;
  textstruct->docid = doc;

  textstruct->size = 0;

  if(savep)
    textstruct->type = s_strdup("WaIsOddBall");
  else
    textstruct->type = s_strdup(type);

  GetDoc(textstruct, size);
}

/* ARGSUSED */
void
ViewResponse(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  static int document_number;

  double_click = FALSE;
  LastClicked = w;

  if(!busy)
    if((document_number = get_selected_response(the_Question)) ==
       NO_ITEM_SELECTED) {
      last_doc = document_number;
    }
    else {
      DocumentID doc;

      if((doc = findDoc(the_Question->q->ResultDocuments, document_number)) == NULL)
	PrintStatus(STATUS_URGENT, STATUS_HIGH, NODOC_MESSAGE);
      else {
#ifdef MOTIF
	last_doc = document_number;
#endif
	if (document_number != last_doc) {
	  Arg args[2];

	  XtSetArg(args[0], XtNsensitive, True);
	  if((w != viewbutton) && (doc->doc->type[1] != NULL)) {
	    XtSetArg(args[1], XtNlabel, "View...");
	  }
	  else {
	    XtSetArg(args[1], XtNlabel, " View  ");
	  }
	  XtSetValues(viewbutton, args, TWO);
	  XtSetValues(saveAsButton, args, ONE);
	  last_doc = document_number;
	}
	else if((w == viewbutton) && (doc->doc->type[1] != NULL)) {
	  PopupTypeMenu(w, doc);
	}
	else {
	  messwidget = the_Question->window->StatusWindow;
	  ViewDoc(doc, doc->doc->type[0], 0, (w == saveAsButton));
	}
      }
    }
}

/* ARGSUSED */
void
ViewRelevant(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  double_click = FALSE;
  LastClicked = w;

  if(!busy) {
    long document_number = get_selected_qdoc(the_Question);

    if(document_number == NO_ITEM_SELECTED) {
      last_qdoc = document_number;
    }
    else {
      DocumentID doc;
#ifdef MOTIF
      last_qdoc = document_number;
#endif
      if((doc = findDoc(the_Question->q->RelevantDocuments, document_number))
	 == NULL) {
	PrintStatus(STATUS_URGENT, STATUS_HIGH, NODOC_MESSAGE);
      }
      else if (document_number != last_qdoc) {
	last_qdoc = document_number;
      }
      else {
	messwidget = the_Question->window->StatusWindow;
	ViewDoc(doc, doc->doc->type[0], 0, false);
      }
    }
  }
}

/* ARGSUSED */
void
EndText(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  Textbuff t = findText(w);

  if(t != NULL) {
    XtPopdown(t->shell);
    KillText(t);
  }
}

/* ARGSUSED */
void
SaveText(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  Arg		args[5];
  Position	x, y;
  Dimension	width, height;
  Cardinal	n;
  Textbuff t;

  if((t = findText(w)) == NULL) {
    XwaisPrintf("couldn't find text.\n");
    return;
  }

  if(savelist == NULL)
    savelist = MakeSaveRequester(top);

  SetPosition(t->textwindow, savereq, true);

  current_text = t;

  ReplaceText(filenamewidget, "");
  ReplaceText(dirnamewidget, app_resources.documentDirectory);

  XtPopup(savereq, XtGrabNone);
  SetDir(NULL, NULL, NULL);
  SetReqButtons(false);

  XtRemoveAllCallbacks(savebutton, COMMANDCALLBACK);
  XtAddCallback(savebutton, COMMANDCALLBACK, DoTSaveCB, t);
}

/* ARGSUSED */
void
DoTSave(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  DoTSaveCB(w, (XtPointer)current_text, NULL);
}

/* ARGSUSED */
void
DontTSave(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  XtPopdown(savereq);
  SetReqButtons(false);
}

/* ARGSUSED */
void
addSection(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  int i;
  Question q = the_Question->q;
  QuestionWindow qw = the_Question->window;
  float top, shown;
  DocList dlist;
  DocumentID doc;
  XawTextPosition p1, p2;
  long l1, l2;
  Textbuff t;

  t = findText(w);

  XawTextGetSelectionPos(t->textwindow, &p1, &p2);

  if(p1 >= 0 && p2 > 0) {
    /* find the line positions */
    l1 = GetLineFromPos(t->text, p1);
    l2 = GetLineFromPos(t->text, p2);

    doc = copy_docid(t->docid);
    doc->start = l1;
    doc->end = l2;
    dlist = makeDocList(doc, NULL);
    /* append it to the current rellist */

    if(q->RelevantDocuments != NULL) {
      DocList doc;

      for(doc = q->RelevantDocuments; doc->nextDoc != NULL; doc = doc->nextDoc);
      doc->nextDoc = dlist;
    }
    else
      q->RelevantDocuments = dlist;

    if(the_Question->Relevant_Items != NULL)
      freeItemList(the_Question->Relevant_Items);
    the_Question->Relevant_Items =
      buildDocumentItemList(q->RelevantDocuments, FALSE);
    q->numdocs = charlistlength(the_Question->Relevant_Items);

    RebuildListWidget(qw->RelevantDocuments, the_Question->Relevant_Items, LIST_BOTTOM);
  }
}

/* ARGSUSED */
void
DoSSave(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  FILE *fp;
  char name[STRINGSIZE];
  Source source;
  SList current_sources;

  source = the_Source;

  XtPopdown(sourcepopup);

  strcpy(name, GetString(snamewid));

  if(!((strlen(name) > 4) &&
       strstr(name, ".src") &&
       (!strcmp(".src", strstr(name, ".src")))))
    strcat(name, ".src");

  if(source->name != NULL) s_free(source->name);
  source->name = s_strdup(name);

  if (source->maintainer != NULL) s_free(source->maintainer);
  source->maintainer = s_strdup(GetString(maintainerwid));

  if (source->description != NULL) s_free(source->description);
  source->description = s_strdup(GetString(descwid));

  strncpy(source->server, GetString(serverwid), STRINGSIZE);
  strncpy(source->service, GetString(servicewid), STRINGSIZE);
  strncpy(source->database, GetString(dbwid), STRINGSIZE);
  strncpy(source->cost, GetString(costwid), STRINGSIZE);
  strncpy(source->units, GetString(unitwid), STRINGSIZE);

  WriteSource(app_resources.userSourceDirectory, source, TRUE);

  NumSources = 0;

  GetSourceNames(app_resources.userSourceDirectory);
  if(app_resources.commonSourceDirectory[0] != 0)
    GetSourceNames(app_resources.commonSourceDirectory);

  RebuildListWidget(sourcewindow, Source_items, LIST_NONE);
}

/* ARGSUSED */
void
DontSSave(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  XtPopdown(sourcepopup);
}

/* ARGSUSED */
void showNext(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  Textbuff t;
  DocumentID d;
  Source source;
  Question q = the_Question->q;

  t = findText(w);
  if(t != NULL) {
    if(t->docid->doc != NULL &&
       t->docid->doc->sourceID != NULL &&
       t->docid->doc->sourceID->filename != NULL)
      source = findsource(t->docid->doc->sourceID->filename,
			  the_Question->q->sourcepath);
    messwidget = t->status;
    if (source == NULL) {
      PrintStatus(STATUS_URGENT, STATUS_HIGH, NOSOURCE_MESSAGE,
		  t->docid->doc->sourceID->filename);
      return;
    }

    if((d = getNextorPrevDoc(q, source, t->docid, TRUE)) != NULL)
      ViewDoc(d, d->doc->type[0], 0, false);
    else
      PrintStatus(STATUS_URGENT, STATUS_HIGH, BADNEXT_MESSAGE);
  }
}

/* ARGSUSED */
void showPrevious(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  Textbuff t;
  DocumentID d;
  Source source;

  t = findText(w);
  if(t != NULL) {
    if(t->docid->doc != NULL &&
       t->docid->doc->sourceID != NULL &&
       t->docid->doc->sourceID->filename != NULL)
      source = findsource(t->docid->doc->sourceID->filename,
			  the_Question->q->sourcepath);
    messwidget = t->status;
    if (source == NULL) {
      PrintStatus(STATUS_URGENT, STATUS_HIGH, NOSOURCE_MESSAGE,
		  t->docid->doc->sourceID->filename);
      return;
    }

    if((d = getNextorPrevDoc(the_Question->q, source, t->docid, FALSE))
       != NULL)
      ViewDoc(d, d->doc->type[0], 0, false);
    else
      PrintStatus(STATUS_URGENT, STATUS_HIGH, BADPREV_MESSAGE);
  }
}

/* ARGSUSED */
void setFile(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  XtPopdown(savereq);
  SetReqButtons(false);
}

/* ARGSUSED */
void quitFile(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  XtPopdown(savereq);
  SetReqButtons(false);
}

char* GetKeywordsUsed() {
  char* result = the_Question->q->keywords_used;

#ifdef BOOLEANS
  static char* nobool_question;
  int i, j;
  char savec;
#endif
    
#ifndef BOOLEANS
  if (result == NULL) result = the_Question->q->keywords;
#else
  if (result == NULL) {
    result = the_Question->q->keywords;
    if (nobool_question != NULL) {
       XtFree(nobool_question);
       nobool_question = NULL;
    }
  } 
#endif
#ifdef BOOLEANS
  if (nobool_question == NULL) {
    nobool_question = XtMalloc(sizeof(unsigned char) * (strlen(result)+1));
    *nobool_question = '\0';
    for(j = 0, i = 0; i <= strlen(result); i++) {	/* step thru keys */
      if ((result[i] == 0) || isspace(result[i]) || ispunct(result[i])) {
        savec = result[i];
        result[i] = '\0';
        /* toss booleans, need code here for partial matches as well? */
        if(! (0 == strcasecmp(&result[j], "and") || 
              0 == strcasecmp(&result[j], "or" ) || 
              0 == strcasecmp(&result[j], "not") )) {
          if(nobool_question[0] != '\0')		/* delimit keys */
	    strcat(nobool_question, " ");
          strcat(nobool_question, &result[j]);
  
        }
        result[i] = savec;
        j = i + 1;
      }
    }
  }
  if(strlen(nobool_question) > 0) /* make sure we didn't strip everything ...*/
    result = nobool_question;
#endif

  return(result);
}

