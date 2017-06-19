/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  HEADER:    path.h                                     * */
/* *                                                        * */
/* *  VERSION:   2.0                                        * */
/* *  MODIFIED:  June 1993 by Meyer                         * */
/* *                                                        * */
/* *  AUTHOR:    P. Graf (graf@mpi-sb.mpg.de)               * */
/* *             C. Meyer (meyer@mpi-sb.mpg.de)             * */
/* *             MPI fuer Informatik                        * */
/* *             Im Stadtwald                               * */
/* *             66123 Saarbruecken                         * */
/* *             Germany                                    * */
/* *                                                        * */
/* *  CAUTION:   The "interface" might have to be           * */
/* *             changed when using this modul with         * */
/* *             "terms" different than our own.            * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/

#ifndef _path_
#define _path_

/**************************************************************/
/* Includes                                                   */
/**************************************************************/

#include "path/list.h"
#include "path/newmemory.h"
#include "path/path_types.h"
#include "path/path_interface.h"

/**************************************************************/
/* Functions for creation and deletion of an path_INDEX       */
/**************************************************************/

path_INDEX path_Create(); 
void       path_Delete();

/**************************************************************/
/* Add and remove entries to an path_INDEX                    */
/**************************************************************/

ID         path_EntryCreate();
void       path_EntryDelete();
void       path_EntryPurge();
int        path_GC();

/**************************************************************/
/* Retrieve entries in an path_INDEX using a path_QUERY       */
/**************************************************************/

path_QUERY path_QueryCreate();
POINTER    path_QueryApply();
void       path_QueryDelete();

/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  CAUTION:                                              * */
/* *                                                        * */
/* *  - The "interface.h" might have to be changed when     * */
/* *    using this modul with other "terms".                * */
/* *  - Creation and deletion of entries of the index may   * */
/* *    arbitrarily be mixed with Retrieval in the index.   * */
/* *  - Use neither path_EntryPurge nor path_GC when there  * */
/* *    exists a path_QUERY for this path_INDEX.            * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/

#endif
