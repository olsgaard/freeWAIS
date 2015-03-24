/*                               -*- Mode: C -*- 
 * wais-ui.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Jonny Goldman?
 * Created On      : Thu Sep 14 16:05:48 1995
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Fri Sep 29 11:34:20 1995
 * Language        : C
 * Update Count    : 5
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 */

/* 
   WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface tools for the WAIS software.
   Do with it as you please.

   jonathan@Think.COM
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: wais-ui.c,v $
 * Revision 2.0.1.3  1995/11/14 18:27:42  pfeifer
 * patch51:
 *
 * Revision 2.0.1.2  1995/09/29  11:18:27  pfeifer
 * patch17: Won't prepend source->directory if source->database starts
 * patch17: with '/'.
 *
 * Revision 2.0.1.1  1995/09/15  09:55:29  pfeifer
 * patch7: Fixed CNIDR copyright line.
 * patch7: Fix from kru@upc.muc.de (Frithjof Kruggel): -s option in
 * patch7: waisq, waissearch.
 *
 * Revision 2.0  1995/09/08  08:05:34  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.3  1995/01/10  16:50:57  pfeifer
 * Fix from Jonny
 *
 * Revision 1.2  1994/08/05  07:30:21  pfeifer
 * Release beta 04
 *
 * Revision 1.31  93/06/23  20:02:23  warnock
 * Initial revision
 * 
 * Revision 1.24  92/06/03  17:19:20  jonathan
 * Reset signal handlers on use.  From mycroft@gnu.ai.mit.edu.
 * 
 * Revision 1.23  92/05/19  17:19:50  jonathan
 * Fixed bug where RetrieveWaisDocument failed to remove Seeker markup.
 * 
 * Revision 1.22  92/05/07  14:52:04  jonathan
 * Changed use of setitimer to alarm.  Thanks to
 * steinkel@carlisle-emh2.army.mil.
 * 
 * Revision 1.21  92/04/30  10:59:05  jonathan
 * Replace strdup with s_strdup.
 * 
 * Revision 1.20  92/04/02  14:21:59  jonathan
 * Copy type in all calls to makeDocObjUsing a generate_retrieval_apdu (which
 * calls makeDocObjUsing...).
 * 
 * Revision 1.19  92/04/01  17:19:35  jonathan
 * Fixed reporting of diagnostics when return text is empty.
 * 
 * Revision 1.18  92/03/17  14:33:17  jonathan
 * Renamed to wais-ui, cleaned up for use by X interface as well as shell
 * interfaces.
 * 
 * Revision 1.17  92/03/08  10:42:28  jonathan
 * Added one last loadSource in ViewWaisDocument.
 * 
 * Revision 1.16  92/03/06  14:50:13  jonathan
 * New and Improved source loading!
 * 
 */

#ifndef lint
static char    *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ui/RCS/wais-ui.c,v 2.0.1.3 1995/11/14 18:27:42 pfeifer Exp $";

#endif

#define _C_QUESTION

#include "wais.h"
#include "globals.h"

void
showDiags (d)
     diagnosticRecord **d;
{
  int             i;

  /* char msg[256]; unused (up) */

  for (i = 0; d[i] != NULL; i++) {
    if (d[i]->ADDINFO != NULL) {
      PrintStatus (STATUS_URGENT, STATUS_HIGH, DIAG_MESSAGE,
		   d[i]->DIAG, d[i]->ADDINFO);
    }
  }
}

void
printDiags (d)
     diagnosticRecord **d;
{
  int             i;

  for (i = 0; d[i] != NULL; i++)
    if (d[i]->ADDINFO != NULL)
      printf ("%s\n", d[i]->ADDINFO);
}

void
write_text_record_completely (fp, record, quote_string_quotes)
     FILE           *fp;
     WAISDocumentText *record;
     Boolean         quote_string_quotes;
{
  long            count;

  /* fprintf(fp," Text\n");
     print_any("     DocumentID:  ", record->DocumentID);
     fprintf(fp,"     VersionNumber:  %d\n", record->VersionNumber);
   */
  for (count = 0; count < record->DocumentText->size; count++) {
    int             ch = record->DocumentText->bytes[count];

    if (27 == ch) {
      /* then we have an escape code */
      /* if the next letter is '(' or ')', then ignore two letters */
      if ('(' == record->DocumentText->bytes[count + 1] ||
	  ')' == record->DocumentText->bytes[count + 1])
	count += 1;		/* it is a term marker */
      else
	count += 4;		/* it is a paragraph marker */
    } else if (isprint (ch)) {
      if (quote_string_quotes && ch == '"')
	putc ('\\', fp);
      putc (ch, fp);
    } else if (ch == '\n' || ch == '\r')
      fprintf (fp, "\n");
  }
}

/* for making searches */

DocList
build_response_list (response, source, seed_words_used)
     SearchResponseAPDU *response;
     SourceID        source;
     char          **seed_words_used;
{
  long            i, k;
  WAISSearchResponse *info;
  DocList         last = NULL, doc, result = NULL;
  DocumentID      docID;

  k = response->NumberOfRecordsReturned;

  if (response->DatabaseDiagnosticRecords != 0) {
    info = (WAISSearchResponse *) response->DatabaseDiagnosticRecords;
    if (seed_words_used != NULL) {
      if (*seed_words_used != NULL)
	s_free (*seed_words_used);
      if (info->SeedWordsUsed != NULL)
	*seed_words_used = s_strdup (info->SeedWordsUsed);
    }
    if (info->DocHeaders != NULL) {
      for (i = 0; i < k; i++) {
	if (info->DocHeaders[i] != NULL) {
	  if (result == NULL) {
	    doc = result = makeDocList (NULL, NULL);
	  } else
	    doc = makeDocList (NULL, NULL);

	  docID = fillDocumentID (info->DocHeaders[i], source);
	  doc->thisDoc = docID;

	  if (last != NULL) {
	    last->nextDoc = doc;
	  }
	  last = doc;
	}
      }
    }
  }
  return result;
}

/* right now this hacks out the ^Q/S too.  I'll do better later. */

void
replacecontrolM (buffer, length)
     char           *buffer;
     long           *length;
{
  char           *here, *there, c;
  long            i, newlength;

  here = there = buffer;
  for (newlength = 0, i = 0; i < *length; i++) {
    c = *here;
    switch (c) {
    case 0:
      *there = 0;
      *length = newlength;
      return;
    case '\r':
      *there = '\n';
      newlength++;
      here++;
      there++;
      break;
    case 19:
    case 17:
      here++;
      break;
    default:
      *there = *here;
      newlength++;
      here++;
      there++;
    }
  }
  *length = newlength;
}

/* This is a compy from ../x/qcommands.c (up) */

static void
truncate_document_list (dl, n)
     DocList         dl;
     long            n;
{
  while (--n > 0)
    if (dl == NULL ||
	(dl = dl->nextDoc) == NULL)
      return;

  freeDocList (dl->nextDoc);
  dl->nextDoc = NULL;
  return;
}

void
SearchWais (q)
     Question        q;
{
  Source          source;

  /* SList asource; unused (up) */
  static long     request_buffer_length;
  DocObj        **Doc;
  int             i;
  char           *request_message, *response_message;

  /* char message[255]; unused (up) */
  DocList         last;
  diagnosticRecord **diag;
  char            message[256];
  char           *database = message;
  long            numdocs, result;
  WAISSearchResponse *response;

  request_message = (char *) q->request_message;
  response_message = (char *) q->response_message;

  /* clear the results */

  /*
     freeDocList(q->ResultDocuments);
   */
  q->ResultDocuments = NULL;

  /* build DocObjs */

  Doc = (DocObj **) s_malloc ((q->numdocs + 1) * sizeof (char *));

  q->numsources = listlength ((List) q->Sources);

  numdocs = q->maxresdocs;
  {
    DocList         dl;

    for (i = 0, dl = q->RelevantDocuments;
	 dl != NULL;
	 dl = dl->nextDoc, i++)
      if (dl->thisDoc->doc != NULL) {
	char           *tmptype = s_strdup ((dl->thisDoc->doc->type) ?
					dl->thisDoc->doc->type[0] : "TEXT");

	if (dl->thisDoc->doc->id != NULL)
	  if (dl->thisDoc->start >= 0)
	    Doc[i] =
	      makeDocObjUsingLines (anyFromDocID (dl->thisDoc->doc->id),
			     tmptype, dl->thisDoc->start, dl->thisDoc->end);
	  else
	    Doc[i] =
	      makeDocObjUsingWholeDocument (anyFromDocID (dl->thisDoc->doc->id),
					    tmptype);

      }
    Doc[i] = NULL;
  }

  /* check to see if the question has a source */
  source = NULL;

  if (q->Sources != NULL) {
    SourceList      slist;

    for (slist = q->Sources;
	 slist != NULL;
	 slist = slist->nextSource) {
      source = findsource (slist->thisSource->filename, q->sourcepath);

      if (source == NULL) {
	PrintStatus (STATUS_URGENT, STATUS_HIGH, NOSOURCE_MESSAGE,
		     slist->thisSource->filename);
      } else {
	PrintStatus (STATUS_INFO, STATUS_LOW, SEARCH_MESSAGE, source->name);

	if (source->initp != TRUE) {
	  PrintStatus (STATUS_INFO, STATUS_LOW, INITSOURCE_MESSAGE);
	  init_for_source (source, request_message, MAX_MESSAGE_LEN,
			   response_message);
	}
	if (source->database[0] == 0)
	  database = NULL;
	else if (source->database[0] == '/')
	  sprintf (database, "%s", source->database);
	else
	  sprintf (database, "%s%s",
		   source->directory, source->database);

	if (source->initp != FALSE) {
	  result = 0;
	  request_buffer_length = source->buffer_length;
	  if (NULL ==
	      generate_search_apdu (request_message + HEADER_LENGTH,
				    &request_buffer_length,
				    q->keywords, database, Doc, numdocs)) {
	    PrintStatus (STATUS_URGENT, STATUS_HIGH, BUFFOVER_MESSAGE);
	  } else if ((result =
		      interpret_message (request_message,
					 (source->buffer_length -
					  request_buffer_length),
					 response_message,
					 source->buffer_length,
					 source->connection,
					 false	/* true verbose */
		      )) == 0) {
	    PrintStatus (STATUS_URGENT, STATUS_LOW, NODATA_MESSAGE);
	    close_source (source);
	  }
	  if (result != 0) {	/* use the repsonse */
	    readSearchResponseAPDU (&q->query_response,
				    response_message + HEADER_LENGTH);

	    if (q->query_response != NULL)
	      if ((response = (WAISSearchResponse *)
		   q->query_response->DatabaseDiagnosticRecords) != NULL) {
		if ((diag = response->Diagnostics) != NULL)
		  showDiags (diag);

		if (q->ResultDocuments != NULL) {
		  char           *keys_used = NULL;

		  last = findLast (q->ResultDocuments);
		  last->nextDoc = build_response_list (q->query_response,
						       slist->thisSource,
						       &keys_used);
		  if (keys_used != NULL) {
		    if (keys_used[0] != 0) {
		      if (q->keywords_used == NULL)
			q->keywords_used = keys_used;
		      else {
			if (strcmp (q->keywords_used, keys_used) != 0) {
			  q->keywords_used =
			    (char *) s_realloc (q->keywords_used,
						strlen (q->keywords_used) + strlen (keys_used) + 1);
			  strcat (q->keywords_used, " ");
			  strcat (q->keywords_used, keys_used);
			}
		      }
		      s_free (keys_used);
		    }
		  }
		} else
		  q->ResultDocuments =
		    build_response_list (q->query_response,
					 slist->thisSource,
					 &q->keywords_used);
		freeWAISSearchResponse (response);
	      }
	  }
	} else {
	  PrintStatus (STATUS_URGENT, STATUS_HIGH, INITFAIL_MESSAGE);
	}
      }
      sort_document_list (q->ResultDocuments);
      truncate_document_list (q->ResultDocuments, q->maxresdocs);
    }
    /* ok, now we've got all the documents, let's sort them */

    sort_document_list (q->ResultDocuments);

    q->numresdocs = listlength ((List) q->ResultDocuments);
    PrintStatus (STATUS_INFO, STATUS_MEDIUM, FOUND_MESSAGE, q->numresdocs);
  } else {
    PrintStatus (STATUS_URGENT, STATUS_HIGH,
	      "\nThis Question has no sources to search.  Please add one.");
    q->ResultDocuments = NULL;
    q->numresdocs = listlength ((List) q->ResultDocuments);
  }
  if (Doc != NULL) {
    doList ((void **) Doc, freeDocObj);
    s_free (Doc);
  }
}

char           *
GetWaisDocument (q, doc, type, fp, written)
     Question        q;
     DocumentID      doc;
     char           *type;
     FILE           *fp;
     long           *written;
{
  static long     request_length, chars_per_page;
  static long     lines, size, count, chars, numChars;
  long            start_byte, end_byte;
  any            *docany;
  WAISDocumentText *text;
  diagnosticRecord **diag;
  char           *viewtext /* , message[255] unused */ ;
  char           *viewbuffer;
  char           *tmptype;
  Source          source;

  /* SList asource;unused */
  char            message[256];
  char           *database = message;	    /* asb: was char * with no allocation??? */
  long            result;
  WAISSearchResponse *response;

  source = NULL;

  if (doc == NULL)
    return NULL;

  if (doc->doc != NULL &&
      doc->doc->sourceID != NULL &&
      doc->doc->sourceID->filename != NULL) {
    source = findsource (doc->doc->sourceID->filename, q->sourcepath);
  }
  if (source == NULL) {
    PrintStatus (STATUS_URGENT, STATUS_HIGH, NOSOURCE_MESSAGE,
		 doc->doc->sourceID->filename);
    return NULL;
  }
  if (fp != stdout)
    PrintStatus (STATUS_INFO, STATUS_MEDIUM, GETDOC_MESSAGE,
		 doc->doc->headline, source->name);

  if (source->database[0] == 0)
    database = NULL;
  else if (source->database[0] == '/')
    sprintf (database, "%s", source->database);
  else
    sprintf (database, "%s%s",
	     source->directory, source->database);

  size = 0;

  lines = doc->doc->numLines;
  if (written != NULL && *written != 0)
    chars = *written;
  else
    chars = doc->doc->numChars;

  numChars = chars + 1000;	/* for slop? */

  docany = anyFromDocID (doc->doc->id);

  if (source->initp == FALSE)
    init_for_source (source, q->request_message, MAX_MESSAGE_LEN,
		     q->response_message);

  if (source->initp == FALSE) {
    PrintStatus (STATUS_URGENT, STATUS_HIGH, INITFAIL_MESSAGE);
    *written = 0;
    return (NULL);
  }
  chars_per_page = source->buffer_length - HEADER_LENGTH - 1000;	/* ? */

  if (fp == NULL) {
    viewbuffer = (char *) s_malloc (chars < 0 ? chars_per_page : numChars);

    if (viewbuffer == NULL) {
      PrintStatus (STATUS_URGENT, STATUS_HIGH, BADALLOC_MESSAGE);
      return NULL;
    }
    viewtext = viewbuffer;
  }
  tmptype = s_strdup ((type ? type : "TEXT"));

  if (chars == 0)
    PrintStatus (STATUS_INFO, STATUS_HIGH,
		 "\nEmpty document!");
  else if (source->initp != FALSE) {
    for (count = 0;
	 (chars < 0) || (count * chars_per_page < chars);
	 count++) {
      /* show as we go... */
      request_length = source->buffer_length;

      if (chars < 0) {
	numChars = (count + 1) * chars_per_page;
	if (fp == NULL) {
	  viewbuffer = s_realloc (viewbuffer, numChars);
	}
      }
      start_byte = count * chars_per_page;
      end_byte = (chars < 0 ?
		  (count + 1) * chars_per_page :
		  MINIMUM ((count + 1) * chars_per_page, chars));
      if (NULL ==
	  generate_retrieval_apdu (q->request_message + HEADER_LENGTH,
				   &request_length, docany, CT_byte,
				   start_byte, end_byte,
				   tmptype, database)) {
	PrintStatus (STATUS_URGENT, STATUS_HIGH, BUFFOVER_MESSAGE);
	return NULL;
      }
      if ((result =
	   interpret_message (q->request_message,
			      source->buffer_length - request_length,
			      q->response_message,
			      source->buffer_length,
			      source->connection,
			      false	/* true verbose */
	   )) == 0) {
	PrintStatus (STATUS_URGENT, STATUS_HIGH, NODATA_MESSAGE);
	close_source (source);
	return NULL;
      }
      if (result != 0)
	readSearchResponseAPDU (&q->retrieval_response,
				q->response_message + HEADER_LENGTH);

      if (q->retrieval_response != NULL) {
	if ((response = (WAISSearchResponse *)
	     q->retrieval_response->DatabaseDiagnosticRecords) != NULL) {
	  diag = response->Diagnostics;

	  if (NULL == response->Text) {
	    if (diag != NULL)
	      showDiags (diag);

	    PrintStatus (STATUS_INFO, STATUS_MEDIUM, DONE_MESSAGE);
	    if (written != NULL)
	      *written = size;
	    return (viewbuffer);
	  } else {
	    text = response->Text[0];
	    if ((type == NULL) || (strcmp (type, "TEXT") == 0)) {
	      long            length = text->DocumentText->size;;

	      delete_seeker_codes (text->DocumentText->bytes, &length);
	      text->DocumentText->size = length;
	      replacecontrolM (text->DocumentText->bytes, &length);
	      text->DocumentText->size = length;
	    }
	    if (text->DocumentText->size > (end_byte - start_byte))
	      /* something wrong! */
	      text->DocumentText->size = (end_byte - start_byte);

	    viewtext = viewbuffer + size;
	    size += text->DocumentText->size;
	    if (size <= numChars) {
	      if (fp == NULL) {
		memcpy (viewtext, text->DocumentText->bytes, text->DocumentText->size);
	      } else {
		dumptext (fp, text->DocumentText->bytes, text->DocumentText->size);
	      }

	      if (fp != stdout)
		PrintStatus (STATUS_CONT, STATUS_LOW, RECEIVE_MESSAGE,
			     size, source->name);

	      if (diag &&
		  diag[0] &&
		  diag[0]->ADDINFO != NULL &&
		  !strcmp (diag[0]->DIAG, D_PresentRequestOutOfRange))
		break;
	      if ((type != NULL) &&
		  (strcmp (type, "TEXT") != 0) &&
		  (text->DocumentText->size != chars_per_page))
		break;
	    } else {
	      PrintStatus (STATUS_URGENT, STATUS_HIGH, BUFFOVER_MESSAGE);
	      break;
	    }
	  }

	  if (diag != NULL)
	    showDiags (diag);

	  freeWAISSearchResponse (response);
	}
      }
    }
  }
  if (written != NULL)
    *written = size;
  return (viewbuffer);
}

void
RetrieveWaisDocument (start_output, end_output, q, doc)
     void            (*start_output) ();
     void            (*end_output) ();
     Question        q;
     DocumentID      doc;
{
  FILE           *fp;
  int             output_started;
  int             screen_output;
  long            written = 0;
  char           *result;

  output_started = FALSE;
  screen_output = FALSE;

  if (output_started == FALSE) {
    if (start_output != NULL) {
      start_output (&fp);
      screen_output = FALSE;
      if (&fp == NULL) {
	PrintStatus (STATUS_URGENT, STATUS_HIGH, "Unable to open output.");
	return;
      }
    } else {
      screen_output = TRUE;
      fp = NULL;
    }
    output_started = TRUE;
  }
  result = GetWaisDocument (q, doc,
			    (doc->doc->type ?
			     (doc->doc->type[0] ?
			      doc->doc->type[0] : "TEXT") : "TEXT"),
			    fp, &written);

  if (screen_output == TRUE)
    DumpTextToScreen (result, written);

  if ((output_started == TRUE) && (screen_output == FALSE))
    end_output (&fp);
}

void
dumptext (fp, buffer, size)
     FILE           *fp;
     char           *buffer;
     long            size;
{
  long            i;

  for (i = 0; i < size; i++)
    fputc (*buffer++, fp);
  fflush (fp);
}

void
test_connection (q)
     Question        q;
{
  Source          source;
  SList           asource;
  static long     request_buffer_length;
  char           *request_message, *response_message;

  /* char message[255]; unused (up) */
  diagnosticRecord **diag;
  char           *database = NULL /* not needed here */ ;

  request_message = (char *) q->request_message;
  response_message = (char *) q->response_message;

  /* check to see if the question has a source */
  source = NULL;

  if (q->Sources != NULL) {
    SourceList      slist;

    for (slist = q->Sources;
	 slist != NULL;
	 slist = slist->nextSource) {
      for (asource = Sources;
	   asource != NULL;
	   asource = asource->nextSource) {
	if (asource->thisSource != NULL &&
	    asource->thisSource->name != NULL &&
	    !strcmp (slist->thisSource->filename,
		     asource->thisSource->name)) {
	  source = asource->thisSource;
	  break;
	}
      }

      if (source == NULL) {
	PrintStatus (STATUS_URGENT, STATUS_HIGH, NOSOURCE_MESSAGE,
		     slist->thisSource->filename);
      } else {
	printf ("Source %s ", slist->thisSource->filename);
	if (source->maintainer)
	  printf ("by %s ", source->maintainer);
	if (source->server[0] != 0)
	  printf ("at %s ", source->server);

	if (source->initp == FALSE) {
	  freopen ("/dev/null", "w", stderr);
	  init_for_source (source, q->request_message, MAX_MESSAGE_LEN,
			   q->response_message);
	  request_buffer_length = source->buffer_length;

	  if (source->initp == FALSE) {
	    printf ("not responding.\n");
	  } else {		/* now lets test the database */
	    if (source->database[0] == 0)
	      database = NULL;
	    else
	      database = source->database;

	    if (NULL ==
		generate_search_apdu (request_message + HEADER_LENGTH,
				      &request_buffer_length,
				      "?", database, NULL, 1)) {
	      PrintStatus (STATUS_URGENT, STATUS_HIGH, BUFFOVER_MESSAGE);
	    } else {
	      long            r = 0;

	      if (0 ==
		  (r = interpret_message (request_message,
					  (source->buffer_length -
					   request_buffer_length),
					  response_message,
					  source->buffer_length,
					  source->connection,
					  false		/* true verbose */
		   ))) {
		PrintStatus (STATUS_URGENT, STATUS_HIGH, NODATA_MESSAGE);
	      }
	      if (r != 0) {
		readSearchResponseAPDU (&q->query_response,
					response_message + HEADER_LENGTH);

		if (q->query_response != NULL)
		  if ((WAISSearchResponse *) q->query_response->DatabaseDiagnosticRecords != NULL)
		    if ((diag =
			 ((WAISSearchResponse *) q->query_response->DatabaseDiagnosticRecords)->Diagnostics) != NULL) {
		      printf ("not responding:\n");
		      printDiags (diag);
		    } else
		      printf ("responding.\n");
	      }
	    }
	  }
	}
      }
    }
  } else {
    printf ("\nError: No sources to search.  Please add one.\n");
  }
}

void
close_source (source)
     Source          source;
{
  SList           t;

  if (source->connection != NULL) {
    fclose (source->connection);
  }
  for (t = Sources; t != NULL; t = t->nextSource) {
    if (strcmp (t->thisSource->server, source->server) == 0 &&
	strcmp (t->thisSource->service, source->service) == 0) {
      t->thisSource->initp = FALSE;
      t->thisSource->connection = NULL;
    }
  }
}

DocumentID
getNextorPrevDoc (q, source, doc, nextp)
     Question        q;
     Source          source;
     DocumentID      doc;
     Boolean         nextp;
{
  static long     request_length;
  static long     size /*, lines, count, numChars unused (up) */ ;
  any            *docany;

  /* char message[255]; unused */
  /* int i; unused (up) */
  diagnosticRecord **diag;
  char           *database, *type;

  /* DocumentID result; unused (up) */
  /* DocID *newdoc; unused (up) */
  DocObj         *Doc[2];
  WAISSearchResponse *info;

  /* char *text, document_id[MAX_FILE_NAME_LEN] unused */ ;
  /* char *loc1, *loc2, *loc3, newtype[100], headline[MAX_FILE_NAME_LEN] unused */ ;
  /* long newsize; unused */

  if (source->database[0] == 0)
    database = NULL;
  else
    database = source->database;

  docany = anyFromDocID (doc->doc->id);

  size = 0;

  if (nextp == TRUE)
    type = "WAIS_NEXT";
  else
    type = "WAIS_PREV";

  if (source->initp == FALSE)
    init_for_source (source, q->request_message, MAX_MESSAGE_LEN,
		     q->response_message);

  request_length = source->buffer_length;

  Doc[0] = makeDocObjUsingWholeDocument (docany, s_strdup (type));
  Doc[1] = NULL;
  if (0 ==
      generate_search_apdu (q->request_message + HEADER_LENGTH,
			    &request_length,
			    "foo", database, Doc, 1)) {
    PrintStatus (STATUS_URGENT, STATUS_HIGH, BUFFOVER_MESSAGE);
    return NULL;
  }
  if (0 ==
      interpret_message (q->request_message,
			 (source->buffer_length - request_length),
			 q->response_message,
			 source->buffer_length,
			 source->connection,
			 false	/* true verbose */
      )) {
    PrintStatus (STATUS_URGENT, STATUS_HIGH, NODATA_MESSAGE);
    source->initp = FALSE;
    return NULL;
  }
  readSearchResponseAPDU (&q->retrieval_response,
			  q->response_message + HEADER_LENGTH);

  if (q->retrieval_response != NULL)
    if ((WAISSearchResponse *) q->retrieval_response->DatabaseDiagnosticRecords != NULL)
      if ((diag =
	   ((WAISSearchResponse *) q->retrieval_response->DatabaseDiagnosticRecords)->Diagnostics) != NULL)
	showDiags (diag);

  info = (WAISSearchResponse *) q->retrieval_response->DatabaseDiagnosticRecords;

  if (info != NULL &&
      info->DocHeaders != NULL) {
    return (fillDocumentID (info->DocHeaders[0], doc->doc->sourceID));
  } else
    return NULL;
}
