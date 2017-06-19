#ifndef UNIV_CONSTR_H
#define UNIV_CONSTR_H

/* prototypes are now in sam_prototypes.h */
/* void  pu_untrail(); 
 *
 * - untrails the protocol-unification-trail
 *
 * special remarks:
 * - set the pu_tr down to the base of the pu_trail
 * - <side effects, return values>
 */

/* c_counter *c_gencounter(WORD tag, choice_pt *ch, int infc);
 *
 * - generates a counter for a constraint on the c_stack 
 *
 * special remarks:
 * - called from c_gen_optimize() 
 * - returns a pointer to the new counter
 * - increments the c-stackpointer after checking for c_stack overflow
 * - initialize new counter to value 0
 * - exit setheo if counter cannot be generated in case of c_stack overflow
 */


/* int c_trail(WORD *obj);
 * 
 *  - trails object on the protocol-unification-trail
 *  
 *   special remarks:
 *   - checks for trail-overflow and produces an error when trail overflows
 *   - returns 1 if the object was successfully trailed
 */

/* int un_protoco (WORD *gp1, environ *lbp1, WORD *gp2, environ *lbp2, int nargs);
 *
 * - protocol - unification    
 *
 * special remarks:
 * - very similiar to un_red(...) from the modul unifred.c
 * - returns 1 if unification possible, 0 if not.
 * - side effects: possible unifications produce trailings, and pro
 *   unification, trail the environment and the argument-pointer of the
 *   trailed T_FVAR.
 */

/* int c_gen_optimize(c_counter* zaehler);
 *
 * - the optimized constraint is placed on the pu_trail between pu_tr and
 *   the pu_trail.  -  take it
 *
 * special remarks:
 * - pu_trail must be less than pu_tr                     
 * - returns 1, if subconstraints are generated 
 */

/* int  c_create(WORD* left_gp, WORD* right_gp, c_counter* zaehler);
 *
 * - creates a subconstraint on the c_stack
 *
 * special remarks:
 * - only called from c_gen_optimize (above) and anl_constr.c
 * - increaeses the c_sp with sizeof(constr_right)
 */

/* int c_check(WORD* var, WORD* right_gp);
 *
 * - Called whenever unification with a T_FVAR is planned. Only T_FVARs do
 *   have constraints - the pointers to the subconstraint lists are
 *   written directly into the T_FVAR. NULL-pointer means: no constraints
 *   do exist for the said variable. c_check gets the term to which the
 *   variable is to be bound (the binding has not been completed yet).
 *   (term = right_gp)
 *   c_check will compare the term to all terms occurring in the 
 *   list where sub_constr points to. This comparision is handled by
 *   un_protoco (protocol unification). If the term doesn't match any of the
 *   subconstraints then it returns to the caller with a permission to
 *   bind term to the T_FVAR. Otherwise the unification will fail and
 *   no binding is produced.
 *
 * special remarks:
 * - called by all macros in unification.h, by files usimple.ins and
 *   unifterm.ins
 * - returns 1, if unification is allowed, and 0, if a fail must follow.
 */

#endif
