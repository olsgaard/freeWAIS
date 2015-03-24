/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/ui.c,v 2.0.1.2 1995/11/09 14:07:36 pfeifer Exp $";
#endif

/* Change log:
 * $Log: ui.c,v $
 * Revision 2.0.1.2  1995/11/09 14:07:36  pfeifer
 * patch46: added a fix from ui-local.c.
 *
 * Revision 2.0.1.1  1995/09/15  09:49:11  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.1  1995/09/15  09:49:11  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:02:20  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.3  1994/05/20  12:57:52  pfeifer
 * beta
 *
 * Revision 1.2  1994/03/08  21:07:06  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.16  92/03/18  09:01:09  jonathan
 * don't free DocObjs[0]->Type, it's a copy of the input!
 * 
 * Revision 1.15  92/03/06  12:53:03  jonathan
 * Plug another memory leak.  From ericb@baker.dartmouth.edu (Eric J Bivona).
 * 
 * Revision 1.14  92/02/14  15:26:53  jonathan
 * Conditionalized interpret_message and locally_answer_message so client need
 * not include search engine.
 * 
 * Revision 1.13  92/02/12  13:53:41  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
 * 
*/

/* 
 * this is a simple ui toolkit for building other ui's on top.
 * -brewster
 * 
 * top level functions:
 *   generate_search_apdu
 *   generate_retrieval_apdu
 *   interpret_message
 *
 */

/* to do:
 *   generate multiple queries for long documents.
 *     this will crash if the file being retrieved is larger than 100k.
 *   
 */

#include "ui.h"
#include "wutil.h"
#include "ustubs.h"
#include "futil.h"

#include <ctype.h>
#include <errno.h>
/* #include <string.h> */
#include "cdialect.h"
#include <sys/types.h>

/* returns a pointer in the buffer of the first free byte.
   if it overflows, then NULL is returned 
 */
char *
generate_search_apdu(buff,
		     buff_len,
		     seed_words,
		     database_name,
		     docobjs,
		     maxDocsRetrieved)
char* buff;     /* buffer to hold the apdu */
long *buff_len;    /* length of the buffer changed to reflect new data written */
char *seed_words;    /* string of the seed words */
char *database_name;
DocObj** docobjs;
long maxDocsRetrieved;
{
  /* local variables */

  SearchAPDU *search3;
  char  *end_ptr;
  static char *database_names[2] = {"", 0};
  any refID;
  WAISSearch *query;
  refID.size = 1;
  refID.bytes = "3";

  database_names[0] = database_name;
  query = makeWAISSearch(seed_words,
                         docobjs, /* DocObjsPtr */
                         0L,
                         1L,     /* DateFactor */
                         0L,     /* BeginDateRange */
                         0L,     /* EndDateRange */
                         maxDocsRetrieved
                         );

  search3 = makeSearchAPDU(30L, 
			   5000L, /* should be large */
			   30L,
                           1L,	/* replace indicator */
                           "",	/* result set name */
                           database_names, /* database name */   
                           QT_RelevanceFeedbackQuery, /* query_type */
                           0L,   /* element name */
                           NULL, /* reference ID */
                           query);

  end_ptr = writeSearchAPDU(search3, buff, buff_len);

  CSTFreeWAISSearch(query);
  freeSearchAPDU(search3);
  return(end_ptr);
}


/* returns a pointer into the buffer of the next free byte.
   if it overflowed, then NULL is returned
 */

char *
 generate_retrieval_apdu(buff,
			buff_len,
			docID,
			chunk_type,
			start,
			end,
			type,
			database_name)
char *buff;
long *buff_len;    /* length of the buffer changed to reflect new data written */
any *docID;
long chunk_type;
long start;
long end;
char *type;
char *database_name;
{
  SearchAPDU *search;
  char  *end_ptr;

  static char *database_names[2];
  static char *element_names[3];
  any refID;

  DocObj *DocObjs[2];
  any *query;			/* changed from char* by brewster */

  if(NULL == type)
    type = s_strdup("TEXT");

  database_names[0] = database_name;
  database_names[1] = NULL;

  element_names[0] = " ";
  element_names[1] = ES_DocumentText;
  element_names[2] = NULL;

  refID.size = 1;
  refID.bytes = "3";
  
  switch(chunk_type){
  case CT_line: 
    DocObjs[0] = makeDocObjUsingLines(docID, type, start, end);
    break;
  case CT_byte:
    DocObjs[0] = makeDocObjUsingBytes(docID, type, start, end);
    break;
  }
  DocObjs[1] = NULL;

  query = makeWAISTextQuery(DocObjs);   
  search = makeSearchAPDU( 10L, 16L, 15L, 
			  1L,	/* replace indicator */
			  "FOO", /* result set name */
			  database_names, /* database name */   
			  QT_TextRetrievalQuery, /* query_type */
			  element_names, /* element name */
			  &refID, /* reference ID */
			  query);
  end_ptr = writeSearchAPDU(search, buff, buff_len);
  /* s_free(DocObjs[0]->Type); it's a copy of the input, don't free it! */
  CSTFreeDocObj(DocObjs[0]);
  CSTFreeWAISTextQuery(query);
  freeSearchAPDU(search);
  return(end_ptr);
}

/* not currently used 

static boolean isnumber _AP((char* string));

static boolean isnumber(string)
char *string;
{
  long count;
  for(count = 0; count < strlen(string); count++){
    if(!isdigit(string[count])){
      return(false);
    }
  }
  return(true);
}

*/

/* this will negotiate with server, and returs the maximum buffer size 
   the server can handle.

   A connection should be established first using open_socket.

*/

long init_connection(inBuffer, outBuffer, bufferSize, connection, userInfo)
char *inBuffer, *outBuffer;
long bufferSize;
FILE *connection;
char *userInfo;
{ 
  InitAPDU* init = NULL;
  InitResponseAPDU* reply = NULL;
  long result;
  /* construct an init */
  init = makeInitAPDU(true,false,false,false,false,bufferSize,bufferSize,
		      userInfo,defaultImplementationID(),
		      defaultImplementationName(),
		      defaultImplementationVersion(),NULL,userInfo);
  /* write it to the buffer */
  result = writeInitAPDU(init,inBuffer+HEADER_LENGTH,&bufferSize) - inBuffer;

  if(result < 0){
    freeInitAPDU(init);
    return(-1);
  }
  if(0 ==
     interpret_message(inBuffer,
		       result - HEADER_LENGTH,
		       outBuffer,
		       bufferSize,
		       connection,
		       false	/* true verbose */	
		       )) {
    /* error making a connection */
    return (-1);
  }
  if (readInitResponseAPDU(&reply,outBuffer + HEADER_LENGTH) == NULL){
    freeWAISInitResponse((WAISInitResponse*)reply->UserInformationField);
    freeInitResponseAPDU(reply);
    return(-1);
  }
  if (reply->Result == false)
    {				/* the server declined service */
      freeWAISInitResponse((WAISInitResponse*)reply->UserInformationField);
      freeInitResponseAPDU(reply);
      return(-1);
    }
  else				/* we got a response back */
    { result = reply->MaximumRecordSize;
      freeWAISInitResponse((WAISInitResponse*)reply->UserInformationField);
      freeInitResponseAPDU(reply);
      return(result);
    }
}

#ifdef LOCAL_SEARCH
/* returns the length of the response, 0 if an error */
long
locally_answer_message(request_message,
		       request_length,
		       response_message,
		       response_buffer_length)
char *request_message;
long request_length;
char *response_message;
long response_buffer_length;
{
  long request_length_internal = request_length;
  long response_length;
  WAISMessage header;
  long maxBufferSize = response_buffer_length;

  readWAISPacketHeader(request_message, &header);
  {
    char length_array[11];
    strncpy(length_array, header.msg_len, 10);
    length_array[10] = '\0';
    request_length_internal = atol(length_array);
  }
  /*
    printf("request length is %ld (%ld from caller)\n", 
    request_length_internal,
    request_length);
    */
  
  response_length =
    interpret_buffer(request_message + HEADER_LENGTH, 
		     request_length_internal,
		     response_message + HEADER_LENGTH,
		     response_buffer_length,
		     &maxBufferSize,
		     (long)header.hdr_vers,
		     "");
  if(0 > response_length)
    return(0);
  writeWAISPacketHeader(response_message,
			response_length,
			(long)'z',	/* Z39.50 */
			"DowQuest  ", /* server name */
			(long)NO_COMPRESSION,	/* no compression */
			(long)NO_ENCODING,(long)header.hdr_vers);
  return(response_length);
}
#endif

/* this is a safe version of unix 'read' it does all the checking
 * and looping necessary
 * to those trying to modify the transport code to use non-UNIX streams:
 *  This is the function to modify!
 */
long read_from_stream(d,buf,nbytes)
long d;				/* this is the stream */
char *buf;
long nbytes;
{
  long didRead;
  long toRead = nbytes;
  long totalRead = 0;		/* paranoia */

  while (toRead > 0){
    didRead = read (d, buf, toRead);
    if(didRead == -1)		/* error*/
      return(-1);
    if(didRead == 0)		/* eof */
      return(-2);		/* maybe this should return 0? */
    toRead -= didRead;
    buf += didRead;
    totalRead += didRead;
  }
  if(totalRead != nbytes)	/* we overread for some reason */
    return(- totalRead);	/* bad news */    
  return(totalRead);
}

/* returns the length of the response, 0 if an error */

long 
transport_message(connection,
		  request_message,
		  request_length,
		  response_message,
		  response_buffer_length)
FILE *connection;
char *request_message;
long request_length;
char *response_message;
long response_buffer_length;
{
  WAISMessage header;
  long response_length;

  
  /* Write out message. Read back header. Figure out response length. */
  
  if( request_length + HEADER_LENGTH
     != fwrite (request_message, 1L, request_length + HEADER_LENGTH, connection))
    return 0;

  fflush(connection);

  /* read for the first '0' */

  while(1){
    if(0 > read_from_stream(fileno(connection), response_message, 1))
      return 0;
    if('0' == response_message[0])
      break;
  }

  if(0 > read_from_stream(fileno(connection), 
				  response_message + 1, 
				  HEADER_LENGTH - 1))
    return 0;

  readWAISPacketHeader(response_message, &header);
  {
    char length_array[11];
    strncpy(length_array, header.msg_len, 10);
    length_array[10] = '\0';
    response_length = atol(length_array);
    /*
      if(verbose){
      printf("WAIS header: '%s' length_array: '%s'\n", 
      response_message, length_array);
      }
      */
    if(response_length > response_buffer_length){
      /* we got a message that is too long, therefore empty the message out,
	 and return 0 */
      long i;
      for(i = 0; i < response_length; i++){
	read_from_stream(fileno(connection), 
			 response_message + HEADER_LENGTH,
			 1);
/*	fread(response_message + HEADER_LENGTH, 1, 1, connection); */
      }
      return(0);
    }
  }
  if(0 > read_from_stream(fileno(connection), 
				  response_message + HEADER_LENGTH,
				  response_length))
/*  if(0 > fread(response_message + HEADER_LENGTH,
	       1, response_length, connection)) */
    return 0;
  return(response_length);
}


/* ------------------------------------------------------------*/

/*  Facility to record messages sent and recieved for testing 
    and timing purposes. */

/* from c:

   putenv(strdup("IR_FILE=/users/menlo-park/brewster/tmp/infile"));

   from csh:

   setenv IR_FILE /users/menlo-park/brewster/tmp/infile

 */

boolean environment_variables_read = false; 
char* ir_file_environment_variable = "IR_FILE";
char* ir_file = NULL;

void read_environment_variables(host, port)
     char* host;
     char* port;
{
  
  if(!environment_variables_read){
    FILE *stream;
    ir_file = (char*)getenv(ir_file_environment_variable);
    if(ir_file){
      printf("IR_file: %s\n", ir_file);
      stream = fopen(ir_file, "w");
      fprintf(stream, "%s %s\n", host, port);
      fclose(stream);
    }
    environment_variables_read = true;
  }
}

/* returns 0 if success */
long write_message_to_file(message, length, filename)
     unsigned char *message;
     long length;
     char*filename;
{
  FILE *stream = fopen(filename, "a");
  if(NULL == stream)
    return(-1);
  
  printf("Writing to file: %s %d characters\n", filename, length);

  fprintf(stream, "---------------------------------\n");
  
  if(length != fwrite(message, sizeof(unsigned char), 
		      length, stream)){
    perror("fwrite error");
    fclose(stream);
    return(-2);
  }

  fprintf(stream, "\n");

  if(0 != fclose(stream))
    return(-3);
  return(0);
}  

/* ------------------------------------------------------------*/

/* returns the number of bytes writeen.  0 if an error */
long
interpret_message(request_message,request_length,
		  response_message,
		  response_buffer_length,
		  connection,
		  verbose)
char *request_message;
long request_length; /* length of the buffer */
char *response_message;
long response_buffer_length;
FILE *connection;
boolean verbose;
{
  long response_length;

  writeWAISPacketHeader(request_message,
			request_length,
			(long)'z',	/* Z39.50 */
			"wais      ", /* server name */
			(long)NO_COMPRESSION,	/* no compression */
			(long)NO_ENCODING,(long)HEADER_VERSION);

  if(ir_file){
    long error_code;
    if(0 != (error_code = 
	     write_message_to_file((unsigned char*)request_message, 
				   request_length + HEADER_LENGTH, 
				   ir_file)))
      printf("Error writing log file Code: %d\n", error_code);
  }
    
  if(connection != NULL) {
    if(0 == 
       (response_length =
	transport_message(connection, request_message,
			  request_length,
			  response_message,
			  response_buffer_length)))
      return(0);
  }
  else{
#ifdef LOCAL_SEARCH    
    if(0 == 
       (response_length =
	locally_answer_message(request_message, request_length, 
			       response_message,
			       response_buffer_length)))
      return(0);
#else
    waislog(WLOG_HIGH, WLOG_ERROR, "Local search not supported in this version");
    return(0);
#endif
  }
  if(verbose){
    printf ("decoded %ld bytes: \n", response_length);
    twais_dsply_rsp_apdu(response_message + HEADER_LENGTH, 
			 request_length);
  }
  if(ir_file){
    long error_code;
    if(0 != (error_code = 
	     write_message_to_file((unsigned char*)response_message, 
				   response_length + HEADER_LENGTH, 
				   ir_file)))
      printf("Error writing log file Code: %d\n", error_code);
  }
  return(response_length);
}

/* this closes the connection to the socket.
 * the mythology is that this is cleaner than exiting
 */

long close_connection(connection)
FILE *connection;
{
  long result = 0;
  
  if(connection != NULL){
    result = fclose(connection);
  }
  return(result);
}
  
void
display_text_record_completely(record,quote_string_quotes)
WAISDocumentText *record;
boolean quote_string_quotes;
{
  long count;
  /* printf(" Text\n");
     print_any("     DocumentID:  ", record->DocumentID);
     printf("     VersionNumber:  %d\n", record->VersionNumber);
     */
  for(count = 0; count < record->DocumentText->size; count++){
    long ch = (unsigned char)record->DocumentText->bytes[count];
    if(27 == ch){
      /* then we have an escape code */
      /* if the next letter is '(' or ')', then ignore two letters */
      if('(' == record->DocumentText->bytes[count + 1] ||
	 ')' == record->DocumentText->bytes[count + 1])
	count += 1;             /* it is a term marker */
      else count += 4;		/* it is a paragraph marker */
    }
    else if (ch == '\t') /* a TAB! */
      putc(ch, stdout);
    else if (isprint(ch)){
      if(quote_string_quotes && ch == '"')
	putc('\\', stdout);
      putc(ch, stdout);
    } 
    else if (ch == '\n' || ch == '\r')
      printf ("\n");
  }
}

/* modifies the string to exclude all seeker codes. sets length to
   the new length. */
char *delete_seeker_codes(string,length)
char *string;
long *length;
{
  long original_count; /* index into the original string */
  long new_count = 0; /* index into the collapsed string */
  for(original_count = 0; original_count < *length; original_count++){
    if(27 == string[original_count]){
      /* then we have an escape code */
      /* if the next letter is '(' or ')', then ignore two letters */
      if('(' == string[original_count + 1] ||
    ')' == string[original_count + 1])
     original_count += 1;    /* it is a term marker */
      else original_count += 4; /* it is a paragraph marker */
    }
    else string[new_count++] = string[original_count];
  }
  *length = new_count;
  return(string);
}


  
/* returns a pointer to a string with good stuff */
char *trim_junk(headline)
char *headline;
{
  long length = strlen(headline) + 1; /* include the trailing null */
  long i,j;
  headline = delete_seeker_codes(headline, &length);


  /* delete leading spaces */
  for(i=0; i < strlen(headline); i++){
    if(isprint(headline[i])){
      break;
    }
  }
  headline = headline + i;
  /* delete trailing stuff */
  for (i = 0, j = 0; i < strlen(headline)+1; i++) {
    if ((headline[i] != '\r') && (headline[i] != '\n')) {
      headline[j++] = headline[i];
    }
  }
  
 
  for(i=strlen(headline) - 1 ; i > 0; i--){
    if(isprint(headline[i])){
      break;
    }
    headline[i] = '\0';
  }
  
  return(headline);
}



