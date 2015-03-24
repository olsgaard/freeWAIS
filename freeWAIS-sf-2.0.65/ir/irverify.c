/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* Change log:
 * $Log: irverify.c,v $
 * Revision 2.0.1.1  1995/09/15 09:46:16  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:00:34  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/03/08  21:06:27  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.6  92/04/01  09:57:36  morris
 * fixed and eof check in readPostings
 * 
 * Revision 1.5  92/03/28  19:48:10  jonathan
 * Fixed Log header.
 * 
 * Revision 1.4  92/02/18  15:36:32  morris
 * made it faster
 * 
 * Revision 1.3  92/02/12  13:32:33  jonathan
 * Added $Log so RCS will put the log message in the header
 * 
*/

#include "irverify.h"
#include "irfiles.h"
#include "panic.h"
#include "futil.h"

#define TEST_READ false

/*---------------------------------------------------------------------------*/

void
printIndex (db)
database* db;
/* iterate over the index printing the contents */
{
  serialPostingFile* spf = NULL;
  char indexFileName[MAX_FILE_NAME_LEN + 1];
  postingsForATerm* posts = NULL;
  
  spf = initSerialPostingFile(index_filename(indexFileName,db));
  
  while ((posts = getPostingsForNextTerm(spf)) != NULL)
   { printPostingsForATerm(posts);
     /* XXX dispose of them */
   }
   
  disposeSerialPostingFile(spf);
}

/*---------------------------------------------------------------------------*/

static void 
print_dictionary_block_and_index _AP((unsigned char* block,long size,serialPostingFile* spf));

static void 
print_dictionary_block_and_index(block,size,spf)
unsigned char *block;
long size;
serialPostingFile* spf;
/* this prints the contents of a dictionary block */
{
  long i;
  postingsForATerm* posts = NULL;
  
  for(i = 0; i < size; i++)
   {
     char *word = dictionary_block_word(i, block);
     long pos = dictionary_block_position(i, block);
     if(word[0] == '\0')
       break;
     printf("Entry %3ld: %21s %7ld\n", i, word,pos);
     posts = getPostingsAt(spf,pos);
     printPostingsForATerm(posts);
     /* XXX dispose of them postings */
   }
}

/*---------------------------------------------------------------------------*/

extern long number_of_dictionary_blocks;
extern unsigned char *dictionary_header_block;
extern unsigned char *dictionary_block;

void
printIndexUsingDictionary(db)
database* db;
/* use the dictionary to go over the index */
{
  /* prints the contents of a dictionary */
  FILE *dictStream = db->dictionary_stream;
  long i;
  long new_number_of_dictionary_blocks;
  serialPostingFile* spf = NULL;
  char indexFileName[MAX_FILE_NAME_LEN + 1];

  spf = initSerialPostingFile(index_filename(indexFileName,db));

  if(NULL == dictStream)
    panic("dictionary dictStream is not open");
  s_fseek(dictStream, 0L, SEEK_SET);
  new_number_of_dictionary_blocks = read_bytes(DICTIONARY_HEADER_SIZE, dictStream);
  if(new_number_of_dictionary_blocks > number_of_dictionary_blocks)
    dictionary_header_block = NULL;
  number_of_dictionary_blocks = new_number_of_dictionary_blocks;
  printf("Number of dictionary blocks %ld\n", number_of_dictionary_blocks);
  if(NULL == (dictionary_header_block =
	      read_dictionary_block(dictionary_header_block,
				    DICTIONARY_HEADER_SIZE,
				    number_of_dictionary_blocks,
				    dictStream)))
    panic("Could not read dictionary header block");
  printf("The Dictionary Header Block:\n");
  print_dictionary_block(dictionary_header_block, number_of_dictionary_blocks);
  for(i = 0; i < number_of_dictionary_blocks; i++)
  {
    long pos = dictionary_block_position(i, dictionary_header_block);
    if(NULL == (dictionary_block =
		read_dictionary_block(dictionary_block,
				      pos, DICTIONARY_BLOCK_SIZE, dictStream)))
      panic("Could not read dictionary block %ld", pos);
    printf("\n\nDictionary block %ld (position %ld):\n", i, pos);
    print_dictionary_block_and_index(dictionary_block, DICTIONARY_BLOCK_SIZE,spf);
  }
  fseek(dictStream, 0L, SEEK_END);
  disposeSerialPostingFile(spf);
}

/*---------------------------------------------------------------------------*/

serialPostingFile*
initSerialPostingFile(filename)
char* filename;
/* open an inverted index file create by irn8. return a structure
   maintaining its state
 */
{
  FILE* stream = NULL;
  serialPostingFile* pf = NULL;

  stream = s_fopen(filename,"rb");
  if (stream == NULL) /* can't open that file */
    return(NULL);
  s_fseek(stream,INDEX_HEADER_SIZE,SEEK_SET);

  pf = (serialPostingFile*)s_malloc((size_t)sizeof(serialPostingFile));
  pf->stream = stream;
  pf->length = file_length(stream);
  pf->current_index_block = INDEX_HEADER_SIZE;

  return(pf);
}

/*---------------------------------------------------------------------------*/

void
disposeSerialPostingFile(pf)
serialPostingFile* pf;
{
  s_fclose(pf->stream);
  s_free(pf);
}

/*---------------------------------------------------------------------------*/

void 
printPostingsForATerm(pfat)
postingsForATerm* pfat;
{
  long i;

  if (pfat->word[0] != '\0')
    printf("word '%s'\n",pfat->word);
    
  for (i = 0; i < pfat->entries; i++)
    printf("\tdoc %ld weight %ld\n",pfat->docs[i],pfat->weights[i]);
}

/*---------------------------------------------------------------------------*/

postingsForATerm*
getPostingsAt(spf,position)
serialPostingFile* spf;
long position;
/* position better be a valid starting position! */
{
  fseek(spf->stream,position,SEEK_SET);
  spf->current_index_block = position;
  return(getPostingsForNextTerm(spf));
}

/*---------------------------------------------------------------------------*/

void
disposePostingsForATerm(pfat)
postingsForATerm* pfat;
{
  s_free(pfat->docs);
  s_free(pfat->weights);
  s_free(pfat);
}

/*---------------------------------------------------------------------------*/

void 
removePostings(pfat,start,run)
postingsForATerm* pfat;
long start;
long run;
/* remove postings start through start + run from the pfat */
{
  void* toPtr = NULL;
  long runLen;
  long toMove;

  if (start + run > pfat->entries)
    return; /* this is an error */

  toPtr = (void*)(pfat->docs + (start * sizeof(docID)));
  runLen = run * sizeof(docID);
  toMove = ((pfat->entries - start) * sizeof(docID)) - runLen;
  memmove(toPtr,toPtr + runLen,toMove);

  toPtr = (void*)(pfat->weights + (start * sizeof(postingWeight)));
  runLen = run * sizeof(docID);
  toMove = ((pfat->entries - start) * sizeof(postingWeight)) - runLen;
  memmove(toPtr,toPtr + runLen,toMove);

  pfat->entries -= run;
}

/*---------------------------------------------------------------------------*/

void
readPostings(spf,posts,not_full_flag)
serialPostingFile* spf;
postingsForATerm* posts;
long not_full_flag;
{
  long count;
  long document_id,weight,number_of_valid_entries;
  long index_block = read_bytes(NEXT_INDEX_BLOCK_SIZE,spf->stream);
  long index_block_size = read_bytes(INDEX_BLOCK_SIZE_SIZE,spf->stream);
  
  if (EOF == index_block_size) 
   { fprintf(stderr,"reading from the index file failed\n");
     return;
   }
      
  if (not_full_flag == INDEX_BLOCK_NOT_FULL_FLAG) /* not full */
    number_of_valid_entries = index_block / INDEX_ELEMENT_SIZE;

  else if (not_full_flag == INDEX_BLOCK_FULL_FLAG) /* full */
   { number_of_valid_entries = 
       (index_block_size - INDEX_BLOCK_HEADER_SIZE) / INDEX_ELEMENT_SIZE;
   }

  else
   { /* bad news,file is corrupted.  this should return error
	code rather than panicing XXX */
     panic("Expected the flag in the inverted file to be valid.  it is %lx",
	   not_full_flag);
   }

  posts->docs = 
    (docID*)s_malloc((size_t)(sizeof(docID) * number_of_valid_entries));
  posts->weights = 
    (postingWeight*)s_malloc((size_t)(sizeof(postingWeight) * 
				      number_of_valid_entries));

  for (count = 0; count < number_of_valid_entries; count++)
   { long val;
     posts->docs[count] = read_bytes(DOCUMENT_ID_SIZE,spf->stream);
     s_fseek(spf->stream,WORD_POSITION_SIZE+CHARACTER_POSITION_SIZE,SEEK_CUR);
     val = read_bytes(WEIGHT_SIZE,spf->stream);
     if(EOF == val)
      { fprintf(stderr,"reading from the inverted file failed\n");
        return;
      }
     else
      { posts->weights[count] = val;
        posts->entries++;
      } 
   }
}

/*---------------------------------------------------------------------------*/

long
readDictionaryIndexBlock(number_of_occurances,word,stream)
long *number_of_occurances;
char *word;
FILE *stream;
/* NOTE - similar to read_dictionary_index_lock */
{
  /* this reads the dictionary index block from the index stream.
     It assumes the stream is positioned at the right after the flag
     returns 0 if it succeeds.
     returns -1 if it is at the end of a file.
     returns -2 if it read something strange.
     Always sets word length to 0 if it fails. */

  char temp[MAX_WORD_LENGTH + 2];
  
  word[0] = '\0';

  s_fseek(stream,NEXT_INDEX_BLOCK_SIZE+INDEX_BLOCK_SIZE_SIZE,SEEK_CUR);
  *number_of_occurances = read_bytes(NUMBER_OF_OCCURANCES_SIZE,stream);
  fgets(temp,MAX_WORD_LENGTH + 2,stream); /* 2 is for the \n and '\0' */

  /* trim the \n */
  if(temp[strlen(temp) - 1] == '\n'){
    temp[strlen(temp) - 1] = '\0';
  }
  strcpy(word, temp);

  return(0);
}

/*---------------------------------------------------------------------------*/

postingsForATerm*
getPostingsForNextTerm(spf)
serialPostingFile* spf;
{
  postingsForATerm* posts = NULL;
  
  posts = (postingsForATerm*)s_malloc((size_t)sizeof(postingsForATerm));
  posts->word[0] = '\0';
  posts->entries = 0;

  /* this is really a 2 step process - read the dictonary block, then read
     the postings.  I don't see any reason to unwrap it though */
  while (true)
   { 
     long flag = read_bytes(INDEX_BLOCK_FLAG_SIZE,spf->stream);

     if (flag == EOF)
      { return(NULL);
      }

     if (flag == INDEX_BLOCK_DICTIONARY_FLAG) /* the dictionary entry */
      { /* read the dictionary part */
	long number_of_occurances;
	if (readDictionaryIndexBlock(&number_of_occurances,
				     posts->word,spf->stream) < 0)
	  panic("read dictionary index block failed");
      }
     else /* the posting entry */
      { readPostings(spf,posts,flag);
	break;
      }
   }

  return(posts);
}

/*---------------------------------------------------------------------------*/




#ifdef old

these routines are slower thatn the current ones, keep them around for a 
while until we are sure the new ones work ok

/*---------------------------------------------------------------------------*/

postingsForATerm*
getPostingsForNextTerm(spf)
serialPostingFile* spf;
{
  postingsForATerm* slow;
  postingsForATerm* fast;
/*
  long filePos = s_ftell(spf->stream);
  slow = getPostingsForNextTermSLOW(spf);
  printf("SLOW:\n");
  printPostingsForATerm(slow);NL();
  s_fseek(spf->stream,filePos,SEEK_SET);
*/
  fast = getPostingsForNextTermFAST(spf);
/*  printf("FAST:\n");
  printPostingsForATerm(fast);NL();
  disposePostingsForATerm(slow);
*/
  return(fast);
}

/*---------------------------------------------------------------------------*/

postingsForATerm*
getPostingsForNextTermSLOW(spf)
serialPostingFile* spf;
{
  postingsForATerm* posts = NULL;
  boolean keepGoing = true;
  
  if (spf->current_index_block >= spf->length)
    return(NULL);

  posts = (postingsForATerm*)s_malloc((size_t)sizeof(postingsForATerm));
  posts->word[0] = '\0';
  posts->entries = 0;

  while (keepGoing) 
   { 
     long flag = read_bytes(INDEX_BLOCK_FLAG_SIZE,spf->stream);
     long next_index_block = read_bytes(NEXT_INDEX_BLOCK_SIZE,spf->stream);
     long index_block_size = read_bytes(INDEX_BLOCK_SIZE_SIZE,spf->stream);

     if (flag == INDEX_BLOCK_DICTIONARY_FLAG)
       { long last_index_block;
	 long index_block_size;
	 long number_of_occurances;
	 char word[MAX_WORD_LENGTH + 1];
	 if (0 > read_dictionary_index_block(spf->current_index_block,
					     &last_index_block,
					     &index_block_size,
					     &number_of_occurances,
					     word,
					     spf->stream))
	   panic("read dictionary index block failed");
	 cprintf(TEST_READ,
		 "%ld: size %3ld word '%s',occurances %ld last block %ld\n",
		 spf->current_index_block,index_block_size,word,
		 number_of_occurances,next_index_block);
	 strcpy(posts->word,word);
       }

     else if (flag == INDEX_BLOCK_NOT_FULL_FLAG)
       { cprintf(TEST_READ,"%ld: size %3ld Not full,valid entries %ld\n",
		 spf->current_index_block,index_block_size,next_index_block);
	 readPostings(spf,posts);
	 keepGoing = false;
       }

     else if (flag == INDEX_BLOCK_FULL_FLAG)
       { cprintf(TEST_READ,"%ld: size %3ld full block,next block %ld\n",
		 spf->current_index_block,index_block_size,next_index_block);
	 readPostings(spf,posts);
	 keepGoing = false;
       }

     else 
       panic("bad entry %ld (ftell %ld),flag was %ld",
	     spf->current_index_block,ftell(spf->stream),flag);

     spf->current_index_block += index_block_size;
     s_fseek(spf->stream,spf->current_index_block,SEEK_SET);
   }

  return(posts);
}

/*---------------------------------------------------------------------------*/

void
readPostings(spf,posts)
serialPostingFile* spf;
postingsForATerm* posts;
{
  long not_full_flag = INDEX_BLOCK_FULL_FLAG;
  long count,index_block_size;
  long document_id,weight,number_of_valid_entries;
  long index_block = spf->current_index_block;
  
  if (index_block >= 0)
    {
      /* read the index block */
      if (0 != fseek(spf->stream,(long)index_block,SEEK_SET))
	{ 
	  fprintf(stderr,
		  "fseek failed into the inverted file to position %ld\n",
		  (long)index_block); 
	  return;
	}
      
      not_full_flag = read_bytes(INDEX_BLOCK_FLAG_SIZE,spf->stream);
      index_block = read_bytes(NEXT_INDEX_BLOCK_SIZE,spf->stream);
      index_block_size = read_bytes(INDEX_BLOCK_SIZE_SIZE,spf->stream);
      if (EOF == index_block_size) 
	{ fprintf(stderr,"reading from the index file failed\n");
	  return;
	}
      
      if (not_full_flag == INDEX_BLOCK_NOT_FULL_FLAG)
	{ /* not full */
	  number_of_valid_entries = index_block;
	}
      else if (not_full_flag == INDEX_BLOCK_FULL_FLAG)
	{ /* full */
	  number_of_valid_entries = index_block_size - INDEX_BLOCK_HEADER_SIZE;
	}
      else
	{ /* bad news,file is corrupted.  this should return error
	     code rather than panicing XXX */
	panic("Expected the flag in the inverted file to be valid.  it is %ld",
	      not_full_flag);
        }

      cprintf(TEST_READ,"  number of valid bytes: %ld\n",
	      number_of_valid_entries);
      
      for (count = 0; count < number_of_valid_entries; 
	   count = count + INDEX_ELEMENT_SIZE)
	{
	  document_id = read_bytes(DOCUMENT_ID_SIZE,spf->stream);
	  (void)read_bytes(WORD_POSITION_SIZE,spf->stream);
	  (void)read_bytes(CHARACTER_POSITION_SIZE,spf->stream);
	  weight = read_bytes(WEIGHT_SIZE,spf->stream);
	  cprintf(TEST_READ,"    entry %ld,Doc_id: %ld,weight %ld\n",
		  count % INDEX_ELEMENT_SIZE,document_id,weight);
	  if(EOF == weight) 
	    { fprintf(stderr,"reading from the doc-id table failed\n");
	      return;
	    }
	  posts->entries++;
	  posts->docs = (docID*)s_realloc(posts->docs,
				  (size_t)(sizeof(docID) * posts->entries));
	  posts->docs[posts->entries - 1] = document_id;
	  posts->weights = (postingWeight*)s_realloc(posts->weights,
				     (size_t)(sizeof(postingWeight) * 
					      posts->entries));
	  posts->weights[posts->entries - 1] = weight;
	}
    }
}

/*---------------------------------------------------------------------------*/

#endif /* ndef old */
