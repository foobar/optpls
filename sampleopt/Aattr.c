#include "Aattr.h"
#include "Aquery.h"

int Aattr_t::TotalNumber (void)
{
  return Aglob_vars()->query->numattrs ();
}

Aattr_t *Aattr_t::NthElement (int N)
{
  return Aglob_vars()->query->attr (N);
}

Aset_t<Aattr_t> Aget_eq_closure (Aset_t<Aattr_t> &attrset)
{
  Aset_t<Aattr_t> eq_closure;

  FOR_EACH_ELEMENT_OF_SET (attrset)
    {
      Aattr_t *a = attrset.CurrentElement ();
      eq_closure.Union (Aglob_vars()->query->get_eqattrs (a));
    }
  return eq_closure;
}
