#include "Bprojlist.h"
#include "Aptree.h"
#include <string.h>

void ProjList::initialize (int n)
{
  curr_elem = 0;
  assert ((num_elems = n) >= 0);
  if (num_elems == 0)
    elems = 0;			// this is a NULL ProjList
  else
    assert (elems = new projelem_t[num_elems]);
}

void ProjList::put (Aptree_t *ptree, char *out_attrname)
{
  assert (curr_elem < num_elems);
  elems[curr_elem].ptree = ptree;
  strncpy (elems[curr_elem].out_attrname, out_attrname, 
	   sizeof (elems[curr_elem].out_attrname));
  curr_elem++;
}

const ProjList::projelem_t &ProjList::get (int elem_num)
{
  assert (elem_num < num_elems);
  return elems[elem_num];
}
