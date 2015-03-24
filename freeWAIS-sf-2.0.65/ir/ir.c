
/* Copyright (c) CNIDR (Work in progress) */

/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.	
  
*/

/* Change log:
 * $Log: ir.c,v $
 * Revision 2.0.1.4  1996/01/31 14:47:48  pfeifer
 * patch63: Use getcwd() is present.
 *
 * Revision 2.0.1.3  1995/09/12  12:27:41  pfeifer
 * patch6: Removed prototypr for alphasort(). gcc and cc do not agree on
 * patch6: it.
 *
 * Revision 2.0.1.2  1995/09/11  12:47:04  pfeifer
 * patch4: Changed TELL_USER to call getuser_name(). Replaced alphasort()
 * patch4: by prototype.
 *
 * Revision 2.0.1.1  1995/09/08  19:16:16  pfeifer
 * patch1: Fixed handling of NULL index_directory in server_security.
 *
 * Revision 2.0  1995/09/08  07:59:13  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.15  1995/06/06  09:33:45  pfeifer
 * Removed the magic help for short queries. Catalog is still returned
 * for
 *         * empty queries
 *         * queries starting with '?' or '*'
 *
 * Revision 1.14  1995/03/10  14:47:16  huynh1
 * in handleFTPSearch: search_status isn't initialized. I have set it
 * true.
 *
 * Revision 1.13  1994/12/13  18:52:45  pfeifer
 * chip@chinacat.unicom.com (Chip Rosenthal) patches.
 * Excluding the merge of libinv and libwais
 *
 * Revision 1.12  1994/08/15  16:17:50  pfeifer
 * fixed unchecked usage of getenv("USER") in printf
 *
 * Revision 1.11  1994/08/05  07:12:05  pfeifer
 * Release beta 04
 *
 * Revision 1.10  1994/04/29  23:23:23  pfeifer
 * Fixed getwd insted of getcwd
 * ifdef'ed alphasort for gcc 2.4.3
 *
 * Revision 1.9  1994/04/06  23:50:22  pfeifer
 * 08
 *
 * Revision 1.8  1994/03/21  17:29:49  huynh1
 * openDatabase modified.
 * Patchlevel 07.
 *
 * Revision 1.7  1994/03/08  20:55:03  pfeifer
 * Patchlevel 04
 *
 * Revision 1.6  93/07/23  02:18:37  warnock
 * eliminated extra argument in call to probe_file in server_security
 * 
 * Revision 1.5  93/07/02  17:49:54  warnock
 * drop-in replacement for handleElementRetrieval from francois
 * 
 * Revision 1.4  93/07/01  20:33:57  warnock
 * Drop-in replacement for server_security routine
 * 
 * Revision 1.3  93/07/01  19:16:39  warnock
 * gethostname -> mygethostname
 * 
 * Revision 1.2  93/07/01  19:01:13  warnock
 * Fix from tovio@sage.ucs.uwa.edu.au when database name array was empty
 * This is Francois' drop-in replacement for ir.c
 * 
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.49  92/05/10  14:43:35  jonathan
 * Made a little safer on NULL docid's when parsing.
 * 
 * Revision 1.48  92/05/05  14:56:33  jonathan
 * Added definition of S_ISDIR for Mach and NeXT.  Fixed isdoctype macro to
 * check for NULLs.
 * 
 * Revision 1.47  92/05/04  11:28:26  jonathan
 * Changed logging result list to use log_level a little better.
 * 
 * Revision 1.46  92/04/30  12:24:45  jonathan
 * split =* for ULTRIX CC.
 * changed a couple of s_free's to free's for ULTRIX CC too.
 * 
 * Revision 1.45  92/04/01  17:08:14  jonathan
 * Added code to handle FTP-like searches.
 * 
 * Revision 1.44  92/03/23  13:25:22  shen
 * only print out the number of the results but not the message in the log
 * 
 * Revision 1.43  92/03/18  08:55:45  jonathan
 * Removed databaseName argument to getDocumentText and getData.
 * 
 * Revision 1.42  92/03/05  07:07:20  shen
 * add two more dummy arguments to call to init_search_engine
 * 
 * Revision 1.41  92/02/23  10:37:53  jonathan
 * enforced limit on results in handleRelevanceFeedbackSearch (particularly
 * for handline help queries).
 * 
 * Revision 1.40  92/02/23  09:57:57  jonathan
 * Prevent return of help messages if query is empty but there are relevant
 * documents.
 * 
 * Revision 1.39  92/02/21  11:07:25  jonathan
 * Added RCSIdent.
 * 
 * Revision 1.38  92/02/21  11:00:10  jonathan
 * Changed logging of init message to WLOG_MEDIUM.
 * 
 * Revision 1.37  92/02/19  16:56:48  jonathan
 * mucked a bit with the no-results case.
 * 
 * Revision 1.36  92/02/19  13:55:22  jonathan
 * Return catalog as result if no hits to search.
 * Plug some more memory leaks.
 * 
 * Revision 1.35  92/02/19  10:39:27  jonathan
 * Off by one in last fix (headerNum>=0).  Fixed it.
 * 
 * Revision 1.34  92/02/19  10:16:04  jonathan
 * Added code to handle too many headers for buffer.  Reduces the number of
 * headers until they fit, and marks the log with a warning.
 * 
 * Revision 1.33  92/02/17  12:40:04  jonathan
 * Return catalog as well as source description for help queries.
 * 
 * 
 * Revision 1.32  92/02/12  13:20:49  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
*/

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/ir.c,v 2.0.1.4 1996/01/31 14:47:48 pfeifer Exp $";
#endif

/*----------------------------------------------------------------------*/
/* This code implements a simple Z39.50+WAIS server, which consults a 
   local database using Brewster's search engine.  The main routine is
   interpret_buffer() which reads the contents of a receive buffer, and 
   writes results back to a send buffer.

The basic structure is:

interpret_buffer gets bytes and returns bytes from whatever transport 
   mechanism. It calls either handleInit (which handles init requests)
   or handleSearch.

handleSearch calls either handleRelevanceFeedbackSearch or 
   handleElementRetrieval based on the type of question.

handleElementRetrieval calls getData or getDocumentText to answer that 
   question.

handleRelevanceFeedbackSearch calls run_search and/or help_header to answer 
   the question.


A server must supply:
  getData, getDocumentText, run_search, and help_header
then it should work.

   To do:
   - help facilities:
     on a null query pass back random documents from the pool (?)
   - Add dates to search responses
 */

/* change log:
 *  3/91 fixed db name defaulting for info server
 *  5/31/91 fixed handleRelevanceFeedbackSearch to do search if
 *          no seedwords but relevant document supplied - JG
 *  5/31/91 fixed databaseName in handleElementRetrieval - HWM
 *  7/19/91 fixed handleElementRetrieval prototype -BK
 */
/*----------------------------------------------------------------------*/

#include "server.h"
#include "ir.h"
#include "wprot.h"
#include "irsearch.h"
#include "docid.h"
#include "cutil.h"
#include "irfiles.h" /* for pathname_name */
#include "irretrvl.h"
#include "sockets.h"  /* for connect_to_server */
#include "panic.h"

#include <ctype.h>
/*
#include <string.h>
#include <math.h>

#ifdef ANSI_LIKE
#include <stdlib.h>
#else
#include "ustubs.h"
#endif
*/
#include <sys/stat.h>
#include "cdialect.h"

/* forward declarations */

static boolean needs_help _AP ((char *question));

static WAISDocumentHeader *help_header _AP((char *database_name, 
					    char *index_directory));

static WAISDocumentHeader *catalog_header _AP((char *database_name, 
					       char *index_directory,
					       boolean results));

static void handleInit _AP((char** recBuf,char** sendBuf,
			    long* sendBufLen,
			    long* maxBufLen));
                       
static void handleSearch _AP((char** recBuf,char** sendBuf,
			      long* sendBufLen,
			      long waisProtocolVersion,
			      char *index_directory));

static void handleRelevanceFeedbackSearch _AP((SearchAPDU* search,
					       char** sendBuf,long* sendBufLen,
					       long waisProtocolVersion,
					       char *index_directory));
                                          
static void handleElementRetrieval _AP((SearchAPDU* search,
					char** sendBuf,
					long* sendBufLen,
					long waisProtocolVersion,
					char *index_directory));
                                   
static void handleFTPSearch _AP((SearchAPDU* search,
				 char** sendBuf,long* sendBufLen,
				 long waisProtocolVersion,
				 char *index_directory));

boolean server_security _AP((char *index_directory, char *database_name));

#define isdoctype(doc, doctype) \
 (((doc) != NULL) && \
 ((doc)->Type != NULL) && \
 !strcmp((doc)->Type, doctype))

#ifdef Mach
#include <sys/inode.h>
#endif /* Mach */

/* francois */
#ifndef S_ISDIR
#define S_ISDIR(f_mode) ((f_mode & S_IFMT) == S_IFDIR)
#endif

/*----------------------------------------------------------------------*/
/* Utility */
  
/*----------------------------------------------------------------------*/
/* note - at present, it is not clear to me what the value of present-status
   is, and how it is to be interpreted.  Furthermore, are our text retrieval
   queries considered presents, or are they searches?
 */
 

/*----------------------------------------------------------------------*/

/* interpret_buffer()
char* receiveBuffer - buffer containing data to interpret
long receiveBufLen - how much data is there
char* sendBuffer - buffer to write results to
long sendBufLen - how much space there is to write to
long* maxBufferSize - see below
long waisProtocolVersion - what version of the wias protocol is in use
char *index_directory - the directory to find the indexes on a search

maxBufferSize is a pointer to a per connection variable that contains the
maximum size send/receive buffer to use.  Seems a lot like sendBufLen
does't it.  Well it usually is, but not always.

Here is how it works from a server's point of view.  

When the client connection is first established, the server spawns a new
process to deal with it.  The new process contains a global variable
(bufferSize in server.c) which is initialized to BUFSZ (defined in server.h).
This is the physical size of the server's internal bufferes.
Clearly that is the absolute maximum size of any z3950 message to or from
this server.

So now *maxBufferSize = sendBufLen.  

Now, the first thing that a z3950 client is likely to do is send an init
APDU.  The only useful thing (and it is useful) that the init APDU
currently does is allow the client and server to negotiate the maxumum size
of the messages that they will send.  This takes place somewhere down
inside of the interpret_buffer() logic where the APDU's are decoded and
response APDU's are recoded.  A pointer to bufferSize is passed to
interpret_buffer() in the maxBufferSize argument, and if the buffer happens
to contain an init message, bufferSize is changed (for the rest of the
connection).

That is the only function maxBufferSize serves.  Note that I could have
gotten rid of sendBufLen, and just used *maxBufferSize, but sendBufLen can
be and does get modified by the z3950 APDU writting code, and we don't want
the overall value being modified.

*/

long
interpret_buffer(receiveBuffer,
		    receiveBufLen,
		    sendBuffer,
		    sendBufLen,
		    maxBufferSize,
		    waisProtocolVersion,
		    index_directory)
char* receiveBuffer;
long receiveBufLen;
char* sendBuffer;
long sendBufLen;
long* maxBufferSize;
long waisProtocolVersion;
char *index_directory;
/* read & interpret receiveBuffer until receiveBufLen.  Write results into
   send buffer.  Return number of bytes written - negative if there was an 
   error 
 */
{
  char* readPos = receiveBuffer;
  char* writePos = sendBuffer;

  while (readPos - receiveBuffer < receiveBufLen && /* there is more to read */
         writePos != NULL	/* no write error */
	 )
    { pdu_type pdu = peekPDUType(readPos);
      switch (pdu)
	{ case initAPDU:
	    handleInit(&readPos,&writePos,&sendBufLen,maxBufferSize);
	    break;
	  case searchAPDU:
	    handleSearch(&readPos,&writePos,&sendBufLen,
			 waisProtocolVersion, index_directory);
	    break;
	  default:
	    /* unknown APDU error */
	    writePos = NULL;
	    waislog(WLOG_HIGH, WLOG_ERROR,
		    "Error in interpret_message: unknown APDU type.");
	    break;
	  }
    }
  
  if(writePos == NULL) {
    waislog(WLOG_HIGH, WLOG_ERROR,
	    "Error in interpret_message: NULL writePos.");
    return (0);
  }
  else return(writePos - sendBuffer);
}

/*----------------------------------------------------------------------*/

static void handleInit _AP((char** recBuf,char** sendBuf,
			    long* sendBufLen,long* maxBufferSize));

static void
handleInit(recBuf,sendBuf,sendBufLen,maxBufferSize)
char** recBuf;
char** sendBuf;
long* sendBufLen;
long* maxBufferSize;
/* negotiate functionality and buffer sizes.  A session ususally begins
   with one of these, but is not required to.  
   NOTE - even if the server decides not to accept the client, it does
   not shut down the connection.  It simply declies acceptatance, and 
   waits for the client to shut down.
 */
{
  InitAPDU* anInit = NULL;
  
  /* read the init - note there is no WAIS protocol extension here */
  *recBuf = readInitAPDU(&anInit,*recBuf);
  
  if (recBuf == NULL || *recBuf == NULL)
    { *sendBuf = NULL;		/* error in the read */
      return;
    }
  else				/* respond to the init */
    { InitResponseAPDU* reply = NULL;
      WAISInitResponse* wais_response = NULL;
      boolean connectionAccepted;
     
      /* negotiate services */
      if (anInit->willPresent == false &&
	  anInit->willDelete == false)
	connectionAccepted = true;
      else
	connectionAccepted = false;
       
      /* negotiate buffer sizes */
      if (*maxBufferSize > anInit->MaximumRecordSize)
	*maxBufferSize = anInit->MaximumRecordSize;
     
      if(anInit->IDAuthentication != NULL)
	waislog(WLOG_MEDIUM, WLOG_INFO, "Init message: %s", 
		anInit->IDAuthentication);

      /* not much use huh? */
      wais_response = makeWAISInitResponse(0L,0L,NULL,NULL,NULL,NULL); 
     
      reply = makeInitResponseAPDU(connectionAccepted,
				   true,false,false,false,false,*maxBufferSize,
				   *maxBufferSize,NULL,
				   defaultImplementationID(),
				   defaultImplementationName(),
				   defaultImplementationVersion(),NULL,
				   wais_response);

      /* write it */
      *sendBuf = writeInitResponseAPDU(reply,*sendBuf,sendBufLen);
	 
      /* free everything */
      freeInitAPDU(anInit);
      freeInitResponseAPDU(reply);
      freeWAISInitResponse(wais_response);
    }
}


/*----------------------------------------------------------------------*/
static boolean
isRemoteDB(db)
char * db;
{
  return(strchr(db,'@') != NULL);
}

/*----------------------------------------------------------------------*/
#include "wmessage.h"

struct {
  char host[256];
  long port;
  FILE *connection;
  long buffer_size;
} last_connection;

static void
forwardSearch(aSearch,sendBuf,sendBufLen,waisProtocolVersion)
SearchAPDU* aSearch;
char** sendBuf;
long* sendBufLen;
long waisProtocolVersion;
{
  FILE *connection;
  char hostname[1000], db[1000], *p, *p2;
  long port, len, rlen;
  char message[BUFSZ], response[BUFSZ];

  p = strchr(aSearch->DatabaseNames[0], '@');
  strncpy(db, aSearch->DatabaseNames[0], p-aSearch->DatabaseNames[0]);
  db[p-aSearch->DatabaseNames[0]] = 0;
  p2 = strchr(p+1, ':');
  if(p2 == NULL) {
    strcpy(hostname, p+1);
    port = 210;
  }
  else {
    strncpy(hostname, p+1, p2-(p+1));
    hostname[p2-(p+1)] = 0;
    port = atoi(p2+1);
  }

  strcpy(aSearch->DatabaseNames[0], db);
  rlen = len = BUFSZ;
  writeSearchAPDU(aSearch, message+HEADER_LENGTH, &len);
  len = BUFSZ-len;
  if(hostname[0] != 0) {
    if(strcmp(hostname, last_connection.host) == 0 &&
       port == last_connection.port)
      connection = last_connection.connection;
    else {
      if (last_connection.connection != NULL)
	close_connection(last_connection.connection);
      strcpy(last_connection.host, hostname);
      last_connection.port = port;
      last_connection.connection = (FILE*)connect_to_server(hostname, port);
      connection = last_connection.connection;
      if(connection != NULL) {
	char userInfo[500], hostname[80], init_message[1000];

	mygethostname(hostname, 80);
#ifdef TELL_USER
	sprintf(userInfo, "server forwarding %s, from host: %s, user: %s",
		VERSION_STRING, hostname, getuser_name());
#else
	sprintf(userInfo, "server forwarding %s, from host: %s", VERSION_STRING, hostname);
#endif

	last_connection.buffer_size =
	  init_connection(init_message, response,
			  BUFSZ,
			  connection,
			  userInfo);
      }
    }
    if(connection != NULL)
      {
	len = interpret_message(message, len,
				response, last_connection.buffer_size,
				connection, false);
      }
    else {
      static diagnosticRecord* diags[2] = {NULL, NULL};
      SearchResponseAPDU* response = NULL;
      WAISSearchResponse* wais_response = NULL;
      char message[255];

      sprintf(message, "Database not available: %s@%s:%d.",
	      db, last_connection.host, last_connection.port);
      diags[0] = makeDiag(true,D_RecordNotAuthorizedToBeSent,
			  message);

      wais_response = makeWAISSearchResponse(NULL,NULL,NULL,
					     NULL,NULL,NULL,NULL,diags);
      response = makeSearchResponseAPDU(0L,0L,
					1L,
					0L,UNUSED,FAILURE,
					aSearch->ReferenceID, wais_response);
      *sendBuf = writeSearchResponseAPDU(response,*sendBuf,sendBufLen);

      freeSearchResponseAPDU(response);
      freeWAISSearchResponse(wais_response);
      waislog(WLOG_HIGH, WLOG_ERROR, message);
      return;
    }
  }
  else
    len = interpret_message(message, len,
			    response, last_connection.buffer_size,
			    NULL, false);
  bcopy(response+HEADER_LENGTH, *sendBuf, len);
  *sendBuf+=len;
}

/*----------------------------------------------------------------------*/

static void
handleSearch(recBuf,sendBuf,sendBufLen,waisProtocolVersion,index_directory)
char** recBuf;
char** sendBuf;
long* sendBufLen;
long waisProtocolVersion;
char *index_directory;
/* figure out what kind of search this is, (query or retrieval) and
   dispatch to the appropriate function 
 */
{
  SearchAPDU* aSearch = NULL;

  /* read the search data */
  *recBuf = readSearchAPDU(&aSearch,*recBuf);

  if (*recBuf == NULL)
    { *sendBuf = NULL;		/* error in the read */
      return;
    }
  else
    {				/* dispatch on the query type */
      if((aSearch->DatabaseNames != NULL) &&
	 (aSearch->DatabaseNames[0] != NULL) &&
	 isRemoteDB(aSearch->DatabaseNames[0]))
	forwardSearch(aSearch,sendBuf,sendBufLen,waisProtocolVersion);
      else {
/* francois - security */
	/* patch from tovio@sage.ucs.uwa.edu.au to fix crash if aSearch->DatabaseNames 
	   is null
	*/
        if ( (aSearch->DatabaseNames != NULL) &&
	    (aSearch->DatabaseNames[0] != NULL) &&
	    (server_security(index_directory,aSearch->DatabaseNames[0]) != true ) ) {
                
                SearchResponseAPDU* response = NULL;
                WAISSearchResponse* wais_response = NULL;

                WAISDocumentHeader** headers = NULL;            
                diagnosticRecord** diags = NULL;
                diagnosticRecord* diag = NULL;
                DocObj** docs = NULL;
                
                
                char msg[MAX_FILENAME_LEN * 2];
                    
                sprintf(msg,"Unautorized access to database: ", aSearch->DatabaseNames[0]);
                diag = makeDiag(false,D_AccessControlFailure,msg);
                diags = (diagnosticRecord **)s_malloc((size_t)(sizeof(diagnosticRecord*) * 2));
                (diags)[0] = diag;
                (diags)[1] = NULL;
                  
                wais_response = makeWAISSearchResponse(NULL,NULL,NULL,NULL,
                                         NULL,NULL,NULL,diags);
                response = makeSearchResponseAPDU(SUCCESS,0L,((diags == NULL) ? 0 : 1),
                                    0L,UNUSED,SUCCESS,aSearch->ReferenceID, wais_response);

                *sendBuf = writeSearchResponseAPDU(response,*sendBuf,sendBufLen);

                if ( strcmp(aSearch->QueryType,QT_RelevanceFeedbackQuery) == 0) {
                        freeWAISSearch((WAISSearch*)aSearch->Query);
                        freeSearchAPDU(aSearch);
                        freeSearchResponseAPDU(response);
                        freeWAISSearchResponse(wais_response); 
                } 
                else if ( strcmp(aSearch->QueryType,QT_TextRetrievalQuery) == 0) {
                        docs = readWAISTextQuery((any*)aSearch->Query);
                        freeAny((any*)aSearch->Query);
                        freeSearchAPDU(aSearch);
                        freeSearchResponseAPDU(response);
                        freeWAISSearchResponse(wais_response); 
                        doList((void**)docs,freeDocObj);
                        s_free(docs);  
                } 

                return;
        }

	if (strcmp(aSearch->QueryType,QT_TextRetrievalQuery) == 0) {
	  handleElementRetrieval(aSearch,sendBuf,sendBufLen,
				 waisProtocolVersion, index_directory);
	}
	else if (strcmp(aSearch->QueryType,QT_RelevanceFeedbackQuery) == 0) {
	  char *seeds, *s;

	  s = seeds = s_strdup(((WAISSearch *)aSearch->Query)->SeedWords);
	  while(*s != 0) {
	    if(*s == '\n' || *s == '\r') *s = ' ';
	    s++;
	  }

	  if(aSearch->DatabaseNames != NULL &&
	     aSearch->DatabaseNames[0] != NULL)
	    waislog(WLOG_LOW, WLOG_SEARCH,
		    "Search! Database: %s, Seed Words: %s", 
		    aSearch->DatabaseNames[0], 
		    seeds);
	  else
	    waislog(WLOG_LOW, WLOG_SEARCH, 
		    "Search! Database: None, Seed Words: %s", 
		    seeds);

	  handleRelevanceFeedbackSearch(aSearch,sendBuf,sendBufLen,
					waisProtocolVersion,
					index_directory);
          s_free(seeds);        /* (up) */
	}
	else {
	  waislog(WLOG_HIGH, WLOG_ERROR, "Unknown search type");
	  *sendBuf = NULL;	/* error - unknown search type */
	}
	fflush(stderr);
      }
    }
}


	 

/*----------------------------------------------------------------------*/

static boolean needs_help(question)
char *question;
/* returns true if the user wants help */
{
  if(question[0] == '\0')  /* null question, must need help */
    return(true);
  if(question[0] == '?')
    return(true);
  if(question[0] == '*')
    return(true);
  return(false);
}

/* returns a help header to be returned or NULL if not possible */
static WAISDocumentHeader *help_header(database_name, index_directory)
     char *database_name;
     char *index_directory;
{
  /* make a help document */
  hit help;
  char local_id[MAX_FILENAME_LEN + 60];

  strncpy(help.filename,
	  merge_pathnames(database_name,index_directory), 
	  MAX_FILENAME_LEN);
  strncat(help.filename, source_ext, MAX_FILENAME_LEN);
  /* printf("help filename %s", help.filename); */

  strncpy(help.headline, "Information on database: ", MAX_FILENAME_LEN);
  strncat(help.headline, pathname_name(database_name), 
	  MAX_FILENAME_LEN);
  sprintf(local_id, "%ld %ld %s", 0L, 0L, help.filename);

  if(probe_file(help.filename))
    { 
      DocID* theDocID = NULL;
      long length;
      long lines;
      char **type = NULL;

      help.start_character = 0;
      help.end_character = 0;
	
      { FILE *stream = s_fopen(help.filename, "r");
	lines = count_lines(stream);
	length = file_length(stream);
        s_fclose(stream);
      }

      type = (char**)s_malloc((size_t)(sizeof(char*) * 2));
      type[0] = s_strdup("WSRC");
      type[1] = NULL;

      /* then there is a source structure to return */
      theDocID = makeDocID();
      theDocID->originalDatabase = stringToAny(database_name); /* XXX */
      theDocID->originalLocalID = stringToAny(local_id);

      return(makeWAISDocumentHeader(anyFromDocID(theDocID),
				    UNUSED,
				    MAX_NORMAL_SCORE,
				    UNUSED,
				    length,lines,
				    type,
				    s_strdup(database_name), /* XXX */
				    NULL, /* date */
				    s_strdup(help.headline),
				    NULL));
    }	
  else 
    return(NULL);
}

/* returns the catalog document to be returned or NULL if not possible */

static WAISDocumentHeader *catalog_header(database_name, index_directory, results)
     char *database_name;
     char *index_directory;
     boolean results;
{
  /* make a help document */
  hit catalog;
  char local_id[MAX_FILENAME_LEN + 60];

  strncpy(catalog.filename,
	  merge_pathnames(database_name,index_directory), 
	  MAX_FILENAME_LEN);
  strncat(catalog.filename, catalog_ext, MAX_FILENAME_LEN);
  /* printf("catalog filename %s", catalog.filename); */

  if(results)
    strncpy(catalog.headline,
	    "Catalog for database: ",
	    MAX_FILENAME_LEN);
  else
    strncpy(catalog.headline,
	    "Search produced no result. Here's the Catalog for database: ",
	    MAX_FILENAME_LEN);

  strncat(catalog.headline, pathname_name(database_name), 
	  MAX_FILENAME_LEN);
  sprintf(local_id, "%ld %ld %s", 0L, 0L, catalog.filename);

  if(probe_file(catalog.filename))
    { 
      DocID* theDocID = NULL;
      long length;
      long lines;
      char **type = NULL;

      catalog.start_character = 0;
      catalog.end_character = 0;
	
      { FILE *stream = s_fopen(catalog.filename, "r");
	lines = count_lines(stream);
	length = file_length(stream);
        s_fclose(stream);
      }

      type = (char**)s_malloc((size_t)(sizeof(char*) * 2));
      type[0] = s_strdup("TEXT");
      type[1] = NULL;

      /* then there is a catalog structure to return */
      theDocID = makeDocID();
      theDocID->originalDatabase = stringToAny(database_name); /* XXX */
      theDocID->originalLocalID = stringToAny(local_id);

      return(makeWAISDocumentHeader(anyFromDocID(theDocID),
				    UNUSED,
				    (results ? MAX_NORMAL_SCORE:0),
				    UNUSED,
				    length,lines,
				    type,
				    s_strdup(database_name), /* XXX */
				    NULL, /* date */
				    s_strdup(catalog.headline),
				    NULL));
    }	
  else 
    return(NULL);
}

/* picks a set of random documents from the database 
static void pick_random_documents(aSearch, headers, &headerNum)
{
  
}
*/


/*----------------------------------------------------------------------*/

static void
handleRelevanceFeedbackSearch(aSearch,sendBuf,sendBufLen,
			      waisProtocolVersion,
			      index_directory)
SearchAPDU* aSearch;
char** sendBuf;
long* sendBufLen;
long waisProtocolVersion;
char *index_directory;
{ 
  DocObj *doc = NULL;
  SearchResponseAPDU* response = NULL;
  WAISSearchResponse* wais_response = NULL;

  WAISDocumentHeader** headers = NULL;
  long headerNum = 0;
  long max_headers = ((WAISSearch *)aSearch->Query)->MaxDocumentsRetrieved;
  char* seedwords_used = NULL;
  diagnosticRecord** diags = NULL;
  char *seed_words_used = s_strdup(((WAISSearch *)aSearch->Query)->SeedWords);
  boolean search_status;

  if(((WAISSearch *)aSearch->Query)->Docs!=NULL &&
     (doc=((WAISSearch *)aSearch->Query)->Docs[0]) != NULL &&
     (isdoctype(doc, "TEXT-FTP") || isdoctype(doc, "FTP-DIR"))) {
    handleFTPSearch(aSearch,sendBuf,sendBufLen, waisProtocolVersion, index_directory);
    return;
  }

  /* construct a response list */
  headers = (WAISDocumentHeader**)
    s_malloc((size_t)
	     (sizeof(WAISDocumentHeader*) * 
	      (1 + max_headers)));
  headers[0] = NULL;

  if(0 != init_search_engine(index_directory, false, true, 0, 0, 0))
    panic("unable to initialize search engine");
  
  /* handle help queries */
  if(seed_words_used[0] == '\0' &&
     ((WAISSearch *)aSearch->Query)->Docs != NULL) {
    1;
  }
  else if(needs_help(seed_words_used)) {
      WAISDocumentHeader *header;
      char *database_name = (aSearch->DatabaseNames == NULL) ?
      INFO_DATABASE_NAME : aSearch->DatabaseNames[0];

    if(headerNum < max_headers) {
      header = help_header(database_name, index_directory);
      if(NULL != header) {
	headers[headerNum++] = header;
	headers[headerNum] = NULL;	
	((WAISSearch *)aSearch->Query)->MaxDocumentsRetrieved--;
      }
    }
    if(headerNum < max_headers) {
      header = catalog_header(database_name, index_directory, true);
      if(NULL != header){
	headers[headerNum++] = header;
	headers[headerNum] = NULL;	
	((WAISSearch *)aSearch->Query)->MaxDocumentsRetrieved--;
      }
    }
  }

  if(seed_words_used[0] == '\0' &&
     ((WAISSearch *)aSearch->Query)->Docs == NULL)
   {
     /* pick_random_documents(aSearch, headers, &headerNum); */
     search_status = true;
   }
  else
   { /* run the search on the database.  If a new
	search engine were to be used, this is where it would be hooked in */
     search_status = run_search(aSearch, headers,&diags, index_directory, 
				&seed_words_used, waisProtocolVersion,
				&headerNum);
   }

#define CATALOG_FOR_NO_RESULTS
#ifdef CATALOG_FOR_NO_RESULTS
  if(headerNum == 0 && headerNum < max_headers) {
    char *database_name = (aSearch->DatabaseNames == NULL) ?
      INFO_DATABASE_NAME : aSearch->DatabaseNames[0];
    WAISDocumentHeader *header = catalog_header(database_name, index_directory, false);

    if(NULL != header){
      waislog(WLOG_MEDIUM, WLOG_INFO,
	      "Search had no hits, returning catalog");
      headers[headerNum++] = header;
      headers[headerNum] = NULL;	
    }
  }
#endif /* CATALOG_FOR_NO_RESULTS */

  wais_response = makeWAISSearchResponse(seedwords_used,headers,NULL,
                                         NULL,NULL,NULL,NULL,diags);
  response = makeSearchResponseAPDU(search_status,0L,
				    headerNum + ((diags == NULL) ? 0 : 1),
				    0L,UNUSED,SUCCESS,
                                    aSearch->ReferenceID, wais_response);
  /* write it */
  {
    char *buff;
    long len;
    boolean it_fit = true;

    while(headerNum >= 0) {
      buff = *sendBuf;
      len = *sendBufLen;
      if ((buff = writeSearchResponseAPDU(response,buff,&len)) == NULL) {

	it_fit = false; /* didn't make it in the buffer. */
	headerNum--; 
	s_free(headers[headerNum]);
	headers[headerNum] = NULL;

	s_free(wais_response);
	wais_response = makeWAISSearchResponse(seedwords_used,headers,NULL,
					       NULL,NULL,NULL,NULL,diags);
	
	freeSearchResponseAPDU(response);
	response = makeSearchResponseAPDU(search_status,0L,
					  headerNum + ((diags == NULL) ? 0:1),
					  0L,UNUSED,SUCCESS,
					  aSearch->ReferenceID, wais_response);
      }
      else {
	break;
      }
    }
    *sendBuf = buff;
    *sendBufLen = len;
    if (!it_fit) {
      waislog(WLOG_HIGH, WLOG_WARNING, 
	      "Buffer overflow, adjusted results from %ld", 
	      ((WAISSearch *)aSearch->Query)->MaxDocumentsRetrieved);
    }
  }

  { /* generate report on results. */
     long i;
  
     waislog(WLOG_MEDIUM, WLOG_RESULTS, "Returned %d result%s.", headerNum,
 	    headerNum == 1 ? "" : "s");
     if (headerNum > 0) {
       waislog(WLOG_LOW, WLOG_RESULTS, "Results:");

        for (i = 0; i < headerNum; i++) {
  	char docname[MAX_FILE_NAME_LEN+50];
  	DocID *docid = docIDFromAny(headers[i]->DocumentID);
  	char *docidstring = anyToString(GetLocalID(docid));
 	waislog(WLOG_LOW, WLOG_RESULTS, " %s", docidstring);
        s_free(docidstring);    /* (up) */
        freeDocID(docid);       /* (up) */
        }

      }
      else
        waislog(WLOG_LOW, WLOG_RESULTS,
	      "Returned 0 results.  Aww.");
  }
  freeWAISSearch((WAISSearch*)aSearch->Query); 
  freeSearchAPDU(aSearch);
  freeSearchResponseAPDU(response);
  freeWAISSearchResponse(wais_response); /* free headers & seed_words_used */
  s_free(seed_words_used);    /* (up) */
}


/*----------------------------------------------------------------------*/
static void 
handleElementRetrieval(aSearch,sendBuf,sendBufLen,waisProtocolVersion, index_directory)
SearchAPDU* aSearch;
char** sendBuf;
long* sendBufLen;
long waisProtocolVersion;
char *index_directory;
/* this is a type 1 query of the restricted form specified in the 
   WAIS-protocol.  Interpret it and write out an appropriate search
   response. (note the valid element sets are Document-Text,Document-Headlines,
   and Document-Codes but we only support text for now).
 */
{ 
  SearchResponseAPDU* response = NULL;
  WAISSearchResponse* wais_response = NULL;
  DocObj** docs = NULL;
  DocObj* doc = NULL;
  char *databaseName;
  void **elementList = NULL;
  void *element = NULL;
  diagnosticRecord** diags = NULL;
  diagnosticRecord* diag = NULL;
  long numDiags = 0L;
  long numElements = 0L;
  long i;
  database* db;
  
  /* francois */
  char new_db_name[MAX_FILENAME_LEN * 2];

  
  /* read the query */
  docs = readWAISTextQuery((any*)aSearch->Query);

  /* francois */
 if (aSearch->DatabaseNames == NULL)
    strcpy(new_db_name,merge_pathnames(INFO_DATABASE_NAME, index_directory));
  else
    strcpy(new_db_name,merge_pathnames(aSearch->DatabaseNames[0], index_directory));



   /* francois - locks */
  db = openDatabase(new_db_name, false, 
#ifdef FIELDS /* tung, 3/94 */
                    true, true);
#else
  true);
#endif

  if (db == NULL){
    char msg[MAX_FILENAME_LEN * 2];
    strncpy(msg,"The following database is not available: ",
	    MAX_FILENAME_LEN);
    s_strncat(msg,aSearch->DatabaseNames[0],MAX_FILENAME_LEN,MAX_FILENAME_LEN);
    diag = makeDiag(false,D_PermanentSystemError,msg);
    diags = (diagnosticRecord **)s_malloc((size_t)(sizeof(diagnosticRecord*) * 2));
    diags[0] = diag;
    diags[1] = NULL;
    
  }
  else {
  

       /* assemble the elements and construct a response */
       for (i = 0L, doc = docs[i]; doc != NULL; doc = docs[++i])
       { 
	 long errorCode;
	 any* bufAny;
	 long size;
    
    
	 if (doc->Type != NULL &&
	     strcmp(doc->Type, "WAIS_NEXT") == 0) {
	   char docname[MAX_FILE_NAME_LEN+50], *buffer;
	  
	  if ((size = 
		next_doc(docname, 
			 anyToString(GetLocalID(docIDFromAny(doc->DocumentID))),
			 db))
	       > 0) {
	     buffer = s_malloc(strlen(docname)+50);
	     sprintf(buffer, "%s, %d", docname, size);
	     bufAny = makeAny(strlen(buffer)+1,buffer);
	     element = (void*)makeWAISDocumentText(duplicateAny(doc->DocumentID),0L,bufAny);
	   }
	   else element = NULL;
	   closeDatabase(db);
	 }
	 else if (doc->Type != NULL &&
		  strcmp(doc->Type, "WAIS_PREV") == 0) {
	   char docname[MAX_FILE_NAME_LEN+50], *buffer;
	   any* bufAny;
	   long size;
    
	   if ((size = 
		previous_doc(docname, 
			     anyToString(GetLocalID(docIDFromAny(doc->DocumentID))),
			     db))
	       > 0) {
	     buffer = s_malloc(strlen(docname)+50);
	     sprintf(buffer, "%s, %d", docname, size);
	     bufAny = makeAny(strlen(buffer),buffer);
	     element = (void*)makeWAISDocumentText(duplicateAny(doc->DocumentID),0L,bufAny);
	   }
	   else element = NULL;
	   closeDatabase(db);
	 }
	 else if (doc->ChunkCode == CT_line) {
	   element = (void*)getDocumentText(doc, &errorCode, index_directory);
	 }
	 else if (doc->ChunkCode == CT_byte) {
	   element = (void*)getData(doc, &errorCode, index_directory);
	 }
    
	 if (errorCode != GDT_NoError)
	   {			/* make a diagnostic record to return */
	     switch (errorCode)
	       { case GDT_UnsupportedChunkType:
		   diag = makeDiag(true,D_PresentRequestOutOfRange,
				   "Bad ChunkType in Request");
		   break;
		 case GDT_BadDocID:
		   diag = makeDiag(true,D_PresentRequestOutOfRange,
				   "Bad DocID in request");
		   break;
		 case GDT_MissingDocID:
		   diag = makeDiag(true,D_PresentRequestOutOfRange,
				   "Missing DocID in request");
		   break;
		 case GDT_BadRange:
		   diag = makeDiag(true,D_PresentRequestOutOfRange,
				   "Request out of range");
		   break;
		 case GDT_MissingDatabase:
		   diag = makeDiag(true,D_PresentRequestOutOfRange,
				   "Database missing from request");
		   break;
		 case GDT_BadDatabase:
		   diag = makeDiag(true,D_PresentRequestOutOfRange,
				   "File not present in specified database");
		   break;
		 default:
		   /* should never get here */
		   diag = NULL;
		   break;
		 };
	     diags = (diagnosticRecord**)s_realloc(diags,(size_t)(sizeof(diagnosticRecord*) * 
								  (numDiags + 2)));
	     diags[numDiags++] = diag;
	     diags[numDiags] = NULL;
	   }
	 if (element != NULL)
	   { if (elementList == NULL) /* create a list */
	       { elementList = (void**)s_malloc((size_t)sizeof(void*) * 2);
	       }
	   else			/* grow the list */
	     { elementList = (void**)s_realloc((char*)elementList,
					       (size_t)(sizeof(void*) * 
							(numElements + 2)));
	     }
	       elementList[numElements++] = element; /* put it in the list */
	       elementList[numElements] = NULL;
	       }
	}
   }

  wais_response = makeWAISSearchResponse(NULL,NULL,NULL,NULL,
                                         (WAISDocumentText**)elementList,NULL,
                                         NULL,diags);
  response = makeSearchResponseAPDU(SUCCESS,0L,numElements + numDiags,0L,UNUSED,
                                    SUCCESS,aSearch->ReferenceID,
				    wais_response);
  
  /* write it */
  *sendBuf = writeSearchResponseAPDU(response,*sendBuf,sendBufLen);
  

  /* clean up */
  freeAny((any*)aSearch->Query);/* have to explicitly free the user info */
  freeSearchAPDU(aSearch);
  freeSearchResponseAPDU(response);
  freeWAISSearchResponse(wais_response); /* frees the elements constructed */
  doList((void**)docs,freeDocObj);
  s_free(docs);  

  /* francois - locks */
  closeDatabase(db);
}



/*----------------------------------------------------------------------*/

static void
handleFTPSearch(aSearch,sendBuf,sendBufLen,
		waisProtocolVersion,
		index_directory)
SearchAPDU* aSearch;
char** sendBuf;
long* sendBufLen;
long waisProtocolVersion;
char *index_directory;
{
  SearchResponseAPDU* response = NULL;
  WAISSearchResponse* wais_response = NULL;

  DocID *t;
  WAISDocumentHeader** headers = NULL;
  long headerNum = 0;
  long max_headers = ((WAISSearch *)aSearch->Query)->MaxDocumentsRetrieved;
  DocObj *doc;
  char* seedwords_used = NULL;
  diagnosticRecord** diags = NULL;
  char *seed_words_used = s_strdup(((WAISSearch *)aSearch->Query)->SeedWords);
  char *database_name = (aSearch->DatabaseNames == NULL) ?
    INFO_DATABASE_NAME : aSearch->DatabaseNames[0];
  boolean search_status = true;
  int i=0;
  char *local_id,*p;
  long start,end;
  char path[200];
  char mpath[200];
  int count;

  /* construct a response list */
  headers = (WAISDocumentHeader**)
    s_malloc((size_t)
	     (sizeof(WAISDocumentHeader*) *
	      (1 + max_headers)));
  headers[0] = NULL;
  for(doc=((WAISSearch *)aSearch->Query)->Docs[i]; ((WAISSearch *)aSearch->Query)->Docs[i]!=NULL;
      doc=((WAISSearch *)aSearch->Query)->Docs[++i]){

    t=docIDFromAny(doc->DocumentID);
    local_id = anyToString(GetLocalID(t));
    freeDocID(t);
    sscanf(local_id,"%ld %ld %s",&start,&end,path);
    if(strcmp(path, "/")) {
      p=strrchr(path,'/');
      if(p)
	*p='\0';
    }

#ifdef HAS_GETCWD
    getcwd(mpath,198);           /* up: was getwd ?? */
#else
    getwd(mpath);
#endif

    if(index_directory && !substrcmp(path, index_directory))
      strcpy(path, index_directory);

    chdir(path);

    loadFileHeaders(path,headers,&headerNum,database_name, 
		    ((WAISSearch *)aSearch->Query)->MaxDocumentsRetrieved,
		    index_directory); 
    chdir(mpath);
  }
  /**** jim jim jim */
  wais_response = makeWAISSearchResponse(seedwords_used,headers,NULL,
					 NULL,NULL,NULL,NULL,diags);
  response = makeSearchResponseAPDU(search_status,0L,
				    headerNum + ((diags == NULL) ? 0 : 1),
				    0L,UNUSED,SUCCESS,
				    aSearch->ReferenceID, wais_response);
  /* write it */
  {
    char *buff;
    long len;
    boolean it_fit = true;

    while(headerNum >= 0) {
      buff = *sendBuf;
      len = *sendBufLen;
      if (headerNum*(sizeof(WAISDocumentHeader)+100) > len || (buff = writeSearchResponseAPDU(response,buff,&len)) == NULL) {

	it_fit = false;		/* didn't make it in the buffer. */
	headerNum--;
	s_free(headers[headerNum]);
	headers[headerNum] = NULL;

	s_free(wais_response);
	wais_response = makeWAISSearchResponse(seedwords_used,headers,NULL,
					       NULL,NULL,NULL,NULL,diags);

	freeSearchResponseAPDU(response);
	response = makeSearchResponseAPDU(search_status,0L,
					  headerNum + ((diags == NULL) ? 0:1),
					  0L,UNUSED,SUCCESS,
					  aSearch->ReferenceID, wais_response);
      }
      else {
	break;
      }
    }
    *sendBuf = buff;
    *sendBufLen = len;
    if (!it_fit) {
      waislog(WLOG_HIGH, WLOG_WARNING,

	      "Buffer overflow, adjusted results from %ld",
	      ((WAISSearch *)aSearch->Query)->MaxDocumentsRetrieved);
    }
  }
  {				/* generate report on results. */
    char *message;
    long size, i;

    /* calculate total length needed for log report */
    for(size = 0L, i = 0; i < headerNum; i++)
      size+=(headers[i]->DocumentID->size+2);
    if (size > 0) {
      message = s_malloc(size);
      message[0] = 0;

      for (i = 0; i < headerNum; i++) {
	char docname[MAX_FILE_NAME_LEN+50];
	DocID *docid = docIDFromAny(headers[i]->DocumentID);
	char *docidstring = anyToString(GetLocalID(docid));

	sprintf(docname, "%s", docidstring);
	s_strncat(message, docname, headers[i]->DocumentID->size, size);

	s_free(docid); 
	s_free(docidstring);

	if ( i < headerNum-1)
	  strcat(message, ", ");
      }
      waislog(WLOG_LOW, WLOG_RESULTS,
	      "Returned %d results: %s", headerNum, message);
      s_free(message);
    }
    else
      waislog(WLOG_LOW, WLOG_RESULTS,
	      "Returned 0 results.  Aww.");
  }
  freeWAISSearch((WAISSearch*)aSearch->Query);
  freeSearchAPDU(aSearch);
  freeSearchResponseAPDU(response);
  freeWAISSearchResponse(wais_response); /* free headers & seed_words_used */
}

extern int alphasort();

static int
filesonly(e)
Direntry_t *e;
{
  struct stat sb;
  int val;
  val = (stat(e->d_name, &sb) >= 0 &&( (sb.st_mode & S_IFMT) == S_IFREG || (sb.st_mode & S_IFMT)==S_IFDIR));
  if((sb.st_mode & S_IFMT)==S_IFDIR){
    if(e->d_name[strlen(e->d_name)-1]=='.')
      return(0);
    strcat(e->d_name,"/");
  }
  return(val);
}

loadFileHeaders(path,headers, headerNum,database_name,maxf,index_directory)
char *path;
WAISDocumentHeader **headers;
long *headerNum;
char *database_name;
long maxf;
char *index_directory;
{
  register int i;
  register int j;
  int k;
  Direntry_t **list;
  hit help;
  char local_id[MAX_FILENAME_LEN + 60];
  DocID* theDocID = NULL;
  struct stat sbuf;
  long length,flen;
  long lines;
  char **type = NULL;
  char *p,tmpb[200];
  int loop;
  int ch,text=1;
  FILE *fp;

  k = *headerNum;
  if ((i = scandir(".", &list, filesonly, alphasort)) < 0) {
    return;
  }
  if(strcmp(path, "/") &&
     (index_directory == NULL || 
      strcmp(path, index_directory))) {
    pathname_directory(path, help.filename);
    stat(help.filename,&sbuf);
    length=lines=sbuf.st_size;
    strncpy(help.headline, help.filename, MAX_FILENAME_LEN);
    sprintf(local_id, "%ld %ld %s", 0L, length, help.filename);
    theDocID = makeDocID();
    theDocID->originalDatabase = stringToAny(database_name); /* XXX */
    theDocID->originalLocalID = stringToAny(local_id);
    type = (char**)s_malloc((size_t)(sizeof(char*) * 2));
    strcpy(tmpb,help.filename);
    type[0] = s_strdup("FTP-DIR");
    type[1] = NULL;

    headers[k++]=makeWAISDocumentHeader(anyFromDocID(theDocID),
					UNUSED, MAX_NORMAL_SCORE, UNUSED, length,lines, type,
					s_strdup(database_name), /* XXX */
					NULL, /* date */
					s_strdup(help.headline),
					NULL);
  }

  if(!strcmp(path, "/")) {
    *path = '\0';
  }
  for (j = 0; j < i; j++){
    if(k>=maxf)
      break;
    sprintf(help.filename,"%s/%s",path,list[j]->d_name);
    stat(list[j]->d_name,&sbuf);
    length=lines=sbuf.st_size;
    strncpy(help.headline, help.filename, MAX_FILENAME_LEN);
    sprintf(local_id, "%ld %ld %s", 0L, length, help.filename);
    theDocID = makeDocID();
    theDocID->originalDatabase = stringToAny(database_name); /* XXX */
    theDocID->originalLocalID = stringToAny(local_id);
    type = (char**)s_malloc((size_t)(sizeof(char*) * 2));
    strcpy(tmpb,help.filename);
    p=strrchr(tmpb,'.');
    if(p){
      ++p;
      if(!strcasecmp(p,"tar") || !strcasecmp(p,"shar"))
	text=0;
      else
	text=1;
    }
    if(text==1){
      fp=fopen(help.filename,"r");
      if(fp==NULL)
	text=0;
      else{
	fseek(fp,0L,2);
	flen=ftell(fp);
	for(loop=2; loop<100; loop++){
	  fseek(fp,flen/loop,0);
	  ch=fgetc(fp);
	  if(ch==EOF || (!isprint(ch) && !isspace(ch)))
	    text=0;
	}
	fclose(fp);
      }
    }

    if(S_ISDIR(sbuf.st_mode))
      type[0] = s_strdup("FTP-DIR");
    else if(text==0)
      type[0] = s_strdup("FTP");
    else
      type[0]=s_strdup("TEXT");

    type[1] = NULL;

    headers[k++]=makeWAISDocumentHeader(anyFromDocID(theDocID),
					UNUSED, MAX_NORMAL_SCORE, 
					UNUSED, length,lines, type,
					s_strdup(database_name), /* XXX */
					NULL, /* date */
					s_strdup(help.headline),
					NULL);
  }
  if(list != NULL) {
    for (j = 0; j < i; j++)
      if(list[j] != NULL) free((char *)list[j]);
    free((char *)list);
  }
  *headerNum = k;
}


/*---------------------------------------------------------------------------*/

boolean
server_security(index_directory,database_name)
char *index_directory;
char *database_name;
{


/* francois - security */

/* This little piece of code implements a very simple security
   scheme for wais.

   Every user connection is validated against a file which 
   contains a list of hosts/domains that can connect to this 
   server. If the file does not exist, then the server is 
   open to everybody.
   
   The names of the security files can be found in server.h

   The format of the SERVER SECURITY file ("SERV_SEC")
   is as follows:

   host_name	host_address

   or 
   
   welchlab.welch.jhu.edu	128.220.59.10
   welchlgate.welch.jhu.edu	128.220.59.13
   
   
   Access can be given to specific domains, so if one 
   wanted to give access to everyone in the 
   welch.jhu.edu domain, the file would look like this:
   
   welch.jhu.edu	128.220.59
   
   Note that the host address is optional, but that the 
   host name is not.
  

  
   The format of the DATABASE SECURITY file ("DATA_SEC")
   is as follows:

   database_name	host_name	host_address

   or 
   
   foo	welchlab.welch.jhu.edu		128.220.59.10
   foo	welchlgate.welch.jhu.edu	128.220.59.13
   
   
   Access can be given to specific domains, so if one 
   wanted to give access to everyone in the 
   welch.jhu.edu domain, the file would look like this:
   
   foo	welch.jhu.edu	128.220.59
   
   Note that the host address is optional, but that the 
   host name is not.
   
   Using a star in this case would allow access to everyone 
   for a particular database, for example:

   foo	welch.jhu.edu	128.220.59
   bar	*	*
   
   This would be useful if you wanted to give public access
   to certain databases on a particular server and restricted 
   access to others.
   
*/
   


    FILE *   security_stream = NULL;
    char     security_filename[MAX_FILE_NAME_LEN+50];
    char     security_line[100];
    char     security_database_name[100];
    char     security_host_name[100];
    char     security_host_address[100];
    boolean  security_flag = false;
    

    if (index_directory == NULL) {
        index_directory = "(none)";
    }
    if ( database_name != NULL ) {
       sprintf(security_filename,"%s/%s",index_directory,DATASECURITYFILE);
    }
    else {
       sprintf(security_filename,"%s/%s",index_directory,SERVSECURITYFILE);
    }


   
    /* see if we can open the security file - if not then we dont use it */
/*    if ( probe_file(security_filename,"r") ) { */
    if ( probe_file(security_filename) ) {
        waislog(WLOG_HIGH, WLOG_INFO,"Using security file : %s", security_filename);
	security_stream = s_fopen(security_filename,"r");
	if ( security_stream  == NULL ) {
           waislog(WLOG_HIGH, WLOG_ERROR,"Could not open security file : %s", security_filename);
           return (false);
	}
     }
     else {
        return (true);
     }
   	
    /* loop getting each line from the security file */
    do {
  	security_line[0] = '\0';
  	security_database_name[0] = '\0';
  	security_host_name[0] = '\0';
  	security_host_address[0] = '\0';
  	
	/* read the line, skip '#' lines */
	if ( (fgets(security_line,100,security_stream) != NULL) && (security_line[0] != '#') ) {
		
		/* do we have a database name to check against? */
		if ( database_name != NULL ) {
			sscanf(security_line,"%s %s %s",security_database_name,security_host_name, security_host_address);
			
			/* check the host address if it exists */
			if ( host_address[0] != 0 && security_host_address[0] != 0 &&
				(( strncmp(host_address,security_host_address,strlen(security_host_address)) == 0 &&
				strcmp(security_database_name,database_name) == 0) ||  
				( strcmp(security_host_address,"*") == 0 && 
				strcmp(security_database_name,database_name) == 0 )) ) {
			    security_flag = true;
			    break;
			}
		
			/* check the host name if it exists*/
			if ( host_name[0] != 0 && security_host_name[0] != 0 &&
			       (( strncmp(host_name + ( strlen(host_name) - strlen(security_host_name) ),
			       security_host_name,strlen(security_host_name)) == 0 &&
			       strcmp(security_database_name,database_name) == 0) ||
			       ( strcmp(security_host_name,"*") == 0  &&
			       strcmp(security_database_name,database_name) == 0 )) ) {
			    security_flag = true;
			    break;
			}

			/* the host name/address could not be resolved, see if we have public access */
			if ( host_address[0] == 0 && host_name[0] == 0 &&
			       strcmp(security_host_name,"*") == 0  &&
			       strcmp(security_host_address,"*") == 0  &&
			       strcmp(security_database_name,database_name) == 0 ) {
			    security_flag = true;
			    break;
			}


		}
		else {
			sscanf(security_line,"%s %s",security_host_name, security_host_address);
			
			/* check the host address */
			if ( host_address[0] != 0 && security_host_address[0] != 0 &&
				((strncmp(host_address,security_host_address,strlen(security_host_address)) == 0) ||
				(strcmp(security_host_address,"*") == 0)) ) {
			    security_flag = true;
			    break;
			}
		
			/* check the host name */
			if ( host_name[0] != 0 && security_host_name[0] != 0 &&
			       ((strncmp(host_name + ( strlen(host_name) - strlen(security_host_name) ),
			       security_host_name,strlen(security_host_name)) == 0) ||
/*			       ((security_host_name,strlen(security_host_name)) == 0) || */
			       (strcmp(security_host_name,"*") == 0)) ) {
			    security_flag = true;
			    break;
			}
			
			/* the host name/address could not be resolved, see if we have public access */
			if ( host_address[0] == 0 && host_name[0] == 0 &&
			       strcmp(security_host_name,"*") == 0  &&
			       strcmp(security_host_address,"*") == 0  ) {
			    security_flag = true;
			    break;
			}

		}
			
 	}
				
    } while ( strlen(security_line) != 0 );
  

    /* close the security file */
    s_fclose(security_stream);


    /* tell the user of an attempted break-in */
    if ( security_flag == false ) {
	if ( database_name != NULL ) {
	    if(host_address[0] != 0){
	      waislog(WLOG_MEDIUM, WLOG_ERROR,"Unauthorised access of database '%s' by: %s [%s], %s ",
		  database_name, host_name, host_address, VERSION_STRING);
	    }
	    else if(host_name[0] != 0){
	      waislog(WLOG_MEDIUM, WLOG_ERROR,"Unauthorised access of database '%s' by: %s, %s",
		  database_name, host_name,  VERSION_STRING);
	    }
	}
	else {
	    if(host_address[0] != 0){
	      waislog(WLOG_MEDIUM, WLOG_ERROR,"Unauthorised access by: %s [%s], %s",
		  host_name, host_address, VERSION_STRING);
	    }
	    else if(host_name[0] != 0){
	      waislog(WLOG_MEDIUM, WLOG_ERROR,"Unauthorised access by: %s, %s",
		  host_name, VERSION_STRING);
	    }
	}
    }
    
    
  return(security_flag);

}


