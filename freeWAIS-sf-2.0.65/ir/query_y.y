/*                               -*- Mode: C -*- 
 * * query_y.y -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Ulrich Pfeifer
 * Created On      : Tue Feb 15 16:11:56 1994
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Wed Jan 31 13:30:23 1996
 * Language        : C
 * Update Count    : 307
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 * $$
 * $Log: query_y.y,v $
 * Revision 2.0.1.5  1996/01/31 14:51:42  pfeifer
 * patch63: Forgott to switch yydebug off :-(
 *
 * Revision 2.0.1.4  1995/11/14  18:19:57  pfeifer
 * patch51: Did we really forgot to integrate 2.0.1.[1-3] in the patches?
 * patch51: Shudder!
 *
 * Revision 2.0.1.3  1995/11/14  16:15:10  pfeifer
 * patch50:
 *
 * Revision 2.0.1.2  1995/10/11  15:57:27  pfeifer
 * patch36: Clean grammar.
 *
 * Revision 2.0.1.1  1995/10/10  18:32:26  pfeifer
 * patch33: Added query term weighting. Did some cleanup of the
 * patch33: grammar. Not yet sure that all expressions remeained
 * patch33: valid. Means Not sure that the grammar works ;-)
 *
 * Revision 2.1  1995/10/10  16:15:24  pfeifer
 * Query term weighting for global terms.
 *
 */

%{
#include "cutil.h"
#include <stdio.h>
#include <ctype.h>
void maybe_downcase();

#define YYSTYPE yystype
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

extern int yylineno;
extern char yytext[];
#ifdef YYBISON
int yydebug;
#else
extern int yydebug;
#endif
static int yyverbose = 0;
static int yyboolean = 0;
static int numeric_field = 0;
static int global_dct_exists = 0;
char *current_field;
char *yybuf;
char *yyresultstring;
#define MAX_NUM_FIELDS 100
char *field_name_array[MAX_NUM_FIELDS];
static long fields_found_this_query = 0;

extern long number_of_operands; /* defined in field_search.c */
/* char** field_name_array = NULL; */

static char * strcatCC _AP((char *w, char *wl));
static char * strcatCCC _AP((char *w, char *wl, char *op));
static char * strcatCCCC _AP((char *w, char *wl, char *op1, char *op2));
%}
%token <sval> WORD
%token <sval> PHONIX SOUNDEX ASSIGN FLOAT
%left <sval> OR
%left <sval> AND
%left <sval> NOT
%nonassoc <sval> PROX_ORDERED PROX_UNORDERED PROX_ATLEAST

%type <sval> query
%type <sval> or
%type <sval> expression
%type <sval> term
%type <sval> factor
%type <sval> unit
%type <sval> relop
%type <sval> phonsound
%type <sval> s_expression
%type <sval> s_term
%type <sval> s_factor
%type <sval> s_unit

%union {
   int  ival;
   char *sval;
   }

%%
query           : expression 
{
  yyresultstring = $<sval>1;    /* who calls free ? (up) */
#ifdef YYDEBUG
  if (yydebug) fprintf(stderr,"query->%s<-\n",yyresultstring);
#endif
}
                ;

or              : %prec OR {$$="";}
                | OR {$$=$1;}
                ;

and             : AND { $<sval>$ = "and"; }
                | NOT { $<sval>$ = "not"; }
                ;

expression      : term
                {
                  $<sval>$ = $<sval>1;
                }
                | expression or term
                {
                  if (yyboolean) {
                    ++number_of_operands;
                    $<sval>$ = strcatXXC($<sval>1, $<sval>3, "or");
                  } else {
                    $<sval>$ = strcatXX($<sval>1, $<sval>3);
                  }
                }
                ;

term            : factor
                {
                  $<sval>$ = $<sval>1;
                }
                | term and factor
                {
                  ++number_of_operands;
                  $<sval>$ = strcatXXC($<sval>1, $<sval>3, $<sval>2);
                }
                ;

factor          : unit
                {
                  $$ = $1;
                }
                | unit PROX_ORDERED unit
                {
                  char num[80];
                  sprintf(num, "%d", $<ival>2);
                  ++number_of_operands;
                  $$ = strcatXXCC($1, $3, "proxordered", num);
                }
                | unit PROX_UNORDERED unit
                {
                  char num[80];
                  sprintf(num, "%d", $<ival>2);
                  ++number_of_operands;
                  $$ = strcatXXCC($1, $3, "proxunordered", num);
                }
                | PROX_ATLEAST unit
                {
                  char num[80];
                  sprintf(num, "%d", $<ival>1);
                  ++number_of_operands;
                  $$ = strcatXCC($2, "proxatleast", num);
                }
                ;

unit            : w_unit 
                {
                  maybe_downcase($<sval>1);
                  if (yyverbose) {
                    $<sval>$ = strcatCCX("text", "=", $<sval>1);
                    global_dct_exists = 1;
                  } else {
                    $<sval>$ = $<sval>1;
                  }
                }
                | '(' expression ')'
                {
                  $<sval>$ = $<sval>2;
                }
                | WORD '=' {current_field = $<sval>1;numeric_field = 0;} 
                          '(' s_expression ')'
                {
                  insert_field(current_field);
                  $<sval>$ = $<sval>5;
                  free(current_field);
                }
                | WORD '=' {current_field = $<sval>1;numeric_field = 0;} w_unit
                {
                  maybe_downcase($<sval>4);
                  if (yyverbose) {
                    insert_field(current_field);
                    $<sval>$ = strcatXCX(current_field, "=", $<sval>4);
                  } else {
                    $<sval>$ = $<sval>4;
                    free(current_field);
                  }
                }
                | WORD relop {current_field = $<sval>1;numeric_field = 1;} 
                  w_unit
                {
                  maybe_downcase($<sval>4);
                  if (yyverbose) {
                    /* was soll hier eigentlich passieren ?
                       "py == 1990" => "py=" in die feldliste
                                       "py == 1990"
                       "py < 1990"  => "py" in die feldliste
                                       "py < 1990"
                       "py > 1990"  => "py" in die feldliste
                                       "py > 1990"
                    */
                    if(!strcmp($<sval>2, ">")) {
                      insert_field_p(current_field, ">");
                    }
                    else if(!strcmp($<sval>2, "<")) {
                      insert_field_p(current_field, "<");
                    }
                    else if(!strcmp($<sval>2, "==")) {
                      insert_field_p(current_field, "=");
                    }
                    else insert_field(current_field);
                    /* up: relop is static, so do not free it !! */
                    $<sval>$ = strcatXCX(current_field, $<sval>2, $<sval>4);
                  } else {
                    $<sval>$ = $<sval>4;
                    free(current_field);
                  }
                  /* up do no free current_field twice */
                }
                ;
relop           : '=' '=' { $<sval>$ = "=="; }
                | '<'     { $<sval>$ = "<"; }
                | '>'     { $<sval>$ = ">"; }
                ;
phonsound       : PHONIX  { $<ival>$ = PHONIX;}
                | SOUNDEX { $<ival>$ = SOUNDEX;}
                ;
s_expression    : s_term 
                {
                  $<sval>$ = $<sval>1   ;
                }
                | s_expression or s_term
                {
                  if (yyboolean) {
                    ++number_of_operands;
                    $<sval>$ = strcatXXC($<sval>1, $<sval>3, "or");
                  } else {
                    $<sval>$ = strcatXX($<sval>1, $<sval>3);
                  }
                }
                ;

s_term          : s_factor 
                {
                  $<sval>$ = $<sval>1;
                }
                | s_term and s_factor 
                {
                  ++number_of_operands;
                  $<sval>$ = strcatXXC($<sval>1, $<sval>3, $<sval>2);
                }
                ;

s_factor        : s_unit
                {
                  $$ = $1;
                }
                | s_unit PROX_ORDERED s_unit
                {
                  char num[80];
                  sprintf(num, "%d", $<ival>2);
                  ++number_of_operands;
                  $$ = strcatXXCC($1, $3, "proxordered", num);
                }
                | s_unit PROX_UNORDERED s_unit
                {
                  char num[80];
                  sprintf(num, "%d", $<ival>2);
                  ++number_of_operands;
                  $$ = strcatXXCC($1, $3, "proxunordered", num);
                }
                | PROX_ATLEAST s_unit
                {
                  char num[80];
                  sprintf(num, "%d", $<ival>1);
                  ++number_of_operands;
                  $$ = strcatXCC($2, "proxatleast", num);
                }
                ;

s_unit          : w_unit
                {
                  maybe_downcase($<sval>1);
                  if (yyverbose) {
                    if(numeric_field) 
                      $<sval>$ = strcatCCX(current_field, "==", $<sval>1);
                    else $<sval>$ = strcatCCX(current_field, "=", $<sval>1);
                  } else {
                    $<sval>$ = $<sval>1;
                  }
                }
                | '(' s_expression ')'
                {
                  $<sval>$ = $<sval>2;
                }
                ;
a_unit          : WORD
                {
                    maybe_downcase($<sval>1);
                    $<sval>$ = $<sval>1;
                }
                | phonsound WORD
                {
                  static char Key[80];

                  maybe_downcase($<sval>2);
                  if ($<ival>1 == PHONIX) {
                      PhonixCode($<sval>2,Key);
                  } else {
                      SoundexCode($<sval>2,Key); /* !!! (UP) */
                  }
                  s_free($<sval>2);
                  $<sval>$ = strdup(Key);
                }
                ;
w_unit          : a_unit { $<sval>$  = $<sval>1; }
                | a_unit ASSIGN FLOAT
                {
                  char *tmp;

                  tmp = strcatCXX("<-", $<sval>3, $<sval>1);
                  $<sval>$ = tmp;
                }

%%

void maybe_downcase(s)
char *s;
{
#ifdef NOPAIRS
    if (*s != '"')
        if (*s != '\'')
            while (*s) *s = tolower(*s),s++;
#endif
}

yyerror(s)
char *s;
{
   waislog(WLOG_HIGH, WLOG_ERROR, "line %d: %s at or near string \"%s\"\n",
           yylineno, s, yytext);
}

yywrap()
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
  yyverbose = 0;
  numeric_field = 0;
  yydebug = 0;
  yybuf   = string;

  if (strchr(string,'=')) 
    yyverbose++;
  else  if (strchr(string,'>')) 
    yyverbose++;
  else if (strchr(string,'<')) yyverbose++;

  yyboolean = yyverbose || (strstr(string," and ") || strstr(string," or ")||
         strstr(string," not ") || strstr(string,"(w") ||
         strstr(string,"w/") || strstr(string,"(n") || strstr(string,"pre/"));


  if (!yyparse()) {
    field_name_array[fields_found_this_query] = NULL;
#ifdef YYDEBUG
    if (yydebug) {
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
    return(yyresultstring);
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
