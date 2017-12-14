#ifndef ASET_H
#define ASET_H

#include "Abitmap.h"
/***************************************************************************

  Defines a template of the Aset_t class. This uses the Abitmap_t class and
  builds on top of that. The set is implemented as a bitmap. This assumes
  a fixed and finite "Universe"  i.e. the elements which can be in the
  Aset_t is fixed and finite (and is a small number). Thus the set is represented
  as a bitmap: a zero at the Nth position indicates that the Nth element
  is not present in the given Aset_t and a 1 in that position says that the
  Nth element is there in the Aset_t. 
  Thus the template has to be instantiated with
  a class which has the following member functions:

      // gives size of the "Universe"
      // i.e. gives the total number of
      // elements that can be there in the Aset_t.
      // this has to be a static member function.
      int SetElementType::TotalNumber (void);

      // gives the number (N) of this element.
      // thus the absence or presence of this
      // element in any Aset_t is indicated by a 0
      // or a 1 in the Nth position in the bitmap.
      int SetElementType.Number (void);

      // given an integer N it returns a pointer to the
      // Nth element of that class. 
      SetElementType *SetElementType::NthElement (int N);

***************************************************************************/

template <class SetElementType>
class Aset_t : private Abitmap_t {
private:
  int currentelement; // for traversals.

public:
  Aset_t (void) : Abitmap_t (SetElementType::TotalNumber ()) {}
//  virtual ~Aset_t(){};  // Keep compiler happy.

  Aset_t (const Aset_t<SetElementType> &otherset) 
    : Abitmap_t (otherset, SetElementType::TotalNumber ()) {}
  
  Aset_t<SetElementType> &operator= (const Aset_t<SetElementType> &otherset) {
    Abitmap_t::Copy (otherset, SetElementType::TotalNumber ()); return *this;
  }

  Aset_t<SetElementType> &Insert (const SetElementType *otherelement) {
    TurnOn (otherelement->Number ()); return *this;
  }
  Aset_t<SetElementType> &Delete (SetElementType *otherelement) {
    TurnOff (otherelement->Number ()); return *this;
  }
  int HasElement (const SetElementType *element) const {
    return IsOn (element->Number ());
  }
  int IsEmpty (void) const {
    return Abitmap_t::IsEmpty (SetElementType::TotalNumber ());
  }
  int Cardinality (void) const {
    return NumberOfBitsOn (SetElementType::TotalNumber ());
  }
  int IsEqualTo (const Aset_t<SetElementType> &otherset) const {
    return Abitmap_t::IsEqualTo (otherset, SetElementType::TotalNumber ());
  }
  // actually this is useful only for singletons...
  SetElementType *FirstElement (void) { 
    int n = FirstBit (SetElementType::TotalNumber ());
    return n == -1 ? 0 :  SetElementType::NthElement (n);
  }

  Aset_t<SetElementType> &Union (const Aset_t<SetElementType> &otherset) {
    Or (otherset, SetElementType::TotalNumber ()); return *this;
  }

  Aset_t<SetElementType> &Intersect (const Aset_t<SetElementType> &otherset) {
    And (otherset, SetElementType::TotalNumber ()); return *this;
  }

  Aset_t<SetElementType> &Minus (const Aset_t<SetElementType> &otherset) {
    Abitmap_t::Minus (otherset, SetElementType::TotalNumber ()); return *this;
  }

  Aset_t<SetElementType> &Complement (void) {
    Abitmap_t::Not (SetElementType::TotalNumber ()); return *this;
  }
  
  int IsFull (void) {
    return Abitmap_t::AreAllBitsOn (SetElementType::TotalNumber ());
  }

  Aset_t<SetElementType> &Clear (void) {
    Abitmap_t::Clear (SetElementType::TotalNumber ()); return *this;
  }

  int IsSubsetOf (const Aset_t<SetElementType> &otherset) const {
    return Abitmap_t::IsSubsetOf (otherset, SetElementType::TotalNumber ()); 
  }

  int MaxElements (void) const {return SetElementType::TotalNumber ();}



  // the next following functions allow us to loop through all
  // the elements of a set. (these are very similar to the
  // the corresponding functions in the Alist_t class).
  // the correct way to use these is 
  //       for (int endofset = set.StartTraversal ();
  //            !endofset;
  //            endofset = set.Advance ())
  //          ...code containing set.CurrentElement ()...
  //
  // DeleteAndAdvance can be used in the place of Advance above

protected:
  // this function is used by StartTraversal, Advance and DeleteAndAdvance
  // it increments currentelement until it points to the next nonzero
  // entry in the Abitmap_t and returns zero if one was found
  // returns 1 if none found.
  int AdvanceToNextElement (void) {
    for (currentelement++; 
	 currentelement < SetElementType::TotalNumber ();
	 currentelement++)
      if (IsOn (currentelement)) return 0;
    return 1;
  }

public:
  int StartTraversal (void) {
    currentelement = -1; return AdvanceToNextElement ();
  }
  int Advance (void) {return AdvanceToNextElement ();}
  SetElementType *CurrentElement (void) const {
    return SetElementType::NthElement (currentelement);
  }
  void DeleteCurrentElement (void) {TurnOff (currentelement);}
  int DeleteAndAdvance (void) {DeleteCurrentElement (); return Advance ();}
  
  // a hash value based on the logical properties.
  // guaranteed to be equal for two bitmap objects
  // if they are equal according to the Abitmap_t::IsEqualTo 
  int Hash (void) const {
    return Abitmap_t::Hash (SetElementType::TotalNumber ());
  }
};

#ifdef __GNUC__
template <class SetElementType> 
inline Aset_t<SetElementType> 
ASetUnion (const Aset_t<SetElementType> &s1, 
	   const Aset_t<SetElementType> &s2) {
  Aset_t<SetElementType> newset = s1;
  return newset.Union (s2);
}
template <class SetElementType> 
inline Aset_t<SetElementType> 
ASetIntersect (const Aset_t<SetElementType> &s1, 
	       const Aset_t<SetElementType> &s2) {
  Aset_t<SetElementType> newset = s1;
  return newset.Intersect (s2);
}
template <class SetElementType> 
inline Aset_t<SetElementType> 
ASetMinus (const Aset_t<SetElementType> &s1, 
	   const Aset_t<SetElementType> &s2) {
  Aset_t<SetElementType> newset = s1;
  return newset.Minus (s2);
}
template <class SetElementType>
inline Aset_t<SetElementType>
ASetComplement (const Aset_t<SetElementType> &s) {
  Aset_t<SetElementType> newset = s;
  return newset.Complement ();
}

#else
template <class SetElementType> 
Aset_t<SetElementType> 
ASetUnion (const Aset_t<SetElementType> &s1, const Aset_t<SetElementType> &s2);

template <class SetElementType> 
Aset_t<SetElementType> 
ASetIntersect (const Aset_t<SetElementType> &s1, 
	       const Aset_t<SetElementType> &s2);

template <class SetElementType> 
Aset_t<SetElementType> 
ASetMinus (const Aset_t<SetElementType> &s1, 
	   const Aset_t<SetElementType> &s2);

template <class SetElementType>
Aset_t<SetElementType>
ASetComplement (const Aset_t<SetElementType> &s);

// this function should not be here.
// it should be in Bprint.c in the bogus directory.
// but CC does not like that, so...
#include <iostream.h>

template <class SetElementType> 
void Bprintset (ostream &os, Aset_t<SetElementType> set);


#endif /* __GNUC__ */



// OK OK. i know that this is not a very good thing to do.
// and that it is rotten programming. but considering the
// benefits, i think it is worth the risk.
// 
// the way you are supposed to use this is
//     FOR_EACH_ELEMENT_OF_SET (set) { ...set.CurrentElement ()... }
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
inline int &_A_eos_(void) {return Aglob_vars()->end_of_set;}

#define FOR_EACH_ELEMENT_OF_SET(s) for (_A_eos_()=(s).StartTraversal (); \
                                        !_A_eos_(); \
                                        _A_eos_()=(s).Advance ())

#define FOR_EACH_ELEMENT_OF_SET_WITH_DELETE(s) \
                                   for (_A_eos_()=(s).StartTraversal (); \
                                        !_A_eos_(); \
                                        _A_eos_()=(s).DeleteAndAdvance ())


#endif /* ASET_H */
