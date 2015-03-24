/*                               -*- Mode: C -*- 
 * Defaults.tmpl.top -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Ulrich Pfeifer
 * Created On      : Wed May 11 09:30:19 1994
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Sun Sep 10 10:47:23 1995
 * Update Count    : 24
 * Status          : Unknown, Use with caution!
 */

#define AUTOCONF
#define SECURE_SERVER
#define RELEVANCE_FEEDBACK
#define USG
#define BOOLEANS
#define PARTIALWORD
#define LITERAL
#define SOUND
#define BIBDB
#define NESTED_BOOLEANS
#define FIELDS
#define ESSENCE
#define STEM_WORDS
#define TELL_USER
#define AUTO_STOPWORDS
#define GET_QUERY_TIMING
/*
 * use the NOPAIRS HACK to avoid problems with upper case chars?
 */
#define NOPAIRS

/*
 * use improved NEW_WEIGHT ?
 */
#define NEW_WEIGHT

/*
 * fork when handling a client
 */
#define QUERY_FORK

/*
 * Length of the headers
 */
#define MAX_HEADER_LEN 100

#define MAX_AUTHOR_LEN 25
#define MAX_DATE_LEN 4
#define MAX_WORD_LENGTH 20 /* change DICTIONARY_ENTRY_SIZE when this is changed */
#define MAX_TYPE_LEN 20
#define ANSI_DATE_LENGTH 6 /* YYMMDD */

#define MAX_HEADLINE_LEN 300

/*
 * these are needed for linking freeWAIS-sf with perl
 */
#define warn wwarn
#define any waisany

/* 
 * these are needed (?) to force metaconfig in reading the units.
 */

/* #define MYREGEXP /**/
/* #define MYCTYPE /**/
