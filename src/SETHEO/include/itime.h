#ifndef ITIME_H
#define ITIME_H

int  instr_time ();
/* int  instr_time (int iterative_bounding, int min_d, int d_inc, int min_loc_inf, 
                 int loc_inf_inc, int min_inf, int inf_inc); */

void  prepare_statistics();
/* void  prepare_statistics(); */

void  disp_statistics();
/* void  disp_statistics(int print_time)
 *
 + - Prints out the statistical information about proof. 
 +
 + special remarks:
 + - When using PRINT_LOG macros in if-statements do consider that they
 +   are already put into parentheses by definition (see machine.h).
 +   Therefore no semicolon is needed after those macros.
 + - <side effects, return values>
 */

#endif
