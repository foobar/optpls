#include "Arel.h"
#include "Aquery.h"

int Arel_t::TotalNumber (void)
{
  return Aglob_vars()->query->numrels ();
}

Arel_t *Arel_t::NthElement (int N)
{
  return Aglob_vars()->query->rel (N);
}

