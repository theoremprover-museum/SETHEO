#ifndef GENLEMMA_H
#define GENLEMMA_H

#include "machine.h"

int head_is_unit_lemma();
/* void head_is_unit_lemma(); 
 *
 * returns: 1 if true, 0 otherwise
 * Note:	FOLD_UP must be defined
 *			otherwise, 1 is returned always
 */


int more_general_in_index();
/* void more_general_in_index(WORD* ga); 
 *
 * returns: 1 if for the given literal in ga, more
 *				general unit-lemmata exist
 *			0 otherwise
 *
 * Note:	the given literal is matched with those
 *			unit-lemmata only which are NOT marked
 *			deleted
 * Note:
 *		updates the statistics
 */

choice_pt *delete_instances_in_index();
/* choice_pt *delete_instances_in_index(WORD* ga);
 *
 * given a literal, all instances thereof are
 * deleted
 * Returns:
 *		minimum of choice-points of the deleted
 *		lemmata, NULL otherwise
 *
 * Note:
 *		updates the statistics
 *		and unitlemma_nr_valid
 */

void  unitlemma_statistics_print();
/* void  unitlemma_statistics_print(); */

void ul_print_tree();
/* void ul_print_tree(FILE* filep, unit_lemma_storage_ctrl_block* lemma, environ* lbp)
 *
 *	print proof tree for the given unitlemma
 *	into a given file.
 *	filep:	where to write
 *	lemma:	the lemma (already compiled)
 *	bp:	root-bp for that subtree
 */





#endif






