/* copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT)

7/29/92

This program is an attempt to scan into the dictionary and inverted file to determine the keywords that best
describe a database.  These could then be included in the description file
 * $Log: irkeywords.c,v $
 * Revision 2.0.1.2  1995/12/18 13:53:02  pfeifer
 * patch56: Rename char* keyword to char* keywrd sice it conficts with
 * patch56: keywords() on some systems.
 *
 * Revision 2.0.1.1  1995/09/15  09:45:07  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:00:05  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.11  1994/08/23  13:23:37  pfeifer
 * removed irlex.h
 *
 * Revision 1.10  1994/08/16  11:44:49  pfeifer
 * Fixed memory leak and other errors in retreive_keywords
 * Added free_keywords
 *
 * Revision 1.9  1994/08/08  07:32:34  pfeifer
 * Moved wais_log_file_name and waislogfile to cutil.[ch]
 *
 * Revision 1.8  1994/08/05  07:12:17  pfeifer
 * Release beta 04
 * 
 * Revision 1.6  1994/05/21  15:03:36  pfeifer
 * There is at least one uninitialized memory read and one memory leeak
 * left!
 * 
 * Revision 1.5  1994/05/20  12:56:22  pfeifer
 * beta
 *
 * Revision 1.4  1994/03/08  21:06:21  pfeifer
 * Patchlevel 04
 *
 * Revision 1.3  93/06/23  19:57:52  warnock
 * Fix from tovio@sage.ucs.uwa.edu.au for empty keywords array
 * 
 * Revision 1.2  93/02/16  17:07:49  freewais
 * added AT&T patches for keyword list
 * 

*/

#include <sys/types.h>
#include <sys/param.h>
#include "cutil.h"
#include "futil.h"
#include "irfiles.h"
#include "irtfiles.h"
#include "panic.h"
#include "ircfiles.h"
#include "irext.h"

unsigned char *dictionary_header = NULL; /* the dictionary header. 
						  loaded once */

long number_of_blocks = 0;  /* also the length of the dictionary 
					  header block */

unsigned char *dictionary = NULL; /* this is one of the dict blocks */

  char *keywrd[100];
  long keyvalue[100];
  long stored;
  short nKeys = 0;

void free_keywords()
{
  int i;
  for (i=0;i<100;i++) {
    if (keywrd[i]) s_free(keywrd[i]);
  }
}

retreive_keywords(db)
  database* db;
{
  long i,j, k, l, tmp, limit;
  char file[MAX_FILE_NAME_LEN + 1 ];
  double x,y;
  char *ptr;
  int tmpval;

  bzero(keywrd, sizeof(char*)*100);
  
  if(NULL == dictionary_header) {
    FILE *stream = db->dictionary_stream;
    s_fclose(stream);
    db->dictionary_stream = s_fopen(dictionary_filename(file, db), "r+b");
    stream = db->dictionary_stream;
    s_fseek(stream, 0L, SEEK_SET);
    number_of_blocks = read_bytes(DICTIONARY_HEADER_SIZE,stream);
    dictionary_header=
      read_dictionary_block(dictionary_header,DICTIONARY_HEADER_SIZE,
			    number_of_blocks,stream);
    if(NULL == dictionary_header) {
      printf("Could not read dictionary header block in db %s.", db->database_file);
      return(0);
    }
  }
  look_up_total_word_count(db);
  stored = 0;
  for (i=0; i<=number_of_blocks; i++) {
    FILE *stream = db->dictionary_stream;
    dictionary = 
      read_dictionary_block(dictionary,
                            dictionary_block_position(i, dictionary_header),
                            DICTIONARY_BLOCK_SIZE, stream);
    /* 
     * This reads one word too much (up)
     * for (j=0; j<=DICTIONARY_BLOCK_SIZE; j++)
     */
    for (j=0; j<DICTIONARY_BLOCK_SIZE; j++)
      /* 
       *  if (strlen(dictionary_block_word(j, dictionary))) {
       *  
       *  The following should be equivalent:
       */
      if (*(dictionary_block_word(j, dictionary))) {
        tmp = dictionary_block_word_occurances(j, dictionary);
        if (tmp == db->total_word_count)
          goto done;

        /* Search for the position where to insert */
        k=0;
        while (k<stored) {      /* stored is the first *free* entry */
          if (tmp > keyvalue[k]) /* we must insert between k-1 and k */
            break;
          k++;
        }

        /* Now k points to the position at which the
         * keyword should be inserted.
         * Entries at k and above must be shifted
         */

        if (k == stored) {      /* insert at the end */
          if (stored < 50) {    /* if less than 50 are in the array */
            keyvalue[stored]= tmp;
            tmpval=strlen(dictionary_block_word(j, dictionary))+1;
            keywrd[stored] = s_malloc(tmpval);
            strcpy(keywrd[stored], dictionary_block_word(j, dictionary));
            stored++;
          }
        } else {                /* insert in the middle */
          if (stored >= 50) {   /* remove the last key */
            stored--;
            s_free(keywrd[stored]);
            keyvalue[stored] = 0;
          }
          /* shift elements from k to stored-1 one to the right */
          for (l=stored; l>k; l--) { 
            keywrd[l]  = keywrd[l-1];
            keyvalue[l] = keyvalue[l-1];
          }
          stored++;
          tmpval=strlen(dictionary_block_word(j, dictionary))+1;
          keywrd[k] = s_malloc(tmpval);
          strcpy(keywrd[k], dictionary_block_word(j, dictionary));
          keyvalue[k] = tmp;
        }
      }
  }
 done:
  /* done getting, now cull and sort */
  x = 0;
  for (i=0; i<50 && x<0.2; i++)
    x += ((double) keyvalue[i])/db->total_word_count;
  limit = i;
  /* patch from tovio@sage.ucs.uwa.edu.au to fix problem with empty
   *	keywords array
   */
  if (i>0)
    for (i=0; i<=limit-1; i++) {
      for (j=i; j<limit; j++)
        if (strcmp(keywrd[i],keywrd[j])>0) {
          ptr=keywrd[i]; keywrd[i]=keywrd[j]; keywrd[j]=ptr;
        }
    }
  nKeys = limit;
}
