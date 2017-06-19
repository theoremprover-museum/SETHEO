/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  HEADER:    list.h                                     * */
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

#ifndef _LIST_
#define _LIST_

/**************************************************************/
/* Includes                                                   */
/**************************************************************/

#include "path/newmemory.h"
#include "path/types.h" 


/**************************************************************/
/* Macros                                                     */
/**************************************************************/


#define list__Nil()       ((LIST)NULL)
#define list__Empty(L)    (((LIST)L)==(LIST)NULL)
#define list__Car(L)      (((LIST)L)->car)
#define list__Cdr(L)      (((LIST)L)->cdr)
#define list__List(_PTR_) list__Cons(_PTR_,list__Nil())

#if (CC)

LIST list__Cons();
LIST list__Nconc();
LIST list__Append();

#else

#define list__Cons(_PTR_,_LIST_) \
({ LIST _NEW_; \
   _NEW_ = (LIST)memory_Malloc(sizeof(LIST_NODE)); \
  _NEW_->car = _PTR_; \
  _NEW_->cdr = _LIST_; \
  _NEW_; })

#define list__Nconc(_LIST1_,_LIST2_) \
({ LIST _Result_;  \
   LIST _Buf1_; \
   LIST _Buf2_; \
   \
   _Buf1_ = _LIST1_; \
   _Buf2_ = _LIST2_; \
   if (list__Empty(_Buf1_)) \
     _Result_ = _Buf2_; \
   else if (list__Empty(_Buf2_)) \
     _Result_ = _Buf1_; \
   else { \
     _Result_ = _Buf1_; \
     for (;!list__Empty(list__Cdr(_Buf1_));_Buf1_=list__Cdr(_Buf1_));\
     _Buf1_->cdr = _Buf2_; \
   } \
   _Result_; \
})

#define list__Append(_LIST1_,_LIST2_) \
({ LIST _Result_;  \
   LIST _Buf1_; \
   LIST _Buf2_; \
   \
   _Buf1_ = _LIST1_; \
   _Buf2_ = _LIST2_; \
   if (list__Empty(_Buf1_)) \
     _Result_ = _Buf2_; \
   else if (list__Empty(_Buf2_)) \
     _Result_ = list_Copy(_Buf1_); \
   else { \
     _Result_ = list_Copy(_Buf1_); \
     for (_Buf1_ = _Result_; \
	  !list__Empty(list__Cdr(_Buf1_)); \
	  _Buf1_ = list__Cdr(_Buf1_)) ; \
     _Buf1_->cdr = _Buf2_; \
   } \
   _Result_; \
})

#endif

#define list__Free(_LIST_)  memory__Free(_LIST_,sizeof(LIST_NODE))

#define list__Delete(_LIST_) \
{ \
    LIST _Current_; \
    LIST _Del_; \
    \
    _Del_=_LIST_; \
    _Current_ = _Del_; \
    while (!list__Empty(_Current_)) { \
      _Del_ = list__Cdr(_Del_); \
      list__Free(_Current_); \
      _Current_ = _Del_; \
    } \
}


/* Assertions: _MEMO_ is a variable of type list.
               _LIST1_ is empty at the beginning. */
#define list__Sonc(_LIST_,_ELEMENT_,_MEMO_) {           \
  if (_LIST_ == NULL) {                                 \
    _LIST_ = list__Cons(_ELEMENT_, list__Nil());        \
    _MEMO_  = _LIST_;                                   \
  } else {                                              \
    _MEMO_->cdr = list__Cons(_ELEMENT_, list__Nil());   \
    _MEMO_ = _MEMO_->cdr;                               \
  }                                                     \
}

#define list__Rplacd(_LIST_,_OBJECT_)   _LIST_->cdr = _OBJECT_
#define list__Rplaca(_LIST_,_PTR_)      _LIST_->car = _PTR_


/**************************************************************/
/* Functions on Lists                                         */
/**************************************************************/

LIST    list_Copy();
LIST    list_CopyWithElement();
LIST    list_CopyElements();
void    list_DeleteWithElement();
LIST    list_Reverse();
LIST    list_NReverse();
LIST    list_PointerSort();
LIST    list_Sort();
LIST    list_DeleteElement();
LIST    list_DeleteElementFree();
LIST    list_PointerDeleteElement();
LIST    list_DeleteDuplicates();
LIST    list_DeleteDuplicatesFree();
LIST    list_PointerDeleteDuplicates();
LIST    list_PointerNUnion();
LIST    list_NUnion();
LIST    list_NIntersect();
BOOL    list_Member();
BOOL    list_PointerMember();
POINTER list_DequeueNext();
void    list_InsertNext();
void    list_Apply();

#endif
