#ifndef MATCH_H
#define MATCH_H

int match();
/* int match(WORD* ga, environ* bp_ga, WORD* ha, environ* bp_ha);
 *
 *	match two terms
 *	returns:
 *	1 if LHS is an instance of RHS     pXX < PXY, but PXY <| PYY
 *	0 otherwise
 *	
 *  Note: this outer function just initializes the counter
 *			for the new ``constants'' and calls the static
 *			function ``do_the_match''.
 */

#endif

