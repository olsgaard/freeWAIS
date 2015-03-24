/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.	
  
   3.26.90	Harry Morris, morris@think.com
   4.14.90  HWM - changed symbol for boolean query from QT_1 to 
   				  QT_BooleanQuery
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/*----------------------------------------------------------------------*/
/* This file implements the type 1 query defined in appendices B & C
   of the SR 1 spec.
 */
/*----------------------------------------------------------------------*/

#ifndef _H_Type_1_Query_
#define _H_Type_1_Query_

#include "cdialect.h"

#include "zutil.h"

/*----------------------------------------------------------------------*/
/* types and constants */

/* new data tags */
#define	DT_AttributeList	(data_tag)44
#define DT_Term			(data_tag)45
#define DT_Operator		(data_tag)46

#define QT_BooleanQuery	"1"		/* standard boolean query */

/* general attribute code - use in place of any attribute */
#define IGNORE	"ig"

/* use value codes */
#define	UV_ISBN	"ub"
#define	CORPORATE_NAME	"uc"
#define	ISSN	"us"
#define	PERSONAL_NAME	"up"
#define	SUBJECT	"uj"
#define	TITLE	"ut"
#define	GEOGRAPHIC_NAME	"ug"
#define	CODEN	"ud"
#define	SUBJECT_SUBDIVISION	"ue"
#define	SERIES_TITLE	"uf"
#define	MICROFORM_GENERATION	"uh"
#define	PLACE_OF_PUBLICATION	"ui"
#define	NUC_CODE	"uk"
#define	LANGUAGE	"ul"
#define	COMBINATION_OF_USE_VALUES	"um"
#define	SYSTEM_CONTROL_NUMBER	"un"
#define	DATE	"uo"
#define	LC_CONTROL_NUMBER	"ur"
#define	MUSIC_PUBLISHERS_NUMBER	"uu"
#define	GOVERNMENT_DOCUMENTS_NUMBER	"uv"
#define	SUBJECT_CLASSIFICATION	"uw"
#define	RECORD_TYPE	"uy"

/* relation value codes */
#define	EQUAL	"re"
#define	GREATER_THAN	"rg"
#define	GREATER_THAN_OR_EQUAL	"ro"
#define	LESS_THAN	"rl"
#define	LESS_THAN_OR_EQUAL	"rp"
#define	NOT_EQUAL	"rn"

/* position value codes */
#define	FIRST_IN_FIELD	"pf"
#define	FIRST_IN_SUBFIELD	"ps"
#define	FIRST_IN_A_SUBFIELD	"pa"
#define	FIRST_IN_NOT_A_SUBFIELD	"pt"
#define	ANY_POSITION_IN_FIELD	"py"

/* structure value codes */
#define	PHRASE	"sp"
#define	WORD	"sw"
#define	KEY	"sk"
#define	WORD_LIST	"sl"

/* truncation value codes */
#define	NO_TRUNCATION	"tn"
#define	RIGHT_TRUNCATION	"tr"
#define	PROC_NUM_INCLUDED_IN_SEARCH_ARG	"ti"

/* completeness value codes */
#define	INCOMPLETE_SUBFIELD	"ci"
#define	COMPLETE_SUBFIELD	"cs"
#define	COMPLETEFIELD	"cf"

/* operator codes */
#define AND	"a"
#define OR	"o"
#define AND_NOT	"n"

/* term types */
#define TT_Attribute		1
#define	TT_ResultSetID		2
#define	TT_Operator			3

#define ATTRIBUTE_SIZE		3
#define OPERATOR_SIZE		2

typedef struct query_term {
  /* type */
  long	TermType;
  /* for term */
  char	Use[ATTRIBUTE_SIZE];
  char	Relation[ATTRIBUTE_SIZE];
  char	Position[ATTRIBUTE_SIZE];
  char	Structure[ATTRIBUTE_SIZE];
  char	Truncation[ATTRIBUTE_SIZE];
  char	Completeness[ATTRIBUTE_SIZE];
  any*	Term;
  /* for result set */
  any*	ResultSetID;
  /* for operator */
  char	Operator[OPERATOR_SIZE];
} query_term;

/*----------------------------------------------------------------------*/
/* functions */

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

query_term* makeAttributeTerm _AP((
        char* use,char* relation,char* position,char* structure,
	char* truncation,char* completeness,any* term));
query_term* makeResultSetTerm _AP((any* resultSet));
query_term* makeOperatorTerm _AP((char* operatorCode));
void freeTerm _AP((query_term* qt));
char* writeQueryTerm _AP((query_term* qt,char* buffer,long* len));
char* readQueryTerm _AP((query_term** qt,char* buffer));
any* writeQuery _AP((query_term** terms));
query_term** readQuery _AP((any* info));

#ifdef __cplusplus
	}
#endif /* def __cplusplus */

/*----------------------------------------------------------------------*/

#endif
