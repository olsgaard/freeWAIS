/*                               -*- Mode: C -*- 
 * 
 * field_y.y -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Ulrich Pfeifer
 * Created On      : Tue Feb 15 16:11:56 1994
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Fri Nov  3 10:11:16 1995
 * Language        : C
 * Update Count    : 454
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 * $$
 * $Log: field_y.y,v $
 * Revision 2.0.1.6  1995/11/03 09:53:43  pfeifer
 * patch43: Fixed off-by-one bug in headlines.
 *
 * Revision 2.0.1.5  1995/10/11  15:56:02  pfeifer
 * patch36: Removed bad free.
 *
 * Revision 2.0.1.5  1995/10/11  15:56:02  pfeifer
 * patch36: Removed bad free.
 *
 * Revision 2.0.1.4  1995/10/06  17:35:11  pfeifer
 * patch24: Clean up the handling of the date layout option. Added
 * patch24: rule for specifying date indexing. Indented the whole.
 *
 * Revision 2.0.1.3  1995/10/06  09:40:35  pfeifer
 * Rather complex DateDescription. May cause alignment problems.
 *
 * Revision 2.0.1.2  1995/10/06  08:42:49  pfeifer
 * Cleanup. Date spec structure.
 *
 * Revision 2.0.1.1  1995/09/08  19:13:29  pfeifer
 * patch1:  * patch1: Fixed prototype for put_tags_in_gdb.
 *
 * Revision 2.0  1995/09/08  07:58:59  pfeifer
 * Metaconfig baseline
 *
 */
%{
#include "cutil.h"
#include <stdio.h>
#include <ctype.h>
#include "futil.h"
#include "field_index.h"
#include "field.h"

#define MAX_REGEXP 100
#define YYSTYPE yystype
extern FILE* yyin;
extern int yylineno;
extern char yytext[];
#ifdef YYBISON
int yydebug;
#else /* not YYBISON */
extern int yydebug;
#endif /* not YYBISON */
extern fields_struct** index_fields;
extern nidx_table* Ntable;   /* defined in field_index.c */
extern ltable* Ltable;       /* defined in field_index.c */

static long* field_id_set = NULL;
static long gfield_id = 0;
static long gregexp_pos = 0;
static long index_pos = 0;
static long number_of_fields_inserted = 0;
static long number_of_index_fields = 0;
static long number_of_indextypes = 0;
static long number_of_headlines = 0;
static long number_of_regexp = 0;
static long offset = 0;
static int is_headline = 0;
static int regexp_not_empty = 0;
static int fieldlist_not_empty = 0;
static int options_not_empty = 0;
static int indexspecs_not_empty = 0;
static int index_kind = 0;
static int layout_specified = 0;
static boolean global_dct_exists = false;
static boolean index_kind_both = false;
/* ------------------------------------------------------------- */

static char** regexp_set = NULL;    /* contains regexp */

static void clear_regexp _AP((long number_of_elements));
static void make_ntable _AP((long *pos));
static void make_index_fields _AP((long number_of_elements));
static void put_end_tag_ftable _AP((long field_id, 
                                    long regexp_pos));
static void put_index_fields _AP((char* field_name, 
                                  char* field_description, 
                                  long field_id,
long regexp_pos,
long index_pos,
boolean field_exists));
static void put_numeric_ftable _AP((long n_index_pos,
                                    int n_len,
                                    long field_id));
static void put_headline _AP((long begin_tag_pos,
                              long end_tag_pos,
                              long hline_begin_tag_pos,
                              long n_len,
                              long n_offset,
                              long number_of_elements));
static long Maxhealine = 0;
static long Nheadline = 0;
static void put_record_end_Ltable _AP((long regexp_pos));
static void put_hdate_ltable _AP((long begin_tag_pos,
                                  DateDescription *date_desc,
                                  long date_begin_tag_pos));
static long Maxregexp = 0;

static long insert_regexp _AP((char* regexp,
                               long number_of_elements,
                               long* regexp_pos));
static long create_index_fields _AP((FILE* stream));
static void put_indextypes _AP((char* itype,
                                int i_kind,
                                long field_id,
                                long ipos));
static void put_tags_in_gdb _AP((long field_id,
				 long begin_tag_pos,
				 long end_tag_pos, long index_pos,
				 boolean *both,
				 long *number_of_index_fields));
%}
%token WORD REGEXP FIELD END LAYOUT STEMMING
%token ISOUNDEX IPHONIX NUMERIC ITEXT LOCAL GLOBAL BOTH
%token INT IGNORE RECORD_END HEADLINE
%token DATE DAY MONTH YEAR STRING DESCRIPTION

%union {
   int  ival;
   char *sval;
   char cval;
   DateDescription date;
   }
%%
docspec         : parseformat | format
                ;
parseformat     : record layout
                ;
format          : record layout speclist
                ;
record          : RECORD_END REGEXP 
{
  number_of_regexp += 
      insert_regexp($<sval>2+1, number_of_regexp, &gregexp_pos);

  s_free($<sval>2);
  put_record_end_Ltable(gregexp_pos);
}
                ;
layout          :
                | LAYOUT 
                  layoutspecs 
                  end {layout_specified = 1;}
                ;

end             :
                | END
                ;

layoutspecs     : layoutspec
                | layoutspec layoutspecs
                ;

layoutspec     : DATE REGEXP date_spec regexp
{
  long begin_tag_pos, date_begin_tag_pos;

  if($<sval>4 != NULL) {
      number_of_regexp += 
          insert_regexp($<sval>4+1, number_of_regexp, &date_begin_tag_pos);
  } else date_begin_tag_pos = -1;
  number_of_regexp += insert_regexp($<sval>2+1,number_of_regexp,&begin_tag_pos);
  /* not all compiler can copy structs :-( */
  put_hdate_ltable(begin_tag_pos, &($<date>3), date_begin_tag_pos);
  s_free($<sval>2);
  if ($<sval>4 != NULL) s_free($<sval>4);
}
                | HEADLINE REGEXP REGEXP INT regexp
{
  long hline_begin_tag_pos, begin_tag_pos, end_tag_pos;

  if($<sval>5 != NULL) {
    number_of_regexp += 
        insert_regexp($<sval>5+1,number_of_regexp,&hline_begin_tag_pos);
  } else hline_begin_tag_pos = -1;
  number_of_regexp += insert_regexp($<sval>2+1,number_of_regexp,&begin_tag_pos);
  number_of_regexp += insert_regexp($<sval>3+1, number_of_regexp, &end_tag_pos);
  put_headline(begin_tag_pos, end_tag_pos, hline_begin_tag_pos, 
	       $<ival>4, offset, number_of_headlines);
  offset += $<ival>4+1;
  ++number_of_headlines;
  s_free($<sval>2);
  s_free($<sval>3);
  if ($<sval>5 != NULL) s_free($<sval>5);
}  
                ;

speclist        : spec
                | spec speclist
                ;
spec            : FIELD REGEXP regexp
{
  /* insert begin_tag */

  number_of_regexp += insert_regexp($<sval>2+1, number_of_regexp, &gregexp_pos);
  if($<sval>3 != NULL) {
    number_of_regexp += insert_regexp($<sval>3+1, number_of_regexp, &index_pos);
  } else 
      index_pos = -1;
}
                  fieldlist
{
  if(fieldlist_not_empty == 0) {
    if(save_index_fields(NULL, &number_of_index_fields, &gfield_id)) {
      make_index_fields(number_of_index_fields);
      put_index_fields(NULL, NULL, gfield_id, gregexp_pos, index_pos, 0);
    } else 
        put_index_fields(NULL, NULL, gfield_id, gregexp_pos, index_pos, 1);
  }
}
                  options
                  indexspecs
                  END REGEXP
{
  if (!fieldlist_not_empty && !options_not_empty && !indexspecs_not_empty) {
    long            pos;

    make_ntable (&pos);
    number_of_regexp +=
      insert_regexp ($<sval>2 + 1, number_of_regexp, &gregexp_pos);
    Ntable->begin_tag_pos[pos] = gregexp_pos;
    number_of_regexp +=
      insert_regexp ($<sval>10 + 1, number_of_regexp, &gregexp_pos);
    Ntable->end_tag_pos[pos] = gregexp_pos;
    Ntable->position += 1;
  } else if (!fieldlist_not_empty 
             && !options_not_empty 
             && indexspecs_not_empty) {
    number_of_regexp +=
      insert_regexp ($<sval>10 + 1, number_of_regexp, &gregexp_pos);
    put_end_tag_ftable (gfield_id, gregexp_pos);
    global_dct_exists = true;
    number_of_indextypes = 0;
    fieldlist_not_empty = 0;
    options_not_empty = 0;
    indexspecs_not_empty = 0;
    regexp_not_empty = 0;
  } else {
    long            i, j, k;
    long            field_id;
                    /* save old value of gregexp_pos */
    long            begin_tag_pos = gregexp_pos; 

    number_of_regexp +=
      insert_regexp ($<sval>10 + 1, number_of_regexp, &gregexp_pos);
    if (number_of_fields_inserted > 1) {
      for (i = 0; i < number_of_fields_inserted; i++) {
	field_id = field_id_set[i];
	put_end_tag_ftable (field_id, gregexp_pos);
	if (index_kind_both) {
	  put_tags_in_gdb (field_id, begin_tag_pos, gregexp_pos,
		      index_pos, &index_kind_both, &number_of_index_fields);

	}
      }
      s_free (field_id_set);
    } else {
      put_end_tag_ftable (gfield_id, gregexp_pos);
      if (index_kind_both) {
	put_tags_in_gdb (gfield_id, begin_tag_pos, gregexp_pos,
		      index_pos, &index_kind_both, &number_of_index_fields);

      }
    }
    if (is_headline) {
      put_headline (-1, gregexp_pos, -1, 0, offset, number_of_headlines);
      ++number_of_headlines;
      is_headline = 0;
    }
    number_of_fields_inserted = 0;
    number_of_indextypes = 0;
    fieldlist_not_empty = 0;
    options_not_empty = 0;
    indexspecs_not_empty = 0;
    regexp_not_empty = 0;
  }
  s_free ($<sval>2);
  s_free ($<sval>10);
  if ($<sval>3 != NULL)
    s_free ($<sval>3);
}
                ;
options         : 
                | option options
{
  options_not_empty = 1;
  $<sval>$ = $<sval>2;
}
                ;
option          : NUMERIC regexp INT
{
  long            i, field_id;

  if ($<sval>2 != NULL) {
    number_of_regexp += 
        insert_regexp ($<sval>2 + 1, number_of_regexp, &index_pos);
  } else
    index_pos = -1;
  if (number_of_fields_inserted > 1) {
    for (i = 0; i < number_of_fields_inserted; i++) {
      field_id = field_id_set[i];
      put_numeric_ftable (index_pos, $<ival>3, field_id);
      save_numeric_index_fields (field_id);
    }
  } else {
    put_numeric_ftable (index_pos, $<ival>3, gfield_id);
    save_numeric_index_fields (gfield_id);
  }
  if ($<sval>2 != NULL)
    s_free ($<sval>2);	/* (up) */
}
                | DATE date_spec
{
    index_fields[gfield_id]->numeric = 1;
    index_fields[gfield_id]->numeric_len = 6; /* indicate date */
    index_fields[gfield_id]->is_date = true;
    memcpy(&(index_fields[gfield_id]->date_desc),
            &($<date>2), sizeof(index_fields[gfield_id]->date_desc));
    save_numeric_index_fields(gfield_id);
}
                | STEMMING
{
  long            i;
  long            field_id;

  if (number_of_fields_inserted > 1) {
    for (i = 0; i < number_of_fields_inserted; i++) {
      field_id = field_id_set[i];
      index_fields[field_id]->stemming = true;
    }
  } else
    index_fields[gfield_id]->stemming = true;
}
                | HEADLINE regexp INT
{
  long         fpos = index_fields[gfield_id]->number_of_Ftable;
  long         regexp_pos = index_fields[gfield_id]->Ftable[fpos].begin_tag_pos;

  if (layout_specified) {
    fprintf (stderr, "syntax error!\nlayout has been specified\n");
    return (1);
  }
  if ($<sval>2 != NULL) {
    ++($<sval>2);
    number_of_regexp += 
        insert_regexp ($<sval>2, number_of_regexp, &gregexp_pos);
  } else
    gregexp_pos = -1;
  put_headline (regexp_pos, -1, gregexp_pos, $<ival>3, offset,
		number_of_headlines);
  offset += $<ival>3 + 1;
  is_headline = 1;
}
|DATE REGEXP date_spec regexp
{
  long            regexp_pos1, regexp_pos2;

  if (layout_specified) {
    fprintf (stderr, "syntax error!\nlayout has been specified\n");
    return (1);
  }
  number_of_regexp += 
      insert_regexp ($<sval>2 + 1, number_of_regexp, &regexp_pos1);
  if ($<sval>4 != NULL) {
    number_of_regexp +=
      insert_regexp ($<sval>3 + 1, number_of_regexp, &regexp_pos2);
  } else
    regexp_pos2 = -1;
  /* not all compiler can copy structs :-( */
  put_hdate_ltable (regexp_pos1, &($<date>3), regexp_pos2);
  s_free ($<sval>2);
  if ($<sval>4 != NULL)
    s_free ($<sval>4);
}
                ;
regexp          : {$<sval>$ = NULL;}
                | REGEXP {$<sval>$ =$<sval>1; }
                ;
indexspecs      : 
                | indexspec indexspecs
{
  indexspecs_not_empty = 1;
  $<sval>$ = $<sval>2;
}
                ;
indexspec       : indextype dicts
{
  long            i;
  long            field_id;

  /* tung: it's no good style to pass the strings from yylex!
     why not use the tokens ? */
  if (number_of_fields_inserted > 1) {
    for (i = 0; i < number_of_fields_inserted; i++) {
      field_id = field_id_set[i];
      put_indextypes ($<sval>1, index_kind,
		      field_id, number_of_indextypes);
    }
  } else
    put_indextypes ($<sval>1, index_kind,
		    gfield_id, number_of_indextypes);
  ++number_of_indextypes;
  s_free ($<sval>1);
}
                ;
indextype       : ITEXT
                | ISOUNDEX
                | IPHONIX
                ;
dicts           : GLOBAL 
{
  if (fieldlist_not_empty) {
    fprintf (stderr, 
             "warning: creating fields use variable LOCAL but not GLOBAL!!!\n");
    index_kind = 1;
  } else {
    index_kind = 0;
    global_dct_exists = true;
  }
}
                | LOCAL  
{
  if (fieldlist_not_empty == 0) {
    fprintf (stderr, "warning: %s\n",
             "creating global database use variable GLOBAL but not LOCAL!!!");
    index_kind = 0;
    global_dct_exists = true;
  } else {
    index_kind = 1;
  }
}
                | BOTH   
{
  if (fieldlist_not_empty == 0) {
    fprintf (stderr, "warning: %s %s\n",
	     "creating global database use variable GLOBAL but not BOTH",
	     "or missing field name!!!");
    index_kind = 0;
    global_dct_exists = true;
  } else {
    index_kind = 2;
    global_dct_exists = true;
    index_kind_both = true;
  }
}
                ;

date_spec       : REGEXP date date date
{
    DateDescription date;
    date.sscanf_arg       = strdup($<sval>1+1); s_free($<sval>1);
    date.sscanf_arg[strlen(date.sscanf_arg)-1] = '\0';
    date.order[0]  = tolower($<cval>2);
    date.order[1]  = tolower($<cval>3);
    date.order[2]  = tolower($<cval>4);
    date.month_is_string = 
        ($<cval>2 == 'M') || ($<cval>3 == 'M') || ($<cval>4 == 'M') ;
    /* not all compiler can copy structs :-( */
    memcpy(&($<date>$), &date, sizeof(date));
}

date            : DAY               {$<cval>$ = 'd';}
                | MONTH monthspec   {$<cval>$ = $<cval>2;}
                | YEAR              {$<cval>$ = 'y';}
                ;

monthspec       : {$<cval>$ = 'm';}
                | STRING {$<cval>$ = 'M';}
                ;                 

descr           : {$<sval>$ = NULL;}
                | DESCRIPTION {$<sval>$ = $<sval>1;}
                ;

fieldlist       : 
                | WORD descr fieldlist
{
  ++number_of_fields_inserted;
  if (number_of_fields_inserted > 1) {
    if (field_id_set == NULL) {
      field_id_set = (long *) s_malloc ((size_t) (sizeof (long) * 5));

      field_id_set[0] = gfield_id;
    } else {
      if (number_of_fields_inserted >= 5) {
	field_id_set = (long *)
	  s_realloc (field_id_set,
		(size_t) (sizeof (long) * (number_of_fields_inserted + 5)));
      }
    }
  }
  if (save_index_fields ($<sval>1, &number_of_index_fields, &gfield_id)) {
    make_index_fields (number_of_index_fields);
    put_index_fields ($<sval>1, $<sval>2, gfield_id, gregexp_pos, index_pos, 0);
  } else
    put_index_fields ($<sval>1, $<sval>2, gfield_id, gregexp_pos, index_pos, 1);
  if (number_of_fields_inserted == 2)
    field_id_set[1] = gfield_id;
  else if (number_of_fields_inserted > 2)
    field_id_set[number_of_fields_inserted - 1] = gfield_id;
  fieldlist_not_empty = 1;
  $<sval>$ = $<sval>3;
  s_free ($<sval>1);
  s_free ($<sval>2);
}
                ;
%%
extern FILE* yyin;
extern int yylineno;
extern char yytext[];
extern int yydebug;


yyerror (s)
     char           *s;
{
  fprintf (stderr, "line %d: syntax error at or near string \"%s\"\n",
	   yylineno+1, yytext);
  fprintf (stderr, "%s\n", s);
}

yywrap ()
{
  return (1);
}

static void 
clear_regexp (number_of_elements)
     long            number_of_elements;
{
  long            i;

  for (i = 0; i < number_of_elements; i++)
    s_free (regexp_set[i]);
  s_free (regexp_set);
}

/* ------------------------------------------------------------- */

static void 
make_ntable (pos)
     long           *pos;
{
  if (Ntable == NULL) {
    Ntable = (nidx_table *) s_malloc ((size_t) sizeof (nidx_table));
    Ntable->position = 0;
  }
  *pos = Ntable->position;
  if (Ntable->begin_tag_pos == NULL && Ntable->end_tag_pos == NULL) {
    Ntable->begin_tag_pos = (long *) s_malloc ((size_t) (sizeof (long)));
    Ntable->end_tag_pos = (long *) s_malloc ((size_t) (sizeof (long)));
  } else {
    Ntable->begin_tag_pos =
      (long *) s_realloc (Ntable->begin_tag_pos,
			  (size_t) (sizeof (long) * (Ntable->position + 1)));

    Ntable->end_tag_pos =
      (long *) s_realloc (Ntable->end_tag_pos,
			  (size_t) (sizeof (long) * (Ntable->position + 1)));
  }
}

/* ------------------------------------------------------------- */

static void 
make_index_fields (number_of_elements)
     long            number_of_elements;
{
  long            i;

  if (index_fields == NULL) {
    index_fields = (fields_struct **) s_malloc ((size_t) sizeof (fields_struct *));
    /* (up) initialize it - when you rely on zero filled records */
    bzero (index_fields, (size_t) sizeof (fields_struct *));
  } else {
    index_fields =
      (fields_struct **) s_realloc (index_fields,
				    (size_t) (sizeof (fields_struct *) *
					      number_of_elements));
    /* (up) initialize it - when you rely on zero filled records */
    bzero (&(index_fields[number_of_elements - 1]), (size_t) sizeof (fields_struct *));
  }
  for (i = 0; i < number_of_elements; i++) {
    if (index_fields[i] == NULL) {
      index_fields[i] = (fields_struct *) s_malloc ((size_t) sizeof (fields_struct));
      /* (up) initialize it - when you rely on zero filled records */
      bzero (index_fields[i], (size_t) sizeof (fields_struct));
    }
  }
  return;			/* up: redundant */
}
/* ------------------------------------------------------------- */

static void 
put_end_tag_ftable (field_id, regexp_pos)
     long            field_id;
     long            regexp_pos;
{
  long            fpos = index_fields[field_id]->number_of_Ftable;

  index_fields[field_id]->Ftable[fpos].end_tag_pos = regexp_pos;
  index_fields[field_id]->number_of_Ftable += 1;
}

/* ------------------------------------------------------------- */

static void 
put_indextypes (itype, i_kind, field_id, ipos)
     char           *itype;
     int             i_kind;
     long            field_id;
     long            ipos;
{
  long            len = strlen (itype);
  long            fpos = index_fields[field_id]->number_of_Ftable;

  if (index_fields[field_id]->Ftable[fpos].indextypes == NULL) {
    index_fields[field_id]->Ftable[fpos].indextypes =
      (char **) s_malloc ((size_t) (sizeof (char *) * NUMBER_OF_INDEXTYPES));

    index_fields[field_id]->Ftable[fpos].index_kind =
      (int *) s_malloc ((size_t) (sizeof (int) * NUMBER_OF_INDEXTYPES));
  }
  index_fields[field_id]->Ftable[fpos].indextypes[ipos] =
    (char *) s_malloc ((size_t) (sizeof (char) * (len + 2)));

  s_strncpy (index_fields[field_id]->Ftable[fpos].indextypes[ipos],
	     itype, len + 1);
  index_fields[field_id]->Ftable[fpos].index_kind[ipos] = i_kind;
}

/* ------------------------------------------------------------- */

static void 
put_index_fields (field_name, field_description,
		  field_id, regexp_pos, index_pos, field_exists)
     char           *field_name;
     char           *field_description;
     long            field_id;
     long            regexp_pos;
     long            index_pos;
     boolean         field_exists;
{
  long            fpos, flen;

  if (index_fields[field_id]->Ftable == NULL) {
    index_fields[field_id]->Ftable =
      (ftable *) s_malloc ((size_t) sizeof (ftable));
    fpos = index_fields[field_id]->number_of_Ftable;
  } else {
    index_fields[field_id]->Ftable = (ftable *)
      s_realloc (index_fields[field_id]->Ftable,
		 (size_t) (sizeof (ftable) * (index_fields[field_id]->number_of_Ftable + 1)));
    /* Tung, please: initialize memory if you rely on zero's (up) */
    bzero (index_fields[field_id]->Ftable
	   + index_fields[field_id]->number_of_Ftable,
	   sizeof (ftable));
    fpos = index_fields[field_id]->number_of_Ftable;
  }
  if (field_exists == 0) {
    if (field_name == NULL) {
      index_fields[field_id]->field_name = NULL;
      index_fields[field_id]->n_index_pos = -1;
    } else {
      flen = strlen (field_name);
      index_fields[field_id]->field_name =	/* who frees this ? (up) */
	(char *) s_malloc ((size_t) (sizeof (char) * (flen + 2)));

      s_strncpy (index_fields[field_id]->field_name, field_name, flen + 1);
      if (field_description) {
	if (index_fields[field_id]->field_description)
	  s_free (index_fields[field_id]->field_description);
	index_fields[field_id]->field_description = strdup (field_description);
      }
      index_fields[field_id]->field_id = field_id;
      index_fields[field_id]->n_index_pos = -1;
    }
    index_fields[field_id]->numeric = 0;
  }
  index_fields[field_id]->Ftable[fpos].begin_tag_pos = regexp_pos;
  index_fields[field_id]->Ftable[fpos].index_pos = index_pos;
}
/* ------------------------------------------------------------- */

static void 
put_numeric_ftable (n_index_pos, n_len, field_id)
     long            n_index_pos;
     int             n_len;
     long            field_id;
{
  long            len;

  index_fields[field_id]->numeric = 1;
  if (n_index_pos > -1)
    index_fields[field_id]->n_index_pos = n_index_pos;
  else
    index_fields[field_id]->n_index_pos = -1;
  index_fields[field_id]->numeric_len = n_len;
}

/* ------------------------------------------------------------- */

static void 
put_tags_in_gdb (field_id, begin_tag_pos, end_tag_pos,
		 index_pos, both, number_of_index_fields)
     long            field_id;
     long            begin_tag_pos;
     long            end_tag_pos;
     long            index_pos;
     long           *number_of_index_fields;
     boolean        *both;
{
  long            j, k;
  long            indextypes_number = 0;
  long            field_id_of_gdb;

  *both = false;
  j = index_fields[field_id]->number_of_Ftable - 1;

  for (k = 0; k < NUMBER_OF_INDEXTYPES; k++) {
    if (index_fields[field_id]->Ftable[j].indextypes[k] != NULL) {
      if (index_fields[field_id]->Ftable[j].index_kind[k] == 2) {
	if (save_index_fields (NULL, number_of_index_fields, &field_id_of_gdb)) {
	  make_index_fields (*number_of_index_fields);
	  put_index_fields (NULL, NULL, field_id_of_gdb, begin_tag_pos, index_pos, 0);
	} else
	  put_index_fields (NULL, NULL, field_id_of_gdb, begin_tag_pos, index_pos, 1);
	put_indextypes (index_fields[field_id]->Ftable[j].indextypes[k],
			0, field_id_of_gdb, indextypes_number);
	++indextypes_number;
	*both = true;
      }
    } else
      break;
  }
  if (*both) {
    put_end_tag_ftable (field_id_of_gdb, end_tag_pos);
    *both = false;
  }
}

/* ------------------------------------------------------------- */

static void 
put_headline (begin_tag_pos, end_tag_pos, hline_begin_tag_pos, n_len, n_offset, number_of_elements)
     long            begin_tag_pos;
     long            end_tag_pos;
     long            hline_begin_tag_pos;
     long            n_len;
     long            n_offset;
     long            number_of_elements;
{
  long            i;

  if (Ltable == NULL) {
    Ltable = (ltable *) s_malloc ((size_t) sizeof (ltable));
    Ltable->date_begin_tag_pos = -1;
    Ltable->date_tag_pos = -1;
  }
  if (Ltable->layout == NULL) {
    Maxhealine = Nheadline + 5;
    Ltable->layout =
      (layout_struct *) s_malloc ((size_t) (sizeof (layout_struct) * Maxhealine));
    for (i = 0; i < Maxhealine; i++)
      Ltable->layout[i].hline_begin_tag_pos = -1;
  } else {
    if (end_tag_pos > -1 && Nheadline == Maxhealine) {
      Maxhealine = Nheadline + 5;
      Ltable->layout = (layout_struct *)
	s_realloc (Ltable->layout, (sizeof (layout_struct) * Maxhealine));
      for (i = Nheadline; i < Maxhealine; i++)
	Ltable->layout[i].hline_begin_tag_pos = -1;
    }
  }
  if (end_tag_pos > -1 && begin_tag_pos > -1 && hline_begin_tag_pos > -1) {
    Ltable->layout[number_of_elements].begin_tag_pos = begin_tag_pos;
    Ltable->layout[number_of_elements].end_tag_pos = end_tag_pos;
    Ltable->layout[number_of_elements].hline_begin_tag_pos = hline_begin_tag_pos;
    Ltable->layout[number_of_elements].len = n_len;
    Ltable->layout[number_of_elements].offset = (n_offset == 0) ? 0 : n_offset;
    Ltable->number_of_headlines += 1;
    ++Nheadline;
  } else {
    if (end_tag_pos > -1) {
      Ltable->layout[number_of_elements].end_tag_pos = end_tag_pos;
      Ltable->number_of_headlines += 1;
      ++Nheadline;
    } else {
      if (hline_begin_tag_pos > -1) {
	Ltable->layout[number_of_elements].hline_begin_tag_pos = hline_begin_tag_pos;
      } else
	Ltable->layout[number_of_elements].hline_begin_tag_pos = -1;
      Ltable->layout[number_of_elements].begin_tag_pos = begin_tag_pos;
      Ltable->layout[number_of_elements].len = n_len;
      Ltable->layout[number_of_elements].offset = 
          (n_offset == 0) ? 0 : n_offset;
    }
  }
  return;			/* return(0); (up) */
}
/* ------------------------------------------------------------- */

static void 
put_record_end_Ltable (regexp_pos)
     long            regexp_pos;
{
  if (Ltable == NULL)
    Ltable = (ltable *) s_malloc ((size_t) sizeof (ltable));

  Ltable->date_tag_pos = -1;
  Ltable->date_begin_tag_pos = -1;
  Ltable->field_record_end_pos = regexp_pos;
  return;			/* return(0); (up) */
}

/* ------------------------------------------------------------- */

static void 
put_hdate_ltable (begin_tag_pos, date_desc, date_begin_tag_pos)
     long             begin_tag_pos;
     DateDescription *date_desc;
     long             date_begin_tag_pos;
{
  long            s_len;

  if (Ltable != NULL) {
    if (Ltable->date_desc.sscanf_arg != NULL)
      s_free (Ltable->date_desc.sscanf_arg);
    if (date_begin_tag_pos > -1)
      Ltable->date_begin_tag_pos = date_begin_tag_pos;
    else
      Ltable->date_begin_tag_pos = -1;
    Ltable->date_tag_pos = begin_tag_pos;
    memcpy(&(Ltable->date_desc), date_desc, sizeof(*date_desc));
  }
}

/* ------------------------------------------------------------- */

static long 
insert_regexp (regexp, number_of_elements, regexp_pos)
     char           *regexp;
     long            number_of_elements;
     long           *regexp_pos;
{
  long            i;
  long            len1 = strlen (regexp);
  long            len2;

  if (regexp_set == NULL) {
    Maxregexp = number_of_elements + MAX_REGEXP;
    regexp_set = (char **) s_malloc ((size_t) (sizeof (char *) * Maxregexp));
  } else {
    if (number_of_elements == Maxregexp) {
      Maxregexp = number_of_elements + 10;
      regexp_set = (char **) s_realloc (regexp_set,
				    (size_t) (sizeof (char *) * Maxregexp));
    }
  }
  for (i = 0; i < number_of_elements; i++) {
    len2 = strlen (regexp_set[i]);
    if ((len1 - 1) == len2)
      if (strncmp (regexp, regexp_set[i], len2) == 0) {
	*regexp_pos = i;
	return (0);
      }
  }
  *regexp_pos = number_of_elements;
  regexp_set[number_of_elements] = (char *) s_malloc ((size_t) (sizeof (char) * len1));

  s_strncpy (regexp_set[number_of_elements], regexp, len1);
  return (1);
}

/* ------------------------------------------------------------- */
static long 
create_index_fields (stream)
     FILE           *stream;
{
  yydebug = 1;
  yyin = stream;
  number_of_index_fields = 0;
  if (!yyparse ()) {
    waislog (WLOG_LOW, WLOG_INFO, "Syntax ok\n");
  } else
    return (-1);
  number_of_indextypes = 0;
  return (number_of_index_fields);
}

/* insert all field specification.
   return -1 if error.
 */

#define format_ext ".fmt"

long 
init_add_fields (name, global_dictionary_exists, db)
     char           *name;
     boolean        *global_dictionary_exists;
     database       *db;
{
  FILE           *input_stream = NULL;
  char            file[256];
  long            number_of_fields = 0;

  s_strncpy (file, name, 255);
  s_strncat (file, format_ext, 255, 255);

  if (probe_file (file)) {
    input_stream = s_fopen (file, "r");
  }
  if (NULL == input_stream) {
    waislog (WLOG_HIGH, WLOG_ERROR,
	     "File_description %s does not exist", file);
    return (-1);
  }
  if (-1 == (number_of_fields = create_index_fields (input_stream)))
    return (-1);
  if (-1 == compile_regexp (number_of_regexp, regexp_set)) {
    fprintf (stderr, "compile regexp fails\n");
    clear_regexp (number_of_regexp);
    return (-1);
  }
  clear_regexp (number_of_regexp);
  s_fclose (input_stream);
  *global_dictionary_exists = global_dct_exists;
  db->number_of_fields = number_of_fields;
  db->index_fields = index_fields;
  return (number_of_fields);
}

/* ------------------------------------------------------------- */
