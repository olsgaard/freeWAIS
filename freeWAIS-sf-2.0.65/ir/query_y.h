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
typedef union {
   int  ival;
   char *sval;
   } YYSTYPE;
extern YYSTYPE querylval;
