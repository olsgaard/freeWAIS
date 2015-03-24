/*                               -*- Mode: C -*- 

 * waisindex.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Jonny Goldman
 * Created On      : Fri Jun 23 14:08:30 1995
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Mon Dec 18 14:18:43 1995
 * Language        : C
 * Update Count    : 22
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 * $$
 */

/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#ifndef lint
static char    *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/waisindex.c,v 2.0.1.5 1995/12/18 13:53:51 pfeifer Exp $";

#endif

/*
 * Building an index with a Unix shell interface.
 *
 * -brewster 6/90
 */

/* Change log:
 * added -stdio option from jik@athena.mit.edu
 * $Log: waisindex.c,v $
 * Revision 2.0.1.5  1995/12/18 13:53:51  pfeifer
 * patch56: Rename char* keyword to char* keywrd sice it conficts with
 * patch56: keywords() on some systems.
 *
 * Revision 2.0.1.4  1995/12/06  11:04:15  pfeifer
 * patch53: Added dbm stuff from egw@aubranch.egwestate.andrews.edu.
 *
 * Revision 2.0.1.3  1995/11/09  14:07:56  pfeifer
 * patch46: Force rewrite of source description now.
 *
 * Revision 2.0.1.2  1995/09/15  09:49:38  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.2  1995/09/15  09:49:38  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.1  1995/09/11  12:56:09  pfeifer
 * patch4: Fixed declaration of number_of_files in main().
 *
 * Revision 2.0  1995/09/08  08:02:30  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.35  1995/06/23  12:15:04  pfeifer
 * unifdef -DBOOLEANS -DNESTED_BOOLEANS -DFIELDS | indent
 *
 * Revision 1.34  1995/04/17  13:19:30  huynh1
 * Release 1.1.2
 *
 * Revision 1.33  1995/02/10  10:33:35  pfeifer
 * Release 1.1.2
 *
 * Revision 1.32  1994/12/13  18:54:55  pfeifer
 * removed the host_name ...
 *
 * Revision 1.31  1994/12/13  18:52:45  pfeifer
 * chip@chinacat.unicom.com (Chip Rosenthal) patches.
 * Excluding the merge of libinv and libwais
 *
 * Revision 1.30  1994/12/12  16:23:58  pfeifer
 * added char host_name[255], host_address[255]; which are needed in
 * sockets.c
 *
 * Revision 1.29  1994/10/10  12:02:18  pfeifer
 * replaced printf's by waislog
 *
 * Revision 1.28  1994/08/19  10:13:27  pfeifer
 * Usage of -tfields now disabels -pos
 *
 * Revision 1.27  1994/08/16  11:45:29  pfeifer
 * Added call to free_keywords to make purify happy
 *
 * Revision 1.26  1994/08/11  14:16:27  pfeifer
 * changed the default of the wordpair flag to "-nopairs"
 *
 * Revision 1.25  1994/08/08  07:32:56  pfeifer
 * Moved wais_log_file_name and waislogfile to cutil.[ch]
 *
 * Revision 1.23  1994/07/13  07:52:36  huynh1
 * Uli
 *
 * Revision 1.22  1994/06/27  14:02:00  huynh1
 * error message for option -stop added.
 *
 * Revision 1.21  1994/06/21  14:27:31  huynh1
 * bug by producing empty file.cat corrected.
 *
 * Revision 1.20  1994/05/27  11:07:46  huynh1
 * error message for not exsting file. beta
 *
 * Revision 1.19  1994/05/20  12:58:34  pfeifer
 * beta
 *
 * Revision 1.18  1994/05/02  13:14:51  pfeifer
 * fixed quotation of '%' in format strings
 *
 * Revision 1.17  1994/05/02  13:12:17  huynh1
 * -stem option.
 *
 * Revision 1.16  1994/04/26  16:28:19  huynh1
 * -stop option added.
 * patchlevel 09.
 *
 * Revision 1.15  1994/03/30  13:27:42  huynh1
 * test
 *
 * Revision 1.12  1994/03/28  09:29:21  pfeifer
 * Integrated Essence
 *
 * Revision 1.11  1994/03/23  12:56:29  huynh1
 * calling of init_add_word modified.
 * patchlevel 07.
 *
 * Revision 1.10  1994/03/10  17:34:21  huynh1
 * Patchlevel 05
 *
 * Revision 1.9  1994/03/08  20:46:20  huynh1
 * Patchlevel 04
 *
 * Revision 1.8  1994/02/15  10:56:57  huynh1
 * option -nfields added.
 *
 * Revision 1.7  1994/02/14  10:27:21  huynh1
 * option -fields field_description_file added.
 *
 * Revision 1.8  1993/10/12  11:18:25  pfeifer
 * Added stopword file for document style bibdb
 *
 * Revision 1.7  1993/09/22  16:09:13  pfeifer
 * What have i done ?
 *
 * Revision 1.6  1993/06/04  10:23:15  pfeifer
 * Pachtlevel BIBDB
 *
 * Revision 1.5  1993/06/02  18:29:00  pfeifer
 * Added code for local formats
 *
 * Revision 1.4  1993/06/01  14:05:54  pfeifer
 * Added code for soundex/phonix indexing and retrieval
 *
 * Revision 1.3  1993/02/16  17:07:49  freewais
 * added AT&T patches for keyword list
 *
 * Revision 1.2  1993/02/16  15:32:21  freewais
 * added AT&T patch to write first 50 dictionary entries to
 * src file
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.47  92/05/10  14:48:17  jonathan
 * Updated for release.
 * 
 * Revision 1.46  92/05/08  10:03:17  jonathan
 * Adjusted memory paramters.  It's closer...
 * 
 * Revision 1.45  92/05/06  17:26:46  jonathan
 * Added switch for indexing contents, new user-specified type name, new type:
 * filename, which only puts the name of the file in the header.
 * 
 * Revision 1.44  92/04/25  21:14:35  brewster
 * added ziff
 * 
 * Revision 1.43  92/04/22  15:29:13  jonathan
 * Added jargon to usage message.
 * 
 * Revision 1.42  92/04/01  17:08:50  jonathan
 * Added FTP type.
 * 
 * Revision 1.41  92/03/25  18:49:39  jonathan
 * Added log_level and log_file arguments.
 * 
 * Revision 1.40  92/03/22  18:38:14  brewster
 * added objective C filter
 * 
 * Revision 1.39  92/03/20  11:02:44  jonathan
 * Added code to handle switches for word_pairs and word_postition info.
 * 
 * Revision 1.38  92/03/17  07:34:32  jonathan
 * Fixed spacing in usage message.
 * 
 * Revision 1.37  92/03/10  10:42:51  morris
 * fixed small bug in command line argument handleing.  doesn't die if there
 * are no args.
 * 
 * Revision 1.36  92/03/05  07:05:32  shen
 * add cm grow percent and textsize to command line and init search engine
 * 
 * Revision 1.35  92/03/04  16:34:09  jonathan
 * Set wais_pid from getpid().
 * 
 * Revision 1.34  92/02/20  09:49:37  jonathan
 * Added bibtex and nhyp filters from S.P.vandeBurgt@research.ptt.nl.
 * 
 * Revision 1.33  92/02/17  14:21:08  jonathan
 * Added switch to disable creation of catalog (-nocat).
 * 
 * Revision 1.32  92/02/17  12:41:55  jonathan
 * Added RCSid.
 * 
 * Revision 1.31  92/02/17  12:41:01  jonathan
 * Build catalog after completion of indexing.
 * 
 * Revision 1.30  92/02/12  13:22:53  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
 */

/* to do:
 *   done: make incremental indexing not index things that are already index
 *   add extra arg -register that will send in description of the server to 
 *           the directory of servers.
 *   done: create a source struct in the .src file
 *   make it continuously index to keep itself uptodate.
 *
 */

#include <string.h>
#include <sys/types.h>
#include <sys/param.h>
#include "cutil.h"
#include "futil.h"
#include "irfiles.h"
#include "irtfiles.h"
#include "panic.h"
#include "ircfiles.h"
#include "irext.h"
#include "stoplist.h"		/* dgg */

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

#include "field.h"
boolean         field_adding_to_existing_index = false;
boolean         force_rewriting_of_sorce_description = true;

#ifdef PROXIMITY
extern boolean  wais_proximity_unit_line;

#endif
#ifdef BIO
#define INDEXER_DATE "2/16/93"
#else
#define INDEXER_DATE "2/16/93"
#endif
#define MAX_LINE_LENGTH 1000

extern char    *keywrd[50], *descript[1000];
extern short    nKeys, nDesLines;

extern double   compare ();

/* for reporting errors, in WAIStation it is defined in CRetrievalApp.c */

extern boolean  indexingForBeta;

#ifdef USE_DBM
/*
Permits standard Unix System V Release 3.2 'dbm' files.

Suppose you have a file named '/d9/data/nature/tree/mango.doc' and the
headline for that file is 'Mangos? Who Said Mangos? What a Delicious Fruit!'.
This headline cannot be found anywhere in the file AND you don't want to
modify the datafile.  Use the 'dbm' routines to create the *.dir and *.pag
files using the filename(s) as the key and your desired headline(s) as the
contents BEFORE you start indexing.

Suppose your final DBM files are '/tmp/treedbm.dir' and '/tmp/treedbm.pag',
use the following options to 'waisindex':

        -dbm_file /tmp/treedbm -t textdbm

With these options 'waisindex' will use the DBM routines to retrieve the
headline.


So in our example,

        DBM_KEY = /d9/data/nature/tree/mango.doc
        DBM_CONTENTS = Mangos? Who Said Mangos?  What a Delicious Fruit!

And DBM_CONTENTS will be stored by the WAIS routines as the headline for that
file.

egw@aubranch.egwestate.andrews.edu, Ellen G. White Estate, July 1995
*/
#include <dbm.h>

char *dbm_file_name;
int  dbm_file_status;

#endif

void
usage (command)
     char           *command;
{				/* no args */
  fprintf (stderr, "Usage: %s [-d index_filename]\n", command);
  fprintf (stderr, "          [-a] /* adding to an existing index, otherwise it erases the index */\n");
#ifdef STEM_WORDS
  fprintf (stderr, "          [-stem] /* stemming for global database */\n");
#endif
  fprintf (stderr, "          [-r] /* recursively index subdirectories */\n");
  fprintf (stderr, "          [-mem mbytes] /* number of megabytes to run this in */\n");
  fprintf (stderr, "          [-register] /* registers the database with the directory of servers.\n");
  fprintf (stderr, "                         This should be done with care. */\n");
  fprintf (stderr, "          [-export] /* uses short dbname and port 210 */\n");
  fprintf (stderr, "          [-e [file]] /* set log output to file, or /dev/null if not specified */\n");
  fprintf (stderr, "          [-l log_level] /* set log level.  0 means log nothing,\n");
  fprintf (stderr, "                            10 [the default] means log everything */\n");
  fprintf (stderr, "          [-v] /* print the version of the software */\n");
  fprintf (stderr, "          [-stdin] /* read file names from stdin */\n");
  fprintf (stderr, "          [-pos | -nopos] /* include (don't include - default) word position information /*\n");
  fprintf (stderr, "          [-nopairs | -pairs] /* don't include (or include - default is nopairs) word pairs /*\n");
  fprintf (stderr, "          [-nocat] /* inhibit creation of catalog /*\n");
  fprintf (stderr, "          [-contents] /* Index the contents: this is good for types that\n");
  fprintf (stderr, "                         inhibit the indexing of the contents (like gif). /*\n");
  fprintf (stderr, "          [-nocontents] /* Index only the filename, not the contents /*\n");
#ifdef AUTO_STOPWORDS		/* tung, 2/95 */
  fprintf (stderr, "          [-stop] /* stopfile of common words to ignore. Stopfile is database.stop. Use this option the stoplist coded in WAIS will not be used */\n");
#else
  fprintf (stderr, "          [-stop stoplist_filename] /* file of common words to ignore */\n");
#endif
#ifdef BIO
  fprintf (stderr, "          [-stop stoplist_filename] /* file of common words to ignore */\n");
  fprintf (stderr, "          [-delim delimiters] /* list of word delimiter symbols */\n");
#endif
  fprintf (stderr, "          [-keywords \"<string>\"] /* Keywords to index for each document. */\n");
  fprintf (stderr, "          [-keyword_file <filename>] /* File of keywords to index. */\n");

  fprintf (stderr, "          [-cmmem  mem\045] /* percent of CM memory (CM code only) */\n");
#ifdef USE_DBM
/*
Permits standard Unix System V Release 3.2 'dbm' files.

egw@aubranch.egwestate.andrews.edu, Ellen G. White Estate, July 1995
*/

  fprintf(stderr,"          [-dbm_file <filename>] /* location of filename.dir and filename.pag. Use with -t textdbm.  */\n");
#endif
  fprintf (stderr, "          [-T  type] /* type becomes the \"TYPE\" of the document. */\n");
/* multitype extensions */
  fprintf (stderr, "          [-M  type,type] /* for multi-type documents. */\n");
#ifdef PROXIMITY
  fprintf (stderr, "          [-line] /* use lines - not words - in proximity operations. */\n");
#endif
  fprintf (stderr, "          [-t	/* format of the file. if none then each file is a document */\n");
  fprintf (stderr, "             text /* simple text files, this is the default */\n");
  fprintf (stderr, "           | bibtex /* BibTeX / LaTeX format */\n");
  fprintf (stderr, "           | bio /* biology abstract format */\n");
  fprintf (stderr, "           | cmapp /* CM applications from Hypercard */\n");
  fprintf (stderr, "           | dash /* entries separated by a row of dashes */\n");
  fprintf (stderr, "           | dvi /* dvi format */\n");
  fprintf (stderr, "           | emacsinfo /* the GNU documentation system */\n");
#ifdef ESSENCE
  fprintf (stderr, "           | essence /* CU-Boulder Essence Summaries */\n");
#endif /* ESSENCE */
  fprintf (stderr, "           | first_line /* first line of file is headline */\n");
  fprintf (stderr, "           | filename /* uses only the filename part of the pathname for the title */\n");
  fprintf (stderr, "           | ftp /* special type for FTP files.  First line of file is headline */\n");
  fprintf (stderr, "           | gif /* gif files, only indexes the filename */\n");
  fprintf (stderr, "           | irg /* internet resource guide */\n");
  fprintf (stderr, "           | jargon /* Jargon File 2.9.8 format*/\n");
  fprintf (stderr, "           | mail_digest /* standard internet mail digest format */\n");
  fprintf (stderr, "           | mail_or_rmail /* mail or rmail or both */\n");
  fprintf (stderr, "           | medline /* medline format */\n");
  fprintf (stderr, "           | mh_bboard /* MH bulletin board format */\n");
  fprintf (stderr, "           | netnews /* netnews format */\n");
  fprintf (stderr, "           | nhyp /* ?:? hyper text format, Polytechnic of Central London */\n");
  fprintf (stderr, "           | one_line /* each line is a document */\n");
  fprintf (stderr, "           | para /* paragraphs separated by blank lines */\n");
  fprintf (stderr, "           | pict /* pict files, only indexes the filename */\n");
  fprintf (stderr, "           | ps /* postscript format */\n");
  fprintf (stderr, "           | refer /* refer format */\n");
#ifdef BIBDB
  fprintf (stderr, "           | irlist           /* irlist mail or rmail or both */\n");
  fprintf (stderr, "           | formfeed         /* entries separated by a formfeed */\n");
  fprintf (stderr, "           | bibdb            /* steve file entries separated by a formfeed */\n");
  fprintf (stderr, "           | bibinf           /* bibinf entries separated by an empty line */\n");
#endif
  fprintf (stderr, "           | rn /* netnews saved by the [rt]?rn newsreader */\n");
  fprintf (stderr, "           | server /* server structures for the dir of servers */\n");
#ifdef NeXT
  fprintf (stderr, "           | objc /* objective-C .h and .m files */\n");
#endif /* def NeXT */
  fprintf (stderr, "           | tiff /* tiff files, only indexes the filename */\n");
  fprintf (stderr, "           | URL what-to-trim what-to-add /* URL */\n");
#ifdef BIO
  fprintf (stderr, "           | genbank  /* GenBank flatfile format */\n");
  fprintf (stderr, "           | embl     /* EMBL flatfile format */\n");
  fprintf (stderr, "           | pir     /* PIR flatfile format */\n");
  fprintf (stderr, "           | prositedoc /* Prosite protein doc format */\n");
  fprintf (stderr, "           | prositedat /* Prosite protein dat format */\n");
  fprintf (stderr, "           | biojournal /* Bio journal TOC on bionet.journals */\n");
  fprintf (stderr, "           | redbook  /* Drosophila redbook text */\n");
  fprintf (stderr, "           | flybase  /* Drosophila Ashburner data files */\n");
  fprintf (stderr, "           | flystock  /* Drosophila stock lists */\n");
  fprintf (stderr, "           | din      /* Drosophila Info. Newsletter */\n");
#endif
#ifdef SOUND
  fprintf (stderr, "           | oneline_phonix   /* Phonebooks PHONIX */\n");
  fprintf (stderr, "           | oneline_soundex  /* Phonebooks SOUNDEX */\n");
#endif
  fprintf (stderr, "           | listserv_digest /* standard internet mail digest format */\n");
#ifdef USE_DBM
/*
Permits standard Unix System V Release 3.2 'dbm' files.

egw@aubranch.egwestate.andrews.edu, Ellen G. White Estate, July 1995
*/
  fprintf(stderr,"           | textdbm /* Get the title from <filename.dir> and <filename.pag>, keyed by the filename being indexed */\n");
#endif
#ifdef STELAR
  fprintf (stderr, "           | AAS_abstract /* AAS meeting abstracts using AAS LaTeX macros */\n");
  fprintf (stderr, "           | stelar /* stelar abstracts - third line is hl */\n");
#endif /* STELAR */
  fprintf (stderr, "           | fields /* document format given in <database>.fmt */\n");
  fprintf (stderr, "          ] filename filename ...\n");
  fprintf (stderr, "-nfields /* create new fields, old fields not deleted, by default all fields will be deleted*/\n");
}

extern char    *keywords;		    /* used in irtfiles.c */
extern char    *keyword_filename;	    /* used in irtfiles.c */

extern boolean  index_contents;

#define BLOCK_FILES_FROM_STDIN 100	/* Number of files in each block reading from stdin */
int             current_max_files = 0;

/* This is the MAIN for building an index.
 */
void
main (argc, argv)
     int             argc;
     char           *argv[];
{
  char          **data_filename_array;
  boolean         global_dictionary_exists = false;
  boolean         index_field = false;
  boolean         index_next_field = false;
  boolean         create_new_fields = false;
  boolean         read_files_from_stdin_array = false;
  char           *first_filename;
  long            prev_number_of_fields = 0;
  long            prev_argc;
  int             number_of_files = 0;
  long            number_of_data_filename = 0;
  long            doc_table_allocated_entries;
  long            i = 0;
  long            mesg;

#ifdef STEM_WORDS
  boolean         stemming = false;

#endif
  database       *db = NULL;
  long            argc_copy = argc;
  char          **argv_copy = argv;
  char           *next_argument;
  char            index_filename[1000];
  boolean         adding_to_existing_index = false;
  boolean         traverse_directory = false;
  boolean         word_positions = false;
  boolean         word_pairs = false;
  long            memory_to_use = -1;
  long            cm_mem_percent = 0;	    /* default */
  long            grow_percent = 0;	    /* default */
  long            text_size = 0;	    /* default */
  boolean         check_for_text_file = false;
  boolean         register_database = false;
  boolean         export_database = false;
  boolean         read_files_from_stdin = false;
  boolean         make_catalog = true;
  char            data_filename[MAXPATHLEN];
  char           *typename = NULL;	    /* this is what the user said */
  long            start_of_filenames;
  long            hashtable_size = 1L << 16;
  long            flush_after_n_words = 300000;
  char           *command_name;

  dataopsrec      dataops;

  /*-------------   these go into dataops   
  boolean (*separator_function)();
  void (*header_function)();
  void (*finish_header_function)();
  long (*date_function)();
  char *type = NULL;     
  int minwordlen= 2;	 
  ---------------*/

  /* dgg -- put all of these separate, datatype-specific functions & params into a record! */
  gDelimiters[0] = '\0';	/* <-- bombs ?? */
  dataops.separator_function = NULL;
  dataops.header_function = NULL;
  dataops.date_function = NULL;
  dataops.finish_header_function = NULL;
  dataops.type = "TEXT";
#ifdef SOUND
  dataops.indextype = NULL;
#endif
  dataops.prev_indextype = NULL;
  dataops.multitype = NULL;
  dataops.addseparatorwords = false;
  dataops.extraheaderweight = true;
  dataops.repeat_weight = 1;
  dataops.minwordlen = 2;
  dataops.wordDelimiter = wordbreak_notalnum;
  dataops.delimiters = gDelimiters;
  wordDelimiter = wordbreak_notalnum;

  /*------
  separator_function = NULL; 
  header_function = NULL;
  date_function = NULL;
  finish_header_function = NULL;
  type = "TEXT";  
  -------*/
  typename = "Text";

  next_argument = next_arg (&argc, &argv);
  command_name = next_argument;

  waislogfile = stderr;
  wais_pid = getpid ();

  if (0 == argc) {
    usage (command_name);
    exit (0);
  }
#ifdef THINK_C
  strcpy (index_filename, "wais:System Folder:wais-index:index");
#else
  strcpy (index_filename, "index");	/* in the current directory */
#endif /* THINK_C */
  stop_list_file ("\0");	/* dgg */

  if (NULL == (next_argument = next_arg (&argc, &argv))) {
    fprintf (stderr, "No arguments specified\n");
    exit (0);
  }
  while ((next_argument != NULL) && '-' == next_argument[0]) {
    /* then we have an argument to process */
    if ((0 == strcmp ("-i", next_argument)) ||	/* -i is for backcompatibility */
	(0 == strcmp ("-d", next_argument))) {
      if (NULL == (next_argument = next_arg (&argc, &argv))) {
	fprintf (stderr, "Expected filename for the index\n");
	exit (0);
      }
      strcpy (index_filename, next_argument);
    }
#ifdef AUTO_STOPWORDS		/* tung, 2/95 */
    else if (0 == strcmp ("-stop", next_argument)) {
      use_both_stoplist = false;	/* use only the stoplist external */
    }
#else
    else if (0 == strcmp ("-stop", next_argument)) {	/* dgg, stoplist file */
      if (NULL == (next_argument = next_arg (&argc, &argv))) {
	fprintf (stderr, "Expected filename for the stoplist\n");
	exit (0);
      }
      stop_list_file (next_argument);
    }
#endif
#ifdef BIO
    else if (0 == strcmp ("-stop", next_argument)) {	/* dgg, stoplist file */
      if (NULL == (next_argument = next_arg (&argc, &argv))) {
	fprintf (stderr, "Expected filename for the stoplist\n");
	exit (0);
      }
      stop_list_file (next_argument);
    } else if (0 == strcmp ("-delim", next_argument)) {		/* dgg, delimiters */
      if (NULL == (next_argument = next_arg (&argc, &argv))) {
	fprintf (stderr, "Expected the delimiters argument\n");
	exit (0);
      }
      strcpy (gDelimiters, next_argument);
    }
#endif

    else if (0 == strcmp ("-a", next_argument)) {
      adding_to_existing_index = true;
      field_adding_to_existing_index = true;
    }
#ifdef STEM_WORDS
    else if (0 == strcmp ("-stem", next_argument)) {
      stemming = true;
    }
#endif
    else if (0 == strcmp ("-r", next_argument)) {
      traverse_directory = true;
    } else if (0 == strcmp ("-register", next_argument)) {
      register_database = true;
    } else if (0 == strcmp ("-export", next_argument)) {
      export_database = true;
    } else if (0 == strcmp ("-v", next_argument)) {
      fprintf (stderr, "%s: %s %s\n", command_name, VERSION_STRING, INDEXER_DATE);
    } else if (0 == strcmp ("-stdin", next_argument)) {
      read_files_from_stdin = true;
    } else if (0 == strcmp ("-nopos", next_argument)) {
      word_positions = false;
    } else if (0 == strcmp ("-pos", next_argument)) {
      if (index_field) {
	waislog (WLOG_MEDIUM, WLOG_WARNING,
		 "Disabeling -pos with field indexing\n");
      } else {
	word_positions = true;
      }
    } else if (0 == strcmp ("-nopairs", next_argument)) {
      word_pairs = false;
    } else if (0 == strcmp ("-pairs", next_argument)) {
      word_pairs = true;
    } else if (0 == strcmp ("-nocat", next_argument)) {
      make_catalog = false;
    } else if (0 == strcmp ("-mem", next_argument)) {
      if (NULL == (next_argument = next_arg (&argc, &argv)))
	panic ("Expected a number for the amount of memory to use");
      memory_to_use = atol (next_argument);
      if (memory_to_use < 1)
	panic ("The -mem argument should not be less than 1");
      if (memory_to_use > 200)
	fprintf (stderr, "Warning: The -mem parameter was %ld Mbytes.  That is a large number of mega bytes in current machines\n", memory_to_use);
    } else if (0 == strcmp ("-cmmem", next_argument)) {
      if (NULL == (next_argument = next_arg (&argc, &argv)))
	panic ("Expected a number (1-100) for percentage of memory to use");
      cm_mem_percent = atol (next_argument);
      if (cm_mem_percent < 1)
	panic ("The -cmmem argument should not be less than 1 and less than 100");
      if (cm_mem_percent > 100)
	panic ("Warning: The -cmmem parameter was %ld\045. It should be between 1-100.", cm_mem_percent);
    } else if (0 == strcmp ("-grow", next_argument)) {
      if (NULL == (next_argument = next_arg (&argc, &argv)))
	panic ("Expected a number (1-100) for database growing percentage");
      grow_percent = atol (next_argument);
      if (grow_percent < 1)
	panic ("The -grow argument should not be less than 1");
    } else if (0 == strcmp ("-textsize", next_argument)) {
      if (NULL == (next_argument = next_arg (&argc, &argv)))
	panic ("Expected a number for text size in megabytes");
      text_size = atol (next_argument);
      if (text_size < 1)
	panic ("The -textsize argument should not be less than 1");
    } else if (0 == strcmp ("-e", next_argument)) {
      char           *peek_argument = peek_arg (&argc, &argv);

      wais_log_file_name = "/dev/null";		/* default to /dev/null */
      if ((peek_argument != NULL) &&
	  ('-' != peek_argument[0])) {
	wais_log_file_name = next_arg (&argc, &argv);
      }				/* end if (explicit log file) */
    }
    /* end if (-e) */
#ifdef PROXIMITY
    else if (0 == strcmp ("-line", next_argument)) {
      wais_proximity_unit_line = true;	/* Use lies - not words - as proximity units */
    }
#endif
    else if (0 == strcmp ("-l", next_argument)) {
      wais_log_level = atol (next_arg (&argc, &argv));
    }
    /* end if (-l) */
    else if (0 == strcmp ("-cm", next_argument)) {
      /* this is an undocumented argument to help use this to
         front end the CM application */
      indexingForBeta = true;
#ifdef USE_DBM
/*
Permits standard Unix System V Release 3.2 'dbm' files.

egw@aubranch.egwestate.andrews.edu, Ellen G. White Estate, July 1995
*/
    else if (0 == strcmp("-dbm_file", next_argument))
       {
		if (NULL == (next_argument = next_arg(&argc, &argv)))
                  {
			fprintf(stderr,"Expected filename for the 'dbm' option \n");
			exit(0);
       	          }

		dbm_file_name = next_argument;
		dbm_file_status = dbminit(dbm_file_name);
		if (dbm_file_status < 0)
		  {
			fprintf(stderr,"Warning: dbminit() on filename '%s' failed. Option ignored.\n",dbm_file_name);
			exit(0);
	          }
       } /* END DBM ELSE */
#endif
    } else if (0 == strcmp ("-T", next_argument)) {
      /* This is a specification for a "Special" type.  The next argument
         is the type name.  This will not index the body of the file. */
      if (NULL == (next_argument = next_arg (&argc, &argv)))
	panic ("Expected a file type");
      typename = next_argument;
      dataops.type = next_argument;
      fprintf (stderr, "irbuild: setting type to %s\n", next_argument);
      dataops.finish_header_function = filename_finish_header_function;
    }
/* multitype extensions */
/* 
   This is a specification for a multi-type document, the types should
   be entered as a comma delimited list.  Note that this only defines
   all the types available in the database, you also need to specify a
   -t option so that the indexer knows how to parse the files. 
   One of the limitations here is that each document must
   be a file with the extension of the file being the document type, so
   the document #### has a text file ####.TEXT and a jfif file
   ####.JFIF, not real nice but needed.

   Note that this contains both the primary and secondary document
   types, whereas dataops.type contains the primary type.
 */

    else if (0 == strcmp ("-M", next_argument)) {
      if (NULL == (next_argument = next_arg (&argc, &argv)))
	panic ("Expected a multitype list");
      dataops.multitype = next_argument;
    } else if (0 == strcmp ("-keywords", next_argument)) {
      if (NULL == (next_argument = next_arg (&argc, &argv)))
	panic ("Expected -keywords argument string");
      keywords = next_argument;
    } else if (0 == strcmp ("-keyword_file", next_argument)) {
      if (NULL == (next_argument = next_arg (&argc, &argv)))
	panic ("Expected -keyword_file filename");
      keyword_filename = next_argument;
    } else if (0 == strcmp ("-contents", next_argument)) {
      index_contents = true;
    } else if (0 == strcmp ("-nocontents", next_argument)) {
      index_contents = false;
    } else if (0 == strcmp ("-t", next_argument)) {
      /* then we have a specialized file */
      index_contents = true;
      if (NULL == (next_argument = next_arg (&argc, &argv)))
	panic ("Expected a file type");
      if (0 == strcmp ("groliers", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = groliers_separator_function;
	dataops.header_function = groliers_header_function;
	dataops.finish_header_function = groliers_finish_header_function;
      }
#ifdef BIO
      else if (0 == strcmp ("genbank", next_argument)) {	/* dgg */
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = genbank_separator_function;
	dataops.header_function = genbank_header_function;
	dataops.finish_header_function = genbank_finish_header_function;
	dataops.date_function = genbank_date_function;
	dataops.repeat_weight = 0;
	dataops.addseparatorwords = true;
	dataops.extraheaderweight = false;
	dataops.minwordlen = 2;
      } else if (0 == strcmp ("embl", next_argument)) {		/* dgg */
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = embl_separator_function;
	dataops.header_function = embl_header_function;
	dataops.finish_header_function = embl_finish_header_function;
	dataops.date_function = embl_date_function;
	dataops.repeat_weight = 0;
	dataops.addseparatorwords = true;
	dataops.extraheaderweight = false;
      } else if (0 == strcmp ("pir", next_argument)) {	/* dgg */
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = pir_separator_function;
	dataops.header_function = pir_header_function;
	dataops.finish_header_function = pir_finish_header_function;
	dataops.date_function = pir_date_function;
	dataops.repeat_weight = 0;
	dataops.addseparatorwords = true;
	dataops.extraheaderweight = false;
      } else if (0 == strcmp ("prositedoc", next_argument)) {	/* dgg */
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = prositedoc_separator_function;
	dataops.header_function = prositedoc_header_function;
	dataops.finish_header_function = prositedoc_finish_header_function;
	dataops.repeat_weight = 0;
	dataops.addseparatorwords = true;
	dataops.extraheaderweight = false;
      } else if (0 == strcmp ("prositedat", next_argument)) {	/* dgg */
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = prositedat_separator_function;
	dataops.header_function = prositedat_header_function;
	dataops.finish_header_function = prositedat_finish_header_function;
	dataops.repeat_weight = 0;
	dataops.addseparatorwords = true;
	dataops.extraheaderweight = false;
      } else if (0 == strcmp ("biojournal", next_argument)) {	/* dgg */
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = biojournal_separator_function;
	dataops.header_function = biojournal_header_function;
	dataops.finish_header_function = biojournal_finish_header_function;
	dataops.repeat_weight = 0;
	dataops.addseparatorwords = true;
	dataops.extraheaderweight = false;
      } else if (0 == strcmp ("redbook", next_argument)) {	/* dgg */
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = redbook_separator_function;
	dataops.header_function = redbook_header_function;
	dataops.finish_header_function = redbook_finish_header_function;
	dataops.repeat_weight = 0;
	dataops.addseparatorwords = true;
	dataops.extraheaderweight = false;
	dataops.wordDelimiter = wordbreak_user;		/* redbook_delimiter;  */
	wordDelimiter = wordbreak_user;		/* wordbreak_notgraph; */
	dataops.minwordlen = 1;
	if (gDelimiters[0] == '\0')
	  strcpy (gDelimiters, "/{}()[]%-:#.~*\";,|");
      } else if (0 == strcmp ("flybase", next_argument)) {	/* dgg */
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = flybase_separator_function;
	dataops.header_function = flybase_header_function;
	dataops.finish_header_function = flybase_finish_header_function;
	dataops.repeat_weight = 0;
	dataops.addseparatorwords = true;
	dataops.extraheaderweight = false;
	dataops.wordDelimiter = wordbreak_user;		/* flybase_delimiter;  */
	wordDelimiter = wordbreak_user;		/* wordbreak_notgraph; */
	dataops.minwordlen = 1;
	if (gDelimiters[0] == '\0')
	  strcpy (gDelimiters, "-/{}:.~*\";,|");

	/* flybase symbols
	   valid data ()$+-?;.\'
	   possible data and delimiter |;[]-?.~
	   delimiters 
	   solution to confusion: set possible delimiters as delimiters, and
	   permit literal searches with "..." or '...' enclosed strings.
	 */

      } else if (0 == strcmp ("flystock", next_argument)) {	/* dgg */
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = bio_separator_function;
	dataops.header_function = bio_header_function;
	dataops.finish_header_function = bio_finish_header_function;
	dataops.repeat_weight = 0;
	dataops.addseparatorwords = true;
	dataops.extraheaderweight = false;
	dataops.wordDelimiter = wordbreak_user;		/* flybase_delimiter;  */
	wordDelimiter = wordbreak_user;		/* wordbreak_notgraph; */
	dataops.minwordlen = 1;
	if (gDelimiters[0] == '\0')
	  strcpy (gDelimiters, "-/{}:.~*\";,|");

	/* flystock symbols
	   valid data []()/-;?+.{}
	   possible data and delimiter  =;.
	   ;. in text field is del, in data field is data
	   delimiters *";,
	   more delimiters (from matthewk)  - / {} :

	   solution to confusion: set possible delimiters as delimiters, and
	   permit literal searches with "..." or '...' enclosed strings.
	   ! want some way to provide field names (report "stylesheet") with
	   searched/fetched records for flybase, flystock, other data files
	   ! want "keyword [field]" limited searches for some of this to make sense !
	 */
      } else if (0 == strcmp ("din", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = din_separator_function;
	dataops.header_function = din_header_function;
	dataops.finish_header_function = din_finish_header_function;
      }
#endif

#ifdef NeXT
      else if (0 == strcmp ("objc", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = wobjc_separator_function;
	dataops.header_function = wobjc_header_function;
	dataops.finish_header_function = wobjc_finish_header_function;
      }
#endif /* def NeXT */
      else if (0 == strcmp ("listserv_digest", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = listserv_digest_separator_function;
	dataops.header_function = listserv_header_function;
	dataops.date_function = listserv_date_function;
	dataops.finish_header_function = listserv_finish_header_function;
      }
#ifdef STELAR
      else if (0 == strcmp ("AAS_abstract", next_argument)) {
	typename = next_argument;
	dataops.separator_function = aasab_separator_function;
	dataops.header_function = aasab_header_function;
	dataops.finish_header_function = aasab_finish_header_function;
      } else if (0 == strcmp ("stelar", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = stelar_separator_function;
	dataops.header_function = stelar_header_function;
	dataops.finish_header_function = stelar_finish_header_function;
      }
#endif /* STELAR */
      else if (0 == strcmp ("mail", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = mail_separator_function;
	dataops.header_function = mail_header_function;
	dataops.date_function = mail_date_function;
	dataops.finish_header_function = mail_finish_header_function;
      } else if (0 == strcmp ("mail_or_rmail", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = mail_or_rmail_separator;
	dataops.header_function = mail_header_function;
	dataops.date_function = mail_date_function;
	dataops.finish_header_function = mail_finish_header_function;
      } else if (0 == strcmp ("mail_digest", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = mail_digest_separator_function;
	dataops.header_function = mail_header_function;
	dataops.date_function = mail_date_function;
	dataops.finish_header_function = mail_finish_header_function;
      } else if (0 == strcmp ("mh_bboard", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = mh_bboard_separator_function;
	dataops.header_function = mail_header_function;
	dataops.date_function = mail_date_function;
	dataops.finish_header_function = mail_finish_header_function;
      } else if (0 == strcmp ("rmail", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = rmail_separator_function;
	dataops.header_function = mail_header_function;
	dataops.date_function = mail_date_function;
	dataops.finish_header_function = mail_finish_header_function;
      } else if (0 == strcmp ("netnews", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = NULL;
	dataops.header_function = mail_header_function;
	dataops.date_function = mail_date_function;
	dataops.finish_header_function = mail_finish_header_function;
      } else if (0 == strcmp ("rn", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = rn_separator_function;
	dataops.header_function = mail_header_function;
	dataops.date_function = mail_date_function;
	dataops.finish_header_function = mail_finish_header_function;
      }
      /* fields meta type */
      else if (0 == strcmp ("fields", next_argument)) {
	index_field = true;
	if (word_positions) {
	  waislog (WLOG_MEDIUM, WLOG_WARNING,
		   "Disabeling -pos with field indexing\n");
	  word_positions = false;
	}
	typename = next_argument;
	if (!dataops.type || (strlen (dataops.type) == 0)) {
	  if (dataops.type)
	    fprintf (stderr, "irbuild: overwriting type %s\n", dataops.type);
	  dataops.type = "TEXT";
	} else {
	  waislog (WLOG_LOW, WLOG_INFO, "irbuild: using type %s\n", dataops.type);
	}
	dataops.separator_function = fields_separator_function;
	dataops.header_function = fields_header_function;
	dataops.finish_header_function = fields_finish_header_function;
	dataops.date_function = fields_date_function;
      }
#ifdef BIBDB
      else if (0 == strcmp ("irlist", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = irlist_separator_function;
	dataops.header_function = irlist_header_function;
	dataops.date_function = irlist_date_function;
	dataops.finish_header_function = mail_finish_header_function;
      }
      /* formfeed-separated items , Intro to Algorithms buglist, etc */
      else if (0 == strcmp ("formfeed", next_argument)) {
	typename = next_argument;
	if (!dataops.type || (strlen (dataops.type) == 0)) {
	  if (dataops.type)
	    fprintf (stderr, "irbuild: overwriting type %s\n", dataops.type);
	  dataops.type = "TEXT";
	} else {
	  fprintf (stderr, "irbuild: using type %s\n", dataops.type);
	}
	dataops.separator_function = formfeed_separator_function;
	dataops.header_function = dash_header_function;
	dataops.finish_header_function = dash_finish_header_function;
      }
      /* formfeed-separated items , steve files */
      else if (0 == strcmp ("bibdb", next_argument)) {
	typename = next_argument;
	if (!dataops.type || (strlen (dataops.type) == 0)) {
	  if (dataops.type)
	    fprintf (stderr, "irbuild: overwriting type %s\n", dataops.type);
	  dataops.type = "TEXT";
	} else {
	  fprintf (stderr, "irbuild: using type %s\n", dataops.type);
/*          stop_list_file("bibdb.stop"); */
	}
	dataops.separator_function = bibdb_separator_function;
	dataops.header_function = bibdb_header_function;
	dataops.date_function = bibdb_date_function;
	dataops.finish_header_function = bibdb_finish_header_function;
      }
      /* formfeed-separated items, bibinbf */
      else if (0 == strcmp ("bibinf", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = bibinf_separator_function;
	dataops.header_function = bibinf_header_function;
#ifdef SIMPLE_BIBINF
	dataops.date_function = bibinf_date_function;
#endif
	dataops.finish_header_function = bibinf_finish_header_function;
      }
#endif
      else if (0 == strcmp ("emacsinfo", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = emacs_info_separator_function;
	dataops.header_function = emacs_info_header_function;
	dataops.finish_header_function = emacs_info_finish_header_function;
      } else if (0 == strcmp ("catalog", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = catalog_separator_function;
	dataops.header_function = catalog_header_function;
	dataops.finish_header_function = catalog_finish_header_function;
      } else if (0 == strcmp ("bio", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = bio_separator_function;
	dataops.header_function = bio_header_function;
	dataops.finish_header_function = bio_finish_header_function;
      } else if (0 == strcmp ("cmapp", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = cmapp_separator_function;
	dataops.header_function = cmapp_header_function;
	dataops.finish_header_function = cmapp_finish_header_function;
      } else if (0 == strcmp ("ftp", next_argument)) {
	dataops.type = "TEXT-FTP";
	typename = next_argument;
	dataops.separator_function = first_line_separator_function;
	dataops.header_function = first_line_header_function;
	dataops.finish_header_function = first_line_finish_header_function;
      } else if (0 == strcmp ("jargon", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = jargon_separator_function;
	dataops.header_function = jargon_header_function;
	dataops.finish_header_function = jargon_finish_header_function;
      } else if (0 == strcmp ("server", next_argument)) {
	typename = next_argument;
	dataops.type = "WSRC";
	dataops.finish_header_function = filename_finish_header_function;
      } else if (0 == strcmp ("text", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	check_for_text_file = true;
      } else if (0 == strcmp ("filename", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.finish_header_function = filename_finish_header_function;
#ifdef USE_DBM
/*
Permits standard Unix System V Release 3.2 'dbm' files.

egw@aubranch.egwestate.andrews.edu, Ellen G. White Estate, July 1995
*/
        else if(0 == strcmp("textdbm", next_argument)){
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.finish_header_function = dbm_finish_header_function;
      } /* END OF "textdbm" ELSE */
#endif
      } else if (0 == strcmp ("irg", next_argument)) {
	typename = next_argument;
	dataops.type = "TEXT";
	dataops.separator_function = irg_separator_function;
	dataops.header_function = irg_header_function;
	dataops.finish_header_function = irg_finish_header_function;
      }
      /* dash-separated items , Intro to Algorithms buglist, etc */
      else if (0 == strcmp ("dash", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = dash_separator_function;
	dataops.header_function = dash_header_function;
	dataops.finish_header_function = dash_finish_header_function;
      }
      /* one_line-separated items */
      else if (0 == strcmp ("one_line", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = one_line_separator_function;
	dataops.header_function = one_line_header_function;
	dataops.finish_header_function = one_line_finish_header_function;
      }
      /* blank line-separated items (paragraphs) */
      else if (0 == strcmp ("para", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = para_separator_function;
	dataops.header_function = para_header_function;
	dataops.finish_header_function = para_finish_header_function;
      }
      /* seeker items */
      else if (0 == strcmp ("seeker", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = seeker_separator_function;
	dataops.header_function = seeker_header_function;
	dataops.finish_header_function = seeker_finish_header_function;
      }
      /* medline format */
      else if (0 == strcmp ("medline", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = medline_separator_function;
	dataops.header_function = medline_header_function;
	dataops.finish_header_function = medline_finish_header_function;
      }
      /* refer format */
      else if (0 == strcmp ("refer", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = refer_separator_function;
	dataops.header_function = refer_header_function;
	dataops.finish_header_function = refer_finish_header_function;
      }
      /* first_line format */
      else if (0 == strcmp ("first_line", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = first_line_separator_function;
	dataops.header_function = first_line_header_function;
	dataops.finish_header_function = first_line_finish_header_function;
      }
      /* rlin items */
      else if (0 == strcmp ("rlin", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = rlin_separator_function;
	dataops.header_function = rlin_header_function;
	dataops.finish_header_function = rlin_finish_header_function;
      } else if (0 == strcmp ("dvi", next_argument)) {
	typename = next_argument;
	dataops.type = "DVI";
	dataops.finish_header_function = filename_finish_header_function;
      } else if (0 == strcmp ("ps", next_argument)) {
	typename = next_argument;
	dataops.type = "PS";
	dataops.finish_header_function = filename_finish_header_function;
      } else if (0 == strcmp ("pict", next_argument)) {
	typename = next_argument;
	dataops.type = "PICT";
	dataops.finish_header_function = filename_finish_header_function;
	index_contents = false;
      } else if (0 == strcmp ("gif", next_argument)) {
	typename = next_argument;
	dataops.type = "GIF";
	dataops.finish_header_function = filename_finish_header_function;
	index_contents = false;
      } else if (0 == strcmp ("tiff", next_argument)) {
	typename = next_argument;
	dataops.type = "TIFF";
	dataops.finish_header_function = filename_finish_header_function;
	index_contents = false;
      }
      /* BibTeX items */
      else if (0 == strcmp ("bibtex", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = bibtex_separator_function;
	dataops.header_function = bibtex_header_function;
	dataops.finish_header_function = bibtex_finish_header_function;
      }
      /* ?:? seperated hypertext items */
      else if (0 == strcmp ("nhyp", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = nhyp_separator_function;
	dataops.header_function = nhyp_header_function;
	dataops.finish_header_function = nhyp_finish_header_function;
      }
      /* Uniform Resource Locators  - from Nat Torkington */
      else if (0 == strcmp ("URL", next_argument)) {
	dataops.type = "URL";
	typename = next_argument;
	URL_trim = s_strdup (next_arg (&argc, &argv));
	URL_prefix = s_strdup (next_arg (&argc, &argv));
      } else if (0 == strcmp ("ziff", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = ziff_separator_function;
	dataops.header_function = ziff_header_function;
	dataops.finish_header_function = ziff_finish_header_function;
#ifdef ESSENCE
      } else if (0 == strcmp ("essence", next_argument)) {
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = essence_separator_function;
	dataops.header_function = essence_header_function;
	dataops.finish_header_function = essence_finish_header_function;
#endif /* ESSENCE */
      }
#ifdef SOUND
      else if (0 == strcmp ("oneline_soundex", next_argument)) {
	dataops.indextype = "SOUNDEX";
	dataops.prev_indextype = "SOUNDEX";
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = one_line_separator_function;
	dataops.header_function = one_line_header_function;
	dataops.finish_header_function = one_line_finish_header_function;
      } else if (0 == strcmp ("oneline_phonix", next_argument)) {
	dataops.indextype = "PHONIX";
	dataops.prev_indextype = "SOUNDEX";
	dataops.type = "TEXT";
	typename = next_argument;
	dataops.separator_function = one_line_separator_function;
	dataops.header_function = one_line_header_function;
	dataops.finish_header_function = one_line_finish_header_function;
      }
#endif
      else {
	panic ("Don't recognize the '%s' type", next_argument);
      }
    } else if (0 == strcmp ("-nfields", next_argument)) {
      create_new_fields = true;
      adding_to_existing_index = true;
    } else {
      panic ("Don't recognize the '%s' option", next_argument);
    }

    next_argument = next_arg (&argc, &argv);

    if (!(read_files_from_stdin || next_argument)) {
      fprintf (stderr, "No files specified\n");
      exit (0);
    }
  }
  prev_argc = argc;
  first_filename = next_argument;

  start_of_filenames = argc_copy - argc - 1;

  /* check index */
  if (0 == strlen (pathname_name (index_filename))) {
    waislog (WLOG_HIGH, WLOG_ERROR,
	     "The pathname specified for the destination of the index files ('%s') should have a leaf filename without an extention rather than just a directory.",
	     index_filename);
    exit (0);
  }
  waislog (WLOG_MEDIUM, WLOG_INDEX, "Starting to build database %s",
	   index_filename);

  if (0 != init_search_engine (index_filename, false, false, cm_mem_percent,
			       text_size, grow_percent))
    panic ("unable to initialize search engine");

  if (true == adding_to_existing_index) {
    db = openDatabase (index_filename, false, false, index_field);
    if (db == NULL) {		/* does not exist, create one */
      db = openDatabase (index_filename, true, false, index_field);
      if (db == NULL)
	panic ("unable to open the database");
    }
    if (index_field) {
      if (-1 == init_add_fields (db->database_file, &global_dictionary_exists, db)) {
	closeDatabase (db);
	exit (0);
      }
      if (create_new_fields)
	prev_number_of_fields = db->number_of_fields;
      if (db->number_of_fields > 0) {
	if (global_dictionary_exists) {
	  if (open_global_Database (false, db)) {
	    closeDatabase (db);
	    exit (0);
	  }
	}
	init_index_fields (&adding_to_existing_index, &create_new_fields,
			   field_adding_to_existing_index,
			   db->number_of_fields, db);
      }
    }
  } else {
    db = openDatabase (index_filename, true, false, index_field);
    if (db == NULL)
      panic ("unable to open the database");
    if (index_field) {
      if (-1 == init_add_fields (db->database_file, &global_dictionary_exists, db)) {
	closeDatabase (db);
	exit (0);
      }
      if (db->number_of_fields > 0) {
	if (global_dictionary_exists) {
	  if (open_global_Database (true, db)) {
	    closeDatabase (db);
	    exit (0);
	  }
	}
	init_index_fields (&adding_to_existing_index, &create_new_fields,
			   field_adding_to_existing_index,
			   db->number_of_fields, db);
      }
    }
  }

#ifdef BIO
  write_delimiters (gDelimiters, db);
#endif

  {				/* set up the memory hashtable */

    if (memory_to_use < 0) {	/* default */
      /* do nothing */
    } else if (memory_to_use <= 2) {
      hashtable_size = 1L << 16;
      flush_after_n_words = 50000;
    } else if (memory_to_use <= 3) {
      hashtable_size = 1L << 16;
      flush_after_n_words = 850000;
    } else if (memory_to_use <= 4) {
      hashtable_size = 1L << 16;
      flush_after_n_words = 110000;
    } else if (memory_to_use <= 5) {
      hashtable_size = 1L << 16;
      flush_after_n_words = 150000;
    } else if (memory_to_use <= 10) {
      /* shown to take about 6MB on a sun4, when it is dict limited */
      hashtable_size = 1L << 16;
      flush_after_n_words = 300000;
    } else if (memory_to_use <= 20) {
      hashtable_size = 1L << 17;
      flush_after_n_words = 600000;
    } else {			/* over 20 Mbytes */
      hashtable_size = 1L << 18;
      flush_after_n_words = 1000000;
    }
    init_add_word (db, hashtable_size, flush_after_n_words);
  }

  if (read_files_from_stdin) {
    if (0 != (next_argument = fgets (data_filename, MAXPATHLEN, stdin))) {
      int             len = strlen (next_argument);

      if (next_argument[len - 1] == '\n') {
	next_argument[len - 1] = '\0';
      }
      if (db->number_of_fields > 0) {
	if (number_of_files >= current_max_files) {
	  char          **new_tab;

	  new_tab =
	    (char **) s_malloc ((current_max_files
				 + BLOCK_FILES_FROM_STDIN)
				* sizeof (char *));

	  if (current_max_files) {
	    bcopy (data_filename_array, new_tab,
		   current_max_files * sizeof (char *));

	    s_free (data_filename_array);
	  }
	  current_max_files += BLOCK_FILES_FROM_STDIN;
	  data_filename_array = new_tab;
	}
	data_filename_array[number_of_files] = s_malloc (len + 1);
	strncpy (data_filename_array[number_of_files],
		 next_argument, len + 1);
	++number_of_files;
	read_files_from_stdin_array = true;
      }
    }
  }
#ifdef STEM_WORDS
  db->stemming = stemming;
#endif

  if (index_field && db->number_of_fields == 0)
    index_field = false;

  for (i = 0; i <= ((db->number_of_fields == 0) ? db->number_of_fields : db->number_of_fields - 1); i++) {
    if (db->number_of_fields > 0) {
      db->current_field_position = i;
      db->current_field_id = db->fields[i].field_id;
      doc_table_allocated_entries = db->doc_table_allocated_entries;
      if (db->fields[i].field_name != NULL) {
	waislog (WLOG_MEDIUM, WLOG_INDEX,
		 "Building field: %s", db->fields[i].field_name);
	if (open_field_streams_init (!adding_to_existing_index,
				   field_adding_to_existing_index, i, db)) {
	  panic ("unable to open the database for field");
	  closeDatabase (db);
	  exit (0);
	}
      } else
	waislog (WLOG_MEDIUM, WLOG_INDEX, "Building global database");
    }
    while (NULL != next_argument) {	/* the first filename is in next_argument already */
      if (directoryp (next_argument)) {
	if (traverse_directory) {
	  index_directory (next_argument, &dataops, db,
			   check_for_text_file,
			   adding_to_existing_index,
			   word_positions, word_pairs
			   ,index_next_field, i
	    );
	}
      } else {			/* not a directory */
	if (db->number_of_fields == 0)
	  waislog (WLOG_MEDIUM, WLOG_INDEX,
		   "Indexing file: %s", next_argument);
	if (db->number_of_fields > 0) {
	  waislog (WLOG_MEDIUM, WLOG_INDEX,
		   "File: %s", next_argument);
	  if (index_next_field) {
	    if ((mesg = field_index_text_file (next_argument, &dataops, db,
					       check_for_text_file,
					       adding_to_existing_index,
					       word_positions, word_pairs,
					       i)) <= 0) {
	      waislog (WLOG_HIGH, WLOG_ERROR, "File not indexed: %s", next_argument);
	      if (mesg == -1) {
		closeDatabase (db);
		exit (0);
	      }
	    }
	  } else {
	    if ((mesg = index_text_file (next_argument, &dataops, db,
					 check_for_text_file,
					 adding_to_existing_index,
				     word_positions, word_pairs, i)) <= 0) {
	      waislog (WLOG_HIGH, WLOG_ERROR, "File not indexed: %s", next_argument);
	      if (mesg == -1) {
		closeDatabase (db);
		exit (0);
	      }
	    }
	  }
	} else {
	  if ((mesg = index_text_file (next_argument, &dataops, db,
				       check_for_text_file,
				       adding_to_existing_index,
				     word_positions, word_pairs, i)) <= 0) {
	    waislog (WLOG_HIGH, WLOG_ERROR, "File not indexed: %s", next_argument);
	    if (mesg == -1) {
	      closeDatabase (db);
	      exit (0);
	    }
	  }
	}
      }
      if (read_files_from_stdin) {
	if (0 != (next_argument = fgets (data_filename, MAXPATHLEN, stdin))) {
	  int             len = strlen (next_argument);

	  if (next_argument[len - 1] == '\n') {
	    next_argument[len - 1] = '\0';
	  }
	  if (db->number_of_fields > 0) {
	    if (number_of_files >= current_max_files) {
	      char          **new_tab;

	      new_tab =
		(char **) s_malloc ((current_max_files
				     + BLOCK_FILES_FROM_STDIN)
				    * sizeof (char *));

	      if (current_max_files) {
		bcopy (data_filename_array, new_tab,
		       current_max_files * sizeof (char *));

		s_free (data_filename_array);
	      }
	      current_max_files += BLOCK_FILES_FROM_STDIN;
	      data_filename_array = new_tab;
	    }
	    data_filename_array[number_of_files] = s_malloc (len + 1);
	    strncpy (data_filename_array[number_of_files],
		     next_argument, len + 1);
	    ++number_of_files;
	  }
	}
      } else if (read_files_from_stdin_array && db->number_of_fields) {
	if (number_of_data_filename < number_of_files) {
	  next_argument = data_filename_array[number_of_data_filename++];
	} else {
	  next_argument = NULL;
	  number_of_data_filename = 0;
	}
      } else {
	if (db->number_of_fields > 0)
	  next_argument = field_next_arg (&argc, &number_of_files, &argv);
	else
	  next_argument = next_arg (&argc, &argv);
      }
    }
    if (db->number_of_fields > 0) {
      if (i < db->number_of_fields - 1)
	db->doc_table_allocated_entries = doc_table_allocated_entries;
      index_next_field = true;
      argc = prev_argc;
      if (read_files_from_stdin_array) {
	next_argument = data_filename_array[number_of_data_filename++];
	/* s_free (data_filename_array[number_of_data_filename-1]); */
	read_files_from_stdin = false;
      } else {
	next_argument = first_filename;
	number_of_files = 0;
      }
      if (db->fields[i].field_name != NULL) {
	field_finished_add_word (db, db->fields[i].field_id);
	if (*(db->field_dictionary_streams) != NULL)
	  s_fclose (*(db->field_dictionary_streams));
	if (*(db->field_index_streams) != NULL)
	  s_fclose (*(db->field_index_streams));
      } else {
	finished_add_word (db);
	if (db->dictionary_stream != NULL)
	  s_fclose (db->dictionary_stream);
	if (db->index_stream != NULL)
	  s_fclose (db->index_stream);
      }
    }
  }
  if (index_field) {
    if (global_dictionary_exists) {
      retreive_keywords (db);
    }
  } else {
    finished_add_word (db);
    retreive_keywords (db);
  }
  {
    char            filename[MAX_FILENAME_LEN + 1];

    if (create_new_fields) {
      remove (source_filename (filename, db));
      db->number_of_fields = prev_number_of_fields;
    }
    if (!probe_file (source_filename (filename, db))) {
      char            database_name[MAX_FILENAME_LEN];

      write_src_structure (source_filename (filename, db),
			   export_database ? pathname_name (index_filename) :
			   truename (index_filename, database_name),
			   typename,
			   &argv_copy[start_of_filenames],
			   argc_copy - start_of_filenames,
			   export_database,
			   210L, db);
    } else {
      char           *oldkeys[50];
      short           oldKeys;

      if (force_rewriting_of_sorce_description) {
	char            database_name[MAX_FILENAME_LEN];

	waislog (WLOG_MEDIUM, WLOG_INDEX, "Rewriting source description.");
	waislog (WLOG_MEDIUM, WLOG_INDEX,
		 "New source description should be exported.");
	write_src_structure (source_filename (filename, db),
			  export_database ? pathname_name (index_filename) :
			     truename (index_filename, database_name),
			     typename,
			     &argv_copy[start_of_filenames],
			     argc_copy - start_of_filenames,
			     export_database,
			     210L, db);
      } else if ((oldKeys = read_src_structure (source_filename (filename, db), oldkeys))) {

	if (compare (keywrd, nKeys, oldkeys, oldKeys) > 0.1) {
	  char            database_name[MAX_FILENAME_LEN];

	  waislog (WLOG_MEDIUM, WLOG_INDEX,
		   "Keyword comparison indicates significant change.");
	  waislog (WLOG_MEDIUM, WLOG_INDEX, "Rewriting source description.");
	  waislog (WLOG_MEDIUM, WLOG_INDEX,
		   "New source description should be exported.");
	  write_src_structure (source_filename (filename, db),
			  export_database ? pathname_name (index_filename) :
			       truename (index_filename, database_name),
			       typename,
			       &argv_copy[start_of_filenames],
			       argc_copy - start_of_filenames,
			       export_database,
			       210L, db);
	}
      } else {
	char            database_name[MAX_FILENAME_LEN];

	waislog (WLOG_MEDIUM, WLOG_INDEX, "No keyword list found.");
	waislog (WLOG_MEDIUM, WLOG_INDEX, "Rewriting source description.");
	waislog (WLOG_MEDIUM, WLOG_INDEX,
		 "New source description should be export ed.");
	write_src_structure (source_filename (filename, db),
			  export_database ? pathname_name (index_filename) :
			     truename (index_filename, database_name),
			     typename,
			     &argv_copy[start_of_filenames],
			     argc_copy - start_of_filenames,
			     export_database,
			     210L, db);
      }
    }
    /* write out a description of the server if appropriate */
    if (register_database) {
      register_src_structure (source_filename (filename, db));
    }
  }
  if (make_catalog)
    build_catalog (db);
  closeDatabase (db);
  free_keywords ();		/* 8/94 (up) */
  waislog (WLOG_MEDIUM, WLOG_INDEX, "Finished build");
  exit (0);
}

read_src_structure (filename, output)
     char           *filename;
     char           *output[50];
{
  FILE           *source_stream = s_fopen (filename, "r");
  char            line[MAX_LINE_LENGTH], *ptr;
  int             keyflag = 0, linelen, i, index;
  int             desflag = 0;
  int             tmp;

  index = 0;
  while (fgets (line, MAX_LINE_LENGTH, source_stream)) {
    linelen = strlen (line);
    if (keyflag) {
      if (!strncmp ("                  )", line, (linelen > 19) ? 19 : linelen))
	keyflag = 0;
      else {
	line[strlen (line) - 1] = '\0';		/* get rid of trailing return */
	ptr = line;		/* parse keyword */
	while (*ptr == ' ')
	  ptr++;
	output[index] = (char *) malloc (strlen (ptr) + 1);
	strcpy (output[index], ptr);
	index++;
      }
    }
    if (!strncmp ("   :keyword-list (", line, (linelen > 18) ? 18 : linelen))
      keyflag = 1;
    if (!strncmp ("   :description", line, (linelen > 15) ? 15 : linelen))
      desflag = 1;
    if (desflag) {
      tmp = strlen (line) + 1;
      descript[nDesLines] = (char *) malloc (tmp);
      strcpy (descript[nDesLines], line);
      nDesLines++;
      if (*line == '\"')
	desflag = 0;
    }
  }
  fclose (source_stream);
  return (index);
}

double
compare (a, alen, b, blen)
     char           *a[50], *b[50];
     short           alen, blen;
{
  int             changes = 0;
  int             i, j;

  for (i = 0; i < alen; i++) {
    for (j = 0; j < blen; j++)
      if (!strcmp (a[i], b[j]))
	break;
    if (j == blen)
      changes++;
  }
  waislog (WLOG_LOW, WLOG_INFO, "%d out of %d\n", changes, alen);	/* info stuff */
  return ((double) changes / alen);
}
