/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: ptree.c 
 * VERSION:
 * DATE:		29.10.92
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 *	print the entire proof-tree
 *	to a given file
 *	
 *	this instruction always succeeds
 * MOD:
 *	22.2.93:	sgi->tp < bp then red.step changed to <= (jsc)
 *	11.3.93	Mueller,Goller neue Constraints, Statistics
 *	03.6.94	johann	new macros
 *	07.6.94	johann	negative pred-symbs have higher index
 *	09.3.95	johann	FROM->TO
 *	22.3.95	johann	FROM->TO
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "ptree.h"
#ifdef STATPART
#include "sps.h"
extern  char lopfilename[50];
#endif

#define INSTR_LENGTH		1




instr_result i_ptree()
{
  
#ifdef STATPART
  char  s[60];
  
  sprintf (s, "%s.tree", lopfilename);
  if( recomp && pvm )
    sprintf (s, "%s.tree-%d", lopfilename, or_numb);
  if( !(ftree = fopen (s, "a"))) {
    PRINT_LOG1("could not open %s - use stdout\n", s);
    ftree = stdout;
    }
#endif  

  /* print preamble */
  fprintf(ftree,"[\n"); 
  ptree_inf_counter = 0;
  
#ifdef STATISTICS1
  reductions_in_proof = 0;
  folding_statistics.factorizations_in_proof = 0;
#endif
  
  /* print the tree, starting with bottom of stack
   * which must contain an environment
   */
  disp_proof_tree(ftree,(environ *)stack,0);
  
  /* print postamble */
  fprintf(ftree,"].\n");
  
  if (!print_all_trees) {
    fclose(ftree);
  }
  
  pc +=INSTR_LENGTH;
  return success;
}
