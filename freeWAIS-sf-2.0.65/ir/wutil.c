/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/*--------------------------------------------------------------------------
 * ABSTRACT:    WAIS_TOOLS
 *
 * AUTHOR
 *   M. Tracy Shen
 *
 *
 *------------------------------------------------------------------------*/

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/wutil.c,v 2.0.1.1 1995/09/15 09:51:02 pfeifer Exp $";
#endif

/* Change log:
 * $Log: wutil.c,v $
 * Revision 2.0.1.1  1995/09/15 09:51:02  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:02:54  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.4  1994/10/06  13:28:31  pfeifer
 * Patch from Mariusz Niewczas
 *
 * Revision 1.3  1994/05/20  12:58:14  pfeifer
 * beta
 *
 * Revision 1.2  1994/03/08  21:07:19  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.3  92/02/12  13:58:52  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
 * 
 * ported to Unix and SaberC -brewster 7/7/90
 * added in bug fixes -Tracy 11/14/90
 */

/*  
 * INCLUDE EXTERNAL CONSTANTS                                         
 */

#include "futil.h"
#include <ctype.h> 
#include "zprot.h"
#include "zutil.h"
#include "wprot.h"
/* #include <string.h> */
#include "cdialect.h"
#include "wutil.h"


/*  
 * DEFINE LOCAL CONSTANTS
 */


#define CARRIAGE_RETURN 13
#define LINE_FEED 10
#define  ESC 27
#define  DC1 17
#define  DC3 19
#define  ETS  3

#define FF  12


#define INIT_CMD   1
#define SEARCH_CMD 2
#define EXTRACT_CMD 3
#define STOP_CMD    4

#define MAX_CMD 4  /* init, query search, extract story, stop */
#define MAX_BUFFER_LENGTH 10000 /* kludge -brewster */

/*
 * DEFINE MACROS
 */

#ifndef MINIMUM
#define MINIMUM(a,b)  (a > b ? b : a)
#endif

#define BIT(n)  (1L << n)

/*
 * define global variables
 */

 /* WAIS element names */
    char *elm_name_tbl[] = {  ES_DocumentHeader
                    ,ES_DocumentShortHeader
                    ,ES_DocumentLongHeader
                    ,ES_DocumentText
                    ,ES_DocumentHeadline
                    ,ES_DocumentCodes
                    };

#ifdef TEST

twais_tmplt_typ1_srch_apdu _AP((char* buff,long* buff_len));
twais_tmplt_typ3_srch_apdu _AP((char* buff,long* buff_len));
dsply_doc_hdr_record _AP((WAISDocumentHeader* record));
dsply_short_hdr_record _AP((WAISDocumentHeader* record));

main()
{
  char cmd[4096];
  long cmd_len;

  printf("\n\n test template request apdu\n\n");

  do 
    twais_format_req_apdu( TRUE, cmd, &cmd_len);
  while ( cmd_len > 0 );

    

  printf("\n\n test formating request apdu\n\n");
  do 
    twais_format_req_apdu( FALSE, cmd, &cmd_len);
  while ( cmd_len > 0 );


  printf("\n\n test display init response apdu\n\n");

  twais_tmplt_init_rsp_apdu(  cmd, &cmd_len);
  twais_dsply_rsp_apdu( cmd, cmd_len);

  printf("\n\n test display type3 search response apdu\n\n");
  twais_tmplt_typ3_srch_rsp_apdu(  cmd, &cmd_len);
  twais_dsply_rsp_apdu( cmd, cmd_len);

  printf("\n\n test display type1 search response apdu\n\n");
  twais_tmplt_typ1_stry_rsp_apdu( cmd, &cmd_len);
  twais_dsply_rsp_apdu( cmd, cmd_len);

}				/* main */

#endif


static void get_int_item _AP((char* itm_name, long *itm_val));

static void
get_int_item(itm_name, itm_val)
char *itm_name;
long  *itm_val;
{
  printf("    %s (integer): ", itm_name);
  scanf("%ld", itm_val);
}

static void get_str_item _AP((char* itm_name,char*  itm_val));

static void
get_str_item(itm_name, itm_val)
char *itm_name;
char *itm_val;
{
  printf("    %s (string): ", itm_name);
  scanf("%s", itm_val);
}

static void get_line_item _AP((char* itm_name,char*  itm_val));

static void
get_line_item(itm_name, itm_val)
char *itm_name;
char *itm_val;
{
   
  printf("    %s (one line string):\n", itm_name);
#ifdef _GETS__
  fgets( itm_val,255,stdin);
  if ( itm_val[0] == 0 )
    fgets(itm_val,255,stdin);
#else
  gets( itm_val);
  if ( itm_val[0] == 0 )
    gets(itm_val);
#endif
}

static void get_req_type _AP((long* req_type));

static void
get_req_type( req_type)
long *req_type;
{

  printf("\n\n");
  printf(" request type 1)init  2)query search 3)extract story 4)stop: ");
  scanf("%ld", req_type);

}				/* get_req_type */

/*
 * "twais_format_req_apdu"  This function gets request message.  It reads the
 *   request message into the the specified buffer.
 *   It returns the the length of the data 
 *   written into the buffer as an output parameter.  
 *   A zero length value indicates "end session".
 *
 * return values:
 *  none
 */
void 
twais_format_req_apdu(use_template,apdu_buff,len)
boolean use_template;
char* apdu_buff;
long* len;
/* use_template :  (I) flag indicating whether use template apdu */
/* apdu_buff    :  (O) buffer to put apdu */
/* len          :  (O) number of bytes written to the buffer */
{
  long req_type;

  do { 

    get_req_type( &req_type);

    switch (req_type) {
    case INIT_CMD: 
      *len = twais_format_init_apdu( use_template, apdu_buff);
      break;
    case SEARCH_CMD: 
      *len = twais_format_typ3_srch_apdu( use_template, apdu_buff);
      break;
    case EXTRACT_CMD: 
      *len = twais_format_typ1_srch_apdu( use_template,  apdu_buff);
      break;
    case STOP_CMD: 
      *len = 0;
      break;
    default: 
      break;
    
    }				/* end switch */
  }  
  while ( req_type < 1 || req_type > MAX_CMD );

}				/* twais_format_req_apdu */



/*
 * "twais_format_init_apdu"
 *   prompt user information to format an init apdu and 
 *   write the apdu to the specified buffer
 *
 * function return:
 *   number of bytes written to the buffer 
 */
long 
twais_format_init_apdu(use_template,apdu_buff)
boolean use_template;
char* apdu_buff;

/* use_template :  (I) flag indicating whether use template apdu */
/* apdu_buff    :  (O) buffer to put apdu */
{
  InitAPDU *init1;
  char  *end_ptr;
  long len;
  long pref_msg_size;
  long max_rec_size;
  any refID;
  char ref_id[128];

  if ( use_template ) {
    twais_tmplt_init_apdu(apdu_buff, &len);
    return( len);
  }      

  get_int_item( "prefered message size", &pref_msg_size);
  get_int_item( "maximum record size", &max_rec_size);

  get_str_item("reference id", ref_id);
  refID.size = strlen(ref_id);
  refID.bytes = &ref_id[0];

  init1 = makeInitAPDU(WILL_USE,WILL_NOT_USE,WILL_NOT_USE,WILL_NOT_SUPPORT,
                       WILL_NOT_SUPPORT,
                       pref_msg_size,
                       max_rec_size, NULL,
                       defaultImplementationID(),defaultImplementationName(),
                       defaultImplementationVersion(), &refID,NULL);

  { long buffer_len = MAX_BUFFER_LENGTH;
    end_ptr = writeInitAPDU(init1, apdu_buff, &buffer_len);
  }
  freeInitAPDU(init1);
  len = (long)end_ptr - (long)apdu_buff;
  return(len);  
}				/* twais_format_init_apdu */

/* 
 * "twais_format_typ3_srch_apdu"  
 *  prompt user information to form a relevance feedback search apdu and
 *  write the apdu to the specified buffer
 *
 * function return:
 *   number of bytes written to the buffer 
 */
long 
twais_format_typ3_srch_apdu(use_template,apdu_buff)
boolean use_template;
char* apdu_buff;

/* use_template :  (I) flag indicating whether use template apdu */
/* apdu_buff    :  (O) buffer to put apdu */
{
  SearchAPDU *search;
  char  *end_ptr;
  long len;
  long small, large, medium;

  char temp_string[80];

  long num_databases, num_elmsets, i;
  char **database_names_ptr;
  char *database_names[11];
  char database_name[10][129];
  long  elm_type;
  char *elm_names[21];
  char elm_name[20][128];
  char **elm_names_ptr = &elm_names[0];

  any refID;
  char ref_id[256];
  char seed_words[257];

  DocObj *DocObjs[5];

  DocObj **DocObjsPtr = 0;
  long DateFactor;
  char BeginDateRange[9], EndDateRange[9];
  long maxDocsRetrieved;
  long num, sw;
  char doc_id[4][200];
  any docID[4];
  long ChunkCode;
  any Start[4], End[4];   
  char start_pos[4][10], end_pos[4][10];
  long start_ipos, end_ipos;
  WAISSearch *query;
  long count;
  for(count = 0; count < 5; count++){
    DocObjs[count] = 0;		/* added by brewster */
  }

  if ( use_template ) {
    twais_tmplt_typ3_srch_apdu(apdu_buff, &len);
    return( len);
  }      


  get_int_item("small set upper bound", &small);
  get_int_item("large set lower bound", &large);
  get_int_item("medium set present number", &medium);

  get_int_item("number of databases(max 10, 0 search entire databases)", &num_databases);

  if ( num_databases == 0 ) {
    database_names_ptr = 0;
  }

  else {
    database_names_ptr = &database_names[0];
    database_names[num_databases] = 0;  
    for ( i=0; i < num_databases; i++ ) {
      database_names[i] = &database_name[i][0];
      sprintf( temp_string,"database name %ld", (i+1) );
      get_str_item(temp_string, database_name[i]);
    }
  }

  get_int_item("number of database-element_set pairs(max 10)", &num_elmsets);

  if ( num_elmsets == 0 ) 
    elm_names_ptr = 0;
  else {
    elm_names[num_elmsets * 2 ] = 0;
    for ( i=0; i < num_elmsets; i++ ) {
      elm_names[i*2] = &elm_name[i*2][0];
      sprintf( temp_string,"database name %ld", (i+1) );
      get_str_item(temp_string, &elm_name[i*2][0]);

      get_int_item("  element type 0)default 1)DocHeaders 2)ShortHeaders\n        3)LongHeaders 4)Text 5)HeadLines 6)Codes", 
		   &elm_type);
      if ( (elm_type < 1) || (elm_type > 6) )
	elm_type = 1;
      elm_names[i*2+1] = &elm_name[i*2+1][0];
      strcpy( &elm_name[i*2+1][0], elm_name_tbl[elm_type-1]);
    }
  }

 
  get_str_item("reference id", ref_id);
  refID.size = strlen(ref_id);
  refID.bytes = &ref_id[0];

  /*
   * format wais search query
   */
  /* fill in the user information */

  get_line_item("seed_words", seed_words);
  get_int_item("number of relevance feedback documents(max 4)", &num);
  if ( num > 0 ) {

    if ( num > 4 )
      num = 4;
    DocObjsPtr = &DocObjs[0];
    for ( i=0; i< num; i++) {
      docID[i].bytes = &doc_id[i][0];
      sprintf(temp_string, "document id %ld", (i+1));
      get_str_item(temp_string, &doc_id[i][0]);
      docID[i].size = strlen( &doc_id[i][0]);
      get_int_item("  (1)whole story (2)part story", &sw);
      if ( sw == 1) {
	DocObjs[i] = makeDocObjUsingWholeDocument( &docID[i],NULL);
      }
      else {
	get_int_item("  ChunkCode (1)byte (2)line (3)parag", &ChunkCode);
	if ( ChunkCode == 1 ) {
	  get_int_item("  start position", &start_ipos);
	  get_int_item("  end position", &end_ipos);
	  DocObjs[i] = makeDocObjUsingBytes( &docID[i], NULL,
					    start_ipos, end_ipos);
	}
	else if ( ChunkCode == 2 ) {
	  get_int_item("  start position", &start_ipos);
	  get_int_item("  end position", &end_ipos);
	  DocObjs[i] = makeDocObjUsingLines( &docID[i], NULL,
					    start_ipos, end_ipos);
	}
	else {
	  get_str_item("  start position", &start_pos[i][0]);
	  get_str_item("  end position", &end_pos[i][0]);
	  Start[i].size = strlen( &start_pos[i][0]);
	  Start[i].bytes = &start_pos[i][0];
	  End[i].size = strlen( &end_pos[i][0]);
	  End[i].bytes = &end_pos[i][0];
	  DocObjs[i] = makeDocObjUsingParagraphs( &docID[i], NULL,
						 &Start[i], &End[i]);
	}
      }				/* end if-else */
    }				/* end for */
  }				/* end if */
   
  get_int_item("DateFactor (1-independent,2-later,3-earlier,4-range)", &DateFactor);
  if ( DateFactor == 2 || DateFactor == 4) 
    get_str_item("begin date  (yyyymmdd)", BeginDateRange);
  else
    BeginDateRange[0] = '\0';
  if ( DateFactor == 3 || DateFactor == 4) 
    get_str_item("end date (yyyymmdd)", EndDateRange);
  else
    EndDateRange[0] = '\0';

  get_int_item("max. documents retrieved", &maxDocsRetrieved);

  query = makeWAISSearch( seed_words, DocObjsPtr, 0L,
			 DateFactor, BeginDateRange, EndDateRange,
			 maxDocsRetrieved);

  search = makeSearchAPDU( small, large, medium, 
			  1L,	/* replace indicator */
			  "FOO", /* result set name */
			  database_names_ptr, /* database name */   
			  QT_RelevanceFeedbackQuery, /* query_type */
			  elm_names_ptr, /* element name */
			  &refID, query);

  {
    long buffer_len = MAX_BUFFER_LENGTH;
    end_ptr = writeSearchAPDU(search, apdu_buff, &buffer_len);
  }
  len = (long)end_ptr - (long)apdu_buff;

  i =0;
  while ( DocObjs[i] != 0 ) {
    CSTFreeDocObj( DocObjs[i]);
    i++;
  }

  CSTFreeWAISSearch(query);

  freeSearchAPDU(search);

  return(len);

}				/* twais_format_typ3_srch_apdu */

/* 
 * "twais_format_typ1_srch_apdu"  
 *  prompt user information to form a text retrieval search apdu and
 *  write the apdu to the specified buffer
 *
 * function return:
 *   number of bytes written to the buffer 
 */
long
twais_format_typ1_srch_apdu(use_template,apdu_buff)
boolean use_template;
char* apdu_buff;

/* use_template :  (I) flag indicating whether use template apdu */
/* apdu_buff    :  (O) buffer to put apdu */
{
  SearchAPDU *search;
  char  *end_ptr;
  long len;
  long small, large, medium;

  char temp_string[80];

  long num_databases, num_elmsets, i;
  char **database_names_ptr;
  char *database_names[11];
  char database_name[10][129];
  long  elm_type;
  char *elm_names[21];
  char elm_name[20][128];
  char **elm_names_ptr = &elm_names[0];

  any refID;
  char ref_id[256];

  any *query;			/* changed by brewster from char * */
  DocObj *DocObjs[11];
  long num_doc;
  long sw;
  char doc_id[10][200];
  any docID[10];
  long ChunkCode;
  any Start[10], End[10];   
  long start_ipos, end_ipos;
  char start_pos[10][10], end_pos[10][10];

  long use_text = 0;


  if ( use_template ) {
    twais_tmplt_typ1_srch_apdu(apdu_buff, &len);
    return( len);
  }      


  get_int_item("small set upper bound", &small);
  get_int_item("large set lower bound", &large);
  get_int_item("medium set present number", &medium);

  get_int_item("number of databases(max 10, 0 search entire databases)", &num_databases);

  if ( num_databases == 0 ) {
    database_names_ptr = 0;
  }
  else {
    database_names_ptr = &database_names[0];
    database_names[num_databases] = 0;  
    for ( i=0; i < num_databases; i++ ) {
      database_names[i] = &database_name[i][0];
      sprintf(temp_string, "database name %ld", (i+1));
      get_str_item( temp_string, database_name[i]);
    }
  }
 
  get_int_item("number of database-element_set pairs(max 10)", &num_elmsets);

  if ( num_elmsets == 0 ) 
    elm_names_ptr = 0;
  else {
    elm_names[num_elmsets * 2 ] = 0;
    for ( i=0; i < num_elmsets; i++ ) {
      elm_names[i*2] = &elm_name[i*2][0];
      sprintf( temp_string,"database name %ld", (i+1) );
      get_str_item(temp_string, &elm_name[i*2][0]);

      get_int_item("  element type 0)default 1)DocHeaders 2)ShortHeaders\n        3)LongHeaders 4)Text 5)HeadLines 6)Codes", 
		   &elm_type);
      if ( (elm_type < 1) || (elm_type > 6) )
	elm_type = 1;
      if ( elm_type == 4)  use_text++;
      elm_names[i*2+1] = &elm_name[i*2+1][0];
      strcpy( &elm_name[i*2+1][0], elm_name_tbl[elm_type-1]);
    }
  }

  get_str_item("reference id", ref_id);
  refID.size = strlen(ref_id);
  refID.bytes = &ref_id[0];

  /*
   * format type 1 query
   */
  get_int_item("num of documents to retrieve (max 10)", &num_doc);
  if ( num_doc > 10 ) num_doc = 10;
  if ( num_doc < 1 ) num_doc = 1;

  DocObjs[num_doc] = 0;

  for ( i=0; i < num_doc; i++ ) {
    docID[i].bytes = &doc_id[i][0];

    sprintf( temp_string, "document id %ld", (i+1));
    get_str_item(temp_string, &doc_id[i][0]);
    docID[i].size = strlen( &doc_id[i][0]);
        
    if ( use_text == 0 )
      DocObjs[i] = makeDocObjUsingWholeDocument( &docID[i],NULL);
    else {         
      get_int_item("  (1)whole story (2)part story", &sw);
      if ( sw == 1) {
	DocObjs[i] = makeDocObjUsingWholeDocument( &docID[i],NULL);
      }
      else {
	get_int_item("  ChunkCode (1)byte (2)line (3)parag", &ChunkCode);
	if ( ChunkCode == 1 ) {
	  get_int_item("  start position", &start_ipos);
	  get_int_item("  end position", &end_ipos);
	  DocObjs[i] = makeDocObjUsingBytes( &docID[i], NULL,
					    start_ipos, end_ipos);
	}
	else if ( ChunkCode == 2 ) {
	  get_int_item("  start position", &start_ipos);
	  get_int_item("  end position", &end_ipos);
	  DocObjs[i] = makeDocObjUsingLines( &docID[i], NULL,
					    start_ipos, end_ipos);
	}
	else {
	  get_str_item("  start position", &start_pos[i][0]);
	  get_str_item("  end position", &end_pos[i][0]);
	  Start[i].size = strlen( &start_pos[i][0]);
	  Start[i].bytes = &start_pos[i][0];
	  End[i].size = strlen( &end_pos[i][0]);
	  End[i].bytes = &end_pos[i][0];
	  DocObjs[i] = makeDocObjUsingParagraphs( &docID[i], NULL,
						 &Start[i], &End[i]);
	}
      }				/* end if-else */
    }
  }

  query = makeWAISTextQuery(DocObjs);   


  search = makeSearchAPDU( small, large, medium, 
			  1L,	/* replace indicator */
			  "FOO", /* result set name */
			  database_names_ptr, /* database name */   
			  QT_TextRetrievalQuery, /* query_type */
			  elm_names_ptr, /* element name */
			  &refID, query);

  {
    long buffer_len = MAX_BUFFER_LENGTH;
    end_ptr = writeSearchAPDU(search, apdu_buff, &buffer_len);
  }
  len = (long)end_ptr - (long)apdu_buff;

  i =0;
  while ( DocObjs[i] != 0) {
    CSTFreeDocObj( DocObjs[i]);
    i++;
  }

  CSTFreeWAISTextQuery( query);

  freeSearchAPDU(search);

  return(len);

}				/* twais_format_typ1_srch_apdu */

static void print_hdline _AP((char* hdline));

static void print_hdline(hdline)
char *hdline;
{
  char buf[301];
  long len, i;

  len = strlen(hdline);
  if (len > 300 )
    len =300;
  for ( i=0; i< len; i++ )
    if ( hdline[i] > 31 && hdline[i] < 127 )
      buf[i] = hdline[i];
    else
      buf[i] = '@';         
  buf[i] = 0;
  printf("     Headline:  %s\n", buf);
}


static void print_any _AP((char* title,any*  any_ptr));

static void print_any( title, any_ptr)
char *title;
any *any_ptr;
{
  long i;
  printf("%s", title);
  if ( any_ptr ) {
    for ( i=0; i < any_ptr->size; i++)
     { if (isprint(any_ptr->bytes[i]))
	 printf("%c", any_ptr->bytes[i]);
       else
	 printf("%ld", (long)any_ptr->bytes[i]);
     }
    printf("\n");
  } 
}


  
static void dsply_long_hdr_record _AP((WAISDocumentLongHeader* record));

static void dsply_long_hdr_record(record)
WAISDocumentLongHeader *record;
/*
 * 'dsply_long_hdr_record'
 *   display one WAIS long header  record 
 */
{
  printf(" LongHeaders\n");

  print_any("     DocumentID:  ", record->DocumentID);

  printf("     VersionNumber:  %ld\n", record->VersionNumber);

  printf("     Score:  %ld,  BestMatch:  %ld\n", 
	 record->Score,
	 record->BestMatch);

  printf("     DocumentLength:  %ld,  Lines:  %ld\n", 
	 record->DocumentLength,
	 record->Lines);

  if ( record->Source )
    printf("     Source:  %s\n", record->Source);

  if ( record->Date )
    printf("     Date:  %s\n", record->Date);

  if ( record->OriginCity)
    printf("     OriginCity:  %s\n", record->OriginCity);

  if ( record->Headline)
#ifdef FOR_DUM_TERM
    print_hdline(record->Headline);
#else
  printf("     Headline:  %s\n", record->Headline);
#endif

  if ( record->StockCodes)
    printf("     StockCodes:  %s", record->StockCodes);

  if ( record->CompanyCodes )
    printf("     CompanyCodes:  %s", record->CompanyCodes);

  if ( record->IndustryCodes)
    printf("     IndustryCodes:  %s", record->IndustryCodes);

  printf("\n");
}
/*
 * dsply_text
 *  display story text
 */
#define TEXT_LINES_PER_PAGE 18
#define  DQ_PAR_LEN   3
#define  DQ_EOL_LEN   1
#define  DQ_MAX_LINE_LEN  256

#define  PRINT_LINE(line, ndx, line_cnt,text_ptr,last_ptr,continue_viewing)\
         line[ndx] = NULL;\
         line_cnt++; \
         printf("%s\n", line);\
         ndx = 0;\
         if ( (line_cnt == TEXT_LINES_PER_PAGE) && (text_ptr <= last_ptr)) {\
            line_cnt = 0;\
            printf("\n  ... more to come, enter 1 to continue or 0 to stop: ");\
            scanf("%ld", &continue_viewing);\
            }

#if 0
/*
 * try to format text
 */
dsply_text( size, text)
long size;
char *text;
{
  char  line[DQ_MAX_LINE_LEN +1];
  char *last_ptr;
  char *text_ptr;
  long  ndx;
  long line_cnt = 0;
  long continue_viewing = 1;
    
  text_ptr = text;

  last_ptr = text_ptr + size;

  while ( (*text_ptr != ETS) && (text_ptr < last_ptr) ) {
    if (  *text_ptr++ == ESC ) {
      if (*text_ptr++ == 'k') 
	break;
    }
  }

  if ( text_ptr >= last_ptr ) {
    printf("**** ERROR in display text -- could not find text\n");
    return;
  }

  ndx = 0;

  while ( (continue_viewing == 1) && ( *text_ptr != ETS) &&
	 (text_ptr < last_ptr) ) {

    /* paragraph id -- skip */
    if ( *text_ptr == ESC) {
      text_ptr++;
      if ( *text_ptr == 'l' )
	text_ptr++;
      if ( ndx > 0 ) {
	PRINT_LINE(line, ndx,line_cnt,text_ptr,last_ptr,continue_viewing);  
      }
      ndx = DQ_PAR_LEN;
      strncpy( line, text_ptr, ndx);
      text_ptr += DQ_PAR_LEN;
      PRINT_LINE(line, ndx,line_cnt,text_ptr,last_ptr,continue_viewing);  
    }

    /* highlight & dehighlight markers -- skip */
    else if ( (*text_ptr == DC1) || (*text_ptr == DC3) ) 
      text_ptr++;

    /* CR -- skip and print */
    else if ( *text_ptr == CARRIAGE_RETURN ) {
      text_ptr += DQ_EOL_LEN;	/* CR, LF */
      PRINT_LINE(line, ndx, line_cnt,text_ptr,last_ptr,continue_viewing);
    }

    else {
      line[ndx++] = *text_ptr++;
      if ( ndx > DQ_MAX_LINE_LEN ) {
	printf("**** ERROR in display text -- line too long %ld\n", ndx);
	return;
      }
    }
  }				/* end while */

  if ( text_ptr >= last_ptr ) {
    printf("**** ERROR in display text -- could not find End Of Text\n");
    return;
  }

  if ( ndx > 0 ) {
    line[ndx] = NULL;
    line_cnt++; 
    printf("%s\n", line);
  }


}				/* dsply_text */
#endif

static void dsply_text _AP((long size,char* text));

static void dsply_text( size, text)
long size;
char *text;
{
  char *text_ptr, *last_ptr;
  long line_cnt = 0;
  long continue_viewing = 1;
  char buff[3200];
  long len;
  char *buff_start, *buff_ptr;
    
  text_ptr = text;

  buff_start = &buff[0];    
  buff_ptr = buff_start;
  
  last_ptr = text_ptr + size;

  while ( (text_ptr <= last_ptr) && (continue_viewing == 1) ) {
    /* control markers */
    if (  *text_ptr == ESC ) {
      text_ptr++;
      text_ptr++;
    }
    /* highlight & dehighlight markers -- skip */
    else if (*text_ptr == DC1 || *text_ptr == DC3)
      text_ptr++;
    else if (*text_ptr == ETS)
      text_ptr++;
    else if ( *text_ptr == CARRIAGE_RETURN ) {
      text_ptr++;		/* CR */
      *buff_ptr++= CARRIAGE_RETURN;
      *buff_ptr++ = LINE_FEED;
      line_cnt++;
      if ( (line_cnt == TEXT_LINES_PER_PAGE) && 
	  (text_ptr <= last_ptr)) {
	len = (long)buff_ptr - (long) buff_start;
	fwrite( buff_start, len, 1, stdout);
	line_cnt = 0;
	buff_ptr = buff_start;
	printf("\n  ... more to come, enter 1 to continue or 0 to stop: ");
	scanf("%ld", &continue_viewing);
      }
    }
    else 
      *buff_ptr++ = *text_ptr++;
  }				/* end while */

  if ( buff_ptr > buff_start) {
    len = (long)buff_ptr - (long)buff_start;
    fwrite( buff_start, len,  1, stdout);
  }

}				/* dsply_text */

static void dsply_text_record _AP((WAISDocumentText*  record));

static void dsply_text_record( record)
WAISDocumentText *record;
/*
 * 'dsply_text_record'
 *   display one WAIS text record 
 */
{
  printf(" Text\n");
  print_any("     DocumentID:  ", record->DocumentID);

  printf("     VersionNumber:  %ld\n", record->VersionNumber);
  dsply_text( record->DocumentText->size,
	     record->DocumentText->bytes);
}

static void dsply_headline_record _AP((WAISDocumentHeadlines*  record));

static void dsply_headline_record( record)
WAISDocumentHeadlines *record;
/*
 * 'dsply_headline_record'
 *   display one WAIS headline record 
 */
{
  printf(" Headlines\n");
  print_any("     DocumentID:  ", record->DocumentID);

  printf("     VersionNumber:  %ld\n", record->VersionNumber);

  if ( record->Source )
    printf("     Source:  %s\n", record->Source);

  if ( record->Date )
    printf("     Date:  %s\n", record->Date);

  if ( record->OriginCity)
    printf("     OriginCity:  %s\n", record->OriginCity);
  print_hdline(record->Headline);

  if ( record->Headline)
#ifdef FOR_DUM_TERM
    print_hdline(record->Headline);
#else
  printf("     Headline:  %s\n", record->Headline);
#endif
}

static void dsply_code_record _AP((WAISDocumentCodes*  record));

static void dsply_code_record( record)
WAISDocumentCodes  *record;
/*
 * 'dsply_code_record'
 *   display one WAIS code record 
 */
{
  printf(" Codes\n");
  print_any("     DocumentID:  ", record->DocumentID);

  printf("     VersionNumber:  %ld\n", record->VersionNumber);

  if ( record->StockCodes)
    printf("     StockCodes:  %s", record->StockCodes);

  if ( record->CompanyCodes )
    printf("     CompanyCodes:  %s", record->CompanyCodes);

  if ( record->IndustryCodes)
    printf("     IndustryCodes:  %s", record->IndustryCodes);

  printf("\n");
}


/*
 * "twais_dsply_rsp_apdu"
 *   display response apdu on stdout
 *   the response apdu is in the specified buffer
 */
void
twais_dsply_rsp_apdu(rsp_buff, rsp_len)
char *rsp_buff;         /* (I) buffer contain response apdu */
long  rsp_len;           /* (I) number of bytes in the buffer */
{

  pdu_type pdu;
  pdu = peekPDUType(rsp_buff);
  switch ( pdu) {
  case initResponseAPDU:
    twais_dsply_init_rsp_apdu( rsp_buff);
    break;
  case searchResponseAPDU:
    twais_dsply_srch_rsp_apdu( rsp_buff);
    break;

  case initAPDU:
    twais_dsply_init_apdu( rsp_buff);
    break;
  case searchAPDU:
    twais_dsply_srch_apdu( rsp_buff);
    break;
  default:
    /* others not supported yet */
    break;
  }

}				/* twais_dsply_rsp_apdu */


/*
 * "twais_dsply_init_rsp_apdu"
 *   display init response apdu on stdout
 *   the response apdu is encoded in the specified buffer
 */
void
twais_dsply_init_rsp_apdu( buffer)
char *buffer;           /* (I) buffer contain the init response apdu */
{

  InitResponseAPDU *response;
  WAISInitResponse *info;
  long i, len;

  printf("\n\n Init Response:\n");

  readInitResponseAPDU(&response,buffer);

  printf("    willSearch: %ld,  willPresent: %ld,  willDelete: %ld\n", 
	 response->willSearch, 
	 response->willPresent,
	 response->willDelete);
  printf("    supportAccessControl: %ld,  supportResourceControl: %ld\n",
	 response->supportAccessControl,
	 response->supportResourceControl);
  printf("    PreferredMessageSize: %ld,  MaximumRecordSize: %ld\n",
	 response->PreferredMessageSize, 
	 response->MaximumRecordSize);
  if ( response->ImplementationID != 0) {
    printf("    ImplementationID: %s\n", 
	   response->ImplementationID);   
  }
  if ( response->ImplementationName != 0) {
    printf("    ImplementationName: %s\n",
	   response->ImplementationName);   
  }
  if ( response->ImplementationVersion != 0) {
    printf("    ImplementationVersion: %s\n",
	   response->ImplementationVersion);   
  }
  if ( response->ReferenceID != 0) {
    print_any("    ReferenceID: ", response->ReferenceID);
  }

  if ( response->UserInformationField != 0) {
    info = (WAISInitResponse *)response->UserInformationField;
    printf("    ChunkCode: %ld,  ChunkIDLength: %ld\n",
	   info->ChunkCode,
	   info->ChunkIDLength);

    printf("    ChunkMarker: ");
    len = strlen( info->ChunkMarker);
    for ( i=0; i< len; i++)
      printf("%ld, ", info->ChunkMarker[i]);
    printf("\n");

    printf("    HighlightMarker: ");
    len = strlen( info->HighlightMarker);
    for ( i=0; i< len; i++)
      printf("%ld, ", info->HighlightMarker[i]);
    printf("\n");

    printf("    DeHighlightMarker: ");
    len = strlen( info->DeHighlightMarker);
    for ( i=0; i< len; i++)
      printf("%ld, ", info->DeHighlightMarker[i]);
    printf("\n");

    printf("    NewlineCharacters: ");
    len = strlen( info->NewlineCharacters);
    for ( i=0; i< len; i++)
      printf("%ld, ", info->NewlineCharacters[i]);
    printf("\n");

  }
  freeInitResponseAPDU( response);

}				/* twais_dsply_init_rsp_apdu */

/*
 * "twais_dsply_init_apdu"
 *   display init apdu on stdout
 *   the apdu is in the specified buffer
 */
void
twais_dsply_init_apdu( buffer)
char *buffer;           /* (I) buffer contain the init response apdu */
{

  InitAPDU *init;

  printf("\n\n Init Request:\n");

  readInitAPDU(&init,buffer);

  printf("    willSearch: %ld,  willPresent: %ld,  willDelete: %ld\n", 
	 init->willSearch, 
	 init->willPresent,
	 init->willDelete);
  printf("    supportAccessControl: %ld,  supportResourceControl: %ld\n",
	 init->supportAccessControl,
	 init->supportResourceControl);
  printf("    PreferredMessageSize: %ld,  MaximumRecordSize: %ld\n",
	 init->PreferredMessageSize, 
	 init->MaximumRecordSize);

  if ( init->IDAuthentication != 0) {
    printf("    IDAuthentication: %s\n", 
	   init->IDAuthentication);   
  }

  if ( init->ImplementationID != 0) {
    printf("    ImplementationID: %s\n", 
	   init->ImplementationID);   
  }

  if ( init->ImplementationName != 0) {
    printf("    ImplementationName: %s\n",
	   init->ImplementationName);   
  }

  if ( init->ImplementationVersion != 0) {
    printf("    ImplementationVersion: %s\n",
	   init->ImplementationVersion);   
  }

  if ( init->ReferenceID != 0) {
    print_any("    ReferenceID: ", init->ReferenceID);
  }

  freeInitAPDU( init);

}				/* twais_dsply_init_apdu */

/*
 * "twais_dsply_srch_rsp_apdu"
 *   display search response apdu on stdout
 *   the response apdu is encoded in the specified buffer
 */
void
twais_dsply_srch_rsp_apdu( buffer)
char *buffer;           /* (I) buffer contain the search response apdu */
{
  SearchResponseAPDU  *response;
  WAISSearchResponse  *info;
  long continue_viewing;
  long i, k;

  printf("\n\n Search Response:\n");

  readSearchResponseAPDU(&response,buffer);
  printf("    SearchStatus:            %ld\n", response->SearchStatus); 
  printf("    ResultCount:             %ld\n", response->ResultCount); 
  printf("    NumberOfRecordsReturned: %ld\n", response->NumberOfRecordsReturned); 
  printf("    PresentStatus:           %ld\n", response->PresentStatus); 
  if ( response->ReferenceID != 0 )
    print_any("    ReferenceID:             ", response->ReferenceID);

  if ( response->DatabaseDiagnosticRecords != 0 ) {
    info = (WAISSearchResponse *)response->DatabaseDiagnosticRecords;
    if ( info->SeedWordsUsed != 0 )
      printf("    SeedWordsUsed:           %s\n", info->SeedWordsUsed); 

    i =0; 
    continue_viewing = 1; 

    if ( info->DocHeaders != 0 ) {
      k =0;
      while ( (continue_viewing == 1) && info->DocHeaders[k] != 0 ) {
	i++;
	printf("\n    record %2d, ", i);
	dsply_doc_hdr_record( info->DocHeaders[k++]);
#ifdef FOR_DUM_TERM
	if ( i < response->NumberOfRecordsReturned ) {
	  printf("\n\n  ... more to come,  enter 1 to continue or 0 to stop: ");
	  scanf("%ld", &continue_viewing);
	}
#endif
      }
    }

    if ( info->ShortHeaders != 0 ) {
      k =0;
      while ( (continue_viewing == 1) && info->ShortHeaders[k] != 0 ) {
	i++;
	printf("\n    record %2d, ", i);
	dsply_short_hdr_record( info->ShortHeaders[k++]);
#ifdef FOR_DUM_TERM
	if ( i < response->NumberOfRecordsReturned ) {
	  printf("\n\n  ... more to come,  enter 1 to continue or 0 to stop: ");
	  scanf("%ld", &continue_viewing);
	}
#endif
      }
    }

    if ( info->LongHeaders != 0 ) {
      k =0;
      while ( (continue_viewing == 1) && (info->LongHeaders[k] != 0) ) {
	i++;
	printf("\n    record %2d, ", i);
	dsply_long_hdr_record( info->LongHeaders[k++]);
#ifdef FOR_DUM_TERM
	if ( i < response->NumberOfRecordsReturned ) {
	  printf("\n\n  ... more to come,  enter 1 to continue or 0 to stop: ");
	  scanf("%ld", &continue_viewing);
	}
#endif
      }
    }

    if ( info->Text != 0 ) {
      k =0;
      while ( (continue_viewing == 1) && (info->Text[k] != 0) ) {
	i++;
	printf("\n    record %2d, ", i);
	dsply_text_record( info->Text[k++]);
#ifdef FOR_DUM_TERM
	if ( i < response->NumberOfRecordsReturned ) {
	  printf("\n\n  ... more to come,  enter 1 to continue or 0 to stop: ");
	  scanf("%ld", &continue_viewing);
	}
#endif
      }
    }

    if ( info->Headlines != 0 ) {
      k =0;
      while ( (continue_viewing ==1) && (info->Headlines[k] != 0) ) {
	i++;
	printf("\n    record %2d, ", i);
	dsply_headline_record( info->Headlines[k++]);
#ifdef DUM_TERM
	if ( i < response->NumberOfRecordsReturned ) {
	  printf("\n\n  ... more to come,  enter 1 to continue or 0 to stop: ");
	  scanf("%ld", &continue_viewing);
	}
#endif
      }
    }
         
    if ( info->Codes != 0 ) {
      k =0;
      while ( (continue_viewing ==1) && (info->Codes[k] != 0) ) {
	i++;
	printf("\n    record %2d, ", i);
	dsply_code_record( info->Codes[k++]);
#ifdef FOR_DUM_TERM
	if ( i < response->NumberOfRecordsReturned ) {
	  printf("\n\n  ... more to come,  enter 1 to continue or 0 to stop: ");
	  scanf("%ld", &continue_viewing);
	}
#endif
      }
    }

    freeWAISSearchResponse(info);         

  }				/* display user info */


  freeSearchResponseAPDU( response);

}				/* twais_dsply_srch_rsp_apdu */


/*
 * 'dsply_doc_hdr_record'
 *   display one WAIS document header record 
 */
dsply_doc_hdr_record( record)
WAISDocumentHeader  *record;
{
  printf(" DocHeaders\n");
  print_any("     DocumentID:  ", record->DocumentID);

  printf("     VersionNumber:  %ld\n", record->VersionNumber);

  printf("     Score:  %ld,  BestMatch:  %ld\n", 
	 record->Score, record->BestMatch);

  printf("     DocumentLength:  %ld,  Lines:  %ld\n", 
	 record->DocumentLength,
	 record->Lines);

  if ( record->Source )
    printf("     Source:  %s\n", record->Source);
  if ( record->Date  )
    printf("     Date:  %s\n", record->Date);

  if ( record->OriginCity )
    printf("     OriginCity:  %s\n", record->OriginCity);

  if ( record->Headline )
#ifdef FOR_DUM_TERM
    print_hdline(record->Headline);
#else
  printf("     Headline:  %s\n", record->Headline);
#endif
}

/*
 * 'dsply_short_hdr_record'
 *   display one WAIS short header record 
 */
dsply_short_hdr_record( record)
WAISDocumentShortHeader  *record;
{
  printf(" ShortHeaders\n");
  print_any("     DocumentID:  ", record->DocumentID);

  printf("     VersionNumber:  %ld\n", record->VersionNumber);
  printf("     Score:  %ld,  BestMatch:  %ld\n", 
	 record->Score, record->BestMatch);
  printf("     DocumentLength:  %ld,  Lines:  %ld\n", 
	 record->DocumentLength,  record->Lines);
}

static void print_docs _AP((DocObj** docs));

static void print_docs( docs)
DocObj **docs;
{
  long i;

  for ( i=0; docs[i] !=0; i++ ) {
    printf("    Document %ld:\n", i+1 );
    print_any("     DocID: ", docs[i]->DocumentID);
    if (docs[i]->Type != NULL)
      printf("     Type: %s\n",docs[i]->Type);
    printf("     ChunkCode: %ld\n", docs[i]->ChunkCode);
    switch ( docs[i]->ChunkCode ) {
    case CT_byte:
    case CT_line:
      printf("     Range: (%ld, %ld)\n", docs[i]->ChunkStart.Pos,
	     docs[i]->ChunkEnd.Pos);
      break;
    case CT_paragraph:
      print_any("     Chunk Start: ", docs[i]->ChunkStart.ID);
      print_any("     Chunk End:   ", docs[i]->ChunkEnd.ID);
      break;
    case CT_document:
    default:
      break;
    }
  }

}				/* print_docs */


/*
 * "twais_dsply_srch_apdu"
 *   display search apdu on stdout
 *   the  apdu is in the specified buffer
 */
void
twais_dsply_srch_apdu( buffer)
char *buffer;           /* (I) buffer contain the search apdu */
{
  SearchAPDU  *search;
  WAISSearch  *info;
  DocObj **docs;
  any *text_search;
  long i;

  printf("\n\n Search Request:\n");

  readSearchAPDU(&search,buffer);

  printf("    SmallSetUpperBound:      %ld\n", search->SmallSetUpperBound); 
  printf("    LargeSetLowerBound:      %ld\n", search->LargeSetLowerBound); 
  printf("    MediumSetPresentNumber:  %ld\n", search->MediumSetPresentNumber); 
  printf("    ReplaceIndicator:        %ld\n", search->ReplaceIndicator); 
  if ( search->ResultSetName != 0 )
    printf("    ResultSetName:           %s\n", search->ResultSetName); 
  if ( search->QueryType != 0 )
    printf("    QueryType:               %s\n", search->QueryType); 
  if ( search->DatabaseNames != 0 )
    for ( i=0; search->DatabaseNames[i] != 0 ; i++ )
      printf("    Databasenames[%ld]:          %s\n", i,
	     search->DatabaseNames[i]);

  if ( search->ElementSetNames != 0 )
    for ( i=0; search->ElementSetNames[i] != 0 ; i++ )
      printf("    ElementSetNames[%ld]:        %s\n", i,
	     search->ElementSetNames[i]);
  

  if ( search->ReferenceID != 0 )
    print_any("    ReferenceID:             ", search->ReferenceID);

  if ( search->Query != 0 ) {

    /* type 1 */
    if ( ! strcmp( search->QueryType, QT_TextRetrievalQuery) ) {
      text_search = (any *) search->Query;
      docs = readWAISTextQuery(text_search);
      if ( docs != 0 )
	print_docs( docs);
      freeAny(text_search);
      doList((void**)docs,freeDocObj);
      s_free(docs);
    }

    else if ( ! strcmp( search->QueryType, QT_RelevanceFeedbackQuery) ) {

      info = (WAISSearch *)search->Query;
      if ( info->SeedWords != 0 )
	printf("    SeedWords:               %s\n", info->SeedWords); 
      if ( info->Docs != 0 )
	print_docs( info->Docs);
      printf("    DateFactor: %ld\n", info->DateFactor);
      if ( info->BeginDateRange )
	printf("    BeginDateRange: %s\n", info->BeginDateRange);
      if ( info->EndDateRange )
	printf("    EndDateRange:   %s\n", info->EndDateRange);
      printf("    MaxDocumentsRetrieved: %ld\n", info->MaxDocumentsRetrieved);

      freeWAISSearch(info);         
    }
    else {
      printf(" Unrecognized query type\n");
    }
  }

  freeSearchAPDU( search);

}				/* twais_dsply_srch_apdu */


void twais_free_apdu(apdu_buff)
char *apdu_buff;        /* (I) buffer contain the apdu */
{
  pdu_type pdu;

  pdu = peekPDUType(apdu_buff);
  switch ( pdu) {
  case (initAPDU):
    freeInitAPDU((struct InitAPDU *)apdu_buff);
    break;
  case (initResponseAPDU):
    freeInitResponseAPDU((struct InitResponseAPDU *)apdu_buff);
    break;
  case (searchAPDU):
    freeSearchAPDU((struct SearchAPDU *)apdu_buff);
    break;
  case (searchResponseAPDU):
    freeSearchResponseAPDU((struct SearchResponseAPDU *)apdu_buff);
    break;
  default:
    break;
  }
  
}				/* twais_free_apdu */


/*----------------------------------------------------------------------------*
 *  template apdus for testing purpose                                        *
 *----------------------------------------------------------------------------*/
void
twais_tmplt_init_apdu(buff, buff_len)
char *buff;             /* (O) buffer to hold the apdu */
long *buff_len;          /* (O) number of bytes written to the buffer */
{
  InitAPDU *init;
  char  *end_ptr;
  long len;
  any refID;
  refID.size = 2;
  refID.bytes = "10";


  init = makeInitAPDU(WILL_USE,WILL_NOT_USE,WILL_NOT_USE,WILL_NOT_SUPPORT,
		      WILL_NOT_SUPPORT,
		      2048L,
		      2048L, NULL,
		      defaultImplementationID(),defaultImplementationName(),
		      defaultImplementationVersion(), &refID,NULL);
  { long buffer_len = MAX_BUFFER_LENGTH;
    end_ptr = writeInitAPDU(init,buff, &buffer_len);
  }
  len = (long)end_ptr - (long)&buff[0];
  *buff_len = len;
  freeInitAPDU(init);
}				/* twais_tmplt_init_apdu */


void
twais_tmplt_init_rsp_apdu(buff, buff_len)
char *buff;             /* (O) buffer to hold the apdu */
long *buff_len;          /* (O) number of bytes written to the buffer */
{
  WAISInitResponse *info;
  InitResponseAPDU *response;
  char  *end_ptr;
  long len;
  any refID;
  char chunk_marker[3];
  char highl_marker[2];
  char dhighl_marker[2];
  char new_line_chars[2];
  refID.size = 1;
  refID.bytes = "0";
  chunk_marker[0] = ESC;
  chunk_marker[1] = '1';
  chunk_marker[2]= 0;
  highl_marker[0] = DC1;
  highl_marker[1] = 0;
  dhighl_marker[0] = DC3;
  dhighl_marker[1] = 0;
  new_line_chars[0] = CARRIAGE_RETURN;
  new_line_chars[1] = 0;

  info =  makeWAISInitResponse(  CT_paragraph /* chunkCode */
			       ,3 /* chunkIDLen */
			       ,chunk_marker /* chunkMarker */
			       ,highl_marker /* highlightMarker */
			       ,dhighl_marker /* deHighlightMarker */
			       ,new_line_chars /* newLineChars */
			       );

  response = makeInitResponseAPDU( ACCEPT, /* result */
				  WILL_USE,WILL_NOT_USE,WILL_NOT_USE, /* search, rsp, del */
				  WILL_NOT_SUPPORT,WILL_NOT_SUPPORT, /* acc ctl, rsc ctl */
				  1024L, 2048L, /* preferred msg size, max msg size */
				  NULL,	/* authentication */
				  defaultImplementationID(),
				  defaultImplementationName(),
				  defaultImplementationVersion(),
				  &refID, /* reference id */
				  info);
  { long buffer_len = MAX_BUFFER_LENGTH;
    end_ptr = writeInitResponseAPDU(response, buff, &buffer_len);
  }
  len = (long)end_ptr - (long)&buff[0];
  *buff_len = len;

  CSTFreeWAISInitResponse( info);
  freeInitResponseAPDU(response);
}				/* twais_tmplt_init_rsp_apdu */



void twais_tmplt_typ1_srch_apdu( buff, buff_len)
char *buff;             /* (O) buffer to hold the apdu */
long *buff_len;          /* (O) number of bytes written to the buffer */
{

  SearchAPDU *search1;
  char  *end_ptr;
  long len;

  static char *database_names[2];
  any docID;
  any refID;

  DocObj *DocObjs[2];
  any *query;			/* changed from char* by brewster */
  database_names[0] = "Quest";
  database_names[1] = NULL;
  docID.size = 12;
  docID.bytes = "0000106776WJ";
  refID.size = 1;
  refID.bytes = "3";
   
  DocObjs[0] = makeDocObjUsingWholeDocument( &docID,NULL);
  DocObjs[1] = NULL;

  query = makeWAISTextQuery(DocObjs);   

  search1 = makeSearchAPDU( 10L, 16L, 15L, 
			   1L,	/* replace indicator */
			   "FOO", /* result set name */
			   database_names, /* database name */   
			   QT_TextRetrievalQuery, /* query_type */
			   0L,	/* element name */
			   &refID, /* reference ID */
			   query);

  {
    long buffer_len = MAX_BUFFER_LENGTH;
    end_ptr = writeSearchAPDU(  search1, buff, &buffer_len);
  }
  len = (long)end_ptr - (long)&buff[0];
  *buff_len = len;

  CSTFreeWAISTextQuery( query);
  freeSearchAPDU(search1);

}				/* twais_tmplt_typ1_srch_apdu */

#if 0

twais_tmplt_typ3_srch_apdu( buff, buff_len)
char *buff;             /* (O) buffer to hold the apdu */
long *buff_len;          /* (O) number of bytes written to the buffer */
{

  SearchAPDU *search3;
  char  *end_ptr;
  static char *database_names[2];
  long len;
  any refID;
  WAISSearch *query;
  database_names[0] = "Quest"
    database_names[1] = NULL;
  refID.size = 1;
  refID.bytes = "3";

  query = makeWAISSearch( "Supercomputers in Taiwan", /* seed_words*/
			 0L,	/* DocObjsPtr */
			 0L,
			 1L,	/* DateFactor */
			 0L,	/* BeginDateRange */
			 0L,	/* EndDateRange */
			 10L	/* maxDocsRetrieved */
			 );

  search3 = makeSearchAPDU( 10L, 16L, 15L, 
			   1L,	/* replace indicator */
			   "FOO", /* result set name */
			   database_names, /* database name */   
			   QT_RelevanceFeedbackQuery, /* query_type */
			   0L,	/* element name */
			   &refID, /* reference ID */
			   query);
  {
    long buffer_len = MAX_BUFFER_LENGTH;
    end_ptr = writeSearchAPDU(  search3, buff, &buffer_len);
  }
  len = (long)end_ptr - (long)&buff[0];
  *buff_len = len;

  CSTFreeWAISSearch( query);
  freeSearchAPDU(search3);

}				/* twais_tmplt_typ3_srch_apdu */
#endif


void twais_tmplt_typ3_srch_rsp_apdu( buff, buff_len)
char *buff;
long *buff_len;
{

  char  *end_ptr;
  long len;
  any refID;

  WAISDocumentHeader  *doc_headers[3];
  WAISSearchResponse *records;
  SearchResponseAPDU *response;
  any doc_id1;
  any doc_id2;
  refID.size = 1;
  refID.bytes = "1";
  doc_id1.size = 12;   
  doc_id1.bytes = "0000106776WJ";
  doc_id2.size = 12;
  doc_id2.bytes = "0000026870WP";

  doc_headers[0] = makeWAISDocumentHeader(
					  &doc_id1, /* docID */
					  1L, /* versionNumber */
					  80L, /* score */
					  1L, /* bestMatch */
					  850L, /* docLen */
					  200L, /* lines */
					  NULL,	/* types */
					  "Source1", /* source */
					  "19900115", /* date */
					  "CRAY sells supercomputer to Taiwan",	/* headline */
					  "New York"); /* originCity */


  doc_headers[1] = makeWAISDocumentHeader(
					  &doc_id2, /* docID */
					  1L, /* versionNumber */
					  60L, /* score */
					  1L, /* bestMatch */
					  550L, /* docLen */
					  100L, /* lines */
					  NULL,
					  "Test Source", /* source */
					  "19900110", /* date */
					  "Test Headline", /* headline */
					  "Test City");	/* originCity */

  doc_headers[2] = 0;

  records = makeWAISSearchResponse( "Supercomputer Taiwan" /* seedWordsUsed*/
				   ,doc_headers	/* docHeaders */
				   ,0 ,0 ,0 ,0 /* shortHeaders, longHeaders, text, headlines */
				   ,0 /* codes */
				   ,NULL /* diagnostics.  KLUDGE */
				   );


  response = makeSearchResponseAPDU( SUCCESS /* result */
				    ,2 /* count */
				    ,2 /* recordsReturned */
				    ,0 /* nextPos */
				    ,0 /* ignore resultStatus since result SUCCESS */
				    ,SUCCESS /* presentStatus */
				    ,&refID /* refID */
				    ,records);


  {
    long buffer_len = MAX_BUFFER_LENGTH;
    end_ptr = writeSearchResponseAPDU(  response, buff, &buffer_len);
  }
  len = (long)end_ptr - (long)&buff[0];
  *buff_len = len;

  CSTFreeWAISDocumentHeader( doc_headers[0]);

  CSTFreeWAISDocumentHeader( doc_headers[1]);

  CSTFreeWAISSearchResponse( records);
  freeSearchResponseAPDU(response);


}				/* twais_tmplt_typ3_srch_rsp_apdu */


twais_tmplt_typ1_stry_rsp_apdu( buff, buff_len)
char *buff;
long *buff_len;
{

  char  *end_ptr;
  long len;
  any refID;

  WAISDocumentText *doc_text[2];
  WAISSearchResponse *records;
  SearchResponseAPDU *response;
  any docID;
  any story;
  char *story_buff;
  
  FILE *fptr;
  refID.size = 1;
  refID.bytes = "1";
  docID.size = 12;
  docID.bytes = "0000106776WJ";
  doc_text[0] = 0;
  doc_text[1] = 0;

  fptr = s_fopen("twais_template.txt", "r");
  if (fptr == NULL ) {
    printf(" unable to open story text file \n");
    return;
  }
   
  /* read story length */
  fread((char*)&story.size, sizeof(long), 1, fptr);
  story_buff = s_malloc( story.size +1);
  if ( story_buff == NULL) {
    printf(" insufficient memory\n");
    s_fclose( fptr);
    return;
  }

  /* read story text */
  fread( story_buff, 1, story.size, fptr);   
  story.bytes = story_buff;
   
  doc_text[0] = makeWAISDocumentText( &docID, 1L, &story);

  records = makeWAISSearchResponse( 0 /* seedWordsUsed*/
				   ,0 ,0 ,0 /* docHeaders, shortHeaders, longHeaders */
				   ,doc_text ,0	/* text, headlines */
				   ,0 /* codes */
				   ,NULL /* diagnostics.  KLUDGE */
				   );


  response = makeSearchResponseAPDU( SUCCESS /* result */
				    ,1 /* count */
				    ,1 /* recordsReturned */
				    ,0 /* nextPos */
				    ,0 /* ignore resultStatus since result SUCCESS */
				    ,SUCCESS /* presentStatus */
				    ,&refID /* refID */
				    ,records);


  {
    long buffer_len = MAX_BUFFER_LENGTH;
    end_ptr = writeSearchResponseAPDU(  response, buff, &buffer_len);
  }
  len = (long)end_ptr - (long)&buff[0];
  *buff_len = len;

  CSTFreeWAISDocumentText( doc_text[0]);

  CSTFreeWAISSearchResponse( records);

  freeSearchResponseAPDU(response);

  s_free(story_buff);
  s_fclose(fptr);

}				/* twais_tmplt_typ1_stry_rsp_apdu */


twais_tmplt_typ3_srch_apdu( buff, buff_len)
char *buff;             /* (O) buffer to hold the apdu */
long *buff_len;          /* (O) number of bytes written to the buffer */
{

  SearchAPDU *search3;
  char  *end_ptr;
  static char *database_names[7] = { 
    "11111111111111111111111111111111111111111111111111",
    "22222222222222222222222222222222222222222222222222",
    "33333333333333333333333333333333333333333333333333",
    "44444444444444444444444444444444444444444444444444",
    "55555555555555555555555555555555555555555555555555",
    "66666666666666666666666666666666666666666666666666",
    0};
  static char *elem_names[7] = { ES_DocumentHeader
				   ,ES_DocumentShortHeader
				     ,ES_DocumentLongHeader
				       ,ES_DocumentText
					 ,ES_DocumentHeadline
					   ,ES_DocumentCodes
					     ,0};

  long len;          
  any refID;
  WAISSearch *query;
  refID.size = 1;
  refID.bytes = "3";

  query = makeWAISSearch( 
			 "What is the penalities for driving without insurance",
			 0L,	/* DocObjsPtr */
			 0L,
			 1L,	/* DateFactor */
			 0L,	/* BeginDateRange */
			 0L,	/* EndDateRange */
			 10L	/* maxDocsRetrieved */
			 );

  search3 = makeSearchAPDU( 10L, 16L, 15L, 
			   1L,	/* replace indicator */
			   "FOO", /* result set name */
			   database_names, /* database name */   
			   QT_RelevanceFeedbackQuery, /* query_type */
			   elem_names, /* element name */
			   &refID, /* reference ID */
			   query);
  {
    long buffer_len = MAX_BUFFER_LENGTH;
    end_ptr = writeSearchAPDU(  search3, buff, &buffer_len);
  }
  len = (long)end_ptr - (long)&buff[0];
  *buff_len = len;

  CSTFreeWAISSearch( query);
  freeSearchAPDU(search3);

}				/* twais_tmplt_typ3_srch_apdu */

