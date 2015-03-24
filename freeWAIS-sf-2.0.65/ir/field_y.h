typedef union {
   int  ival;
   char *sval;
   char cval;
   DateDescription date;
   } YYSTYPE;
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


extern YYSTYPE fieldlval;
