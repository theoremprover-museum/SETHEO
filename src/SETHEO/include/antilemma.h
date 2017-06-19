#ifndef ANTILEMMA_H
#define ANTILEMMA_H

#include "machine.h"

choice_pt *immediate_antilemma_fail();
/* choice_pt *immediate_antilemma_fail(); */

void free_anl_list();
/* void free_anl_list(a_list *list); */

int anl_gen_constr();
/* int anl_gen_constr(); */

void  delay_anl_constr();
/* void delay_anl_constr(c_counter* constr, int bound); */



#endif
