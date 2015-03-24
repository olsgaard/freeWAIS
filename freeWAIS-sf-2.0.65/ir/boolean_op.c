/* 
 * boolean_op.c -- 
 * SCCS Status     : %W%	%G%
 * Author          : Huynh Quoc T. Tung
 * Created On      : Sun Oct 17 16:40:46 1993
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Fri Nov  3 13:25:22 1995
 * Update Count    : 35
 * Status          : Unknown, Use with caution!
 */

/*********************   PROCEDURE DESCRIPTION   ************************
 * Data structure used is the pushdown stack.
 * Basic operations:
 * push(v)
 * long v;
 *
 * push an item onto the stack (insert it at the beginning)
 *
 * pop() : pop an item (remove it from the beginning)
 * 
 * void Union(result, set1, set2)
 * search_result_struct *result;
 * search_result_struct *set1;
 * search_result_struct *set2;
 *
 * union two sets and write it into result.
 *
 * void Intersection(result, set1, set2)
 * search_result_struct * result;
 * search_result_struct * set1;
 * search_result_struct * set2;
 * 
 * intersection two sets and write it into result.
 *
 * void Difference(result, set1, set2)
 * search_result_struct *result;
 * search_result_struct *set1;
 * search_result_struct *set2;
 *
 * elements of set1 which are common to elements of set2 will be ignored.
 *
 * void Proximity(range, result, set1, set2)
 * int range;
 * search_result_struct * result;
 * search_result_struct * set1;
 * search_result_struct * set2;
 *
 * like Intersection, but set elements have to be within range words of
 * each other.  If range is negative, elements of set1 must precede
 * elements of set2.
 *
 * void Atleast(least_hits, result, set)
 * int least_hits;
 * search_result_struct * result;
 * search_result_struct * set;
 *
 * all elements of set with at least least_hits are written into result.
 */

/*
#include <stdio.h>
#include <string.h>
*/
#include "cdialect.h"
#include "irfiles.h"
#include "boolean_op.h"
#ifdef NEW_WEIGHT /* tung , 5/94 */
#include "irtfiles.h"
#endif

struct node
{ 
  long key; 
  struct node *next;
};
struct node *head, *z, *t;


boolean IsOperator(op)
     char *op;
{
  if(!strcmp(op,"and") ||
     !strcmp(op,"or") ||
     !strcmp(op,"not") ||
     !strncmp(op, "proxordered", 11) ||
     !strncmp(op, "proxunordered", 13) ||
     !strncmp(op, "proxatleast", 11)
     )
    return(true);
  else return(false);
}


void stackinit()
{
  head = (struct node *) malloc(sizeof (* head));
  z = (struct node *) malloc(sizeof(* z));
  head->next = z; 
  head->key = 0;
  z->next = z;
}

void push(v)
     long v;
{
  t = (struct node *) malloc(sizeof(* t));
  t->key = v;
  t->next = head->next;
  head->next = t;
}

long pop()
{
  long x;
  
  t = head->next;
  head->next = t->next;
  x = t->key;
  free(t);
  return x;
}

int stackempty()
{
  return head->next == z;
}


void Union(result, set1, set2)
     search_result_struct *result;
     search_result_struct *set1;
     search_result_struct *set2;
{
  doc_descr_struct	*array1 = set1->doc_ids_array,
			*array2 = set2->doc_ids_array,
			*resarray = result->doc_ids_array;
  long	array1len = set1->number_of_hits,
	array2len = set2->number_of_hits,
	index1 = 0,
	index2 = 0,
	indexres = 0;

  while (index1 < array1len && index2 < array2len) {
    while (index1 < array1len
    && array1[index1].doc_id < array2[index2].doc_id) {
      resarray[indexres].doc_id = array1[index1].doc_id;
      resarray[indexres].numwords = array1[index1].numwords;
#ifdef PROXIMITY
      resarray[indexres].word_position = array1[index1].word_position;
#endif
      resarray[indexres].score = array1[index1].score;
      ++indexres;
      ++index1;
    }
    while (index2 < array2len
    && array1[index1].doc_id > array2[index2].doc_id) {
      resarray[indexres].doc_id = array2[index2].doc_id;
      resarray[indexres].numwords = array2[index2].numwords;
#ifdef PROXIMITY
      resarray[indexres].word_position = array2[index2].word_position;
#endif
      resarray[indexres].score = array2[index2].score;
      ++indexres;
      ++index2;
    }
    while (index1 < array1len && index2 < array2len
    && array1[index1].doc_id == array2[index2].doc_id) {
      resarray[indexres].doc_id = array1[index1].doc_id;
      resarray[indexres].numwords =
        array1[index1].numwords + array2[index2].numwords;
#ifdef PROXIMITY
      resarray[indexres].word_position =
        s_malloc(resarray[indexres].numwords * sizeof(long));
      {
      long *cur1, *cur2, *curres;
      long *end1, *end2;

      cur1   = array1 [index1].word_position;
      cur2   = array2 [index2].word_position;
      curres = resarray [indexres].word_position;

      end1   = array1 [index1].word_position + array1 [index1].numwords;
      end2   = array2 [index2].word_position + array2 [index2].numwords;

      while (cur1 < end1 && cur2 < end2)
          if (*cur1 < *cur2)
            *curres++ = *cur1++;
          else if (*cur1 > *cur2)
            *curres++ = *cur2++;
          else
            *curres++ = *cur1++;
            cur2++;

      if (cur1 < end1) {
          memcpy (curres, cur1, (end1 - cur1) * sizeof (long));
          curres += end1 - cur1;
      }
      else if (cur2 < end2) {
          memcpy (curres, cur2, (end2 - cur2) * sizeof (long));
          curres += end2 - cur2;
      }

      resarray[indexres].numwords = curres - resarray[indexres].word_position;

      s_free(array1[index1].word_position);
      s_free(array2[index2].word_position);
      }
#endif
      resarray[indexres].score = array1[index1].score + array2[index2].score;
      ++indexres;
      ++index1;
      ++index2;
    }
  }
  if (index1 == array1len && index2 < array2len)	{
    memcpy(&resarray[indexres], &array2[index2],
           (array2len - index2) * sizeof(doc_descr_struct));
    indexres += array2len - index2;
  }
  else if (index1 < array1len && index2 == array2len)	{
    memcpy(&resarray[indexres], &array1[index1],
           (array1len - index1) * sizeof(doc_descr_struct));
    indexres += array1len - index1;
  }
  result->number_of_hits = indexres;
}


void Or_Operator(op1, op2)
     search_result_struct* op1;
     search_result_struct* op2;
{
  search_result_struct temp;

  if (op1->number_of_hits == 0) {
    if (op1->doc_ids_array != NULL)
      s_free(op1->doc_ids_array);
    push(op2->operand_id);
  }
  else if (op2->number_of_hits == 0) {
    if (op2->doc_ids_array != NULL)
      s_free(op2->doc_ids_array);
    push(op1->operand_id);
  } 
  else if((op1->number_of_hits != 0) &&
          (op2->number_of_hits != 0)) { 
    temp.doc_ids_array = s_malloc((op1->number_of_hits + op2->number_of_hits) *
           sizeof(doc_descr_struct));
    temp.number_of_hits = 0;
    
    Union(&temp, op2, op1);
    
    s_free(op1->doc_ids_array); op1->doc_ids_array = NULL;
    op1->number_of_hits = 0;
    s_free(op2->doc_ids_array); op1->doc_ids_array = NULL;
    op2->number_of_hits = 0;

    if (temp.number_of_hits > 0)
      op2->doc_ids_array = temp.doc_ids_array;
    else
      s_free(temp.doc_ids_array);
    op2->number_of_hits = temp.number_of_hits;
    push(op2->operand_id);
  }
}


void Intersection(result, set1, set2)
     search_result_struct * result;
     search_result_struct * set1;
     search_result_struct * set2;
{
  doc_descr_struct      *array1 = set1->doc_ids_array,
                        *array2 = set2->doc_ids_array,
                        *resarray = result->doc_ids_array;
  long  array1len = set1->number_of_hits,
        array2len = set2->number_of_hits,
        index1 = 0,
        index2 = 0,
        indexres = 0;

  while (index1 < array1len && index2 < array2len) {
    while (array1[index1].doc_id == array2[index2].doc_id) {
      if (array1[index1].doc_id == array2[index2].doc_id) {
        resarray[indexres].doc_id = array2[index2].doc_id;
        resarray[indexres].numwords =
          array1[index1].numwords + array2[index2].numwords;
#ifdef PROXIMITY
        resarray[indexres].word_position =
          s_malloc(resarray[indexres].numwords * sizeof(long));
      {
      long *cur1, *cur2, *curres;
      long *end1, *end2;

      cur1   = array1 [index1].word_position;
      cur2   = array2 [index2].word_position;
      curres = resarray [indexres].word_position;

      end1   = array1 [index1].word_position + array1 [index1].numwords;
      end2   = array2 [index2].word_position + array2 [index2].numwords;

      while (cur1 < end1 && cur2 < end2)
          if (*cur1 < *cur2)
            *curres++ = *cur1++;
          else if (*cur1 > *cur2)
            *curres++ = *cur2++;
          else
            *curres++ = *cur1++;
            cur2++;

      if (cur1 < end1) {
          memcpy (curres, cur1, (end1 - cur1) * sizeof (long));
          curres += end1 - cur1;
      }
      else if (cur2 < end2) {
          memcpy (curres, cur2, (end2 - cur2) * sizeof (long));
          curres += end2 - cur2;
      }

      resarray[indexres].numwords = curres - resarray[indexres].word_position;

      s_free(array1[index1].word_position);
      s_free(array2[index2].word_position);
      }
#endif
#ifdef NEW_WEIGHT
        resarray[indexres].score = MINIMUM(array1[index1].score, array2[index2].score);
#else
        resarray[indexres].score = array1[index1].score + array2[index2].score;
#endif
        ++indexres;
        ++index1;
        ++index2;
        if ((index1 >= array1len) || (index2 >= array2len)) break;
      }
    }
    if (index2 < array2len)
      while (index1 < array1len
      && array1[index1].doc_id < array2[index2].doc_id)
        ++index1;
    if (index1 < array1len)
      while (index2 < array2len
      && array1[index1].doc_id > array2[index2].doc_id)
        ++index2;
  }
  result->number_of_hits = indexres;
}


void And_Operator(op1, op2)
     search_result_struct* op1;
     search_result_struct* op2;
{
  search_result_struct temp;

  if (op1->number_of_hits == 0) {
    if (op1->doc_ids_array != NULL)
      s_free(op1->doc_ids_array);
    if (op2->doc_ids_array != NULL)
      s_free(op2->doc_ids_array);
    push(op1->operand_id);
  }
  else if (op2->number_of_hits == 0) {
    if(op1->doc_ids_array != NULL)
      s_free(op1->doc_ids_array);
    if(op2->doc_ids_array != NULL)
      s_free(op2->doc_ids_array);
    push(op2->operand_id);
  }
  else if((op1->number_of_hits != 0) &&
          (op2->number_of_hits != 0)) {
    temp.doc_ids_array = s_malloc(MINIMUM(op1->number_of_hits, op2->number_of_hits) *
           sizeof(doc_descr_struct));
    temp.number_of_hits = 0;

    Intersection(&temp, op2, op1);

    s_free(op1->doc_ids_array); op1->doc_ids_array = NULL;
    op1->number_of_hits = 0;
    s_free(op2->doc_ids_array); op2->doc_ids_array = NULL;
    op2->number_of_hits = 0;

    if(temp.number_of_hits > 0)
      op2->doc_ids_array = temp.doc_ids_array;
    else
      s_free(temp.doc_ids_array);
    op2->number_of_hits = temp.number_of_hits;
    push(op2->operand_id);
  }
}


void Difference(result, set1, set2)
     search_result_struct *result;
     search_result_struct *set1;
     search_result_struct *set2;
{
  doc_descr_struct      *array1 = set1->doc_ids_array,
                        *array2 = set2->doc_ids_array,
                        *resarray = result->doc_ids_array;
  long  array1len = set1->number_of_hits,
        array2len = set2->number_of_hits,
        index1 = 0,
        index2 = 0,
        indexres = 0;

  while (index1 < array1len && index2 < array2len) {
    while (index1 < array1len && index2 < array2len
    && array1[index1].doc_id < array2[index2].doc_id) {
      resarray[indexres].doc_id = array1[index1].doc_id;
      resarray[indexres].numwords = array1[index2].numwords;
#ifdef PROXIMITY
      resarray[indexres].word_position = array1[index1].word_position;
#endif
      resarray[indexres].score = array1[index1].score;
      ++indexres;
      ++index1;
    }
    if (index2 < array2len)
      while (index1 < array1len
      && array1[index1].doc_id == array2[index2].doc_id)
        ++index1;
    if (index1 < array1len)
      while (index2 < array2len
      && array1[index1].doc_id > array2[index2].doc_id)
        ++index2;
  }
  if (index1 < array1len)	{
    memcpy(&resarray[indexres], &array1[index1],
           (array1len - index1) * sizeof(doc_descr_struct));
    indexres += array1len - index1;
  }
  result->number_of_hits = indexres;
}


void Not_Operator(op1, op2)
     search_result_struct* op1;
     search_result_struct* op2;
{
  search_result_struct temp;

  if(op1->number_of_hits == 0) {
    if(op1->doc_ids_array != NULL)
      s_free(op1->doc_ids_array);
    if(op2->doc_ids_array != NULL)
      s_free(op2->doc_ids_array);
    push(op1->operand_id);
  }
  else if(op2->number_of_hits == 0) {
    if(op2->doc_ids_array != NULL)
      s_free(op2->doc_ids_array);
    push(op1->operand_id);
  }
  else if((op1->number_of_hits != 0) &&
          (op2->number_of_hits != 0)) {
    temp.doc_ids_array = s_malloc(op2->number_of_hits *
           sizeof(doc_descr_struct));
    temp.number_of_hits = 0;
    
    Difference(&temp, op2, op1);
    
    s_free(op1->doc_ids_array); op1->doc_ids_array = NULL;
    op1->number_of_hits = 0;
    s_free(op2->doc_ids_array); op2->doc_ids_array = NULL;
    op2->number_of_hits = 0;

    if(temp.number_of_hits > 0)
      op2->doc_ids_array = temp.doc_ids_array;
    else
      s_free(temp.doc_ids_array);
    op2->number_of_hits = temp.number_of_hits;
    push(op2->operand_id);
  }
}


void Proximity(range, result, set1, set2)
     int range;
     search_result_struct * result;
     search_result_struct * set1;
     search_result_struct * set2;
{
  doc_descr_struct      *array1 = set1->doc_ids_array,
                        *array2 = set2->doc_ids_array,
                        *resarray = result->doc_ids_array;
  long array1len = set1->number_of_hits,
       array2len = set2->number_of_hits,
       index1 = 0,
       index2 = 0,
       pindex1 = 0,
       pindex2 = 0,
       indexres = 0,
       diff,
       *temp_positions = NULL;

  while (index1 < array1len && index2 < array2len) {
    if (array1[index1].doc_id == array2[index2].doc_id) {
      resarray[indexres].doc_id = array1[index1].doc_id;
      resarray[indexres].numwords = 0;
      temp_positions = s_malloc(sizeof(long) *
        (array1[index1].numwords + array2[index2].numwords));
      for (pindex1 = pindex2 = 0; pindex1 < array1[index1].numwords
            && pindex2 < array2[index2].numwords; ) {
        diff = array2[index2].word_position[pindex2]
             - array1[index1].word_position[pindex1];
        if ((range > 0) ? (abs(diff) <= range) : (diff >= 0 && diff <= -range)) {
          temp_positions[resarray[indexres].numwords] =
            array1[index1].word_position[pindex1];
          ++resarray[indexres].numwords;
          temp_positions[resarray[indexres].numwords] =
            array2[index2].word_position[pindex2];
          ++resarray[indexres].numwords;
#ifdef NEW_WEIGHT
          resarray[indexres].score =
            MINIMUM(array1[index1].score, array2[index2].score);
#else
          resarray[indexres].score = array1[index1].score + array2[index2].score;
#endif
          ++pindex1;
          ++pindex2;
        } else if (array1[index1].word_position[pindex1] <
                   array2[index2].word_position[pindex2])
            pindex1++;
          else
            pindex2++;
      }
      if (resarray[indexres].numwords > 0) {
        resarray[indexres].word_position =
          s_malloc(resarray[indexres].numwords * sizeof(long));
        memcpy(resarray[indexres].word_position, temp_positions,
          resarray[indexres].numwords * sizeof(long));
        ++indexres;
      }
      s_free(temp_positions);
      ++index1;
      ++index2;
    }
    if (index2 < array2len)
      while (index1 < array1len
      && array1[index1].doc_id < array2[index2].doc_id)
        ++index1;
    if (index1 < array1len)
      while (index2 < array2len
      && array1[index1].doc_id > array2[index2].doc_id)
        ++index2;
  }
  result->number_of_hits = indexres;
}


void Prox_Operator(arg, op1, op2)
     int arg;
     search_result_struct* op1;
     search_result_struct* op2;
{
  search_result_struct temp;

  if(op1->number_of_hits == 0) {
    if(op1->doc_ids_array != NULL)
      s_free(op1->doc_ids_array);
    if(op2->doc_ids_array != NULL)
      s_free(op2->doc_ids_array);
    push(op1->operand_id);
  }
  else if(op2->number_of_hits == 0) {
    if(op1->doc_ids_array != NULL)
      s_free(op1->doc_ids_array);
    if(op2->doc_ids_array != NULL)
      s_free(op2->doc_ids_array);
    push(op2->operand_id);
  }
  else if((op1->number_of_hits != 0) && (op2->number_of_hits != 0)) {
    temp.doc_ids_array = s_malloc(MINIMUM(op1->number_of_hits, op2->number_of_hits) *
           sizeof(doc_descr_struct));
    temp.number_of_hits = 0;

    Proximity(arg, &temp, op2, op1);

    s_free(op1->doc_ids_array); op1->doc_ids_array = NULL;
    op1->number_of_hits = 0;
    s_free(op2->doc_ids_array); op2->doc_ids_array = NULL;
    op2->number_of_hits = 0;

    if(temp.number_of_hits > 0)
      op2->doc_ids_array = temp.doc_ids_array;
    else {
      s_free(temp.doc_ids_array);
      temp.doc_ids_array = NULL;
    }
    op2->number_of_hits = temp.number_of_hits;
    push(op2->operand_id);
  }
}


void Atleast(least_hits, result, set)
     int least_hits;
     search_result_struct * result;
     search_result_struct * set;
{
  doc_descr_struct      *array = set->doc_ids_array,
                        *resarray = result->doc_ids_array;
  long  arraylen = set->number_of_hits,
        indexop = 0,
        indexres = 0,
	last_doc_id;

  while (indexop < arraylen) {
    if (array[indexop].numwords >= least_hits) {
      resarray[indexres].doc_id = array[indexop].doc_id;
      resarray[indexres].numwords = array[indexop].numwords;
      resarray[indexres].word_position = array[indexop].word_position;
      resarray[indexres].score = array[indexop].score;
      ++indexres;
    }
    ++indexop;
  }
  result->number_of_hits = indexres;
}


void Atleast_Operator(arg, op)
     int arg;
     search_result_struct* op;
{
  search_result_struct temp;

  if(op->number_of_hits == 0) {
    if(op->doc_ids_array != NULL)
      s_free(op->doc_ids_array);
    push(op->operand_id);
  }
  else {
    temp.doc_ids_array =
      s_malloc(op->number_of_hits * sizeof(doc_descr_struct));

    Atleast(arg, &temp, op);

    s_free(op->doc_ids_array); op->doc_ids_array = NULL;
    op->number_of_hits = 0;

    if(temp.number_of_hits > 0)
      op->doc_ids_array = temp.doc_ids_array;
    else
      s_free(temp.doc_ids_array);
    op->number_of_hits = temp.number_of_hits;
    push(op->operand_id);
  }
}

boolean stackinitialized = false;

boolean boolean_operations(operator, result_array)
     char *operator;
     search_result_struct* result_array;
{
  long operand_id1, operand_id2;
  int i, arg;
  char *argptr;
  
  operand_id1 = operand_id2 = -1L;
  
  if (!stackinitialized) {
    stackinit();
    stackinitialized = true;
  }
  if (!strcmp(operator, "or")) {
    if(stackempty())
      operand_id1 = -1L;
    else operand_id1 = pop();
    if(stackempty())
      operand_id2 = -1L;
    else operand_id2 = pop();
    if((operand_id1 == -1) || (operand_id2 == -1)) {
      waislog(WLOG_HIGH, WLOG_ERROR,
              "boolean search failed, too few operands.\n");
      return(false);
    }
    else Or_Operator(&result_array[operand_id1], &result_array[operand_id2]);
  }
  else if (!strcmp(operator, "and")) {
    if(stackempty())
      operand_id1 = -1L;
    else operand_id1 = pop();
    if(stackempty())
      operand_id2 = -1L;
    else operand_id2 = pop();
    if((operand_id1 == -1) || (operand_id2 == -1)) {
      waislog(WLOG_HIGH, WLOG_ERROR,
              "boolean search failed, too few operands.\n");
      return(false);
    }
    else And_Operator(&result_array[operand_id1], &result_array[operand_id2]);
  }
  else if (!strcmp(operator, "not")) {
    if(stackempty())
      operand_id1 = -1L;
    else operand_id1 = pop();
    if(stackempty())
      operand_id2 = -1L;
    else operand_id2 = pop();
    if((operand_id1 == -1) || (operand_id2 == -1)) {
      waislog(WLOG_HIGH, WLOG_ERROR,
              "boolean search failed, too few operands.\n");
      return(false);
    }
    else Not_Operator(&result_array[operand_id1], &result_array[operand_id2]);
  }
  else if (!strncmp(operator, "proxatleast", 11)) {
    argptr = strpbrk(operator, "0123456789");
    if (argptr != NULL)
      arg = atoi(argptr);
    if (stackempty())
      operand_id1 = -1L;
    else operand_id1 = pop();
    if (argptr == NULL || operand_id1 == -1) {
      waislog(WLOG_HIGH, WLOG_ERROR,
              "atleast search failed, too few operands.\n");
      return(false);
    }
    else Atleast_Operator(arg, &result_array[operand_id1]);
  }
  else if (!strncmp(operator, "prox", 4)
	&& strstr(operator, "ordered") != NULL) {
    argptr = strpbrk(operator, "0123456789");
    if (argptr != NULL)
      arg = atoi(argptr);
    if (strstr(operator, "unordered") == NULL)	/* i.e. ordered. */
      arg = -arg;		/* signify this by using negative number. */
    if (stackempty())
      operand_id1 = -1L;
    else operand_id1 = pop();
    if (stackempty())
      operand_id2 = -1L;
    else operand_id2 = pop();
    if (argptr == NULL || (operand_id1 == -1) || (operand_id2 == -1)) {
      waislog(WLOG_HIGH, WLOG_ERROR,
              "proximity search failed, too few operands.\n");
      return(false);
    }
    else Prox_Operator(arg, &result_array[operand_id1], &result_array[operand_id2]);
  }
}

boolean save_operand_id(operand_id, search_result_array)
     long operand_id;
     search_result_struct* search_result_array;
{
  static long doc_ids_array_len = 0;
  long num_hits = search_result_array[operand_id].number_of_hits;

  if (!stackinitialized) {
    stackinit();
    stackinitialized = true;
  }
  push(operand_id);
  return(true);
}


long retriev_result(search_result, result)
     search_result_struct *search_result;
     double* result;
{
  int doc_id, count;
  long op, number_of_hits = 0;

  if (stackinitialized && !stackempty())
    op = pop();
  else  
    op = 0;     /* assume first element */
  if ((search_result != NULL) && (search_result[op].doc_ids_array != NULL)
  && (result != NULL))
    for (count=0; count < search_result[op].number_of_hits; count++) {
      doc_id = search_result[op].doc_ids_array[count].doc_id;
      result[doc_id] = MAXIMUM(search_result[op].doc_ids_array[count].score,
        result[doc_id]);
      ++number_of_hits;
    }

  stackinitialized = false;
  
  return(number_of_hits);
} 
