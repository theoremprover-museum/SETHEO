#ifndef BOUNDS_H
#define BOUNDS_H

int  look_ahead_for_bound_fail();
/* int  look_ahead_for_bound_fail(); */

int  bound_fail();
/* int  bound_fail(int depth_value, int inf_value, int old_inf_value, int local_inf_value, int sgs_value); */

int  fvars_bound_fail();
/* int  fvars_bound_fail(int fvars_value); */

int  tc_bound_fail();
/* int  tc_bound_fail(int tc_value); */

int  sig_bound_fail();
/* int  sig_bound_fail(int sig_value); */

void  init_bounds(); 
/* void  init_bounds(int tc_init,int sgs_init, int fvars_init); */

#endif
