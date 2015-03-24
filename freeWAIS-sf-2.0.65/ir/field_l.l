%{
/*                               -*- Mode: C -*- 
 * field_l.l -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Ulrich Pfeifer
 * Created On      : Tue Feb 15 16:03:22 1994
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Wed Dec 20 14:17:22 1995
 * Update Count    : 274
 * Status          : Unknown, Use with caution!
 */


/* HISTORY 
 * 10-Mar-1994		Ulrich Pfeifer	
 *    added support for backslash escaped '/'.
 */
#include "field_index.h"
#include "field_y.h"
#include "cutil.h"
#ifdef FLEX_SCANNER
int yylineno = 0;
#define UPDATELINENO if (yytext[0]=='\n') yylineno++;
#define YY_SKIP_YYWRAP
#undef yywrap
#define yywrap() (1)
#else /* not FLEX_SCANNER */
#define UPDATELINENO
#endif /* not FLEX_SCANNER */

#undef  ECHO
#define ECHO 
%}
WORD  [^ \t\n=()/]+
%%
"#".*       { ECHO; }
"/"[^/]*"/" { ECHO; 
	      if (yytext[yyleng-2] == '\\') { 
                /* the ending '/' was escaped, lets get the rest */
                unput(yytext[yyleng-1]);/* yes yyless() is better */ 
		/* unput has backed up over the backslash, so
			thats one less character to glue back on */
		yyleng-- ;  /* adjust for omitted backslash */
                yymore(); 
	      } else {
                /* Flex bug?  When there is more than one escape, the
                   call to unput adds two chars to the front of yytext.
                   We now skip them :-(
                   */
                /* while (*s && *s != '/') s++; */
                yylval.sval = s_malloc(4*strlen(yytext)+1); /* Space for escapes */

		{ char *s = yytext;
                  char *t = yylval.sval;
		  while (*s) { 
		    if (*s == '\\') {
                      /*  escape */
                      s++;
		      if (*s == '/') { /* escaped '/' */
			*t = *s;
		      } else if (*s == '\\') { /* double \\ */
                        *t = '\\';
		      }	else if ((*s == 'n' || *s == 'J')) {
#ifdef ESCAPE_NEWLINE
                        sprintf(t, "\\%03o", (int) '\n');
                        t+=3;
#else /* not ESCAPE_NEWLINE */
#ifdef MYREGEXP
                        *t = '\n';
#else /* not MYREGEXP */
                        *t = '$';
#endif /* not MYREGEXP */
#endif /* not ESCAPE_NEWLINE */
		      } else if (*s >= 'A' && *s <= 'Z') { 
#ifdef ESCAPE_CONTROL
                        sprintf(t, "\\%03o", (int) (*s-'A'+1));
                        t+=3;
#else /* not ESCAPE_CONTROL */
                        *t = (*s-'A'+1);
#endif /* not ESCAPE_CONTROL */
		      }	else if (*s == 'r') {
#ifdef ESCAPE_CR
                        sprintf(t, "\\%03o", (int) '\r');
                        t+=3;
#else /* not ESCAPE_CR */
                        *t = '\r';
#endif /* not ESCAPE_CR */
		      }	else if (*s == 't') {
#ifdef ESCAPE_TAB
                        sprintf(t, "\\%03o", (int) '\t');
                        t+=3;
#else /* not ESCAPE_TAB */
                        *t = '\t';
#endif /* not ESCAPE_TAB */
		      }	else if (*s == 'f') {
#ifdef ESCAPE_FORMFEED
                        sprintf(t, "\\%03o", (int) '\f');
                        t+=3;
#else /* not ESCAPE_FORMFEED */
                        *t = '\f';
#endif /* not ESCAPE_FORMFEED */
		      } else {       /* let the escaped char unmodified */
			*t = *s;
		      }
                      /* fprintf(stderr, "%c%c %c%c\n", *s, *(s+1), *t, *(t+1));*/
                      s++; t++;
		    } else {
                        /* No escape */
                        *(t++) = *(s++);
                    }
		  }
		  *t = *s; /* should be '\0' */
                  /* fprintf(stderr,"\nregexp:%s:%d %d %d %d\n", yylval.sval, (int)yylval.sval, t, (int)yytext, s); */
		  return(REGEXP); 
		}
	      }
	    }
[0-9]+            { ECHO; yylval.ival = atoi(yytext); return(INT); }
"<date>"          { ECHO; return(DATE); }
"date":?          { ECHO; return(DATE); }
"<end>"           { ECHO; return(END); }
"end":?           { ECHO; return(END); }
"<field>"         { ECHO; return(FIELD); }
"region":?        { ECHO; return(FIELD); }
"<headline>"      { ECHO; return(HEADLINE); }
"headline":?      { ECHO; return(HEADLINE); }
"<layout>"        { ECHO; return(LAYOUT); }
"layout":?        { ECHO; return(LAYOUT); }
"<numeric>"       { ECHO; return(NUMERIC); }
"numeric":?       { ECHO; return(NUMERIC); }
"<record-end>"    { ECHO; return(RECORD_END); }
"record-sep":?    { ECHO; return(RECORD_END); }
"stemming"        { ECHO; return(STEMMING); }
[ \t\n]           { ECHO; UPDATELINENO; }
"SOUNDEX"         { /* tung: it is very bad style to use the strings here! use a enum (up) */
                    ECHO; yylval.sval = strdup(yytext); return(ISOUNDEX); }
"PHONIX"          { ECHO; yylval.sval = strdup(yytext); return(IPHONIX); }
"TEXT"            { ECHO; yylval.sval = strdup(yytext); return(ITEXT); }
"LOCAL"           { ECHO; return(LOCAL); }
"IGNORE"          { ECHO; return(IGNORE); }
"GLOBAL"          { ECHO; return(GLOBAL); }
"BOTH"            { ECHO; return(BOTH); }
"day"             { ECHO; yylval.sval = "day"; return(DAY); }
"month"           { ECHO; yylval.sval = "month"; return(MONTH); }
"year"            { ECHO; yylval.sval = "year"; return(YEAR); }
"string"          { ECHO; yylval.sval = "string";  return(STRING); }
\"[^"]*\"         { ECHO; 
                    yylval.sval = strdup(yytext+1); 
                    yylval.sval[strlen(yylval.sval)-1] = '\0'; 
                    return(DESCRIPTION); }
{WORD}            { ECHO; yylval.sval = strdup(yytext); return(WORD); }
