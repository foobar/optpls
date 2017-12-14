#ifndef __GNUC__
#include "Aset.h"

template <class SetElementType> 
Aset_t<SetElementType> 
ASetUnion (const Aset_t<SetElementType> &s1, 
	     const Aset_t<SetElementType> &s2) {
  Aset_t<SetElementType> newset = s1;
  return newset.Union (s2);
}
template <class SetElementType> 
Aset_t<SetElementType> 
ASetIntersect (const Aset_t<SetElementType> &s1, 
		 const Aset_t<SetElementType> &s2) {
  Aset_t<SetElementType> newset = s1;
  return newset.Intersect (s2);
}
template <class SetElementType> 
Aset_t<SetElementType> 
ASetMinus (const Aset_t<SetElementType> &s1, 
	     const Aset_t<SetElementType> &s2) {
  Aset_t<SetElementType> newset = s1;
  return newset.Minus (s2);
}
template <class SetElementType>
Aset_t<SetElementType>
ASetComplement (const Aset_t<SetElementType> &s) {
  Aset_t<SetElementType> newset = s;
  return newset.Complement ();
}
template <class SetElementType>
void Bprintset (ostream &os, Aset_t<SetElementType> set)
{
  FOR_EACH_ELEMENT_OF_SET (set)
    os << ' ' << set.CurrentElement ()->Number ();
}

#endif

