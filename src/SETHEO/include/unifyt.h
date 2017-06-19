#ifndef UNIFYT_H
#define UNIFYT_H

/* WORD *copy_struct (); */
/* WORD *copy_struct (WORD* obj, environ* bp, WORD* var);
 *
 * - copies the structure obj onto the heap
 *   if var occurs in obj then occur check fail will be detected
 *
 * special remarks:
 * - called by unification and built-ins
 * - Returns NULL in case of occur check, a pointer to copied object,
 *   or pointer to object itself in case of T_GTERM
 */



#endif
