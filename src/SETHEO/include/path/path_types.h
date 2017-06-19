/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  TYPES:     path_types.h     		            * */
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
/* ********************************************************** */
/**************************************************************/

#ifndef __PATH_TYPES_H__
#define __PATH_TYPES_H__

#include "path/path_interface.h"

/**************************************************************/
/* Constants                                                  */
/**************************************************************/

#define INDEXDEPTH     15
#define HASHMAX        501

typedef enum {ALL,
		INSTANCE,
		UNIFIER,
		GENERALIZATION,
		ALL_MIN,
		INSTANCE_MIN,
		UNIFIER_MIN,
		GENERALIZATION_MIN} path_MODE;

/**************************************************************/
/* path_INDEX                                                 */
/**************************************************************/

/* A key contains path information. The position of b in the
   User_TERM f(f(a,b),c) is referred to by the key [f1f2b], for
   example */

typedef struct key {
  int          position;
  user__SYMBOL symbol;
} KEY[INDEXDEPTH];

typedef long int ID;

typedef struct identifier {
  ID               id;
  POINTER          pointer;
  BOOL             deleted;
} IDENTIFIER_NODE, *IDENTIFIER;

/* Headers solve hash collisions in the hash table by
   chaining. */

typedef struct header {
  struct header *next;
  KEY           key;
  LIST          connector, lastconnector;
} HEADER_NODE, *HEADER, *HASHTABLE[HASHMAX];

typedef struct fpa {
  ID         lastid;
  HEADER     all;
  BOOL       inserted;
  HASHTABLE  table;
} path_INDEX_NODE, *path_INDEX;

/**************************************************************/
/* path_QUERY                                                 */
/**************************************************************/

/* The query trees consist of three different kinds of nodes
   (AND-, OR-, or LEAF-nodes). For a query the query tree
   is constructed and pointers to entries of the index are
   bubbled up to the root of the query tree. */

typedef enum {LEAF, AND, OR} NODETYPE;

typedef struct tree {
  NODETYPE nodetype;
  union {
    struct {
      HEADER terms;
      LIST   lastterm;
    } LEAF;
    struct {
      LIST subs;
      int  sons;
    } AND;
    struct {
      struct tree *left,    *right;
      IDENTIFIER  lastleft, lastright;
    } OR;
  } node;
  BOOL nilreturned;
} TREE_NODE, *TREE;

typedef struct root {
  path_INDEX  index;
  ID          lastquery;
  ID          lastReturnedId;
  BOOL        minimal;
  TREE        tree;
} path_QUERY_NODE, *path_QUERY;

#define path__MinimizedTree(_ROOT_)  _ROOT_->minimal
#define path__LastReturnedId(_ROOT_) _ROOT_->lastReturnedId

#endif
