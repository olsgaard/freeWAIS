/* WIDE AREA INFORMATION SERVER SOFTWARE:`
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.	
/*

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/zprot.c,v 2.0.1.1 1995/09/15 09:51:07 pfeifer Exp $";
#endif

/* Change log:
 * $Log: zprot.c,v $
 * Revision 2.0.1.1  1995/09/15 09:51:07  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:02:59  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.4  1994/08/05  07:13:01  pfeifer
 * Release beta 04
 *
 * Revision 1.3  1994/05/20  12:58:19  pfeifer
 * beta
 *
 * Revision 1.2  1994/03/08  21:07:22  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.4  92/02/12  13:59:51  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
 * 
  
   3.26.90	Harry Morris, morris@think.com
   3.30.90  Harry Morris - Changed any->bits to any->bytes
   4.11.90  HWM - generalized conditional includes (see c-dialect.h)
*/

#define _C_Z39_50_

#include "zprot.h"

#include "zutil.h"
#include "cutil.h"
/*#include <string.h>*/

#ifndef ANSI_LIKE
#include "ustubs.h"
#endif

#define RESERVE_SPACE_FOR_HEADER(spaceLeft)		\
	*spaceLeft -= HEADER_LEN;
	
#define RELEASE_HEADER_SPACE(spaceLeft)			\
	if (*spaceLeft > 0)							\
	  *spaceLeft += HEADER_LEN;
	
/*----------------------------------------------------------------------*/

InitAPDU* 
makeInitAPDU(search,
	     present,
	     deleteIt,
	     accessControl,
	     resourceControl,
	     prefSize,
	     maxMsgSize,
	     auth,
	     id,
	     name,
	     version,
	     refID,
	     userInfo)
boolean search;
boolean present;
boolean deleteIt;
boolean accessControl;
boolean resourceControl;
long prefSize;
long maxMsgSize;
char* auth;
char* id;
char* name;
char* version;
any* refID;
void* userInfo;

/* build an init APDU with user specified information */
{ 
  InitAPDU* init = (InitAPDU*)s_malloc((size_t)sizeof(InitAPDU));

  init->PDUType = initAPDU;
  init->willSearch = search;
  init->willPresent = present;
  init->willDelete = deleteIt;
  init->supportAccessControl = accessControl;
  init->supportResourceControl = resourceControl;
  init->PreferredMessageSize = prefSize;
  init->MaximumRecordSize = maxMsgSize;
  init->IDAuthentication = s_strdup(auth);
  init->ImplementationID = s_strdup(id);
  init->ImplementationName = s_strdup(name);
  init->ImplementationVersion = s_strdup(version);
  init->ReferenceID = duplicateAny(refID);
  init->UserInformationField = userInfo; /* not copied */
  
  return(init);
}

/*----------------------------------------------------------------------*/

void 
freeInitAPDU(init)
InitAPDU* init;
/* free an initAPDU */
{
  s_free(init->IDAuthentication);
  s_free(init->ImplementationID);
  s_free(init->ImplementationName);
  s_free(init->ImplementationVersion);
  freeAny(init->ReferenceID);
  s_free(init);
}

/*----------------------------------------------------------------------*/

char* 
writeInitAPDU(init,buffer,len)
InitAPDU* init;
char* buffer;
long* len;
/* write the init to a buffer, adding system information */
{
  char* buf = buffer + HEADER_LEN; /* leave room for the header-length-indicator */
  long size;
  bit_map* optionsBM = NULL;

  RESERVE_SPACE_FOR_HEADER(len);
  
  buf = writePDUType(init->PDUType,buf,len);
  
  buf = writeProtocolVersion(buf,len);
  
  optionsBM = makeBitMap((unsigned long)5,init->willSearch,init->willPresent,
                         init->willDelete,init->supportAccessControl,
                         init->supportResourceControl);
  buf = writeBitMap(optionsBM,DT_Options,buf,len);
  freeBitMap(optionsBM);

  buf = writeNum(init->PreferredMessageSize,DT_PreferredMessageSize,buf,len);
  buf = writeNum(init->MaximumRecordSize,DT_MaximumRecordSize,buf,len);
  buf = writeString(init->IDAuthentication,DT_IDAuthentication,buf,len);
  buf = writeString(init->ImplementationID,DT_ImplementationID,buf,len);
  buf = writeString(init->ImplementationName,DT_ImplementationName,buf,len);
  buf = writeString(init->ImplementationVersion,DT_ImplementationVersion,buf,len);
  buf = writeAny(init->ReferenceID,DT_ReferenceID,buf,len);
  
  /* go back and write the header-length-indicator */
  RELEASE_HEADER_SPACE(len);
  size = buf - buffer - HEADER_LEN; 
  writeBinaryInteger(size,HEADER_LEN,buffer,len);

  if (init->UserInformationField != NULL)
    buf = writeInitInfo(init,buf,len);   
    
  return(buf); 
}

/*----------------------------------------------------------------------*/

char* 
readInitAPDU(init,buffer)
InitAPDU** init;
char* buffer;
{
  char* buf = buffer;
  boolean search,present,delete,accessControl,resourceControl;
  long prefSize,maxMsgSize;
  char *auth,*id,*name,*version;
  long size; 
  pdu_type pduType;
  bit_map* versionBM = NULL;
  bit_map* optionsBM = NULL;
  any *refID = NULL;
  void* userInfo = NULL;
  
  auth = id = name = version = NULL;
  
  /* read required part */
  buf = readBinaryInteger(&size,HEADER_LEN,buf); 
  buf = readPDUType(&pduType,buf);
  buf = readBitMap(&versionBM,buf); 
  buf = readBitMap(&optionsBM,buf);
  buf = readNum(&prefSize,buf);
  buf = readNum(&maxMsgSize,buf);
  
  /* decode optionsBM */
  search = bitAtPos(0L,optionsBM);
  present = bitAtPos(1L,optionsBM);
  delete = bitAtPos(2L,optionsBM);
  accessControl = bitAtPos(3L,optionsBM);
  resourceControl = bitAtPos(4L,optionsBM);
  
  /* read optional part */
  while (buf < (buffer + size + HEADER_LEN)) 
    { data_tag tag = peekTag(buf);
      switch (tag)
	{ case DT_IDAuthentication:
	    buf = readString(&auth,buf);
	    break;
	  case DT_ImplementationID:
	    buf = readString(&id,buf);
	    break;
	  case DT_ImplementationName:
	    buf = readString(&name,buf);
	    break;
	  case DT_ImplementationVersion:
	    buf = readString(&version,buf);
	    break;
	  case DT_ReferenceID:
	    buf = readAny(&refID,buf);
	    break;
	  default:
	    freeBitMap(versionBM);
	    freeBitMap(optionsBM);
	    s_free(auth);
	    s_free(id);
	    s_free(name);
	    s_free(version);
	    freeAny(refID);
	    REPORT_READ_ERROR(buf);
	    break;
	  }
    }

  buf = readInitInfo(&userInfo,buf);
  s_free(userInfo);
  if (buf == NULL)
    { freeBitMap(versionBM);
      freeBitMap(optionsBM);
      s_free(auth);
      s_free(id);
      s_free(name);
      s_free(version);
      freeAny(refID);
    }
  RETURN_ON_NULL(buf);

  /* construct the basic init object */
  *init = makeInitAPDU(search,present,delete,accessControl,resourceControl,
                       prefSize,maxMsgSize,auth,id,name,version,refID,userInfo);
                          
  freeBitMap(versionBM);
  freeBitMap(optionsBM);
  s_free(auth);
  s_free(id);
  s_free(name);
  s_free(version);
  freeAny(refID);
  
  return(buf);
}

/*----------------------------------------------------------------------*/

InitResponseAPDU* 
makeInitResponseAPDU(result,
		     search,
		     present,
		     deleteIt,
		     accessControl,
		     resourceControl,
		     prefSize,
		     maxMsgSize,
		     auth,
		     id,
		     name,
		     version,
		     refID,
		     userInfo)
boolean result;
boolean search;
boolean present;
boolean deleteIt;
boolean accessControl;
boolean resourceControl;
long prefSize;
long maxMsgSize;
char* auth;
char* id;
char* name;
char* version;
any* refID;
void* userInfo;
/* build an initResponse APDU with user specified information */
{ 
  InitResponseAPDU* init = (InitResponseAPDU*)s_malloc((size_t)sizeof(InitResponseAPDU));

  init->PDUType = initResponseAPDU;
  init->Result = result;
  init->willSearch = search;
  init->willPresent = present;
  init->willDelete = deleteIt;
  init->supportAccessControl = accessControl;
  init->supportResourceControl = resourceControl;
  init->PreferredMessageSize = prefSize;
  init->MaximumRecordSize = maxMsgSize;
  init->IDAuthentication = s_strdup(auth);
  init->ImplementationID = s_strdup(id);
  init->ImplementationName = s_strdup(name);
  init->ImplementationVersion = s_strdup(version);
  init->ReferenceID = duplicateAny(refID);
  init->UserInformationField = userInfo; /* not copied! */
  
  return(init);
}

/*----------------------------------------------------------------------*/

void 
freeInitResponseAPDU(init)
InitResponseAPDU* init;
/* free an initAPDU */
{
  s_free(init->IDAuthentication);
  s_free(init->ImplementationID);
  s_free(init->ImplementationName);
  s_free(init->ImplementationVersion);
  freeAny(init->ReferenceID);
  s_free(init);
}

/*----------------------------------------------------------------------*/

char* 
writeInitResponseAPDU(init,buffer,len)
InitResponseAPDU* init;
char* buffer;
long* len;
/* write the initResponse to a buffer, adding system information */
{ 
  char* buf = buffer + HEADER_LEN; /* leave room for the header-length-indicator */
  long size;
  bit_map* optionsBM = NULL;

  RESERVE_SPACE_FOR_HEADER(len);
  
  buf = writePDUType(init->PDUType,buf,len);
  buf = writeBoolean(init->Result,buf,len);
  buf = writeProtocolVersion(buf,len);
  
  optionsBM = makeBitMap((unsigned long)5,init->willSearch,init->willPresent,
                         init->willDelete,init->supportAccessControl,
                         init->supportResourceControl);
  buf = writeBitMap(optionsBM,DT_Options,buf,len);
  freeBitMap(optionsBM);

  buf = writeNum(init->PreferredMessageSize,DT_PreferredMessageSize,buf,len);
  buf = writeNum(init->MaximumRecordSize,DT_MaximumRecordSize,buf,len);
  buf = writeString(init->IDAuthentication,DT_IDAuthentication,buf,len);
  buf = writeString(init->ImplementationID,DT_ImplementationID,buf,len);
  buf = writeString(init->ImplementationName,DT_ImplementationName,buf,len);
  buf = writeString(init->ImplementationVersion,DT_ImplementationVersion,buf,len);
  buf = writeAny(init->ReferenceID,DT_ReferenceID,buf,len);
  
  /* go back and write the header-length-indicator */
  RELEASE_HEADER_SPACE(len);
  size = buf - buffer - HEADER_LEN; 
  writeBinaryInteger(size,HEADER_LEN,buffer,len);

  if (init->UserInformationField != NULL)
    buf = writeInitResponseInfo(init,buf,len);   
    
  return(buf);
}

/*----------------------------------------------------------------------*/

char* 
readInitResponseAPDU(init,buffer)
InitResponseAPDU** init;
char* buffer;
{
  char* buf = buffer;
  boolean search,present,delete,accessControl,resourceControl;
  long prefSize,maxMsgSize;
  char *auth,*id,*name,*version;
  long size; 
  pdu_type pduType;
  bit_map* versionBM = NULL;
  bit_map* optionsBM = NULL;
  boolean result;
  any *refID = NULL;
  void* userInfo = NULL;
  
  auth = id = name = version = NULL;
  refID = NULL;
  
  /* read required part */
  buf = readBinaryInteger(&size,HEADER_LEN,buf); 
  buf = readPDUType(&pduType,buf);
  buf = readBoolean(&result,buf);
  buf = readBitMap(&versionBM,buf); 
  buf = readBitMap(&optionsBM,buf);
  buf = readNum(&prefSize,buf);
  buf = readNum(&maxMsgSize,buf);
  
  /* decode optionsBM */
  search = bitAtPos(0L,optionsBM);
  present = bitAtPos(1L,optionsBM);
  delete = bitAtPos(2L,optionsBM);
  accessControl = bitAtPos(3L,optionsBM);
  resourceControl = bitAtPos(4L,optionsBM);
  
  /* read optional part */
  while (buf < (buffer + size + HEADER_LEN)) 
    { data_tag tag = peekTag(buf);
      switch (tag)
	{ case DT_IDAuthentication:
	    buf = readString(&auth,buf);
	    break;
	  case DT_ImplementationID:
	    buf = readString(&id,buf);
	    break;
	  case DT_ImplementationName:
	    buf = readString(&name,buf);
	    break;
	  case DT_ImplementationVersion:
	    buf = readString(&version,buf);
	    break;
	  case DT_ReferenceID:
	    buf = readAny(&refID,buf);
	    break;
	  default:
	    freeBitMap(versionBM);
	    freeBitMap(optionsBM);
	    s_free(auth);
	    s_free(id);
	    s_free(name);
	    s_free(version);
	    freeAny(refID);
	    REPORT_READ_ERROR(buf);
	    break;
	  }
    }

  buf = readInitResponseInfo(&userInfo,buf);
  if (buf == NULL)
    { freeBitMap(versionBM);
      freeBitMap(optionsBM);
      s_free(auth);
      s_free(id);
      s_free(name);
      s_free(version);
      freeAny(refID);
    }
  RETURN_ON_NULL(buf);
  
  /* construct the basic init object */
  *init = makeInitResponseAPDU(result,
			       search,present,delete,accessControl,resourceControl,
			       prefSize,maxMsgSize,auth,id,name,version,refID,userInfo);
			 	 			        	
  freeBitMap(versionBM);
  freeBitMap(optionsBM);
  s_free(auth);
  s_free(id);
  s_free(name);
  s_free(version);
  freeAny(refID);
  
  return(buf);
}

/*----------------------------------------------------------------------*/

InitResponseAPDU* 
replyToInitAPDU(init,result,userInfo)
InitAPDU* init;
boolean result;
void* userInfo;
/* respond to an init message in the default way - echoing back
   the init info 
 */
{
  InitResponseAPDU* initResp;
  initResp = makeInitResponseAPDU(result,
				  init->willSearch,init->willPresent,init->willDelete,
				  init->supportAccessControl,init->supportResourceControl,
				  init->PreferredMessageSize,init->MaximumRecordSize,
				  init->IDAuthentication,defaultImplementationID(),defaultImplementationName(),
				  defaultImplementationVersion(),
				  init->ReferenceID,userInfo);
  return(initResp);
}

/*----------------------------------------------------------------------*/

SearchAPDU* 
makeSearchAPDU(small,
	       large,
	       medium,
	       replace,
	       name,
	       databases,
	       type,
	       elements,
	       refID,
	       queryInfo)
long small;
long large;
long medium;
boolean replace;
char* name;
char** databases;
char* type;
char** elements;
any* refID;
void* queryInfo;
{
  char* ptr = NULL;
  long i;
  SearchAPDU* query = (SearchAPDU*)s_malloc((size_t)sizeof(SearchAPDU));
  query->PDUType = searchAPDU;
  query->SmallSetUpperBound = small;
  query->LargeSetLowerBound = large;
  query->MediumSetPresentNumber = medium;
  query->ReplaceIndicator = replace;
  query->ResultSetName = s_strdup(name);
  query->DatabaseNames = NULL; 
  if (databases != NULL)
    { for (i = 0, ptr = databases[i]; ptr != NULL; ptr = databases[++i])
	{ if (query->DatabaseNames == NULL)
	    query->DatabaseNames = (char**)s_malloc((size_t)(sizeof(char*) * 2));
        else
          query->DatabaseNames = (char**)s_realloc((char*)query->DatabaseNames,
						   (size_t)(sizeof(char*) * (i + 2)));
	    query->DatabaseNames[i] = s_strdup(ptr);
	    query->DatabaseNames[i+1] = NULL;
	  }
      }
  query->QueryType = s_strdup(type);
  query->ElementSetNames = NULL; 
  if (elements != NULL)
    { for (i = 0, ptr = elements[i]; ptr != NULL; ptr = elements[++i])
	{ if (query->ElementSetNames == NULL)
	    query->ElementSetNames = (char**)s_malloc((size_t)(sizeof(char*) * 2));
        else
          query->ElementSetNames = (char**)s_realloc((char*)query->ElementSetNames,
						     (size_t)(sizeof(char*) * (i + 2)));
	    query->ElementSetNames[i] = s_strdup(ptr);
	    query->ElementSetNames[i+1] = NULL;
	  }
      }
  query->ReferenceID = duplicateAny(refID);
  query->Query = queryInfo;	/* not copied! */
  return(query);
}

/*----------------------------------------------------------------------*/

void 
freeSearchAPDU(query)
SearchAPDU* query;
{
  s_free(query->ResultSetName);
  s_free(query->QueryType);
  doList((void**)query->DatabaseNames,fs_free); /* can't use the macro here ! */
  s_free(query->DatabaseNames);
  doList((void**)query->ElementSetNames,fs_free); /* can't use the macro here ! */
  s_free(query->ElementSetNames);
  freeAny(query->ReferenceID);
  s_free(query);
}

/*----------------------------------------------------------------------*/

#define DB_DELIMITER 	"\037" 	/* hex 1F occurs between each database name */
#define ES_DELIMITER_1 	"\037" 	/* separates database name from element name */
#define ES_DELIMITER_2 	"\036" 	/* hex 1E separates <db,es> groups from one another */

char* 
writeSearchAPDU(query,buffer,len)
SearchAPDU* query;
char* buffer;
long* len;
{ 
  char* buf = buffer + HEADER_LEN; /* leave room for the header-length-indicator */
  long size,i;
  char* ptr = NULL;
  char* scratch = NULL;

  RESERVE_SPACE_FOR_HEADER(len);
  
  buf = writePDUType(query->PDUType,buf,len);
  buf = writeBinaryInteger(query->SmallSetUpperBound,(size_t)3,buf,len);
  buf = writeBinaryInteger(query->LargeSetLowerBound,(size_t)3,buf,len);
  buf = writeBinaryInteger(query->MediumSetPresentNumber,(size_t)3,buf,len);
  buf = writeBoolean(query->ReplaceIndicator,buf,len);
  buf = writeString(query->ResultSetName,DT_ResultSetName,buf,len);
  /* write database names */
  if (query->DatabaseNames != NULL)
    { for (i = 0,scratch = NULL, ptr = query->DatabaseNames[i]; ptr != NULL; ptr = query->DatabaseNames[++i])
	{ if (scratch == NULL)
	    scratch = s_strdup(ptr);
        else
	  { size_t newScratchSize = (size_t)(strlen(scratch) + strlen(ptr) + 3);
	    scratch = (char*)s_realloc(scratch,newScratchSize);
	    s_strncat(scratch,DB_DELIMITER,2L,newScratchSize);
	    s_strncat(scratch,ptr,strlen(ptr) + 1,newScratchSize);
	  }
	  }
	buf = writeString(scratch,DT_DatabaseNames,buf,len);
	s_free(scratch);
      }
  buf = writeString(query->QueryType,DT_QueryType,buf,len);
  /* write element set names */
  if (query->ElementSetNames != NULL)
    { for (i = 0,scratch = NULL, ptr = query->ElementSetNames[i]; ptr != NULL; ptr = query->ElementSetNames[++i])
	{ if (scratch == NULL)
	    { if (query->ElementSetNames[i+1] == NULL) /* there is a single element set name */
		{ scratch = (char*)s_malloc((size_t)strlen(ptr) + 3);
		  strncpy(scratch,ES_DELIMITER_1,2);
		  s_strncat(scratch,ptr,strlen(ptr) + 1,strlen(ptr) + 2);
		}
	    else		/* this is the first of a series of element set names */
	      { size_t newScratchSize = (size_t)(strlen(ptr) + strlen(query->ElementSetNames[i + 1]) + 3);
		scratch = s_strdup(ptr); /* the database name */
		ptr = query->ElementSetNames[++i]; /* the element set name */
		scratch = (char*)s_realloc(scratch,newScratchSize);
		s_strncat(scratch,ES_DELIMITER_1,2L,newScratchSize);
		s_strncat(scratch,ptr,strlen(ptr) + 1,newScratchSize); 
	      }
	      }
        else
	  { char* esPtr = query->ElementSetNames[++i]; /* the element set name */
	    size_t newScratchSize = (size_t)(strlen(scratch) + strlen(ptr) + strlen(esPtr) + 3);
	    scratch = (char*)s_realloc(scratch,newScratchSize);
	    s_strncat(scratch,ES_DELIMITER_2,2L,newScratchSize);
	    s_strncat(scratch,ptr,strlen(ptr) + 1,newScratchSize);
	    s_strncat(scratch,ES_DELIMITER_1,2L,newScratchSize);
	    s_strncat(scratch,esPtr,strlen(esPtr) + 1,newScratchSize); 
	  }
	  }
	buf = writeString(scratch,DT_ElementSetNames,buf,len);
	s_free(scratch);
      }						
  buf = writeAny(query->ReferenceID,DT_ReferenceID,buf,len);
    
  /* go back and write the header-length-indicator */
  RELEASE_HEADER_SPACE(len);
  size = buf - buffer - HEADER_LEN; 
  writeBinaryInteger(size,HEADER_LEN,buffer,len);

  if (query->Query != NULL)
    buf = writeSearchInfo(query,buf,len);    
    
  return(buf);
}

/*----------------------------------------------------------------------*/

char*
readSearchAPDU(query,buffer)
SearchAPDU** query;
char* buffer;
{
  char* buf = buffer;
  long size;
  pdu_type pduType;
  long small,large,medium,numItems;
  boolean replace;
  char *name,*databaseNames,*type,*elements;
  char **dbList,**elemList;
  any *refID;
  void* userInfo = NULL;
  
  name = databaseNames = type = elements = NULL;
  dbList = elemList = NULL;
  refID = NULL;

  /* read required part */
  buf = readBinaryInteger(&size,HEADER_LEN,buf); 
  buf = readPDUType(&pduType,buf);
  buf = readBinaryInteger(&small,(size_t)3,buf);
  buf = readBinaryInteger(&large,(size_t)3,buf);
  buf = readBinaryInteger(&medium,(size_t)3,buf);
  buf = readBoolean(&replace,buf);
  
  /* read optional part */
  while (buf < (buffer + size + HEADER_LEN)) 
    { data_tag tag = peekTag(buf);
      switch (tag)
	{ case DT_ResultSetName:
	    buf = readString(&name,buf);
	    break;
	  case DT_DatabaseNames:
	    {			/* parse the database names */
	      char *tok;
	      buf = readString(&databaseNames,buf);
	      tok = strtok(databaseNames,DB_DELIMITER);
	      numItems = 0;
	      while (tok != NULL)
		{ if (dbList == NULL)
		    dbList = (char**)s_malloc((size_t)(sizeof(char*) * 2));
		else
		  dbList = (char**)s_realloc((char*)dbList,(size_t)(sizeof(char*) * (numItems+2)));
		    dbList[numItems++] = s_strdup(tok);
		    dbList[numItems] = NULL;
		    tok = strtok(NULL,DB_DELIMITER);
		  }
	    }
	    break;
	  case DT_QueryType:
	    buf = readString(&type,buf);
	    break;
	  case DT_ElementSetNames:
	    {			/* parse the element set names */
	      char *tok;
	      buf = readString(&elements,buf);
	      if (elements[0] == ES_DELIMITER_1[0]) /* this is a single element w/ no db */
		{ elemList = (char**)s_malloc((size_t)(sizeof(char*) * 2));
		  elemList[0] = s_strdup(elements);
		  elemList[1] = NULL;
		}
	      else		/* its a group of <DB/ES> pairs */
		{ char* esTok = NULL;
		  tok = strtok(elements,ES_DELIMITER_1); /* db name */
		  esTok = strtok(NULL,ES_DELIMITER_2); /* es name */
		  numItems = 0;
		  while (tok != NULL)
		    { if (elemList == NULL)
			elemList = (char**)s_malloc((size_t)(sizeof(char*) * 3));
		    else
		      elemList = (char**)s_realloc((char*)elemList,(size_t)(sizeof(char*) * (numItems * 2 + 1)));
			elemList[numItems++] = s_strdup(tok);
			elemList[numItems++] = s_strdup(esTok);
			elemList[numItems] = NULL;
			tok = strtok(NULL,ES_DELIMITER_1);
			esTok = strtok(NULL,ES_DELIMITER_2);
		      } 
		}
	    }
	    break;
	  case DT_ReferenceID:
	    buf = readAny(&refID,buf);
	    break;
	  default:
	    s_free(name);
	    s_free(type);
	    s_free(databaseNames);
	    doList((void**)dbList,fs_free);
	    s_free(dbList);
	    s_free(elements);
	    doList((void**)elemList,fs_free);
	    s_free(elemList);
	    freeAny(refID);
	    REPORT_READ_ERROR(buf);
	    break;
	  }
    }
  
  buf = readSearchInfo(&userInfo,buf);
  if (buf == NULL)
    { s_free(name);
      s_free(type);
      s_free(databaseNames);
      doList((void**)dbList,fs_free);
      s_free(dbList);
      s_free(elements);
      doList((void**)elemList,fs_free);
      s_free(elemList);
      freeAny(refID);
    }
  RETURN_ON_NULL(buf);
    
  /* construct the search object */
  *query = makeSearchAPDU(small,large,medium,replace,name,dbList,type,elemList,refID,userInfo);

  s_free(name);
  s_free(type);
  s_free(databaseNames);
  doList((void**)dbList,fs_free);
  s_free(dbList);
  s_free(elements);
  doList((void**)elemList,fs_free);
  s_free(elemList);
  freeAny(refID);
  
  return(buf);
}

/*----------------------------------------------------------------------*/

SearchResponseAPDU* 
makeSearchResponseAPDU(result,count,recordsReturned,nextPos,resultStatus,
		       presentStatus,refID,records)
long result;
long count;
long recordsReturned;
long nextPos;
long resultStatus;
long presentStatus;
any* refID;
void* records;
{
  SearchResponseAPDU* query = (SearchResponseAPDU*)s_malloc((size_t)sizeof(SearchResponseAPDU));
  query->PDUType = searchResponseAPDU;
  query->SearchStatus = result;
  query->ResultCount = count;
  query->NumberOfRecordsReturned = recordsReturned;
  query->NextResultSetPosition = nextPos;
  query->ResultSetStatus = resultStatus;
  query->PresentStatus = presentStatus;
  query->ReferenceID = duplicateAny(refID);
  query->DatabaseDiagnosticRecords = records;
  return(query);  
}

/*----------------------------------------------------------------------*/

void 
freeSearchResponseAPDU(queryResponse)
SearchResponseAPDU* queryResponse;
{
  freeAny(queryResponse->ReferenceID);
  s_free(queryResponse);
}

/*----------------------------------------------------------------------*/

char* 
writeSearchResponseAPDU(queryResponse,buffer,len)
SearchResponseAPDU* queryResponse;
char* buffer;
long* len;
{
  char* buf = buffer + HEADER_LEN; /* leave room for the header-length-indicator */
  long size;

  RESERVE_SPACE_FOR_HEADER(len);
  
  buf = writePDUType(queryResponse->PDUType,buf,len);
  buf = writeBinaryInteger(queryResponse->SearchStatus,(size_t)1,buf,len);
  buf = writeBinaryInteger(queryResponse->ResultCount,(size_t)3,buf,len);
  buf = writeBinaryInteger(queryResponse->NumberOfRecordsReturned,(size_t)3,buf,len);
  buf = writeBinaryInteger(queryResponse->NextResultSetPosition,(size_t)3,buf,len);
  buf = writeNum(queryResponse->ResultSetStatus,DT_ResultSetStatus,buf,len);
  buf = writeNum(queryResponse->PresentStatus,DT_PresentStatus,buf,len);
  buf = writeAny(queryResponse->ReferenceID,DT_ReferenceID,buf,len);
    
  /* go back and write the header-length-indicator */
  RELEASE_HEADER_SPACE(len);
  size = buf - buffer - HEADER_LEN; 
  writeBinaryInteger(size,HEADER_LEN,buffer,len);

  if (queryResponse->DatabaseDiagnosticRecords != NULL)
    buf = writeSearchResponseInfo(queryResponse,buf,len);    
    
  return(buf);
}

/*----------------------------------------------------------------------*/

char* 
readSearchResponseAPDU(queryResponse,buffer)
SearchResponseAPDU** queryResponse;
char* buffer;
{
  char* buf = buffer;
  long size;
  pdu_type pduType;
  long result,count,recordsReturned,nextPos;
  long resultStatus,presentStatus;
  any *refID = NULL;
  void* userInfo = NULL;
  
  /* read required part */
  buf = readBinaryInteger(&size,HEADER_LEN,buf); 
  buf = readPDUType(&pduType,buf);
  buf = readBinaryInteger(&result,(size_t)1,buf);
  buf = readBinaryInteger(&count,(size_t)3,buf);
  buf = readBinaryInteger(&recordsReturned,(size_t)3,buf);
  buf = readBinaryInteger(&nextPos,(size_t)3,buf);
  
  resultStatus = presentStatus = UNUSED;
  refID = NULL;

  /* read optional part */
  while (buf < (buffer + size + HEADER_LEN)) 
    { data_tag tag = peekTag(buf);
      switch (tag)
	{ case DT_ResultSetStatus:
	    buf = readNum(&resultStatus,buf);
	    break;
	  case DT_PresentStatus:
	    buf = readNum(&presentStatus,buf);
	    break;
	  case DT_ReferenceID:
	    buf = readAny(&refID,buf);
	    break;
	  default:
	    freeAny(refID);
	    REPORT_READ_ERROR(buf);
	    break;
	  }
    }
  
  buf = readSearchResponseInfo(&userInfo,buf);
  if (buf == NULL)
    freeAny(refID);
  RETURN_ON_NULL(buf);
  
  /* construct the search object */
  *queryResponse = makeSearchResponseAPDU(result,count,recordsReturned,nextPos,
					  (long)resultStatus,(long)presentStatus,refID,userInfo);

  freeAny(refID);
  
  return(buf);
}

#ifdef NOTUSED
/*----------------------------------------------------------------------*/


PresentAPDU* 
makePresentAPDU(recsReq,startPos,resultID,refID,info)
long recsReq;
long startPos;
char* resultID;
any* refID;
void* info;
{
  PresentAPDU* present = (PresentAPDU*)s_malloc((size_t)sizeof(PresentAPDU));
  present->PDUType = presentAPDU;
  present->NumberOfRecordsRequested = recsReq;
  present->ResultSetStartPosition = startPos;
  present->ResultSetID = s_strdup(resultID);
  present->ElementSetNames = NULL; /* XXX for now */
  present->ReferenceID = duplicateAny(refID);
  present->PresentInfo = info;
  return(present);  
}

/*----------------------------------------------------------------------*/

void 
freePresentAPDU(present)
PresentAPDU* present;
{
  /* handle element set names */ 
  s_free(present->ResultSetID);
  freeAny(present->ReferenceID);
  s_free(present);
}

/*----------------------------------------------------------------------*/

char* 
writePresentAPDU(present,buffer,len)
PresentAPDU* present;
char* buffer;
long* len;
{
  char* buf = buffer + HEADER_LEN; /* leave room for the header-length-indicator */
  long size;

  RESERVE_SPACE_FOR_HEADER(len);
  
  buf = writePDUType(present->PDUType,buf,len);
  buf = writeBinaryInteger(present->NumberOfRecordsRequested,(size_t)3,buf,len);
  buf = writeBinaryInteger(present->ResultSetStartPosition,(size_t)3,buf,len);
  buf = writeString(present->ResultSetID,DT_ResultSetID,buf,len); 
  /* handle element set names */
  buf = writeAny(present->ReferenceID,DT_ReferenceID,buf,len);
    
  /* go back and write the header-length-indicator */
  RELEASE_HEADER_SPACE(len);
  size = buf - buffer - HEADER_LEN; 
  writeBinaryInteger(size,HEADER_LEN,buffer,len);

  if (present->PresentInfo != NULL)
    buf = writePresentInfo(present,buf,len);   
    
  return(buf); 
}

/*----------------------------------------------------------------------*/

char* 
readPresentAPDU(present,buffer)
PresentAPDU** present;
char* buffer;
{
  char* buf = buffer;
  long size;
  pdu_type pduType;
  long recsReq,startPos;
  char* resultID = NULL;
  any *refID = NULL;
  void* userInfo = NULL;
  
  /* read required part */
  buf = readBinaryInteger(&size,HEADER_LEN,buf); 
  buf = readPDUType(&pduType,buf);
  buf = readBinaryInteger(&recsReq,(size_t)3,buf);
  buf = readBinaryInteger(&startPos,(size_t)3,buf);
  buf = readString(&resultID,buf);
  
  /* read optional part */
  while (buf < (buffer + size + HEADER_LEN)) 
   { data_tag tag = peekTag(buf);
     switch (tag)
      { case DT_ReferenceID:
          buf = readAny(&refID,buf);
          break;
        default:
          s_free(resultID);
          freeAny(refID);
          REPORT_READ_ERROR(buf);
          break;
      }
   }
  
  buf = readPresentInfo(&userInfo,buf);
  if (buf == NULL)
   { s_free(resultID);
     freeAny(refID);
   }
  RETURN_ON_NULL(buf);

  /* construct the present object */
  *present = makePresentAPDU(recsReq,startPos,resultID,refID,userInfo);

  s_free(resultID);
  freeAny(refID);
  
  return(buf);
}

/*----------------------------------------------------------------------*/


PresentResponseAPDU* 
makePresentResponseAPDU(status,recsRet,nextPos,refID,records)
boolean status;
long recsRet;
long nextPos;
any* refID;
void* records;
{
  PresentResponseAPDU* present = (PresentResponseAPDU*)s_malloc((size_t)sizeof(PresentResponseAPDU));
  present->PDUType = presentResponseAPDU;
  present->PresentStatus = status;
  present->NumberOfRecordsReturned = recsRet;
  present->NextResultSetPosition = nextPos;
  present->ReferenceID = duplicateAny(refID);
  present->DatabaseDiagnosticRecords = records;
  return(present);  
}

/*----------------------------------------------------------------------*/

void 
freePresentResponseAPDU(present)
PresentResponseAPDU* present;
{ 
  freeAny(present->ReferenceID);
  s_free(present);
}

/*----------------------------------------------------------------------*/

char* 
writePresentResponseAPDU(present,buffer,len)
PresentResponseAPDU* present;
char* buffer;
long* len;
{
  char* buf = buffer + HEADER_LEN; /* leave room for the header-length-indicator */
  long size;

  RESERVE_SPACE_FOR_HEADER(len);
  
  buf = writePDUType(present->PDUType,buf,len);
  buf = writeBoolean(present->PresentStatus,buf,len);
  buf = writeBinaryInteger(present->NumberOfRecordsReturned,(size_t)3,buf,len);
  buf = writeBinaryInteger(present->NextResultSetPosition,(size_t)3,buf,len);
  buf = writeAny(present->ReferenceID,DT_ReferenceID,buf,len);
    
  /* go back and write the header-length-indicator */
  RELEASE_HEADER_SPACE(len);
  size = buf - buffer - HEADER_LEN; 
  writeBinaryInteger(size,HEADER_LEN,buffer,len);

  if (present->DatabaseDiagnosticRecords != NULL)
    buf = writePresentResponseInfo(present,buf,len);
    
  return(buf);
}

/*----------------------------------------------------------------------*/

char* 
readPresentResponseAPDU(present,buffer)
PresentResponseAPDU** present;
char* buffer;
{
  char* buf = buffer;
  long size;
  pdu_type pduType;
  boolean status;
  long recsRet,nextPos;
  any *refID = NULL;
  void* userInfo = NULL;

  /* read required part */
  buf = readBinaryInteger(&size,HEADER_LEN,buf); 
  buf = readPDUType(&pduType,buf);
  buf = readBoolean(&status,buf);
  buf = readBinaryInteger(&recsRet,(size_t)3,buf);
  buf = readBinaryInteger(&nextPos,(size_t)3,buf);

  /* read optional part */
  while (buf < (buffer + size + HEADER_LEN)) 
   { data_tag tag = peekTag(buf);
     switch (tag)
      { case DT_ReferenceID:
          buf = readAny(&refID,buf);
          break;
        default:
          freeAny(refID);
          REPORT_READ_ERROR(buf);
          break;
      }
   }
  
  buf = readPresentResponseInfo(&userInfo,buf);
  if (buf == NULL)
    freeAny(refID);
  RETURN_ON_NULL(buf);

  /* construct the presentResponse object */
  *present = makePresentResponseAPDU(status,recsRet,nextPos,refID,userInfo);

  freeAny(refID);
  
  return(buf);
}

/*----------------------------------------------------------------------*/


#endif /* def NOTUSED */
