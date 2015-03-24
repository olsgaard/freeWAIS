/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: document.h,v $
 * Revision 2.0.1.1  1995/09/15 09:53:14  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:05:00  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:29:56  pfeifer
 * Release beta 04
 *
 * Revision 1.1  93/06/23  20:02:31  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.2  92/03/17  14:37:32  jonathan
 * Cleaned up for use with X interface as well.
 * Prototyped everything.
 * 
 *
 */

#ifndef _H_DOCUMENT
#define _H_DOCUMENT

#include <docid.h>
#include "source.h"

typedef struct CRetDocument
 { 
   /* instance variables */
   DocID *id;
   SourceID sourceID;
   any*			myConnection;				/* really a ConnectionID but circular includes preclude that */
   long 			numLines;    				/* number of lines in document */
   long 			numChars;   				/* number of characters */
   char 		*blocks,*pendingBlocks; 	/* lists of CLineBlocks */
   long 			best;
   char		*source,*headline,*city,*stock,*company,*industry, **type, *date;
   struct CRetDocument 	*next,*prev; 				/* linked docs if there are any */
   char			*paraStarts;					/* i'th paragraph starts at line paraStarts[i] 
   												   -1 terminated 
   												 */
   
 } _CRetDocument, *CRetDocument;

typedef struct DocumentID {
  short rawScore, normalScore;
  long start, end;
  CRetDocument doc;
} _DocumentID, *DocumentID;

typedef struct doclist {
  DocumentID thisDoc;
  struct doclist *nextDoc;
} _DocList, *DocList;

/* functions */

void freeItemList _AP((char** list));
char **buildDocumentItemList _AP((DocList doclist, Boolean scorep));
DocList ReadListOfDocuments _AP((FILE* file));
short ReadFragment _AP((FILE* file, long* dest));
short ReadDoc _AP((FILE* file, CRetDocument doc));
short ReadDocument _AP((DocumentID doc, FILE* file));
CRetDocument MakeNewDocument _AP((void));
DocList makeDocList _AP((DocumentID doc, DocList rest));
DocumentID copy_docid _AP((DocumentID doc));
void WriteDocument _AP((DocumentID doc, FILE* fp));
void sort_document_list _AP((DocList dlist));
DocList findLast _AP((DocList dlist));
DocumentID findDoc _AP((DocList dlist, int number));
DocumentID fillDocumentID _AP((WAISDocumentHeader* docHeader,SourceID source));
void freeDocumentID _AP((DocumentID doc));
void freeDocList _AP((DocList doclist));

#endif
