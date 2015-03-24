/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* Hash Table routines
 * for strings based on Common Lisp interface functions.
 *  -brewster
 */

/* This hashtable will never grow, rather it just adds entries to the buckets.
 * 
 * The contents are all in one contiguous block so that the contents can be 
 * sorted.  This will cause problems because of the reallocs needed.
 *
 */

/* Change log:
 * $Log: hash.c,v $
 * Revision 2.0.1.2  1995/11/14 18:10:43  pfeifer
 * patch51:
 *
 * Revision 2.0.1.1  1995/09/15  09:41:15  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  07:59:08  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.12  1994/12/12  16:23:18  pfeifer
 * typo
 *
 * Revision 1.11  1994/12/12  16:19:38  pfeifer
 * hash_entry_numeric_compare should return int!
 *
 * Revision 1.10  1994/09/02  14:34:21  pfeifer
 * fixed overlapping memory copies
 *
 * Revision 1.9  1994/08/08  10:29:42  pfeifer
 * Fixed the MAX_OCCURANCES - STOP_WORD_FLAG bug
 *
 * Revision 1.8  1994/08/05  07:10:54  pfeifer
 * Release beta 04
 *
 * Revision 1.7  1994/07/22  12:30:05  huynh1
 * left for holidays
 *
 * Revision 1.6  1994/07/13  07:52:36  huynh1
 * Uli
 *
 * Revision 1.5  1994/06/06  11:00:52  huynh1
 * get_hash updated.
 *
 * Revision 1.4  1994/05/20  12:55:57  pfeifer
 * beta
 *
 * Revision 1.3  1994/03/08  20:37:44  pfeifer
 * Patchlevel 04
 *
 * Revision 1.2  1994/02/14  10:49:47  huynh1
 * new code for field concept added.
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.9  92/05/06  17:27:38  jonathan
 * Added cast for htable->contents malloc.
 * 
 * Revision 1.8  92/02/21  11:06:53  jonathan
 * Added header, RCSIdent.
 * 
 * Revision 1.7  92/02/16  12:37:57  jonathan
 * Changed bzero's to memset's.
 * 
 * Revision 1.6  92/02/12  13:20:08  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
*/

/* Cool things we could do from here:
     A) Use a signature to see if the word is in the hashtable, so that
get_hash will return NULL very quickly for the new words.  This will reduce
the number of times a whole bucket must be searched.  This might not help
much since the file that I was indexing had 8M words, and only 350k
different words, so there are 1 NULL in 30 requests expected. Therefore,
unless searching down a list of buckets takes a long time, this is not a
big problem.  The only case I can think of is if it induces paging.
    B) Since the most recently accessed entry of the hashtable is at the
front, we can sort the hashtable entries so that the heads of all the
buckets are clumped, so that hopefully these will stay in memory, and the
infrequently used ones would move to the paged out space.
*/

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/hash.c,v 2.0.1.2 1995/11/14 18:10:43 pfeifer Exp $";
#endif

#include "cdialect.h"
#include "cutil.h"
#include "hash.h"
/* #include <string.h> */
#include "panic.h"

#define ALIGNMENT_SIZE 4 /* the word size to align elements of the contents on */


#ifdef NEW_WEIGHT /* tung, 5/94 */
#include "irfiles.h" 
#include "irhash.h"             /* MAX_OCCURANCES */
#endif

hashtable*
make_hashtable (number_of_buckets, number_of_elements, element_size)
     long number_of_buckets;
     long number_of_elements;
     long element_size;
{
  hashtable *htable = (hashtable *)s_malloc(sizeof(hashtable));

  if(number_of_buckets > 0)
    htable->number_of_buckets = number_of_buckets;
  else
    htable->number_of_buckets = DEFAULT_NUMBER_OF_BUCKETS;

  htable->buckets_mask = htable->number_of_buckets - 1;

  if(element_size > 0){
    htable->element_size = element_size;
  }
  else
    return(NULL);
  
  htable->number_of_elements = 0;

  if(number_of_elements > 0)
    htable->max_number_of_elements = number_of_elements;
  else
    htable->max_number_of_elements = DEFAULT_NUMBER_OF_ELEMENTS;

  htable->buckets
    = (long*)s_malloc(sizeof(long) * htable->number_of_buckets);
  /* this depends on -1L to be made up of -1 characters */
  memset(htable->buckets, -1, sizeof(long) * htable->number_of_buckets);

  htable->contents 
    = (hash_entry*)s_malloc(htable->max_number_of_elements * 
			    htable->element_size);
  return(htable);
}


/*----------------------------------------------------------------- */

/* returns a hashcode for a string.  
 * key is a string that is assumed to be downcased (if appropriate),
 * function is a number from 0 to 20 that are fairly independent hash 
 *    functions.
 */


/*===========================*
 *===  Hashing Functions  ===*
 *===========================*/


/*___________________________________________________________________________*/

/* Date: Fri, 13 Dec 91 16:28:53 EST
   From: Henry Massalin <henry@cs.columbia.edu> */

#define reg	register

hash(s)
reg	unsigned char	*s;
{
  reg	int		c;
  reg	unsigned long	h = 0;
  extern	unsigned long	RandHash[];

  if((h = (unsigned long)*s++) == 0)
    return(h);
  while((c = *s++) != 0)
    h += RandHash[ 0xFF & ((h >> 24) ^ c )];
  return(h);
}

/*___________________________________________________________________________*/

/*
 * Almost any random permutation will work.  Of course, now that I've
 * picked this one, everyone else must use it also, to be consistent.
 *
 * This array is constructed from four independent permutations of the
 * integers 0..255.  A different permutation in each of the bytes.
 */
unsigned long
	RandHash[256] = {
	0x4BEF74B7,0x7FE1F5A6,0xB4C08BF6,0xC2CF0CD0,
	0x788E2A29,0xB043B6B9,0x5B56C90C,0x324D8FE7,
	0x8D9C854A,0xCAE06AC8,0x902076A1,0x949EAA31,
	0xDC070A08,0xF7499B6D,0x17883EAA,0x22A65A35,
	0x34FC6407,0x8322F367,0xD105BB0F,0xD8A29526,
	0x8A1B708A,0xBE85B1E1,0xC66AC334,0xC1B13A1C,
	0xEDF644E4,0x3EA3806C,0x2732D31D,0xEBF504A9,
	0x08BB2B52,0x4775992C,0x3C0B8CF7,0x5FA0AEA2,
	0x7616D761,0x66AA86DB,0xC73305C0,0x09E6EA99,
	0xC9789DD6,0xF47340F1,0x3941303E,0x7542233A,
	0xE0812ED1,0xFF2E54FE,0x07B887EB,0x68CD0E93,
	0xB124C17F,0xD01A9C5B,0x96B490ED,0xA7CAE5FC,
	0x18D41F37,0x6E8DA1BC,0x9B631940,0x35ABCFD4,
	0xACA1FB0A,0x2F71B921,0x6F10A747,0xA6680796,
	0x01F12271,0xFDBE0378,0x02F0FF79,0xEC3FF2D2,
	0x84485C9C,0x60BF9113,0x98CC35E3,0xAA2A8D4D,
	0xA5CBA2E9,0xF0AE1EF2,0x990118E2,0x64A966F5,
	0x8595490D,0x7E9B4391,0xE8311B7D,0x2474BCBF,
	0x19D6E286,0xDFD32D5C,0xAB5116CA,0x7D72D6D5,
	0x0BF44F5E,0x6170B3D3,0x2C471CC2,0x2EFD1118,
	0x8C878836,0x62FE52E0,0x2B8C1277,0x2ACEBDEC,
	0xB2E8A8A3,0x30B757AE,0x708BBA8D,0x501EF654,
	0x49A7EF1E,0xFE7636AD,0xB54BBE4F,0x1ADBDB8C,
	0x72E24DD9,0xF68A2517,0xF56FEEDF,0x5A7EB75D,
	0xD606F9E6,0xC32526B8,0x5EB613F3,0x0C08CBDE,
	0x8B965E59,0xB361EB75,0x3FADE465,0xB8FFD16F,
	0x26442988,0x00D2D830,0x3D58FE41,0x3B386E44,
	0x1297E914,0xAFC11083,0xC5EBE8B6,0x15A49ECD,
	0xF950C263,0xA0DA0982,0x4CBDF101,0xE227F77E,
	0x556DB8DA,0xA1BA2125,0x1E3BD409,0xFCF25155,
	0x110A983B,0x7B7F1A4C,0xE7FB4C69,0xC8994E1F,
	0x586C4A42,0x5C1FAF2F,0xAD777D48,0x9186696B,
	0xD52181B4,0xE94C39EF,0x9EDEB2FF,0x802FDA20,
	0xA228D902,0x52BC7CFD,0x53DD47AC,0x6D12D25F,
	0xCE1D6D8F,0xF12C8E2B,0xDD5700C1,0x9A402851,
	0x386E53D7,0x0DF88281,0x4A55CDC4,0x653692B0,
	0x203CAC8E,0x747C6164,0xC47A02B2,0x1B4FC82D,
	0xF8934B7A,0x36A82CB5,0x690CD080,0x2800A524,
	0x1C045D0E,0xBF09CAF0,0x8814ABB3,0xE3D0776A,
	0x219AA6D8,0xCC375653,0xFA2327A8,0x57F75F27,
	0x732DA903,0x8F696738,0x3A453223,0xDA1C9F32,
	0xCF90152A,0x0F64BFF4,0xD7533FA5,0x8E7D55DC,
	0xBC598998,0x45110DBA,0x4D92E6FB,0x7746C01A,
	0x951348A0,0xBBFA7E9B,0x055EAD7B,0x06B36BBB,
	0x0EE31D97,0x37297F11,0xEF529450,0x51C6E168,
	0x1F67C63F,0x6B023443,0x449F5BFA,0x48C39A4B,
	0xD30E426E,0x0ADFE31B,0x467B6085,0x7CC49676,
	0x140DFA60,0xE16B7274,0x235AEC05,0xD2D7DCC6,
	0x13946212,0x339D2462,0xFB82F8EE,0xB6197970,
	0xBD0F3C9D,0xE64E3B28,0xE4B07A87,0x54C92F95,
	0x6CC583AB,0xD4182015,0x86DC755A,0xA930A333,
	0x82B273EA,0x923EED7C,0xF354A02E,0xEEEEA439,
	0x1D89089A,0xD9EDDEE8,0x1017B504,0xA45CB0C7,
	0x93C7E0A4,0x43D95858,0xDB654594,0x31D578CF,
	0xCDF98489,0xB915C7F9,0x67846FE5,0x04D150AF,
	0x5DF314DD,0xCB911772,0xA8627156,0x258FC573,
	0xEAA5E79F,0x633D4119,0x793501C9,0x7AD80F0B,
	0x4E5DF04E,0x97AF6884,0xAE3A9745,0xA3ACF48B,
	0x16398A10,0xDE8346C3,0x42C20606,0x03EAB4B1,
	0x8798FDCC,0x9CE4FC22,0x81B5DD3D,0x563431BE,
	0x2D799349,0xF2036C92,0xB7E97B3C,0x71E559C5,
	0x5960C4BD,0x295B63F8,0xBA5F0B57,0xE5B93DCE,
	0x6A4ACC90,0x89EC6516,0x4180D566,0x40263300,
	0x9DE738CB,0x4F2B3746,0x9F66DF9E,0xC0C8CEA7
};

/*___________________________________________________________________________*/

/*
 * Create permutations
 */
#if 0
#define N		256
#define	ulong		unsigned long

main()
{
	int     i, j;
	ulong	p[N], q[N];

	permute(p);
	for(i=3; --i >= 0; ) {
		permute(q);
		for(j=N; --j >= 0; )
			p[j] = (p[j] << 8) + q[j];
	}

	printf("ulong	RandHash[%d] = {", N);
	for(i=0; i<N; i++)
		printf("%s0x%08X", ((i&3) ? "," : "\n\t"), p[i]);
	printf("\n};\n\n");
}

permute(p)
ulong	*p;
{
	int     i, r, t;

	/* initialize array */
	for(i=N; --i >= 0; )
		p[i] = i;

	/* generate random permutation */
	for(i=N; --i >= 0; ) {
		r = (i * (random() & 0xFFFF)) >> 16;
		t = p[r]; p[r] = p[i]; p[i] = t;
	}
#if 0
	/* make sure there are no cycles of len < N */
	for(i=N; --i >= 0; ) {
	}
#endif
}

#endif

/* --------------------------- */

#ifdef UNUSED

/* courtesy ses@ccgr.technion.ac.il, but it turns out in
   informal timings that it increases the index time.  sigh. */

static char coeff[] = {
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1};


long hash_code(key, function)
     char *key;
     long function;
{
  register char *foo;
  register long hash = 0;
  register int l;

  for(l=function,foo=key;l<sizeof(coeff) && *foo ;l++)
    hash = hash + (*(foo++) * coeff[l]);

  return(hash);
}

#endif /* def UNUSED   */


/*----------------------------------------------------------------- */
#ifdef NEW_WEIGHT /* tung, 5/94 */
extern long bucket_ids_doc_array[]; /* defined in weight.c */
extern long number_of_bucket_ids;   /* defined in irtfiles.c */
#endif

#define HASHTABLE_GROW_FACTOR 4

hash_entry *put_hash(key, htable, element)
     char *key;
     hashtable *htable;
     hash_entry *element;
{
  /* allocate the entry out of the contents area, then add the entry to 
     the head of the bucket list. */

  /* old long bucket_number = hash_code(key, 0) & htable->buckets_mask; */
  long bucket_number = hash((unsigned char*)key) & htable->buckets_mask;
  long old_bucket_value =  (htable->buckets)[bucket_number];
  long new_bucket_value = htable->number_of_elements++;
  hash_entry *entry_header;

  if(new_bucket_value >= htable->max_number_of_elements){
    long new_size = 
      HASHTABLE_GROW_FACTOR * htable->max_number_of_elements * 
	htable->element_size;
	
    waislog(WLOG_LOW, WLOG_INFO, 
	    "Expanding hashtable from %ld bytes to %ld bytes address %ld...",
	   htable->max_number_of_elements * htable->element_size ,
	   new_size, htable->contents);
    htable->max_number_of_elements *= HASHTABLE_GROW_FACTOR;
    htable->contents = (hash_entry*)s_realloc(htable->contents, new_size);
    if(NULL == htable->contents)
      panic("Out of virtual memory.");
  }
  entry_header = &(htable->contents)[new_bucket_value];
  memcpy((char *)entry_header, (char *)element, htable->element_size);
  /* printf("Writing key: '%s' into index %ld\n", key, new_bucket_value); */
  strncpy(entry_header->key, key, MAX_KEY_SIZE + 1); 
  entry_header->next = old_bucket_value;

  (htable->buckets)[bucket_number] = new_bucket_value;
#ifdef NEW_WEIGHT /* tung, 5/94 */
  bucket_ids_doc_array[number_of_bucket_ids++] = new_bucket_value;
#endif
  return(entry_header);
}

/*----------------------------------------------------------------- */

/* returns a pointer to the element stored or NULL if none. */
hash_entry *get_hash (key, htable)
     char *key;
     hashtable *htable;
{
  /* this looks up the value in the bucket and puts it at the head of the 
     bucket. */

  /* old long bucket_number = hash_code(key, 0) & htable->buckets_mask; */
  long bucket_number = hash((unsigned char*)key) & htable->buckets_mask;
  long old_bucket_value =  (htable->buckets)[bucket_number];

  long next_contents_index = old_bucket_value;

  while(next_contents_index != -1){
    /* keep looking down the list for the right value */
    if(0 == strcmp((htable->contents)[next_contents_index].key, 
		   key)) {
#ifdef NEW_WEIGHT /* tung, 5/94 */
      if((htable->contents)[next_contents_index].number_of_occurances < MAX_OCCURANCES) {
	if((htable->contents)[next_contents_index].occurances_in_doc == 0) {
	  bucket_ids_doc_array[number_of_bucket_ids++] = next_contents_index;
	}
      }
#endif
      /* found it */
      return(&(htable->contents)[next_contents_index]);
    }
    else 
      next_contents_index = (htable->contents)[next_contents_index].next;
  }
  return(NULL);
}
  

/*----------------------------------------------------------------- */

boolean clr_hashtable (htable)
     hashtable *htable;
{    
  htable->number_of_elements = 0;
  memset(htable->buckets, -1, sizeof(long) * htable->number_of_buckets);
  return(true);
}

/*----------------------------------------------------------------- */

/* removes contents and free's memory for the hastable.   
   returns true if successful, false otherwise */
boolean free_hashtable(htable)
     hashtable *htable;
{
  s_free(htable->contents);
  s_free(htable->buckets);
  s_free(htable);
  return(true);
}

/*----------------------------------------------------------------- */

/* returns the number of elements in the hashtable */
long hashtable_count(htable)
     hashtable *htable;
{
  return(htable->number_of_elements);
}

/*----------------------------------------------------------------- */


static int hash_entry_compare _AP((hash_entry*i,hash_entry* j));

static int hash_entry_compare(i,j)
hash_entry *i;
hash_entry *j;
{
  return(strcmp(i->key, j->key));
}


boolean sort_hashtable(htable)
     hashtable *htable;
{
  qsort(htable->contents,
	htable->number_of_elements,
	(size_t)sizeof(hash_entry),
	hash_entry_compare);
  return(true);
}


#ifdef FIELDS /* tung, 1/94 */
static int hash_entry_numeric_compare _AP((hash_entry*i,hash_entry* j));

static int hash_entry_numeric_compare(i,j)
hash_entry *i;
hash_entry *j;
{
  int i_sign = 1;
  int j_sign = 1;
  char* i_key = i->key;
  char* j_key = j->key;
  double i_number, j_number;
  
  if(i_key[0] == '-') {
    i_sign = -1;
    i_key = i_key + 1;
  }
  if(j_key[0] == '-') {
    j_sign = -1;
    j_key = j_key + 1;
  }

  i_number = atof(i_key) * i_sign;
  j_number = atof(j_key) * j_sign;
  if(i_number > j_number) return(1);
  else if(i_number < j_number) return(-1);
  else return(0);
}

boolean sort_hashtable_numeric(htable)
     hashtable *htable;
{
  qsort(htable->contents,
	htable->number_of_elements,
	(size_t)sizeof(hash_entry),
	hash_entry_numeric_compare);
  return(true);
}
#endif

/*----------------------------------------------------------------- */
/* print routines */


static void print_content_element(index, htable)
     long index;
     hashtable *htable;
{  
  printf("  Index: %ld, key: '%s', Next index %ld\n", 
	 index, (htable->contents[index]).key, 
	 (htable->contents[index]).next);
}

void print_bucket(bucket, htable)
     long bucket;
     hashtable *htable;
{
  long next_contents_index = (htable->buckets)[bucket];

  printf(" Bucket: %ld\n", bucket);
  while(next_contents_index != -1){
    hash_entry * entry_header = &(htable->contents)[next_contents_index];
    print_content_element(next_contents_index, htable);
    next_contents_index = entry_header->next;
  }
  return;
}

long bucket_length(bucket, htable)
     long bucket;
     hashtable *htable;
{
  long next_contents_index = (htable->buckets)[bucket];
  long answer = 0;
  while(next_contents_index != -1){
    answer++;
    next_contents_index = ((htable->contents)[next_contents_index]).next;
  }
  return(answer);
}

static int bucket_length_compare _AP((long*i,long* j));

static int bucket_length_compare(i,j)
long *i;
long *j;
{
  return((*j) - (*i));
}

void analyze_hashtable_distribution(htable)
     hashtable * htable;
{
  long count = 0;
  long max_length = 128;
  long *bucket_length_array = (long *)malloc(sizeof(long) * max_length);
  memset((char*)bucket_length_array, 0, (sizeof(long) * max_length));

  waislog(WLOG_LOW, WLOG_INFO, "--Hashtable Analysis--");
  waislog(WLOG_LOW, WLOG_INFO, "Number of buckets: %ld",
	  htable->number_of_buckets);
  waislog(WLOG_LOW, WLOG_INFO, "Number of allocated elements %ld",
	  htable->number_of_elements);
  waislog(WLOG_LOW, WLOG_INFO, "Max number of elements %ld",
	  htable->max_number_of_elements);

  while(count < htable->number_of_buckets){
    long length = bucket_length(count, htable);
    if(length > max_length){  /* too small, start again */
      max_length *= 2;
      free((char*)bucket_length_array);
      bucket_length_array = (long *)malloc(sizeof(long) * max_length);
      memset((char*)bucket_length_array, 0, (sizeof(long) * max_length));
      count = 0;
    }
    else{
      bucket_length_array[length]++;
      count++;
    }
  }
  /* print the buckets */
  for(count = 0; count < max_length; count++){
    if(bucket_length_array[count] > 0){
      waislog(WLOG_LOW, WLOG_INFO, "Length %4ld, number of buckets: %5ld.",
	      count, bucket_length_array[count]);
    }
  }
  free((char*)bucket_length_array);
}

void print_hashtable(htable)
     hashtable * htable;
{
  long count;
  printf("Number of buckets: %ld\n",  htable->number_of_buckets);
  printf("Bucket mask %ld\n", htable->buckets_mask);
  printf("Element Size %ld\n", htable->element_size);
  printf("Number of allocated elements %ld\n", htable->number_of_elements);
  printf("Max number of elements %ld\n", htable->max_number_of_elements);
  
  printf("\nContents:\n");
  for(count = 0; count < htable->number_of_elements; count++){
    print_content_element(count, htable);	
  }
  printf("\nBuckets:\n");
  for(count = 0; count < htable->number_of_buckets; count++){
    if((htable->buckets)[count] != -1)
      print_bucket(count, htable);	
  }
} 

/* for saber trials:

foo(){
hashtable *htable;
hash_entry entry;
htable = make_hashtable(0, 0, sizeof(hash_entry));
print_hashtable(htable);
put_hash("food", htable, &entry);
print_hashtable(htable);
*get_hash("food", htable);
put_hash("food", htable, &entry);
print_hashtable(htable);
}
*/
