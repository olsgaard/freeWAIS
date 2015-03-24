/*                               -*- Mode: C -*- 
 * field_index.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Huynh Quoc T. Tung
 * Created On      : Fri Mar  4 11:15:41 1994
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Fri Jan 19 11:16:48 1996
 * Language        : C
 * Update Count    : 210
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 * HISTORY
 * 
 * $$
 * $Log: field_index.c,v $
 * Revision 2.0.1.12  1996/01/23 15:09:57  pfeifer
 * patch62:
 *
 * Revision 2.0.1.11  1995/11/14  18:08:14  pfeifer
 * patch51: regexp_bolp().
 *
 * Revision 2.0.1.10  1995/11/14  08:29:42  pfeifer
 * patch49: typo
 *
 * Revision 2.0.1.9  1995/11/13  13:01:11  pfeifer
 * patch48: Fixed false beginning-of-line matches with region end
 * patch48: expressions.
 *
 * Revision 2.0.1.8  1995/11/09  14:04:35  pfeifer
 * patch46: Keeps now trac of used file buffers.
 *
 * Revision 2.0.1.7  1995/11/03  09:53:18  pfeifer
 * patch43: Completely rewrote how_index_line. The regions now cannot
 * patch43: overlap themselfes - which was wiered. Also regions are
 * patch43: closed now when field or document changes. Now buggy format
 * patch43: descriptions where the region end expressions never matches
 * patch43: can be processed without exhausting cpu and disk.
 *
 * Revision 2.0.1.6  1995/10/27  09:54:52  pfeifer
 * patch41: Data queries should now be yyyymmdd. The next century starts
 * patch41: soon.
 *
 * Revision 2.0.1.5  1995/10/16  12:33:29  pfeifer
 * patch38: Code for removing tag in HTML/URL
 *
 * Revision 2.0.1.4  1995/10/11  15:55:26  pfeifer
 * patch36: Fixed memory read error.
 *
 * Revision 2.0.1.3  1995/10/06  17:26:19  pfeifer
 * patch24: Moved fields_getdate_internal() (former fields_getdate) from
 * patch24: ircfiles.c. Fixed clear_Ltable to honor new
 * patch24: DateDescription. Added code in how_index_line() for indexing
 * patch24: dates by calling fields_getdate_internal().
 *
 * Revision 2.0.1.2  1995/09/10  11:20:45  pfeifer
 * patch2: Changed SYSVREGEXP to MYREGEXP to really use the included
 * patch2: regexp packet if requested.
 *
 * Revision 2.0.1.1  1995/09/08  19:12:54  pfeifer
 * patch1: Removed prototype for getrnge. Sunos 4.1.4 and 5.3 do not
 * patch1: agree on it.
 *
 * Revision 2.0  1995/09/08  07:58:41  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.29  1995/06/23  12:40:36  pfeifer
 * unifdef -DBOOLEANS -DNESTED_BOOLEANS -DFIELDS | indent
 *
 * Revision 1.28  1995/06/16  15:40:41  pfeifer
 * prefremove: memory leak (80 Bytes!) fixed
 *
 * Revision 1.27  1995/04/17  13:01:16  pfeifer
 * fix in pref_remove
 *
 * Revision 1.26  1995/03/14  17:07:21  pfeifer
 * *** empty log message ***
 *
 */

#include "cdialect.h"
#include "futil.h"
#include "field_index.h"
#include "irhash.h"		/* STOP_WORD_FLAG */
#define MAX_FIELDS_NAMES 30
#define MAX_LINE_LENGTH 1000

#ifdef STEM_WORDS
boolean         index_stemming = false;	    /* used in irtfiles.c */
#endif /* STEM_WORDS */

static boolean* regexp_bol;
static boolean  not_field = false;	    /* not_field = true,
					       if line is not to add in a field */
static boolean  is_field = false;	    /* is_field = true, 
					       if line is to add in a field */

fields_struct **index_fields = NULL;

nidx_table     *Ntable = NULL;		    /* for lines which are not indexed 
					       defined in field_y.c */
ltable         *Ltable = NULL;		    /* for layout */

boolean regexp_bolp(num)
int num;
{
    return(regexp_bol[num]);
}

#ifdef MYREGEXP
/* --------------------- sgrep function --------------------------- */

#include "../regexp/regexp.h"

static int      number_of_expbuf = 0;
regexp        **expbuf_set;

/* ------------------------------------------------------------- */
/* compile regexp only one,
   regexp's compiled are in expbuf_set.
   return 0 if success,
   return -1 if error. */

long 
compile_regexp (number_of_regexp, regexp_set)
     long            number_of_regexp;
     char          **regexp_set;
{
  long            i;

  number_of_expbuf = number_of_regexp;
  regexp_bol = s_malloc(number_of_regexp * sizeof(boolean));

  if (number_of_regexp == 0)
    return (0);
  expbuf_set = (regexp **)
    malloc (number_of_regexp * sizeof (regexp *));
  for (i = 0; i < number_of_regexp; i++) {
    if (regexp_set[i][0] == '^') {
        regexp_bol[i] = true;
    } else {
        regexp_bol[i] = false;
    }
    expbuf_set[i] = regcomp (regexp_set[i]);
    if (expbuf_set[i] == NULL) {
      return (-1);
    }
  }
  return (0);
}
/* ------------------------------------------------------------- */

static char    *sgrep _AP ((char *s, regexp * expbuf, long *begin_pos, long *end_pos));

static char    *
sgrep (s, expbuf, begin_pos, end_pos)
     char           *s;
     regexp         *expbuf;
     long           *begin_pos;
     long           *end_pos;
{
  if (regexec (expbuf, s)) {	/* match */
    if (begin_pos != NULL)
      *begin_pos = expbuf->startp[0] - s;
    if (end_pos != NULL)
      *end_pos = expbuf->endp[0] - s;
    return (expbuf->startp[0]);
  }
  return (NULL);
}
/* ------------------------------------------------------------- */
/* matching line with regexp.
   regexp_pos is the position of compiled regexp.
 */
char           *
match (line, begin_pos, end_pos, regexp_pos)
     char           *line;
     long           *begin_pos;
     long           *end_pos;
     long            regexp_pos;
{
  return (sgrep (line, expbuf_set[regexp_pos],
		 begin_pos, end_pos));
}
#else /* not MYVREGEXP */
/* --------------------- sgrep function --------------------------- */

#define INIT register char *regexp=instring;
#define GETC() (*regexp++)
#define PEEKC() (*regexp)
#define UNGETC(c) (regexp--)
#define RETURN(pointer) return(0);	/* return 0 on sucess */
#define ERROR(val) return((char *) val);	/* return error code on failure */
#include <regexp.h>

char          **expbuf_set = NULL;

/* ------------------------------------------------------------- */
/* compile regexp only one,
   regexp's compiled are in expbuf_set.
   return 0 if success,
   return -1 if error. */

static int     *circf_set = NULL;
static int      number_of_expbuf = 0;

long 
compile_regexp (number_of_regexp, regexp_set)
     long            number_of_regexp;
     char          **regexp_set;
{
  long            i;
  int             status;
  char            eof;
  char           *c;
  char           *endbuf;

  number_of_expbuf = number_of_regexp;
  regexp_bol = s_malloc(number_of_regexp * sizeof(boolean));

  if (number_of_regexp == 0)
    return (0);

  if (circf_set == NULL)
    circf_set = (int *) s_malloc ((size_t) (sizeof (int) * number_of_regexp));

  expbuf_set =
    (char **) s_malloc ((size_t) (sizeof (char *) * number_of_regexp));

  for (i = 0; i < number_of_regexp; i++) {
    if (regexp_set[i][0] == '^') {
        regexp_bol[i] = true;
    } else {
        regexp_bol[i] = false;
    }
    expbuf_set[i] = (char *) s_malloc ((size_t) (sizeof (char) * 1024));

    endbuf = (expbuf_set[i]) + 1024;	/* end of buffer */
    status = (int) compile (regexp_set[i], expbuf_set[i], endbuf, eof = '\0');
    circf_set[i] = circf;
    if (status)
      return (-1);
  }
  return (0);
}
/* ------------------------------------------------------------- */

static char    *sgrep _AP ((char *s, char *expbuf, int lcircf, long *begin_pos, long *end_pos));
static char    *
sgrep (s, expbuf, lcircf, begin_pos, end_pos)
     char           *s;
     char           *expbuf;
     int             lcircf;
     long           *begin_pos;
     long           *end_pos;
{
  char           *c;

  circf = lcircf;
  if (step (s, expbuf)) {	/* match */
    if (begin_pos != NULL)
      *begin_pos = loc1 - s;
    if (end_pos != NULL)
      *end_pos = loc2 - s;
    return (loc1);
  }
  return (NULL);
}
/* ------------------------------------------------------------- */
/* matching line with regexp.
   regexp_pos is the position of compiled regexp.
 */
char           *
match (line, begin_pos, end_pos, regexp_pos)
     char           *line;
     long           *begin_pos;
     long           *end_pos;
     long            regexp_pos;
{
  return (sgrep (line, expbuf_set[regexp_pos], circf_set[regexp_pos],
		 begin_pos, end_pos));
}
#endif /* not MYREGEXP */
/* ------------------------------------------------------------- */

static void clear_expbuf _AP ((void));
static void 
clear_expbuf ()
{
  long            i;

  for (i = 0; i < number_of_expbuf; i++)
    s_free ( /* (char *) */ (expbuf_set[i]));
  s_free (expbuf_set);
}

/* ------------------------------------------------------------- */
static void clear_ntable _AP ((void));
static void 
clear_ntable ()
{
  if (Ntable != NULL) {
    s_free (Ntable->begin_tag_pos);
    s_free (Ntable->end_tag_pos);
    s_free (Ntable);
  }
}

static char    *months[] =
{"Jan", "Feb", "Mar", "Apr", "May", "Jun",
 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL};

long 
fields_getdate_internal (s, date_desc)
     char            *s;
     DateDescription *date_desc;
{
  int             year, month, day;
  char            cmonth[25];

  /* fprintf(stderr, "fields_getdate_internal(%s)\n", s); */
  if (strncmp (date_desc->order, "dmy", 3) == 0) {
    if (date_desc->month_is_string) {
      sscanf (s, date_desc->sscanf_arg, &day, cmonth, &year);
      for (month = 0; months[month] != NULL; month++)
	if (!strcasecmp (cmonth, months[month]))
	  break;
    } else
      sscanf (s, date_desc->sscanf_arg, &day, &month, &year);
  } else if (strncmp (date_desc->order, "dym", 3) == 0) {
    if (date_desc->month_is_string) {
      sscanf (s, date_desc->sscanf_arg, &day, &year, cmonth);
      for (month = 0; months[month] != NULL; month++)
	if (!strcasecmp (cmonth, months[month]))
	  break;
    } else
      sscanf (s, date_desc->sscanf_arg, &day, &year, &month);
  } else if (strncmp (date_desc->order, "mdy", 3) == 0) {
    if (date_desc->month_is_string) {
      sscanf (s, date_desc->sscanf_arg, cmonth, &day, &year);
      for (month = 0; months[month] != NULL; month++)
	if (!strcasecmp (cmonth, months[month]))
	  break;
    } else
      sscanf (s, date_desc->sscanf_arg, &month, &day, &year);
  } else if (strncmp (date_desc->order, "myd", 3) == 0) {
    if (date_desc->month_is_string) {
      scanf (s, date_desc->sscanf_arg, cmonth, &year, &day);
      for (month = 0; months[month] != NULL; month++)
	if (!strcasecmp (cmonth, months[month]))
	  break;
    } else
      sscanf (s, date_desc->sscanf_arg, &month, &year, &day);
  } else if (strncmp (date_desc->order, "ydm", 3) == 0) {
    if (date_desc->month_is_string) {
      sscanf (s, date_desc->sscanf_arg, &year, &day, cmonth);
      for (month = 0; months[month] != NULL; month++)
	if (!strcasecmp (cmonth, months[month]))
	  break;
    } else
      sscanf (s, date_desc->sscanf_arg, &year, &day, &month);
  } else {
    if (date_desc->month_is_string) {
      sscanf (s, date_desc->sscanf_arg, &year, cmonth, &day);
      for (month = 0; months[month] != NULL; month++)
	if (!strcasecmp (cmonth, months[month]))
	  break;
    } else
      sscanf (s, date_desc->sscanf_arg, &year, &month, &day);
  }

  if (year <= 99) {
      year += 1900;
  } 
  year = year % 10000;
  if (day > 0 && month <= 12 && year > 0) {
    if (date_desc->month_is_string)
      return (10000 * year + 100 * (month + 1) + day);
    else
      return (10000 * year + 100 * month + day);
  }
}

/* ------------------------------------------------------------- */

static void clear_fields_Ftable _AP ((database * db));
static void 
clear_fields_Ftable (db)
     database       *db;
{
  long            i, j, k;

  if (index_fields != NULL) {
    for (i = 0; i < db->number_of_fields; i++) {
      j = index_fields[i]->number_of_Ftable;
      while (j > 0) {
	s_free (index_fields[i]->Ftable[j - 1].index_kind);
	for (k = 0; k < NUMBER_OF_INDEXTYPES; k++) {
	  if (index_fields[i]->Ftable[j - 1].indextypes[k] == NULL)
	    break;
	  else
	    s_free (index_fields[i]->Ftable[j - 1].indextypes[k]);
	}
	s_free (index_fields[i]->Ftable[j - 1].indextypes);
	--j;
      }
      s_free (index_fields[i]->Ftable);
      if (index_fields[i]->field_name)
	s_free (index_fields[i]->field_name);
      if (index_fields[i]->field_description)
	s_free (index_fields[i]->field_description);
      s_free (index_fields[i]);
      /*
      if (index_fields[i]->date_desc.sscanf_arg)
          s_free (index_fields[i]->date_desc.sscanf_arg);
          */
    }
    s_free (index_fields);
  }
  db->index_fields = NULL;
  return;			/* return(0) (up) */
}
/* ------------------------------------------------------------- */

static void clear_Ltable _AP ((void));
static void
clear_Ltable ()
{
  if (Ltable != NULL) {
    s_free (Ltable->layout);
/*
   if (Ltable->date_desc != NULL)
   s_free (Ltable->date_desc);
 */
    if (Ltable->date_desc.sscanf_arg != NULL)
      s_free (Ltable->date_desc.sscanf_arg);
    s_free (Ltable);
  }
  return;			/* return(0); (up) */
}

/* ------------------------------------------------------------- */

void 
clear_fields (db)
     database       *db;
{
  clear_fields_Ftable (db);
  clear_Ftable_pos ();
  clear_ntable ();
  clear_Ltable ();
  clear_expbuf ();
  return;			/* return(0); (up) */
}

/* ------------------------------------------------------------- */
static index_fields_struct *index_fields_array;
long            Maxfield_names = 0;
long            Nfield_names = 0;

static void clear_index_fields_array _AP ((long number_of_elements));
static void 
clear_index_fields_array (number_of_elements)
     long            number_of_elements;
{
  long            i;

  if (index_fields_array != NULL) {
    for (i = 0; i < number_of_elements; i++) {
      s_free (index_fields_array->field_names[i]);
    }
    s_free (index_fields_array->numeric);
    /* should'nt we free this too ?(up) */
    s_free (index_fields_array->field_names);
    s_free (index_fields_array);
  }
  Maxfield_names = 0;
  Nfield_names = 0;
}
/* ------------------------------------------------------------- */

/* store the name and field_id of fields to generate */

long 
save_index_fields (field_name, number_of_elements, field_id)
     char           *field_name;
     long           *number_of_elements;
     long           *field_id;
{
  long            i = 0;
  long            len;

  if (index_fields_array != NULL) {
    for (i = 0; i < *number_of_elements; i++) {
      if (field_name == NULL) {
	if (index_fields_array->field_names[i] == NULL) {
	  *field_id = i;
	  return (0);
	}
      } else {
	if (index_fields_array->field_names[i] != NULL)
	  if (!strcmp (field_name, index_fields_array->field_names[i])) {
	    *field_id = i;
	    return (0);
	  }
      }
    }
    if (Nfield_names >= Maxfield_names) {
      Maxfield_names += MAX_FIELDS_NAMES;
      index_fields_array->field_names =
	(char **) s_realloc (index_fields_array->field_names,
			     (size_t) (sizeof (char *) * Maxfield_names));

      index_fields_array->numeric =
	(boolean *) s_realloc (index_fields_array->numeric,
			       (size_t) (sizeof (boolean) * Maxfield_names));
    }
    ++Nfield_names;
    *field_id = i;
    *number_of_elements += 1;
    index_fields_array->numeric[i] = false;
    if (field_name != NULL) {
      len = strlen (field_name);
      index_fields_array->field_names[i] =
	(char *) s_malloc ((size_t) (sizeof (char) * (len + 2)));

      s_strncpy (index_fields_array->field_names[i], field_name, len + 1);
    } else
      index_fields_array->field_names[i] = NULL;
  } else {
    index_fields_array = (index_fields_struct *)
      s_malloc ((size_t) (sizeof (index_fields_struct)));
    Maxfield_names = Nfield_names + MAX_FIELDS_NAMES;
    index_fields_array->field_names =	/* nobody frees this? (up) */
      (char **) s_malloc ((size_t) (sizeof (char *) * Maxfield_names));

    index_fields_array->numeric =
      (boolean *) s_malloc ((size_t) (sizeof (boolean) * Maxfield_names));
    if (field_name != NULL) {
      len = strlen (field_name);
      index_fields_array->field_names[i] =
	(char *) s_malloc ((size_t) (sizeof (char) * (len + 2)));

      s_strncpy (index_fields_array->field_names[i], field_name, len + 1);
    } else
      index_fields_array->field_names[i] = NULL;
    index_fields_array->numeric[i] = false;
    ++Nfield_names;
    *field_id = 0;
    *number_of_elements += 1;
  }
  return (1);
}
/* ------------------------------------------------------------- */

void 
save_numeric_index_fields (field_id)
     long            field_id;
{
  index_fields_array->numeric[field_id] = true;
}

/* ------------------------------------------------------------- */

static long    *Ftable_pos = NULL;

void 
clear_Ftable_pos ()
{
  if (Ftable_pos != NULL)
    s_free (Ftable_pos);
  Ftable_pos = NULL;
}

/* ------------------------------------------------------------- */
/* checking how a line should be indexed.
 */
static long current_field_id     = -111;
static long current_document_id  = -111;
static long current_table_length = -111;

long
how_index_line (field_id, line,
		number_of_not_ended_section,
		document_id,
		weight,
		file_position_before_line,
		line_length,
		newline_terminated,
		db,
		wordfunction1,
		wordfunction2,
		word_position, word_pairs,
		minwordlen, type)
     long            field_id;
     char           *line;
     long           *number_of_not_ended_section;
     long            document_id;
     long            weight;
     long            file_position_before_line;
     long           *line_length;
     boolean        *newline_terminated;
     database       *db;
     wordfunc       *wordfunction1;
     wordfunc       *wordfunction2;
     boolean         word_position, word_pairs;
     int             minwordlen;
     char           *type;
{
    long            begin_pos;		    /* This is  a  pointer  to the 
				             * first character that matched 
					     * the regular expression.
					     */
    long            end_pos;		    /* This is  a  pointer  to the
					     * to the character after  the  
					     * last  character that matches 
					     * the regular expression.
					     */
    long            skip_pos, index_pos, i;
    char            index_line[MAX_LINE_LENGTH+1]; /* the part of the  line which is indexed. */
    
    if ((field_id != current_field_id) ||
        (document_id != current_document_id)) {
        size_t size = (size_t) sizeof (long) 
            * db->index_fields[field_id]->number_of_Ftable;;
        /* We could check for regions not closed */
        if (size > current_table_length) {
            /* Cannot reuse old table */
            clear_Ftable_pos ();
            Ftable_pos == NULL;
        }
        if (Ftable_pos == NULL) {
            /* alloc new table */
            Ftable_pos = (long *) s_malloc (size);
            current_table_length = size;
        }
        /* initialize */
        memset(Ftable_pos, 0, size);
        *number_of_not_ended_section = 0;
        current_field_id    = field_id;
        current_document_id = document_id;
    }  

    /* for all possible regions */
    for (i = 0; i < db->index_fields[field_id]->number_of_Ftable; i++) {
        int start_expr = db->index_fields[field_id]->Ftable[i].begin_tag_pos;
        int end_expr   = db->index_fields[field_id]->Ftable[i].end_tag_pos;
        int skip_expr  = db->index_fields[field_id]->Ftable[i].index_pos;

        begin_pos = 0;        /* start indexing at the beginning of the line */
        end_pos   = 0;        /* We look for the closing regexp after end_pos */
        if (Ftable_pos[i] == 0) { 
            /* The region is not open, let us look if it starts here */
            /* match() sets reference parameters only if a match occurs */
            if (match (line, &begin_pos, &end_pos, start_expr)) {
                Ftable_pos[i] = 1; /* now this region is open */
                *number_of_not_ended_section++;
                if ((skip_expr > -1) &&
                    match (line, NULL, &end_pos, skip_expr)) {
                    /* end_pos now points to end of the skipped section? */
                    begin_pos = end_pos;
                }
            }
        }

        /* we start indexing at begin_pos and look for the end from end_pos */
        if (Ftable_pos[i]) { /* the region is open */
            skip_pos = strlen(line)+1;
            if (((end_pos == 0) || (!regexp_bol[end_expr])) &&
                match (&(line[end_pos]), &skip_pos, NULL, end_expr)) {
                /* region end here */
                Ftable_pos[i] = 0;
                *number_of_not_ended_section--;
                skip_pos += end_pos;
            } 
        
            /* Now we can index from begin_pos to skip_pos. index_line
               must have the same length than line to keep char counts
               valid */

            memset (index_line, (int) ' ', MAX_LINE_LENGTH);
            (void) strncpy(&(index_line[begin_pos]), line+begin_pos, 
                    skip_pos-begin_pos);
            index_line[strlen(line)+1] = '\0';
            /* fprintf(stderr, "#%s%%\n", index_line); */
        
            if (db->index_fields[field_id]->numeric) {  
                /* It is a numeric field. */
                if (db->index_fields[field_id]->is_date) { 
                    /* this is a date */
                    int             date;
                    date = fields_getdate_internal 
                        (index_line,
                         &(db->index_fields[field_id]->date_desc));
                    sprintf (index_line, "%6d\0", date);
                } else {                /* this is plain numeric */
                    index_pos = 0;
                    if (db->index_fields[field_id]->n_index_pos > -1) {
                        if (match (index_line, NULL, &index_pos,
                                   db->index_fields[field_id]->n_index_pos)) {
                            memset (index_line, (int) ' ', index_pos);
                        }
                    }
                    if (db->index_fields[field_id]->numeric_len > 0) {
                        index_line[db->index_fields[field_id]->
                                     numeric_len+index_pos] = '\0';
                    }
                }
            }
            if (-1 == index_line_section (field_id, i,
                                          index_line,
                                          document_id,
                                          weight,
                                          file_position_before_line,
                                          line_length,
                                          newline_terminated,
                                          db,
                                          wordfunction1,
                                          wordfunction2,
                                          word_position, word_pairs,
                                          minwordlen, type))
                waislog (WLOG_HIGH, WLOG_ERROR, "map_over_words failed");
        }
    }
    return (0);
}

/* ------------------------------------------------------------- */

/* line: it is a line that is not to index.
   line_length: number of character of line.
   newline_terminated = true if the line terminates with a newline,
   else newline_terminated = false.
 */

long 
count_words (line, line_length, newline_terminated)
     char           *line;
     long           *line_length;
     boolean        *newline_terminated;
{
  unsigned long   ch;
  long            char_count = 0;
  long            word_len = 0;
  long            word_count = 0;

  for (ch = (unsigned char) line[char_count++];
       ch != '\0'; ch = (unsigned char) line[char_count++]) {

    boolean         alnum = isalnum (ch);

    if (alnum)
      word_len++;
    else {
      if (word_len != 0)
	if (word_len >= 2)
	  word_count++;
      word_len = 0;
    }
  }
  if (newline_terminated != NULL) {
    if ((char_count >= 2) && ('\n' != line[char_count - 2]))
        *newline_terminated = false;
    else
      *newline_terminated = true;
  }
  if (line_length != NULL)
    *line_length = char_count - 1;
  return (word_count);
}
/* ------------------------------------------------------------- */

void strip_anchors(s) 
char *s;
{
    while (*s) {
        if (*s == '<') {
            while (*s && *s != '>') {
                *(s++) = ' ';
            }
            if (*s == '>') {
                *s = ' ';
            }
        }
        s++;
    }
}

/* The line should be indexed as in the field_description file */

long 
index_line_section (field_id, fpos,
		    line,
		    document_id,
		    weight,
		    file_position_before_line,
		    line_length,
		    newline_terminated,
		    db,
		    wordfunction1,
		    wordfunction2,
		    word_position, word_pairs,
		    minwordlen, type)
     long            field_id;
     long            fpos;
     char           *line;
     long            document_id;
     long            weight;
     long            file_position_before_line;
     long           *line_length;
     boolean        *newline_terminated;
     database       *db;
     wordfunc       *wordfunction1;
     wordfunc       *wordfunction2;
     boolean         word_position, word_pairs;
     int             minwordlen;
     char           *type;
{
  long            i = 0;
  long            number_of_words = 0;

  char          **indextypes;
  int            *index_kind;

  /* fprintf(stderr, "#%d %d", field_id, fpos); */
  indextypes = db->index_fields[field_id]->Ftable[fpos].indextypes;
  index_kind = db->index_fields[field_id]->Ftable[fpos].index_kind;

  if (type && (!strncmp (type, "HTML",  4) ||
               !strncmp (type, "URL",  3))) {
      strip_anchors(line);
  }
  
  while (indextypes[i]) {
    if (!strncmp (indextypes[i], TEXT, strlen (TEXT)))
      type = NULL;
    else {
      type = indextypes[i];
    }
    if (index_kind[i] == I_GLOBAL) {
      not_field = true;
      is_field = false;
    } else {
      not_field = false;
      is_field = true;
    }

    if (not_field) {
#ifdef STEM_WORDS
      if (db->stemming)
	index_stemming = true;
      else
	index_stemming = false;
#endif /* STEM_WORDS */
      number_of_words = map_over_words (line, document_id,
					weight,
					file_position_before_line,
					line_length,
					newline_terminated,
					db,
					wordfunction1,
					word_position, word_pairs,
#ifdef SOUND
					minwordlen, type);
#else /* not SOUND */
					minwordlen);
#endif /* not SOUND */
#ifdef STEM_WORDS
      index_stemming = false;
#endif /* STEM_WORDS */
    } else {			/* is_field */
#ifdef STEM_WORDS
      if (db->fields[field_id].stemming)
	index_stemming = true;	/* used in map_over_words of irtfiles.c */
      else
	index_stemming = false;
#endif /* STEM_WORDS */
      number_of_words = map_over_words (line, document_id,
					weight,
					file_position_before_line,
					line_length,
					newline_terminated,
					db,
					wordfunction2,
					word_position, word_pairs,
#ifdef SOUND
					minwordlen, type);
#else /* not SOUND */
					minwordlen);
#endif /* not SOUND */
#ifdef STEM_WORDS
      index_stemming = false;
#endif /* STEM_WORDS */
    }
    ++i;
  }
  if (is_field)
    db->fields[field_id].total_word_count += number_of_words;
  return (number_of_words);
}
/* ------------------------------------------------------------- */

static char    *rmprefix = NULL;

static int 
rmselector (file)
     Direntry_t  *file;
{
  /* printf("selector: %s\n", file->d_name); */
  return (!strncmp (file->d_name, rmprefix, strlen (rmprefix)));
}

static int 
prefremove (dir, prefix)
     char           *dir, *prefix;
{
  Direntry_t **matches;
  char            path[MAX_FILENAME_LEN];
  int             i = 0;
  int             numentr = 0;

  rmprefix = prefix;
  strcpy (path, dir);
  strncat (path, "/", MAX_FILENAME_LEN);

  if ((numentr = scandir (dir, &matches, rmselector, NULL)) > 0) {
    for (i = 0; i < numentr; i++) {
      path[strlen (dir) + 1] = '\0';
      strncat (path, matches[i]->d_name, MAX_FILENAME_LEN);
      s_free (matches[i]);
      waislog (WLOG_LOW, WLOG_INFO, "deleting \"%s\"", path);
      if (unlink (path)) {
	waislog (WLOG_HIGH, WLOG_ERROR, "unlink failed");
      }
    }
  }
  s_free (matches);
  return (i);
}

/* insert all fields to create */
boolean 
init_index_fields (adding_to_existing_index, create_new_fields,
		   field_adding_to_existing_index, number_of_fields, db)
     boolean        *adding_to_existing_index;
     boolean        *create_new_fields;
     boolean         field_adding_to_existing_index;
     long            number_of_fields;
     database       *db;
{
  long            i;
  long            fna_len;
  long            number_of_fields_not_exists = 0;

  /* char* system_call; */
  char           *field_name = NULL;

  if (number_of_fields == 0)
    return (0);

  db->fields =
    (field_db *) s_malloc ((size_t) (sizeof (field_db) * number_of_fields));
  if (db->field_index_streams == NULL) {
    db->field_index_streams = (FILE **) s_malloc ((size_t) sizeof (FILE *));
    db->number_of_fieldstreams = 1;
  }
  if (db->field_dictionary_streams == NULL)
    db->field_dictionary_streams = (FILE **) s_malloc ((size_t) sizeof (FILE *));

  if (*create_new_fields && !field_adding_to_existing_index) {
    field_name = (char *) s_malloc ((size_t) (sizeof (char) * (MAX_FILENAME_LEN + 1)));

    db->number_of_fields = 0;
  }
  for (i = 0; i < number_of_fields; i++) {
    /* only fields which not exist will be created */
    if (*create_new_fields && !field_adding_to_existing_index) {
      if (index_fields_array->field_names[i] != NULL) {
	s_strncpy (field_name, db->database_file, MAX_FILENAME_LEN);
	s_strncat (field_name, field_ext, MAX_FILENAME_LEN, MAX_FILENAME_LEN);
	s_strncat (field_name, index_fields_array->field_names[i],
		   MAX_FILENAME_LEN, MAX_FILENAME_LEN);
	s_strncat (field_name, dictionary_ext, MAX_FILENAME_LEN, MAX_FILENAME_LEN);
	if (!probe_file (field_name)) {
	  db->fields[number_of_fields_not_exists].field_id = i;
	  db->fields[number_of_fields_not_exists].index_file_number = 0;
	  db->fields[number_of_fields_not_exists].total_word_count = 0;
	  db->fields[number_of_fields_not_exists].numeric = index_fields_array->numeric[i];
	  db->fields[number_of_fields_not_exists].stemming = index_fields[i]->stemming;
	  fna_len = strlen (index_fields_array->field_names[i]);
	  db->fields[number_of_fields_not_exists].field_name =
	    (char *) s_malloc ((size_t) (sizeof (char) * (fna_len + 2)));

	  s_strncpy (db->fields[number_of_fields_not_exists].field_name,
		     index_fields_array->field_names[i], fna_len + 1);
	  ++number_of_fields_not_exists;
	}
      }
      db->number_of_fields += number_of_fields_not_exists;
    } else {			/* insert all field names in database */
      db->fields[i].field_id = i;
      db->fields[i].index_file_number = 0;
      db->fields[i].total_word_count = 0;
      if (index_fields_array->field_names[i] != NULL) {
	fna_len = strlen (index_fields_array->field_names[i]);
	db->fields[i].field_name =
	  (char *) s_malloc ((size_t) (sizeof (char) * (fna_len + 2)));

	s_strncpy (db->fields[i].field_name, index_fields_array->field_names[i],
		   fna_len + 1);
      } else
	db->fields[i].field_name = NULL;
      db->fields[i].numeric = index_fields_array->numeric[i];
      db->fields[i].stemming = index_fields[i]->stemming;
    }
  }
  clear_index_fields_array (number_of_fields);

  /* delete all fields exist */

  if (!*adding_to_existing_index) {
    if (!*create_new_fields) {
      char            dir[MAX_FILENAME_LEN];
      char           *prefix;
      int             files_deleted;

      strcpy (dir, db->database_file);
      strncat (dir, field_ext, MAX_FILENAME_LEN);
      for (prefix = dir + strlen (dir); prefix >= dir 
           && *prefix != '/'; prefix--);
      if (*prefix == '/') {	/* path given */
	*(prefix++) = '\0';
	files_deleted = prefremove (dir, prefix);
      } else {			/* database in current dir */
	files_deleted = prefremove ("./", dir);
      }
      /*
         system_call = (char*)s_malloc((size_t)(sizeof(char) * (1000 + 3)));
         strncpy(system_call, "rm ", MAX_FILENAME_LEN + 3);
         s_strncat(system_call, db->database_file,
         MAX_FILENAME_LEN, MAX_FILENAME_LEN);
         s_strncat(system_call, field_ext, MAX_FILENAME_LEN, MAX_FILENAME_LEN);
         s_strncat(system_call, "*", MAX_FILENAME_LEN, MAX_FILENAME_LEN);
         s_strncat(system_call, dictionary_ext, 
         MAX_FILENAME_LEN, MAX_FILENAME_LEN);
         system(system_call);
         s_free(system_call);
       */
    }
  }
  /* insert only new fields, old fields not deleted and
   * adding new words in global dictionary
   */
  if (*create_new_fields && field_adding_to_existing_index) {
    *create_new_fields = false;
  }
  /* insert only new fields, old fields not deleted and 
   * not updates global fields.
   */
  else if (*create_new_fields && !field_adding_to_existing_index) {
    db->doc_table_allocated_entries = 1;
    s_free (field_name);
  }
  return (0);
}

/* ------------------------------------------------------------- */
/* open stream for each field.
 * return 0, success.
 * return 1, error
 */

boolean 
open_field_streams_init (initialize, field_adding_to_existing_index, field_id, db)
     boolean         initialize;
     boolean         field_adding_to_existing_index;
     long            field_id;
     database       *db;
{
  char            file[1001];

  if (db->field_index_streams == NULL) {
    db->field_index_streams = (FILE **) s_malloc ((size_t) sizeof (FILE *));
    db->number_of_fieldstreams = 1;
  }
  if (db->field_dictionary_streams == NULL)
    db->field_dictionary_streams = (FILE **) s_malloc ((size_t) sizeof (FILE *));

  /* ext_open_database for fields */
  if (initialize) {
    *(db->field_index_streams) = NULL;
    *(db->field_index_streams) =
      s_fopen (field_index_filename (file, db->fields[field_id].field_name, db),
	       "w+b");
    if (*(db->field_index_streams) == NULL) {
      waislog (WLOG_HIGH, WLOG_ERROR,
	       "2can't open the inverted index file of field %s\n",
	       db->fields[field_id].field_name);
      disposeDatabase (db);
      return (1);
    }
  } else {
    *(db->field_dictionary_streams) =
      s_fopen (field_dictionary_filename (file,
				       db->fields[field_id].field_name, db),
	       "r+b");
    if (*(db->field_dictionary_streams) == NULL) {
      if (!field_adding_to_existing_index) {
	waislog (WLOG_HIGH, WLOG_ERROR,
		 "can't open the word hash file %s\n", file);
	disposeDatabase (db);
	return (1);
      }
    }
    *(db->field_index_streams) =
      s_fopen (field_index_filename (file, db->fields[field_id].field_name, db),
	       "r+b");
    if (*(db->field_index_streams) == NULL) {
      if (field_adding_to_existing_index) {
	*(db->field_index_streams) =
	  s_fopen (field_index_filename (file, db->fields[field_id].field_name, db),
		   "w+b");
	if (*(db->field_index_streams) == NULL) {
	  waislog (WLOG_HIGH, WLOG_ERROR,
		   "2can't open the inverted index file of field %s\n",
		   db->fields[field_id].field_name);
	  disposeDatabase (db);
	  return (1);
	}
      } else {
	waislog (WLOG_HIGH, WLOG_ERROR,
		 "2can't open the inverted index file of field %s\n",
		 db->fields[field_id].field_name);
	disposeDatabase (db);
	return (1);
      }
    }
  }
  return (0);
}

boolean 
open_global_Database (initialize, db)
     boolean         initialize;
     database       *db;
{
  char            file[MAX_FILE_NAME_LEN + 1];

  if (initialize == false) {
    db->dictionary_stream = s_fopen (dictionary_filename (file, db), "r+b");
    if (db->dictionary_stream == NULL) {
      waislog (WLOG_HIGH, WLOG_ERROR, "can't open the word hash file %s\n", file);
      disposeDatabase (db);
      return (false);
    }
  }
  return (ext_open_database (db, initialize, false));
}
