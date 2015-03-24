%{
/*                               -*- Mode: C -*- 
 * * qyery_l.l -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Ulrich Pfeifer
 * Created On      : Tue Feb 15 16:03:22 1994
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Wed Jan 31 15:36:36 1996
 * Language        : C
 * Update Count    : 100
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 * $$
 * $Log: query_l.l,v $
 * Revision 2.0.1.8  1996/01/31 14:50:51  pfeifer
 * patch63: Changed the query term weighting syntax to
 * patch63: 'Probabilistic <* 3 Indexing'.
 * patch63: Replace single quotes around a string by double quotes
 * patch63: after recogniszing. So single quotes may be legal chars.
 *
 * Revision 2.0.1.7  1996/01/23 15:11:17  pfeifer
 * patch62:
 *
 * Revision 2.0.1.6  1996/01/14 17:26:19  pfeifer
 * patch61: Numeric searches with negative numbers did not work.
 *
 * Revision 2.0.1.5  1995/12/20 13:04:13  pfeifer
 * patch57: Adapted to 'flex version 2.5.2'.
 * patch57: Numeric searches with '.' did not work.
 *
 * Revision 2.0.1.4  1995/12/06  11:02:50  pfeifer
 * patch53: Removed bad free() call (Patch from RodneyBarnett).
 *
 * Revision 2.0.1.3  1995/10/11  15:56:51  pfeifer
 * patch36: Query parser now knows about the legal character set.
 *
 * Revision 2.0.1.2  1995/10/11  10:07:33  pfeifer
 * Words are now forced to be made up of legal characters only.
 *
 * Revision 2.0.1.1  1995/10/10  18:30:44  pfeifer
 * patch33: Handling for query term weighting with '<- 99.99' as prefix
 * patch33: operator.
 *
 */

/* HISTORY 
 * 23-Mar-1994		Ulrich Pfeifer	
 *    Last Modified: Wed Mar 23 11:09:53 1994 #29 (Ulrich Pfeifer)
 *    Added flex support
 * 10-Mar-1994		Ulrich Pfeifer	
 *    Support for literal searches
 */

#include "query_y.h"
#include "cutil.h"

#undef  ECHO
#define ECHO 
/*
[Oo][Rr]
[Nn][Oo][Tt] 
*/
extern char* yybuf;

int prox_type;
int prox_value;
#ifdef PROXIMITY
static int recognize_proxy = 1;
#else /* PROXIMITY */
static int recognize_proxy = 0;
#endif /* PROXIMITY */

#ifdef FLEX_SCANNER
int yylineno = 0;
#define YY_SKIP_YYWRAP
#undef yywrap
#define yywrap() (1)

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
                  { \
                  int c = *(yybuf++); \
                  if (c=='\n') yylineno++; \
                  result = (c == '\0') ? YY_NULL : (buf[0] = c, 1); \
                  }
#else /* not FLEX_SCANNER */
#undef input
#define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):*(yybuf++))==10?(yylineno++,yytchar):yytchar)=='\0'?0:yytchar)
#endif /* not FLEX_SCANNER */
/* The '/' in WORD is necessary for very odd cases, when '/' is legal char and
   is followed by numbers in the query. Dont know if this is important enough
   to exclude '/' from beeing part of a word.
*/
%}
WORD  [^ \t\n=()<>/]+
%x NUMBER QWEIGHT

%%

"and"|"AND"|"And" { ECHO; return(AND); }
"or"|"OR"|"Or"    { ECHO; return(OR); }
"not"|"NOT"|"Not" { ECHO; return(NOT); }
"phonix"          { ECHO; return(PHONIX);}
"soundex"         { ECHO; return(SOUNDEX);}

[pP][rR][eE]"/"/[0-9] {
                    if (!recognize_proxy) REJECT;
                    ECHO;
                    BEGIN NUMBER;
                    prox_value = 1;   /* default value of 1 */
                    prox_type  = PROX_ORDERED;
                  }
[wW]"/"/[0-9]     {
                    if (!recognize_proxy) REJECT;
                    ECHO;
                    BEGIN NUMBER;
                    prox_value = 1;   /* default value of 1 */
                    prox_type  = PROX_UNORDERED;
                  }
atleast/[0-9]+    {
                    if (!recognize_proxy) REJECT;
                    ECHO;
                    BEGIN NUMBER;
                    prox_value = 2;   /* default value of 2 */
                    prox_type  = PROX_ATLEAST;
                  }
<NUMBER>[0-9]+    { ECHO; prox_value = atoi(yytext); }
<NUMBER>")"       {
                    BEGIN INITIAL;	/* end of number, consume this char */
                    yylval.ival = prox_value;
                    return(prox_type);
                  }
<NUMBER>.         {
                    unput(yytext[0]);	/* not a number, put it back */
                    BEGIN INITIAL;
                    yylval.ival = prox_value;
                    return(prox_type);
                  }
"<*"              { ECHO; BEGIN QWEIGHT; return(ASSIGN); }
<QWEIGHT>[0-9]+(\.[0-9]+)? { 
                    ECHO; BEGIN 0; yylval.sval = s_strdup(yytext); 
                    return (FLOAT); }
[()=<>]           { ECHO; return(yytext[0]); }
[ \t\n]           { ECHO; }
{WORD}            { char *a = yytext;
                    ECHO; 
                    /* fprintf(stderr, "word?: %s\n", a); */ 
                    while (*a && (isalnum(*a) || *a == '*' || *a == '.' || *a == '-' ||  *a == '+')) {
                        a++;
                    }
                    if (*a) {
                        if (a == yytext) {
                            /* No char accepted */
                            /* fprintf(stderr, "unput: %c %s %d\n", *a, a, 1);*/
                            REJECT;
                        } else {
                            char *s = a;
                            char ch = *a;
                            /*
                            fprintf(stderr, "unput: %c %s %d\n", *a, a, 
                                    strlen(a));
                                    */
                            *a = '\0';
                            yylval.sval = s_strdup(yytext); 
                            *a = ch;
                            yyless(strlen(yylval.sval));
                            return(WORD);
                        }
                    } else {
                        /* fprintf(stderr, "word!: %s\n", a, yytext); */
                        yylval.sval = s_strdup(yytext); 
                        return(WORD);
                    }
                  }
\'[^']*\'         { 
                    if (recognize_proxy) REJECT;
                    yytext[0] = '"'; yytext[yyleng-1] = '"';
	            ECHO; yylval.sval = s_strdup(yytext); return(WORD);}
\"[^"]*\"         { 
                    if (recognize_proxy) REJECT;
                    ECHO; yylval.sval = s_strdup(yytext); return(WORD);}
%%
