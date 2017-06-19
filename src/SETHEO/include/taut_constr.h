#ifndef TAUT_CONSTR_H
#define TAUT_CONSTR_H

int ta_gen_constr();
/* int ta_gen_constr(WORD* gp1, WORD* gp2);
 * - generates tautologie-constraints for the arguments in the gp1, und gp2
 *
 * special remarks:
 * - called from i_cgen und i_neq_built
 * - returns 0, if generated constraint is violated
 */




#endif
