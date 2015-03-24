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
 * $Log: document.c,v $
 * Revision 2.0.1.3  1995/11/14 18:26:33  pfeifer
 * patch51:
 *
 * Revision 2.0.1.2  1995/09/15  09:53:11  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.1  1995/09/11  13:00:48  pfeifer
 * patch4:
 *
 * Revision 2.0  1995/09/08  08:04:33  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.3  1995/01/10  16:51:38  pfeifer
 * Little fix for multiple submissions of the same query, which breaks xwasq.
 * Not the last!
 *
 * Revision 1.2  1994/08/05  07:29:53  pfeifer
 * Release beta 04
 *
 * Revision 1.1  93/06/23  20:02:04  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.10  92/05/07  14:50:27  jonathan
 * Fixed listing for sections starting at 0.
 * 
 * Revision 1.9  92/04/02  14:23:36  jonathan
 * fleshed out copy_cretdoc.
 * 
 * Revision 1.8  92/04/01  17:16:20  jonathan
 * Added some copying functions.
 * 
 * Revision 1.7  92/03/17  14:36:40  jonathan
 * Cleaned up for use with X interface as well.
 * 
 * Revision 1.6  92/03/08  09:13:13  jonathan
 * strip lf from headlines.
 * 
 */

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ui/RCS/document.c,v 2.0.1.3 1995/11/14 18:26:33 pfeifer Exp $";
#endif

#include "wais.h"
#include "util.h"
#include "globals.h"
#include "document.h"


static void** copyList(list)
void **list;
{
  void **result = NULL;
  void **l1, **l2;
  long i;

  if(list != NULL) {
    for(l1 = list, i = 0; *l1 != NULL; i++, l1++);
      if((result = (void**)s_malloc((1+i)*sizeof(void*))) != NULL) {
      for(l1 = list, l2 = result; *l1 != NULL; l1++, l2++) {
	*l2 = s_strdup(*l1);
      }
      result[i] = NULL;
    }
  }
  return result;
}
      
static void freeList(list)
void **list;
{
  void** l = list;
  while(*l != NULL) {
    s_free(*l);
    l++;
  }
  s_free(list);
}

static void strip_lf(line)
char *line;
{
  long i;

  if (line!=NULL) {
    do {
      if (*line=='\r' || *line=='\n') *line='_';
    } while(*line++);
  }
}
    
static void
setdate(date, source)
char *date, *source;
{
  date[8] = 0;
  date[2] = date[5] = '/';
  date[0] = source[2];
  date[1] = source[3];
  date[3] = source[4];
  date[4] = source[5];
  date[6] = source[0];
  date[7] = source[1];
}

void
freeItemList(list)
char **list;
{
  char **temp;

  temp = list;

  while(*temp != NULL) {
    s_free(*temp);
    temp++;
  }

  s_free (list);
}

char **
buildDocumentItemList(doclist, scorep)
DocList doclist;
Boolean scorep;
{
  char **result, date[9];
  int num, i;
  DocList doc;

  /* find the length of the doclist in the question */

  for(num = 0, doc = doclist;
      doc != NULL && doc->thisDoc != NULL;
      num++, doc = doc->nextDoc);

  result = (char**) s_malloc((1+num)*sizeof(char*));
  result[num] = NULL;
  if(num > 0)
    for(i = 0, doc = doclist; i<num; i++, doc = doc->nextDoc) {
      if(doc->thisDoc != NULL) {
	if(scorep == TRUE) {
	  result[i] = s_malloc(strlen(doc->thisDoc->doc->headline)+26);
	  if ((doc->thisDoc->doc->date == NULL) ||
	      (strcmp(doc->thisDoc->doc->date, "0") == 0)) {
	    if (doc->thisDoc->doc->numChars > 1024)
	      sprintf(result[i], "%5d %4.1fK %s", 
		      doc->thisDoc->rawScore,
		      ((float)doc->thisDoc->doc->numChars/1024.0),
		      doc->thisDoc->doc->headline);
	    else
	      sprintf(result[i], "%5d %5d %s", 
		      doc->thisDoc->rawScore,
		      doc->thisDoc->doc->numChars,
		      doc->thisDoc->doc->headline);
	  }
	  else {
	    setdate(date, doc->thisDoc->doc->date);
	    if (doc->thisDoc->doc->numChars > 1024)
	      sprintf(result[i], "%5d %4.1fK (%s) %s",
		      doc->thisDoc->rawScore, 
		      ((float)doc->thisDoc->doc->numChars/1024.0),
		      date,
		      doc->thisDoc->doc->headline);
	    else
	      sprintf(result[i], "%5d %4d (%s) %s",
		      doc->thisDoc->rawScore, 
		      doc->thisDoc->doc->numChars,
		      date,
		      doc->thisDoc->doc->headline);
	    
	  }
	}
	else {
	  result[i] = s_malloc(strlen(doc->thisDoc->doc->headline)+30);
	  if(doc->thisDoc->end > 0) {
	    if ((doc->thisDoc->doc->date != NULL) &&
		(strcmp(doc->thisDoc->doc->date, "0") != 0)) {
	      setdate(date, doc->thisDoc->doc->date);
	      sprintf(result[i], "[%d,%d] (%s) %s", 
		      doc->thisDoc->start, doc->thisDoc->end, date,
		      doc->thisDoc->doc->headline);
	    }
	    else {
	      sprintf(result[i], "[%d,%d] %s", 
		      doc->thisDoc->start, doc->thisDoc->end,
		      doc->thisDoc->doc->headline);
	    }
	  }
	  else
	    if ((doc->thisDoc->doc->date != NULL) &&
		(strcmp(doc->thisDoc->doc->date, "0") != 0)) {
	      setdate(date, doc->thisDoc->doc->date);
	      sprintf(result[i], "(%s) %s",
		      date, doc->thisDoc->doc->headline);
	    }
	    else {
	      sprintf(result[i], "%s", doc->thisDoc->doc->headline);
	    }
	}
      }
      if(result[i] != NULL) result[i] = trim_junk(result[i]);
    }
  return(result);
}

DocList ReadListOfDocuments(file)
FILE *file;
{
  short check_result;
  DocumentID documentid = NULL;
  DocList result, this, last;
          
  /* initialize */
  this = last = result = NULL;

  if(ReadStartOfList(file) == FALSE)
    return(NULL);

  while(TRUE) {
    documentid = (DocumentID)s_malloc(sizeof(_DocumentID));
    documentid->start = -1;
    documentid->end = -1;
    check_result = ReadDocument(documentid, file);
    if(check_result == END_OF_STRUCT_OR_LIST) {
      s_free(documentid);
      return(result);
    }
    else if(check_result == FALSE)
      return(result);

    else if(check_result == TRUE) {
      if(result == NULL)
	result = this = (DocList)s_malloc(sizeof(_DocList));
      else
	this = (DocList)s_malloc(sizeof(_DocList));
      this->thisDoc = documentid;
      if(last != NULL)
	last->nextDoc = this;
      last = this;
    }
  }
}

short
ReadFragment(file, dest)
FILE *file;
long *dest;
{
  char temp_string[MAX_SYMBOL_SIZE];
  short check_result;

  /* initialize */
  check_result = CheckStartOfStruct("fragment", file);

  if(FALSE == check_result){ 
    return(false);
  }
  if(END_OF_STRUCT_OR_LIST == check_result)
    {
      return(FALSE);
    }
    
  /* read the slots: */
  while(check_result != END_OF_STRUCT_OR_LIST){
    short check_result = ReadSymbol(temp_string, file, MAX_SYMBOL_SIZE);
    if(END_OF_STRUCT_OR_LIST == check_result) {
      return(true);
    }
    if(0 == strcmp(temp_string, ":byte-pos")){
      if(FALSE == ReadLong(file, dest)){
	return(false);
      }
    }
    else if(0 == strcmp(temp_string, ":line-pos")){
      if(FALSE == ReadLong(file, dest)){
	return(false);
      }
    }
    else if(0 == strcmp(temp_string, ":para-id")){
      if(FALSE == ReadLong(file, dest)){
	return(false);
      }
    }
    else			/* we don't know what this is */
      SkipObject(file);
  }
  return(true);
}

/* Read a document from a file.  If it is the end of a list instead of
 * at a document, then return END_OF_STRUCT_OR_LIST, 
 * if it hits an error on loading, return FALSE,
 * otherwise return TRUE.
 */
short
ReadDoc(file, doc)
FILE *file;
CRetDocument doc;
{
  char temp_string[MAX_SYMBOL_SIZE];
  short check_result;
  long lines, chars, best;
  long i, numtypes = 0;
  char *types[100];
  DocID* docid = NULL;
  SourceID aSid;
  char headline[MAX_SYMBOL_SIZE];
  char dateStr[MAX_SYMBOL_SIZE];
          
  /* initialize */
  check_result = CheckStartOfStruct("document", file);
  headline[0] = '\0';
  dateStr[0] = '\0';

  if(FALSE == check_result){ 
    return(false);
  }
  if(END_OF_STRUCT_OR_LIST == check_result)
    {
      return(FALSE);
    }
    
  /* read the slots: */
  while(check_result != END_OF_STRUCT_OR_LIST){
    short check_result = ReadSymbol(temp_string, file, MAX_SYMBOL_SIZE);
    if(END_OF_STRUCT_OR_LIST == check_result) {
      if(*headline != 0)
	doc->headline = s_strdup(headline);
      doc->type = (char**)s_malloc((numtypes+1)*sizeof(char*));
      for(i = 0; i < numtypes; i++)
	doc->type[i] = types[i];
      doc->type[numtypes] = NULL;
      return(true);
    }
    if(FALSE == check_result){
      return(false);
    } 
    if(0 == strcmp(temp_string, ":number-of-lines")) {
      ReadLong(file,&lines);
      doc->numLines = lines;
    }
    else if ((0 == strcmp(temp_string, ":number-of-bytes")) ||
	     (0 == strcmp(temp_string, ":number-of-characters"))){
      ReadLong(file,&chars);
      doc->numChars = chars;
    }
    else if(0 == strcmp(temp_string, ":best-line")){
      ReadLong(file,&best);
      doc->best = best;
    }
    else if(0 == strcmp(temp_string, ":date")){
      if(FALSE == ReadString(dateStr, file, MAX_SYMBOL_SIZE)){
	return(false);
      }
      else doc->date = s_strdup(dateStr);
    }
    else if(0 == strcmp(temp_string, ":headline")){
      if(FALSE == ReadString(headline, file, MAX_SYMBOL_SIZE))
	return(false);
      doc->headline = s_strdup(headline);
    }
    else if(0 == strcmp(temp_string, ":doc-id")){
      docid = (DocID*)s_malloc(sizeof(DocID));
      if(FALSE == ReadDocID(docid, file)){
	return(false);
      }
      doc->id = docid;
    }
    else if(0 == strcmp(temp_string, ":source")){
      long return_value;
      aSid = (SourceID)s_malloc(sizeof(_SourceID));
      return_value = ReadSourceID(file, aSid);
      if (return_value == END_OF_STRUCT_OR_LIST || return_value == false)
	{ s_free(aSid);
	  return(false);
	}
      doc->sourceID = aSid;
    }
    else if(strcmp(temp_string, ":type") == 0) {
      if(FALSE == ReadString(temp_string, file, MAX_SYMBOL_SIZE))
	return(false);
      types[numtypes++] = s_strdup(temp_string);
    }
    else			/* we don't know what this is */
      SkipObject(file);
  }
  doc->type = (char**)s_malloc((numtypes+1)*sizeof(char*));
  for(i = 0; i < numtypes; i++)
    doc->type[i] = types[i];

  doc->type[numtypes] = NULL;

  return(true);
}

short ReadDocument(doc, file)
DocumentID doc;
FILE *file;
{
  CRetDocument document = NULL;
  short status;
  char temp_string[MAX_SYMBOL_SIZE];
  short result = CheckStartOfStruct("document-id",file);
  
/*
  start = end = NULL;
*/
  doc->rawScore = doc->normalScore = -1;
  
  if (result == false)
    return(false);
  if (result == END_OF_STRUCT_OR_LIST)
    return(END_OF_STRUCT_OR_LIST);
    
  while (TRUE)
   { short check_result;
     long val;
     check_result = ReadSymbol(temp_string,file,MAX_SYMBOL_SIZE);

     if (check_result == false)
       return(false);
     if (check_result == END_OF_STRUCT_OR_LIST)
       return(true);
       
     if (strcmp(temp_string,":score") == 0)
      { if (ReadLong(file,&val) == false)
          return(false);
        doc->rawScore = (short)val;
      }
     else if (strcmp(temp_string,":document") == 0) {
       document = MakeNewDocument();
       status = ReadDoc(file, document);
       if(status == false) {
	 fprintf(stderr, "error reading document structure.\n");
       }
       doc->doc = document;
     }
     else if(0 == strcmp(temp_string, ":start")){
       if(FALSE == ReadFragment(file, &val))
	 return(false);
       doc->start = val;
     }
     else if(0 == strcmp(temp_string, ":end")){
       if(FALSE == ReadFragment(file, &val))
	 return(false);
       doc->end = val;
     }
     else
       SkipObject(file);
   }
}

CRetDocument
MakeNewDocument()
{
  CRetDocument result;

  result = (CRetDocument)s_malloc(sizeof(_CRetDocument));
  result->id = NULL;
  result->sourceID = NULL;
  result->myConnection= NULL;
  result->numLines = 0;
  result->numChars = 0;
  result->blocks = result->pendingBlocks = NULL;
  result->best = 0;
  result->source = result->headline = result->city = NULL;
  result->stock = result->company = result->industry = NULL;
  result->type = NULL;
  result->date = NULL;
  result->next = result->prev = NULL;
  result->paraStarts = NULL; /* should be array[1] = -1; */
  
  return(result);
}


DocList
makeDocList(doc, rest)
DocumentID doc;
DocList rest;
{
  DocList result;
  if((result = (DocList)s_malloc(sizeof(_DocList))) != NULL) {
    result->thisDoc = doc;
    result->nextDoc = rest;
  }
  return(result);
}

static CRetDocument
copy_cretdoc(doc)
CRetDocument doc;
{
  CRetDocument result = NULL;
  if(doc != NULL &&
     (result = (CRetDocument)s_malloc(sizeof(_CRetDocument))) != NULL) {
    result->id = copyDocID(doc->id);
    result->sourceID = doc->sourceID; /* not copied - not freed! */
    result->numLines = doc->numLines;
    result->numChars = doc->numChars;
    result->best = doc->best;
    result->source = s_strdup(doc->source);
    result->headline = s_strdup(doc->headline);
    result->city = s_strdup(doc->city);
    result->stock = s_strdup(doc->stock);
    result->company = s_strdup(doc->company);
    result->industry = s_strdup(doc->industry);
    result->date = s_strdup(doc->date);
    result->type = (char**)copyList(doc->type);
  }
  return result;
}

DocumentID
copy_docid(doc)
DocumentID doc;
{
  DocumentID result;
  
  result = (DocumentID)s_malloc(sizeof(_DocumentID));
  result->rawScore = doc->rawScore;
  result->start = doc->start;
  result->end = doc->end;
  result->doc = copy_cretdoc(doc->doc);

  return result;
}

void WriteDocument(doc, fp)
DocumentID doc;
FILE *fp;
{
  WriteStartOfStruct("document-id", fp);
  WriteNewline(fp);
  WriteSymbol(":score", fp);
  WriteLong(doc->rawScore, fp);
  WriteNewline(fp);
  if(doc->start >= 0) {
    WriteSymbol(":start", fp);
    WriteNewline(fp);
    WriteStartOfStruct("fragment", fp);
    WriteNewline(fp);
    WriteSymbol(":line-pos", fp);
    WriteLong(doc->start, fp);
    WriteEndOfStruct(fp);
    WriteNewline(fp);
  }
  if(doc->end > 0) {
    WriteSymbol(":end", fp);
    WriteNewline(fp);
    WriteStartOfStruct("fragment", fp);
    WriteNewline(fp);
    WriteSymbol(":line-pos", fp);
    WriteLong(doc->end, fp);
    WriteEndOfStruct(fp);
    WriteNewline(fp);
  }
  if(doc->doc != NULL) {
    WriteSymbol(":document", fp);
    WriteNewline(fp);
    WriteStartOfStruct("document", fp);
    WriteNewline(fp);
    if(doc->doc->headline != NULL) {
      WriteSymbol(":headline", fp);
      WriteString(doc->doc->headline, fp);
    }
    if (doc->doc->id != NULL) {
      WriteNewline(fp);
      WriteSymbol(":doc-id", fp);
      WriteDocID(doc->doc->id, fp);
      WriteNewline(fp);
    }
    if (doc->doc->sourceID != NULL) {
      WriteSymbol(":source", fp);
      WriteNewline(fp);
      WriteStartOfStruct("source-id", fp);
      WriteNewline(fp);
      WriteSymbol(":filename", fp);
      WriteString(doc->doc->sourceID->filename, fp);
      WriteNewline(fp);
      WriteEndOfStruct(fp);
      WriteNewline(fp);
    }
    WriteSymbol(":number-of-lines", fp);
    WriteLong(doc->doc->numLines, fp);
    WriteNewline(fp);
    WriteSymbol(":number-of-bytes", fp);
    WriteLong(doc->doc->numChars, fp);
    WriteNewline(fp);
    if(doc->doc->type != NULL) {
      char **doctypes;
      for(doctypes = doc->doc->type;
	  *doctypes != NULL;
	  doctypes++) {
	WriteSymbol(":type", fp);
	WriteString(*doctypes, fp);
	WriteNewline(fp);
      }
    }
    WriteSymbol(":best-line", fp);
    WriteLong(doc->doc->best, fp);
    WriteNewline(fp);
    if(doc->doc->date != NULL) {
      WriteSymbol(":date", fp);
      WriteString(doc->doc->date, fp);
      WriteNewline(fp);
    }
    else {
      WriteSymbol(":date", fp);
      WriteString("0", fp);
      WriteNewline(fp);
    }
    WriteNewline(fp);
    WriteEndOfStruct(fp);
  }
  WriteNewline(fp);
  WriteEndOfStruct(fp);
  WriteNewline(fp);
}

void sort_document_list(dlist)
DocList dlist;
{
  DocList d;
  DocumentID di;
  Boolean Changed = TRUE;

  while(Changed) {
    Changed = FALSE;
    for(d = dlist; d != NULL && d->nextDoc != NULL; d = d->nextDoc)
      if(d->thisDoc != NULL && d->nextDoc->thisDoc != NULL)
	if(d->thisDoc->rawScore < d->nextDoc->thisDoc->rawScore) {
	  Changed = TRUE;
	  di = d->thisDoc;
	  d->thisDoc = d->nextDoc->thisDoc;
	  d->nextDoc->thisDoc = di;
	}
  }
}
      
DocList
findLast(dlist)
DocList dlist;
{
  DocList d;

  if(dlist == NULL || dlist->nextDoc == NULL) return dlist;
  for(d = dlist; d->nextDoc != NULL; d = d->nextDoc);
  return d;
}

DocumentID
findDoc(dlist, number)
DocList dlist;
int number;
{
  DocList d;
  int i;

  for(i = 0, d = dlist; (d != NULL) && (i < number); i++, d = d->nextDoc);

  if (d != NULL)
    return(d->thisDoc);
  else
    return(NULL);
}

DocumentID
fillDocumentID(docHeader, source)
WAISDocumentHeader* docHeader;
SourceID source;
{
  DocumentID docID;

  if((docID = (DocumentID)s_malloc(sizeof(_DocumentID))) != NULL) {
    docID->rawScore = docHeader->Score;
    docID->start = docID->end = -1;
    if((docID->doc = (CRetDocument)s_malloc(sizeof(_CRetDocument))) != NULL) {
      docID->doc->sourceID = source;
      /*	  docID->doc->version = docHeader->VersionNumber; */
      docID->doc->numLines = docHeader->Lines;
      docID->doc->numChars = docHeader->DocumentLength;
      docID->doc->best = docHeader->BestMatch;

      if(docHeader->Headline != NULL) {
	docID->doc->headline = s_strdup(docHeader->Headline);
	/*strip_lf(docID->doc->headline);*/
      }
      if(docHeader->Source != NULL)
	docID->doc->source = s_strdup(docHeader->Source);
      if(docHeader->OriginCity != NULL)
	docID->doc->city = s_strdup(docHeader->OriginCity);
      if(docHeader->Date != NULL)
	docID->doc->date = s_strdup(docHeader->Date);
      if(docHeader->Types != NULL) {
	char **types = docHeader->Types;
	long numtypes = 0, t;
	while (*types++) numtypes++;
	docID->doc->type = (char **)s_malloc((numtypes+1)*sizeof(char*));
	for(types = docHeader->Types, t=0;
	    t < numtypes;
	    t++)
	  docID->doc->type[t] = s_strdup(types[t]);
	docID->doc->type[numtypes] = NULL;
      }
      else {
	docID->doc->type = (char **)s_malloc(2*sizeof(char*));
	docID->doc->type[0] = s_strdup("TEXT");
	docID->doc->type[1] = NULL;
      }
      if((docID->doc->id = docIDFromAny(docHeader->DocumentID))
	== NULL) {
	docID->doc->id = (DocID*)s_malloc(sizeof(DocID));
	memset(docID->doc->id, 0, sizeof(DocID));
	docID->doc->id->originalLocalID =
	  duplicateAny(docHeader->DocumentID);
      }
/*
      if(docHeader->DocumentID != NULL)
	docID->doc->id->originalLocalID =
	  duplicateAny(docHeader->DocumentID);
*/
    }
  }
  return docID;
}

void
freeCRetDocument(doc)
CRetDocument doc;
{
  s_free(doc->source);
  s_free(doc->headline);
  s_free(doc->city);
  s_free(doc->stock);
  s_free(doc->company);
  s_free(doc->industry);
  s_free(doc->date);
  if(doc->type != NULL)
    freeList(doc->type);
  freeDocID(doc->id);
  s_free(doc);
}

void
freeDocumentID(docID)
DocumentID docID;
{
  if(docID != NULL) {
    if(docID->doc != NULL) {
      freeCRetDocument(docID->doc);
      docID->doc = NULL;
    }
    s_free(docID);
  }
}

void
freeDocList(doclist)
DocList doclist;
{
  DocList dl;
  while(doclist != NULL) {
    freeDocumentID(doclist->thisDoc);
    dl = doclist;
    doclist = doclist->nextDoc;
    s_free(dl);
  }
}
