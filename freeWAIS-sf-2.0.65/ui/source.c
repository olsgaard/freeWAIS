/* 
  WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface tools for the WAIS software.
   Do with it as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: source.c,v $
 * Revision 2.0.1.2  1995/09/15 09:53:54  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.1  1995/09/11  13:01:12  pfeifer
 * patch4: Changed TELL_USER to call getuser_name().
 *
 * Revision 2.0  1995/09/08  08:05:19  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.7  1994/08/22  14:13:31  pfeifer
 * jp made the getenv code safer
 *
 * Revision 1.6  1994/08/15  16:17:05  pfeifer
 * fixed uncheckt usage of getenv("USER") in printf
 *
 * Revision 1.5  1994/08/05  07:30:16  pfeifer
 * Release beta 04
 *
 * Revision 1.4  93/07/02  19:27:26  warnock
 * fix call to getdomainname from mike@tab00.larc.nasa.gov
 * 
 * Revision 1.3  93/07/01  19:21:26  warnock
 * gethostname -> mygethostname
 * 
 * Revision 1.2  93/06/23  20:06:03  warnock
 * Removed FORWARDER_SERVER pointer to Francois
 * 
 * Revision 1.1  93/06/23  20:02:20  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.16  92/04/02  14:22:23  jonathan
 * Added sopysourceID.
 * 
 * Revision 1.15  92/04/01  17:18:39  jonathan
 * Added FORWARDER_SERVER stuff.
 * 
 * Revision 1.14  92/03/17  14:39:55  jonathan
 * Cleaned up.
 * 
 * Revision 1.13  92/03/06  14:50:53  jonathan
 * New and Improved source loading!
 * 
 * Revision 1.12  92/03/05  11:47:30  jonathan
 * Replaced directory routines with scandir's.
 * 
 * Revision 1.11  92/03/04  19:21:11  jonathan
 * Handle EOF on read of source.  This fixes looping when source description
 * is broken.
 * 
 * Revision 1.10  92/03/04  18:45:17  jonathan
 * Modified WriteSource to use WriteString for description.
 * 
 * Revision 1.9  92/03/01  13:53:30  jonathan
 * Added routines for X interface in effort to combine x/source.c and
 * ui/source.c
 * 
 * Revision 1.8  92/03/01  13:07:25  jonathan
 * Added check of s->thisSource to avoid errors when there are no sources.
 * 
 */

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ui/RCS/source.c,v 2.0.1.2 1995/09/15 09:53:54 pfeifer Exp $";
#endif

#define _C_SOURCE
/* (jp) */
#define EXTERN_SOURCE_ITEMS
#define EXTERN_SOURCE_PATH

#include "wais.h"
#include "globals.h"

#define DESC_SIZE 65535

/*
#if defined(BSD) || defined(BSD43)
#include <strings.h>
#define strchr index
#endif
*/
#include "../ir/cdialect.h"
extern int alphasort();

void
freeSourceID(sid)
SourceID sid;
{
  if (sid != NULL) {
    if (sid->filename != NULL) s_free(sid->filename);
    s_free(sid);
  }
}

SourceID
copysourceID(sid)
SourceID sid;
{
  SourceID result = NULL;
  if(sid != NULL) {
    if((result = (SourceID) s_malloc(sizeof(_SourceID))) != NULL) {
      result->filename = s_strdup(sid->filename);
    }
  }
  return result;
}

char **
buildSourceItemList(sourcelist)
SourceList sourcelist;
{
  char **result;
  int num, i;
  SourceList source;

  /* find the length of the sidlist in the question */

  for(num = 0, source = sourcelist; 
      source != NULL && source->thisSource != NULL;
      num++, source = source->nextSource);

  result = (char**)s_malloc((1+num)*sizeof(char*));
  if(num > 0)
    for(i =0, source = sourcelist; i<num; i++, source = source->nextSource)
      result[i] = source->thisSource->filename;
  result[num] = NULL;
  return(result);
}

char **
buildSItemList(sourcelist)
SList sourcelist;
{
  char **result;
  int num, i;
  SList source;

  /* find the length of the sidlist in the question */

  for(num = 0, source = sourcelist; 
      source != NULL;
      num++, source = source->nextSource);

  result = (char**) s_malloc((1+num)*sizeof(char*));
  if(num > 0)
    for(i =0, source = sourcelist; i<num; i++, source = source->nextSource)
      if(source->thisSource != NULL)
	result[i] = source->thisSource->name;
  result[num] = NULL;
  return(result);
}

short
ReadSourceID(file, sid)
FILE *file;
SourceID sid;
{
  char temp_string[MAX_SYMBOL_SIZE];
  char filename[MAX_SYMBOL_SIZE];
  short check_result;

  check_result = CheckStartOfStruct("source-id", file);
  filename[0] = '\0';
  if(FALSE == check_result){ 
    return(false);
  }
  if(END_OF_STRUCT_OR_LIST == check_result)
    {
      return(FALSE);
    }
    
  /* read the slots: */
  while(TRUE){

    short check_result = ReadSymbol(temp_string, file, MAX_SYMBOL_SIZE);
    if(END_OF_STRUCT_OR_LIST == check_result) break;
    if(FALSE == check_result){
      return(false);
    } 
    if(0 == strcmp(temp_string, ":filename")) {
      if (FALSE == ReadString(filename, file, MAX_SYMBOL_SIZE))
	return(false);
      sid->filename = s_strdup(filename);
    }
    else
      SkipObject(file);
  }
  return(TRUE);
}

SourceList ReadListOfSources(file)
FILE *file;
{
  short check_result;
  SourceID sid = NULL;
  SourceList result, this, last;
          
  /* initialize */
  this = last = result = NULL;

  if(ReadStartOfList(file) == FALSE)
    return(NULL);

  while(TRUE) {
    sid = (SourceID)s_malloc(sizeof(_SourceID));
    check_result = ReadSourceID(file, sid);
    if(check_result == END_OF_STRUCT_OR_LIST) {
      s_free(sid);
      return(result);
    }
    else if(check_result == FALSE)
      return(result);

    else if(check_result == TRUE) {
      if(result == NULL)
	result = this = (SourceList)s_malloc(sizeof(_SourceList));
      else
	this = (SourceList)s_malloc(sizeof(_SourceList));
      this->thisSource = sid;
      if(last != NULL)
	last->nextSource = this;
      last = this;
    }
  }
}

Boolean ReadSource(source, file)
Source source;
FILE *file;
{
  char temp_string[MAX_SYMBOL_SIZE];
  char desc_string[DESC_SIZE];
  short check_result;
  long port;

  long version;

  /* make sure it's a Source */
  
  check_result = CheckStartOfStruct("source", file);
  if(FALSE == check_result){ 
    return(false);
  }
  if(END_OF_STRUCT_OR_LIST == check_result)
    {
      return(FALSE);
    }
    
  strcpy(source->server, "");
  strcpy(source->service, "");

  /* read the slots: */
  while(TRUE){

    short check_result = ReadSymbol(temp_string, file, MAX_SYMBOL_SIZE);
    if((END_OF_STRUCT_OR_LIST == check_result)  || 
       (EOF == check_result))
      break;
    if(FALSE == check_result){
      return(false);
    } 
    if(0 == strcmp(temp_string, ":version")) {
      if(FALSE == ReadLong(file, &version))
	return(false);
    }
    else if(0 == strcmp(temp_string, ":ip-name")) {
      if(FALSE == ReadString(temp_string, file, MAX_SYMBOL_SIZE))
	return(false);
      strcpy(source->server, temp_string);
    }
    else if(0 == strcmp(temp_string, ":ip-address")) {
      if(FALSE == ReadString(temp_string, file, MAX_SYMBOL_SIZE))
	return(false);
      strcpy(source->server, temp_string);
    }
    else if(0 == strcmp(temp_string, ":configuration")) {
      if(FALSE == ReadString(temp_string, file, MAX_SYMBOL_SIZE))
	return(false);
      find_value(temp_string, "IPAddress", source->server, STRINGSIZE);
      find_value(temp_string, "RemotePort", source->service, STRINGSIZE);
    }
    else if(0 == strcmp(temp_string, ":tcp-port")) {
      if(FALSE == ReadLong(file, &port))
	return(false);
      sprintf(source->service,"%d", port);
    }
    else if(0 == strcmp(temp_string, ":port")) {
      if(FALSE == ReadLong(file, &port))
	return(false);
      sprintf(source->service,"%d", port);
    }
    else if(0 == strcmp(temp_string, ":maintainer")) {
      if(FALSE == ReadString(temp_string, file, MAX_SYMBOL_SIZE))
	return(false);
      if(source->maintainer != NULL) s_free(source->maintainer);
      source->maintainer = s_strdup(temp_string);
    }
    else if(0 == strcmp(temp_string, ":database-name")) {
      if(FALSE == ReadString(temp_string, file, MAX_SYMBOL_SIZE))
	return(false);
      strcpy(source->database, temp_string);
    }
    else if(0 == strcmp(temp_string, ":cost")) {
      double cost;
      if(FALSE == ReadDouble(file, &cost))
	return(false);
      sprintf(source->cost, "%.2f", cost);
    }
    else if(0 == strcmp(temp_string, ":cost-unit")) {
      if(FALSE == ReadSymbol(temp_string, file, MAX_SYMBOL_SIZE))
	return(false);
      strcpy(source->units, temp_string);
    }
    else if(0 == strcmp(temp_string, ":description")) {
      if(FALSE == ReadString(desc_string, file, DESC_SIZE))
	return(false);
      if(source->description != NULL) s_free(source->description);
      source->description = s_strdup(desc_string);
    }
    else if(0 == strcmp(temp_string, ":update-time")) {
      if(EOF == SkipObject(file)) break;
    }
    else
      if(EOF == SkipObject(file)) break; /* we don't know the key, so we don't know how
			   to interpret the value, skip it */
  }

  return(TRUE);
}

Boolean ReadSourceFile(asource, filename, directory)
Source asource;
char *filename, *directory;
{
  FILE *fp;
  char pathname[MAX_FILENAME_LEN+1];
  Boolean result;

  strncpy(pathname, directory, MAX_FILENAME_LEN);
  strncat(pathname, filename, MAX_FILENAME_LEN);

  if((fp = fopen(pathname, "r")) == NULL)
    return FALSE;

  asource->name = s_strdup(filename);
  asource->directory = s_strdup(directory);

  result = ReadSource(asource, fp);
  fclose(fp);
  return(result);
}

Source
loadSource(name, sourcepath)
char *name;
char *sourcepath;
{
  char *i, *p, source_dir[MAX_FILENAME_LEN], sp[1000];
  Source source = (Source)s_malloc(sizeof(_Source));
/*
#ifndef ANSI_LIKE
  extern char *getenv();
#endif
*/
  if(sourcepath == NULL || sourcepath[0] == 0) {
    if((sourcepath = (char*)getenv("WAISSOURCEPATH")) == NULL)
      return NULL;
  }

  for (p = sourcepath, i = (char*)strchr(p, ':');
       i != NULL;
       p = i+1) {
    if((i = (char*)strchr(p, ':')) == NULL)
      strcpy(source_dir, p);
    else {
      strncpy(source_dir, p, i-p);
      source_dir[i-p] = 0;
    }

    if(ReadSourceFile(source, name, source_dir)) {
      set_connection(source);
      Sources = makeSList(source, Sources);
      return (source);
    }
  }
  s_free(source);
  source = NULL;
  return (source);
}

void set_connection(source)
Source source;
{
  SList s;
  for(s = Sources; s != NULL; s = s->nextSource) {
    if(s->thisSource != NULL)
      if (!strcmp(source->server, s->thisSource->server) &&
	  !strcmp(source->service, s->thisSource->service)) {
	source->connection = s->thisSource->connection;
	source->buffer_length = s->thisSource->buffer_length;
	source->initp = s->thisSource->initp;
	break;
      }
  }
}

Boolean newSourcep(name)
char *name;
{
  SList s;

  for (s = Sources; s != NULL; s = s->nextSource)
    if((s->thisSource != NULL) &&
       !strcmp(name, s->thisSource->name))
      return FALSE;

  return TRUE;
}

Boolean is_source(name, test)
char *name;
Boolean test;
{
  char lastchar;

  lastchar = name[strlen(name)-1];
  if(test) 
    return ((strlen(name) > 4) &&
	  strstr(name, ".src") &&
	  (!strcmp(".src", strstr(name, ".src"))));
  else 
    return (lastchar != '~' &&
	    lastchar != '#' &&
	    strcmp(name, ".") &&
	    strcmp(name, ".."));
}

static Boolean newSource(name)
char *name;
{
  int i;

  for(i =0; i < NumSources; i++)
    if(!strcmp(name, Source_items[i]))
      return FALSE;

  return TRUE;
}

static int
issfile(dp)
Direntry_t *dp;
{
  return(is_source(dp->d_name, TRUE) &&
	 newSource(dp->d_name));
}

void SortSourceNames(n)
int n;
{
  Boolean Changed = TRUE;
  int i;
  char *qi;

  while(Changed) {
    Changed = FALSE;
    for(i = 0; i < n-1; i++)
      if(0 < strcasecmp(Source_items[i], Source_items[i+1])) {
	Changed = TRUE;
	qi = Source_items[i];
	Source_items[i] = Source_items[i+1];
	Source_items[i+1] = qi;
      }
  }
}

void
GetSourceNames(directory)
char *directory;
{
  Direntry_t **list;
  int i, j;

  if ((j = scandir(directory, &list, issfile, alphasort)) < 0) {
      PrintStatus(STATUS_INFO, STATUS_HIGH, "Error on open of source directory: %s.\n", directory);
      return;
    }

  if(NumSources > 0)
    Source_items = (char**) s_realloc(Source_items, (NumSources+j+1) * sizeof(char*));
  else {
    if(Source_items != NULL) {
      for (i = 0; Source_items[i] != NULL; i++) s_free(Source_items[i]);
      s_free(Source_items);
    }
    Source_items = (char**) s_malloc((j+1) * sizeof(char*));
  }

  for (i = 0; i < j; i++) {
    Source_items[i+NumSources] = s_strdup(list[i]->d_name);
    s_free(list[i]);
  }

  NumSources+=j;
  SortSourceNames(NumSources);
  Source_items[NumSources] = NULL;

  s_free(list);
}

/* read all the sources from a directory.  If test is true, only files ending
   in .src are valid
   */

void
ReadSourceDirectory(directory, test)
char *directory;
Boolean test;
{
  char filename[MAX_FILENAME_LEN];
  FILE *fp;
  int i, j , newNumSources;
  SList Last;
  Source source;
  Direntry_t **list;

  if ((j = scandir(directory, &list, NULL, NULL)) < 0) {
    return;
  }

  /* find the end of the sourcelist */
  if(Sources == NULL)
    Sources = makeSList(NULL, NULL);

  for(Last = Sources; Last->nextSource != NULL; Last = Last->nextSource);

  for (i = 0; i < j; i++) {
    if (is_source(list[i]->d_name, test)) {
      if(newSourcep(list[i]->d_name)) {
	strcpy(filename, directory);
	strcat(filename, list[i]->d_name);
	if ((fp = fopen(filename, "r")) != NULL) {
	  source = (Source)s_malloc(sizeof(_Source));
	  memset(source, 0, sizeof(_Source));
	  source->initp = FALSE;
	  source->name = s_strdup(list[i]->d_name);
	  source->directory = s_strdup(directory);
	  ReadSource(source, fp);
	  fclose(fp);
	  if(Last->thisSource == NULL)
	    Last->thisSource = source;
	  else {
	    Last->nextSource = makeSList(source, NULL);
	    Last = Last->nextSource;
	  }
	  NumSources++;
	}
      }
    }
  }
  free((char *)list);
}

void WriteSource(directory, source, overwrite)
     char *directory;
     Source source;
     Boolean overwrite;
{
  char filename[MAX_FILENAME_LEN];
  FILE *fp;
  
  /* build filename */
  
  strcpy(filename, directory);
  strcat(filename, source->name);
  
  /* test to see if it exists */
  
  if (overwrite == FALSE) 
    if ((fp = fopen(filename, "r")) != NULL) {
      PrintStatus(STATUS_INFO, STATUS_HIGH, 
		  "File %s exists, click again to overwrite.\n", filename);
      fclose(fp);
      return;
    }
  
  if ((fp = fopen(filename, "w")) == NULL) {
    PrintStatus(STATUS_INFO, STATUS_HIGH, "Error opening %s.\n", filename);
    return;
  }
  
  fprintf(fp, "(:source\n :version 3\n");
  if(source->server != NULL) 
    if(source->server[0] != 0)
      if(isdigit(source->server[0])) /* then it's an ip-address */
	fprintf(fp, " :ip-address \"%s\"\n", source->server);
      else
	fprintf(fp, " :ip-name \"%s\"\n", source->server);

  if(source->service != NULL) 
    if(source->service[0] != 0)
      fprintf(fp, " :tcp-port %s\n", source->service);

  fprintf(fp, " :database-name \"%s\"\n", source->database);
  if(source->cost != NULL) 
    if(source->cost[0] != 0)
      fprintf(fp, " :cost %s \n", source->cost);
  else
      fprintf(fp, " :cost 0.00 \n");

  if(source->units != NULL) 
    if(source->units[0] != 0)
      fprintf(fp, " :cost-unit %s \n", source->units);
  else
    fprintf(fp, " :cost-unit :free \n");
  
  if(source->maintainer != NULL) 
    if(source->maintainer[0] != 0)
      fprintf(fp, " :maintainer \"%s\"\n", 
	      source->maintainer);
  else
      fprintf(fp, " :maintainer \"%s\"\n", 
	      current_user_name());

  if(source->description != NULL) 
    if(source->description[0] != 0) {
      fprintf(fp, " :description ");
      WriteString(source->description, fp);
    }
  else
    fprintf(fp, " :description \"Created with %s by %s on %s.\"\n",
	    command_name, current_user_name(), printable_time());

  fprintf(fp, "\n)");
  fclose(fp);
}

SourceList
  makeSourceList(source, rest)
SourceID source;
SourceList rest;
{
  SourceList result;
  if((result = (SourceList)s_malloc(sizeof(_SourceList))) != NULL) {
    result->thisSource = source;
    result->nextSource = rest;
  }
  return(result);
}

SList
  makeSList(source, rest)
Source source;
SList rest;
{
  SList result;
  if((result = (SList)s_malloc(sizeof(_SList))) != NULL) {
    result->thisSource = source;
    result->nextSource = rest;
  }
  return(result);
}

void FreeSource(source)
Source source;
{
  if (source != NULL) {
    if(source->name != NULL)
      s_free (source->name);
    if(source->directory != NULL)
      s_free (source->directory);
    if(source->description != NULL)
      s_free (source->description);
    if(source->maintainer != NULL)
      s_free (source->maintainer);
    s_free(source);
  }
}

void FreeSources(sources)
SList sources;
{
 SList s, n;

 for (s = sources; s != NULL; s = n) {
   n = s->nextSource;
   FreeSource(s->thisSource);
   s_free(s);
 }

 NumSources = 0;
 sources = NULL;
}

Source
  findsource(name, sourcepath)
char* name;
char* sourcepath;
{
  SList asource;

  for(asource = Sources; 
      asource != NULL;
      asource = asource->nextSource) {
    if (!strcmp(name, asource->thisSource->name)) {
      return asource->thisSource;
    }
  }
  return (loadSource(name, sourcepath));
}

Source
  findSource(n)
int n;
{
  SList asource;

  for(asource = Sources; 
      (n > 0) && (asource != NULL);
      asource = asource->nextSource, n--);

  if (asource != NULL) return asource->thisSource;
  else return NULL;
}

void
format_source_cost(str,source)
char *str;
Source source;
{
  sprintf(str,"Free");
  if ((source->units != NULL) && (source->cost != NULL)) {
     
     if(0 == strcmp(source->units, ":dollars-per-query"))
        sprintf(str,"$%s/query",source->cost);

     if(0 == strcmp(source->units, ":dollars-per-minute"))
        sprintf(str,"$%s/minute",source->cost);

     if(0 == strcmp(source->units, ":dollars-per-retrieval"))
        sprintf(str,"$%s/retrieval",source->cost);
 
     if(0 == strcmp(source->units, ":dollars-per-session"))
        sprintf(str,"$%s/session",source->cost);
 
     if(0 == strcmp(source->units, ":other"))
        sprintf(str,"Special",source->cost);
  }
}

void
freeSourceList(slist)
SourceList slist;
{
  SourceList sl;
  while(slist != NULL) {
    sl = slist;
    freeSourceID(sl->thisSource);
    slist = sl->nextSource;
    s_free(sl);
  }
}

#include <sockets.h>

/* #define FORWARDER_SERVER "welchlab.welch.jhu.edu" */

Boolean init_for_source(source, request, length, response)
Source source;
char *request;
long length;
char *response;
/* send an init message to the source.  A side effect is that the 
   negotiation of buffer sizes.  The final size is put in 
   source->buffer_length 
 */
{
  char userInfo[500];
  char message[500];
  char hostname[80];
/*  char domain[80]; */
  char *domain;
  struct hostent *my_host_ent;

  mygethostname(hostname, 80);
/* Security patch from mike@tab00.larc.nasa.gov */
  if (((my_host_ent = gethostbyname(hostname)) == NULL) ||
      (domain = (char *)index(my_host_ent->h_name, '.')) == NULL)
    domain = "unknown";
  else
    domain++;

/*  getdomainname(domain, 80); */

#ifdef TELL_USER
  {
  char *env_user = getuser_env_name();
  sprintf(userInfo, "%s %s, from host: %s.%s, user: %s",
	  command_name, VERSION_STRING, hostname, domain, env_user);
  }
#else
  sprintf(userInfo, "%s %s, from host: %s.%s",
	  command_name, VERSION_STRING, hostname, domain);
#endif

  if(source->initp == FALSE) {
    if(source->server[0] == 0)
      source->connection = NULL;
    else {
      source->connection = connect_to_server(source->server,
					     atoi(source->service));
#ifdef FORWARDER_SERVER

#ifndef FORWARDER_SERVICE
#define FORWARDER_SERVICE "210"
#endif

      if(source->connection == NULL) {
	strncat(source->database, "@", STRINGSIZE);
	strncat(source->database, source->server, STRINGSIZE);
	strncat(source->database, ":", STRINGSIZE);
	strncat(source->database, source->service, STRINGSIZE);
	strncpy(source->server, FORWARDER_SERVER, STRINGSIZE);
	strncpy(source->service, FORWARDER_SERVICE, STRINGSIZE);
	source->connection = connect_to_server(source->server,
					       atoi(source->service));
      }
#endif

      if (source->connection == NULL) {
	PrintStatus(STATUS_URGENT, STATUS_HIGH, "Bad Connection to server.");
	source->initp = FALSE;
	return source->initp;
      }
    }
    source->buffer_length = 
      init_connection(request, response,
		      length,
		      source->connection,
		      userInfo);

    if (source->buffer_length < 0) {
      PrintStatus(STATUS_URGENT, STATUS_HIGH,
		  "\nError connecting to server: %s service: %s.",
		  source->server, source->service);
      source->initp = FALSE;
    }
    else {
      SList s;

      source->initp = TRUE;
      /* set the init and connection for other sources with the same
	 host and port. */
      for (s = Sources; s != NULL; s = s->nextSource) {
      	if (s->thisSource != source) {
	  if (strcmp(s->thisSource->server, source->server) == 0 &&
	      strcmp(s->thisSource->service, source->service) == 0) {
	    s->thisSource->connection = source->connection;
	    s->thisSource->buffer_length = source->buffer_length;
	    s->thisSource->initp = TRUE;
	  }
	}
      }
    }
    return source->initp;
  }
  return source->initp;
}
