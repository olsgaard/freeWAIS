/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* This is a simple shell user interface for generating wprot packets.
 * -brewster 7/90
 *
 * $Log: waissearch.c,v $
 * Revision 2.0.1.2  1995/09/15 09:55:57  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.1  1995/09/11  13:01:29  pfeifer
 * patch4: Changed TELL_USER to call getuser_name().
 *
 * Revision 2.0  1995/09/08  08:05:48  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.9  1995/05/03  12:24:07  pfeifer
 * getenv("USER") fix
 *
 * Revision 1.8  1994/10/06  13:28:06  pfeifer
 * Patch from Mariusz Niewczas
 *
 * Revision 1.7  1994/09/27  17:27:30  pfeifer
 * Steve Hsieh: 100000 -> BUFSZ
 *
 * Revision 1.6  1994/09/07  13:31:13  pfeifer
 * ctype is now included from cdialect.h after inclusion of string.h.
 * ds, inclusion
 * of string.h after that caused probems
 *
 * Revision 1.5  1994/08/08  07:33:09  pfeifer
 * Moved wais_log_file_name and waislogfile to cutil.[ch]
 *
 * Revision 1.3  1994/04/06  23:55:24  pfeifer
 * 08, autoconf
 *
 * Revision 1.2  1994/03/11  09:13:19  pfeifer
 * some fixes ?
 *
 * Revision 1.3  93/07/02  19:26:46  warnock
 * fix call to getdomainname from mike@tab00.larc.nasa.gov
 * 
 * Revision 1.2  93/07/01  19:22:15  warnock
 * gethostname -> mygethostname
 * 
 * Revision 1.1  93/06/23  20:02:19  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.34  92/06/03  17:29:49  jonathan
 * Fixed so it exits on NULL input.
 * 
 * Revision 1.33  92/04/29  13:21:12  jonathan
 * Updated for release.
 * 
 * Revision 1.32  92/04/09  14:19:53  morris
 * now, specifying a port on the command line does a remote search on the
 * local machine at the specified port.  the previous behavior was to do a
 * local search, even when you specify a port.
 * 
 * Revision 1.31  92/03/17  14:39:28  jonathan
 * modified to work with new ui scheme.
 * 
 * Revision 1.30  92/03/06  14:50:34  jonathan
 * Correctly handle init_connection <= 0.
 * 
 * Revision 1.29  92/03/06  12:34:57  jonathan
 * Use result of init_connection to set message length.
 * 
 * Revision 1.28  92/02/29  20:04:32  jonathan
 * Fixed spelling of opening.
 * 
 * Revision 1.27  92/02/21  11:47:48  jonathan
 * fixed code to re-init properly (instead of using connect_to_server).
 * 
 * Revision 1.26  92/02/20  16:13:15  jonathan
 * Added -m for max results.
 * 
 * Revision 1.25  92/02/16  12:13:36  jonathan
 * Added domainname to init message.
 * 
 *
 * Important functions:
 *   display_search_response
 *   main
 *
 */

/* to do:
 *  fix the number parser in view document number
 *  fix if <cr> put to view document number, same as q
 *  show the index again after viewing a document
 *  do a 'more' type thing on viewing the doc  
 *  do something to allow relevance feedback
 */

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ui/RCS/waissearch.c,v 2.0.1.2 1995/09/15 09:55:57 pfeifer Exp $";
#endif

#include <ui.h>
#include <sockets.h>
#include <docid.h>
/* Security patch from mike@tab00.larc.nasa.gov */
#include <cdialect.h>
/*#include <sys/types.h>*/

#define MAX_MESSAGE_LEN BUFSZ
#define CHARS_PER_PAGE 10000 /* number of chars retrieved in each request */
#define MAX_FILE_NAME_LEN 1000
#define Z39_50_SERVICE "210"

#define WAISSEARCH_DATE "Thu Aug 27 1992"

/* modified from Jonny G's version in ui/question.c */
void showDiags(d)
diagnosticRecord **d;
{
  long i;

  for (i = 0; d[i] != NULL; i++) {
    if (d[i]->ADDINFO != NULL) {
      printf("Code: %s, %s\n", d[i]->DIAG, d[i] ->ADDINFO);
    }
  }
}

/* modified from tracy shen's version in wutil.c
 * displays either a text record of a set of headlines.
 */
void
display_search_response(response)
SearchResponseAPDU *response;

{
  WAISSearchResponse  *info;
  long continue_viewing;
  long i, k;

  printf("\n Search Response:\n");

  printf("  NumberOfRecordsReturned: %d\n", 
	 response->NumberOfRecordsReturned); 
  if ( response->DatabaseDiagnosticRecords != 0 ) {
    info = (WAISSearchResponse *)response->DatabaseDiagnosticRecords;
    i =0; 
    continue_viewing = 1; 

    if (info->Diagnostics != NULL)
      showDiags(info->Diagnostics);

    if ( info->DocHeaders != 0 ) {
      k =0;
      while ( (continue_viewing == 1) && info->DocHeaders[k] != 0 ) {
	i++;
	printf("  %2d: Score: %4ld, lines:%4ld '%s'\n", 
	       i, (info->DocHeaders[k]->Score),
	       info->DocHeaders[k]->Lines,
	       trim_junk(info->DocHeaders[k]->Headline));
	k++;
      }
    }
    if ( info->ShortHeaders != 0 ) {
      k =0;
      while ( (continue_viewing == 1) && info->ShortHeaders[k] != 0 ) {
	i++;
	printf("\n    ShortHeader record %2d, (can't display)", i);
      }
    }
    if ( info->LongHeaders != 0 ) {
      k =0;
      while ( (continue_viewing == 1) && (info->LongHeaders[k] != 0) ) {
	i++;
	printf("\n    Longheader record %2d, (cant display) ", i);
	/* dsply_long_hdr_record( info->LongHeaders[k++]); */
      }
    }
    if ( info->Text != 0 ) {
      k =0;
      while ( (continue_viewing == 1) && (info->Text[k] != 0) ) {
	i++;
	printf("\n    Text record %2d, ", i);
	display_text_record_completely( info->Text[k++], false);
      }
    }
    if ( info->Headlines != 0 ) {
      k =0;
      while ( (continue_viewing ==1) && (info->Headlines[k] != 0) ) {
	i++;
	printf("\n    headline record %2d, (cant display) ", i);
	/* dsply_headline_record( info->Headlines[k++]); */
      }
    }
    if ( info->Codes != 0 ) {
      k =0;
      while ( (continue_viewing ==1) && (info->Codes[k] != 0) ) {
	i++;
	printf("\n    code record %2d, (dont knowhow to display) ", i);
	/* dsply_code_record( info->Codes[k++]); */
      }
    }
  }				/* display user info */
}



#define MAX_KEYWORDS_LENGTH 1000
#define MAX_SERVER_LENGTH 1000
#define MAX_DATABASE_LENGTH 1000
#define MAX_SERVICE_LENGTH 1000

void
main(argc, argv)
int argc;
char *argv[];
{
  char userInfo[500];
  char* request_message = NULL; /* arbitrary message limit */
  char* response_message = NULL; /* arbitrary message limit */
  long request_buffer_length;	/* how of the request is left */
  SearchResponseAPDU  *query_response;
  SearchResponseAPDU  *retrieval_response;
  WAISSearchResponse  *query_info, *retrieval_info;
  char keywords[MAX_KEYWORDS_LENGTH + 1];
  char server_name[MAX_SERVER_LENGTH + 1];	
  char service[MAX_SERVICE_LENGTH + 1];
  char database[MAX_DATABASE_LENGTH + 1];
  char *next_argument, *command_name;
  long count, Max_Docs = 40, message_length = MAX_MESSAGE_LEN;
  FILE *connection;

#ifdef THINK_C
  argc = ccommand(&argv);
#endif

  next_argument = next_arg(&argc, &argv);
  command_name = next_argument;

  if(0 == argc){		/* no args */
    printf("Usage: %s\n", command_name);
    printf("       [-h host-machine]    /* defaults to localhost */\n");
    printf("       [-p service-or-port] /* defaults to z39_50 */\n");
    printf("       [-d database]        /* defaults to nil */\n");
    printf("       [-m maximum_results] /* defaults to 40 /*\n");
    printf("       [-v]                 /* print the version */\n");
    printf("       word word...\n");
    exit(0);
  }
  
#ifdef THINK_C
  strncpy(server_name,"wais:System Folder:wais-index:index",MAX_SERVER_LENGTH);
#else
  server_name[0] = '\0';  /* null it out */
#endif	/* THINK_C */

  database[0] = '\0';  /* null it out */
  service[0] = '\0';

  if(NULL == (next_argument = next_arg(&argc, &argv))){
    printf("No arguments specified\n");
    exit(0);
  }
  while('-' == next_argument[0]){
    /* then we have an argument to process */
    if(0 == strcmp("-debug", next_argument)){
      waislogfile = stderr;
    }
    else if(0 == strcmp("-h", next_argument)){
      if(NULL == (next_argument = next_arg(&argc, &argv))){
	printf("Expected a hostname\n");
	exit(0);
      }
      strncpy(server_name, next_argument, MAX_SERVER_LENGTH);
    }
    else if((0 == strcmp("-s", next_argument)) || /* -s is for backcompatibility */
	    (0 == strcmp("-p", next_argument))){
      if(NULL == (next_argument = next_arg(&argc, &argv))){
	printf("Expected a service name or portname\n");
	exit(0);
      }
      strncpy(service, next_argument, MAX_SERVICE_LENGTH);
    }	
    else if(0 == strcmp("-d", next_argument)){
      if(NULL == (next_argument = next_arg(&argc, &argv))){
	printf("Expected a database name\n");
	exit(0);
      }
      strncpy(database, next_argument, MAX_DATABASE_LENGTH);
    }
    else if(0 == strcmp("-m", next_argument)){
      if(NULL == (next_argument = next_arg(&argc, &argv))){
	printf("Expected a number\n");
	exit(0);
      }
      Max_Docs = atoi(next_argument);
    }
    else if(0 == strcmp("-v", next_argument)){
      printf("%s version: %s, %s\n",
	     command_name, VERSION_STRING, WAISSEARCH_DATE);
    }
    else{
      panic("Don't recognize the %s option", next_argument);
    }
    if(NULL == (next_argument = next_arg(&argc, &argv))){
      printf("No search words specified\n");
      exit(0);
    }
  }
  /* collect up the words for the query */
  strncpy(keywords, next_argument, MAX_KEYWORDS_LENGTH);
  while(NULL != (next_argument = next_arg(&argc, &argv))){
    strncat(keywords, " ", MAX_KEYWORDS_LENGTH);
    strncat(keywords, next_argument, MAX_KEYWORDS_LENGTH);
  }
    
  
  if (server_name[0] == '\0' && service[0] == '\0')
    connection = NULL; /* do local searching */
  else /* remote search, fill in defaults if necessary */
   { if (server_name[0] == '\0')
       mygethostname(server_name,MAX_SERVER_LENGTH); /*default to local machine*/
     if (service[0] == '\0')
       strcpy(service, Z39_50_SERVICE); /* default */
     if ((connection = connect_to_server(server_name,atoi(service))) == NULL) 
      {	fprintf (stderr, "Error opening connection to %s via service %s.\n",
		 server_name, service);
	exit(-1);
      }
   }

  /* init logging if we want to.  this is just for timing studies */
  read_environment_variables(server_name, service);

  request_message = (char*)s_malloc((size_t)message_length * sizeof(char));
  response_message = (char*)s_malloc((size_t)message_length * sizeof(char));

  {
    char hostname[80];
/*    char domainname[80]; */
    char *domainname;
    struct hostent *my_host_ent;

    mygethostname(hostname, 80);
/* Security patch from mike@tab00.larc.nasa.gov */
    if (((my_host_ent = gethostbyname(hostname)) == NULL) ||
	(domainname = (char *)index(my_host_ent->h_name, '.')) == NULL)
      domainname = "unknown";
    else
      domainname++;
/*    getdomainname(domainname, 80); */

#ifdef TELL_USER
    sprintf(userInfo, "waissearch %s, from host: %s.%s, user: %s",
	    VERSION_STRING, hostname, domainname, getuser_name());
#else
    sprintf(userInfo, "waissearch %s, from host: %s.%s", 
	    VERSION_STRING, hostname, domainname);
#endif

    if((message_length = 
	init_connection(request_message, response_message,
			message_length,
			connection,
			userInfo)) <= 0) {
      fprintf (stderr, "Error opening connection to %s via service %s.\n",
	       server_name, service);
      exit(-1);
    }
  }

  while(1){		/* continue to search until the user gets tired */
    request_buffer_length = message_length; /* how of the request is left */
    if(NULL ==
	generate_search_apdu(request_message + HEADER_LENGTH, 
			     &request_buffer_length, 
			     keywords, database, NULL, Max_Docs)) {
      printf("Error creating search APDU: request too large");
      break;
    }

    if(0 ==
       interpret_message(request_message, 
			 message_length - request_buffer_length, 
			 response_message,
			 message_length,
			 connection,
			 false	/* true verbose */
			 )) { /* perhaps the server shut down on us, let's see: */
      if ( connection != NULL) {
	fclose(connection);
	if ((message_length = 
		  init_connection(request_message, response_message,
				  message_length,
				  connection,
				  userInfo)) <= 0)
	  {
	    fprintf (stderr, "Error opening connection to %s via service %s.\n",
		     server_name, service);
	    exit(-1);
	  }
	if(0 ==
	   interpret_message(request_message, 
			     message_length - request_buffer_length, 
			     response_message,
			     message_length,
			     connection,
			     false /* true verbose */
			     ))
	  panic("really couldn't deliver message");
	}
      else
	panic("returned message too large");
    }

    readSearchResponseAPDU(&query_response, response_message + HEADER_LENGTH);
    display_search_response(query_response);
    query_info = (WAISSearchResponse *)query_response->DatabaseDiagnosticRecords;

    if ( query_response->DatabaseDiagnosticRecords != 0 &&
	query_info->DocHeaders !=0) {
      long document_number = -1;
      any* lastDocID = NULL;

      while(1){			/* keep viewing until user wants out */
	while(1){		/* keep asking until we have an answer */
	  char document_string[1000]; /* to hold the users response */
	  long count;

	  if(document_number>0 && 
	     document_number<query_response->NumberOfRecordsReturned)
	    lastDocID = query_info->DocHeaders[document_number-1]->DocumentID;

	  printf("\nView document number [type 0 or q to quit]: ");
	  fflush(stdout);
/* tung */
/*	  if(NULL == fgets(document_string, 1000, stdin)) {
	    close_connection(connection);
	    exit(-1);
	  } */
          fgets(document_string, 1000, stdin);
          if(document_string[0] == '\0') {
            close_connection(connection);
	    exit(-1);
	  }
/* tung */
	  /* trim \n from string */
	  if(strlen(document_string) > 0)
	    document_string[strlen(document_string) -1] = '\0';

	  if(document_string[0] == 'q'){
	    document_number = 0; /* signal to quit */
	    break;
	  }
	  if(document_string[0] == 'n'){
	    if(NULL == lastDocID){
	      printf("A document first must be selected\n");
	    }
	    else{
	      document_number = -3; /* signal to get next document */
	      break;
	    }
	  }
	  if(document_string[0] == 'p'){
	    if(NULL == lastDocID){
	      printf("A document first must be selected\n");
	    }
	    else{
	      document_number = -2; /* signal to get previous document */
	      break;
	    }
	  }
	  /* check for a number */
	  for(count = 0; count < strlen(document_string); count++){
	    if(!isdigit(document_string[count])){
	      document_number = -1; /* ask again */
	      break;		/* break out of for loop */
	    }
	  }
	  if (count == strlen(document_string)){
	    /* it is a legal number */
	    document_number = atol(document_string);
	  }
	  if(document_number >= 0 &&
	     document_number <= query_response->NumberOfRecordsReturned){
	    break;		/* correct entry */
	  }
	  printf("\nEntry must be a number between 0 and %ld [type 0 or q to quit] you entered \"%s\"\n", query_response->NumberOfRecordsReturned, document_string);
	  
	}
	if(document_number == 0){
	  break;		/* leave the viewing loop */
	}
	if(document_number < -1) { /* handle next and previous */
	  char *type;
	  DocObj *Doc[2];
	  	    
	  if(document_number == -3)
	    type = "WAIS_NEXT";
	  else if(document_number == -2)
	    type = "WAIS_PREV";
	  request_buffer_length = message_length; /* how of the request is left */
	  Doc[0] =
	    makeDocObjUsingWholeDocument(lastDocID, type);
	  Doc[1] = NULL;

	  if(0 ==
	     generate_search_apdu(request_message + HEADER_LENGTH, 
				  &request_buffer_length, 
				  "foo", database, Doc, 1)) {
	    panic("request too long");
	  }
	  if(0 ==
	     interpret_message(request_message, 
			       message_length - request_buffer_length, 
			       response_message,
			       message_length,
			       connection,
			       false /* true verbose */	
			       )) { /* perhaps the server shut down on us, let's see: */
	    if ( connection != NULL) {
	      fclose(connection);
	      if (0 == init_connection(request_message, response_message,
				       message_length,
				       connection,
				       userInfo))
		{
		  fprintf (stderr, "Error opening connection to %s via service %s.\n",
			   server_name, service);
		  exit(-1);
		}
	      if(0 ==
		 interpret_message(request_message, 
				   message_length - request_buffer_length, 
				   response_message,
				   message_length,
				   connection,
				   false /* true verbose */
				   ))
		panic("really couldn't deliver message");
	    }
	    else
	      panic("returned message too large");
	  }

	    readSearchResponseAPDU(&retrieval_response, 
				   response_message + HEADER_LENGTH);

	    retrieval_info = (WAISSearchResponse *)retrieval_response->DatabaseDiagnosticRecords;

	    if ( retrieval_info != NULL &&
		retrieval_info->DocHeaders != NULL) {
	      char *type;
	      long size;

	    lastDocID = duplicateAny(retrieval_info->DocHeaders[0]->DocumentID);
	    if(retrieval_info->DocHeaders[0]->Types == NULL)
	      type = s_strdup("TEXT");
	    else
	      type = s_strdup(retrieval_info->DocHeaders[0]->Types[0]);
	    printf("Headline: %s\n", 
		   retrieval_info->DocHeaders[0]->Headline);
	    size = retrieval_info->DocHeaders[0]->DocumentLength;
	    for(count = 0; 
		count * CHARS_PER_PAGE < size;
		count++){
	      request_buffer_length = message_length; /* how of the request is left */
	      if(0 ==
		 generate_retrieval_apdu(request_message + HEADER_LENGTH,
					 &request_buffer_length, 
					 lastDocID,
					 CT_byte,
					 count * CHARS_PER_PAGE,
					 MINIMUM((count + 1) * CHARS_PER_PAGE, size),
					 type,
					 database
					 )) {
		printf("Error generating retrieval APDU: request too long");
		break;
	      }
	      if(0 ==
		 interpret_message(request_message, 
				   message_length - request_buffer_length, 
				   response_message,
				   message_length,
				   connection,
				   false /* true verbose */	
				   )) { /* perhaps the server shut down on us, let's see: */
		if ( connection != NULL) {
		  fclose(connection);
		  if ((connection=connect_to_server(server_name,atoi(service))) == NULL) 
		    {
		      fprintf (stderr, "Error opening connection to %s via service %s.\n",
			       server_name, service);
		      exit(-1);
		    }
		  if(0 ==
		     interpret_message(request_message, 
				       message_length - request_buffer_length, 
				       response_message,
				       message_length,
				       connection,
				       false /* true verbose */
				       )) {
		    printf("really couldn't deliver message");
		    break;
		  }
		}
		else {
		  printf("returned message too large");
		  break;
		}
	      }

	      readSearchResponseAPDU(&retrieval_response, 
				     response_message + HEADER_LENGTH);

	      /* display_search_response(retrieval_response); the general thing */
	      if(NULL == ((WAISSearchResponse *)retrieval_response->DatabaseDiagnosticRecords)->Text){
		display_search_response(retrieval_response);
		printf("No text was returned");
		break;
	      }
	      display_text_record_completely
		(((WAISSearchResponse *)retrieval_response->DatabaseDiagnosticRecords)->Text[0], false);
	    }
	  }
	  freeWAISSearchResponse( retrieval_response->DatabaseDiagnosticRecords); 
	  freeSearchResponseAPDU( retrieval_response);
	}
	else {
	  printf("Headline: %s\n", 
		 query_info->DocHeaders[document_number - 1]->Headline);
	  /* we must retrieve the document in parts since it might be very long*/
	  for(count = 0; 
	      (query_info->DocHeaders[document_number - 1]->DocumentLength == 0) ||
	      (count * CHARS_PER_PAGE <
	      query_info->DocHeaders[document_number - 1]->DocumentLength);
	      count++){
	    char *type;
	    if(query_info->DocHeaders[document_number - 1]->Types == NULL)
	      type = s_strdup("TEXT");
	    else
	      type = s_strdup(query_info->DocHeaders[document_number - 1]->Types[0]);
	    request_buffer_length = message_length; /* how of the request is left */
	    if(0 ==
	       generate_retrieval_apdu(request_message + HEADER_LENGTH,
				       &request_buffer_length, 
				       query_info->DocHeaders[document_number - 1]->DocumentID, 
				       CT_byte,
				       count * CHARS_PER_PAGE,
				       (query_info->DocHeaders[document_number - 1]->DocumentLength ? 
					MINIMUM((count + 1) * CHARS_PER_PAGE,
					    query_info->DocHeaders[document_number - 1]->DocumentLength) :
					(count + 1) * CHARS_PER_PAGE),
				       type,
				       database
				       )) {
	      printf("Error generating retrieval APDU: request too long");
	      break;
	    }
	     
	    if(0 ==
	       interpret_message(request_message, 
				 message_length - request_buffer_length, 
				 response_message,
				 message_length,
				 connection,
				 false /* true verbose */	
				 )) { /* perhaps the server shut down on us, let's see: */
	      if ( connection != NULL) {
		fclose(connection);
		if ((connection=connect_to_server(server_name,atoi(service))) == NULL) 
		  {
		    fprintf (stderr, "Error opening connection to %s via service %s.\n",
			     server_name, service);
		    exit(-1);
		  }
		if(0 ==
		   interpret_message(request_message, 
				     message_length - request_buffer_length, 
				     response_message,
				     message_length,
				     connection,
				     false /* true verbose */
				     )) {
		  printf("really couldn't deliver message");
		  break;
		}
	      }
	      else {
		printf("Error deliverring message");
		break;
	      }
	    }

	    readSearchResponseAPDU(&retrieval_response, 
				   response_message + HEADER_LENGTH);

	    /* display_search_response(retrieval_response); the general thing */
	    if(NULL == ((WAISSearchResponse *)retrieval_response->DatabaseDiagnosticRecords)->Text){
	      display_search_response(retrieval_response);
	      printf("No text was returned");
	      break;
	    }
	    display_text_record_completely
	      (((WAISSearchResponse *)retrieval_response->DatabaseDiagnosticRecords)->Text[0], false);
	    if(((WAISSearchResponse *)retrieval_response->DatabaseDiagnosticRecords)->Diagnostics != NULL) {
	      showDiags(((WAISSearchResponse *)retrieval_response->DatabaseDiagnosticRecords)->Diagnostics);
	      break;
	    }
	  }
	  freeWAISSearchResponse( retrieval_response->DatabaseDiagnosticRecords); 
	  freeSearchResponseAPDU( retrieval_response);
	}
      }
    }

    freeWAISSearchResponse(query_response->DatabaseDiagnosticRecords);         
    freeSearchResponseAPDU( query_response);
    printf("Search for new words [type q to quit]: ");
    fflush(stdout);
#if 0
    if(NULL == gets(keywords) ||
       (strlen(keywords) == 1 && keywords[0] == 'q')){
      close_connection(connection);
      break;			/* leave the asking loop */
    }
#endif
    fgets(keywords,MAX_KEYWORDS_LENGTH,stdin);
    if(keywords == NULL ||
       (strlen(keywords) <= 3 && keywords[0] == 'q')){
      close_connection(connection);
      break;			/* leave the asking loop */
    }
    
  }

  s_free(request_message);
  s_free(response_message);

  exit(0);
}
