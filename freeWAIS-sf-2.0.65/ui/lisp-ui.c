/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */



/* This is a simple wais interface for lisp clients.
 * It takes in lisp-y looking requests and sends out Z39.50 packets.
 *
 * -brewster 7/90
 *
 * Important functions:
 *   display_search_response
 *   main
 */

#include <stdio.h>
#include <ctype.h>
#include "../ir/cdialect.h"
#include "../ir/ui.h"

#define CHARS_PER_PAGE 10000 /* number of chars retrieved in each request */
#define MAX_MESSAGE_LEN BUFSZ
#define VERBOSE false /* true */

/****************************************************************
 *
 *     	Function Name:  display_search_response
 *
 *
 * 	Purpose: displays the formatted list of documents in the form  
 *               that the super-text-server would like to see. 
 *
 * 	Returns: nothing.
 *
 *	Algorithm:   . ...
 *
 */


/* modified from tracy shen's version in wutil.c
 * displays a set of headlines.
 */
void
display_search_response(response,database,server_name,service_name)
SearchResponseAPDU *response;
char *database;
char *server_name;
char *service_name;
{
  WAISSearchResponse  *info;
  long k;
  printf("(:query (");

  if ( response->DatabaseDiagnosticRecords != 0 ) {
    info = response->DatabaseDiagnosticRecords;
    if(info->DocHeaders != 0){
      k =0;
      while(info->DocHeaders[k] != 0 ){       
	char doc_id[1000];
	strncpy(doc_id,	info->DocHeaders[k]->DocumentID->bytes,
		MINIMUM(info->DocHeaders[k]->DocumentID->size, 1000));
	doc_id[MINIMUM(info->DocHeaders[k]->DocumentID->size, 1000 - 1)] 
	  = '\0';
	printf("(:doc-id \"%s\" :document-length %ld :types (\"%s\") :database-name \"%s\" :server-name \"%s\" :score %ld :service \"%s\" :header \"",
	       doc_id,
	       info->DocHeaders[k]->DocumentLength,
	       (info->DocHeaders[k]->Types == NULL)?"TEXT":info->DocHeaders[k]->Types[0],
	       database,
	       server_name,
	       info->DocHeaders[k]->Score,
	       service_name
	       );
	{ char *header = trim_junk(info->DocHeaders[k]->Headline);
	  long i;
	  for(i=0; i < strlen(header); i++){
	    char ch = header[i];
	    if(ch == '"')
	      putc('\\', stdout);
	    putc(ch, stdout);
	  }
	}
	printf("\")");
	k++;
      }
    }
  }
  printf(")\n:end t)\n");	/* finish it */
  fflush(stdout);
}


/*******************
 *     Parsing     *
 *******************/

/* In a alist of keys and values,
 * this finds the string value associated with a key.
 * The way this is implemented is a kludge since it searches for
 * the key with string search rather than making a list and
 * searching down it.  This can error if a value contains a string version
 * of the key.
 * What we really need is list facilities.
 *
 * Side effects value.
 * Returns 0 if it wins, 1 if there is no such slot, -1 if error.
 */

/* these are states of the parser */
#define BEFORE 1
#define DURING 2
#define QUOTE 5

long find_string_slot(source,key,value,value_size,delete_internal_quotes)
char *source;
char *key;
char *value;
long value_size;
boolean delete_internal_quotes;
{
  char ch;
  short state = BEFORE;
  long position = 0;  /* position in value */
  char *pos =strstr(source, key); /* address into source */

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

/* reads a long int a file returns true if successful, false otherwise */
long find_long_slot(source,key,answer)
char *source;
char *key;
long *answer;
{
  char ch;
  short state = BEFORE;
  char *pos =strstr(source, key); /* address into source */
  long count = 0;
  boolean isNegative = false;
  *answer = 0;

  if(NULL == pos)
    return(1);

  for(pos = pos + strlen(key); pos < source + strlen(source); pos++){
    ch = *pos;
    if (isdigit(ch)){
      if(state == BEFORE){
	state = DURING;
      }
      count++;
      if(count == 12){
	/* then we have an error in the file, 32 bit numbers can not be more
	   than 10 digits long */
	return(-1);
      }
      *answer = *answer * 10 + (ch - '0');
    }
    else if (ch == '-') {
      if (isNegative)
	/* then we have an error since there should be only one - in a number */
	return(-1);
      if (state == BEFORE) {
	/* we are ok since the - must come before any digits */
	isNegative = true;
	state = DURING;
      }
      else {
	break;			/* we are done */
      }
    }
    else if(!isspace(ch)){
      /* then we have an error since it should be a digit or a space */
      return(-1);
    }
    /* we do not have an digit */
    else if(state == DURING){
      break;			/* we are done */
    }
    /* otherwise we are still before the start */
  }
  if (isNegative)
    *answer *= -1;
  return(0);
}



/************
 *   MAIN   *
 ************/

#define MAX_QUERY_SIZE 10000
#define MAX_NAME_SIZE 1000
void main()
{
  char query[MAX_QUERY_SIZE + 1];
  char request_message[MAX_MESSAGE_LEN]; /* arbitrary message limit */
  char response_message[MAX_MESSAGE_LEN]; /* arbitrary message limit */
  long request_buffer_length;	/* how of the request is left */
  SearchResponseAPDU  *query_response;
  SearchResponseAPDU  *retrieval_response;
  char service[MAX_NAME_SIZE + 1];
  char database[MAX_NAME_SIZE + 1];
  char server_name[MAX_NAME_SIZE + 1];
  char type[MAX_NAME_SIZE + 1];

  server_name[0] = '\0';	/* null it out */
  database[0] = '\0';		/* null it out */
  service[0] = '\0';		/* null it out */

  while(TRUE){
    /* continue to search until the user gets tired */

    fgets(query, MAX_QUERY_SIZE, stdin);
    if(NULL != strstr(query, ":fill-in") ||
       NULL != strstr(query, ":query")){
      /* then we have a fill-in or query. 
	 extract the common fields */

      if(0 > find_string_slot(query, ":database", database,
			      MAX_NAME_SIZE, false)){
	panic("Could not read the database from query %s", query);
      }
      if(0 > find_string_slot(query, ":server", server_name,
			      MAX_NAME_SIZE, false)){
	panic("Could not read the host from query %s", query);
      }
      if(0 > find_string_slot(query, ":service", service,
			      MAX_NAME_SIZE, false)){
	panic("Could not read the service from query %s", query);
      }
    }

    if(NULL != strstr(query, ":fill-in")){
      /* then we have a fill-in request */
      long count;
      long document_length;
      char doc_id[MAX_NAME_SIZE + 1];
      any doc_id_any;
      if(0 != find_string_slot(query, ":doc-id", doc_id,
			       MAX_NAME_SIZE, false)){
	panic("Could not read the doc-id from request %s", query);
      }
      doc_id_any.bytes = doc_id;
      doc_id_any.size = strlen(doc_id);

      if(0 != find_long_slot(query, ":document-length", &document_length)){
	panic("Could not read the document length from doc_id %s", query);
      }
      if(0 != find_string_slot(query, ":type", type,
			       MAX_NAME_SIZE, false)){
	panic("Could not read the type from request %s", query);
      }

      printf("(:fill-in (:text \""); /* start the reply */
      for(count = 0; 
	  count * CHARS_PER_PAGE < document_length;
	  count++){
	request_buffer_length = MAX_MESSAGE_LEN; /* how of the request is left */
	if(NULL ==
	   generate_retrieval_apdu(request_message + HEADER_LENGTH,
				   &request_buffer_length, 
				   &doc_id_any,
				   CT_byte,
				   count * CHARS_PER_PAGE,
				   MINIMUM((count + 1) * CHARS_PER_PAGE, 
				       document_length),
				   type,
				   database
				   ))
	  panic("query was too large");
	
	if(0 ==
	   interpret_message(request_message, 
			     MAX_MESSAGE_LEN - request_buffer_length, 
			     response_message,
			     MAX_MESSAGE_LEN,
			     server_name, 
			     service, 
			     VERBOSE
			     ))
	  panic("return message too large");

	readSearchResponseAPDU(&retrieval_response, 
			       response_message + HEADER_LENGTH);
	if(NULL == ((WAISSearchResponse *)retrieval_response->DatabaseDiagnosticRecords)->Text)
	  panic("No text was returned");

	display_text_record_completely
	  (((WAISSearchResponse *)retrieval_response->DatabaseDiagnosticRecords)->Text[0], true);
      }
      printf("\")\n:end t)\n");	/* finish the text */
      fflush(stdout);
    }
    else if(NULL != strstr(query, ":query")){
      /* then we have a query */
      char seed_words[MAX_QUERY_SIZE + 1];
      if(0 != find_string_slot(query, ":seed-words", seed_words,
			       MAX_QUERY_SIZE, false)){
	panic("Could not read the seed words from query %s", query);
      }
      request_buffer_length = MAX_MESSAGE_LEN; /* how of the request is left */
      if(NULL ==
	 generate_search_apdu(request_message + HEADER_LENGTH, 
			      &request_buffer_length, 
			      seed_words, database, NULL))
	panic("Query was too large");
	 
      if(0 ==
	 interpret_message(request_message,
			   MAX_MESSAGE_LEN - request_buffer_length, 
			   response_message,
			   MAX_MESSAGE_LEN,
			   server_name, 
			   service,
			   VERBOSE
			   ))
	panic("return message too large");

      readSearchResponseAPDU(&query_response, 
			     response_message + HEADER_LENGTH);
      display_search_response(query_response, database, server_name,
			      service);
    }
    else
      panic("Dont know how to handle query %s\n", query);
  }
}
    
  
/* sample
(:query :seed-words ("food") :database "/u/kahle/wais-index-irn8/foo" :server "gandalf" :service "8000")
*/
