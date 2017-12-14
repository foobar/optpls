#include <Asubop.h>
#include <Aquery.h>

int Asubop_t::TotalNumber (void)
{
  return Aglob_vars()->query->numsubops ();
}

Asubop_t *Asubop_t::NthElement (int N)
{
  return Aglob_vars()->query->subop (N);
}

