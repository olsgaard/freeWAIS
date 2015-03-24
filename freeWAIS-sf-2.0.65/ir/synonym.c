
/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * 08/12/94  Changes to allow for caching of synonym tables in shared memory
 *           segments made by Alberto Accomazzi (alberto@cfa.harvard.edu).
 *           Two kinds of shared memory segments are used:
 *           a "master" one containing an array of structures describing
 *           what and how big the synonym tables cached in shared memory
 *           are, and the synonym table data itself.  The first one is
 *           created by the main server body, while the other tables are
 *           created, if necessary, when a new synonym table is opened.
 *           The synonym data as well as the synonym table (consisting
 *           of pointers to the data area) are now both stored in contiguous
 *           memory areas.  This allows us to copy them directly into 
 *           shared memory by just changing offsets of the pointers in
 *           the synonym table.
 *           Since the master shared memory area is created only by the
 *           server program, no caching takes place during indexing.
 *           Beware that the shared memory areas may grow large depending
 *           on the size of your synonym files.  The maximum number of
 *           cached synonym files is defined in "synonym.h" as
 *           MAX_SYN_CACHE.
 */

#include "synonym.h"
#include "cutil.h"
#include <errno.h>
#ifdef Mach
extern int errno;
#endif

char *syn_Buffer;

/* key to master shmem area */
int cacheSynId;

#ifdef CACHE_SYN
int synCached = 0;
#define NAPTIME 1
#define MAXNAPTIME 5
#endif

int     syn_compare (arg1, arg2)
        void * arg1, *arg2;
{
    t_Synonym * p1, *p2;
    p1 = (t_Synonym *) arg1;
    p2 = (t_Synonym *) arg2;
    return strcasecmp (p1 -> key, p2 -> key);
}

char   *lookup_Synonym (word, syn_Table, syn_Table_Size)
char   *word;
t_Synonym * syn_Table;
int     syn_Table_Size;
{
    t_Synonym tmp;
    t_Synonym * rec;
    if (syn_Table_Size == 0)
	return word;
    if (syn_Table_Size==1) {
      if (syn_Table->key == NULL) {
        return(word);
      } else {
        if (strcasecmp(word,syn_Table->key)) {
          return(word);
        } else {
          return(syn_Table->root);
        }
      }
    } else {
      tmp.key = (char *) s_malloc (strlen (word) + 1);
      strcpy (tmp.key, word);
      rec = (t_Synonym *) bsearch (&tmp, syn_Table, 
                                   syn_Table_Size, sizeof (t_Synonym), 
				   syn_compare);
      s_free (tmp.key);
    }
    if (rec)
	return rec -> root;
    return word;
}


void syn_ReadFile (fn, ptr, syn_Table_Size)
char   *fn;
t_Synonym ** ptr;
int    *syn_Table_Size;
{
    int syn_Table_Max, syn_Buffer_Max, syn_Buffer_Size = 0, i = 0;
    char   *key,
           *datum,
           *oldsynb,
            inputLine[SYN_FILE_LINE_LENGTH];
    FILE * fp;
    long offset;
    t_Synonym * syn_Table = NULL;
#ifdef CACHE_SYN
    t_cacheSyn *syn_Cache, *cs = NULL;
    int TheId = 0, doshmem = 1;
    key_t shm_key;
    t_Synonym *tblptr;
    char *shmptr, *pcs;
#endif

    *syn_Table_Size = 0;
    
#ifdef CACHE_SYN
    /* see if this synonym list is in shared memory */
    if (cacheSynId && (pcs = (char *) shmat (cacheSynId, 0, 0)) !=
	((char *)-1)) {
        syn_Cache = (t_cacheSyn *) pcs;
	for (i = 0; i < MAX_SYN_CACHE; i++) {
	    cs = (t_cacheSyn *) pcs;
	    if (!strcmp(cs->synfile, fn) || !cs->id) {
	        /* found the synonym list */
	        TheId = cs->id;
		break;
	    }
	    pcs += sizeof(t_cacheSyn);
	}
	if (TheId == -1) {
	    /* some other process is handling this shared memory segment;
	     * sleep for a while */
	    int naptime = 0;
	    waislog(WLOG_MEDIUM, WLOG_INFO,
		    "Another process is locking shared memory segment for file %s", cs->synfile);
	    while (cs->id == -1 && naptime < MAXNAPTIME) {
	        sleep(NAPTIME);
		naptime += NAPTIME;
		waislog(WLOG_MEDIUM, WLOG_INFO,
			"Shared memory entry for synonym file %s locked, waiting since %d seconds", fn, naptime);
	    }
	    if (cs->id == -1) {
	        TheId = 0;
		doshmem = 0;
		waislog(WLOG_HIGH, WLOG_WARNING,
			"Warning - shared memory lock for synonym file %s won't go away after %d seconds, disabling shared memory use for this search", fn, naptime); 
	    }
	    else 
	        TheId = cs->id;
	} 
	/* got the right Id, now attatch it */
	if (TheId) {
	    if ((tblptr = (t_Synonym *) shmat (TheId, 0, 0)) != 
		 ((t_Synonym *) -1)) {
	        /* create pointers in syn_Table from offsets in shmem table */
	        *syn_Table_Size = cs->table_size;
		syn_Buffer = (char *) (tblptr + 
				       cs->table_size * sizeof (t_Synonym));
	        syn_Table = (t_Synonym *) tblptr;
	        for (i = 0; i < cs->table_size; i++) {
		    syn_Table[i].root += (long) tblptr;
		    syn_Table[i].key  += (long) tblptr;
		}
		(*ptr) = (t_Synonym *) syn_Table;
		synCached = 1;
		return;
	    }
	    else {
	        doshmem = 0;
	        waislog (WLOG_HIGH, WLOG_WARNING, "Warning, couldn't attatch shared memory segment for file %s", fn);
	    }
	}
	else if (doshmem) {
	    /* this slot is empty, lock it now */
	    cs->id = -1;
	    strcpy(cs->synfile, fn);
	    waislog(WLOG_MEDIUM, WLOG_INFO,
		    "Locking shared memory segment for file %s", fn);
	}
    }
    else { 
        if (cacheSynId)
	     waislog (WLOG_HIGH, WLOG_WARNING, "Warning, couldn't attatch master shared memory segment");
	doshmem = 0;
    }
#endif /* CACHE_SYN */

    fp = fopen (fn, "rb");
    if (!fp) {
	waislog (WLOG_HIGH, WLOG_WARNING, "Warning: couldn't open %s - synonym translation disabled", fn);
	return;
    }
    /* allocate first buffer for synonym table and strings */
    syn_Table_Max = BUFSIZ / sizeof (t_Synonym);
    syn_Table = (t_Synonym *) s_malloc (syn_Table_Max * sizeof (t_Synonym));
    syn_Buffer_Max = 10 * BUFSIZ;
    syn_Buffer = (char *) s_malloc (syn_Buffer_Max);
    if (!syn_Table || !syn_Buffer) {
	waislog (WLOG_HIGH, WLOG_WARNING, "Warning: error allocating space for %s - synonym translation disabled", fn);
	goto wrapup;
    }
    

    /* read entries from file and write them into syn_Table */
    while (!feof (fp)) {
	fgets (inputLine, SYN_FILE_LINE_LENGTH - 1, fp);
	datum = (char *) strtok (inputLine, " \n\r");
	if (datum == NULL)
	  break;
	if (datum[0] != '#')	/* skip comment lines */
	    while ((key = (char *) strtok (NULL, " \n\r"))) {
		(*syn_Table_Size)++;
		if (*syn_Table_Size >= syn_Table_Max) {
		    syn_Table_Max += BUFSIZ / sizeof (t_Synonym);
		    syn_Table = (t_Synonym *) realloc (syn_Table, 
						       (syn_Table_Max * 
							sizeof (t_Synonym)));
		    if (!syn_Table) {
		        waislog (WLOG_HIGH, WLOG_WARNING, 
				 "Warning: error allocating space for %s - synonym translation disabled", fn);
			(*syn_Table_Size) = 0;
			goto wrapup;
		    }
		}
		if (syn_Buffer_Size + strlen (datum) + strlen (key) + 2 >=
		    syn_Buffer_Max) {
		    syn_Buffer_Max += 10 * BUFSIZ;
		    /* save location of original pointer to buffer */
		    oldsynb = syn_Buffer;
		    syn_Buffer = (char *) realloc (syn_Buffer, syn_Buffer_Max);
		    if (!syn_Buffer) {
		        waislog (WLOG_HIGH, WLOG_WARNING, 
				 "Warning: error allocating space for %s - synonym translation disabled", fn);
			(*syn_Table_Size) = 0;
			goto wrapup;
		    }
		    if (oldsynb != syn_Buffer) {
		        /* data area has moved; reset pointers */
			offset = syn_Buffer - oldsynb;
			for (i = 0; i < *syn_Table_Size - 1; i++) {
			    syn_Table[i].root += offset;
			    syn_Table[i].key  += offset;
			}
		    }
		}
		synptr (root) = (char *) syn_Buffer + syn_Buffer_Size;
		syn_Buffer_Size += strlen (datum) + 1;
		strcpy (synptr (root), datum);
		synptr (key) = (char *) syn_Buffer + syn_Buffer_Size;
		syn_Buffer_Size += strlen (key) + 1;
		strcpy (synptr (key), key);
	    }
    }
    fclose (fp);
    qsort (syn_Table, (*syn_Table_Size), sizeof (t_Synonym), syn_compare);
    (*ptr) = (t_Synonym *) syn_Table;

wrapup:

#ifdef CACHE_SYN

    if (!doshmem)
        return;
    /* try to put synonym table in a shared memory segment if a slot
     * in the master table was locked */
    if (cs->id != -1)
        return;

    offset = *syn_Table_Size * sizeof (t_Synonym);
    tblptr = (t_Synonym *) s_malloc (offset);
    if (!tblptr) {
        waislog (WLOG_HIGH, WLOG_WARNING, 
		 "Warning: cannot allocate memory for resetting synonym pointers");
	cs->id = 0;
	(cs->synfile)[0] = '\0';
	return;
    }
    shm_key = 1;
    TheId = -1;
    while (TheId == (-1)) {
        TheId = shmget (shm_key, offset + syn_Buffer_Size, 
			0644 | IPC_CREAT | IPC_EXCL);
	if (TheId == (-1) && errno != EEXIST) {
	    waislog (WLOG_HIGH, WLOG_WARNING, 
		     "Warning: cannot get shared memory for synonym file %s, caching disabled", fn);
	    break;
	}
	shm_key++;
    }
    if (TheId <= 0) {
        s_free (tblptr);
	cs->id = 0;
	(cs->synfile)[0] = '\0';
        return;
    }
    if ((shmptr = (char *) shmat (TheId, 0, 0)) == ((char *)-1)) {
        waislog (WLOG_HIGH, WLOG_WARNING, 
		 "Warning: cannot attatch shared memory segment for synonym file %s, caching disabled", fn);
        s_free (tblptr);
	cs->id = 0;
	(cs->synfile)[0] = '\0';
	return;
    }

    /* translate absolute pointer values to relative offsets (see description
     * of data layout in the shared memory segment) */
    for (i = 0; i < *syn_Table_Size; i++) {
        tblptr[i].root = (char *) (offset + syn_Table[i].root - syn_Buffer);
        tblptr[i].key  = (char *) (offset + syn_Table[i].key  - syn_Buffer);
    }

    /* now just copy the two memory blocks in the shmem segment */
    memcpy (shmptr, tblptr, offset);
    memcpy (shmptr + offset, syn_Buffer, syn_Buffer_Size);

    /* fill entry in master shmem table */
    cs->table_size = *syn_Table_Size;
    cs->id = TheId;

#endif /* CACHE_SYN */

    return;
}


void syn_Free (syn_Table, syn_Table_Size)
t_Synonym * syn_Table;
int    *syn_Table_Size;
{
#ifdef CACHE_SYN
    if (synCached) {
        /* reset pointers in shared memory */
        register int i;
	long offset = *syn_Table_Size * sizeof (t_Synonym);
	/* translate absolute pointer values to relative offsets
	 * (see description of data layout in the shared memory segment) */
	for (i = 0; i < *syn_Table_Size; i++) {
	    syn_Table[i].root -= (long) syn_Table;
	    syn_Table[i].key  -= (long) syn_Table;
	}
	/* now forget all about shmem segments */
	shmdt((char *)syn_Table);
	syn_Buffer = NULL;
	*syn_Table_Size = 0;
	return;
    }
#endif        
    if (syn_Table)
        s_free (syn_Table);
    syn_Table = NULL;
    *syn_Table_Size = 0;
    if (syn_Buffer)
        s_free (syn_Buffer);
    syn_Buffer = NULL;
    return;
}


    
/* 
   test stub for synonym routines 

char* log_file_name=NULL;
FILE* logfile;
main( argc,argv )
int argc;
char* argv[];
{
  char word[256];  
  t_Synonym* syn_Table;
  int syn_Table_Size;
  logfile=stderr;
  log_file_name="/dev/null";
  syn_ReadFile( "test.syn",&syn_Table,&syn_Table_Size );
  printf("Enter synonyms at the prompt.  Hit CTRL-D when you're done.\nWord   : ");
#ifndef _GETS_
  while (fgets(word, 256, stdin))
#else
  while (gets(word))
#endif
      printf("Synonym: %s\nWord   : ", lookup_Synonym(word,syn_Table,syn_Table_Size));
  syn_Free( syn_Table,&syn_Table_Size );
} 


*/
