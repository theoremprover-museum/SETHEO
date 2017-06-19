#ifndef REG_CONSTR_H
#define REG_CONSTR_H

int rc_gen_constr();
/* int rc_gen_constr(WORD rc_predicate, WORD* goal_p);
 *
 * - generates regularity-constraints for the predicate rc_predicate
 *
 * special remarks:
 *  - rc_predicate is already without a tag
 * - only called from genreg.ins or call
 * - returns 0, if generated constraint is violated
 */

int rc_genx_constr();
/* int rc_genx_constr(WORD rc_predicate, WORD* goal_p);
 *
 * - generates regularity-constraints for the predicate rc_predicate
 * - (extended regularity) in addition to rc_gen_constr, folded-up
 * - are regarded environments
 * special remarks:
 * -
 * - rc_predicate is already without a tag
 * - returns 0, if generated constraint is violated
 */

int rc_genu_constr();
/* int rc_genu_constr(WORD rc_predicate, WORD* goal_p);
 *
 * - generates regularity-constraints for the predicate rc_predicate
 * - in addition to rc_gen_constr open subgoals are regarded
 * special remarks:
 * - rc_predicate is already without a tag
 * - only called from genreg.ins or call
 * - returns 0, if generated constraint is violated
 */

int rc_genu_help();
/* rc_genu_help(WORD rc_predicate, WORD* goal_p, environ* rc_bp);
 * 
 * generate regularity-constraints fo the open subgoals of rc_bp         
 */

#endif
