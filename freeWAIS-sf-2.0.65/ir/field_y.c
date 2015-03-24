
/*  A Bison parser, made from field_y.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	WORD	258
#define	REGEXP	259
#define	FIELD	260
#define	END	261
#define	LAYOUT	262
#define	STEMMING	263
#define	ISOUNDEX	264
#define	IPHONIX	265
#define	NUMERIC	266
#define	ITEXT	267
#define	LOCAL	268
#define	GLOBAL	269
#define	BOTH	270
#define	INT	271
#define	IGNORE	272
#define	RECORD_END	273
#define	HEADLINE	274
#define	DATE	275
#define	DAY	276
#define	MONTH	277
#define	YEAR	278
#define	STRING	279
#define	DESCRIPTION	280


#include "cutil.h"
#include <stdio.h>
#include <ctype.h>
#include "futil.h"
#include "field_index.h"
#include "field.h"

#define MAX_REGEXP 100
#define YYSTYPE fieldstype
extern FILE* fieldin;
extern int fieldlineno;
extern char fieldtext[];
#ifdef YYBISON
int fielddebug;
#else /* not YYBISON */
extern int fielddebug;
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

typedef union {
   int  ival;
   char *sval;
   char cval;
   DateDescription date;
   } YYSTYPE;

#ifndef YYLTYPE
typedef
  struct fieldltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  fieldltype;

#define YYLTYPE fieldltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		75
#define	YYFLAG		-32768
#define	YYNTBASE	26

#define YYTRANSLATE(x) ((unsigned)(x) <= 280 ? fieldtranslate[x] : 50)

static const char fieldtranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25
};

#if YYDEBUG != 0
static const short fieldprhs[] = {     0,
     0,     2,     4,     7,    11,    14,    15,    19,    20,    22,
    24,    27,    32,    38,    40,    43,    44,    45,    56,    57,
    60,    64,    67,    69,    73,    78,    79,    81,    82,    85,
    88,    90,    92,    94,    96,    98,   100,   105,   107,   110,
   112,   113,   115,   116,   118,   119
};

static const short fieldrhs[] = {    27,
     0,    28,     0,    29,    30,     0,    29,    30,    34,     0,
    18,     4,     0,     0,     7,    32,    31,     0,     0,     6,
     0,    33,     0,    33,    32,     0,    20,     4,    45,    40,
     0,    19,     4,     4,    16,    40,     0,    35,     0,    35,
    34,     0,     0,     0,     5,     4,    40,    36,    49,    37,
    38,    41,     6,     4,     0,     0,    39,    38,     0,    11,
    40,    16,     0,    20,    45,     0,     8,     0,    19,    40,
    16,     0,    20,     4,    45,    40,     0,     0,     4,     0,
     0,    42,    41,     0,    43,    44,     0,    12,     0,     9,
     0,    10,     0,    14,     0,    13,     0,    15,     0,     4,
    46,    46,    46,     0,    21,     0,    22,    47,     0,    23,
     0,     0,    24,     0,     0,    25,     0,     0,     3,    48,
    49,     0
};

#endif

#if YYDEBUG != 0
static const short fieldrline[] = { 0,
   142,   142,   144,   146,   148,   157,   158,   163,   164,   167,
   168,   171,   185,   205,   206,   208,   219,   229,   300,   301,
   307,   329,   338,   351,   371,   393,   394,   396,   397,   403,
   423,   424,   425,   427,   438,   449,   465,   479,   480,   481,
   484,   485,   488,   489,   492,   493
};

static const char * const fieldtname[] = {   "$","error","$undefined.","WORD","REGEXP",
"FIELD","END","LAYOUT","STEMMING","ISOUNDEX","IPHONIX","NUMERIC","ITEXT","LOCAL",
"GLOBAL","BOTH","INT","IGNORE","RECORD_END","HEADLINE","DATE","DAY","MONTH",
"YEAR","STRING","DESCRIPTION","docspec","parseformat","format","record","layout",
"end","layoutspecs","layoutspec","speclist","spec","@1","@2","options","option",
"regexp","indexspecs","indexspec","indextype","dicts","date_spec","date","monthspec",
"descr","fieldlist",""
};
#endif

static const short fieldr1[] = {     0,
    26,    26,    27,    28,    29,    30,    30,    31,    31,    32,
    32,    33,    33,    34,    34,    36,    37,    35,    38,    38,
    39,    39,    39,    39,    39,    40,    40,    41,    41,    42,
    43,    43,    43,    44,    44,    44,    45,    46,    46,    46,
    47,    47,    48,    48,    49,    49
};

static const short fieldr2[] = {     0,
     1,     1,     2,     3,     2,     0,     3,     0,     1,     1,
     2,     4,     5,     1,     2,     0,     0,    10,     0,     2,
     3,     2,     1,     3,     4,     0,     1,     0,     2,     2,
     1,     1,     1,     1,     1,     1,     4,     1,     2,     1,
     0,     1,     0,     1,     0,     3
};

static const short fielddefact[] = {     0,
     0,     1,     2,     6,     5,     0,     3,     0,     0,     8,
    10,     0,     4,    14,     0,     0,     9,     7,    11,    26,
    15,     0,     0,    26,    27,    16,    26,    38,    41,    40,
     0,    12,    45,    13,    42,    39,     0,    43,    17,    37,
    44,    45,    19,    46,    23,    26,    26,     0,    28,    19,
     0,     0,     0,    22,    32,    33,    31,     0,    28,     0,
    20,    21,    24,    26,     0,    29,    35,    34,    36,    30,
    25,    18,     0,     0,     0
};

static const short fielddefgoto[] = {    73,
     2,     3,     4,     7,    18,    10,    11,    13,    14,    33,
    43,    49,    50,    26,    58,    59,    60,    70,    24,    31,
    36,    42,    39
};

static const short fieldpact[] = {     3,
     8,-32768,-32768,    19,-32768,     9,    25,    27,    28,    29,
     9,    30,-32768,    25,    32,    33,-32768,-32768,-32768,    34,
-32768,    17,   -14,    34,-32768,-32768,    34,-32768,    18,-32768,
   -14,-32768,    36,-32768,-32768,-32768,   -14,    16,-32768,-32768,
-32768,    36,    -6,-32768,-32768,    34,    34,    39,    15,    -6,
    31,    35,    -3,-32768,-32768,-32768,-32768,    38,    15,     2,
-32768,-32768,-32768,    34,    41,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    46,    48,-32768
};

static const short fieldpgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,    42,-32768,    40,-32768,-32768,
-32768,    -1,-32768,   -24,    -9,-32768,-32768,-32768,   -42,   -27,
-32768,-32768,    10
};


#define	YYLAST		54


static const short fieldtable[] = {    32,
    23,    45,    34,    37,    46,    54,    28,    29,    30,    40,
    64,     5,    47,    48,    67,    68,    69,    28,    29,    30,
     1,    51,    52,    55,    56,     6,    57,     8,     9,    12,
    15,    16,    27,    20,    17,    22,    23,    25,    38,    71,
    41,    35,    53,    65,    72,    74,    62,    75,    61,    66,
    63,    44,    19,    21
};

static const short fieldcheck[] = {    24,
     4,     8,    27,    31,    11,    48,    21,    22,    23,    37,
    53,     4,    19,    20,    13,    14,    15,    21,    22,    23,
    18,    46,    47,     9,    10,     7,    12,    19,    20,     5,
     4,     4,    16,     4,     6,     4,     4,     4,     3,    64,
    25,    24,     4,     6,     4,     0,    16,     0,    50,    59,
    16,    42,    11,    14
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */


/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define fielderrok		(fielderrstatus = 0)
#define fieldclearin	(fieldchar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto fielderrlab1
/* Like YYERROR except do call fielderror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto fielderrlab
#define YYRECOVERING()  (!!fielderrstatus)
#define YYBACKUP(token, value) \
do								\
  if (fieldchar == YYEMPTY && fieldlen == 1)				\
    { fieldchar = (token), fieldlval = (value);			\
      fieldchar1 = YYTRANSLATE (fieldchar);				\
      YYPOPSTACK;						\
      goto fieldbackup;						\
    }								\
  else								\
    { fielderror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		fieldlex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		fieldlex(&fieldlval, &fieldlloc, YYLEX_PARAM)
#else
#define YYLEX		fieldlex(&fieldlval, &fieldlloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		fieldlex(&fieldlval, YYLEX_PARAM)
#else
#define YYLEX		fieldlex(&fieldlval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	fieldchar;			/*  the lookahead symbol		*/
YYSTYPE	fieldlval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE fieldlloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int fieldnerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int fielddebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int fieldparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __field_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__field_memcpy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__field_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into fieldparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int
fieldparse(YYPARSE_PARAM)
     YYPARSE_PARAM_DECL
{
  register int fieldstate;
  register int fieldn;
  register short *fieldssp;
  register YYSTYPE *fieldvsp;
  int fielderrstatus;	/*  number of tokens to shift before error messages enabled */
  int fieldchar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	fieldssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE fieldvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *fieldss = fieldssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *fieldvs = fieldvsa;	/*  to allow fieldoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE fieldlsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *fieldls = fieldlsa;
  YYLTYPE *fieldlsp;

#define YYPOPSTACK   (fieldvsp--, fieldssp--, fieldlsp--)
#else
#define YYPOPSTACK   (fieldvsp--, fieldssp--)
#endif

  int fieldstacksize = YYINITDEPTH;

#ifdef YYPURE
  int fieldchar;
  YYSTYPE fieldlval;
  int fieldnerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE fieldlloc;
#endif
#endif

  YYSTYPE fieldval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int fieldlen;

#if YYDEBUG != 0
  if (fielddebug)
    fprintf(stderr, "Starting parse\n");
#endif

  fieldstate = 0;
  fielderrstatus = 0;
  fieldnerrs = 0;
  fieldchar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  fieldssp = fieldss - 1;
  fieldvsp = fieldvs;
#ifdef YYLSP_NEEDED
  fieldlsp = fieldls;
#endif

/* Push a new state, which is found in  fieldstate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
fieldnewstate:

  *++fieldssp = fieldstate;

  if (fieldssp >= fieldss + fieldstacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *fieldvs1 = fieldvs;
      short *fieldss1 = fieldss;
#ifdef YYLSP_NEEDED
      YYLTYPE *fieldls1 = fieldls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = fieldssp - fieldss + 1;

#ifdef fieldoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if fieldoverflow is a macro.  */
      fieldoverflow("parser stack overflow",
		 &fieldss1, size * sizeof (*fieldssp),
		 &fieldvs1, size * sizeof (*fieldvsp),
		 &fieldls1, size * sizeof (*fieldlsp),
		 &fieldstacksize);
#else
      fieldoverflow("parser stack overflow",
		 &fieldss1, size * sizeof (*fieldssp),
		 &fieldvs1, size * sizeof (*fieldvsp),
		 &fieldstacksize);
#endif

      fieldss = fieldss1; fieldvs = fieldvs1;
#ifdef YYLSP_NEEDED
      fieldls = fieldls1;
#endif
#else /* no fieldoverflow */
      /* Extend the stack our own way.  */
      if (fieldstacksize >= YYMAXDEPTH)
	{
	  fielderror("parser stack overflow");
	  return 2;
	}
      fieldstacksize *= 2;
      if (fieldstacksize > YYMAXDEPTH)
	fieldstacksize = YYMAXDEPTH;
      fieldss = (short *) alloca (fieldstacksize * sizeof (*fieldssp));
      __field_memcpy ((char *)fieldss1, (char *)fieldss, size * sizeof (*fieldssp));
      fieldvs = (YYSTYPE *) alloca (fieldstacksize * sizeof (*fieldvsp));
      __field_memcpy ((char *)fieldvs1, (char *)fieldvs, size * sizeof (*fieldvsp));
#ifdef YYLSP_NEEDED
      fieldls = (YYLTYPE *) alloca (fieldstacksize * sizeof (*fieldlsp));
      __field_memcpy ((char *)fieldls1, (char *)fieldls, size * sizeof (*fieldlsp));
#endif
#endif /* no fieldoverflow */

      fieldssp = fieldss + size - 1;
      fieldvsp = fieldvs + size - 1;
#ifdef YYLSP_NEEDED
      fieldlsp = fieldls + size - 1;
#endif

#if YYDEBUG != 0
      if (fielddebug)
	fprintf(stderr, "Stack size increased to %d\n", fieldstacksize);
#endif

      if (fieldssp >= fieldss + fieldstacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (fielddebug)
    fprintf(stderr, "Entering state %d\n", fieldstate);
#endif

  goto fieldbackup;
 fieldbackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* fieldresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  fieldn = fieldpact[fieldstate];
  if (fieldn == YYFLAG)
    goto fielddefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* fieldchar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (fieldchar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (fielddebug)
	fprintf(stderr, "Reading a token: ");
#endif
      fieldchar = YYLEX;
    }

  /* Convert token to internal form (in fieldchar1) for indexing tables with */

  if (fieldchar <= 0)		/* This means end of input. */
    {
      fieldchar1 = 0;
      fieldchar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (fielddebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      fieldchar1 = YYTRANSLATE(fieldchar);

#if YYDEBUG != 0
      if (fielddebug)
	{
	  fprintf (stderr, "Next token is %d (%s", fieldchar, fieldtname[fieldchar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, fieldchar, fieldlval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  fieldn += fieldchar1;
  if (fieldn < 0 || fieldn > YYLAST || fieldcheck[fieldn] != fieldchar1)
    goto fielddefault;

  fieldn = fieldtable[fieldn];

  /* fieldn is what to do for this token type in this state.
     Negative => reduce, -fieldn is rule number.
     Positive => shift, fieldn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (fieldn < 0)
    {
      if (fieldn == YYFLAG)
	goto fielderrlab;
      fieldn = -fieldn;
      goto fieldreduce;
    }
  else if (fieldn == 0)
    goto fielderrlab;

  if (fieldn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (fielddebug)
    fprintf(stderr, "Shifting token %d (%s), ", fieldchar, fieldtname[fieldchar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (fieldchar != YYEOF)
    fieldchar = YYEMPTY;

  *++fieldvsp = fieldlval;
#ifdef YYLSP_NEEDED
  *++fieldlsp = fieldlloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (fielderrstatus) fielderrstatus--;

  fieldstate = fieldn;
  goto fieldnewstate;

/* Do the default action for the current state.  */
fielddefault:

  fieldn = fielddefact[fieldstate];
  if (fieldn == 0)
    goto fielderrlab;

/* Do a reduction.  fieldn is the number of a rule to reduce with.  */
fieldreduce:
  fieldlen = fieldr2[fieldn];
  if (fieldlen > 0)
    fieldval = fieldvsp[1-fieldlen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (fielddebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       fieldn, fieldrline[fieldn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = fieldprhs[fieldn]; fieldrhs[i] > 0; i++)
	fprintf (stderr, "%s ", fieldtname[fieldrhs[i]]);
      fprintf (stderr, " -> %s\n", fieldtname[fieldr1[fieldn]]);
    }
#endif


  switch (fieldn) {

case 5:
{
  number_of_regexp += 
      insert_regexp(fieldvsp[0].sval+1, number_of_regexp, &gregexp_pos);

  s_free(fieldvsp[0].sval);
  put_record_end_Ltable(gregexp_pos);
;
    break;}
case 7:
{layout_specified = 1;;
    break;}
case 12:
{
  long begin_tag_pos, date_begin_tag_pos;

  if(fieldvsp[0].sval != NULL) {
      number_of_regexp += 
          insert_regexp(fieldvsp[0].sval+1, number_of_regexp, &date_begin_tag_pos);
  } else date_begin_tag_pos = -1;
  number_of_regexp += insert_regexp(fieldvsp[-2].sval+1,number_of_regexp,&begin_tag_pos);
  /* not all compiler can copy structs :-( */
  put_hdate_ltable(begin_tag_pos, &(fieldvsp[-1].date), date_begin_tag_pos);
  s_free(fieldvsp[-2].sval);
  if (fieldvsp[0].sval != NULL) s_free(fieldvsp[0].sval);
;
    break;}
case 13:
{
  long hline_begin_tag_pos, begin_tag_pos, end_tag_pos;

  if(fieldvsp[0].sval != NULL) {
    number_of_regexp += 
        insert_regexp(fieldvsp[0].sval+1,number_of_regexp,&hline_begin_tag_pos);
  } else hline_begin_tag_pos = -1;
  number_of_regexp += insert_regexp(fieldvsp[-3].sval+1,number_of_regexp,&begin_tag_pos);
  number_of_regexp += insert_regexp(fieldvsp[-2].sval+1, number_of_regexp, &end_tag_pos);
  put_headline(begin_tag_pos, end_tag_pos, hline_begin_tag_pos, 
	       fieldvsp[-1].ival, offset, number_of_headlines);
  offset += fieldvsp[-1].ival+1;
  ++number_of_headlines;
  s_free(fieldvsp[-3].sval);
  s_free(fieldvsp[-2].sval);
  if (fieldvsp[0].sval != NULL) s_free(fieldvsp[0].sval);
;
    break;}
case 16:
{
  /* insert begin_tag */

  number_of_regexp += insert_regexp(fieldvsp[-1].sval+1, number_of_regexp, &gregexp_pos);
  if(fieldvsp[0].sval != NULL) {
    number_of_regexp += insert_regexp(fieldvsp[0].sval+1, number_of_regexp, &index_pos);
  } else 
      index_pos = -1;
;
    break;}
case 17:
{
  if(fieldlist_not_empty == 0) {
    if(save_index_fields(NULL, &number_of_index_fields, &gfield_id)) {
      make_index_fields(number_of_index_fields);
      put_index_fields(NULL, NULL, gfield_id, gregexp_pos, index_pos, 0);
    } else 
        put_index_fields(NULL, NULL, gfield_id, gregexp_pos, index_pos, 1);
  }
;
    break;}
case 18:
{
  if (!fieldlist_not_empty && !options_not_empty && !indexspecs_not_empty) {
    long            pos;

    make_ntable (&pos);
    number_of_regexp +=
      insert_regexp (fieldvsp[-8].sval + 1, number_of_regexp, &gregexp_pos);
    Ntable->begin_tag_pos[pos] = gregexp_pos;
    number_of_regexp +=
      insert_regexp (fieldvsp[0].sval + 1, number_of_regexp, &gregexp_pos);
    Ntable->end_tag_pos[pos] = gregexp_pos;
    Ntable->position += 1;
  } else if (!fieldlist_not_empty 
             && !options_not_empty 
             && indexspecs_not_empty) {
    number_of_regexp +=
      insert_regexp (fieldvsp[0].sval + 1, number_of_regexp, &gregexp_pos);
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
      insert_regexp (fieldvsp[0].sval + 1, number_of_regexp, &gregexp_pos);
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
  s_free (fieldvsp[-8].sval);
  s_free (fieldvsp[0].sval);
  if (fieldvsp[-7].sval != NULL)
    s_free (fieldvsp[-7].sval);
;
    break;}
case 20:
{
  options_not_empty = 1;
  fieldval.sval = fieldvsp[0].sval;
;
    break;}
case 21:
{
  long            i, field_id;

  if (fieldvsp[-1].sval != NULL) {
    number_of_regexp += 
        insert_regexp (fieldvsp[-1].sval + 1, number_of_regexp, &index_pos);
  } else
    index_pos = -1;
  if (number_of_fields_inserted > 1) {
    for (i = 0; i < number_of_fields_inserted; i++) {
      field_id = field_id_set[i];
      put_numeric_ftable (index_pos, fieldvsp[0].ival, field_id);
      save_numeric_index_fields (field_id);
    }
  } else {
    put_numeric_ftable (index_pos, fieldvsp[0].ival, gfield_id);
    save_numeric_index_fields (gfield_id);
  }
  if (fieldvsp[-1].sval != NULL)
    s_free (fieldvsp[-1].sval);	/* (up) */
;
    break;}
case 22:
{
    index_fields[gfield_id]->numeric = 1;
    index_fields[gfield_id]->numeric_len = 6; /* indicate date */
    index_fields[gfield_id]->is_date = true;
    memcpy(&(index_fields[gfield_id]->date_desc),
            &(fieldvsp[0].date), sizeof(index_fields[gfield_id]->date_desc));
    save_numeric_index_fields(gfield_id);
;
    break;}
case 23:
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
;
    break;}
case 24:
{
  long         fpos = index_fields[gfield_id]->number_of_Ftable;
  long         regexp_pos = index_fields[gfield_id]->Ftable[fpos].begin_tag_pos;

  if (layout_specified) {
    fprintf (stderr, "syntax error!\nlayout has been specified\n");
    return (1);
  }
  if (fieldvsp[-1].sval != NULL) {
    ++(fieldvsp[-1].sval);
    number_of_regexp += 
        insert_regexp (fieldvsp[-1].sval, number_of_regexp, &gregexp_pos);
  } else
    gregexp_pos = -1;
  put_headline (regexp_pos, -1, gregexp_pos, fieldvsp[0].ival, offset,
		number_of_headlines);
  offset += fieldvsp[0].ival + 1;
  is_headline = 1;
;
    break;}
case 25:
{
  long            regexp_pos1, regexp_pos2;

  if (layout_specified) {
    fprintf (stderr, "syntax error!\nlayout has been specified\n");
    return (1);
  }
  number_of_regexp += 
      insert_regexp (fieldvsp[-2].sval + 1, number_of_regexp, &regexp_pos1);
  if (fieldvsp[0].sval != NULL) {
    number_of_regexp +=
      insert_regexp (fieldvsp[-1].sval + 1, number_of_regexp, &regexp_pos2);
  } else
    regexp_pos2 = -1;
  /* not all compiler can copy structs :-( */
  put_hdate_ltable (regexp_pos1, &(fieldvsp[-1].date), regexp_pos2);
  s_free (fieldvsp[-2].sval);
  if (fieldvsp[0].sval != NULL)
    s_free (fieldvsp[0].sval);
;
    break;}
case 26:
{fieldval.sval = NULL;;
    break;}
case 27:
{fieldval.sval =fieldvsp[0].sval; ;
    break;}
case 29:
{
  indexspecs_not_empty = 1;
  fieldval.sval = fieldvsp[0].sval;
;
    break;}
case 30:
{
  long            i;
  long            field_id;

  /* tung: it's no good style to pass the strings from fieldlex!
     why not use the tokens ? */
  if (number_of_fields_inserted > 1) {
    for (i = 0; i < number_of_fields_inserted; i++) {
      field_id = field_id_set[i];
      put_indextypes (fieldvsp[-1].sval, index_kind,
		      field_id, number_of_indextypes);
    }
  } else
    put_indextypes (fieldvsp[-1].sval, index_kind,
		    gfield_id, number_of_indextypes);
  ++number_of_indextypes;
  s_free (fieldvsp[-1].sval);
;
    break;}
case 34:
{
  if (fieldlist_not_empty) {
    fprintf (stderr, 
             "warning: creating fields use variable LOCAL but not GLOBAL!!!\n");
    index_kind = 1;
  } else {
    index_kind = 0;
    global_dct_exists = true;
  }
;
    break;}
case 35:
{
  if (fieldlist_not_empty == 0) {
    fprintf (stderr, "warning: %s\n",
             "creating global database use variable GLOBAL but not LOCAL!!!");
    index_kind = 0;
    global_dct_exists = true;
  } else {
    index_kind = 1;
  }
;
    break;}
case 36:
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
;
    break;}
case 37:
{
    DateDescription date;
    date.sscanf_arg       = strdup(fieldvsp[-3].sval+1); s_free(fieldvsp[-3].sval);
    date.sscanf_arg[strlen(date.sscanf_arg)-1] = '\0';
    date.order[0]  = tolower(fieldvsp[-2].cval);
    date.order[1]  = tolower(fieldvsp[-1].cval);
    date.order[2]  = tolower(fieldvsp[0].cval);
    date.month_is_string = 
        (fieldvsp[-2].cval == 'M') || (fieldvsp[-1].cval == 'M') || (fieldvsp[0].cval == 'M') ;
    /* not all compiler can copy structs :-( */
    memcpy(&(fieldval.date), &date, sizeof(date));
;
    break;}
case 38:
{fieldval.cval = 'd';;
    break;}
case 39:
{fieldval.cval = fieldvsp[0].cval;;
    break;}
case 40:
{fieldval.cval = 'y';;
    break;}
case 41:
{fieldval.cval = 'm';;
    break;}
case 42:
{fieldval.cval = 'M';;
    break;}
case 43:
{fieldval.sval = NULL;;
    break;}
case 44:
{fieldval.sval = fieldvsp[0].sval;;
    break;}
case 46:
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
  if (save_index_fields (fieldvsp[-2].sval, &number_of_index_fields, &gfield_id)) {
    make_index_fields (number_of_index_fields);
    put_index_fields (fieldvsp[-2].sval, fieldvsp[-1].sval, gfield_id, gregexp_pos, index_pos, 0);
  } else
    put_index_fields (fieldvsp[-2].sval, fieldvsp[-1].sval, gfield_id, gregexp_pos, index_pos, 1);
  if (number_of_fields_inserted == 2)
    field_id_set[1] = gfield_id;
  else if (number_of_fields_inserted > 2)
    field_id_set[number_of_fields_inserted - 1] = gfield_id;
  fieldlist_not_empty = 1;
  fieldval.sval = fieldvsp[0].sval;
  s_free (fieldvsp[-2].sval);
  s_free (fieldvsp[-1].sval);
;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */


  fieldvsp -= fieldlen;
  fieldssp -= fieldlen;
#ifdef YYLSP_NEEDED
  fieldlsp -= fieldlen;
#endif

#if YYDEBUG != 0
  if (fielddebug)
    {
      short *ssp1 = fieldss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != fieldssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++fieldvsp = fieldval;

#ifdef YYLSP_NEEDED
  fieldlsp++;
  if (fieldlen == 0)
    {
      fieldlsp->first_line = fieldlloc.first_line;
      fieldlsp->first_column = fieldlloc.first_column;
      fieldlsp->last_line = (fieldlsp-1)->last_line;
      fieldlsp->last_column = (fieldlsp-1)->last_column;
      fieldlsp->text = 0;
    }
  else
    {
      fieldlsp->last_line = (fieldlsp+fieldlen-1)->last_line;
      fieldlsp->last_column = (fieldlsp+fieldlen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  fieldn = fieldr1[fieldn];

  fieldstate = fieldpgoto[fieldn - YYNTBASE] + *fieldssp;
  if (fieldstate >= 0 && fieldstate <= YYLAST && fieldcheck[fieldstate] == *fieldssp)
    fieldstate = fieldtable[fieldstate];
  else
    fieldstate = fielddefgoto[fieldn - YYNTBASE];

  goto fieldnewstate;

fielderrlab:   /* here on detecting error */

  if (! fielderrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++fieldnerrs;

#ifdef YYERROR_VERBOSE
      fieldn = fieldpact[fieldstate];

      if (fieldn > YYFLAG && fieldn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -fieldn if nec to avoid negative indexes in fieldcheck.  */
	  for (x = (fieldn < 0 ? -fieldn : 0);
	       x < (sizeof(fieldtname) / sizeof(char *)); x++)
	    if (fieldcheck[x + fieldn] == x)
	      size += strlen(fieldtname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (fieldn < 0 ? -fieldn : 0);
		       x < (sizeof(fieldtname) / sizeof(char *)); x++)
		    if (fieldcheck[x + fieldn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, fieldtname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      fielderror(msg);
	      free(msg);
	    }
	  else
	    fielderror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	fielderror("parse error");
    }

  goto fielderrlab1;
fielderrlab1:   /* here on error raised explicitly by an action */

  if (fielderrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (fieldchar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (fielddebug)
	fprintf(stderr, "Discarding token %d (%s).\n", fieldchar, fieldtname[fieldchar1]);
#endif

      fieldchar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  fielderrstatus = 3;		/* Each real token shifted decrements this */

  goto fielderrhandle;

fielderrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  fieldn = fielddefact[fieldstate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (fieldn) goto fielddefault;
#endif

fielderrpop:   /* pop the current state because it cannot handle the error token */

  if (fieldssp == fieldss) YYABORT;
  fieldvsp--;
  fieldstate = *--fieldssp;
#ifdef YYLSP_NEEDED
  fieldlsp--;
#endif

#if YYDEBUG != 0
  if (fielddebug)
    {
      short *ssp1 = fieldss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != fieldssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

fielderrhandle:

  fieldn = fieldpact[fieldstate];
  if (fieldn == YYFLAG)
    goto fielderrdefault;

  fieldn += YYTERROR;
  if (fieldn < 0 || fieldn > YYLAST || fieldcheck[fieldn] != YYTERROR)
    goto fielderrdefault;

  fieldn = fieldtable[fieldn];
  if (fieldn < 0)
    {
      if (fieldn == YYFLAG)
	goto fielderrpop;
      fieldn = -fieldn;
      goto fieldreduce;
    }
  else if (fieldn == 0)
    goto fielderrpop;

  if (fieldn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (fielddebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++fieldvsp = fieldlval;
#ifdef YYLSP_NEEDED
  *++fieldlsp = fieldlloc;
#endif

  fieldstate = fieldn;
  goto fieldnewstate;
}

extern FILE* fieldin;
extern int fieldlineno;
extern char fieldtext[];
extern int fielddebug;


fielderror (s)
     char           *s;
{
  fprintf (stderr, "line %d: syntax error at or near string \"%s\"\n",
	   fieldlineno+1, fieldtext);
  fprintf (stderr, "%s\n", s);
}

fieldwrap ()
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
  fielddebug = 1;
  fieldin = stream;
  number_of_index_fields = 0;
  if (!fieldparse ()) {
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
