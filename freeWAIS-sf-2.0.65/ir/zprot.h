/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.	
  
   3.26.90	Harry Morris, morris@think.com
   3.30.90  Harry Morris - Changed any->bits to any->bytes
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef _H_Z39_50

#define _H_Z39_50

#include "cdialect.h"

#include "zutil.h"

/*----------------------------------------------------------------------*/
/* APDU types */

typedef struct InitAPDU {
	pdu_type		PDUType;
	boolean			willSearch,willPresent,willDelete;
	boolean			supportAccessControl,supportResourceControl;
	long			PreferredMessageSize;
	long			MaximumRecordSize;
	char*			IDAuthentication;
	char*			ImplementationID;
	char* 			ImplementationName;
	char*			ImplementationVersion;
	any*			ReferenceID;
	void*			UserInformationField;
	} InitAPDU;

typedef struct InitResponseAPDU {
	pdu_type		PDUType;
	boolean			Result;
	boolean			willSearch,willPresent,willDelete;
	boolean			supportAccessControl,supportResourceControl;
	long			PreferredMessageSize;
	long 			MaximumRecordSize;
	char*			IDAuthentication;
	char*			ImplementationID;
	char* 			ImplementationName;
	char*			ImplementationVersion;
	any*			ReferenceID;
	void*			UserInformationField;
	} InitResponseAPDU;

typedef struct SearchAPDU {
	pdu_type		PDUType;
	long	 		SmallSetUpperBound;
	long			LargeSetLowerBound;
	long	 		MediumSetPresentNumber;
	boolean 		ReplaceIndicator;
	char*			ResultSetName;
	char**			DatabaseNames;   
	char*			QueryType;
	char**			ElementSetNames;  
	any*			ReferenceID;
	void*			Query;
	} SearchAPDU;

typedef struct SearchResponseAPDU {
	pdu_type		PDUType;
	long			SearchStatus;
	long			ResultCount;
	long			NumberOfRecordsReturned;
	long		 	NextResultSetPosition;
	long			ResultSetStatus;
	long 			PresentStatus;
	any*			ReferenceID;
	void*			DatabaseDiagnosticRecords;
	} SearchResponseAPDU;

typedef struct PresentAPDU {
	pdu_type		PDUType;
	long			NumberOfRecordsRequested;
	long			ResultSetStartPosition;
	char*		 	ResultSetID;
	char*			ElementSetNames;
	any*			ReferenceID;
	void*			PresentInfo;
	} PresentAPDU;

typedef struct PresentResponseAPDU {
	pdu_type		PDUType;
	boolean			PresentStatus;
	long			NumberOfRecordsReturned;
	long			NextResultSetPosition;
	any*			ReferenceID;
	void*			DatabaseDiagnosticRecords;
	} PresentResponseAPDU;

/*----------------------------------------------------------------------*/
/* Functions */

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

InitAPDU* makeInitAPDU _AP((boolean search,boolean present,boolean deleteIt,
			    boolean accessControl,boolean resourceControl,
			    long prefMsgSize,long maxMsgSize,
			    char* auth,char* id,char* name, char* version,
			    any* refID,void* userInfo));
void freeInitAPDU _AP((InitAPDU* init));
char* writeInitAPDU _AP((InitAPDU* init,char* buffer,long* len));
char* readInitAPDU _AP((InitAPDU** init,char* buffer));


InitResponseAPDU* makeInitResponseAPDU _AP((boolean result,
					    boolean search,boolean present,boolean deleteIt,
					    boolean accessControl,boolean resourceControl,
					    long prefMsgSize,long maxMsgSize,
					    char* auth,char* id,char* name, char* version,
					    any* refID,void* userInfo));
void freeInitResponseAPDU _AP((InitResponseAPDU* init));
char* writeInitResponseAPDU _AP((InitResponseAPDU* init,char* buffer,long* len));
char* readInitResponseAPDU _AP((InitResponseAPDU** init,char* buffer));
InitResponseAPDU* replyToInitAPDU _AP((InitAPDU* init,boolean result,void* userInfo));

SearchAPDU* makeSearchAPDU _AP((long small,long large, long medium,
				boolean replace,char* name,char** databases,
				char* type,char** elements,any* refID,void* queryInfo));
void freeSearchAPDU _AP((SearchAPDU* query));
char* writeSearchAPDU _AP((SearchAPDU* query,char* buffer,long* len));
char* readSearchAPDU _AP((SearchAPDU** query,char* buffer));

SearchResponseAPDU* makeSearchResponseAPDU _AP((long result,long count,
						long recordsReturned,long nextPos,
						long resultStatus,long presentStatus,
						any* refID,void* records));
void freeSearchResponseAPDU _AP((SearchResponseAPDU* queryResponse));
char* writeSearchResponseAPDU _AP((SearchResponseAPDU* queryResponse,char* buffer,long* len));
char* readSearchResponseAPDU _AP((SearchResponseAPDU** queryResponse,char* buffer));

PresentAPDU* makePresentAPDU _AP((long recsReq, long startPos,
				  char* resultID,any* refID,void* info));
void freePresentAPDU _AP((PresentAPDU* present));
char* writePresentAPDU _AP((PresentAPDU* present,char* buffer,long* len));
char* readPresentAPDU _AP((PresentAPDU** present,char* buffer));

PresentResponseAPDU* makePresentResponseAPDU _AP((boolean status,long recsRet,
						  long nextPos,any* refID,
						  void* records));
void freePresentResponseAPDU _AP((PresentResponseAPDU* present));
char* writePresentResponseAPDU _AP((PresentResponseAPDU* present,char* buffer,long* len));
char* readPresentResponseAPDU _AP((PresentResponseAPDU** present,char* buffer));

/*----------------------------------------------------------------------*/
/* user extension hooks: */

extern char* writeInitInfo _AP((InitAPDU* init,char* buffer,long* len));
extern char* readInitInfo _AP((void** info,char* buffer));

extern char* writeInitResponseInfo _AP((InitResponseAPDU* init,char* buffer,long* len));
extern char* readInitResponseInfo _AP((void** info,char* buffer));

extern char* writeSearchInfo _AP((SearchAPDU* query,char* buffer,long* len));
extern char* readSearchInfo _AP((void** info,char* buffer));

extern char* writeSearchResponseInfo _AP((SearchResponseAPDU* query,char* buffer,long* len));
extern char* readSearchResponseInfo _AP((void** info,char* buffer));

extern char* writePresentInfo _AP((PresentAPDU* present,char* buffer,long* len));
extern char* readPresentInfo _AP((void** info,char* buffer));

extern char* writePresentResponseInfo _AP((PresentResponseAPDU* present,char* buffer,long* len));
extern char* readPresentResponseInfo _AP((void** info,char* buffer));

#ifdef __cplusplus
	}
#endif /* def __cplusplus */

/*----------------------------------------------------------------------*/

#endif /* ndef _H_Z39_50 */


