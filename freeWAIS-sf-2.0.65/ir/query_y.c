#ifndef lint
static char querysccsid[] = "@(#)yaccpar 1.8 (Berkeley) 01/20/91";
#endif
#define YYBYACC 1
#include "cutil.h"
#include <stdio.h>
#include <ctype.h>
void maybe_downcase();

#define YYSTYPE querystype
#define strcatXC(A,B)  strcatCC((A),(B)),free(A)
#define strcatCX(A,B)  strcatCC((A),(B)),free(B)
#define strcatXX(A,B)  strcatCC((A),(B)),free(A),free(B)
#define strcatXXC(A,B,C) strcatCCC((A),(B),(C)),free(A),free(B)
#define strcatXCC(A,B,C) strcatCCC((A),(B),(C)),free(A)
#define strcatCCX(A,B,C) strcatCCC((A),(B),(C)),free(C)
#define strcatXXX(A,B,C) strcatCCC((A),(B),(C)),free(A),free(B),free(C)
#define strcatCXX(A,B,C) strcatCCC((A),(B),(C)),free(B),free(C)
#define strcatXCX(A,B,C) strcatCCC((A),(B),(C)),free(A),free(C)
#define strcatCCXX(A,B,C,D) strcatCCCC((A),(B),(C),(D)),free(C),free(D)
#define strcatCXCX(A,B,C,D) strcatCCCC((A),(B),(C),(D)),free(B),free(D)
#define strcatXCXX(A,B,C,D) strcatCCCC((A),(B),(C),(D)),free(A),free(C),free(D)
#define strcatXXCX(A,B,C,D) strcatCCCC((A),(B),(C),(D)),free(A),free(B),free(D)
#define strcatXXCC(A,B,C,D) strcatCCCC((A),(B),(C),(D)),free(A),free(B)

extern int querylineno;
extern char querytext[];
#ifdef YYBISON
int querydebug;
#else
extern int querydebug;
#endif
static int queryverbose = 0;
static int queryboolean = 0;
static int numeric_field = 0;
static int global_dct_exists = 0;
char *current_field;
char *querybuf;
char *queryresultstring;
#define MAX_NUM_FIELDS 100
char *field_name_array[MAX_NUM_FIELDS];
static long fields_found_this_query = 0;

extern long number_of_operands; /* defined in field_search.c */
/* char** field_name_array = NULL; */

static char * strcatCC _AP((char *w, char *wl));
static char * strcatCCC _AP((char *w, char *wl, char *op));
static char * strcatCCCC _AP((char *w, char *wl, char *op1, char *op2));
typedef union {
   int  ival;
   char *sval;
   } YYSTYPE;
#define WORD 257
#define PHONIX 258
#define SOUNDEX 259
#define ASSIGN 260
#define FLOAT 261
#define OR 262
#define AND 263
#define NOT 264
#define PROX_ORDERED 265
#define PROX_UNORDERED 266
#define PROX_ATLEAST 267
#define YYERRCODE 256
short querylhs[] = {                                        -1,
    0,    1,    1,   12,   12,    2,    2,    3,    3,    4,
    4,    4,    4,    5,    5,   14,    5,   15,    5,   16,
    5,    6,    6,    6,    7,    7,    8,    8,    9,    9,
   10,   10,   10,   10,   11,   11,   17,   17,   13,   13,
};
short querylen[] = {                                         2,
    1,    0,    1,    1,    1,    1,    3,    1,    3,    1,
    3,    3,    2,    1,    3,    0,    6,    0,    4,    0,
    4,    2,    1,    1,    1,    1,    1,    3,    1,    3,
    1,    3,    3,    2,    1,    3,    1,    2,    1,    3,
};
short querydefred[] = {                                      0,
    0,   25,   26,    0,    0,    0,    0,    0,    8,    0,
    0,   14,    0,    0,   23,   24,   20,   13,    0,    3,
    0,    4,    5,    0,    0,    0,   38,    0,   22,    0,
    0,    0,   15,    0,    9,   11,   12,   40,    0,   37,
   19,   21,    0,    0,    0,    0,   29,    0,   35,   34,
    0,   17,    0,    0,    0,    0,   36,    0,   30,   32,
   33,
};
short querydgoto[] = {                                       6,
   21,    7,    8,    9,   10,   17,   11,   45,   46,   47,
   48,   24,   12,   30,   31,   32,   13,
};
short querysindex[] = {                                    -15,
   -6,    0,    0,   -4,  -15,    0, -262, -220,    0, -218,
 -252,    0, -250,  -53,    0,    0,    0,    0,  -35,    0,
  -15,    0,    0,  -15,   -4,   -4,    0, -246,    0,  -18,
 -231, -231,    0, -220,    0,    0,    0,    0,  -10,    0,
    0,    0,   57,  -10,  -34, -220,    0, -208,    0,    0,
  -23,    0,  -10,  -10,   57,   57,    0, -220,    0,    0,
    0,
};
short queryrindex[] = {                                      0,
    1,    0,    0,    0,    0,    0,   46,   34,    0,   23,
    0,    0,   12,   62,    0,    0,    0,    0,   51,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   40,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   51,  -27,    0,  -38,    0,    0,
   51,    0,    0,    0,    0,    0,    0,  -21,    0,    0,
    0,
};
short querygindex[] = {                                      0,
  -16,   19,   10,   15,    7,    0,    0,   21,   -2,   13,
  -39,  -37,    6,    0,    0,    0,    0,
};
#define YYTABLESIZE 321
short querytable[] = {                                      20,
   37,   31,   31,   50,   27,   33,   52,   29,   54,   28,
   18,   39,   27,   27,   38,   60,   61,   57,   28,   28,
   54,   39,   10,   19,    5,   40,    2,    3,   53,   44,
   34,   36,   37,    6,   53,    5,   41,   42,   35,    7,
   37,   37,   22,   23,   49,    1,   25,   26,   49,   49,
   58,   39,   39,   15,   14,   16,   55,   56,   49,   49,
   49,   49,   10,   10,   51,    0,   59,    0,    0,    0,
    0,    0,    0,    6,    6,    0,    0,    0,    0,    7,
    7,    0,    0,    0,    0,    2,    0,    0,    0,    0,
    2,    0,    0,    0,    0,    0,   44,    0,    0,    0,
    0,   16,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   31,   31,
   31,    0,    0,   31,   31,   31,   20,   20,   31,   27,
   27,   27,    0,    0,   27,   28,   28,   28,   20,   27,
   28,    1,    2,    3,    0,   28,   40,    2,    3,    0,
    0,    4,    1,    2,    3,    0,   43,   37,   37,   37,
   37,    0,   37,   37,   37,   37,   37,   37,   39,   39,
   39,    0,    0,   39,   39,   39,   39,   39,   39,   10,
   10,   10,    0,    0,   10,   10,   10,    0,    0,   10,
    6,    6,    6,    0,    0,    6,    7,    7,    7,    0,
    6,    7,    2,    2,    2,    0,    7,    2,    2,    2,
    0,    0,    2,   40,    2,    3,    0,    2,   18,   18,
   18,
};
short querycheck[] = {                                     262,
    0,   40,   41,   43,  257,   41,   41,   61,   46,  260,
    4,    0,   40,   41,  261,   55,   56,   41,   40,   41,
   58,   40,    0,    5,   40,  257,  258,  259,   45,   40,
   21,   25,   26,    0,   51,   40,   31,   32,   24,    0,
   40,   41,  263,  264,   39,    0,  265,  266,   43,   44,
   53,   40,   41,   60,   61,   62,  265,  266,   53,   54,
   55,   56,   40,   41,   44,   -1,   54,   -1,   -1,   -1,
   -1,   -1,   -1,   40,   41,   -1,   -1,   -1,   -1,   40,
   41,   -1,   -1,   -1,   -1,   40,   -1,   -1,   -1,   -1,
   40,   -1,   -1,   -1,   -1,   -1,   40,   -1,   -1,   -1,
   -1,   40,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,
  259,   -1,   -1,  262,  263,  264,  262,  262,  267,  257,
  258,  259,   -1,   -1,  262,  257,  258,  259,  262,  267,
  262,  257,  258,  259,   -1,  267,  257,  258,  259,   -1,
   -1,  267,  257,  258,  259,   -1,  267,  257,  258,  259,
  260,   -1,  262,  263,  264,  265,  266,  267,  257,  258,
  259,   -1,   -1,  262,  263,  264,  265,  266,  267,  257,
  258,  259,   -1,   -1,  262,  263,  264,   -1,   -1,  267,
  257,  258,  259,   -1,   -1,  262,  257,  258,  259,   -1,
  267,  262,  257,  258,  259,   -1,  267,  257,  258,  259,
   -1,   -1,  267,  257,  258,  259,   -1,  267,  257,  258,
  259,
};
#define YYFINAL 6
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 267
#if YYDEBUG
char *queryname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'<'","'='","'>'",0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"WORD",
"PHONIX","SOUNDEX","ASSIGN","FLOAT","OR","AND","NOT","PROX_ORDERED",
"PROX_UNORDERED","PROX_ATLEAST",
};
char *queryrule[] = {
"$accept : query",
"query : expression",
"or :",
"or : OR",
"and : AND",
"and : NOT",
"expression : term",
"expression : expression or term",
"term : factor",
"term : term and factor",
"factor : unit",
"factor : unit PROX_ORDERED unit",
"factor : unit PROX_UNORDERED unit",
"factor : PROX_ATLEAST unit",
"unit : w_unit",
"unit : '(' expression ')'",
"$$1 :",
"unit : WORD '=' $$1 '(' s_expression ')'",
"$$2 :",
"unit : WORD '=' $$2 w_unit",
"$$3 :",
"unit : WORD relop $$3 w_unit",
"relop : '=' '='",
"relop : '<'",
"relop : '>'",
"phonsound : PHONIX",
"phonsound : SOUNDEX",
"s_expression : s_term",
"s_expression : s_expression or s_term",
"s_term : s_factor",
"s_term : s_term and s_factor",
"s_factor : s_unit",
"s_factor : s_unit PROX_ORDERED s_unit",
"s_factor : s_unit PROX_UNORDERED s_unit",
"s_factor : PROX_ATLEAST s_unit",
"s_unit : w_unit",
"s_unit : '(' s_expression ')'",
"a_unit : WORD",
"a_unit : phonsound WORD",
"w_unit : a_unit",
"w_unit : a_unit ASSIGN FLOAT",
};
#endif
#define queryclearin (querychar=(-1))
#define queryerrok (queryerrflag=0)
#ifdef YYSTACKSIZE
#ifndef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#endif
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int querydebug;
int querynerrs;
int queryerrflag;
int querychar;
short *queryssp;
YYSTYPE *queryvsp;
YYSTYPE queryval;
YYSTYPE querylval;
short queryss[YYSTACKSIZE];
YYSTYPE queryvs[YYSTACKSIZE];
#define querystacksize YYSTACKSIZE

void maybe_downcase(s)
char *s;
{
#ifdef NOPAIRS
    if (*s != '"')
        if (*s != '\'')
            while (*s) *s = tolower(*s),s++;
#endif
}

queryerror(s)
char *s;
{
   waislog(WLOG_HIGH, WLOG_ERROR, "line %d: %s at or near string \"%s\"\n",
           querylineno, s, querytext);
}

querywrap()
{
   return(1);
}

static char *strcatCC(w, wl)
char *w, *wl;
{
   char *wln;
   int len;

   len = strlen(w) + strlen(wl) + 2;
   wln = (char *)malloc(len);

   wln = strcpy(wln, w);
   if (strncmp(w, "prox", 4) ||
   (strstr(w, "ordered") == NULL && strstr(w, "atleast") == NULL))
     wln = strcat(wln, " ");
   wln = strcat(wln, wl);

   return(wln);
}

static char *strcatCCC(w, wl, op)
char *w, *wl, *op;
{
   char *wln;
   int len;

   len = strlen(w) + strlen(wl) + strlen(op) + 3;
   wln = (char *)malloc(len);

   wln = strcpy(wln, w);
   if ((*wl == '=') || (*wl == '<') || (*wl == '>')) {
     /* no blank */
   } else {
     wln = strcat(wln, " ");
   }
   wln = strcat(wln, wl);
   if (strncmp(wl, "prox", 4) ||
   (strstr(wl, "ordered") == NULL && strstr(wl, "atleast") == NULL))
     wln = strcat(wln, " ");
   wln = strcat(wln, op);

   return(wln);
}

static char *strcatCCCC(w, wl, op1, op2)
char *w, *wl, *op1, *op2;
{
   char *wln;
   int len;

   len = strlen(w) + strlen(wl) + strlen(op1) + strlen(op2) + 3;
   wln = (char *)malloc(len);

   wln = strcpy(wln, w);
   if ((*wl == '=') || (*wl == '<') || (*wl == '>')) {
     /* no blank */
   } else {
     wln = strcat(wln, " ");
   }
   wln = strcat(wln, wl);
   wln = strcat(wln, " ");      
   wln = strcat(wln, op1);
   if (strncmp(op1, "prox", 4) ||
   (strstr(op1, "ordered") == NULL && strstr(op1, "atleast") == NULL))
     wln = strcat(wln, " ");
   wln = strcat(wln, op2);

   return(wln);
}

int is_duplicate_field(field_name)
     char* field_name;
{
  long i = 0;
  long len = strlen(field_name);

  for (i=0;i<fields_found_this_query;i++)
    if (!strncmp(field_name, field_name_array[i], len))
      return(1); /* it is duplicate */

  return(0); /* not duplicate and field_name can inserted. */
}

int insert_field_p(field_name,c)
     char* field_name;
     char* c;
{
  char field_name_p[MAX_WORD_LENGTH];
  strncpy(field_name_p,field_name,MAX_WORD_LENGTH);
  strncat(field_name_p,c,MAX_WORD_LENGTH);
  return(insert_field(field_name_p));
}

int insert_field(field_name)
     char* field_name;
{
  long i = 0;
  long len = strlen(field_name);

  if (fields_found_this_query > MAX_NUM_FIELDS) {
    /* overflow: should be handled with realloc ? */
    return(-1);
  }
  if (!is_duplicate_field(field_name)) {
      /* dont know why not use strdup (up)?
         field_name_array[fields_found_this_query] = strdup(field_name); 
         */
    field_name_array[fields_found_this_query] =
      (char*)malloc((size_t)((MAX_WORD_LENGTH + 1) * sizeof(char)));
    /* warn if malloc failed !! (up)*/
    strncpy(field_name_array[fields_found_this_query],field_name,MAX_WORD_LENGTH);
    fields_found_this_query++;
  }
  return(0);
}


char* analyse_string(string, number_of_fields, global_dictionary_exists)
     char* string;
     long* number_of_fields;
     int* global_dictionary_exists;
{
  int result;

  fields_found_this_query = 0;
  queryverbose = 0;
  numeric_field = 0;
  querydebug = 0;
  querybuf   = string;

  if (strchr(string,'=')) 
    queryverbose++;
  else  if (strchr(string,'>')) 
    queryverbose++;
  else if (strchr(string,'<')) queryverbose++;

  queryboolean = queryverbose || (strstr(string," and ") || strstr(string," or ")||
         strstr(string," not ") || strstr(string,"(w") ||
         strstr(string,"w/") || strstr(string,"(n") || strstr(string,"pre/"));


  if (!queryparse()) {
    field_name_array[fields_found_this_query] = NULL;
#ifdef YYDEBUG
    if (querydebug) {
      int i;
      fprintf(stderr,"Found %d fields\n", fields_found_this_query);
      for (i=0;i<fields_found_this_query;i++) {
        fprintf(stderr,"feld %d %s\n", i, 
                (field_name_array[i]==NULL)?"(nil)":field_name_array[i]);
      }
      sleep(2);
    }
#endif
    *number_of_fields = fields_found_this_query;
    *global_dictionary_exists = global_dct_exists;
    global_dct_exists = 0;
    return(queryresultstring);
  } else {
    *number_of_fields = 0;
    field_name_array[0] = NULL;
    return(NULL);
  }
}


#ifdef TEST_QUERY
long number_of_operands = 0;

int main _AP((int argc, char *argv[]));

main(argc,argv)
int argc;
char *argv[];
{
  long dummy;
  char *ptr;

  if (argc == 1) {
    char s[80];
    fprintf(stderr, ">");
    gets(s);
    while (s != NULL && *s)   {
      ptr = analyse_string(s, &dummy, &dummy);
      printf("%s => %s\n> ", s, (ptr == NULL) ? "unparseable" : ptr);
      gets(s);
    }
  } else {
    if (argc != 2) {
      fprintf(stderr, "USAGE: %s <query>\n", argv[0]);
      exit(1);
    }
    ptr = analyse_string(argv[1], &dummy, &dummy);
    printf("%s => %s\n> ", argv[1], (ptr == NULL) ? "unparseable" : ptr);
  }
  exit(0);
}
#endif
#define YYABORT goto queryabort
#define YYACCEPT goto queryaccept
#define YYERROR goto queryerrlab
int
queryparse()
{
    register int querym, queryn, querystate;
#if YYDEBUG
    register char *querys;
    extern char *getenv();

    if (querys = getenv("YYDEBUG"))
    {
        queryn = *querys;
        if (queryn >= '0' && queryn <= '9')
            querydebug = queryn - '0';
    }
#endif

    querynerrs = 0;
    queryerrflag = 0;
    querychar = (-1);

    queryssp = queryss;
    queryvsp = queryvs;
    *queryssp = querystate = 0;

queryloop:
    if (queryn = querydefred[querystate]) goto queryreduce;
    if (querychar < 0)
    {
        if ((querychar = querylex()) < 0) querychar = 0;
#if YYDEBUG
        if (querydebug)
        {
            querys = 0;
            if (querychar <= YYMAXTOKEN) querys = queryname[querychar];
            if (!querys) querys = "illegal-symbol";
            printf("querydebug: state %d, reading %d (%s)\n", querystate,
                    querychar, querys);
        }
#endif
    }
    if ((queryn = querysindex[querystate]) && (queryn += querychar) >= 0 &&
            queryn <= YYTABLESIZE && querycheck[queryn] == querychar)
    {
#if YYDEBUG
        if (querydebug)
            printf("querydebug: state %d, shifting to state %d\n",
                    querystate, querytable[queryn]);
#endif
        if (queryssp >= queryss + querystacksize - 1)
        {
            goto queryoverflow;
        }
        *++queryssp = querystate = querytable[queryn];
        *++queryvsp = querylval;
        querychar = (-1);
        if (queryerrflag > 0)  --queryerrflag;
        goto queryloop;
    }
    if ((queryn = queryrindex[querystate]) && (queryn += querychar) >= 0 &&
            queryn <= YYTABLESIZE && querycheck[queryn] == querychar)
    {
        queryn = querytable[queryn];
        goto queryreduce;
    }
    if (queryerrflag) goto queryinrecovery;
#ifdef lint
    goto querynewerror;
#endif
querynewerror:
    queryerror("syntax error");
#ifdef lint
    goto queryerrlab;
#endif
queryerrlab:
    ++querynerrs;
queryinrecovery:
    if (queryerrflag < 3)
    {
        queryerrflag = 3;
        for (;;)
        {
            if ((queryn = querysindex[*queryssp]) && (queryn += YYERRCODE) >= 0 &&
                    queryn <= YYTABLESIZE && querycheck[queryn] == YYERRCODE)
            {
#if YYDEBUG
                if (querydebug)
                    printf("querydebug: state %d, error recovery shifting\
 to state %d\n", *queryssp, querytable[queryn]);
#endif
                if (queryssp >= queryss + querystacksize - 1)
                {
                    goto queryoverflow;
                }
                *++queryssp = querystate = querytable[queryn];
                *++queryvsp = querylval;
                goto queryloop;
            }
            else
            {
#if YYDEBUG
                if (querydebug)
                    printf("querydebug: error recovery discarding state %d\n",
                            *queryssp);
#endif
                if (queryssp <= queryss) goto queryabort;
                --queryssp;
                --queryvsp;
            }
        }
    }
    else
    {
        if (querychar == 0) goto queryabort;
#if YYDEBUG
        if (querydebug)
        {
            querys = 0;
            if (querychar <= YYMAXTOKEN) querys = queryname[querychar];
            if (!querys) querys = "illegal-symbol";
            printf("querydebug: state %d, error recovery discards token %d (%s)\n",
                    querystate, querychar, querys);
        }
#endif
        querychar = (-1);
        goto queryloop;
    }
queryreduce:
#if YYDEBUG
    if (querydebug)
        printf("querydebug: state %d, reducing by rule %d (%s)\n",
                querystate, queryn, queryrule[queryn]);
#endif
    querym = querylen[queryn];
    queryval = queryvsp[1-querym];
    switch (queryn)
    {
case 1:
{
  queryresultstring = queryvsp[0].sval;    /* who calls free ? (up) */
#ifdef YYDEBUG
  if (querydebug) fprintf(stderr,"query->%s<-\n",queryresultstring);
#endif
}
break;
case 2:
{queryval.sval="";}
break;
case 3:
{queryval.sval=queryvsp[0].sval;}
break;
case 4:
{ queryval.sval = "and"; }
break;
case 5:
{ queryval.sval = "not"; }
break;
case 6:
{
                  queryval.sval = queryvsp[0].sval;
                }
break;
case 7:
{
                  if (queryboolean) {
                    ++number_of_operands;
                    queryval.sval = strcatXXC(queryvsp[-2].sval, queryvsp[0].sval, "or");
                  } else {
                    queryval.sval = strcatXX(queryvsp[-2].sval, queryvsp[0].sval);
                  }
                }
break;
case 8:
{
                  queryval.sval = queryvsp[0].sval;
                }
break;
case 9:
{
                  ++number_of_operands;
                  queryval.sval = strcatXXC(queryvsp[-2].sval, queryvsp[0].sval, queryvsp[-1].sval);
                }
break;
case 10:
{
                  queryval.sval = queryvsp[0].sval;
                }
break;
case 11:
{
                  char num[80];
                  sprintf(num, "%d", queryvsp[-1].ival);
                  ++number_of_operands;
                  queryval.sval = strcatXXCC(queryvsp[-2].sval, queryvsp[0].sval, "proxordered", num);
                }
break;
case 12:
{
                  char num[80];
                  sprintf(num, "%d", queryvsp[-1].ival);
                  ++number_of_operands;
                  queryval.sval = strcatXXCC(queryvsp[-2].sval, queryvsp[0].sval, "proxunordered", num);
                }
break;
case 13:
{
                  char num[80];
                  sprintf(num, "%d", queryvsp[-1].ival);
                  ++number_of_operands;
                  queryval.sval = strcatXCC(queryvsp[0].sval, "proxatleast", num);
                }
break;
case 14:
{
                  maybe_downcase(queryvsp[0].sval);
                  if (queryverbose) {
                    queryval.sval = strcatCCX("text", "=", queryvsp[0].sval);
                    global_dct_exists = 1;
                  } else {
                    queryval.sval = queryvsp[0].sval;
                  }
                }
break;
case 15:
{
                  queryval.sval = queryvsp[-1].sval;
                }
break;
case 16:
{current_field = queryvsp[-1].sval;numeric_field = 0;}
break;
case 17:
{
                  insert_field(current_field);
                  queryval.sval = queryvsp[-1].sval;
                  free(current_field);
                }
break;
case 18:
{current_field = queryvsp[-1].sval;numeric_field = 0;}
break;
case 19:
{
                  maybe_downcase(queryvsp[0].sval);
                  if (queryverbose) {
                    insert_field(current_field);
                    queryval.sval = strcatXCX(current_field, "=", queryvsp[0].sval);
                  } else {
                    queryval.sval = queryvsp[0].sval;
                    free(current_field);
                  }
                }
break;
case 20:
{current_field = queryvsp[-1].sval;numeric_field = 1;}
break;
case 21:
{
                  maybe_downcase(queryvsp[0].sval);
                  if (queryverbose) {
                    /* was soll hier eigentlich passieren ?
                       "py == 1990" => "py=" in die feldliste
                                       "py == 1990"
                       "py < 1990"  => "py" in die feldliste
                                       "py < 1990"
                       "py > 1990"  => "py" in die feldliste
                                       "py > 1990"
                    */
                    if(!strcmp(queryvsp[-2].sval, ">")) {
                      insert_field_p(current_field, ">");
                    }
                    else if(!strcmp(queryvsp[-2].sval, "<")) {
                      insert_field_p(current_field, "<");
                    }
                    else if(!strcmp(queryvsp[-2].sval, "==")) {
                      insert_field_p(current_field, "=");
                    }
                    else insert_field(current_field);
                    /* up: relop is static, so do not free it !! */
                    queryval.sval = strcatXCX(current_field, queryvsp[-2].sval, queryvsp[0].sval);
                  } else {
                    queryval.sval = queryvsp[0].sval;
                    free(current_field);
                  }
                  /* up do no free current_field twice */
                }
break;
case 22:
{ queryval.sval = "=="; }
break;
case 23:
{ queryval.sval = "<"; }
break;
case 24:
{ queryval.sval = ">"; }
break;
case 25:
{ queryval.ival = PHONIX;}
break;
case 26:
{ queryval.ival = SOUNDEX;}
break;
case 27:
{
                  queryval.sval = queryvsp[0].sval   ;
                }
break;
case 28:
{
                  if (queryboolean) {
                    ++number_of_operands;
                    queryval.sval = strcatXXC(queryvsp[-2].sval, queryvsp[0].sval, "or");
                  } else {
                    queryval.sval = strcatXX(queryvsp[-2].sval, queryvsp[0].sval);
                  }
                }
break;
case 29:
{
                  queryval.sval = queryvsp[0].sval;
                }
break;
case 30:
{
                  ++number_of_operands;
                  queryval.sval = strcatXXC(queryvsp[-2].sval, queryvsp[0].sval, queryvsp[-1].sval);
                }
break;
case 31:
{
                  queryval.sval = queryvsp[0].sval;
                }
break;
case 32:
{
                  char num[80];
                  sprintf(num, "%d", queryvsp[-1].ival);
                  ++number_of_operands;
                  queryval.sval = strcatXXCC(queryvsp[-2].sval, queryvsp[0].sval, "proxordered", num);
                }
break;
case 33:
{
                  char num[80];
                  sprintf(num, "%d", queryvsp[-1].ival);
                  ++number_of_operands;
                  queryval.sval = strcatXXCC(queryvsp[-2].sval, queryvsp[0].sval, "proxunordered", num);
                }
break;
case 34:
{
                  char num[80];
                  sprintf(num, "%d", queryvsp[-1].ival);
                  ++number_of_operands;
                  queryval.sval = strcatXCC(queryvsp[0].sval, "proxatleast", num);
                }
break;
case 35:
{
                  maybe_downcase(queryvsp[0].sval);
                  if (queryverbose) {
                    if(numeric_field) 
                      queryval.sval = strcatCCX(current_field, "==", queryvsp[0].sval);
                    else queryval.sval = strcatCCX(current_field, "=", queryvsp[0].sval);
                  } else {
                    queryval.sval = queryvsp[0].sval;
                  }
                }
break;
case 36:
{
                  queryval.sval = queryvsp[-1].sval;
                }
break;
case 37:
{
                    maybe_downcase(queryvsp[0].sval);
                    queryval.sval = queryvsp[0].sval;
                }
break;
case 38:
{
                  static char Key[80];

                  maybe_downcase(queryvsp[0].sval);
                  if (queryvsp[-1].ival == PHONIX) {
                      PhonixCode(queryvsp[0].sval,Key);
                  } else {
                      SoundexCode(queryvsp[0].sval,Key); /* !!! (UP) */
                  }
                  s_free(queryvsp[0].sval);
                  queryval.sval = strdup(Key);
                }
break;
case 39:
{ queryval.sval  = queryvsp[0].sval; }
break;
case 40:
{
                  char *tmp;

                  tmp = strcatCXX("<-", queryvsp[0].sval, queryvsp[-2].sval);
                  queryval.sval = tmp;
                }
break;
    }
    queryssp -= querym;
    querystate = *queryssp;
    queryvsp -= querym;
    querym = querylhs[queryn];
    if (querystate == 0 && querym == 0)
    {
#if YYDEBUG
        if (querydebug)
            printf("querydebug: after reduction, shifting from state 0 to\
 state %d\n", YYFINAL);
#endif
        querystate = YYFINAL;
        *++queryssp = YYFINAL;
        *++queryvsp = queryval;
        if (querychar < 0)
        {
            if ((querychar = querylex()) < 0) querychar = 0;
#if YYDEBUG
            if (querydebug)
            {
                querys = 0;
                if (querychar <= YYMAXTOKEN) querys = queryname[querychar];
                if (!querys) querys = "illegal-symbol";
                printf("querydebug: state %d, reading %d (%s)\n",
                        YYFINAL, querychar, querys);
            }
#endif
        }
        if (querychar == 0) goto queryaccept;
        goto queryloop;
    }
    if ((queryn = querygindex[querym]) && (queryn += querystate) >= 0 &&
            queryn <= YYTABLESIZE && querycheck[queryn] == querystate)
        querystate = querytable[queryn];
    else
        querystate = querydgoto[querym];
#if YYDEBUG
    if (querydebug)
        printf("querydebug: after reduction, shifting from state %d \
to state %d\n", *queryssp, querystate);
#endif
    if (queryssp >= queryss + querystacksize - 1)
    {
        goto queryoverflow;
    }
    *++queryssp = querystate;
    *++queryvsp = queryval;
    goto queryloop;
queryoverflow:
    queryerror("yacc stack overflow");
queryabort:
    return (1);
queryaccept:
    return (0);
}
