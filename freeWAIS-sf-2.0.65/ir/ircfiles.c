/*                               -*- Mode: C -*- 
 * 
 * ircfiles.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Brewster@think.com
 * Created On      : Fri Oct  6 10:48:49 1995
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Tue Jan 16 13:14:06 1996
 * Language        : C
 * Update Count    : 21
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 */

/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* this file defines a set of helper functions
 * for indexing common types of files.
 * -brewster 7/90
 */

/* I encourage adding customizations.
 * (too bad they all have to be hard coded, but
 *  C did not have convenient dynamic linking facilities)
 *
 * Add three functions to this file:
 * boolean foo_separator_function(char *line){}
 * void foo_header_function(char *line){}
 * long foo_date_function(char *line){}
 * void foo_finish_header_function(char *header){}
 *
 * then add the prototypes to ircfiles.h
 * then add the functions to the big case statement in irbuild.c
 *
 *
 * to do:
 *   filter for digests
 *
 *   Tracy pointed out 2 things which we should consider when redesigning the
 *   parser:
 *
 *   - there should be a way for the parser to decide to skip a section of 
 *   input text (ie. not index it).  she does this by having global variable 
 *   which is set by her custom seperator function when it wants to tell 
 *   map_over_words() to not add the words on the current line
 *
 *   - there should be a way to switch lexers depending what section of a
 *   document you are in (since word separators will change).  This is 
 *   needed by the european patent office too.
 *
 */


/* Change log:
 * 8/90 brewster added the library customizations
 * 6/91 and before - added a bunch of other filters - JG
 * $Log: ircfiles.c,v $
 * Revision 2.0.1.9  1996/01/23 15:10:57  pfeifer
 * patch62: Fix from Brian Bullen.
 *
 * Revision 2.0.1.8  1995/12/06  10:56:06  pfeifer
 * patch53: Added dbm stuff from egw@aubranch.egwestate.andrews.edu.
 * patch53: Did not add the program for filling the dbm database yet.
 *
 * Revision 2.0.1.7  1995/11/14  18:11:52  pfeifer
 * patch51: Fixed erroneous beginning of line matches for headlines.
 *
 * Revision 2.0.1.6  1995/11/03  09:53:52  pfeifer
 * patch43: Fixed off-by-one bug in headlines.
 *
 * Revision 2.0.1.5  1995/10/27  09:58:11  pfeifer
 * patch41: Strip centuries from data for insertion in the headline. The
 * patch41: original code assumes date to be yymmdd. Since the date in
 * patch41: the headline is of limited use, we do not fix that now.
 *
 * Revision 2.0.1.4  1995/10/06  17:36:54  pfeifer
 * patch24: Moved fields_getdate_internal (former fields_getdate)
 * patch24: index_fields.c. New fields_getdate() just calls
 * patch24: fields_getdate_internal with the global date specification.
 *
 * Revision 2.0.1.3  1995/10/06  09:40:35  pfeifer
 * Rather complex DateDescription. May cause alignment problems.
 *
 * Revision 2.0.1.2  1995/09/15  09:41:38  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.1  1995/09/11  12:48:38  pfeifer
 * patch4: Replaced inclusion of string.h by cdialect.h. Moved example
 * patch4: lines starting with '#' to the right.
 *
 * Revision 2.0  1995/09/08  07:59:20  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.13  1995/02/10  10:33:13  pfeifer
 * Release 1.1.2
 *
 * Revision 1.12  1994/12/19  16:53:59  pfeifer
 * Michael Schmidt one-off bug with date
 *
 * Revision 1.11  1994/08/23  12:32:00  pfeifer
 * Moved HEADLINE constants to Defaults.tmpl
 *
 * Revision 1.10  1994/06/03  11:42:56  huynh1
 * bug in headlines specification corrected.
 *
 * Revision 1.9  1994/05/19  12:53:11  huynh1
 * fields_header_function updated.
 *
 * Revision 1.8  1994/03/28  09:29:21  pfeifer
 * Integrated Essence
 *
 * Revision 1.7  1994/03/23  13:05:46  pfeifer
 * removed the include iso.h
 *
 * Revision 1.6  1994/03/11  15:16:54  huynh1
 * bug fix corrected.
 * Patchlevel 05.
 *
 * Revision 1.5  1994/03/08  20:38:58  pfeifer
 * Patchlevel 04
 *
 * Revision 1.4  1994/02/14  10:28:38  huynh1
 * layout_length added.
 *
 * Revision 1.3  93/07/21  18:45:34  warnock
 * Added new functions to handle listserv logs and STELAR-specific types
 * 
 * Revision 1.2  93/07/19  16:31:50  warnock
 * Added document type URL from Nathan.Torkington@vuw.ac.nz
 * 
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.34  92/05/06  17:28:23  jonathan
 * Added filename_finish_header_function.  Puts leaf name into header.
 * 
 * Revision 1.33  92/05/05  11:10:50  jonathan
 * Added fix to bibtex indexer to ignore subsequent "booktitles" after title
 * has been set.  Thanks to Lutz Prechelt (prechelt@ira.uka.de).
 * 
 * Revision 1.32  92/04/30  12:31:08  jonathan
 * Fixed syntax errors in OBJ C functions.
 * 
 * Revision 1.31  92/04/29  14:08:57  shen
 * chnage catalaog header string to "Title:"
 * 
 * Revision 1.30  92/04/26  14:45:08  brewster
 * debug ziff
 * 
 * Revision 1.29  92/04/26  14:39:24  brewster
 * tweeked ziff filter
 * 
 * Revision 1.28  92/04/25  21:14:05  brewster
 * added ziff
 * 
 * Revision 1.27  92/04/20  15:21:06  morris
 * added todo's for tracy
 * 
 * Revision 1.26  92/03/22  18:38:29  brewster
 * added objective C filter
 * 
 * Revision 1.25  92/03/13  08:21:37  jonathan
 * Added length limits to scanf's in my_getdate, thanks to
 * sendall@dxpt01.cern.ch (Mike Sendall).
 * 
 * Revision 1.24  92/02/29  20:13:54  jonathan
 * separated =- for some compilers that get confused (ULTRIX).
 * 
 * Revision 1.23  92/02/20  09:50:14  jonathan
 * Added bibtex and nhyp filters from S.P.vandeBurgt@research.ptt.nl.
 * 
 * Revision 1.22  92/02/12  13:11:25  jonathan
 * Changed library catalog functions for new format (from fad).
 * 
 * 
 *
 */

#include "cdialect.h"
#include <ctype.h>
#include "cutil.h"
#include "ircfiles.h"

#ifdef FIELDS
#include "field_index.h"
#endif /* FIELDS */
extern char *current_filename;
extern int current_filecount;

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

void dbm_finish_header_function _AP((char *header));
#endif

static char    *months[] =
{"Jan", "Feb", "Mar", "Apr", "May", "Jun",
 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL};

static char* trim_trailing_newline _AP((char* string));
static char* trim_leading_blanks _AP((char* string));

static char    *
trim_trailing_newline (string)
     char           *string;
{
  if (string)
    if (strlen (string) > 0)
      if (string[strlen (string) - 1] == '\n')
	string[strlen (string) - 1] = '\0';
  return (string);
}

static char    *
trim_leading_blanks (string)
     char           *string;
{
  if (string)
    if (strlen (string) > 0)
      while (string[0] <= ' ')
	string++;
  return (string);
}


#ifdef BIO

char bio_header1[MAX_HEADER_LEN + 1];
char bio_header2[MAX_HEADER_LEN + 1];


/* ==========================================
 *
 * ===  Genbank Flat-file Customizations  ===
 *
 * d.g.gilbert, 15feb92, 
 * gilbertd@bio.indiana.edu
 *
 * ==========================================
 */

#define genbank_data_tab	12
#define genbank_date_tab	63

/* Genbank Flat-file format:
LOCUS       ACAAC01      1571 bp ds-DNA             INV       05-NOV-1991 << start entry
12345678901234567890123456789012345678901234567890123456789012345678901234567890
.........1.........2.........3.........4.........5.........6.........7.........8
all data starts at tab=13
on locus line, data starts at tab=63
...
LOCUS       blah	<< Start entry, index LOCUS_NAME, includes DATE
DEFINITION  blah   	<< Index def line == HEADER line
ACCESSION   blah	<< Index acc line
KEYWORDS    blah	<< index keywords
SOURCE      blah	<< index source
  ORGANISM  blah	<< index organism
            blah	<< index taxonomy
			blah	<< "
  AUTHORS   blah	<< Index
  TITLE     blah	<< Index
            blah	<< Index
ANYOTHERS	jazz	<< skipit
//  				<< end of entry == entry separator
LOCUS       ACAAC01      1571 bp ds-DNA             INV       05-NOV-1991 << start entry
DEFINITION  Acanthamoeba castelani gene encoding actin I.
ACCESSION   V00002 J01016
KEYWORDS    actin.
SOURCE      Acanthamoeba castellanii DNA.
  ORGANISM  Acanthamoeba castellanii
            Eukaryota; Animalia; Protozoa; Sarcomastigophora; Sarcodina;
            Rhizopoda; Lobosa; Gymnamoeba; Amoebida; Acanthopodina;
            Acanthamoebidae.
REFERENCE   1  (bases 1 to 1571)
  AUTHORS   Nellen,W. and Gallwitz,D.
  TITLE     Actin genes and actin messenger RNA in Acanthamoeba castellani.
            Nucleotide sequence of the split actin gene I
  JOURNAL   J. Mol. Biol. 159, 1-18 (1982)
COMMENT     SWISS-PROT; P02578; ACT1$ACACA.
               From EMBL 26   entry ACAC01;  dated 22-JAN-1991.

FEATURES             Location/Qualifiers
	>>> ignore all features
	
BASE COUNT      313 a    535 c    389 g    334 t
ORIGIN
        1 ggagaagcgt gcacgcaata accaagcgac agagcaacct ctctggcacc acgccccaca
	>>> ignore all seq data in indexing
//  <<< end of entry
LOCUS       ACAMHCA      5894 bp ds-DNA             INV       30-SEP-1988
...
*****/


static  boolean  keepindexing = false;

void genbank_filter_for_index(line)
char* line;
{
  /* check whether to index anything in line, 
   * call this from genbank_header_function which is called for
   * each line.  
   * Blank out parts of line not for indexing...
   */
  char *c;
  long  i;
  
  if (strlen(line) <= genbank_data_tab) {
	 for (c=line ; *c>=' '; c++) *c=' ';
	 keepindexing= false;  
  	 }
	 
  else if (substrcmp(line, "            ")) {  
		/* most lines are like this, including nucleotides */
  	 if (!keepindexing) for (c=line ; *c>=' '; c++) *c=' ';
     }
	 
  /*******
   else if (substrcmp(line, "LOCUS       ")){
    	// I think this is bad, locus not in index ... 
	 for (c=line, i=0; *c>=' ' && i<genbank_data_tab; i++, c++) *c=' ';
	 for ( ; *c>' '; c++) ;  // leave LOCUS ID intact  
	 for ( ; *c>=' '; c++) *c=' '; 
	 keepindexing= false;
     }
	******/ 
	
  else if ( 
   	   substrcmp(line, "DEFINITION  ") 
  	|| substrcmp(line, "LOCUS       ")
  	|| substrcmp(line, "ACCESSION   ")
  	|| substrcmp(line, "KEYWORDS    ")
  	|| substrcmp(line, "SOURCE      ")
  	|| substrcmp(line, "  ORGANISM  ")
  	|| substrcmp(line, "  AUTHORS   ")
  	|| substrcmp(line, "  TITLE     ")
	){
	 for (c=line, i=0; *c>=' ' && i<genbank_data_tab; i++, c++) *c=' ';
	 keepindexing= true;
     }

  else {
	 for (c=line ; *c>=' '; c++) *c=' ';
	 keepindexing= false;
     }
}



boolean genbank_separator_function(line)
char *line;
{
/* !! with // as separator, we get // at top of entry which will
   screw up seqanal software... */
/*  if ((strlen(line) > 1) && (0==strncmp(line, "//", 2))){
     return(true);
     }  
*/
  if ((strlen(line) > genbank_data_tab) && substrcmp(line, "LOCUS       ")){
     return(true);
     }
  else{
    return(false);
    }
}

long genbank_getdate(line)
char *line;
{
	/* genbank date == 30-SEP-1988*/
  char date[255], *temp;
  int day, month, year;
  char cmonth[25];

  strcpy(date, line);

  temp = date;

  while(!isdigit(*temp)) temp++;

  /* sscanf(temp, "%d %s %d", &day, cmonth, &year); */
  sscanf(temp, "%d-%s-%d", &day, cmonth, &year);

  for(month = 0; months[month] != NULL; month++)
    /* if(!strcmp(cmonth, months[month])) break; */
    if(!strcasecmp(cmonth, months[month])) break; /* was stricmp !! */

  if (year > 99) year = year % 100;

  if(day > 0 && 
     month < 12 &&
     year > 0) {
    return (10000 * year + 100 * (month+1) + day);
  }
  return 0;
}

long genbank_date_function(line)
char *line;
{
  if ((strlen(line) > genbank_data_tab) && substrcmp(line, "LOCUS       ")){
    return(genbank_getdate(line+genbank_date_tab));
  }
  else 
    return -1;
}

  

char *genbank_def = bio_header1;
char *genbank_accession= bio_header2;

void genbank_header_function(line)
char *line;
{
  if ((strlen(line) > genbank_data_tab) && substrcmp(line, "DEFINITION  ") &&
     (strlen(genbank_def) == 0)){
    strncpy(genbank_def, line + genbank_data_tab, MAX_HEADER_LEN);
    trim_trailing_newline(genbank_def);
    }
	
  else if ((strlen(line) > genbank_data_tab) &&
     substrcmp(line, "ACCESSION   ") &&
     (strlen(genbank_accession) == 0)){
	/* cut extra acc. numbers from this -- we want only 1st */
	char *cp;
	for (cp=line+genbank_data_tab; *cp==' '; cp++) ;
    strncpy(genbank_accession, cp, MAX_HEADER_LEN);
	cp= strchr(genbank_accession, ' ');
	if (cp!=NULL) *cp=0; /* drop after 1st */
    trim_trailing_newline(genbank_accession);
    }
  
  genbank_filter_for_index( line);
  
}

void genbank_finish_header_function(header)
char *header;
{
  if(strlen(genbank_def) != 0 && strlen(genbank_accession) != 0){
    strncpy(header, genbank_accession, MAX_HEADER_LEN);
    s_strncat(header, " ", MAX_HEADER_LEN, MAX_HEADER_LEN);
    s_strncat(header, genbank_def, MAX_HEADER_LEN, MAX_HEADER_LEN);
  }
  else if(strlen(genbank_def) != 0){
    strncpy(header, genbank_def, MAX_HEADER_LEN);
  }
  else if(strlen(genbank_accession) != 0){
    strncpy(header, genbank_accession, MAX_HEADER_LEN);
  }
  else{
    strcpy(header, "Unknown Entry");
  }
  genbank_def[0] = '\0';
  genbank_accession[0] = '\0';
}


/* ==========================================
 *
 * ===  PIR Protein Customizations  ===
 *
 * d.g.gilbert, 11Mar92, 
 * gilbertd@bio.indiana.edu
 *
 * ==========================================
 */

#define pir_data_tab	16

/* pir Flat-file format:
ENTRY           CCHU       #Type Protein
12345678901234567890123456789012345678901234567890123456789012345678901234567890
.........1.........2.........3.........4.........5.........6.........7.........8
all data starts at tab=17 or further

ENTRY       blah	<< Start entry, index it
TITLE	    blah   	<< Index def line == HEADER line
ACCESSION   blah	<< Index acc line
KEYWORDS    blah	<< index keywords
SOURCE      blah	<< index source
REFERENCE   blah	<< Index
SUPERFAMILY blah	<< Index
            blah	<< Index
ANYOTHERS	jazz	<< skipit
any word starting w/ "#", skipit
///  				<< end of entry == entry separator
ENTRY           CCHU       #Type Protein
TITLE           Cytochrome c - Human
DATE            #Sequence 30-Sep-1991 #Text 30-Sep-1991
PLACEMENT          1.0    1.0    1.0    1.0    1.0
SOURCE          Homo sapiens #Common-name man
ACCESSION       A31764\ A05676\ A00001
REFERENCE
   #Authors     Evans M.J., Scarpulla R.C.
   #Journal     Proc. Natl. Acad. Sci. U.S.A. (1988) 85:9625-9629
   #Title       The human somatic cytochrome c gene: two classes of
                  processed pseudogenes demarcate a period of rapid
                  molecular evolution.
   #Reference-number A31764
   #Accession   A31764
   #Molecule-type DNA
   #Residues    1-105 <EVA>
   #Cross-reference GB:M22877
REFERENCE
   #Authors     Matsubara H., Smith E.L.
   #Journal     J. Biol. Chem. (1963) 238:2732-2753
   #Reference-number A05676
   #Accession   A05676
   #Molecule-type protein
   #Residues    2-28;29-46;47-100;101-105 <MATS>
REFERENCE
   #Authors     Matsubara H., Smith E.L.
   #Journal     J. Biol. Chem. (1962) 237:3575-3576
   #Reference-number A00001
   #Comment     66-Leu is found in 10% of the molecules in pooled
                  protein.
GENETIC
   #Introns        57/1
SUPERFAMILY     #Name cytochrome c
KEYWORDS        acetylation\ electron transport\ heme\
                  mitochondrion\ oxidative phosphorylation\
                  polymorphism\ respiratory chain
FEATURE
   2-105                   #Protein cytochrome c (experimental)
                             <MAT>\
   2                       #Modified-site acetylated amino end
                             (experimental)\
   15,18                   #Binding-site heme (covalent)\
   19,81                   #Binding-site heme iron (axial ligands)
SUMMARY       #Molecular-weight 11749  #Length 105  #Checksum  3247
SEQUENCE
                5        10        15        20        25        30
      1 M G D V E K G K K I F I M K C S Q C H T V E K G G K H K T G
     31 P N L H G L F G R K T G Q A P G Y S Y T A A N K N K G I I W
     61 G E D T L M E Y L E N P K K Y I P G T K M I F V G I K K K E
     91 E R A D L I A Y L K K A T N E
///

*****/


void pir_filter_for_index(line)
char* line;
{
  /* check whether to index anything in line, 
   * call this from pir_header_function which is called for
   * each line.  
   * Blank out parts of line not for indexing...
   */
  char *c;
  long  i;
  
  if (strlen(line) <= pir_data_tab) {
	 for (c=line ; *c>=' '; c++) *c=' ';
	 keepindexing= false;  
  	 }
 
/* drop some ref junk that is not of much indexing interest... */
  else if (substrcmp(line, "   #Reference-number ") 
  	|| substrcmp(line, "   #Residues ")
  	|| substrcmp(line, "   #Accession ")
   	|| substrcmp(line, "   #Residues ")
    	|| substrcmp(line, "   #Cross-reference ")
   	|| substrcmp(line, "   #Molecule-type ")
   	|| substrcmp(line, "   #Journal ") ) {   
	 for (c=line ; *c>=' '; c++) *c=' ';
	 /* keepindexing is based on last main keyword (ENTRY, REF...) */
  	 }

  else if (substrcmp(line, "   ")) {  
	/* some good & bad continuation lines start like this */
  	 if (!keepindexing) for (c=line ; *c>=' '; c++) *c=' ';
     }
	 	
  else if ( 
   	   substrcmp(line, "ENTRY           ") 
  	|| substrcmp(line, "TITLE           ")
  	|| substrcmp(line, "SOURCE          ")
  	|| substrcmp(line, "ACCESSION       ")
  	|| substrcmp(line, "REFERENCE")  
/* REFERENCE line seems to have no data on line, but it follows (keepindexing) */
  	|| substrcmp(line, "SUPERFAMILY     ")
  	|| substrcmp(line, "KEYWORDS        ")
	){
	 for (c=line, i=0; *c>=' ' && i<pir_data_tab; i++, c++) *c=' ';
	 keepindexing= true;
     }

  else {
	 for (c=line ; *c>=' '; c++) *c=' ';
	 keepindexing= false;
     }
  
  /* pir -- blank out #words */
  for (c=line; *c != 0; ) {
    if (*c=='#') do { *c++=' '; } while (*c > ' ');
    else c++;    
    }

}



boolean pir_separator_function(line)
char *line;
{
/* !! with /// as separator, we get /// at top of entry which will
   screw up seqanal software... */
/*  if ((strlen(line) > 1) && (0==strncmp(line, "///", 2))){
     return(true);
     }  
*/
  if ((strlen(line) > pir_data_tab) && substrcmp(line, "ENTRY           ")){
     return(true);
     }
  else{
    return(false);
    }
}


long pir_date_function(line)
char *line;
{ /* later maybe */
    return -1;
}

  

char *pir_def = bio_header1;
char *pir_accession= bio_header2;

void pir_header_function(line)
char *line;
{
  if ((strlen(line) > pir_data_tab) &&
     substrcmp(line, "TITLE           ") &&
     (strlen(pir_def) == 0)){
    strncpy(pir_def, line + pir_data_tab, MAX_HEADER_LEN);
    trim_trailing_newline(pir_def);
    }
	
  else if ((strlen(line) > pir_data_tab) &&
     substrcmp(line, "ACCESSION      ") &&
     (strlen(pir_accession) == 0)){
	/* cut extra acc. numbers from this -- we want only 1st */
	char *cp;
	for (cp=line+pir_data_tab; *cp==' '; cp++) ;
            strncpy(pir_accession, cp, MAX_HEADER_LEN);
	cp= strchr(pir_accession, ' ');
	if (cp!=NULL) *cp=0; /* drop after 1st */
        trim_trailing_newline(pir_accession);
        }
  
  pir_filter_for_index( line);
  
}

void pir_finish_header_function(header)
char *header;
{
  if(strlen(pir_def) != 0 && strlen(pir_accession) != 0){
    strncpy(header, pir_accession, MAX_HEADER_LEN);
    s_strncat(header, " ", MAX_HEADER_LEN, MAX_HEADER_LEN);
    s_strncat(header, pir_def, MAX_HEADER_LEN, MAX_HEADER_LEN);
  }
  else if(strlen(pir_def) != 0){
    strncpy(header, pir_def, MAX_HEADER_LEN);
  }
  else if(strlen(pir_accession) != 0){
    strncpy(header, pir_accession, MAX_HEADER_LEN);
  }
  else{
    strcpy(header, "Unknown Entry");
  }
  pir_def[0] = '\0';
  pir_accession[0] = '\0';
}



/* ==========================================
 * ===    EMBL Flat-file Customizations  ===
 * d.g.gilbert, 23Feb92, 
 * ==========================================
 */

#define embl_data_tab	5

/* EMBL Flat-file format:

ID   BAAMYLA    standard; DNA; PRO; 7872 BP.
1234567890
XX
AC   X62835;
XX
DT   12-NOV-1991 (Rel. 29, Last updated, Version 1)
DT   12-NOV-1991 (Rel. 29, Created)
XX
DE   B.acidocaldarius amy gene for amylase
XX
KW   amy gene; amylase.
XX
OS   Bacillus acidocaldarius
OC   Prokaryota; Bacteria; Firmicutes; Endospore-forming rods and cocci;
OC   Bacillaceae; Bacillus.
XX
RN   [1]
RP   1-7872
RA   Hemila H.O.;
RT   ;
RL   Submitted (22-OCT-1991) on tape to the EMBL Data Library by:
RL   H.O. Hemila, Institute of Biotechnology, Valimotie 7, 00380
RL   Helsinki, FINLAND
XX
RN   [2]
RP   1-7872
RA   Koivula T., Hemilae H.;
RT   ;
RL   Unpublished.
XX
CC   *source: strain=ATCC 27009;
CC   *source: clone_library=lambda gt-10;
XX
FH   Key             Location/Qualifiers
FH
FT   -35_signal      3224..3229
FT   -10_signal      3246..3251
FT   RBS             3288..3294
FT                   /note="amy gene"
FT   CDS             3297..7202
FT                   /gene="amy" /product="amylase"
FT   CDS             7332..>7872
FT                   /product="malE protein-homologue"
XX
SQ   Sequence 7872 BP; 1615 A; 2240 C; 2473 G; 1544 T; 0 other;
     cgttcctcgt gccgtccgaa gcgttcccga cgaatctgcg cggcaccgcc gcgggatctc
//
*****/



void embl_filter_for_index(line)
char* line;
{
  /* check whether to index anything in line, 
   * call this from embl_header_function which is called for
   * each line.  
   * Blank out parts of line not for indexing...
   */
  char *c;
  long  i;
  
  if (strlen(line) <= embl_data_tab) {
	 for (c=line; *c>=' '; c++) *c=' ';
  	 }
	 
  else if ( 
   	   substrcmp(line, "DE   ") 
  	|| substrcmp(line, "ID   ")
  	|| substrcmp(line, "AC   ")
  	|| substrcmp(line, "KW   ")
  	|| substrcmp(line, "OS   ")
  	|| substrcmp(line, "OC   ")
  	|| substrcmp(line, "RA   ")
  	|| substrcmp(line, "RT   ")
	){
	 for (c=line, i=0; *c>=' ' && i<embl_data_tab; i++, c++) *c=' ';
     }

  else {
	 for (c=line ; *c>=' '; c++) *c=' ';
     }
}



boolean embl_separator_function(line)
char *line;
{
/* !! with // as separator, we get // at top of entry which will
   screw up seqanal software... */
/*  if ((strlen(line) > 1) && (0==strncmp(line, "//", 2))){
     return(true);
     }  
*/
  if ((strlen(line) > embl_data_tab) && substrcmp(line, "ID   ")){
     return(true);
     }
  else{
    return(false);
    }
}

/* embl date == 30-SEP-1988 == genbank_date*/

long embl_date_function(line)
char *line;
{
  if ((strlen(line) > embl_data_tab) && substrcmp(line, "DT   ")){
    return(genbank_getdate(line+embl_data_tab));
  }
  else 
    return -1;
}

  

char *embl_def = bio_header1;
char *embl_accession= bio_header2;

void embl_header_function(line)
char *line;
{
  if ((strlen(line) > embl_data_tab) &&
     substrcmp(line, "DE   ") &&
     (strlen(embl_def) == 0)){
    strncpy(embl_def, line + embl_data_tab, MAX_HEADER_LEN);
    trim_trailing_newline(embl_def);
    }
	
  else if ((strlen(line) > embl_data_tab) &&
     substrcmp(line, "AC   ") &&
     (strlen(embl_accession) == 0)){
	/* cut extra acc. numbers from this -- we want only 1st */
	char *cp;
	for (cp=line+embl_data_tab; *cp==' '; cp++) ;
  	strncpy(embl_accession, cp, MAX_HEADER_LEN);
	cp= strchr(embl_accession, ' ');
	if (cp!=NULL) *cp=0; /* drop after 1st */
    	trim_trailing_newline(embl_accession);
    }
  
  embl_filter_for_index( line);
  
}

void embl_finish_header_function(header)
char *header;
{
  if(strlen(embl_def) != 0 && strlen(embl_accession) != 0){
    strncpy(header, embl_accession, MAX_HEADER_LEN);
    s_strncat(header, " ", MAX_HEADER_LEN, MAX_HEADER_LEN);
    s_strncat(header, embl_def, MAX_HEADER_LEN, MAX_HEADER_LEN);
  }
  else if(strlen(embl_def) != 0){
    strncpy(header, embl_def, MAX_HEADER_LEN);
  }
  else if(strlen(embl_accession) != 0){
    strncpy(header, embl_accession, MAX_HEADER_LEN);
  }
  else{
    strcpy(header, "Unknown Entry");
  }
  embl_def[0] = '\0';
  embl_accession[0] = '\0';
}



/* ==========================================
 *
 * ===  Prosite Dat & Doc Customizations  ===
 *
 * d.g.gilbert, 18feb92, 
 * gilbertd@bio.indiana.edu
 *
 * ==========================================
 */

#define prositedat_data_tab	5

/* Prosite DOC format:

{END}
{PDOC00002}
{PS00002; GLYCOSAMINOGLYCAN}
{BEGIN}
*************************************
* Glycosaminoglycan attachment site *
*************************************

Proteoglycans [1] are complex glycoconjugates consisting of a core  protein to
which a variable number of glycosaminoglycan chains  (such as heparin sulfate,
chondroitin sulfate, etc.) are covalently attached. The glycosaminoglycans are
attached to the core proteins through  a xyloside residue which is  in turn is
linked to  a serine   residue of the protein.    A consensus sequence for  the
attachment  site seems  to exist [2].   However,  it must be noted  that  this
consensus is only based on the sequence of three proteoglycans core proteins.

-Consensus pattern: S-G-x-G
                    [S is the attachment site]
 Additional rule: There must be at least  two acidic amino acids from -2 to -4
                  relative to the serine.
-Last update: June 1988 / First entry.

[ 1] Hassel J.R., Kimura J.H., Hascall V.C.
     Annu. Rev. Biochem. 55:539-567(1986).
[ 2] Bourdon M.A., Krusius T., Campbell S., Schwarz N.B.
     Proc. Natl. Acad. Sci. U.S.A. 84:3194-3198(1987).
{END}
{PDOC00003}
{PS00003; SULFATATION}
{BEGIN}

*****/

/* Prosite DAT format:
//
ID   ASN_GLYCOSYLATION; PATTERN.
1234567890
AC   PS00001;
DT   APR-1990 (CREATED); APR-1990 (DATA UPDATE); APR-1990 (INFO UPDATE).
DE   N-glycosylation site.
PA   N-{P}-[ST]-{P}.
CC   /TAXO-RANGE=??E?V;
CC   /SITE=1,carbohydrate;
CC   /SKIP-FLAG=TRUE;
DO   PDOC00001;
//

*****/

boolean prositedoc_separator_function(line)
char *line;
{
  if ((strlen(line) > strlen("{END}")) && substrcmp(line, "{END}")){
     return(true);
     }
  else{
    return(false);
    }
}


char *prositedoc_def = bio_header1;
char *prositedoc_accession= bio_header2;

void prositedoc_header_function(line)
char *line;
{
  if ((strlen(line)>2) && (line[0]=='*') && (line[1]==' ') && 
     (strlen(prositedoc_def) == 0)){
    strncpy(prositedoc_def, line + 2, MAX_HEADER_LEN);
    trim_trailing_newline(prositedoc_def);
    }
  else if ((strlen(line)>2) && (line[0]=='{') && 
     (!substrcmp(line, "{END}")) &&
     (strlen(prositedoc_accession) == 0)){
	char *cp;
    strncpy(prositedoc_accession, line+1, MAX_HEADER_LEN);
	cp= strchr(prositedoc_accession, '}');
	if (cp!=NULL) *cp=0;  
    trim_trailing_newline(prositedoc_accession);
    }
  
}

void prositedoc_finish_header_function(header)
char *header;
{
  if(strlen(prositedoc_def) != 0 && strlen(prositedoc_accession) != 0){
    strncpy(header, prositedoc_accession, MAX_HEADER_LEN);
    s_strncat(header, " ", MAX_HEADER_LEN, MAX_HEADER_LEN);
    s_strncat(header, prositedoc_def, MAX_HEADER_LEN, MAX_HEADER_LEN);
  }
  else if(strlen(prositedoc_def) != 0){
    strncpy(header, prositedoc_def, MAX_HEADER_LEN);
  }
  else if(strlen(prositedoc_accession) != 0){
    strncpy(header, prositedoc_accession, MAX_HEADER_LEN);
  }
  else{
    strcpy(header, "Unknown Entry");
  }
  prositedoc_def[0] = '\0';
  prositedoc_accession[0] = '\0';
}


boolean prositedat_separator_function(line)
char *line;
{
/* !! with // as separator, we get // at top of entry which will
   screw up seqanal software... */
/*  if ((strlen(line) > 1) && (0==strncmp(line, "//", 2))){
     return(true);
     }  
*/
  if ((strlen(line) > prositedat_data_tab) && substrcmp(line, "ID  ")){
     return(true);
     }
  else{
    return(false);
    }
}


char *prositedat_def = bio_header1;
char *prositedat_accession= bio_header2;

void prositedat_header_function(line)
char *line;
{
  int   i;

  if ((strlen(line) > prositedat_data_tab) &&
     substrcmp(line, "DE   ") &&
     (strlen(prositedat_def) == 0)){
    strncpy(prositedat_def, line + prositedat_data_tab, MAX_HEADER_LEN);
    trim_trailing_newline(prositedat_def);
    }
	
  else if ((strlen(line) > prositedat_data_tab) &&
     substrcmp(line, "AC   ") &&
     (strlen(prositedat_accession) == 0)){
	/* cut extra acc. numbers from this -- we want only 1st */
	char *cp;
	for (cp=line+prositedat_data_tab; *cp==' '; cp++) ;
    strncpy(prositedat_accession, cp, MAX_HEADER_LEN);
	cp= strchr(prositedat_accession, ' ');
	if (cp!=NULL) *cp=0; /* drop after 1st */
    trim_trailing_newline(prositedat_accession);
    }
   
  if (strlen(line) > prositedat_data_tab)
	for (i=0; i<prositedat_data_tab; i++) line[i]= ' '; 
  
}

void prositedat_finish_header_function(header)
char *header;
{
  if(strlen(prositedat_def) != 0 && strlen(prositedat_accession) != 0){
    strncpy(header, prositedat_accession, MAX_HEADER_LEN);
    s_strncat(header, " ", MAX_HEADER_LEN, MAX_HEADER_LEN);
    s_strncat(header, prositedat_def, MAX_HEADER_LEN, MAX_HEADER_LEN);
  }
  else if(strlen(prositedat_def) != 0){
    strncpy(header, prositedat_def, MAX_HEADER_LEN);
  }
  else if(strlen(prositedat_accession) != 0){
    strncpy(header, prositedat_accession, MAX_HEADER_LEN);
  }
  else{
    strcpy(header, "Unknown Entry");
  }
  prositedat_def[0] = '\0';
  prositedat_accession[0] = '\0';
}

/* ==================== 
 *      Bio Journals        
 * (modified EMBL format) 
 * dgg
 * ================== 
 */

/******
//
RA   Casida L.E. Jr.;
123456
RT   "Protozoan Response to the Addition of Bacterial Predators and Other
RT   Bacteria to Soil.";
RL   Appl. Environ. Microbiol. 55:1857-1859(1989).
//
RA   Caldwell B.A., Ye C., Griffiths R.P., Moyer C.L., Morita R.Y.;
RT   "Plasmid Expression and Maintenance during Long-Term Starvation-Survival
RT   of Bacteria in Well Water.";
RL   Appl. Environ. Microbiol. 55:1860-1864(1989).
//
*******/

#define biojournal_tab	5

boolean biojournal_separator_function(line)
char *line;
{
  if ((strlen(line) > 1) && (0==strncmp(line, "//", 2))){
     return(true);
     }  
/*  if ((strlen(line) > biojournal_tab) && substrcmp(line, "RA   ")){
     return(true);
     }
*/
  else{
    return(false);
    }
}


char *biojournal_title = bio_header1;
char *biojournal_author= bio_header2;

void biojournal_header_function(line)
char *line;
{
  int   i;

  if ((strlen(line) > biojournal_tab) && substrcmp(line, "RT   ") &&
     (strlen(biojournal_title) == 0)){
    strncpy(biojournal_title, line + biojournal_tab, MAX_HEADER_LEN);
    trim_trailing_newline(biojournal_title);
    }
	
  else if ((strlen(line) > biojournal_tab) && substrcmp(line, "RA   ") &&
     (strlen(biojournal_author) == 0)){
	char *cp;
    strncpy(biojournal_author, line+biojournal_tab, MAX_HEADER_LEN);
	cp= strchr(biojournal_author, ' ');
	if (cp!=NULL) *cp=0; /* drop after 1st */
    trim_trailing_newline(biojournal_author);
    }
   
   if (strlen(line) > biojournal_tab)
	for (i=0; i<biojournal_tab; i++) line[i]= ' '; 
}

void biojournal_finish_header_function(header)
char *header;
{
  if(strlen(biojournal_title) != 0 && strlen(biojournal_author) != 0){
    strncpy(header, biojournal_author, MAX_HEADER_LEN);
    s_strncat(header, " ", MAX_HEADER_LEN, MAX_HEADER_LEN);
    s_strncat(header, biojournal_title, MAX_HEADER_LEN, MAX_HEADER_LEN);
  }
  else if(strlen(biojournal_title) != 0){
    strncpy(header, biojournal_title, MAX_HEADER_LEN);
  }
  else if(strlen(biojournal_author) != 0){
    strncpy(header, biojournal_author, MAX_HEADER_LEN);
  }
  else{
    strcpy(header, "Unknown Entry");
  }
  biojournal_title[0] = '\0';
  biojournal_author[0] = '\0';
}


/* ==========================================
 *
 * ===  Drosophila Redbook Customizations  ===
 *
 * d.g.gilbert, 18feb92, 
 * gilbertd@bio.indiana.edu
 * ==========================================
 */

/*------ example
  #Abnormal:  see A
  #abnormal abdomen:  see a( )
  #Abnormal abdomen:  see A
  # abnormal eye:  see mit15
  #abnormal oocytes:  see abo
  #abnormal tergites:  see abt
  #abnormal wings:  see abw
  #abo:  abnormal oocyte
  location:  2-44.0 (mapped with respect to J, 2-41).
  origin:  Naturally occurring allele recovered near Rome,
  Italy.
  references:  Sandler, Lindsley, Nicoletti, and Trippa,
  ...
----*/


boolean redbook_separator_function(line)
char *line;
{
  if(*line=='#'){
    return(true);
  }
  else{
    return(false);
  }
}

char *redbook_header = bio_header1;

void redbook_header_function(line)
char *line;
{
  if(redbook_separator_function(line)){
    strncpy(redbook_header, line + 1, MAX_HEADER_LEN);
  }
}

void redbook_finish_header_function(header)
char *header;
{
  if(strlen(redbook_header) == 0){
    strncpy(header, "Unknown", MAX_HEADER_LEN);
  }
  else{
    strncpy(header, redbook_header, MAX_HEADER_LEN);
  }
  redbook_header[0] = '\0';
}


/* ==========================================
 *
 * ===  Drosophila flybase Customizations  ===
 *
 * d.g.gilbert, 18feb92, 
 * gilbertd@bio.indiana.edu
 * ==========================================
 */

/*----------------------------------------
::::::::::::::
ABAUTOSY.TEXT
::::::::::::::
LS(2)P6
     24E-24F        28A-28D             A
LS(2)P11
     25E-25F        35D                 A
::::::::::::::
ABDELETE.TEXT
::::::::::::::
Df(1)FM7
     1A             1B2-1B3             Df   |l(1)1Aa--ac|
In(1)y3P$+L$-sc8$+R$-
     1A             1B2-1B3             Df   |y--ac|
::::::::::::::
ABDUPLIC.TEXT
::::::::::::::
In(1)sc8$+L$-EN$+R$-
     1A             1B2-1B3             Dp   |l(1)1Ac--ac|
In(1)sc8$+L$-y3P$+R$-
     1A             1B2-1B3             Dp   |y--ac|
::::::::::::::
ABINSERT.TEXT
::::::::::::::
TE298
     1E             []                  I
TE276
     3A1-3A2        []                  I
::::::::::::::
ABINVERT.TEXT
::::::::::::::
In(1)l-v227
     1-2            19-20               In
In(1)y-G
     1A             1C3-1C4             In   |y--y|;;
::::::::::::::
ABREFS.TEXT
::::::::::::::
3R3L.3R                                 Novitski, Genetics 98:257
B$+S$- v$++$- y$++Y$-                   Voelker, Genetics 107:279
B$+S$-Ybb$+l$-                          Polembo, Molec.Gen.Genet. 195:35
::::::::::::::
ABTRANSL.TEXT
::::::::::::::
T(1;2)gl$++$-
     1A             21C1                T
T(1;2)y-v1
     1A             39                  T    |y--y|;;
T(1;2)SP55
     1A             41                  T
::::::::::::::
ABTRANSP.TEXT
::::::::::::::
Tp(3;1)pn36
     1A             61A                 Tp
Tp(1;1)Si2
     1A1-1A8        14D2-14E1           Tp   |r--r|;;
Tp(1;1)Si2
     1A1-1A8        18F                 Tp
::::::::::::::
COSMID.TEXT
::::::::::::::
1A\S\T\U      0          0            0                23E12
1A\VS         0          ~50          BH\W             125H10
1B\M\U        0          0            0                88B3
::::::::::::::
FUNCTION.TEXT
::::::::::::::
3-hydroxy-3-methylglutaryl-coenzyme-A-reductase               1.1.1.34  HmG-CoAR
6-pyruvoyl-tetrahydropterin-synthase                                     pr
14-3-3-protein                                                           D14-3-3
::::::::::::::
LOCI.TEXT
::::::::::::::
3S18;    3S18-element
              repetitive-element-3S18
4.5SRNA;    4.5SRNA
          3-[21]     65A
              RNA-4.5S
5HT-R1;    serotonin-receptor-1
          3-[102]    100A
              serotonin-receptor
              transmembrane-protein
              G-protein-coupled-receptor
5HT-R2A;    serotonin-receptor
          2-[87]     56A-56B
::::::::::::::
LZSYN.TEXT
::::::::::::::
 Acp-g1              AcpG
 Acr96A              Acr96Aa
 Aldox-1             Aldox1
::::::::::::::
MAP.TEXT
::::::::::::::
1-[0]          1A6 ?                            l(1)Ac
1-0.0                                           cc
1-0.0                                           clv-1
::::::::::::::
REFS.TEXT
::::::::::::::
4.5SRNA              Steffenson            Genetics 110:s84
5HT-R1               Boschert              12th Europ.Dros.Conference
5HT-R2A              Boschert              12th Europ.Dros.Conference
::::::::::::::
SYNONYMS.TEXT
::::::::::::::
1C                                Pk36A
1J                                Pk91C
2sm$+lab$-                        sm
3-2                               Pk45C
::::::::::::::
UID.TEXT
::::::::::::::
00001;4.5SRNA
00002;5SRNA
00003;7SLRNA
00004;17.6
-----------------------------*/

/*  need something like this for some doc formats.... */



boolean flybase_separator_function(line)
char *line;
{
  if (isgraph(*line)) {
    return(true);
  }
  else{
    return(false);
  }
}

char *flybase_header = bio_header1;

void flybase_header_function(line)
char *line;
{
  char *c;
	int  i;
	
  if (flybase_separator_function(line)) {
	  for (c=line, i=0; isgraph(*c) && (i<MAX_HEADER_LEN); )  
			flybase_header[i++]= *c++; 
		flybase_header[i]= '\0';
    }
}

void flybase_finish_header_function(header)
char *header;
{
  if(strlen(flybase_header) == 0){
    strncpy(header, "Unknown", MAX_HEADER_LEN);
  }
  else{
    strncpy(header, flybase_header, MAX_HEADER_LEN);
  }
  flybase_header[0] = '\0';
}

/* ==========================================
 * DIN news -- like BIO but "***" separator 
 * dgg
 */

boolean din_hit_head = false;
char din_header[MAX_HEADER_LEN + 1];

boolean din_separator_function(line)
char *line;
{
  if ((strlen(line) >= 3) && substrcmp(line, "***")) {
    return(true);
  }
  else{
    return(false);
  }
}


void din_header_function(line)
char *line;
{
  if(din_hit_head	/* we just hit a seperator previous to this */
     && strlen(line) > 3		/* line is valid */
     && isalnum(*line)			/* and is word */
     && (!din_separator_function(line)) /* we are not on the separator now */
     && strlen(din_header) == 0){ /* and we have not saved the headline yet */
    strcpy(din_header, line);
    waislog(WLOG_MEDIUM, WLOG_INDEX, "storing line: %s", din_header);
    din_hit_head = false;
  }
}

void din_finish_header_function(header)
char *header;
{
  din_hit_head = true;  /* turn on the flag */
  if(strlen(din_header) == 0){
    strcpy(header, "Unknown Title");
  }
  else{
    strcpy(header, din_header);
  }
  din_header[0] = '\0';
}



#endif /* BIO */


/* =================================
 * ===  Groliers Customizations  ===
 * =================================
 */

boolean groliers_separator_function(line)
char *line;
{
  if((strlen(line) > strlen("ARTICLE")) &&
     substrcmp(line, "ARTICLE")){
    /* printf("hit %s\n", line); */
    return(true);
  }
  else{
    return(false);
  }
}

char groliers_header[MAX_HEADER_LEN + 1];

void groliers_header_function(line)
char *line;
{
  if(groliers_separator_function(line)){
    s_strncpy(groliers_header, line + strlen("ARTICLE") + 2, MAX_HEADER_LEN);
  }
}

void groliers_finish_header_function(header)
char *header;
{
  if(strlen(groliers_header) == 0){
    s_strncpy(header, "Unknown Title", MAX_HEADER_LEN);
  }
  else{
    s_strncpy(header, groliers_header, MAX_HEADER_LEN);
  }
  groliers_header[0] = '\0';
}


/* ==============================
 * ===  RMail Customizations  ===
 * ==============================
 */

/* this is just a preliminary version. A good version would
 * produce a headline like gnu emacs RMAIL
 */


boolean mail_separator_function(line)
char *line;
{
  /* this should really look for a "<cr><cr>From " rather than "<cr>From " */
  if((strlen(line) > strlen("From ")) &&
     substrcmp(line, "From ")){
    return(true);
  }
  else{
    return(false);
  }
}

boolean rmail_separator_function(line)
char *line;
{
  if(0 == strcmp(line, "\n")){
    return(true);
  }
  else{
    return(false);
  }
}

/* This one is portable, but might get the wrong answer.
   I'm open to better code.  - Jonny G
*/


long my_getdate(line)
char *line;
{
  char date[255], *temp;
  int day, month, year;
  char cmonth[25], dow[5], tod[10];

  strcpy(date, line);

  temp = date;

  while(!isdigit(*temp)) temp++;

  sscanf(temp, "%d %25s %d", &day, cmonth, &year);

  for(month = 0; months[month] != NULL; month++)
    if(!strcmp(cmonth, months[month])) break;

  if (year > 99) year = year % 100;

  if(day > 0 && 
     month < 12 &&
     year > 0) {
    return (10000 * year + 100 * (month+1) + day);
  }

  month = -1; day = -1; year = -1;

  sscanf(temp, "%d/%d/%d", &month, &day, &year);

  if (year > 99) year = year % 100;

  if(day > 0 && 
     month < 12 &&
     year > 0) {
    return (10000 * year + 100 * (month+1) + day);
  }

  month = -1; day = -1; year = -1;

  sscanf(temp, "%d/%d/%d", &year, &month, &day);

  if (year > 99) year = year % 100;

  if(day > 0 && 
     month < 12 &&
     year > 0) {
    return (10000 * year + 100 * (month+1) + day);
  }

  temp = date;

  sscanf(temp, "%5s %25s %d %10s %d", dow, cmonth, &day, tod, &year);

  for(month = 0; months[month] != NULL; month++)
    if(!strcmp(cmonth, months[month])) break;

  if (year > 99) year = year % 100;

  if(day > 0 && 
     month < 12 &&
     year > 0) {
    return (10000 * year + 100 * (month+1) + day);
  }

  return 0;
}

long mail_date_function(line)
char *line;
{
  if((strlen(line) > strlen("Date: ")) &&
     substrcmp(line, "Date: ")){
    return(my_getdate(line+6));
  }
  else if((strlen(line) > strlen("From ")) &&
	  substrcmp(line, "From ")){
    char *p;
    p = (char*)index(line+5, ' ');
    if(p != NULL)
      return(my_getdate(p+1));
  }
  else return -1;
}

  

char mail_subject[MAX_HEADER_LEN + 1];
char mail_from[MAX_HEADER_LEN + 1];

void mail_header_function(line)
char *line;
{
  if((strlen(line) > strlen("Subject: ")) &&
     substrcmp(line, "Subject: ") &&
     (strlen(mail_subject) == 0)){
    strcpy(mail_subject, "Re: ");
    s_strncat(mail_subject, line + strlen("Subject: "), MAX_HEADER_LEN, MAX_HEADER_LEN);
    trim_trailing_newline(mail_subject);
  }
  else if((strlen(line) > strlen("From: ")) &&
     substrcmp(line, "From: ") &&
     (strlen(mail_from) == 0)){
    /* this should find the <foo@bar> field in the from list */
    s_strncpy(mail_from, line + strlen("From: "), MAX_HEADER_LEN);
    trim_trailing_newline(mail_from);
  }
  
}

void mail_finish_header_function(header)
char *header;
{
  if(strlen(mail_subject) != 0 &&
     strlen(mail_from) != 0){
    /* trim the from line if needed */
    if(strlen(mail_from) > 10){
      mail_from[10] = '\0';
    }
    s_strncpy(header, mail_from, MAX_HEADER_LEN);
    s_strncat(header, " ", MAX_HEADER_LEN, MAX_HEADER_LEN);
    s_strncat(header, mail_subject, MAX_HEADER_LEN, MAX_HEADER_LEN);
    /* printf("%s\n", header); */
  }
  else if(strlen(mail_subject) != 0){
    s_strncpy(header, mail_subject, MAX_HEADER_LEN);
  }
  else if(strlen(mail_from) != 0){
    s_strncpy(header, mail_from, MAX_HEADER_LEN);
  }
  else{
    strcpy(header, "Unknown Subject");
  }
  mail_from[0] = '\0';
  mail_subject[0] = '\0';
}




boolean mail_or_rmail_separator(line)
char *line;
{
  static boolean blank_line = false;

  if((strlen(line) > strlen("From ")) &&
     substrcmp(line, "From ") &&
     blank_line == true){
    blank_line = false;
    return(true);
  }
  
  if(substrcmp(line, "")){
    blank_line = true;
    return(true);
  }    
  
  if(!strcmp(line, "\n")){
      blank_line = true;
    }
    else{
      blank_line = false;
    }

  return(false);
}


/* ========================================
 * ===  Mail Digest Customizations     ====
 * ========================================
 */

boolean mail_digest_separator_function(line)
char *line;
{
  if((strlen(line) > strlen("-----------------------------")) &&
     substrcmp(line, "------------------------------")){
    return(true);
  }
  else{
    return(false);
  }
}

/* ========================================
 * ===  Listserv Digest Customizations     ====
 * ========================================
 */

char listserv_from[MAX_HEADER_LEN + 1];
char listserv_subject[MAX_HEADER_LEN + 1];

boolean listserv_digest_separator_function(line)
char *line;
{
  if((strlen(line) > strlen("========================================")) &&
     substrcmp(line,"========================================")){
    return(true);
  }
  else{
    return(false);
  }
}  

long listserv_date_function(line)
char *line;
{
  if((strlen(line) > strlen("Date: ")) && substrcmp(line, "Date: ")){
    return(my_getdate(line+6));
  }
  else if((strlen(line) > strlen("From: ")) &&
     substrcmp(line, "From: ") && (strlen(listserv_from) == 0)){
    /* this should find the <foo@bar> field in the from list */
    s_strncpy(listserv_from, line + strlen("From:          "), MAX_HEADER_LEN);
    trim_trailing_newline(listserv_from);
  }
  else return -1;
}

void listserv_header_function(line)
char *line;
{
  if((strlen(line) > strlen("Subject: ")) &&
     substrcmp(line, "Subject: ") && (strlen(listserv_subject) == 0)){
    strcpy(listserv_subject, "Re: ");
    s_strncat(listserv_subject, line + strlen("Subject:      "), MAX_HEADER_LEN, MAX_HEADER_LEN);
    trim_trailing_newline(listserv_subject);
  }
  else if((strlen(line) > strlen("From: ")) &&
     substrcmp(line, "From: ") && (strlen(listserv_from) == 0)){
/*    printf("1: ->%s<-\n",line); */
    /* this should find the <foo@bar> field in the from list */
    s_strncpy(listserv_from, line + strlen("From:         "), MAX_HEADER_LEN);
    trim_trailing_newline(listserv_from);
    trim_leading_blanks(listserv_from);
/*    printf("2: ->%s<-\n",listserv_from); */
  }
  
}

void listserv_finish_header_function(header)
char *header;
{
  if(strlen(listserv_subject) != 0 && strlen(listserv_from) != 0){
    /* trim the from line if needed */
    if(strlen(listserv_from) > 15){
      listserv_from[15] = '\0';
    }
    trim_leading_blanks(listserv_from);
    s_strncpy(header, listserv_from, MAX_HEADER_LEN);
    s_strncat(header, " ", MAX_HEADER_LEN, MAX_HEADER_LEN);
    s_strncat(header, listserv_subject, MAX_HEADER_LEN, MAX_HEADER_LEN);
    /* printf("%s\n", header); */ 
  }
  else if(strlen(listserv_subject) != 0){
    s_strncpy(header, listserv_subject, MAX_HEADER_LEN);
  }
  else if(strlen(listserv_from) != 0){
    s_strncpy(header, listserv_from, MAX_HEADER_LEN);
  }
  else{
    strcpy(header, "Unknown Subject");
  }
  listserv_from[0] = '\0';
  listserv_subject[0] = '\0';
}

/* ========================================
 * ===  Library Catalog Customizations  ===
 * ========================================
 */

#define TITLE_MARKER "Title: "
#define FIRST_LINE_MARKER "Call No...."

/* just use the title */

boolean catalog_separator_function(line)
char *line;
{
  if (strstr(line, FIRST_LINE_MARKER)) {
    return(true);
  }
  else{
    return(false);
  }
}

char catalog_header[MAX_HEADER_LEN + 1];

void catalog_header_function(line)
char *line;
{
  char * title_start;
  if (title_start = strstr(line, TITLE_MARKER))
    {
      strncpy(catalog_header, title_start + strlen(TITLE_MARKER), MAX_HEADER_LEN);
    }
}

void catalog_finish_header_function(header)
char *header;
{
  if(strlen(catalog_header) == 0){
    strcpy(header, "Unknown Title");
  }
  else{
    s_strncpy(header, catalog_header, MAX_HEADER_LEN);
  }
  catalog_header[0] = '\0';
}



/* ============================
 * ===  Bio Customizations  ===
 * ============================
 */

/* customizations for a DB of genetic abstracts */

boolean hit_header = false;

boolean bio_separator_function(line)
char *line;
{
  if((strlen(line) > strlen(">>>")) &&
     substrcmp(line, ">>>")){
    return(true);
  }
  else{
    return(false);
  }
}

char bio_header[MAX_HEADER_LEN + 1];

void bio_header_function(line)
char *line;

{
  if(hit_header			/* we just hit a seperator previous to this */
     && (!bio_separator_function(line)) /* we are not on the separator now */
     && strlen(bio_header) == 0){ /* and we have not saved the headline yet */
    strcpy(bio_header, line);
    waislog(WLOG_MEDIUM, WLOG_INDEX, "storing line: %s", bio_header);
    hit_header = false;
  }
}

void bio_finish_header_function(header)
char *header;

{
  hit_header = true; /* turn on the flag */
  if(strlen(bio_header) == 0){
    strcpy(header, "Unknown Title");
  }
  else{
    strcpy(header, bio_header);
  }
  bio_header[0] = '\0';
}

/* =================================
 * ===  CMApp   Customizations  ===
 * =================================
 */

boolean cmapp_separator_function(line)
char *line;
{
  if((strlen(line) > strlen("@A")) &&
     substrcmp(line, "@A")){
    /* printf("hit %s\n", line); */
    return(true);
  }
  else{
    return(false);
  }
}

char cmapp_header[MAX_HEADER_LEN + 1];

void cmapp_header_function(line)
char *line;
{
  if((strlen(line) > strlen("APPLICATION:")) &&
     substrcmp(line, "APPLICATION:")){
    /* printf("hit %s\n", line); */
    s_strncpy(cmapp_header, line + strlen("APPLICATION:"), MAX_HEADER_LEN);
  }
}

void cmapp_finish_header_function(header)
char *header;
{
  if(strlen(cmapp_header) == 0){
    s_strncpy(header, "Unknown Title", MAX_HEADER_LEN);
  }
  else{
    s_strncpy(header, cmapp_header, MAX_HEADER_LEN);
  }
  cmapp_header[0] = '\0';
}

/* =================================
 * ===  Jargon   Customizations  ===
 * =================================
 *
 * GW - updated for Jargon File 2.9.8
 */

/*

Format of an entry:

[blank line]
:Title of This entry: first line of text of this entry
   second line of text of this entry
   third line of text of this entry
[blank line]

Any line which starts with a colon is considered to be the beginning
of an entry.

-GW

*/

static int jargon_seen_entry = 0;

boolean jargon_separator_function(line)
register char *line;
{
  if(!jargon_seen_entry && line[0] == ':')
    jargon_seen_entry = 1;
  return line[0] == ':';
}

char jargon_header[MAX_HEADER_LEN + 1];

void jargon_header_function(line)
char *line;
{
  if(line[0] != ':')
    return;

  strncpy(jargon_header,line+1,MAX_HEADER_LEN);
  jargon_header[MAX_HEADER_LEN] = '\0';

  if(NULL != (line = strchr(jargon_header,':'))){
    if(line[1] == ':')
      line++;
    line++;
    line[0] = '\0';
  }
}	

void jargon_finish_header_function(header)
char *header;
{
  if(jargon_seen_entry) {
    strncpy(header, jargon_header, MAX_HEADER_LEN);
  }
  jargon_header[0] = '\0';
}


/* =================================
 * ===  Internet Resource Guide  ===
 * =================================
 */


char irg_header[MAX_HEADER_LEN + 1];
boolean irg_header_set = FALSE;

boolean irg_separator_function(line)
char *line;
{
  if(line[0] == 12){  /* control L */
    irg_header_set = FALSE;
    return(true);
  }
  else
    return(false);
}

void irg_header_function(line)
char *line;
{
  if((irg_header_set == FALSE) &&
     (line[0] == 32 )){ /* space */
    s_strncpy(irg_header, line + strspn(line, " "), MAX_HEADER_LEN);
    irg_header_set = TRUE;
  }
  
}

void irg_finish_header_function(header)
char *header;
{
  if(strlen(irg_header) == 0){
    s_strncpy(header, "Unknown Title", MAX_HEADER_LEN);
  }
  else{
    s_strncpy(header, irg_header, MAX_HEADER_LEN);
  }
  irg_header[0] = '\0';
  irg_header_set = FALSE;
}

/* ========================
 * ===  Dash Separator  ===
 * ========================
 */


/*
 * dash-seperate entries
 * used in Introduction to Algorithms bug.list, suggestions, etc.
 * --------------------... at least 20 dashes
 * header
 * item
 *  ..
 * --------------------... at least 20 dashes
 */

boolean dash_separator_function(line)
char *line;
{
  if((strlen(line) > 20) && substrcmp(line,"--------------------")){
    /* printf("hit %s\n", line); */
    return(true);
  }
  else{
    return(false);
  }
}

char dash_header[MAX_HEADER_LEN + 1];

void dash_header_function(line)
char *line;
{
  if(!dash_separator_function(line) &&
     (strlen(dash_header) < (MAX_HEADER_LEN - 1))){
    s_strncat(dash_header, line, 
	      MAX_HEADER_LEN, MAX_HEADER_LEN);
    trim_trailing_newline(dash_header);
    strncat(dash_header, "  ", MAX_HEADER_LEN);
  }
}

void dash_finish_header_function(header)
char *header;
{
  if (strlen(dash_header) == 0) {
    strcpy(header, "No Title");
  }
  else {
    s_strncpy(header, dash_header, MAX_HEADER_LEN);
  }
  dash_header[0] = '\0';
}


/* ============================
 * ===  one_line Separator  ===
 * ============================
 */

/* this is where each line is a document (good for databases) */

boolean one_line_hit_header = false;

boolean one_line_separator_function(line)
char *line;
{
    return(true);
}

char one_line_header[MAX_HEADER_LEN + 1];

void one_line_header_function(line)
char *line;
{
    s_strncpy(one_line_header, line, MAX_HEADER_LEN);
}

void one_line_finish_header_function(header)
char *header;
{
  if (strlen(one_line_header) == 0) {
    strcpy(header, "No Title");
  }
  else {
    s_strncpy(header, one_line_header, MAX_HEADER_LEN);
  }
  one_line_header[0] = '\0';
}

/* =============================
 * ===  Paragraph Separator  ===
 * =============================
 */

/* paragraph files - seperated by a blank line.  Next line is the header */

char para_header[MAX_HEADER_LEN +1];
static boolean para_start = true;

boolean para_separator_function(line)
char *line;
{
  if (para_start == true) {
    para_start = false;
    return true;
  }
  if (strlen(line) < 2)
    para_start = true;
  return false;
}

void para_header_function(line)
char *line;
{
  if (para_header[0] == 0)
    s_strncpy(para_header, line, MAX_HEADER_LEN);
}

void para_finish_header_function(header)
char *header;
{
  if (strlen(para_header) == 0) {
    strcpy(header, "No Title");
  }
  else {
    s_strncpy(header, para_header, MAX_HEADER_LEN);
  }
  para_header[0] = 0;
}  

/* ==========================
 * ===  Seeker Separator  ===
 * ==========================
 */

boolean seeker_separator_function(line)
char *line;
{
  return(dash_separator_function(line));
}

char seeker_header[MAX_HEADER_LEN + 1];
boolean in_headline = FALSE;

void seeker_header_function(line)
char *line;
{
  if(strlen(line) > strlen("Headline:") &&
     substrcmp(line, "Headline:")){
    in_headline = TRUE;
    seeker_header[0] = '\0';
    /* printf("hit headline!\n"); */
  }
  else if(in_headline == TRUE &&
	  (strlen(seeker_header) < (MAX_HEADER_LEN - 1))){
    s_strncat(seeker_header, line, 
	      MAX_HEADER_LEN, MAX_HEADER_LEN);
    trim_trailing_newline(seeker_header);
  }
}

void seeker_finish_header_function(header)
char *header;
{
  if (strlen(seeker_header) == 0) {
    strcpy(header, "No Title");
  }
  else {
    s_strncpy(header, seeker_header, MAX_HEADER_LEN);
  }
  seeker_header[0] = '\0';
  in_headline = TRUE;
}  

/* ==========================
 * ===  RLIN Separator  ===
 * ==========================
 */

boolean rlin_separator_function(line)
char *line;
{
  return(dash_separator_function(line));
}

char rlin_header[MAX_HEADER_LEN + 1];
boolean rlin_in_headline = FALSE;

void rlin_header_function(line)
char *line;
{
  if(rlin_separator_function(line)){
    rlin_in_headline = TRUE;
    rlin_header[0] = '\0';
    /* printf("hit headline!\n"); */
  }
  else if(rlin_in_headline == TRUE &&
	  (strlen(rlin_header) < (MAX_HEADER_LEN - 1))){
    s_strncat(rlin_header, line, 
	      MAX_HEADER_LEN, MAX_HEADER_LEN);
    trim_trailing_newline(rlin_header);
  }
}

void rlin_finish_header_function(header)
char *header;
{
  if (strlen(rlin_header) == 0) {
    strcpy(header, "No Title");
  }
  else {
    s_strncpy(header, rlin_header, MAX_HEADER_LEN);
  }
  rlin_header[0] = '\0';
  in_headline = TRUE;
}  

/* ========================================
 * ===  MH_BBoard  Customizations     ====
 * ========================================
 */

/* gcardwel@uci.edu
MH bboards use a series of control A's to do a blank line.. yuk!
*/

boolean mh_bboard_separator_function(line)
char *line;
{
  static boolean blank_line = false;

  if((strlen(line) > strlen("BBoard-ID: ")) &&
     substrcmp(line, "BBoard-ID: ") &&
     blank_line == true){
    blank_line = false;
    return(true);
  }
  
  if(!strcmp(line, "\001\001\001\001\n")){
    blank_line = true;
  }
  else{
    blank_line = false;
  }
  return (false);
}

/*
 * Customization for files saved from within the 'rn' newsreader.
 *
 * These can either be in 'mail' format, or they can be in a similar
 * format which starts each article with the pseudo-header
 * 'Article: 42 of comp.sys.foobar'.  Other than that, we treat this
 * just like 'mail'.
 *
 * wollman@uvm.edu, Sun Sep  8 20:12:21 EDT 1991
 *
 * dgg added "Path:" fix for netnews/NNTP fetches (!NOT MAIL, NO "From ")
 * gilbertd@sunflower.bio.indiana.edu
 */
boolean rn_separator_function(line)
char *line;
{
  if(!strncmp(line,"From ",5) ||
    !strncmp(line,"Path: ",6) ||
    !strncmp(line,"Article ",7) ||
    !strncmp(line,"Article: ",9))
    return true;
  return false;
}

/*
 * Customization for files saved NNTP netnews fetches (!NOT MAIL FORMAT, NO "From ".
 *
 * gilbertd@sunflower.bio.indiana.edu
 */
boolean netnews_separator_function(line)
char *line;
{
  if(!strncmp(line,"From ",5) ||
     !strncmp(line,"Article ",7) ||
     !strncmp(line,"Article: ",9))
    return true;
  return false;
}

/*
 * Customizations for GNU Emacs Info files
 *
 * When indexing info files, the user must index the files with real text
 * in them, rather than the file with the tag and indirect tables; otherwise
 * you'll end up with lots of garbage in your index.
 *
 * G. Wollman
 */

static int done_headline = 0;

boolean emacs_info_separator_function(line) /* hate K&R-style definitions */
char *line;
{
  if(line[0] == (char)31) {
    done_headline = 0;
    return true;
  }
  return false;
}

static char emacs_info_headline[MAX_HEADER_LEN+1];

void emacs_info_header_function(line)
register char *line;
{
  int i;

  if(done_headline)
    return;

  if(strncmp(line,"File: ",6))
    return;

  done_headline = 1;
  line += 6;			/* skip over "File: " */

  i = 1;
  emacs_info_headline[0] = '(';
  while(*line && *line != ',' && (i < MAX_HEADER_LEN-1))
    emacs_info_headline[i++] = *line++;

  emacs_info_headline[i++] = ')';

  line += 9;			/* skip over ", Node: " */

  /* copy the name of the info node into the headline */
  while(*line && (i < MAX_HEADER_LEN) && (*line != ','))
    emacs_info_headline[i++] = *line++;

  emacs_info_headline[i++] = '\0';
}

void emacs_info_finish_header_function(header)
char *header;
{
  strcpy(header,emacs_info_headline);
}

/* ========================================
 * ===    Medline  Customizations      ====
 * ========================================
 */

/* 
	Francois Schiettecatte
	with help from:
	Tom Emmel
	Karen Phipps
*/

char medline_header[MAX_HEADER_LEN +1];
char medline_title[MAX_HEADER_LEN + 1];
char medline_date[MAX_HEADER_LEN + 1];
char medline_author[MAX_HEADER_LEN + 1];

static boolean medline_start = true;


boolean medline_separator_function(line)
char *line;
{
  if (medline_start == true) {
    medline_start = false;
    return true;
  }
  if (strlen(line) < 2)
    medline_start = true;
  return false;
}


void medline_header_function(line)
char *line;
{
 char *ptr;

 if((strlen(line) > strlen("TI ")) && 
     (substrcmp(line, "TI "))){    
    strncpy(medline_title, line + strlen("TI "), MAX_HEADER_LEN);
  }
 
  if((strlen(line) > strlen("SO ")) &&
     (substrcmp(line, "SO "))){
   ptr = strchr(line,'1');
   if (ptr) { 
     strncpy(medline_date, ptr, MAX_DATE_LEN);
   }
  }

  if((strlen(line) > strlen("AU ")) &&
     (substrcmp(line, "AU "))){
    ptr = strtok(line + strlen("AU "),".,");
    strcpy(medline_author,ptr);
    strncat(medline_author, "  ", MAX_AUTHOR_LEN); 
  } 
}

void medline_finish_header_function(header)
char *header;
{
  if(strlen(medline_author) > 0 ){
   strncat(medline_header,medline_author, MAX_HEADER_LEN);
  }
  
  if(strlen(medline_date) > 0 ){
    strncat(medline_header,"(", MAX_HEADER_LEN);
    strncat(medline_header,medline_date, MAX_HEADER_LEN);
    strncat(medline_header,") ", MAX_HEADER_LEN);
  }

  if(strlen(medline_title) > 0 ){
    strncat(medline_header,medline_title, MAX_HEADER_LEN);
  }
  
  if(strlen(medline_header) == 0){
    strcpy(header, "No Title");
  }
  else{
    strncpy(header, medline_header, MAX_HEADER_LEN);
  }

  medline_header[0] = '\0';
  medline_title[0] = '\0';
  medline_date[0] = '\0';
  medline_author[0] = '\0';
}




/* ========================================
 * ===    Refer  Customizations      ====
 * ========================================
 */


/* 
	Francois Schiettecatte
	with help from:
	Tom Emmel
	Karen Phipps
*/

char refer_header[MAX_HEADER_LEN +1];
char refer_title[MAX_HEADER_LEN + 1];
char refer_date[MAX_HEADER_LEN + 1];
char refer_author[MAX_HEADER_LEN + 1];

static boolean refer_start = true;


boolean refer_separator_function(line)
char *line;
{
  if (refer_start == true) {
    refer_start = false;
    return true;
  }
  if (strlen(line) < 2)
    refer_start = true;
  return false;
}


void refer_header_function(line)
char *line;
{
  if((strlen(line) > strlen("%T ")) && 
     (substrcmp(line, "%T "))){    
    strncpy(refer_title, line + strlen("%T "), MAX_HEADER_LEN);
  }
  else if((strlen(line) > strlen("%B ")) && 
     (substrcmp(line, "%B ")) && (strlen(refer_title) == 0)){    
    strncpy(refer_title, line + strlen("%B "), MAX_HEADER_LEN);
  }
  
  if((strlen(line) > strlen("%D ")) &&
     (substrcmp(line, "%D "))){
    strncpy(refer_date, line + strlen("%D "), MAX_DATE_LEN);
  }

  if((strlen(line) > strlen("%A ")) &&
     (substrcmp(line, "%A ")) && (strlen(refer_author) == 0)){
    strncpy(refer_author, line + strlen("%A "), MAX_AUTHOR_LEN);
    strncat(refer_author, " ", MAX_AUTHOR_LEN);
  }
  else if((strlen(line) > strlen("%E ")) &&
     (substrcmp(line, "%E ")) && (strlen(refer_author) == 0)){
    strncpy(refer_author, line + strlen("%E "), MAX_AUTHOR_LEN);
    strncat(refer_author, " ", MAX_AUTHOR_LEN);
  }
}

void refer_finish_header_function(header)
char *header;
{
  if(strlen(refer_author) > 0 ){
    strncat(refer_header,refer_author, MAX_HEADER_LEN);
  }
  
  if(strlen(refer_date) > 0 ){
    strncat(refer_header,"(", MAX_HEADER_LEN);
    strncat(refer_header,refer_date, MAX_HEADER_LEN);
    strncat(refer_header,") ", MAX_HEADER_LEN);
  }

  if(strlen(refer_title) > 0 ){
    strncat(refer_header,refer_title, MAX_HEADER_LEN);
  }
  
  if(strlen(refer_header) == 0){
    strncpy(header, "No Title", MAX_HEADER_LEN);
  }
  else{
    strncpy(header, refer_header, MAX_HEADER_LEN);
  }

  refer_header[0] = '\0';
  refer_author[0] = '\0';
  refer_date[0] = '\0';
  refer_title[0] = '\0';
}




/* ===========================================
 * ===    First Line  Customizations      ====
 * ===========================================
 */

/* this means the first line of the file is the headline.
   useful for the lyrics server */

/* paragraph files - seperated by a blank line.  Next line is the header */

char first_line_header[MAX_HEADER_LEN +1];

boolean first_line_separator_function(line)
char *line;
{
  return false;
}

void first_line_header_function(line)
char *line;
{
  if (first_line_header[0] == '\0')
    s_strncpy(first_line_header, line, MAX_HEADER_LEN);
}

void first_line_finish_header_function(header)
char *header;
{
  if (strlen(first_line_header) == 0) {
    strcpy(header, "No Title");
  }
  else {
    s_strncpy(header, first_line_header, MAX_HEADER_LEN);
  }
  first_line_header[0] = 0;
}  

/* =========================
 * ===  BIBTEX Separator ===
 * =========================
 * S.P.vandeBurgt@research.ptt.nl (Stan)
 *
 * BibTeX entries
 *
 * @......{
 * ......
 * title = header
 * .......}
 *
 */

static char bibtex_header[MAX_HEADER_LEN + 1];

boolean bibtex_separator_function(line)
char *line;
{
    char *p = line;

    while (isspace(*p)) p++; /* skip space */
    return(*p == '@');
}

void bibtex_header_function(line)
char *line;
{
    char *p = line;

    p = strstr(line, "title");
    if (p == NULL) p = strstr(line, "Title");
    if (p == NULL) p = strstr(line, "TITLE");
    if (p != NULL && (p == line || !isalpha(*(p-1))))
    {
      p += 5;

      while (isspace(*p)) p++; /* skip space */
      if (*p == '=')          /* should be an '=' now */
      {
          p++;
          /* skip bibtex char's */
          while (isspace(*p) || *p == '"' || *p == '{') p++;
          strncpy(bibtex_header, p, MAX_HEADER_LEN);
          for (p = bibtex_header; *p != '\0'; p++)
          {
              /* replace bibtex char's */
              if (*p == '\n' || *p == '"' || *p == '}' || *p == '{')
              {
                  *p = ' ';
              }
          }
      }
    }
}

void bibtex_finish_header_function(header)
char *header;
{
    if (bibtex_header[0] == '\0')
    {
      strcpy(header, "Unknown Title");
    }
    else{
      strncpy(header, bibtex_header, MAX_HEADER_LEN);
    }
    bibtex_header[0] = '\0';
}


/* =========================
 * ===  NHYP Separator ===
 * =========================
 * S.P.vandeBurgt@research.ptt.nl (Stan)
 * Nhyp entries
 *
 * ?:? header
 * ......
 * ......
 *
 */

static char nhyp_header[MAX_HEADER_LEN + 1];

boolean nhyp_separator_function(line)
char *line;
{
    return(strstr(line, "?:?") != NULL);
}

void nhyp_header_function(line)
char *line;
{
    char *p = line;

    p = strstr(line, "?:?");
    if (p != NULL)
    {
      p += 3;
      while (isspace(*p)) p++; /* skip space */
      strncpy(nhyp_header, p, MAX_HEADER_LEN);
      trim_trailing_newline(nhyp_header);
    }
}

void nhyp_finish_header_function(header)
char *header;
{
    if (nhyp_header[0] == '\0')
    {
      strcpy(header, "Unknown Title");
    }
    else{
      strncpy(header, nhyp_header, MAX_HEADER_LEN);
    }
    nhyp_header[0] = '\0';
}



/* ==========================
 * ===  Objective-C code  ===
 * ==========================
 */


#ifdef NeXT /* only do this if it is on a NeXT */

/*----------------------- FSA -------------------*/
#define fsa_max_edges 4
#define fsa_error_state (-1)


typedef struct
{
    int if_input;
    int then_goto;
}
    fsa_edge;


/* action (if non-NULL) is excuted before transfer to next state is made */
/* action takes as arg the int input that will decide the next state */
typedef struct
{
    int default_goto;
    int n_edges;
    fsa_edge edges[fsa_max_edges];
    int (*action)();
}
    fsa_vertex;


int fsa_step(input, state_p, table)
int input;
int *state_p;
fsa_vertex *table;
{
    int next_state, e;
    int (*this_action)();


    if(*state_p < 0) return(*state_p = fsa_error_state);
    this_action = table[*state_p].action;
    if(this_action) this_action(input);
    for(e=0; e<table[*state_p].n_edges; e++)
    	if(input == table[*state_p].edges[e].if_input)
	    { next_state = table[*state_p].edges[e].then_goto; break; }
    if(e >= table[*state_p].n_edges) next_state = table[*state_p].default_goto;
    if(next_state < 0) next_state = fsa_error_state;
    return(*state_p = next_state);
}


/* sends null char as last input, returns final state */
int fsa_run(s, state_p, table)
char *s;
int *state_p;
fsa_vertex *table;
{
    char *p;
    

    for(p=s; *p; p++)
    	fsa_step((int) *p, state_p, table);
    fsa_step(0, state_p, table);
    return(*state_p);
}


/*----------------------- end FSA -------------------*/


static int wobjc_brace_level = 0;
static int wobjc_paren_level = 0;
static int wobjc_strip_state = 0;
static int wobjc_context = 0;
static boolean wobjc_separator = false;
static char wobjc_class[MAX_HEADER_LEN+1];
static char *wobjc_class_end = 0;
static char wobjc_header[MAX_HEADER_LEN+1];
static char *wobjc_header_end = 0;


#define WOBJC_BLANK " \t\n\r"
#define WOBJC_WORD "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM_0123456789"


/* Flag next line as separator, when context fsa says so. */
static int wobjc_separate(input)
int input;
{
    return(wobjc_separator = true);
}


/* FSA to parse objective-C constructs. */
static fsa_vertex wobjc_context_fsa[] =
{
    { 0, 1, {{ '@', 1 }}},			/* look for objc constructs */
    { 0, 1, {{ 'i', 20 }}},
    { 3, 1, {{ ' ', 2 }}},    			/* look for @imp class */
    { 4, 1, {{ 'A', 3 }}},
    { 4, 3, {{ '+', 6 },{ '-', 8 },{ '@', 10 }}},/* in @imp */
    { 4, 3, {{ '+', 6 },{ '-', 8 },{ '@', 10 }}, wobjc_separate},
    { 6, 1, {{ '{', 7 }}},			/* look for -method: */
    { 5, 1, {{ '{', 7 }}},
    { 8, 1, {{ '{', 9 }}},			/* look for +method: */
    { 5, 1, {{ '{', 9 }}},
    { 4, 1, {{ 'e', 11 }}},			/* look for @end of @imp */
    { 4, 1, {{ 'n', 12 }}},
    { 4, 1, {{ 'd', 0 }}},
    { 14, 1, {{ ' ', 13 }}},   			/* look for @intf class */
    { 15, 1, {{ 'A', 14 }}},
    { 15, 1, {{ '@', 16 }}},			/* in @intf */
    { 15, 1, {{ 'e', 17 }}},			/* look for @end of @intf */
    { 15, 1, {{ 'n', 18 }}},
    { 15, 1, {{ 'd', 19 }}},
    { 0, 1, {{ '@', 1 }}, wobjc_separate},
    { 0, 2, {{ 'm', 21 },{ 'n', 33 }}},		/* look for @impl */
    { 0, 1, {{ 'p', 22 }}},
    { 0, 1, {{ 'l', 23 }}},
    { 0, 1, {{ 'e', 24 }}},
    { 0, 1, {{ 'm', 25 }}},
    { 0, 1, {{ 'e', 26 }}},
    { 0, 1, {{ 'n', 27 }}},
    { 0, 1, {{ 't', 28 }}},
    { 0, 1, {{ 'a', 29 }}},
    { 0, 1, {{ 't', 30 }}},
    { 0, 1, {{ 'i', 31 }}},
    { 0, 1, {{ 'o', 32 }}},
    { 0, 1, {{ 'n', 2 }}},
    { 0, 1, {{ 't', 34 }}},			/* look for @intf */
    { 0, 1, {{ 'e', 35 }}},
    { 0, 1, {{ 'r', 36 }}},
    { 0, 1, {{ 'f', 37 }}},
    { 0, 1, {{ 'a', 38 }}},
    { 0, 1, {{ 'c', 39 }}},
    { 0, 1, {{ 'e', 13 }}}
};


/* Action to be used by stripping fsa in non-commented, non-quoted state. */
/* This runs context fsa. */
static int wobjc_process_stripped_code(input)
int input;
{
    int context_input;
    

    switch(input)
    {
    	/* Increment brace/paren levels as appropriate. */
    case '{': wobjc_brace_level++; break;
    case '}': if(wobjc_brace_level > 0) wobjc_brace_level--; break;
    case '(': wobjc_paren_level++; break;
    case ')': if(wobjc_paren_level > 0) wobjc_paren_level--; break;
    case '\"': break;
    case '\'': break;
    case '/': break;
    	
    default:
	/* If in correct context and not in brace/paren/comment/quote, */
	/* then record header info.  */
    	if(wobjc_brace_level==0 && wobjc_paren_level==0)
	{
	    /* Recording class or instance method.  Ignore multiple blanks. */
	    if(wobjc_context==6 || wobjc_context==8)
	    {
		if(!wobjc_header_end || wobjc_header_end==wobjc_header)
		{
		    strcpy(wobjc_header, (wobjc_context==6 ? "+[" : "-["));
		    strcat(wobjc_header, wobjc_class);
		    strcat(wobjc_header, " ");
		    wobjc_header_end = wobjc_header+strlen(wobjc_header);
		}
		if((wobjc_header_end - wobjc_header)<(MAX_HEADER_LEN-5)
		    && !(strchr(WOBJC_BLANK, *(wobjc_header_end-1))
			&& strchr(WOBJC_BLANK, input)))
		    { *wobjc_header_end+= input; *wobjc_header_end = 0; }
	    }
	    

	    /* Recording class name for @implementation or @interface. */
	    if(strchr(WOBJC_WORD, input)
	    	&& (wobjc_context==2 || wobjc_context==3
		    || wobjc_context==13 || wobjc_context==14))
	    {
	    	if(wobjc_context==2 || wobjc_context==13 || !wobjc_class_end)
		    wobjc_class_end = wobjc_class;
	    	if(wobjc_context==13
		    || (wobjc_context==14 && !wobjc_header_end))
		    wobjc_header_end = wobjc_header;
		if((wobjc_class_end - wobjc_class_end)<(MAX_HEADER_LEN/2))
		    { *wobjc_class_end+= input; *wobjc_class_end = 0; }
		if((wobjc_context==13 || wobjc_context==14)
		    && (wobjc_header_end-wobjc_header_end)<(MAX_HEADER_LEN/2))
		    { *wobjc_header_end+= input; *wobjc_header_end = 0; }
	    }
	}
	
	/* Since not in comment/quote, run context fsa. */
	/* Input is modified like this: */
	/*	Non-zero brace level => '{'. */
	/*	Else spaces => ' '. */
	/*	Else if in correct contexts, word letters => 'A'. */
	context_input = input;
	if(wobjc_brace_level>0) context_input = '{';
	else if(strchr(WOBJC_BLANK, input)) context_input = ' ';
	else if((wobjc_context==3 || wobjc_context==14)
	    && strchr(WOBJC_WORD, input))
	    	context_input = 'A';
	fsa_step(context_input, &wobjc_context, wobjc_context_fsa);
	break;
    }
    return(true);
}


/* FSA to strip out comments and quotes. */
static fsa_vertex wobjc_strip_fsa[] =
{
    { 0, 3, {{ '/', 1 },{ '\"', 5 },{ '\'', 7 }}, wobjc_process_stripped_code},
    { 0, 2, {{ '*', 2 },{ '/', 4 }}},		/* look for comment */
    { 2, 1, {{ '*', 3 }}},			/* in /* comment */
    { 2, 2, {{ '/', 0 },{ '*', 3 }}},
    { 4, 1, {{ '\n', 0 }, { '\0', 0 }}},	/* in // comment */
    { 5, 2, {{ '\\', 6 },{ '\"', 0 }}},		/* in " quote */
    { 5, 0, },
    { 7, 2, {{ '\\', 8 },{ '\'', 0 }}},		/* in ' quote */
    { 7, 0, }
};


boolean wobjc_separator_function(line)
char *line;
{
    if(wobjc_separator) { wobjc_separator = false; return true; }
    else return false;
}


void wobjc_header_function(line)
char *line;
{
    /* Run stripping fsa, which will run context fsa. */
    fsa_run(line, &wobjc_strip_state, wobjc_strip_fsa);
    return;
}


void wobjc_finish_header_function(header)
char *header;
{
    char *p;
    

    /* Flush terminal blanks and balance opening '[' if any. */
    for(p=wobjc_header+strlen(wobjc_header);
    	p>wobjc_header && strchr(WOBJC_BLANK, *(p-1)); p--);
    if(wobjc_header[0]=='+' || wobjc_header[0]=='-') *p+= ']';
    *p = 0;
    

    /* Copy out final header. */
    strcpy(header, wobjc_header);
    wobjc_header[0] = 0;
    wobjc_header_end = wobjc_header;
    return;
}

#endif /* def NeXT */


/* ==============================
 * ===  Ziff computer select  ===
 * ==============================
 */

/* these filters index ziff computer select cd-rom files that
   have been offloaded from the CDROM.  This is for indexing
   the CACM files that have been explicitly ok'ed by ACM.
   All other uses would violate the no lan access restrictions
   of Ziff */


#define ZIFF_TITLE_MARKER_1 "Title:     "
#define ZIFF_TITLE_MARKER_2 "Company:   "
#define ZIFF_FIRST_LINE_MARKER " *****"

/* just use the title */

boolean ziff_separator_function(line)
char *line;
{
  if (strstr(line, ZIFF_FIRST_LINE_MARKER)) {
    return(true);
  }
  else{
    return(false);
  }
}

char ziff_header[MAX_HEADER_LEN + 1];

void ziff_header_function(line)
char *line;
{
  if (strstr(line, ZIFF_TITLE_MARKER_1) ||
      strstr(line, ZIFF_TITLE_MARKER_2))
    {
      strncpy(ziff_header, line + strlen(ZIFF_TITLE_MARKER_1), 
	      MAX_HEADER_LEN);
    }
}

void ziff_finish_header_function(header)
char *header;
{
  if(strlen(ziff_header) == 0){
    strcpy(header, "Unknown Title");
  }
  else{
    s_strncpy(header, ziff_header, MAX_HEADER_LEN);
  }
  ziff_header[0] = '\0';
}

/* special header function for filename only type */

void filename_finish_header_function(header)
char* header;
{
  char *p = strrchr(current_filename, '/');

  if (p != NULL) {
    p++;
  } else {
    p = current_filename;
  }

  s_strncpy(header, p, MAX_HEADER_LEN);
}


#ifdef BIBDB
/* ============================
 * ===  Bibdb Separator  ===
 * ============================
 */


/*
 * formfedd seperate entries
 * each page is one entry
 */

boolean bibdb_hit_header = 0;

boolean bibdb_separator_function(line)
char *line;
{
  if((strlen(line) < 3) && substrcmp(line,"")){
    return(true);
  }
  else{
    return(false);
  }
}

char bibdb_header[MAX_HEADER_LEN + 1];

void bibdb_header_function(line)
char *line;
{

  /*
  printf("bibdb_header_function: %s %d %d %d\n", line, bibdb_hit_header,
	 bibdb_separator_function(line), strncmp(line, "CK: ", 4)); 
	 */
  if((bibdb_hit_header<2)
     && (!bibdb_separator_function(line)))
    if (!strncmp(line, "CK: ", 4)) {
      strncpy(bibdb_header, line+4, MAX_HEADER_LEN);
      bibdb_hit_header++;
    } else if (!strncmp(line, "TI: ", 4)) {
      int i;
      for (i=0;i<21;i++) if (!bibdb_header[i]) bibdb_header[i]=' ';
      strncpy(&(bibdb_header[20]), line+4, MAX_HEADER_LEN-21);
      bibdb_hit_header++;
    }
}

void bibdb_finish_header_function(header)
char *header;

{
  bibdb_hit_header = 0; /* turn on the flag */
  if (strlen(bibdb_header) == 0) {
    strcpy(header, "No Title");
  }
  else {
    strncpy(header, bibdb_header, MAX_HEADER_LEN);
  }
  bibdb_header[0] = '\0';
}

long bgetdate(s)
     char *s;
{
  int year, month, day;
  sscanf(s,"%2d%2d%2d", &year, &month, &day);
  return (10000 * year + 100 * month + day);
}

long bibdb_date_function(line)
char *line;
{
  if((strlen(line) > strlen("ED: ")) &&
     substrcmp(line, "ED: ")){
    return(bgetdate(line+4));
  }
  else return -1;
}

/* ============================
 * ===  Formfeed Separator  ===
 * ============================
 */


/*
 * formfeed-seperate entries
 * each page is one entry
 */

boolean formfeed_hit_header = false;

boolean formfeed_separator_function(line)
char *line;
{
  if((strlen(line) < 3) && substrcmp(line,"")){
    /* printf("hit %s\n", line); */
    return(true);
  }
  else{
    return(false);
  }
}

char formfeed_header[MAX_HEADER_LEN + 1];

void formfeed_header_function(line)
char *line;
{
  if(formfeed_hit_header
     && (!formfeed_separator_function(line))
     && strlen(formfeed_header) == 0) {
    strncpy(formfeed_header, line, MAX_HEADER_LEN);
    formfeed_hit_header = false;
  }
}

void formfeed_finish_header_function(header)
char *header;

{
  formfeed_hit_header = true; /* turn on the flag */
  if (strlen(formfeed_header) == 0) {
    strcpy(header, "No Title");
  }
  else {
    strncpy(header, formfeed_header, MAX_HEADER_LEN);
  }
  formfeed_header[0] = '\0';
}



/* ==============================
 * ===  Bibinf Separator ===
 * ==============================
 */

/* each section is one entry */

int bibinf_hit_header = 0;

boolean bibinf_separator_function(line)
char *line;
{
  if((strlen(line) < 3) &&  ((*line == '\n')
     || (*line == '\0'))) {
    return(true);
  }
  else{
    return(false);
  }
}

#ifdef SIMPLE_BIBINF
char bibinf_autor[MAX_HEADER_LEN + 1];
char bibinf_title[MAX_HEADER_LEN + 1];

void bibinf_header_function(line)
char *line;
{

  if((bibinf_hit_header<2)
     && (!bibinf_separator_function(line))) {
    if (!strncmp(line, "Titel: ", 7)) {
      strncpy(bibinf_title, line+7, MAX_HEADER_LEN);
      bibinf_hit_header++;
    } else if (!strncmp(line, "Autor: ", 7)) {
      strncpy(bibinf_autor, line+7, MAX_HEADER_LEN);
      bibinf_hit_header++;
    }
  }
}
void bibinf_finish_header_function(header)
char *header;

{
  if (bibinf_hit_header == 0) {
    strcpy(header, "No Title");
  }
  else {
    int i;
    if (strlen(bibinf_autor)>0) {
      strncpy(header, bibinf_autor, 25);
    } else {
      strncpy(header, "No author given",  25);
    }
    for (i=strlen(header);i<MAX_HEADER_LEN;header[i++]=' ');
    if (strlen(bibinf_title)>0) {
      strncpy(&(header[26]), bibinf_title, MAX_HEADER_LEN-26);
    } else {
      strncpy(&(header[26]), "No title given",  MAX_HEADER_LEN-26);
    }
  bibinf_hit_header = 0; /* turn on the flag */
  }
  bibinf_autor[0] = '\0';
  bibinf_title[0] = '\0';
}

#else

char bibinf_header[MAX_HEADER_LEN + 1];
boolean titel = false;
boolean autor = false;
boolean hrsgb = false;

void bibinf_header_function(line)
char *line;
{
  char *word;
  char bibinf_header_copy[30];
  int i;

  if(bibinf_hit_header            /* begin of Autor-line */
     && (!bibinf_separator_function(line))
     && (!strncmp(line, "Autor: ", 7))){
    s_strncpy(bibinf_header_copy, line+7, 26);
    word = strtok(bibinf_header_copy," ");
    while(word){
      if(isalnum(*word)){
        trim_trailing_newline(word);
        s_strncat(bibinf_header,word,26,26);
        s_strncat(bibinf_header," ",26,26);
      }
      else if(word[0] == '|'){
        bibinf_header[strlen(bibinf_header)-1] = '\0';
        s_strncat(bibinf_header,"; ",26,26);
      } 
      word = strtok(NULL," ");
    }
    autor = true;
  }
  else if(autor && bibinf_hit_header     /* next words of Autor-line */
          && (!bibinf_separator_function(line))
          && (strchr(line, ':') == NULL)){
    s_strncpy(bibinf_header_copy, line, 26);
    word = strtok(bibinf_header_copy," ");
    while(word){
      if(isalnum(*word)){
        trim_trailing_newline(word);
        s_strncat(bibinf_header,word,26,26);
        s_strncat(bibinf_header," ",26,26);
      }
      else if(word[0] == '|'){
        bibinf_header[strlen(bibinf_header)-1] = '\0';
        s_strncat(bibinf_header,"; ",26,26);
      } 
      word = strtok(NULL," ");
    }
  }                            /* end of Autor-line */
                               /* begin of Herausgeber-line */
  else if(bibinf_hit_header
          && (!bibinf_separator_function(line))
          && (!strncmp(line, "Herausgeber: ", strlen("Herausgeber: ")))){
    s_strncpy(bibinf_header_copy, line+strlen("Herausgeber: "), 26);
    word = strtok(bibinf_header_copy," ");
    while(word){
      if(isalnum(*word)){
        trim_trailing_newline(word);
        s_strncat(bibinf_header,word,26,26);
        s_strncat(bibinf_header," ",26,26);
      }
      else if(word[0] == '|'){
        bibinf_header[strlen(bibinf_header)-1] = '\0';
        s_strncat(bibinf_header,"; ",26,26);
      }
      word = strtok(NULL," ");
    }
    hrsgb = true;
  }
  else if(hrsgb && bibinf_hit_header      /* next words of Hrsgb-line */
          && (!bibinf_separator_function(line))
          && (strchr(line, ':') == NULL)){
    s_strncpy(bibinf_header_copy, line, 26);
    word = strtok(bibinf_header_copy," ");
    while(word){
      if(isalnum(*word)){
        trim_trailing_newline(word);
        s_strncat(bibinf_header,word,26,26);
        s_strncat(bibinf_header," ",26,26);
      }
      else if(word[0] == '|'){
        bibinf_header[strlen(bibinf_header)-1] = '\0';
        s_strncat(bibinf_header,"; ",26,26);
      } 
      word = strtok(NULL," ");
    }
  }                       /* end of Hrsgb-line */

  else if(bibinf_hit_header          /* begin of Titel-line */
          && (!bibinf_separator_function(line))
          && (!strncmp(line, "Titel: ", 7))) {
    autor = false;
    hrsgb = false;
    for(i=strlen(bibinf_header); i < 25; i++)
      s_strncat(bibinf_header," ",MAX_HEADER_LEN,MAX_HEADER_LEN);
    s_strncat(bibinf_header,": ",MAX_HEADER_LEN,MAX_HEADER_LEN);
    s_strncat(bibinf_header, line+7,MAX_HEADER_LEN,MAX_HEADER_LEN);
    titel = true;
  }
  else if                           /* next words of Titel-line */
    (titel && bibinf_hit_header
     && (!bibinf_separator_function(line))
     && (strchr(line, ':') == NULL)){
      trim_trailing_newline(bibinf_header);
      for(i=strlen(bibinf_header) - 1; isspace(bibinf_header[i]); i--)
        bibinf_header[i] = '\0'; 
      s_strncat(bibinf_header," ",MAX_HEADER_LEN,MAX_HEADER_LEN);
      s_strncat(bibinf_header,line,MAX_HEADER_LEN,MAX_HEADER_LEN);
      if(strlen(bibinf_header) == MAX_HEADER_LEN -1)
        bibinf_header[MAX_HEADER_LEN-2] = '\n'; 
    }
  else if
    (titel && bibinf_hit_header
     && (!bibinf_separator_function(line))
     && (strchr(line, ':') != NULL)){
      titel = false;
      bibinf_hit_header = false; 
    }
  else if (titel){
    titel = false;
    bibinf_hit_header = false;     
  }
}

void bibinf_finish_header_function(header)
char *header;

{
  bibinf_hit_header = true; /* turn on the flag */
  if (strlen(bibinf_header) == 0) {
    strcpy(header, "No Title");
  }
  else {
    strncpy(header, bibinf_header, MAX_HEADER_LEN);
/*    s_strncpy(header, bibinf_header,60);
    if(header[strlen(header)-1] != '\n')
      strcat(header,"...\n"); */
  }
  bibinf_header[0] = '\0';
}


#endif /* SIMPLE_BIBINF */

long binfgetdate(s)
     char *s;
{
  int year, month, day;

  sscanf(s, "%4d-%2d-%2d", &year, &month, &day);
  return (10000 * (year - 1900) + 100 * month + day);
}

long bibinf_date_function(line)
char *line;
{
  if((strlen(line) > strlen("Erfasst: ")) &&
     substrcmp(line, "Erfasst: ")){
    return(binfgetdate(line+9));
  }
  else return -1;
}
  

/* ========================================
 * ===  Irlist Digest Customizations     ====
 * ========================================
 */

boolean irlist_separator_function(line)
char *line;
{
  if (mail_or_rmail_separator(line)) return(true);
  
  if((strlen(line) > strlen("*********")) &&
     substrcmp(line, "**********")){
    return(true);
  }
  else{
    return(false);
  }
}

void irlist_header_function(line)
char *line;
{
  if((strlen(line) > strlen("Re: ")) &&
     substrcmp(line, "Re: ") &&
     (strlen(mail_subject) == 0)){
    strcpy(mail_subject, "Re: ");
    s_strncat(mail_subject, line + strlen("Re: "), MAX_HEADER_LEN, MAX_HEADER_LEN);
    trim_trailing_newline(mail_subject);
  }
  else if((strlen(line) > strlen("Fr: ")) &&
     substrcmp(line, "Fr: ") &&
     (strlen(mail_from) == 0)){
    /* this should find the <foo@bar> field in the from list */
    strncpy(mail_from, line + strlen("Fr: "), MAX_HEADER_LEN);
    trim_trailing_newline(mail_from);
  }
  else mail_header_function(line);
}

long irlist_date_function(line)
char *line;
{
  static long last_date = 0;
  long this_date;
  
  if ((this_date = mail_date_function(line)) > 0)
    return(last_date = this_date);
  else
    return(last_date);
}

#endif

#ifdef ESSENCE
/*
 *  Essence Summaries
 *
 *  Darren R. Hardy (hardy@cs.colorado.edu)
 *  University of Colorado at Boulder
 *  October 8, 1992
 *
 *  File Format:
 *
 *	EssenceSummary: core filename
 *	filename
 *	file type
 *	summarized information until EOF
 */

#define ESSENCE_TAG		"EssenceSummary:"
#define ESSENCE_TAG_LENGTH	15
#define ESSENCE_BUF		4096

int essence_track_header = -2;
char essence_header[ESSENCE_BUF];	/* Complete WAIS Header */
char essence_corefilename[ESSENCE_BUF];	/* Essence CoreFilename */
char essence_filename[ESSENCE_BUF];	/* Essence Filename (may be null) */
char essence_filetype[ESSENCE_BUF];	/* Essence FileType */

boolean essence_separator_function(line)
char *line;
{
/*  
	Uncomment this to handle concatenated Essence summaries.
	This will be useful for handling record-level indexes.

	if ((strlen(line) > ESSENCE_TAG_LENGTH) && 
	    !strncmp(line, ESSENCE_TAG, ESSENCE_TAG_LENGTH)) {
		return (true);
  	} 
*/
    	return(false);
}

void essence_header_function(line)
char *line;
{
	/*
   	 *  If we found the "EssenceSummary:" tag, then read in the
         *  CoreFilename and initialize variables.
	 */
	if ((strlen(line) > ESSENCE_TAG_LENGTH) && 
	    !strncmp(line, ESSENCE_TAG, ESSENCE_TAG_LENGTH)) {
		essence_header[0] = essence_corefilename[0] = 
		essence_filename[0] = essence_filetype[0] = '\0';
    		s_strncat(essence_corefilename, 
			  line + ESSENCE_TAG_LENGTH + 1, 
			  strlen(line) - (ESSENCE_TAG_LENGTH + 2), 
			  MAX_HEADER_LEN);
		essence_track_header = 2;
  	} else if (essence_track_header > 0) {
		/*
		 *  We've read the CoreFilename, now read in the Filename
		 *  and FileType respectively.  
		 */
    		s_strncat(essence_track_header == 2 ? 
			  essence_filename : essence_filetype, 
			  line, strlen(line) - 1, MAX_HEADER_LEN);
    		essence_track_header--;
  	}
}

/*
 *  build the WAIS header using "Filename FileType CoreFilename".  If Filename
 *  is NULL, then use the last component of CoreFilename as Filename.
 */
void essence_finish_header_function(header)
char *header;
{
	char *fnp;	/* Filename pointer */

	/*
	 *  If we don't know the CoreFilename, then we don't have a Headline.
	 *  Otherwise, build the headline from the CoreFilename, Filename,
	 *  and FileType.  If the Filename is empty, then use the last
	 *  component of the CoreFilename as the Filename.  The headline is
	 *  in the following format:
	 *
	 *	Filename FileType CoreFilename
	 */
	if (strlen(essence_corefilename) == 0) 
		s_strncpy(header, "Unknown Title", MAX_HEADER_LEN);
	else {
		if (essence_filename[0] == '\n') 	/* Empty Filename */
			/* fnp is last component of CoreFilename */
			fnp = (fnp = rindex(essence_corefilename,'/')) == NULL ?
			      &essence_corefilename[0] : fnp + 1;
		else
			fnp = &essence_filename[0];	/* We have a Filename */

		/* Build the header */
		sprintf(essence_header, "%s %s %s", fnp, essence_filetype, 
			essence_corefilename);
		s_strncpy(header, essence_header, MAX_HEADER_LEN);
	}
	/* Reset the variables */
	essence_header[0] = essence_corefilename[0] = essence_filename[0] = 
	essence_filetype[0] = '\0';
	essence_track_header = -1;
}

#endif /* ESSENCE */
#ifdef STELAR

/*=================================
 *
 *      STELAR Abstracts
 *
 *=================================*/
char stelar_header[MAX_HEADER_LEN+1];

boolean stelar_separator_function(line)
     char *line;
{
  return(false);
}

void stelar_header_function(line)
     char *line;
{
  char *p;

  if(stelar_header[0]=='\0' && isspace(line[0]) && strlen(line)>3){
    p=line;
    while(isspace(*p)) ++p;
    strncpy(stelar_header,p,MAX_HEADER_LEN);
  }
}

void stelar_finish_header_function(header)
     char *header;
{
  if(strlen(stelar_header)==0){
    strcpy(header,"No Title");
  } else {
    strncpy(header,stelar_header,MAX_HEADER_LEN);
  }
  stelar_header[0]='\0';
}

/*=================================
 *
 *      AAS Meeting Abstracts
 *      (using AAS abstract LaTeX macros)
 *
 *=================================*/

#define AASAB_TITLE_MARKER "\\title{"
#define AASAB_AUTHOR_MARKER "\\author{"

char aasab_header[MAX_HEADER_LEN+1];
char aasab_author[MAX_HEADER_LEN+1];
char aasab_title[MAX_HEADER_LEN+1];

boolean aasab_separator_function(line)
     char *line;
{
  if ((strlen(line) > strlen("\\documentstyle["))
      && (substrcmp(line, "aasab"))) {
    return(true);
  } else {
    return(false);
  }
}

void aasab_header_function(line)
     char *line;
{
  char *aas_start;
  if (aas_start = strstr(line, AASAB_TITLE_MARKER)) {
    s_strncpy( aasab_title, aas_start+strlen(AASAB_TITLE_MARKER), MAX_HEADER_LEN);
  } else
    if (aas_start = strstr(line, AASAB_AUTHOR_MARKER)) {
      s_strncpy( aasab_author, aas_start+strlen(AASAB_AUTHOR_MARKER), MAX_HEADER_LEN);
    }
}

void aasab_finish_header_function(line)
     char *line;
{
  int nchars;
  char *p = strrchr(current_filename, '/');

  if (strlen(aasab_title) == 0) {
    strcpy( line, "Unknown Title");
  } else {
    nchars = 0;
    while ((nchars < 20) && (aasab_author[nchars] != '}')) {
      aasab_header[nchars] = aasab_author[nchars];
      nchars++;
    }
    aasab_header[nchars] = '\0';

    if (p != NULL) {
      p++;
    } else {
      p = current_filename;
    }

    s_strncpy( line, p, MAX_HEADER_LEN);
    s_strncat( line, " ", strlen(" "), MAX_HEADER_LEN);
    s_strncat( aasab_header, "<> RE: ", strlen("<> RE: "), MAX_HEADER_LEN);
    s_strncat( aasab_header, aasab_title, MAX_HEADER_LEN, MAX_HEADER_LEN);
    s_strncat( line, aasab_header, MAX_HEADER_LEN, MAX_HEADER_LEN);
  }
  aasab_title[0] = '\0';
}

#endif /* STELAR */

char *URL_prefix=NULL;
char *URL_trim=NULL;

#ifdef FIELDS
/* ============================
 * ===  Fields Separator  ===
 * ============================
 */


/*
 * fields-seperate entries
 * each page is one entry
 */

#define MAX_LINE_LENGTH 1000
extern ltable* Ltable;  /* table for layout */
boolean fields_hit_header = false;

boolean fields_separator_function(line)
     char *line;
{
  char tmpline[MAX_LINE_LENGTH];
  long tmplen;

  strncpy(tmpline, line, MAX_LINE_LENGTH);
  tmplen = strlen(tmpline);
  /*if(tmpline[tmplen - 1] == '\n')
    tmpline[tmplen - 1] = '\0';*/
  
  if (match(tmpline, NULL, NULL, Ltable->field_record_end_pos)) {
      /* clear_Ftable_pos;         /* close all fields (more should happen here) */
    return(true);
  }
  else{
    return(false);
  } 
}

char fields_header[MAX_HEADER_LEN + 1] = "\n";
long Lpos = 0;
long rest_len = 0;
long number_not_ended_section = 0;
long gmax_header_len = 0;

void fields_header_function(line)
     char *line;
{
  long i, tmplen;
  long begin_pos, end_pos, prev_end_pos;
  long length;
  char *start;
  char tmpline[MAX_LINE_LENGTH];
  boolean end_pos_matched = false;
  
  strncpy(tmpline, line , MAX_LINE_LENGTH);
  tmpline[MAX_LINE_LENGTH] = '\000'; /*960114::BrianBullen guarantee null terminated*/
  tmplen = strlen(tmpline);
  /*if(tmpline[tmplen - 1] == '\n')
    tmpline[tmplen - 1] = '\0';*/
  
  if (fields_header[0] == '\n') {
    bzero(fields_header,MAX_HEADER_LEN+1);
  }

  if(number_not_ended_section > 0) {
    if(match(line,&end_pos,NULL,Ltable->layout[Lpos].end_tag_pos)){
      if(end_pos > 0) {
	for(i=0; i<tmplen; i++) {
	  if(isspace(line[i]) || (line[i] == '\t'))
	    /* do nothing */;
	  else {
	    strncpy(tmpline, &line[i], MAX_LINE_LENGTH);
	    break;
	  }
	}
	if(rest_len > 0) {
	  length = Ltable->layout[Lpos].len;
	  strncat(&(fields_header[Ltable->layout[Lpos].offset]), tmpline, rest_len);
	  if(length == strlen(&(fields_header[Ltable->layout[Lpos].offset])))
	    number_not_ended_section = 0;
	  else if(length > strlen(&(fields_header[Ltable->layout[Lpos].offset])))
	    rest_len = length - strlen(&(fields_header[Ltable->layout[Lpos].offset]));
	} else number_not_ended_section = 0;
      }
      else number_not_ended_section = 0;
    }
    else {
      for(i=0; i<tmplen; i++) {
	if(isspace(line[i]) || (line[i] == '\t'))
	  /* do nothing */;
	else {
	  strncpy(tmpline, &line[i], MAX_LINE_LENGTH);
	  break;
	}
      }
      if(rest_len > 0) {
	strncat(&(fields_header[Ltable->layout[Lpos].offset]),tmpline,rest_len);
	length = Ltable->layout[Lpos].len;
	if(length == strlen(&(fields_header[Ltable->layout[Lpos].offset])))
	  number_not_ended_section = 0;
	else if(length > strlen(&(fields_header[Ltable->layout[Lpos].offset])))
	  rest_len = length - strlen(&(fields_header[Ltable->layout[Lpos].offset]));
      } else number_not_ended_section = 0;
    }
  }
  if(gmax_header_len == 0) {
    for(i=0; i < Ltable->number_of_headlines; i++) {
      if(gmax_header_len < MAX_HEADER_LEN) {
	gmax_header_len += Ltable->layout[i].len;
	if(gmax_header_len > MAX_HEADER_LEN) 
	  Ltable->layout[i].len -= (gmax_header_len - MAX_HEADER_LEN);
      } 
      else Ltable->layout[i].len = 0;
    }
  }
	
  if (!fields_separator_function(line)) {
    for(i=0; i < Ltable->number_of_headlines; i++) {
      if(match(line, &begin_pos, &end_pos, Ltable->layout[i].begin_tag_pos)) {
	prev_end_pos = end_pos;
	if(((end_pos == 0) || (!regexp_bolp(Ltable->layout[i].end_tag_pos))) &&
           (match(&line[end_pos],&end_pos,NULL,Ltable->layout[i].end_tag_pos))){
          /* 960114::BrianBullen - how can end_pos be == 0 - we just matched */
          /* UP: we may have mathed and empty string? */
	  end_pos_matched = true;
	  end_pos += prev_end_pos;
	}
	length = Ltable->layout[i].len;
        if(Ltable->layout[i].hline_begin_tag_pos > -1) {
          if(match(line,NULL,&begin_pos, Ltable->layout[i].hline_begin_tag_pos)) {
	    if(end_pos_matched)
	      tmpline[end_pos] = '\0';
            start = &tmpline[begin_pos];
	  } else {
	    if(end_pos_matched)
	      tmpline[end_pos] = '\0';
	    start = tmpline;
	  }
        }
        else {
	  if(end_pos_matched)
	    tmpline[end_pos] = '\0';
	  start = &tmpline[begin_pos];
	}
	if((strlen(start) != 0) && (Ltable->layout[i].offset < MAX_HEADER_LEN)) {
	  strncat(&(fields_header[Ltable->layout[i].offset]), start,
		  length - strlen(&(fields_header[Ltable->layout[i].offset])));
	  rest_len = length - strlen(&(fields_header[Ltable->layout[i].offset]));
	  if(rest_len > 0) 
	    if(end_pos_matched == false) {
	      ++number_not_ended_section;
	      Lpos = i;
	    } 
	}
	break;/*960114::BrianBullen err, if we started a headline we 
                dont want to try again, as allowing overlapping headlines
 		would be really complicated for all concerned */

      }
    }
  }
}


#if 0
void fields_header_function(line)
     char *line;
{
  long i;
  long begin_pos, end_pos;
  long length;
  char *start;
  if (fields_header[0] == '\n') {
    bzero(fields_header,MAX_HEADER_LEN+1);
  }
  if (!fields_separator_function(line)) {
    for(i=0; i < Ltable->number_of_headlines; i++) {
      if(match(line, NULL, &end_pos, Ltable->layout[i].begin_tag_pos)) {
        if(match(line,NULL,&begin_pos, Ltable->layout[i].hline_begin_tag_pos)){
          length = Ltable->layout[i].len;
          start = &line[end_pos];
          if(match(start,NULL,&end_pos,Ltable->layout[i].end_tag_pos)){
            if (end_pos < length) {
              length = end_pos;
            }
          } 
          strncat(&(fields_header[Ltable->layout[i].offset]), start, length);
        }
      }
    }
  }
}
#endif

void fields_finish_header_function(header)
char *header;

{
  int i;

  for (i=0;i<=MAX_HEADER_LEN;i++) {
    if (fields_header[i]) 
      if (fields_header[i] == '\n')
        header[i] = ' ';
      else
        header[i] = fields_header[i];
    else
      header[i] = ' ';
  }

  header[MAX_HEADER_LEN]='\0';

  for (i=MAX_HEADER_LEN-1;i>=0;i--) {
    if (header[i] == ' ') {
      header[i] = '\0';
    } else {
      break;
    }
  }
  bzero(fields_header,MAX_HEADER_LEN+1);
}


long
fields_getdate (s)
     char           *s;
{
  return (fields_getdate_internal(s, &(Ltable->date_desc)) % 1000000);
}

long
fields_date_function (line)
     char           *line;
{
  long            begin_pos = 0;
  char            tmpline[MAX_LINE_LENGTH];
  long            tmplen;

  strncpy (tmpline, line, MAX_LINE_LENGTH);
  tmplen = strlen (tmpline);

  if (Ltable->date_tag_pos > -1) {
    if (match (tmpline, NULL, NULL, Ltable->date_tag_pos) != NULL) {
      if (Ltable->date_begin_tag_pos > -1) {
	if (match (tmpline, NULL, &begin_pos, Ltable->date_begin_tag_pos)
	    != NULL)
	  return (fields_getdate (tmpline + (begin_pos - 1)));
      } else
	return (fields_getdate (tmpline));
    }
  }
  return (-1);
}
#endif
/* FIELDS */
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

datum db_key;
datum db_contents;

#endif

#ifdef USE_DBM
/*
Permits standard Unix System V Release 3.2 'dbm' files.

egw@aubranch.egwestate.andrews.edu, Ellen G. White Estate, July 1995
*/

void dbm_finish_header_function(header)
char *header;
{

	db_key.dptr=current_filename;
	db_key.dsize=strlen(current_filename)+1;

	db_contents=fetch(db_key);
	if (db_contents.dptr == NULL)
	  {
		strcpy(header,"No Title");
   	  }
	else
	  {
		strncpy(header,db_contents.dptr,MAX_HEADER_LEN);
	  }
}
#endif

