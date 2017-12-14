#include <assert.h>

#include <Aglob_vars.h>
#include <Aquery.h>

#include "Afunc.h"
#include "Aptree.h"

// make a copy of a whole tree.
Aptree_t Afunc_t::copy_tree (const Aptree_t &ptree) const
{
  Aptree_t newptree;;
  newptree._func = duplicate ();
  
  newptree._args = 0;
  newptree._nargs = ptree.nargs ();
  if (newptree._nargs > 0)
    {
      newptree._args = new Aptree_t[newptree._nargs];
      assert (newptree._args);
      for (int i=0; i<newptree._nargs; i++)
	newptree.new_arg (i, ptree.arg (i)->copy_tree ());
    }
  return newptree;
}

// this set of functions together are used to replace all
// occurrences of Arelattr_t by Aattref_t.

void Afunc_t::replace_relattrs (Aptree_t &ptree)
{
  for (int i=0; i<ptree.nargs(); i++)
    ptree.arg(i)->replace_relattrs ();
}

void Aattref_t::replace_relattrs (Aptree_t &ptree)
{
  Arelref_t *relref = (Arelref_t *)ptree.arg (0)->func ();
  assert (relref->type () == tuplevar || relref->type () == tupleref);

  _attr = Aglob_vars()->query->attr (relref->rel (), attrname ());
  assert (_attr);
  return;
}

// unused
void Arelattr_t::replace_relattrs (Aptree_t &ptree)
{
  Aattr_t *attr = Aglob_vars()->query->attr (_relname, _attrname);
  assert (attr);
  
//  Aattref_t *attref = new Aattref_t (attr);
// irrelevant
  assert (!"this is not supposed to be called");
  
//  ptree._func = attref;
  
  // no change required to ptree._nargs and ptree._args

  delete this;
}


// these two functions together are used to insert into
// "rels" and "attrs" those attributes and relations which
// are referenced in a ptree

void Afunc_t::update_sets (const Aptree_t &ptree,
			   Aset_t<Arel_t> &rels,
			   Aset_t<Aattr_t> &attrs) const
{
  for (int i=0; i<ptree.nargs(); i++)
    ptree.arg(i)->update_sets (rels, attrs);
}

void Aattref_t::update_sets (const Aptree_t &ptree,
			     Aset_t<Arel_t> &rels,
			     Aset_t<Aattr_t> &attrs) const
{
  attrs.Insert (_attr);
  rels.Insert (_attr->rel ());
}


// this set of functions is used in conjunction with Aptree_t::is_equal_to 
// to determine the equality of operations in a ptree.
int Arelref_t::are_data_members_equal (const Afunc_t &f) const
{
  return _rel->is_equal_to (((Arelref_t &)f).rel ());
}

int Aattref_t::are_data_members_equal (const Afunc_t &f) const
{
  return _attr->is_equal_to (((Aattref_t &)f).attr ());
}

int Af_apply_t::are_data_members_equal (const Afunc_t &f) const
{
  return ((Af_apply_t &)f).funcid () == funcid ();
}


int Aoidref_t::are_data_members_equal (const Afunc_t &f) const
{
  return _rel->is_equal_to (((Aoidref_t &)f)._rel) &&
    strcmp (((Aoidref_t &)f).oidstr (), oidstr ()) == 0;
}

int Aliteral_t::are_data_members_equal (const Afunc_t &f) const
{
  return 0;			// as a first cut we are going to
				// assume that no two literals are equal.
}



