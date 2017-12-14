#ifndef ALIST_H
#define ALIST_H

#include <iostream.h>
#include <assert.h>

/***************************************************************************

  WARNING

  This defines a list class with the following nice properties:
  
    * singly linked list => space efficient.
    * so it can be used as a list, stack or a queue (allows InsertAtEnd).
    * allows you to delete the current element in constant time.
    * a nice convenient iterator class.

  The code is a little dirty due to various special cases, but
    * it all works, and
    * trust me, there is no simpler way to make it work.

  So, dont make changes in this code unless you are very sure of what you
  are doing. 

***************************************************************************/


/***************************************************************************

  this file has the definitions of the template for the Alist_t class.
  class Link is a private class used by the Alist_t class.
  each element of the a list is stored in one instance of the Link class.

***************************************************************************/

// each element of a list is stored in one instance of class Link.
template <class ListElementType> class Alist_t;

template <class ListElementType> 
class Link {
friend class Alist_t<ListElementType>;
private:
  ListElementType *value;      // the actual element is stored here.
  Link<ListElementType> *next; // next element in the list.

private: // this is a private class. Only Alist_t<ListElementType> can use it.

  // constructor
  Link (ListElementType *element=0, Link<ListElementType> *newnext=0) {
    value = element; next = newnext;
  }

};





/***************************************************************************

  a list is actually a chain of circularly linked instances of class Link.
  a "head" pointer always points to the first element of the list.
  a list is empty if the head points to NULL (zero).

  a "current" pointer is used for traversals through the list.

***************************************************************************/

template <class ListElementType>
class Alist_t {
private:
  Link<ListElementType> *head; // points to first element of list.
  Link<ListElementType> *current; // used for traversals through the list.

public:
  // construct an empty list.
  Alist_t (void) {head = current = 0;} 

  // construct a list containing a single element
  Alist_t (ListElementType *element) {
    head = current = new Link<ListElementType> (element);
    head->next = head; // remember, its a circularly linked list.
  }    

  // return non zero if list is empty.
  int IsEmpty (void) {return head == 0;} 

  // start a traversal of the list (from the first element).
  // returns non zero if list is empty. zero otherwise.
  // at any given time, only one traversal can be in progress.
  int StartTraversal (void) {return (current = head) == 0;}


  // return the current element of the list.
  ListElementType *Element (void) {
    if (IsEmpty ()) return 0;
    else return current->value;
  }

  // advance the "current" pointer to the next element.
  // used for a traversal. 
  // it returns non zero if an attempt is made to advance the
  // "current" pointer past the last element of a list
  // and returns zero other wise.
  // it is an error to advance on an empty Alist_t.
  int Advance (void) {
    if (IsEmpty ()) return 1; // you are already at end of list.
    else return (current = current->next) == head; // non zero if end of list.
  }

  // Insert a new element before current element and make that element current
  // if list is empty then make new element the first as well as
  // the current element of the list.
  void Insert (ListElementType *element) {
    if (IsEmpty ())
      {
	head = current = new Link<ListElementType> (element);
	head->next = head; // remember, its a circularly linked list.
      }
    else
      // insert a new Link after the current element,
      // copy contents of the current element to the new Link
      // and replace contents of the current element by the new contents.
      {
	current->next = new Link<ListElementType> (current->value,
						   current->next);
	current->value = element;
      }
  }
   
  // Insert a new element at the end of the list
  // if list is empty then make new element the first as well as
  // the current element of the list.
  void InsertAtEnd (ListElementType *element) {
    if (IsEmpty ())
      {
	head = current = new Link<ListElementType> (element);
	head->next = head; // remember, its a circularly linked list.
      }
    else
      // insert a new Link after the head,
      // copy contents of the head to the new Link
      // and replace contents of the head element by the new contents.
      // and finally move head to the new Link...
      {
	head->next = new Link<ListElementType> (head->value, head->next);
	head->value = element;
	if (current == head)
	  current = current->next;
	head = head->next;
      }
  }

  // Delete the current element and 
  // advance the "current" pointer to the next element.
  // used for a traversal. 
  // it returns zero if last element is deleted
  // and returns non zero other wise.
  // it is and error to use this on an empty list.
  int DeleteAndAdvance (void) {
    int _absurdRetVal = 0;
    
    if (IsEmpty()) _absurdRetVal = 1; // you are already at end of list.

    int deletingthefirstelement;
    
    if (_absurdRetVal == 0)
      {
	if (head == current) 
	  deletingthefirstelement = 1; 
		// we`re trying to delete the first element
	else
	  deletingthefirstelement = 0;
    
	Link<ListElementType> *tmp = current->next;
    
	if (tmp == head)
	  if (tmp->next == head)
	    {
	      // if we come here it means that there is only one element
	      // in the list and that is to be deleted.
	      delete tmp;
	      head = current = 0;
	      _absurdRetVal = 1;
	    }
	  else
	    {
	      // we are trying to delete the last element of the list.
	      head = current; // think about this and convince yourself that
	      // this is the right thing to do.
	      // when the dust has settled, current and head
	      // will both point to the head of the list.
	    }

	if (!_absurdRetVal)
	  {
	    *current = *tmp;
	    delete tmp;
	    if (head == current && !deletingthefirstelement)
	      _absurdRetVal = 1; // we deleted the last element.
	  }
      }
    return _absurdRetVal;
  }

  // find (first occurence) of a given element from a list and delete it.
  // returns zero if the element was not found. 
  int FindAndDelete (ListElementType *element) {
    Link<ListElementType> *oldcurrent = current; 
    for (int endoflist = StartTraversal (); !endoflist; endoflist = Advance ())
      if (Element () == element) {
	if (oldcurrent == current || oldcurrent == current->next)
	  DeleteAndAdvance (); // no need to reset current.
	else
	  {
	    DeleteAndAdvance ();
	    assert (current);
	    current = oldcurrent;
	  }
	return 1;
      }
    if (current)
      current = oldcurrent;
    return 0;
  }

  // delete the whole list
  void DeleteList (void) {
    for (int endoflist = StartTraversal (); !endoflist; 
	 endoflist = DeleteAndAdvance ());
  }
  
  // give number of elements of the list
  int Length (void)
  {
    int length = 0;
    for (int endoflist = StartTraversal ();
	 !endoflist; endoflist = Advance ())
      length++;
    return length;
  }

  // given a list of N elements, allocate an array of N elements
  // and initialize it with the elements of the list.
  ListElementType **ListToArray (int *arraysize = 0) {
    int length = Length ();
    if (arraysize)
      *arraysize = length;

    ListElementType **array = new ListElementType * [length];

    int i; int endoflist;
    Link<ListElementType> *oldcurrent = current;
    for (i=0, endoflist = StartTraversal (); 
	 !endoflist; endoflist = Advance (), i++)
      array[i] = Element ();
    current = oldcurrent;
    return array;
  }

  // get the first element of the list.
  ListElementType *FirstElement (void) {
    if (head) return head->value;
    else return 0;
  }

  // delete the first element of the list and return it.
  // you should not do this in the middle of a traversal.
  ListElementType *PopFirstElement (void) {
    ListElementType *firstelement = FirstElement ();
    if (firstelement)
      {
	StartTraversal ();
	DeleteAndAdvance ();
      }
    return firstelement;
  }
};


// OK OK. i know that this is not a very good thing to do.
// and that it is rotten programming. but considering the
// benefits, i think it is worth the risk.
// 
// the way you are supposed to use this is
//     FOR_EACH_ELEMENT_OF_LIST (list) {statement;statement;list.Element ();}
// statement can be a single statement followed by a semicolon
// or it can be a compound statement enclosed in {}s.
// 
// you can even nest two (or more) of these constructs and it
// will still work because you are not allowed to use the value
// of __iterator inside the loops. (write an example of your own
// and figure out that it acutally works).
//
// DO NOT make l (the argument of this macro) a complex expression
// with side effects. because it is going to be evaluated twice.
// just be careful of what you put in there and you will be alright.

#include <Aglob_vars.h>
inline int &_A_eol_ (void) {return Aglob_vars()->end_of_list;}

#define FOR_EACH_ELEMENT_OF_LIST(l) for (_A_eol_()=(l).StartTraversal (); \
                                         !_A_eol_(); \
                                         _A_eol_()=(l).Advance ())

#define FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION(l) \
                                     for (_A_eol_()=(l).StartTraversal (); \
                                         !_A_eol_(); \
                                         _A_eol_()=(l).DeleteAndAdvance ())


#endif
