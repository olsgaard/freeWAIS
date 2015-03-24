#ifndef BOOLEAN_OP_H
#define BOOLEAN_OP_H
/* 
 * boolean_op.h -- 
 * SCCS Status     : %W%	%G%
 * Author          : Huynh Quoc T. Tung
 * Created On      : Sun Oct 17 16:40:58 1993
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Fri Sep  8 16:53:50 1995
 * Update Count    : 20
 * Status          : Unknown, Use with caution!
 */

typedef struct doc_descr_struct {
  long doc_id;
  double score;
  long numwords;
  long *word_position;
} doc_descr_struct;

typedef struct search_result_struct {
  long operand_id;
  long number_of_hits;
  doc_descr_struct *doc_ids_array;
} search_result_struct;


boolean save_operand_id _AP((long operand_id, search_result_struct* search_result_array));
boolean IsOperator _AP((char* op));
long retriev_result _AP((search_result_struct *search_result, double* result));
boolean boolean_operations _AP((char* operator, search_result_struct* result_array));
#endif /* not BOOLEAN_OP_H */
