/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  HEADER:    list.c                                     * */
/* *                                                        * */
/* *  VERSION:   1.1                                        * */
/* *  MODIFIED:  May 2  1993 by Weidenbach                  * */
/* *                                                        * */
/* *  AUTHORS:   P. Graf (graf@mpi-sb.mpg.de)               * */
/* *             C. Weidenbach (weidenb@mpi-sb.mpg.de)      * */
/* *             MPI fuer Informatik                        * */
/* *             Im Stadtwald                               * */
/* *             6600 Saarbruecken                          * */
/* *             Germany                                    * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/

#include "path/list.h"

/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  MEMORY MANAGEMENT                                     * */
/* *                                                        * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/

#if (CC)
LIST list__Cons(Pointer, List)
     POINTER Pointer;
     LIST List;
{ 
  LIST New;
  New = (LIST)memory_Malloc(sizeof(LIST_NODE));
  New->car = Pointer;
  New->cdr = List;
  return New;
}

LIST list__Nconc(List1, List2)
     LIST List1;
     LIST List2;
{
  if (list__Empty(List1)) 
    return List2;
  else if (list__Empty(List2)) 
    return List1; 
  else { 
    LIST Current; 
    for (Current = List1; 
	 !list__Empty(list__Cdr(Current)); 
	 Current = list__Cdr(Current)) ; 
    Current->cdr = List2; 
    return List1; 
  } 
}

LIST list__Append(List1, List2)
     LIST List1, List2;
{
  if (list__Empty(List1)) 
    return List2;
  else if (list__Empty(List2)) 
    return list_Copy(List1); 
  else { 
    LIST Current; 
    List1 = list_Copy(List1); 
    for (Current = List1; 
	 !list__Empty(list__Cdr(Current)); 
	 Current = list__Cdr(Current)) ; 
    Current->cdr = List2;
    return List1;
  } 
}

#endif

LIST    list_Copy(List)
/**************************************************************
INPUT:   A List.
RETURNS: The copy of the list.
CAUTION: The entries of the list are NOT copied !
***************************************************************/
LIST List;
{
  LIST Copy;
  LIST Scan1,Scan2;

  if (list__Empty(List))
    return(list__Nil());

  Copy = list__List(list__Car(List));
  Scan1 = Copy;
  Scan2 = list__Cdr(List);

  while (!list__Empty(Scan2)) {
    list__Rplacd(Scan1, list__List(list__Car(Scan2)));
    Scan1 = list__Cdr(Scan1);
    Scan2 = list__Cdr(Scan2);
  }
  return Copy;
}

LIST    list_CopyWithElement(List, CopyElement)
/**************************************************************
INPUT:   A List and a copy function for the elements.
RETURNS: The copy of the list.
CAUTION: The entries of the list are NOT copied !
***************************************************************/
LIST    List;
POINTER (*CopyElement)();
{
  LIST Copy;
  LIST Scan1,Scan2;

  if (list__Empty(List))
    return(list__Nil());

  Copy = list__List(CopyElement(list__Car(List)));
  Scan1 = Copy;
  Scan2 = list__Cdr(List);

  while (!list__Empty(Scan2)) {
    list__Rplacd(Scan1, list__List(CopyElement(list__Car(Scan2))));
    Scan1 = list__Cdr(Scan1);
    Scan2 = list__Cdr(Scan2);
  }
  return Copy;
}

LIST    list_CopyElements(List, CopyElement)
/**************************************************************
INPUT:   A List and a copy function for the elements.
RETURNS: The List where all elements are copied
CAUTION: The List is not copied !
***************************************************************/
LIST    List;
POINTER (*CopyElement)();
{
  LIST Scan;

  Scan = List;

  while (!list__Empty(Scan)) {
    list__Rplaca(Scan, CopyElement(list__Car(Scan)));
    Scan = list__Cdr(Scan);
  }
  return List;
}

void list_DeleteWithElement(List, ElementDelete)
/**************************************************************
INPUT:   A List and a delete function for the elements.
RETURNS: void.
EFFECT:  The List and all elements are deleted.
***************************************************************/
LIST List;
void (*ElementDelete)();
{
  LIST Scan;

  while (!list__Empty(List)) {
    Scan = list__Cdr(List);
    ElementDelete(list__Car(List));
    list__Free(List);
    List = Scan;
    }
}


LIST list_Reverse(List)
/**************************************************************
INPUT:	 A list
RETURNS: A list where the order of the lements is reversed.
***************************************************************/
LIST List;
{
  LIST ReverseList;
  LIST Scan;
  
  ReverseList = list__Nil();

  for (Scan=List;!list__Empty(Scan);Scan=list__Cdr(Scan)) 
    ReverseList = list__Cons(list__Car(Scan),ReverseList);

  return(ReverseList);
}

LIST list_NReverse(List)
/**************************************************************
INPUT:	 A list
RETURNS: The same list with it's order being reversed.
CAUTION: Destructive.
***************************************************************/
LIST List;
{
  LIST ReverseList;
  LIST Scan1,Scan2;
  
  ReverseList = list__Nil();

  for (Scan1=List;!list__Empty(Scan1);Scan1=list__Cdr(Scan1)) 
    ReverseList = list__Cons(list__Car(Scan1),ReverseList);

  for (Scan1=List,Scan2=ReverseList;
       !list__Empty(Scan1);
       Scan1=list__Cdr(Scan1),Scan2=list__Cdr(Scan2)) 
    Scan1->car = Scan2->car;

  list__Delete(ReverseList);
  return(List);
}

LIST list_Sort(List, Test)
/**************************************************************
INPUT:	 A list and a 'less' function on the elements.
RETURNS: The same list where the elements are sorted with respect to Test.
CAUTION: Destructive.
***************************************************************/
LIST List;
BOOL (*Test)();
{
  LIST Scan1,Scan2,Min;
  POINTER Exchange;
  
  for (Scan1=List;!list__Empty(Scan1);Scan1=list__Cdr(Scan1)) {
    Min = Scan1;
    for (Scan2=list__Cdr(Scan1);!list__Empty(Scan2);Scan2=list__Cdr(Scan2)) {
      if (Test(list__Car(Scan2), list__Car(Min))) {
	Exchange = list__Car(Min);
	list__Car(Min) = list__Car(Scan2);
	list__Car(Scan2) = Exchange;
      }
    }
  }

  return(List);
}

LIST list_PointerSort(List)
/**************************************************************
INPUT:	 A list
RETURNS: The same list where the elements are sorted as pointers.
CAUTION: Destructive.
***************************************************************/
LIST List;
{
  LIST Scan1,Scan2,Min;
  POINTER Exchange;
  
  for (Scan1=List;!list__Empty(Scan1);Scan1=list__Cdr(Scan1)) {
    Min = Scan1;
    for (Scan2=list__Cdr(Scan1);!list__Empty(Scan2);Scan2=list__Cdr(Scan2)) {
      if ((unsigned int)list__Car(Scan2)<(unsigned int)list__Car(Min)) {
	Exchange = list__Car(Min);
	list__Car(Min) = list__Car(Scan2);
	list__Car(Scan2) = Exchange;
      }
    }
  }

  return(List);
}



POINTER list_DequeueNext(List)
/**************************************************************
INPUT:	 A list
RETURNS: A pointer to a dequeued element.
SUMMARY: We dequeue the element pointed to by list__Cdr(List);
***************************************************************/
LIST List;
{
  POINTER Pointer;
  LIST    Memo;

  if (list__Empty(List)) return (POINTER)NULL;

  Memo = list__Cdr(List);

  if (list__Empty(Memo)) return (POINTER)NULL;
      
  Pointer = list__Car(Memo);
  List->cdr = Memo->cdr;
  memory__Free(Memo,sizeof(LIST_NODE));
  return Pointer;
}



void list_InsertNext(List, Pointer)
/**************************************************************
INPUT:	 A list and a pointer to anything.
RETURNS: A list with Pointer being added at the position that
         follows List.
SUMMARY: We enqueue the element at position list__Cdr(List);
***************************************************************/
LIST List;
POINTER Pointer;
{
#if DEBUG
  if (list__Empty(List)) {
    printf("\n\n NULL Pointer in list_InsertNext\n");
    DumpCore();
  }
#endif

  list__Rplacd(List, list__Cons(Pointer, list__Cdr(List)));
}



void list_Apply(Function, List)
/**************************************************************
INPUT:	 A pointer to a non-resulting function.
SUMMARY: Apply the function to all members of the list.
***************************************************************/
void (*Function)();
LIST List;
{
  while (!list__Empty(List)) {
    Function(list__Car(List));
    List = list__Cdr(List);
  }
}

LIST list_DeleteElementFree(List, Element, Test, Free)
/**************************************************************
INPUT:	 A list, an element pointer, an equality test for elements
         and a free function for elements
RETURNS: The list where Element is deleted from List with respect to Test.
EFFECTS: If List contains Element with respect to EqualityTest,
         Element is deleted from List and freed
CAUTION: Destructive. Be careful, the first element of a list cannot
         be changed destructively by call by reference.
***************************************************************/
LIST    List;
POINTER Element;
BOOL    (*Test)();
void    (*Free)();
{
  LIST   Scan1,Scan2;

  while(!list__Empty(List) && Test(Element, list__Car(List))) {
    Scan1 = list__Cdr(List);
    Free(list__Car(List));
    list__Free(List);
    List = Scan1;
  }

  if (list__Empty(List)) return(list__Nil());
  
  Scan2 = List;
  Scan1 = list__Cdr(List);

  while(!list__Empty(Scan1)) {
    if (Test(Element, list__Car(Scan1))) {
      list__Cdr(Scan2) = list__Cdr(Scan1);
      Free(list__Car(Scan1));
      list__Free(Scan1);
      Scan1 = list__Cdr(Scan2);
    }
    else {
      Scan2 = Scan1;
      Scan1 = list__Cdr(Scan1);
    }
  }
  return(List);     
}


LIST list_DeleteElement(List, Element, Test)
/**************************************************************
INPUT:	 A list, an element pointer, an equality test for elements
RETURNS: The list where Element is deleted from List with respect to Test.
EFFECTS: If List contains Element with respect to EqualityTest,
         Element is deleted from List
CAUTION: Destructive. Be careful, the first element of a list cannot
         be changed destructively by call by reference.
***************************************************************/
LIST    List;
POINTER Element;
BOOL    (*Test)();
{
  LIST   Scan1,Scan2;

  while(!list__Empty(List) && Test(Element, list__Car(List))) {
    Scan1 = list__Cdr(List);
    list__Free(List);
    List = Scan1;
  }

  if (list__Empty(List)) return(list__Nil());
  
  Scan2 = List;
  Scan1 = list__Cdr(List);

  while(!list__Empty(Scan1)) {
    if (Test(Element, list__Car(Scan1))) {
      list__Cdr(Scan2) = list__Cdr(Scan1);
      memory__Free(Scan1, sizeof(LIST_NODE));
      Scan1 = list__Cdr(Scan2);
    }
    else {
      Scan2 = Scan1;
      Scan1 = list__Cdr(Scan1);
    }
  }
  return(List);     
}


LIST list_PointerDeleteElement(List, Element)
/**************************************************************
INPUT:	 A list and an element pointer
RETURNS: The list where Element is deleted from List with to pointer equality.
EFFECTS: If List contains Element with respect to pointer equality,
         Element is deleted from List
CAUTION: Destructive. Be careful, the first element of a list cannot
         be changed destructively by call by reference.
***************************************************************/
LIST    List;
POINTER Element;
{
  LIST   Scan1,Scan2;

  while(!list__Empty(List) && Element == list__Car(List)) {
    Scan1 = list__Cdr(List);
    memory__Free(List, sizeof(LIST_NODE));
    List = Scan1;
  }

  if (list__Empty(List)) return(list__Nil());
  
  Scan2 = List;
  Scan1 = list__Cdr(List);

  while(!list__Empty(Scan1)) {
    if (Element == list__Car(Scan1)) {
      list__Cdr(Scan2) = list__Cdr(Scan1);
      memory__Free(Scan1, sizeof(LIST_NODE));
      Scan1 = list__Cdr(Scan2);
    }
    else {
      Scan2 = Scan1;
      Scan1 = list__Cdr(Scan1);
    }
  }
  return(List);     
}

BOOL list_Member(List, Element, Test)
/**************************************************************
INPUT:	 A list and an element pointer and an equality test for two elements.
RETURNS: TRUE iff Element is in List with respect to Test
***************************************************************/
LIST    List;
POINTER Element;
BOOL    (*Test)();
{
  while (!list__Empty(List)) {
    if (Test(Element, list__Car(List)))
      return TRUE;
    List = list__Cdr(List);
  }
  
  return FALSE;
}

BOOL list_PointerMember(List, Element)
/**************************************************************
INPUT:	 A list and an element pointer.
RETURNS: TRUE iff Element is in List with respect to pointer equality.
***************************************************************/
LIST    List;
POINTER Element;
{
  while (!list__Empty(List)) {
    if (Element== list__Car(List))
      return TRUE;
    List = list__Cdr(List);
  }
  
  return FALSE;
}

LIST list_DeleteDuplicatesFree(List, Test, Free)
/**************************************************************
INPUT:	 A list, an equality test for elements, and a free for elements
RETURNS: The list where multiple occurrances are deleted.
CAUTION: Destructive and frees all duplicates.
***************************************************************/
LIST List;
BOOL (*Test)();
void (*Free)();
{
  LIST Scan;
  
  Scan = List;

  while (!list__Empty(Scan)) {
    list__Rplacd(Scan, list_DeleteElementFree(list__Cdr(Scan), list__Car(Scan), Test, Free));
    Scan = list__Cdr(Scan);
  }
  return List;
}

LIST list_DeleteDuplicates(List, Test)
/**************************************************************
INPUT:	 A list, an equality test for elements
RETURNS: The list where multiple occurrances are deleted.
CAUTION: Destructive.
***************************************************************/
LIST List;
BOOL (*Test)();
{
  LIST Scan;
  
  Scan = List;

  while (!list__Empty(Scan)) {
    list__Rplacd(Scan, list_DeleteElement(list__Cdr(Scan), list__Car(Scan), Test));
    Scan = list__Cdr(Scan);
  }
  return List;
}

LIST list_PointerDeleteDuplicates(List)
/**************************************************************
INPUT:	 A list
RETURNS: The list where multiple occurrances are deleted.
CAUTION: Destructive.
***************************************************************/
LIST List;
{
  LIST Scan;
  
  Scan = List;

  while (!list__Empty(Scan)) {
    list__Rplacd(Scan, list_PointerDeleteElement(list__Cdr(Scan), list__Car(Scan)));
    Scan = list__Cdr(Scan);
  }
  return List;
}

LIST list_PointerNUnion(List1, List2)
/**************************************************************
INPUT:	 Two lists.
RETURNS: Regarding both lists as sets, the union of the sets.
CAUTION: Destructive.
***************************************************************/
LIST List1,List2;
{
  return(list_PointerDeleteDuplicates(list__Nconc(List1,List2)));
}

LIST list_NUnion(List1, List2, Test)
/**************************************************************
INPUT:	 Two lists and an equality test for the elements.
RETURNS: Regarding both lists as sets, the union of the sets.
CAUTION: Destructive.
***************************************************************/
LIST List1,List2;
BOOL (*Test)();
{
  return(list_DeleteDuplicates(list__Nconc(List1,List2), Test));
}

LIST list_NIntersect(List1, List2, Test)
/**************************************************************
INPUT:	 Two lists and an equality test for the elements.
RETURNS: Regarding both lists as sets, the intersection of the sets.
CAUTION: Destructive on List1
***************************************************************/
LIST List1,List2;
BOOL (*Test)();
{
  LIST Scan1, Scan2;
  
  while (!list__Empty(List1) && !list_Member(List2, list__Car(List1), Test)) {
    Scan1 = list__Cdr(List1);
    list__Free(List1);
    List1 = Scan1;
  }

  if (list__Empty(List1))
    return(List1);

  Scan1 = List1;
  Scan2 = list__Cdr(List1);

  while(!list__Empty(Scan2)) {
    if (list_Member(List2, list__Car(Scan2), Test)) {
      Scan2 = list__Cdr(Scan2);
      Scan1 = list__Cdr(Scan1);
    }
    else {
      list__Rplacd(Scan1, list__Cdr(Scan2));
      list__Free(Scan2);
      Scan2 = list__Cdr(Scan1);
    }
  }
  return(List1);
}
      
  
    
  
