/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  MODULE:    path.c                                     * */
/* *                                                        * */
/* *  VERSION:   2.0                                        * */
/* *  MODIFIED:  September 1993 by Meyer                    * */
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


#include "path/path.h"

path_INDEX  MyIndex; /* Avoid the parameter FPA in many functions. */
KEY         MyKey;   /* Avoid copying arrays in function calls .. */
   
/* Solution for correct retrieval after new term insertion:
   A leaf node contains a pointer to an header.
   If a leaf node of a query tree is created and there is no
   associated header in the index a new empty header is created.
   Everytime this leaf node is evaluated and some new terms were
   inserted in the index the evaluation function just has to
   look in the connector list of the associated header. */
   
/* START_ID+1 will be the id of the first inserted term. */
#define START_ID 0

/**************************************************************/
/* Macros for access on identifiers                           */
/**************************************************************/

#define    path__IdentifierId(I)      (((IDENTIFIER)I)->id)
#define    path__IdentifierPointer(I) (((IDENTIFIER)I)->pointer)
#define    path__IdentifierDeleted(I) (((IDENTIFIER)I)->deleted)

/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  KEYS                                                  * */
/* *  HASHING AND HEADER RETRIEVAL                          * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/**************************************************************
  static int path__KeyLength(KEY _KEY_)
***************************************************************
INPUT:   A key
OUTPUT:  The length of the key.
EXAMPLE: path__KeyLength(f1g2a)==2. path__KeyLength(f)==0. 
CAUTION: Macro!
***************************************************************/
#define path__KeyLength(_KEY_)    (_KEY_[0].position)


/**************************************************************
static enum {SMALLER = (-1), EQUAL, BIGGER}
   path_KeyCompare(KEY Key1, KEY Key2)
***************************************************************
INPUT:   Two Keys.
RETURNS: EQUAL,   if Key1 is the same key as Key2.
         BIGGER,  if Key1 is bigger than Key2.
         SMALLER, if Key1 is smaller than Key2.
SUMMARY: Compares the keys.
**************************************************************/
static enum {SMALLER = (-1), EQUAL, BIGGER}
    path_KeyCompare(Key1, Key2)
    KEY Key1;
    KEY Key2;
{
  int i;
  for (i = 0; i <= path__KeyLength(Key1); i++) {
    if (Key1[i].position   >Key2[i].position)    return BIGGER;
    if (Key1[i].position   <Key2[i].position)    return SMALLER;
    if ((int)Key1[i].symbol>(int)Key2[i].symbol) return BIGGER;
    if ((int)Key1[i].symbol<(int)Key2[i].symbol) return SMALLER;
  }
  return EQUAL;
}


/**************************************************************
  static int path_HashFunction(KEY Key)
***************************************************************
INPUT:	 A key.
RETURNS: An index of the hash table.
SUMMARY: Performs the hashing.
**************************************************************/
static int path_HashFunction(Key)
     KEY Key;
{
  int i;
  unsigned long val;
  
  val = 0;
  for (i=0; i <= path__KeyLength(Key); i++) {
    val = (val*3 + Key[i].position + (unsigned)Key[i].symbol);
  }
  return (val % HASHMAX);
}


/**************************************************************
  static HEADER path_CreateEmptyHeader(HEADER NextHead,
                                       KEY Key)
***************************************************************
INPUT:	 The header which follows a new created header in the
         hash collision list. A key that represents
	 a path to the entry in the fpa-index.
RETURNS: A new created and inserted empty header.
**************************************************************/
HEADER path_CreateEmptyHeader(NextHead, Key)
     HEADER NextHead;
     KEY    Key;
{
  HEADER NewHead;
  int    i;

  NewHead = (HEADER)memory_Malloc(sizeof(HEADER_NODE));

  for (i=0; i <= path__KeyLength(Key); i++) {
    NewHead->key[i].position = Key[i].position;
    NewHead->key[i].symbol   = Key[i].symbol;
  }
  
  NewHead->connector = NewHead->lastconnector = list__Nil();
  NewHead->next      = NextHead;
  return NewHead;
}


/**************************************************************
  static HEADER path_KeyToHeaderCreate(KEY Key)
***************************************************************
INPUT:	 A key that represents a path to the entry
         in the fpa-index.
RETURNS: The header with key 'Key' or if none exists a new
         created and inserted empty header.
**************************************************************/
static HEADER path_KeyToHeaderCreate(Key)
     KEY Key;
{
  int    hash      = path_HashFunction(Key);
  HEADER Header    = MyIndex->table[hash];
  HEADER LastHead  = NULL;

  /* Header has to be inserted at the first position in the */
  /* list of Headers ... */
  if ((Header==(HEADER)NULL) ||
      (path_KeyCompare(Key,Header->key)==SMALLER))
    return (MyIndex->table[hash] =
	    path_CreateEmptyHeader(Header, Key));
  
  /* Scan all hashheads until the first is found which is */
  /* equal or bigger than the Key. */
  for (;(Header!=(HEADER)NULL) && 
       (path_KeyCompare(Key,Header->key)==BIGGER);
       LastHead = Header,
       Header = Header->next);

  /* If no such Head was found, a new Header must be created. */
  if ((Header==(HEADER)NULL) ||
      !(path_KeyCompare(Key,Header->key)==EQUAL))
    return (LastHead->next =
	    path_CreateEmptyHeader(Header, Key));
  
  /* The correct Header has been found. */

  return Header;
}


/**************************************************************
  static HEADER path_KeyToHeader(KEY Key)
***************************************************************
INPUT:   A Key.
RETURNS: A Hashead which is the head of a list of connectors
         pointing to declarations having the path properties
	 of key.
***************************************************************/
static HEADER path_KeyToHeader(Key)
     KEY Key;
{
  HEADER Header;
  
  for (Header  = MyIndex->table[path_HashFunction(Key)];
       Header != (HEADER)NULL;
       Header  = Header->next) {
    
    switch (path_KeyCompare(Header->key,Key)) {
    case EQUAL:   return Header;
    case BIGGER:  return (HEADER)NULL;
    case SMALLER: break;  /* Continue search. */
    }
  }
  return Header;
}


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  GARBAGE COLLECTION                                    * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/**************************************************************
  void path_GCAll(HEADER Header, int *Counter)
***************************************************************
INPUT:	 The all-key 'Header' whose connector list is searched 
         for marked entries, '*Counter' counts the 'hits'.
RETURNS: None.
SUMMARY: Deletes all marked entries in the connector list of
         'Header', counts all 'hits' and adds it to '*Counter'.
CAUTION: Because of structure sharing of the identifiers, call
         path_GCAll only on the all-key header. The marked
	 identifiers will be freed, so make sure that all
	 entries of these identifiers in all other headers
	 are deleted before calling path_GCAll.
**************************************************************/
void path_GCAll(Header, Counter)
     HEADER Header;
     int *Counter;
{
  LIST Memo;
  LIST Result;
  LIST Scan;
  LIST Next;

  Result = Header->connector;
  
  while (!list__Empty(Result)
	 && path__IdentifierDeleted(list__Car(Result))) {
    Memo   = Result;
    Result = list__Cdr(Result);
    (*Counter)++;
    memory__Free(list__Car(Memo),sizeof(IDENTIFIER_NODE));
    memory__Free(Memo,sizeof(LIST_NODE));
  }

  Header->connector = Result;
  
  if (list__Empty(Result)) {
    Header->lastconnector = list__Nil();
    return;
  }
  
  for (Scan = Result,
       Next = list__Cdr(Result);
       !list__Empty(Next);)
    if (path__IdentifierDeleted(list__Car(Next))) {
      (*Counter)++;
      list__Rplacd(Scan, list__Cdr(Next));
      memory__Free(list__Car(Next),sizeof(IDENTIFIER_NODE));
      memory__Free(Next, sizeof(LIST_NODE));
      Next = list__Cdr(Scan);
    } else {
      Scan = Next;
      Next = list__Cdr(Next);
    }

  Header->lastconnector = Scan;
}


/**************************************************************
  void path_GCList(HEADER Header, int *Counter)
***************************************************************
INPUT:	 A 'Header' whose connector list is searched for marked
         entries, '*Counter' counts the 'hits'.
RETURNS: None.
SUMMARY: Deletes all marked entries in the connector list of
         'Header', counts all 'hits' and adds it to '*Counter'.
**************************************************************/
void path_GCList(Header, Counter)
     HEADER Header;
     int *Counter;
{
  LIST Memo;
  LIST Result;
  LIST Scan;
  LIST Next;

  Result = Header->connector;
  
  while (!list__Empty(Result)
	 && path__IdentifierDeleted(list__Car(Result))) {
    Memo   = Result;
    Result = list__Cdr(Result);
    (*Counter)++;
    memory__Free(Memo,sizeof(LIST_NODE));
  }

  Header->connector = Result;
  
  if (list__Empty(Result)) {
    Header->lastconnector = list__Nil();
    return;
  }
  
  for (Scan = Result,
       Next = list__Cdr(Result);
       !list__Empty(Next);)
    if (path__IdentifierDeleted(list__Car(Next))) {
      (*Counter)++;
      list__Rplacd(Scan, list__Cdr(Next));
      memory__Free(Next, sizeof(LIST_NODE));
      Next = list__Cdr(Scan);
    } else {
      Scan = Next;
      Next = list__Cdr(Next);
    }

  Header->lastconnector = Scan;
}


/**************************************************************
  int path_GC(path_INDEX Index)
***************************************************************
INPUT:	 An index for GC.
RETURNS: The number of 'hits' (number of deleted entries).
SUMMARY: Scans all headers in 'Index' for marked identifiers
         and deletes these entries.
CAUTION: Erases all empty headers. Do not use path_GC if
         path_QUERY trees exist.
**************************************************************/
int path_GC(Index)
     path_INDEX Index;
{
  int Counter;
  int i;
  HEADER Scan;
  HEADER LastHead;
  
  Counter = 0;
  
  /* Scan the whole index structure. */
  for (i=0;i<HASHMAX;i++) {

    LastHead = (HEADER)NULL;
    for (Scan=Index->table[i]; Scan!=(HEADER)NULL;) {

      path_GCList(Scan, &Counter);

      if (list__Empty(Scan->connector)) { 
	
	if (Scan == Index->table[i]) {
	  Index->table[i] = Scan->next;
	  memory__Free(Scan, sizeof(HEADER_NODE));
	  Scan = Index->table[i];
	} else {
	  LastHead->next = Scan->next;
	  memory__Free(Scan, sizeof(HEADER_NODE));
	  Scan = LastHead->next;
	}
      } else {
	LastHead = Scan;
	Scan = Scan->next;
      }
    }
  }

  /* Scan the all-key and delete the marked entries. */
  path_GCAll(Index->all, &Counter);

  return Counter; 
}


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  INDEX CREATION AND DELETION                           * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/**************************************************************
  path_INDEX path_Create()
***************************************************************
INPUT:	 None.
RETURNS: An FPA-Index.
SUMMARY: Initializes the index.
CAUTION: MUST BE CALLED BEFORE ANY OTHER INDEX FUNCTION.
**************************************************************/
path_INDEX path_Create()
{
  path_INDEX Index;
  int i;
  
  Index = (path_INDEX)malloc(sizeof(path_INDEX_NODE));
  Index->lastid                    = START_ID;
  Index->all                       = (HEADER)memory_Malloc(sizeof(HEADER_NODE));
  path__KeyLength(Index->all->key) = 0;
  Index->all->key[0].symbol        = user__SymbolSpecial();
  Index->all->connector            = Index->all->lastconnector = list__Nil();
  Index->all->next                 = (HEADER)NULL;
  
  for (i=0;i<HASHMAX;i++)
    Index->table[i] = (HEADER)NULL;
  
  return Index;
}


/**************************************************************
  void path_Delete(path_INDEX Index)
***************************************************************
INPUT:	 An index.
RETURNS: None.
SUMMARY: Deletes the whole index structure.
CAUTION: ENTRIES OF THE path_INDEX ARE NOT DELETED !
**************************************************************/
void path_Delete(Index)
     path_INDEX Index;
{
  int i;
  LIST Connector, NextConnector;

  /* First we have to delete the all-key-connector
     AND the identifiers. */

  Connector = NextConnector = Index->all->connector;
  memory__Free(Index->all, sizeof(HEADER_NODE));  

  while (!list__Empty(Connector)) {                              
    NextConnector = list__Cdr(NextConnector);                                    
    memory__Free(list__Car(Connector),sizeof(IDENTIFIER_NODE));  
    memory__Free(Connector,sizeof(LIST_NODE));                   
    Connector = NextConnector;                                           
  }

  /* Delete all connectors for every collision class. */

  for (i=0;i<HASHMAX;i++) {
    HEADER Scan = Index->table[i];
    while (Scan != (HEADER)NULL) {
      HEADER Next;
      Next = Scan->next;
      list__Delete(Scan->connector);
      memory__Free(Scan, sizeof(HEADER_NODE));
      Scan = Next;
    }       
  }

  /* Returns the storage of the hashtable as well. */

  free(Index);
}


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  CREATE NEW ENTRIES                                    * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


void path_ApplyToAllKeys();


/**************************************************************
  static HEADER path_CreateHeader(HEADER NextHead,
                                 IDENTIFIER Identifier,
				 KEY Key)
***************************************************************
INPUT:	 The header which follows a new created header in the
         hash collision list. An identifier consisting of an id
	 and a pointer to the entry of the index
	 which has to be inserted. A key that represents
	 a path to the entry in the path_INDEX-index.
RETURNS: A new created and inserted header.
**************************************************************/
HEADER path_CreateHeader(NextHead, Identifier, Key)
     HEADER     NextHead;
     IDENTIFIER Identifier;
     KEY        Key;
{
  HEADER NewHead;
  int    i;

  NewHead      = (HEADER)memory_Malloc(sizeof(HEADER_NODE));

  for (i=0; i <= path__KeyLength(Key); i++) {
    NewHead->key[i].position   = Key[i].position;
    NewHead->key[i].symbol     = Key[i].symbol;
  }
  
  NewHead->connector = NewHead->lastconnector =
    list__Cons(Identifier, list__Nil());
  NewHead->next      = NextHead;
  return NewHead;
}


/**************************************************************
static void path_EntryCreateSingle(IDENTIFIER Identifier, KEY Key)
***************************************************************
INPUT:	 An identifier which consists of an id and a pointer
         to the new entry that has to be integrated
         in the path_INDEX-index.
	 The key, for which Connector and (if necessary) Header
	 are created.
RETURNS: None.
**************************************************************/
static void path_EntryCreateSingle(Identifier, Key)
     IDENTIFIER Identifier;
     KEY        Key;
{
  int    hash      = path_HashFunction(Key);
  HEADER Header    = MyIndex->table[hash];
  HEADER LastHead  = NULL;

  /* Header has to be inserted at the first position in the */
  /* list of Headers ... */
  if ((Header==(HEADER)NULL) ||
      (path_KeyCompare(Key,Header->key)==SMALLER)) {
    MyIndex->table[hash] = path_CreateHeader(Header,Identifier,Key);
    return;
  }
  
  /* Scan all hashheads until the first is found which is */
  /* equal or bigger than the Key. */
  for (;(Header!=(HEADER)NULL) && 
       (path_KeyCompare(Key,Header->key)==BIGGER);
       LastHead = Header,
       Header = Header->next);

  /* If no such Head was found, a new Header must be created. */
  if ((Header==(HEADER)NULL) ||
      !(path_KeyCompare(Key,Header->key)==EQUAL)) {
    LastHead->next = path_CreateHeader(Header, Identifier, Key);
    return;
  }
  
  /* The correct Header has been found. A new list entry is
     appended at the end of the connector list. */

  {
    LIST NewConnector = list__Cons(Identifier, list__Nil());

    if (list__Empty(Header->connector)) {
      Header->connector = Header->lastconnector = NewConnector;
    } else {
      /* There are entries in the header. */
      list__Cdr(Header->lastconnector) = NewConnector;
      Header->lastconnector            = NewConnector;
    }
  }

}


/**************************************************************
  ID path_EntryCreate(path_INDEX Index,
                     POINTER Pointer,
     	             user__TERM Term)
***************************************************************
INPUT:	 A pointer to a new entry of the fpa-index. A term that
         has the path properties that will be searched later.
RETURNS: The id which is associated with 'Pointer'.
SUMMARY: Integrates a new entry in the fpa-index.
**************************************************************/
ID path_EntryCreate(Index, Pointer, Term)
     path_INDEX Index;
     POINTER Pointer;
     user__TERM Term;
{
  IDENTIFIER NewIdentifier;
  LIST       NewConnector;

  MyIndex = (path_INDEX)Index;
  
  /* For all entries of 'Pointer' only one identifier is created. */

  NewIdentifier = (IDENTIFIER)memory_Malloc(sizeof(IDENTIFIER_NODE));
  path__IdentifierId(NewIdentifier)      = ++(Index->lastid);
  path__IdentifierPointer(NewIdentifier) = Pointer;
  path__IdentifierDeleted(NewIdentifier) = FALSE;

  /* Append the identifier at the end of the all-key. */

  NewConnector = list__Cons(NewIdentifier, list__Nil());

  if (list__Empty(Index->all->lastconnector)) {
    /* The all-key is empty. */
    Index->all->lastconnector = NewConnector;
    Index->all->connector     = NewConnector;
  } else {
    /* There are entries in the all-key. */
    list__Cdr(Index->all->lastconnector) = NewConnector;
    Index->all->lastconnector            = NewConnector;
  }

  /* Compute initial key (top level key). */
  path__KeyLength(MyKey) = 0;
  MyKey[0].symbol = 
    user__TermIsVariable(Term) ?
      user__SymbolSpecial() : user__TermTop(Term);

  path_ApplyToAllKeys(path_EntryCreateSingle,NewIdentifier,MyKey,Term);

  return path__IdentifierId(NewIdentifier);
}


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  DELETE ENTRIES                                        * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/**************************************************************
void path_EntryDelete(path_INDEX Index, POINTER Pointer)
***************************************************************
INPUT:	 An id which is associated with a pointer to an entry
         of the path_INDEX-index.
RETURNS: None.
SUMMARY: Marks an entry in the path_INDEX-index as deleted.
**************************************************************/
void path_EntryDelete(Index, Pointer)
     path_INDEX Index;
     POINTER    Pointer;
{
  LIST Scan;
  
  /* Mark the entry in the all-key as 'deleted'. */

  for (Scan = Index->all->connector;
       !list__Empty(Scan) &&
       (Pointer != path__IdentifierPointer(list__Car(Scan)));            
       Scan = list__Cdr(Scan));

  /* If there is no appropriate entry in the all-key-connector,
     there are no more such entries in the index. */

  if (list__Empty(Scan))
    return;

  /* Otherwise mark the correct entry. */

  path__IdentifierDeleted(list__Car(Scan)) = TRUE;
}


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  PURGE ENTRIES                                         * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/**************************************************************
  static void path_EntryPurgeSingle(IDENTIFIER Identifier, KEY Key)
***************************************************************
INPUT:	 An identifier to the entry of the index which has to be
         deleted. A key that represents a path to the entry
	 in the path_INDEX-index.
RETURNS: None.
SUMMARY: Deletes the Connector and (if necessary) the Header
         leading to 'Pointer'.
**************************************************************/
static void path_EntryPurgeSingle(Identifier, Key)
     IDENTIFIER Identifier;
     KEY Key;
{
  int    hash;
  HEADER Header;
  HEADER LastHead;
  
  hash = path_HashFunction(Key);
  LastHead = (HEADER)NULL;
  
  /* Scan the hash collision class for appropriate header. */
  for (Header = MyIndex->table[hash];
       (Header !=(HEADER)NULL) &&
       (path_KeyCompare(Key,Header->key)==BIGGER);
       LastHead = Header, Header = Header->next);
  
  /* Assertion: (Header points to header with key Key). */
  /* If the list of connectors consists of a single connector */
  /* delete the header and return storage also. */
  if ((Header == (HEADER)NULL) ||
      (path_KeyCompare(Key,Header->key) != EQUAL))
    return;

  /* Header with only one connector ? */
  if (list__Empty(list__Cdr(Header->connector))) {

    if (Identifier != list__Car(Header->connector))
      return;

    if (MyIndex->table[hash]==Header)
      MyIndex->table[hash] = Header->next;
    else
      LastHead->next = Header->next;
    list__Delete(Header->connector);
    memory__Free(Header, sizeof(HEADER_NODE));

  } else {

    ID   Id;                                                   
    LIST Last;
    LIST Scan;
    
    Id = path__IdentifierId(Identifier);
                                                                 
    for (Scan = Header->connector, Last = list__Nil();                    
	 !list__Empty(Scan) &&                                   
	 (Id > path__IdentifierId(list__Car(Scan)));            
	 Last = Scan, Scan = list__Cdr(Scan));             
	                                                         
    if (list__Empty(Scan) || (Identifier != list__Car(Scan)))
      return;

    if (Scan == Header->connector) {                                   
      Header->connector = list__Cdr(Header->connector);                            
      memory__Free(Scan, sizeof(LIST_NODE));               
    } else {
      if (Scan == Header->lastconnector)
	Header->lastconnector = Last;
      list_DequeueNext(Last);
    }
  }
}


/**************************************************************
void path_EntryPurge(path_INDEX Index, POINTER Pointer, user__TERM Term)
***************************************************************
INPUT:	 An id which is associated with a pointer to an entry
         of the path_INDEX-index. A term that has the path properties
	 that will be deleted.
RETURNS: None.
SUMMARY: Deletes an entry in the path_INDEX-index.
CAUTION: If all terms are purged there can still be some
         empty headers left in the index because of empty header
	 creation during retrieval (to erase all empty headers
	 just call path_GC; a better way to delete all terms 
	 in the index is to use path_EntryDelete and path_GC).
	 Do not use path_EntryPurge if path_QUERY trees exist.
**************************************************************/
void path_EntryPurge(Index, Pointer, Term)
     path_INDEX Index;
     POINTER    Pointer;
     user__TERM Term;
{
  IDENTIFIER OldIdentifier;
  LIST       Last;
  LIST       Scan;

  MyIndex = (path_INDEX)Index;
  
  /* First delete the entry in the all-key. */

  for (Scan = Index->all->connector, Last = list__Nil();                    
       !list__Empty(Scan) &&
       (Pointer != path__IdentifierPointer(list__Car(Scan)));            
       Last = Scan, Scan = list__Cdr(Scan));             

  if (list__Empty(Scan))
    /* If there is no appropriate entry in the all-key-connector,
       there are no more such entries in the index. */
    return;

  /* Otherwise the correct entry is saved for later deletetion. */
  OldIdentifier = list__Car(Scan);                         

  /* Caution: 'lastconnector' must be updated
     if the last cell is deleted. */

  if (Scan == Index->all->connector)
    /* Scan is the first cons cell in the connector list. */
    if (Scan == Index->all->lastconnector) {
      /* Scan is the only cons cell in the connector list. */
      Index->all->lastconnector = list__Nil();
      Index->all->connector     = list__Nil();
    } else
      /* Scan is not the only one in the connector list but the first. */
      Index->all->connector = list__Cdr(Index->all->connector);
  else
    /* Scan is located somewhere after the first cons cell
       in the connector list. */
    if (Scan == Index->all->lastconnector) {
      /* Scan is the last cons cell in the connector list. */
      Index->all->lastconnector = Last;
      list__Cdr(Last)           = list__Nil();
    } else
      /* Scan is somewhere in the middle of the connector list. */
      list__Cdr(Last) = list__Cdr(Scan);

  /* Now free the cons cell of this entry. */

  memory__Free(Scan, sizeof(LIST_NODE));               

  /* Compute initial key (top level key). */

  path__KeyLength(MyKey) = 0;
  MyKey[0].symbol =
    user__TermIsVariable(Term) ?
      user__SymbolSpecial() : user__TermTop(Term);
 
  /* Look for more entries of 'OldIdentifier'. */

  path_ApplyToAllKeys(path_EntryPurgeSingle,OldIdentifier,MyKey,Term);

  /* Finally free the identifier itself.
     Caution: Identifiers are created only once for several key entries. */

  memory__Free(OldIdentifier, sizeof(IDENTIFIER_NODE));
}


/**************************************************************
void path_ApplyToAllKeys(void (*Function)(),
                        IDENTIFIER Identifier,
			KEY Key,
			TERM Term)
***************************************************************
INPUT:	 Function is either the insertion or deletion function
         on connectors. Identifier is a structure consisting
	 of an id which will be associated with a pointer.
	 The identifier is an entry of
         the path_INDEX-index which either is to be created or deleted.
	 Term is a Subterm of a term and Counter, Position,
	 and Key describe the path to 'Term'.
RETURNS: None.
SUMMARY: For all keys of 'Term' either call path_CreateHashEnt()
	 or path_DeleteHashEnt().
**************************************************************/
void path_ApplyToAllKeys(Function, Identifier, Key, Term)
     void (*Function)();
     IDENTIFIER Identifier;
     KEY        Key;
     user__TERM Term;
{
  /* Operate on current key. */
  Function(Identifier, Key);

  /* Cut if the INDEXDEPTH is reached. */
  if (path__KeyLength(Key) < INDEXDEPTH-1) {
    
    user__ARG ArgList;
    int Position;
    
    /* Apply mechanism recursively to the arguments. */
    path__KeyLength(Key)++;
    for (ArgList  = user__ArgList(Term),
	 Position = 1;
	 !user__ArgEmpty(ArgList);
	 ArgList = user__ArgCdr(ArgList),
	 Position++) {
      
      user__TERM   Current;
      user__SYMBOL Symbol;

      Current = user__ArgCar(ArgList);
      Symbol  = user__TermTop(Current);

      Key[path__KeyLength(Key)].position = Position;
      Key[path__KeyLength(Key)].symbol = 
	user__TermIsVariable(Current) ?
	  user__SymbolSpecial() : Symbol;
      path_ApplyToAllKeys(Function, Identifier, Key, Current);
    }
    path__KeyLength(Key)--;
  }
}


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  QUERY TREE EVALUATION                                 * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/**************************************************************
  void path_QueryDeleteHelp(TREE Tree)
***************************************************************
INPUT:	 A query tree.
RETURNS: None.
SUMMARY: Deletes the query tree and returns the memory to the
         memory management module.
***************************************************************/
void path_QueryDeleteHelp(Tree)
     TREE Tree;
{
  if (Tree == (TREE)NULL)
    return;

  switch (Tree->nodetype) {
  case LEAF:
    
    memory__Free(Tree, sizeof(TREE_NODE));
    break;
    
  case AND:
    
    {
      LIST List;
      for (List = Tree->node.AND.subs;
	   List != list__Nil();
	   List = list__Cdr(List))
	path_QueryDeleteHelp(list__Car(List));
    }
    list__Delete(Tree->node.AND.subs);
    memory__Free(Tree, sizeof(TREE_NODE));
    break;
    
  case OR:
    
    path_QueryDeleteHelp(Tree->node.OR.left);
    path_QueryDeleteHelp(Tree->node.OR.right);
    memory__Free(Tree, sizeof(TREE_NODE));
    break;

  default:
    Error__Out(("path_QueryDeleteHelp: Incorrect Mode"));
    break;
  }
}


/**************************************************************
  IDENTIFIER path_QueryApplyHelp(TREE Tree,
                               ID Minimum, 
                               BOOL Inserted)
***************************************************************
INPUT:	 A tree, an id and a flag.
RETURNS: An identifier to an entry of the index.
SUMMARY: Each time path_QueryApplyHelp is called, identifiers are
         bubbled up in the query tree until one identifier
	 reaches the root of the query tree whose id is greater
	 than 'Minimum'.
	 This identifier refers to an entry of the index which
	 fulfills all restrictions expressed by the query tree.
	 The flag 'Inserted' indicates that new terms were
	 inserted in the index since the last call to 
	 path_QueryApply.
**************************************************************/
IDENTIFIER path_QueryApplyHelp(Tree, Minimum, Inserted)
TREE Tree;
ID   Minimum;
BOOL Inserted;
{
  if (Tree->nilreturned && !Inserted)
    return (IDENTIFIER)NULL;

  switch (Tree->nodetype) {

  case LEAF:
    
    {
      LIST Scan;
      
      /* This is the first visit if 'lastterm' is nil. */

      Scan = list__Empty(Tree->node.LEAF.lastterm) ?
	Tree->node.LEAF.terms->connector 
	  : list__Cdr(Tree->node.LEAF.lastterm);

      if (list__Empty(Scan))
	/* Return null if there are no more new inserted terms.
	   Assertion: 'Tree->nilreturned' is TRUE. */
	return (IDENTIFIER)NULL;

      {
	LIST Ahead;

	/* Scan for an identifier with an id equal or bigger
	   than 'Minimum'. */
	for (Ahead = list__Cdr(Scan);
	     !list__Empty(Ahead) &&
	     ((path__IdentifierId(list__Car(Scan)) < Minimum) ||
	      (path__IdentifierDeleted(list__Car(Scan))));
	     Scan = Ahead, Ahead = list__Cdr(Ahead));
      
	/* 'Scan' will be the returned value. */
	Tree->node.LEAF.lastterm = Scan;

	if (list__Empty(Ahead)) {
	  /* 'Scan' is the last returned value until more new terms
	     are inserted in the index. */
	  Tree->nilreturned = TRUE;

	  /* Only identifiers with an id equal or bigger
	     than 'Minimum' and not marked as 'deleted'
	     are returned. */
	  if ((path__IdentifierId(list__Car(Scan)) < Minimum) ||
	      (path__IdentifierDeleted(list__Car(Scan))))
	    return (IDENTIFIER)NULL;
	  else
	    return list__Car(Scan);
	}

	/* At this point the identifier's id must be
	   equal or bigger than 'Minimum' and the identifier
	   is not marked as 'deleted'. */
	Tree->nilreturned = FALSE;
	return list__Car(Scan);
      }
    }
    
  case AND:

    {
      int NumberOfEqualSons;
      int NumberOfSons;
      LIST CurrentSon;
      IDENTIFIER Identifier;

      NumberOfEqualSons = 0;
      NumberOfSons      = Tree->node.AND.sons;
      CurrentSon        = Tree->node.AND.subs;
      
      do {
	Identifier = path_QueryApplyHelp(list__Car(CurrentSon),
					 Minimum, 
					 Inserted);
	
	/* Return of NULL in a subtree of an AND node
	   leads to setting 'nilreturned' to TRUE
	   and to the Result NULL. */

	if (Identifier==(IDENTIFIER)NULL) {
	  Tree->nilreturned = TRUE;
	  return (IDENTIFIER)NULL;
	}
	
	/* Check if the last call returned the same */
	/* result as the previous calls. */

	if (path__IdentifierId(Identifier) > Minimum) {
	  NumberOfEqualSons = 1;
	  Minimum = path__IdentifierId(Identifier);
	} else
	  NumberOfEqualSons++;

	/* Use Tree->node.AND.subs as a cyclic list */
	  
	if ((CurrentSon = list__Cdr(CurrentSon)) == list__Nil())
	  CurrentSon = Tree->node.AND.subs;
	
      } while (NumberOfSons != NumberOfEqualSons);

      Tree->nilreturned = FALSE;
      return Identifier;
    }

  case OR:
    
    {
      IDENTIFIER Identifier1;
      IDENTIFIER Identifier2;
      
      Identifier1 = Tree->node.OR.lastleft; 
      if ((Identifier1==(IDENTIFIER)NULL) ||
	  (path__IdentifierId(Identifier1) < Minimum))
	Identifier1 = path_QueryApplyHelp(Tree->node.OR.left, 
					  Minimum,
					  Inserted);
      
      Identifier2 = Tree->node.OR.lastright;
      if ((Identifier2==(IDENTIFIER)NULL) ||
	  (path__IdentifierId(Identifier2) < Minimum))
	Identifier2 = path_QueryApplyHelp(Tree->node.OR.right, 
					  Minimum,
					  Inserted);
      
      if (Identifier1 == (IDENTIFIER)NULL) {
	
	if (Identifier2 == (IDENTIFIER)NULL) {
	  
	  /* (Identifier1==NULL) && (Identifier2==NULL) */
	  Tree->node.OR.lastleft  = (IDENTIFIER)NULL;
	  Tree->node.OR.lastright = (IDENTIFIER)NULL;
	  Tree->nilreturned       = TRUE;
	  return (IDENTIFIER)NULL;
	  
	} else {
	  
	  /* (Identifier1==NULL) && (Identifier2!=NULL) */
	  Tree->node.OR.lastright = (IDENTIFIER)NULL;
	  Tree->nilreturned       = FALSE;
	  return Identifier2;
	}
	
      } else {
	
	if (Identifier2 == (IDENTIFIER)NULL) {
	  
	  /* (Identifier1!=NULL) && (Identifier2==NULL) */
	  Tree->node.OR.lastleft = (IDENTIFIER)NULL;
	  Tree->nilreturned      = FALSE;
	  return Identifier1;
	
	} else {
	  
	  /* (Identifier1!=NULL) && (Identifier2!=NULL) */
	  if (path__IdentifierId(Identifier1) <
	      path__IdentifierId(Identifier2)) {
	    Tree->node.OR.lastleft  = (IDENTIFIER)NULL;
	    Tree->node.OR.lastright = Identifier2;
	    Tree->nilreturned       = FALSE;
	    return Identifier1;
	  } else {
	    Tree->node.OR.lastleft  = Identifier1;
	    Tree->node.OR.lastright = (IDENTIFIER)NULL;
	    Tree->nilreturned       = FALSE;
	    return Identifier2;
	  }
	}
      }
    }

  default: 
    Error__Out(("path_QueryApplyHelp: Incorrect Mode"));
    return (IDENTIFIER)NULL;

  }
}


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  MACROS FOR THE QUERY TREE CONSTRUCTION                * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/**************************************************************
  static TREE path__CreateLeaf(TREE _RESULT_, KEY _KEY_)
***************************************************************
INPUT:
OUTPUT:
SUMMARY:
EFFECTS:
CAUTION:  Macro!
***************************************************************/
#define path__CreateLeaf(_RESULT_,_KEY_)                      \
{                                                             \
  HEADER Header;                                              \
  Header = path_KeyToHeaderCreate(_KEY_);                     \
  _RESULT_ = (TREE)memory_Malloc(sizeof(TREE_NODE));          \
  _RESULT_->nodetype           = LEAF;                        \
  _RESULT_->node.LEAF.lastterm = list__Nil();                 \
  _RESULT_->node.LEAF.terms    = Header;                      \
  if (list__Empty(Header->connector))                         \
    _RESULT_->nilreturned = TRUE;                             \
  else                                                        \
    _RESULT_->nilreturned = FALSE;                            \
}


/**************************************************************
  static TREE path__CreateOrAndReturn(TREE _LEFT_, TREE _RIGHT_)
***************************************************************
INPUT:
OUTPUT:
SUMMARY:
EFFECTS:
CAUTION:  Macro!
***************************************************************/
#define path__CreateOrAndReturn(_LEFT_,_RIGHT_)                \
{                                                             \
  TREE _RESULT_;                                              \
  _RESULT_ = (TREE)memory_Malloc(sizeof(TREE_NODE));          \
  _RESULT_->nodetype          = OR;                           \
  _RESULT_->node.OR.left      = _LEFT_;                       \
  _RESULT_->node.OR.right     = _RIGHT_;                      \
  _RESULT_->node.OR.lastleft  = (IDENTIFIER)NULL;             \
  _RESULT_->node.OR.lastright = (IDENTIFIER)NULL;             \
  _RESULT_->nilreturned       = (_LEFT_->nilreturned &&       \
                                 _RIGHT_->nilreturned);       \
  return _RESULT_;                                            \
}


/**************************************************************
  static TREE path__CreateAnd(_RESULT_,_TERM_,_KEY_,_CNT_,
                             _FKT_,_MODE_)
***************************************************************
INPUT:
OUTPUT:
SUMMARY:
EFFECTS:
CAUTION:  Macro!
***************************************************************/
#define path__CreateAnd(_RESULT_,_TERM_,_KEY_,_CNT_,_FKT_,_MODE_) \
{                                                             \
  user__ARG ArgList;                                          \
  int  Position;                                              \
                                                              \
  _RESULT_ = (TREE)NULL;                                      \
                                                              \
  /* Consider all possible subnodes of the AND-node */        \
  for (ArgList = user__ArgList(_TERM_), Position=1;           \
       (!user__ArgEmpty(ArgList));                            \
       ArgList = user__ArgCdr(ArgList), Position++) {         \
                                                              \
    /* Consider variables while searching general. only */    \
    if ((_MODE_==GENERALIZATION)                              \
        || !user__TermIsVariable(user__ArgCar(ArgList))) {    \
                                                              \
      TREE New;                                               \
      New = _FKT_(user__ArgCar(ArgList),_KEY_,Position,_CNT_);\
                                                              \
      if (_RESULT_ == (TREE)NULL)  /* First Loop ! */         \
                                                              \
        _RESULT_ = New;                                       \
                                                              \
      else if (_RESULT_->nodetype!=AND) {                     \
                                                              \
        if (New->nodetype!=AND) {                             \
          TREE _NODE_;                                        \
	  /* none of the roots is an AND-node */              \
          _NODE_ = (TREE)memory_Malloc(sizeof(TREE_NODE));    \
          _NODE_->nodetype      = AND;                        \
          _NODE_->node.AND.sons = 2;                          \
          _NODE_->node.AND.subs =                             \
            list__Cons(_RESULT_, list__Cons(New,list__Nil()));\
          _NODE_->nilreturned   = (New->nilreturned ||        \
                                   _RESULT_->nilreturned);    \
          _RESULT_ = _NODE_;                                  \
	} else {                                              \
	  /* only New is an AND-node */                       \
          New->node.AND.sons++;                               \
          New->node.AND.subs =                                \
            list__Cons(_RESULT_, New->node.AND.subs);         \
          New->nilreturned   = (New->nilreturned ||           \
                                _RESULT_->nilreturned);       \
          _RESULT_           = New;                           \
        }                                                     \
                                                              \
      } else {                                                \
                                                              \
        if (New->nodetype!=AND) {                             \
          /* only _RESULT_ is an AND-node */                  \
          _RESULT_->node.AND.sons++;                          \
          _RESULT_->node.AND.subs =                           \
            list__Cons(New, _RESULT_->node.AND.subs);         \
          _RESULT_->nilreturned   = (New->nilreturned ||      \
                                     _RESULT_->nilreturned);  \
        } else {                                              \
          /* both roots are AND-nodes */                      \
          _RESULT_->node.AND.sons+= New->node.AND.sons;       \
          _RESULT_->node.AND.subs =                           \
            list__Nconc(_RESULT_->node.AND.subs,              \
                        New->node.AND.subs);                  \
          _RESULT_->nilreturned   = (New->nilreturned ||      \
                                     _RESULT_->nilreturned);  \
          memory__Free(New, sizeof(TREE_NODE));               \
        }                                                     \
      }                                                       \
    }                                                         \
  }                                                           \
}


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  QUERY TREE CONSTRUCTION                               * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/**************************************************************
static TREE path_QueryCreateInstance(user__TERM Term,
                                   KEY Key,
	       			   int Position,
				   int Counter)
***************************************************************
INPUT:	 A term and a key which describes the path to this
         (Sub-)'Term'. Position contains an integer describing
	 at what position the Term occurs in it's superterm.
	 Counter contains the depth of the key.
	 Mode is one of the constants:
	 UNIFICATION, GENERALIZATION, or INSTANCE.
	 The Mode ALL does not reach this point !
**************************************************************/
TREE path_QueryCreateInstance(Term, Key, Position, Counter)
     user__TERM Term;
     KEY Key;
     int Position;
     int Counter;
{
  /* Store number of components of a key in the first pos. */
  Key[Counter].position	= Position;
  path__KeyLength(Key)	= Counter;
  
  /*********************************************/
  /* Term can never be a variable.             */
  /* Term is a constant .OR.                   */
  /* Term has reached INDEXDEPTH .OR.          */
  /* All arguments are variables (cut required)*/
  /*********************************************/
  if (user__TermIsConstant(Term)
      || (Counter == (INDEXDEPTH-1))
      || user__TermAllArgsAreVar(Term)) {
    
    TREE Leaf;
    Key[Counter].symbol = user__TermTop(Term);
    path__CreateLeaf(Leaf, Key);
    return Leaf;
    
  } else {
    
    TREE AndNode;
    Key[Counter].symbol = user__TermTop(Term);
    Counter++;
    path__CreateAnd(AndNode, Term, Key, Counter,
		    path_QueryCreateInstance, INSTANCE);
    return AndNode;
  }
}


/**************************************************************
  TREE path_QueryCreateUnify(user__TERM Term,
                          KEY Key,
			  int Position,
			  int Counter)
***************************************************************
INPUT:	 A term and a key which describes the path to this
         (Sub-)'Term'. Position contains an integer describing
	 at what position the Term occurs in it's superterm.
	 Counter contains the depth of the key.
	 Mode is one of the constants:
	 UNIFICATION, GENERALIZATION, or INSTANCE.
OUTPUT:  A query tree for terms unifiable with 'Term'.
**************************************************************/
TREE path_QueryCreateUnify(Term, Key, Position, Counter)
     user__TERM Term;
     KEY Key;
     int Position;
     int Counter;
{
  Key[Counter].position	= Position;
  path__KeyLength(Key)  	= Counter;
  
  /*********************************************/
  /* Term is a constant .OR.                   */
  /* Term has reached INDEXDEPTH .OR.          */
  /* All arguments are variables (cut required)*/
  /*********************************************/
  
  if ((!user__TermIsVariable(Term) && user__TermIsConstant(Term))
      || (Counter == (INDEXDEPTH-1))
      || user__TermAllArgsAreVar(Term)) {
    
    TREE Leaf1, Leaf2;

    Key[Counter].symbol = user__SymbolSpecial();
    path__CreateLeaf(Leaf1, Key);
    Key[Counter].symbol = user__TermTop(Term);
    path__CreateLeaf(Leaf2, Key);
    path__CreateOrAndReturn(Leaf1, Leaf2);
    
  } else {
    
    TREE Leaf, AndNode;
    
    Key[Counter].symbol = user__SymbolSpecial();
    path__CreateLeaf(Leaf, Key);
    Key[Counter].symbol = user__TermTop(Term);
    Counter++;
    path__CreateAnd(AndNode, Term, Key, Counter,
		    path_QueryCreateUnify, UNIFIER);
    path__CreateOrAndReturn(Leaf, AndNode);
  }
}



/**************************************************************
TREE path_QueryCreateGen(Term, Key, Position, Counter)
***************************************************************
INPUT:	 A term and a key which describes the path to this
         (Sub-)'Term'. Position contains an integer describing
	 at what position the Term occurs in it's superterm.
	 Counter contains the depth of the key.
	 Mode is one of the constants:
	 UNIFICATION, GENERALIZATION, or INSTANCE.
	 The Mode ALL does not reach this point !
**************************************************************/
TREE path_QueryCreateGen(Term, Key, Position, Counter)
     user__TERM Term;
     KEY Key;
     int Position;
     int Counter;
{
  /* Store number of components of a key in the first pos. */
  Key[Counter].position	= Position;
  path__KeyLength(Key)  	= Counter;

  /*********************************************/
  /* Term is a variable.                       */
  /*********************************************/
  if (user__TermIsVariable(Term)) {
    TREE   Leaf;
    Key[Counter].symbol = user__SymbolSpecial();
    path__CreateLeaf(Leaf, Key);
    return Leaf;
  }

  /*********************************************/
  /* Term is a constant .OR.                   */
  /* INDEXDEPTH is reached                     */
  /*********************************************/
  if (user__TermIsConstant(Term)
      || (Counter == (INDEXDEPTH-1))) {

    TREE   Leaf1, Leaf2;

    Key[Counter].symbol = user__SymbolSpecial();
    path__CreateLeaf(Leaf1,Key);
    Key[Counter].symbol = user__TermTop(Term);
    path__CreateLeaf(Leaf2,Key);
    path__CreateOrAndReturn(Leaf1, Leaf2);
  
  } else {
    
    TREE   Leaf, AndNode;

    Key[Counter].symbol = user__SymbolSpecial();
    path__CreateLeaf(Leaf, Key);
    Key[Counter].symbol = user__TermTop(Term);
    Counter++;
    path__CreateAnd(AndNode, Term, Key, Counter,
		    path_QueryCreateGen, GENERALIZATION);
    path__CreateOrAndReturn(Leaf, AndNode);
  }
}


/**************************************************************
path_QUERY path_QueryCreateAll(Index)
***************************************************************
INPUT:	 An index.
RETURNS: A query tree root.
**************************************************************/
path_QUERY path_QueryCreateAll(Index)
     path_INDEX Index;
{
  path_QUERY Root;
  
  Root = (path_QUERY)memory_Malloc(sizeof(path_QUERY_NODE));
  Root->index     = Index;
  Root->lastquery = Index->lastid;
  path__LastReturnedId(Root) = START_ID;
  path__MinimizedTree(Root)  = FALSE;

  Root->tree = (TREE)memory_Malloc(sizeof(TREE_NODE));
  Root->tree->nodetype           = LEAF;
  Root->tree->node.LEAF.lastterm = list__Nil();
  Root->tree->node.LEAF.terms    = Index->all;
  if (list__Empty(Index->all->connector))
    Root->tree->nilreturned = TRUE;
  else
    Root->tree->nilreturned = FALSE;
  return Root;
}


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  QUERY TREES IN FPA-STYLE                              * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/* The following macros and functions create query trees
   like the fpa-version (minimal trees). This kind of query tree
   is used for simple retrieval according to a question like
   this one:
   Are there any generalization partners for my query term
   in the index at the moment ?
   Building query trees like the fpa-version does not affect
   the size of the index because no empty headers will be created
   and the resulting trees are smaller and therefore faster
   to evaluate. */


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  FPA QUERY TREE EVALUATION                             * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/**************************************************************
  IDENTIFIER path_FpaQueryApplyHelp(TREE Tree, ID Limit)
***************************************************************
INPUT:	 A tree, an id and a flag.
RETURNS: An identifier to an entry of the index.
SUMMARY: Each time path_FpaQueryApplyHelp is called, identifiers
         are bubbled up in the query tree until one identifier
	 reaches the root of the query tree whose id is greater
	 than 'Limit'.
	 This identifier refers to an entry of the index which
	 fulfills all restrictions expressed by the query tree.
**************************************************************/
IDENTIFIER path_FpaQueryApplyHelp(Tree, Minimum)
     TREE Tree;
     ID   Minimum;
{
  if (Tree == (TREE)NULL)
    return (IDENTIFIER)NULL;

  switch (Tree->nodetype) {

  case LEAF:
    
    {
      LIST Scan;
      
      /* Scan for an identifier with an id equal or bigger
	 than 'Minimum'. */
      for (Scan = list__Empty(Tree->node.LEAF.lastterm) ?
	   Tree->node.LEAF.terms->connector 
	   : list__Cdr(Tree->node.LEAF.lastterm);
	   !list__Empty(Scan) &&
	   ((path__IdentifierId(list__Car(Scan)) < Minimum) ||
	    (path__IdentifierDeleted(list__Car(Scan))));
	   Scan = list__Cdr(Scan));

      if (list__Empty(Scan)) {
	/* There are no more terms. */
	memory__Free(Tree, sizeof(TREE_NODE));
	return (IDENTIFIER)NULL;
      } else {
	/* 'Scan' will be the returned value. */
	Tree->node.LEAF.lastterm = Scan;
	return list__Car(Scan);
      }
    }
    
  case AND:

    {
      int NumberOfEqualSons;
      int NumberOfSons;
      LIST CurrentSon;
      IDENTIFIER Identifier;

      NumberOfEqualSons = 0;
      NumberOfSons      = Tree->node.AND.sons;
      CurrentSon        = Tree->node.AND.subs;
      
      do {
	Identifier = path_FpaQueryApplyHelp(list__Car(CurrentSon),
					    Minimum);
	
	/* Return of NULL in a subtree of an AND node
	   leads to freeing all subtrees and the AND node itself
	   and to the Result NULL. */

	if (Identifier==(IDENTIFIER)NULL) {

	  LIST List;

	  for (List = Tree->node.AND.subs;
	       !list__Empty(List);
	       List = list__Cdr(List)) {
	    if (CurrentSon != List)
	      path_QueryDeleteHelp(list__Car(List));
	  }

	  list__Delete(Tree->node.AND.subs);
	  memory__Free(Tree, sizeof(TREE_NODE));
	  return (IDENTIFIER)NULL;
	}
	
	/* Check if the last call returned the same */
	/* result as the previous calls. */

	if (path__IdentifierId(Identifier) > Minimum) {
	  NumberOfEqualSons = 1;
	  Minimum = path__IdentifierId(Identifier);
	} else
	  NumberOfEqualSons++;

	/* Use Tree->node.AND.subs as a cyclic list */
	  
	if ((CurrentSon = list__Cdr(CurrentSon)) == list__Nil())
	  CurrentSon = Tree->node.AND.subs;
	
      } while (NumberOfSons != NumberOfEqualSons);

      return Identifier;
    }

  case OR:
    
    {
      IDENTIFIER Identifier1;
      IDENTIFIER Identifier2;
      
      Identifier1 = Tree->node.OR.lastleft; 
      if ((Identifier1==(IDENTIFIER)NULL) ||
	  (path__IdentifierId(Identifier1) < Minimum))
	Identifier1 = path_FpaQueryApplyHelp(Tree->node.OR.left, 
					     Minimum);
      
      Identifier2 = Tree->node.OR.lastright;
      if ((Identifier2==(IDENTIFIER)NULL) ||
	  (path__IdentifierId(Identifier2) < Minimum))
	Identifier2 = path_FpaQueryApplyHelp(Tree->node.OR.right, 
					     Minimum);
      
      if (Identifier1 == (IDENTIFIER)NULL) {
	
	if (Identifier2 == (IDENTIFIER)NULL) {
	  
	  /* (Identifier1==NULL) && (Identifier2==NULL) */
	  memory__Free(Tree, sizeof(TREE_NODE));
	  return (IDENTIFIER)NULL;
	  
	} else {
	  
	  /* (Identifier1==NULL) && (Identifier2!=NULL) */
	  Tree->node.OR.left      = (TREE)NULL;
	  Tree->node.OR.lastright = (IDENTIFIER)NULL;
	  return Identifier2;
	}
	
      } else {
	
	if (Identifier2 == (IDENTIFIER)NULL) {
	  
	  /* (Identifier1!=NULL) && (Identifier2==NULL) */
	  Tree->node.OR.right    = (TREE)NULL;
	  Tree->node.OR.lastleft = (IDENTIFIER)NULL;
	  return Identifier1;
	
	} else {
	  
	  /* (Identifier1!=NULL) && (Identifier2!=NULL) */
	  if (path__IdentifierId(Identifier1) <
	      path__IdentifierId(Identifier2)) {
	    Tree->node.OR.lastleft  = (IDENTIFIER)NULL;
	    Tree->node.OR.lastright = Identifier2;
	    return Identifier1;
	  } else {
	    Tree->node.OR.lastleft  = Identifier1;
	    Tree->node.OR.lastright = (IDENTIFIER)NULL;
	    return Identifier2;
	  }
	}
      }
    }

  default: 
    Error__Out(("path_FpaQueryApplyHelp: Incorrect Mode"));
    return (IDENTIFIER)NULL;

  }
}


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  MACROS FOR THE FPA QUERY TREE CONSTRUCTION            * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/**************************************************************
  static TREE path__FpaCreateLeaf(TREE _RESULT_, KEY _KEY_)
***************************************************************
INPUT:
OUTPUT:
SUMMARY:
EFFECTS:
CAUTION:  Macro!
***************************************************************/
#define path__FpaCreateLeaf(_RESULT_,_KEY_)                   \
{                                                             \
  HEADER Header;                                              \
  Header = path_KeyToHeader(_KEY_);                           \
  if (Header==(HEADER)NULL ||                                 \
      list__Empty(Header->connector))                         \
    _RESULT_ = (TREE)NULL;                                    \
  else {                                                      \
    _RESULT_ = (TREE)memory_Malloc(sizeof(TREE_NODE));        \
    _RESULT_->nodetype           = LEAF;                      \
    _RESULT_->node.LEAF.lastterm = list__Nil();               \
    _RESULT_->node.LEAF.terms    = Header;                    \
    _RESULT_->nilreturned        = FALSE;                     \
  }                                                           \
}


/**************************************************************
  static TREE path__FpaCreateOrAndReturn(TREE _LEFT_, TREE _RIGHT_)
***************************************************************
INPUT:
OUTPUT:
SUMMARY:
EFFECTS:
CAUTION:  Macro!
***************************************************************/
#define path__FpaCreateOrAndReturn(_LEFT_,_RIGHT_)            \
{                                                             \
  if      (_LEFT_  == (TREE)NULL) return _RIGHT_;             \
  else if (_RIGHT_ == (TREE)NULL) return _LEFT_;              \
  else {                                                      \
  TREE _RESULT_;                                              \
  _RESULT_ = (TREE)memory_Malloc(sizeof(TREE_NODE));          \
  _RESULT_->nodetype          = OR;                           \
  _RESULT_->node.OR.left      = _LEFT_;                       \
  _RESULT_->node.OR.right     = _RIGHT_;                      \
  _RESULT_->node.OR.lastleft  = (IDENTIFIER)NULL;             \
  _RESULT_->node.OR.lastright = (IDENTIFIER)NULL;             \
  _RESULT_->nilreturned       = FALSE;                        \
  return _RESULT_;                                            \
  }                                                           \
}


/**************************************************************
  static TREE path__FpaCreateAnd(_RESULT_,_TERM_,_KEY_,_CNT_,
                                 _FKT_,_MODE_)
***************************************************************
INPUT:
OUTPUT:
SUMMARY:
EFFECTS:
CAUTION:  Macro!
***************************************************************/
#define path__FpaCreateAnd(_RESULT_,_TERM_,_KEY_,_CNT_,_FKT_,_MODE_) \
{                                                             \
  user__ARG ArgList;                                          \
  int       Position;                                         \
                                                              \
  _RESULT_ = (TREE)NULL;                                      \
                                                              \
  /* Consider all possible subnodes of the AND-node */        \
  for (ArgList = user__ArgList(_TERM_), Position=1;           \
       (!user__ArgEmpty(ArgList));                            \
       ArgList = user__ArgCdr(ArgList), Position++) {         \
                                                              \
    /* Consider variables while searching general. only */    \
    if ((_MODE_==GENERALIZATION_MIN) ||                       \
        !user__TermIsVariable(user__ArgCar(ArgList))) {       \
                                                              \
      TREE New;                                               \
      New = _FKT_(user__ArgCar(ArgList),_KEY_,Position,_CNT_);\
                                                              \
      if (New == (TREE)NULL) {                                \
                                                              \
        path_QueryDeleteHelp(_RESULT_);                       \
        _RESULT_ = (TREE)NULL;                                \
        break;                                                \
                                                              \
      } else if (_RESULT_ == (TREE)NULL)  /* First Loop ! */  \
                                                              \
        _RESULT_ = New;                                       \
                                                              \
      else if (_RESULT_->nodetype!=AND) {                     \
                                                              \
        if (New->nodetype!=AND) {                             \
          TREE _NODE_;                                        \
	  /* None of the roots is an AND-node. */             \
          _NODE_ = (TREE)memory_Malloc(sizeof(TREE_NODE));    \
          _NODE_->nodetype      = AND;                        \
          _NODE_->node.AND.sons = 2;                          \
          _NODE_->node.AND.subs =                             \
            list__Cons(_RESULT_, list__Cons(New,list__Nil()));\
          _NODE_->nilreturned   = FALSE;                      \
          _RESULT_ = _NODE_;                                  \
	} else {                                              \
	  /* Only New is an AND-node. */                      \
          New->node.AND.sons++;                               \
          New->node.AND.subs =                                \
            list__Cons(_RESULT_, New->node.AND.subs);         \
          _RESULT_           = New;                           \
        }                                                     \
                                                              \
      } else {                                                \
                                                              \
        if (New->nodetype!=AND) {                             \
          /* Only _RESULT_ is an AND-node. */                 \
          _RESULT_->node.AND.sons++;                          \
          _RESULT_->node.AND.subs =                           \
            list__Cons(New, _RESULT_->node.AND.subs);         \
        } else {                                              \
          /* Both roots are AND-nodes. */                     \
          _RESULT_->node.AND.sons+= New->node.AND.sons;       \
          _RESULT_->node.AND.subs =                           \
            list__Nconc(_RESULT_->node.AND.subs,              \
                        New->node.AND.subs);                  \
          memory__Free(New, sizeof(TREE_NODE));               \
        }                                                     \
      }                                                       \
    }                                                         \
  }                                                           \
}


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  FPA QUERY TREE CONSTRUCTION                           * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/**************************************************************
static TREE path_FpaQueryCreateInstance(user__TERM Term,
                                      KEY Key,
			 	      int Position,
				      int Counter)
***************************************************************
INPUT:	 A term and a key which describes the path to this
         (Sub-)'Term'. Position contains an integer describing
	 at what position the Term occurs in it's superterm.
	 Counter contains the depth of the key.
	 Mode is one of the constants:
	 UNIFICATION, GENERALIZATION, or INSTANCE.
	 The Mode ALL does not reach this point !
OUTPUT:  A query tree for terms which are instances of 'Term'.
**************************************************************/
TREE path_FpaQueryCreateInstance(Term, Key, Position, Counter)
     user__TERM Term;
     KEY Key;
     int Position;
     int Counter;
{
  /* Store number of components of a key in the first pos. */
  Key[Counter].position	= Position;
  path__KeyLength(Key)	= Counter;
  
  /*********************************************/
  /* Term can never be a variable.             */
  /* Term is a constant .OR.                   */
  /* Term has reached INDEXDEPTH .OR.          */
  /* All arguments are variables (cut required)*/
  /*********************************************/
  if (user__TermIsConstant(Term)
      || (Counter == (INDEXDEPTH-1))
      || user__TermAllArgsAreVar(Term)) {
    
    TREE Leaf;
    Key[Counter].symbol = user__TermTop(Term);
    path__FpaCreateLeaf(Leaf, Key);
    return Leaf;
    
  } else {
    
    TREE AndNode;
    Key[Counter].symbol = user__TermTop(Term);
    Counter++;
    path__FpaCreateAnd(AndNode, Term, Key, Counter,
		       path_FpaQueryCreateInstance, INSTANCE_MIN);
    return AndNode;
  }
}


/**************************************************************
  TREE path_FpaQueryCreateUnify(user__TERM Term,
                              KEY Key,
			      int Position,
			      int Counter)
***************************************************************
INPUT:	 A term and a key which describes the path to this
         (Sub-)'Term'. Position contains an integer describing
	 at what position the Term occurs in it's superterm.
	 Counter contains the depth of the key.
	 Mode is one of the constants:
	 UNIFICATION, GENERALIZATION, or INSTANCE.
OUTPUT:  A query tree for terms unifiable with 'Term'.
**************************************************************/
TREE path_FpaQueryCreateUnify(Term, Key, Position, Counter)
     user__TERM Term;
     KEY Key;
     int Position;
     int Counter;
{
  Key[Counter].position	= Position;
  path__KeyLength(Key)  = Counter;
  
  /*********************************************/
  /* Term is a constant .OR.                   */
  /* Term has reached INDEXDEPTH .OR.          */
  /* All arguments are variables (cut required)*/
  /*********************************************/
  
  if ((!user__TermIsVariable(Term) && user__TermIsConstant(Term))
      || (Counter == (INDEXDEPTH-1))
      || user__TermAllArgsAreVar(Term)) {
    
    TREE Leaf1, Leaf2;

    Key[Counter].symbol = user__SymbolSpecial();
    path__FpaCreateLeaf(Leaf1, Key);
    Key[Counter].symbol = user__TermTop(Term);
    path__FpaCreateLeaf(Leaf2, Key);
    path__FpaCreateOrAndReturn(Leaf1, Leaf2);
    
  } else {
    
    TREE Leaf, AndNode;
    
    Key[Counter].symbol = user__SymbolSpecial();
    path__FpaCreateLeaf(Leaf, Key);
    Key[Counter].symbol = user__TermTop(Term);
    Counter++;
    path__FpaCreateAnd(AndNode, Term, Key, Counter,
		       path_FpaQueryCreateUnify, UNIFIER_MIN);
    path__FpaCreateOrAndReturn(Leaf, AndNode);
  }
}


/**************************************************************
TREE path_FpaQueryCreateGen(Term, Key, Position, Counter)
***************************************************************
INPUT:	 A term and a key which describes the path to this
         (Sub-)'Term'. Position contains an integer describing
	 at what position the Term occurs in it's superterm.
	 Counter contains the depth of the key.
	 Mode is one of the constants:
	 UNIFICATION, GENERALIZATION, or INSTANCE.
	 The Mode ALL does not reach this point !
OUTPUT:  A query tree for terms which are generalizations of 'Term'.
**************************************************************/
TREE path_FpaQueryCreateGen(Term, Key, Position, Counter)
     user__TERM Term;
     KEY Key;
     int Position;
     int Counter;
{
  /* Store number of components of a key in the first pos. */
  Key[Counter].position	= Position;
  path__KeyLength(Key)  = Counter;

  /*********************************************/
  /* Term is a variable.                       */
  /*********************************************/
  if (user__TermIsVariable(Term)) {
    TREE   Leaf;
    Key[Counter].symbol = user__SymbolSpecial();
    path__FpaCreateLeaf(Leaf, Key);
    return Leaf;
  }

  /*********************************************/
  /* Term is a constant .OR.                   */
  /* INDEXDEPTH is reached                     */
  /*********************************************/
  if (user__TermIsConstant(Term)
      || (Counter == (INDEXDEPTH-1))) {

    TREE Leaf1, Leaf2;

    Key[Counter].symbol = user__SymbolSpecial();
    path__FpaCreateLeaf(Leaf1,Key);
    Key[Counter].symbol = user__TermTop(Term);
    path__FpaCreateLeaf(Leaf2,Key);
    path__FpaCreateOrAndReturn(Leaf1, Leaf2);
  
  } else {
    
    TREE Leaf, AndNode;

    Key[Counter].symbol = user__SymbolSpecial();
    path__FpaCreateLeaf(Leaf, Key);
    Key[Counter].symbol = user__TermTop(Term);
    Counter++;
    path__FpaCreateAnd(AndNode, Term, Key, Counter,
		       path_FpaQueryCreateGen, GENERALIZATION_MIN);
    path__FpaCreateOrAndReturn(Leaf, AndNode);
  }
}


/**************************************************************
path_QUERY path_FpaQueryCreateAll(Index)
***************************************************************
INPUT:	 An index.
RETURNS: A query tree root.
**************************************************************/
path_QUERY path_FpaQueryCreateAll(Index)
     path_INDEX Index;
{
  path_QUERY Root;
  
  if (list__Empty(Index->all->connector))
    return (path_QUERY)NULL;

  Root = (path_QUERY)memory_Malloc(sizeof(path_QUERY_NODE));
  Root->index     = Index;
  Root->lastquery = Index->lastid;
  path__LastReturnedId(Root) = START_ID;
  path__MinimizedTree(Root)  = TRUE;

  Root->tree = (TREE)memory_Malloc(sizeof(TREE_NODE));
  Root->tree->nodetype           = LEAF;
  Root->tree->node.LEAF.lastterm = list__Nil();
  Root->tree->node.LEAF.terms    = Index->all;
  Root->tree->nilreturned        = FALSE;

  return Root;
}


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  FUNCTIONS FOR FPA- AND PATH-TREES                     * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/**************************************************************
  void path_QueryDelete(path_QUERY Root)
***************************************************************
INPUT:	 A query tree root.
RETURNS: None.
SUMMARY: Deletes the query tree and returns the memory to the
         memory management module.
***************************************************************/
void path_QueryDelete(Root)
     path_QUERY Root;
{
  if (Root == (path_QUERY)NULL)
    return;

  path_QueryDeleteHelp(Root->tree);

  memory__Free(Root, sizeof(path_QUERY_NODE));
}


/**************************************************************
  POINTER path_QueryApply(path_QUERY Root)
***************************************************************
INPUT:	 A query tree root and an id.
RETURNS: An identifier to an entry of the index.
SUMMARY: Each time path_QueryApply is called, identifiers are
         bubbled up in the query tree until one identifier
	 reaches the root of the query tree whose id is greater
	 than 'Minimum'.
	 This identifier refers to an entry of the index which
	 fulfills all restrictions expressed by the query tree.
**************************************************************/
POINTER path_QueryApply(Root)
     path_QUERY Root;
{
  IDENTIFIER Identifier;

  if (Root == (path_QUERY)NULL)
    return (POINTER)NULL;

  MyIndex = Root->index;

  if (path__MinimizedTree(Root)) {
    Identifier = path_FpaQueryApplyHelp(Root->tree,
					path__LastReturnedId(Root));

    if (Identifier == (IDENTIFIER)NULL) {
      Root->tree = (TREE)NULL;
      return (POINTER)NULL;
    }
  } else {
    if (MyIndex->lastid != Root->lastquery) {
      Root->lastquery = MyIndex->lastid;
      Identifier = path_QueryApplyHelp(Root->tree,
				       path__LastReturnedId(Root),
				       TRUE);
    } else
      Identifier = path_QueryApplyHelp(Root->tree, 
				       path__LastReturnedId(Root),
				       FALSE);

    if (Identifier == (IDENTIFIER)NULL)
      return (POINTER)NULL;
  }

  path__LastReturnedId(Root) = path__IdentifierId(Identifier);
  return path__IdentifierPointer(Identifier);
}


/**************************************************************
path_QUERY path_QueryCreate(Index, Mode, Term)
***************************************************************
INPUT:	 An index.
RETURNS: A query tree root.
**************************************************************/
path_QUERY path_QueryCreate(Index, Mode, Term)
     path_INDEX Index;
     path_MODE  Mode;
     user__TERM Term;
{
  KEY Key;

  MyIndex = (path_INDEX)Index;

  switch (Mode) {

  case ALL:

    return path_QueryCreateAll(Index);

  case INSTANCE:

    if (user__TermIsVariable(Term))
      return path_QueryCreateAll(Index);
    else {
      path_QUERY Root;
      
      Root = (path_QUERY)memory_Malloc(sizeof(path_QUERY_NODE));
      Root->index     = Index;
      Root->lastquery = Index->lastid;
      Root->tree      = path_QueryCreateInstance(Term,Key,0,0);
      path__LastReturnedId(Root) = START_ID;
      path__MinimizedTree(Root)  = FALSE;
      return Root;
    }
    
  case GENERALIZATION:
    
    {
      path_QUERY Root;

      Root = (path_QUERY)memory_Malloc(sizeof(path_QUERY_NODE));
      Root->index     = Index;
      Root->lastquery = Index->lastid;
      Root->tree      = path_QueryCreateGen(Term,Key,0,0);
      path__LastReturnedId(Root) = START_ID;
      path__MinimizedTree(Root)  = FALSE;
      return Root;
    }

  case UNIFIER:

    if (user__TermIsVariable(Term))
      return path_QueryCreateAll(Index);
    else {
      path_QUERY Root;

      Root = (path_QUERY)memory_Malloc(sizeof(path_QUERY_NODE));
      Root->index     = Index;
      Root->lastquery = Index->lastid;
      Root->tree      = path_QueryCreateUnify(Term,Key,0,0);
      path__LastReturnedId(Root) = START_ID;
      path__MinimizedTree(Root)  = FALSE;
      return Root;
    }

  case ALL_MIN:

    return path_FpaQueryCreateAll(Index);

  case INSTANCE_MIN:

    if (user__TermIsVariable(Term))
      return path_FpaQueryCreateAll(Index);
    else {
      TREE Tree;

      Tree = path_FpaQueryCreateInstance(Term,Key,0,0);

      if (Tree == (TREE)NULL)
	return ((path_QUERY)NULL);
      else {
	path_QUERY Root;

	Root = (path_QUERY)memory_Malloc(sizeof(path_QUERY_NODE));
	Root->index     = Index;
	Root->lastquery = Index->lastid;
	Root->tree      = Tree;
	path__LastReturnedId(Root) = START_ID;
	path__MinimizedTree(Root)  = TRUE;
	return Root;
      }
    }
    
  case GENERALIZATION_MIN:
    
    {
      TREE Tree;

      Tree = path_FpaQueryCreateGen(Term,Key,0,0);

      if (Tree == (TREE)NULL)
	return ((path_QUERY)NULL);
      else {
	path_QUERY Root;

	Root = (path_QUERY)memory_Malloc(sizeof(path_QUERY_NODE));
	Root->index     = Index;
	Root->lastquery = Index->lastid;
	Root->tree      = Tree;
	path__LastReturnedId(Root) = START_ID;
	path__MinimizedTree(Root)  = TRUE;
	return Root;
      }
    }

  case UNIFIER_MIN:

    if (user__TermIsVariable(Term))
      return path_FpaQueryCreateAll(Index);
    else {
      TREE Tree;

      Tree = path_FpaQueryCreateUnify(Term,Key,0,0);

      if (Tree == (TREE)NULL)
	return ((path_QUERY)NULL);
      else {
	path_QUERY Root;

	Root = (path_QUERY)memory_Malloc(sizeof(path_QUERY_NODE));
	Root->index     = Index;
	Root->lastquery = Index->lastid;
	Root->tree      = Tree;
	path__LastReturnedId(Root) = START_ID;
	path__MinimizedTree(Root)  = TRUE;
	return Root;
      }
    }

  default:
    Error__Out(("path_QueryCreate: Incorrect Mode"));
    return (path_QUERY)NULL;
  }
}


/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  OUTPUT FUNCTIONS (DEBUGGING ONLY)                     * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


#if DEBUG

void path_PrintKey(Key)
/**************************************************************
INPUT:   A 'Key'.
RETURNS: None.
SUMMARY: Prints the key to stdout.
**************************************************************/
KEY Key;
{
  int i;
  
  printf("[%d - ", path__KeyLength(Key));
  symbol_Print(Key[0].symbol);

  for (i=1; i <= path__KeyLength(Key); i++) {
    printf("%d", Key[i].position);
    symbol_Print(Key[i].symbol);
  }
 
  printf("]");
}

void path_PrintTreeHelp(Tree, level)
/**************************************************************
INPUT:   A tree and an integer which determines the indention
         of the outut. Originally this integer should have the
	 value 0.
RETURNS: None.
SUMMARY: Prints the query tree to stdout.
**************************************************************/
TREE Tree;
int  level;
{
  LIST List;
  int i;
  
  if (Tree==NULL)
    return;

  for (i=0; i<level; i++)
    printf("   ");
  
  switch (Tree->nodetype) {

  case LEAF:

    path_PrintKey(Tree->node.LEAF.terms->key);
    printf(" -> ");/*
    if (!list__Empty(Tree->node.LEAF.terms->connector))
      term_Print(list__Car(Tree->node.LEAF.terms->connector));
    else
      printf("NULL");*/
    printf("\n");
    break;
    
  case AND:
    printf("and\n");
    for (List = Tree->node.AND.subs;
	 !list__Empty(List);
	 List = list__Cdr(List))
      path_PrintTreeHelp(list__Car(List), level+1);
    break;
    
  case OR:
    printf("or\n");
    path_PrintTreeHelp(Tree->node.OR.left, level+1);
    path_PrintTreeHelp(Tree->node.OR.right, level+1);
    break;
  }
}

void path_PrintTree(Root, level)
/**************************************************************
INPUT:   A tree and an integer which determines the indention
         of the outut. Originally this integer should have the
	 value 0.
RETURNS: None.
SUMMARY: Prints the query tree to stdout.
**************************************************************/
path_QUERY Root;
int        level;
{
  if (Root==NULL)
    return;

  path_PrintTreeHelp(Root->tree, level);
}

void path_Print(Index)
/**************************************************************
INPUT:   An index.
RETURNS: None.
SUMMARY: Prints the keys that the index contains.
**************************************************************/
path_INDEX Index;
{
  HEADER Header;
  int i;
  
  for (i=0; i < HASHMAX; i++) {
    for (Header = Index->table[i];
	 Header != NULL;
	 Header = Header->next) {
      printf("%d: ",i);
      path_PrintKey(Header->key);
      list_Apply(term_Print, Header->connector);
      printf("\n");
    }
  }
}

#endif
