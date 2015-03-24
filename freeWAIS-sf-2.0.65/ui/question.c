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
 * $Log: question.c,v $
 * Revision 2.0.1.1  1995/09/15 09:53:45  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:05:15  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:30:12  pfeifer
 * Release beta 04
 *
 * Revision 1.1  93/06/23  20:02:16  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.4  92/03/17  14:38:24  jonathan
 * Rename from qread.  This is more like source.c now.  For use with X
 * interface.
 * 
 */

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ui/RCS/question.c,v 2.0.1.1 1995/09/15 09:53:45 pfeifer Exp $";
#endif

#define _C_QREAD

#include "wais.h"
#include "globals.h"

short readRect(question, file)
Question question;
FILE *file;
{
  char temp_string[MAX_SYMBOL_SIZE];
  short check_result;
  long left, right, top, bottom;

  check_result = CheckStartOfStruct("rect", file);
  if(FALSE == check_result){ 
    return(false);
  }
  if(END_OF_STRUCT_OR_LIST == check_result)
    {
      return(FALSE);
    }
    
  /* read the slots: */
  while(TRUE){
    short check_result = ReadSymbol(temp_string, file, MAX_SYMBOL_SIZE);
    if(END_OF_STRUCT_OR_LIST == check_result) break;
    if(FALSE == check_result){
      return(false);
    } 
    if(0 == strcmp(temp_string, ":left")) {
      if (FALSE == ReadLong(file, &left))
	return(false);
    }
    else if(0 == strcmp(temp_string, ":right")) {
      if (FALSE == ReadLong(file, &right))
	return(false);
    }
    else if(0 == strcmp(temp_string, ":top")) {
      if (FALSE == ReadLong(file, &top))
	return(false);
    }
    else if(0 == strcmp(temp_string, ":bottom")) {
      if (FALSE == ReadLong(file, &bottom))
	return(false);
    }
    else
      SkipObject(file);
  }
  return(TRUE);
}

short ReadQuestion(question, file)
Question question;
FILE *file;
{
  char temp_string[5000];
  short check_result;

  long version;

  /* make sure it's a question */
  
  check_result = CheckStartOfStruct("question", file);
  if(FALSE == check_result){ 
    return(false);
  }
  if(END_OF_STRUCT_OR_LIST == check_result)
    {
      return(FALSE);
    }
    
  /* clear stuff */
  freeSourceList(question->Sources);
  question->Sources = NULL;

  freeDocList(question->RelevantDocuments);
  question->RelevantDocuments = NULL;

  freeDocList(question->ResultDocuments);
  question->ResultDocuments = NULL;

  strcpy(question->keywords, "");

  /* read the slots: */
  while(TRUE){
    short check_result = ReadSymbol(temp_string, file, MAX_SYMBOL_SIZE);
    if(END_OF_STRUCT_OR_LIST == check_result) break;
    if(FALSE == check_result){
      return(false);
    } 

#ifdef DEBUG
    fprintf(stderr, "Question slot: '%s'\n", temp_string); /* for debugging */
#endif

    if(0 == strcmp(temp_string, ":version")) {
      if(FALSE == ReadLong(file, &version))
	return(false);
      question->version = (short)version;
    }
    else if(0 == strcmp(temp_string, ":sort-results-by")) {
      SkipObject(file);
    }
    else if(0 == strcmp(temp_string, ":window-geometry")) {
      SkipObject(file);
    }
    else if(0 == strcmp(temp_string, ":seed-words")) {
      if(FALSE == ReadString(temp_string, file, 5000))
	return(false);
      strcpy(question->keywords, temp_string);
    }
    else if(0 == strcmp(temp_string, ":seed-words-used")) {
      if(FALSE == ReadString(temp_string, file, 5000))
	return(false);
      question->keywords_used = s_strdup(temp_string);
    }
    else if(0 == strcmp(temp_string, ":relevant-documents")) {
      freeDocList(question->RelevantDocuments);
      question->RelevantDocuments = ReadListOfDocuments(file);
    }
    else if(0 == strcmp(temp_string, ":sourcepath")) {
      if(FALSE == ReadString(temp_string, file, 5000))
	return(false);
      if(question->sourcepath != NULL) s_free(question->sourcepath);
      question->sourcepath = s_strdup(temp_string);
    }
    else if(0 == strcmp(temp_string, ":sources")) {
      freeSourceList(question->Sources);
      question->Sources = ReadListOfSources(file);
    }
    else if(0 == strcmp(temp_string, ":result-documents")) {
      freeDocList(question->ResultDocuments);
      question->ResultDocuments = ReadListOfDocuments(file);
    }
    else if(0 == strcmp(temp_string, ":maximum-results")) {
      if(!ReadLong(file, (long*)&question->maxresdocs)) return(FALSE);
    }
    else
      SkipObject(file);
  }

  return(TRUE);
}

void WriteQuestionfp(fp, question)
FILE *fp;
Question question;
{
  DocList doc;
  SourceList sources;

  WriteNewline(fp);
  WriteStartOfStruct("question", fp);
  WriteNewline(fp);
  WriteSymbol(":version", fp);
  WriteLong(2, fp);
  WriteNewline(fp);
  WriteSymbol(":seed-words", fp);
  WriteString(question->keywords, fp);
  WriteNewline(fp);
  if(question->keywords_used != NULL) {
    WriteSymbol(":seed-words-used", fp);
    WriteString(question->keywords_used, fp);
    WriteNewline(fp);
  }
  WriteSymbol(":relevant-documents", fp);
  WriteNewline(fp);
  WriteStartOfList(fp);
  for(doc = question->RelevantDocuments; doc != NULL; doc = doc->nextDoc)
    WriteDocument(doc->thisDoc, fp);
  WriteEndOfList(fp);
  WriteNewline(fp);
  WriteSymbol(":sourcepath", fp);
  WriteString(question->sourcepath, fp);
  WriteNewline(fp);
  WriteSymbol(":sources", fp);
  WriteNewline(fp);
  WriteStartOfList(fp);
  for(sources = question->Sources; sources != NULL; sources = sources->nextSource) {
    WriteStartOfStruct("source-id", fp);
    WriteNewline(fp);
    WriteSymbol(":filename", fp);
    WriteString(sources->thisSource->filename, fp);
    WriteNewline(fp);
    WriteEndOfStruct(fp);
  }
  WriteNewline(fp);
  WriteEndOfList(fp);
  WriteNewline(fp);
  WriteSymbol(":maximum-results", fp);
  WriteLong(question->maxresdocs, fp);
  WriteNewline(fp);
  WriteSymbol(":result-documents", fp);
  WriteNewline(fp);
  WriteStartOfList(fp);
  for(doc = question->ResultDocuments; doc != NULL; doc = doc->nextDoc)
    WriteDocument(doc->thisDoc, fp);
  WriteEndOfList(fp);
  WriteNewline(fp);
  WriteEndOfList(fp);
}

void WriteQuestion(filename, question)
char *filename;
Question question;
{
  FILE *fp;

  /* test to see if it exists */

  if ((fp = fopen(filename, "w")) == NULL) {
    char outstring[STRINGSIZE];
    sprintf(outstring, "Error opening %s.\n", filename);
    PrintStatus(STATUS_URGENT, STATUS_HIGH,outstring);
    return;
  }
  
  WriteQuestionfp(fp, question);
  fclose(fp);
  question->modified = FALSE;
}
