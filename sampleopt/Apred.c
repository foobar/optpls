#include "Apred.h"
#include "Aquery.h"
#include "Aoptions.h"
#include <assert.h>

void Apred_t::init (int num, Aptree_t &p)
{
  _number = num;
  _type = bogus;
  _ptree = p;
  
}

int Apred_t::TotalNumber (void)
{
  return Aglob_vars()->query->numpreds ();
}

Apred_t *Apred_t::NthElement (int N)
{
  return Aglob_vars()->query->pred (N);
}


void Apred_t::update_sets (void)
{
  _rels = new Aset_t<Arel_t>; assert (_rels);
  _attrs = new Aset_t<Aattr_t>; assert (_attrs);
  _eqattrs = new Aset_t<Aattr_t>; assert (_eqattrs);

  _ptree.update_sets (*_rels, *_attrs);
  *_eqattrs = Aget_eq_closure (*_attrs);

  // now let us sneak in computations about the pred type also.
  Aattr_t *a1, *a2;		// bogus.
  if (_ptree.func ()->type () == Afunc_t::oidref)
    _type = oidref;
  else if (_attrs->Cardinality () == 1)
    _type = singleattr;
  else if (_rels->Cardinality () == 1)
    _type = singlerel;
  else if (is_equijoin (a1, a2))
    _type = equijoin;
  else 
    _type = general;

  // let us also compute the the leftattrs and rightattrs.
  // these are required for computing whether this pred can
  // be used in a index-nested-loops-join.
  if (_ptree.func ()->is_relop () &&
      (_type == equijoin || _type == general)) // these are the only cases
					       // in which it is useful
    {
      Aset_t<Arel_t> bogus;
      assert (leftattrs = new Aset_t<Aattr_t>);
				// all attributes referenced in the
				// left operand of the relop
      assert (rightattrs = new Aset_t<Aattr_t>);
				// all attribute referenced in the
				// right operand of the relop
      _ptree.arg(0)->update_sets (bogus, *leftattrs);
      _ptree.arg(1)->update_sets (bogus, *rightattrs);
    }
}

int Apred_t::index_scannable (const Aattr_t *a) const
{
  if (!a) return 0;
  if (_type != singleattr) return 0;
  if (Aglob_vars()->oopt->no_equivalences)
    {if (!_attrs->HasElement (a)) return 0;}
  else
    {if (!_eqattrs->HasElement (a)) return 0;}

  if (_ptree.func ()->is_relop () &&
      _ptree.arg(0)->func ()->is_attref () &&
      _ptree.arg(1)->func ()->is_literal ())
    return 1;
  if (_ptree.func ()->type () == Afunc_t::between &&
      _ptree.arg(0)->func ()->is_attref () &&
      _ptree.arg(1)->func ()->is_literal () &&
      _ptree.arg(2)->func ()->is_literal ())
    return 1;

  return 0;
}


// if the optimizer option "no_equivalences" is set, then
// this function checks whether these attributes are
// really equal to each other. if the option is not set
// the it checks whether they are equivalent to each other
// according to the equivalence classes.
inline int Aare_attrs_pseudo_equal (const Aattr_t *a1, const Aattr_t *a2)
{
  if (Aglob_vars()->oopt->no_equivalences)
    return a1->is_equal_to (a2);
  else
    return Aglob_vars()->query->are_attrs_equivalent (a1, a2);
}

// a predicate can be used in an index-nested-loops join if
// the following conditions hold:
//      a) it is a "relop" predicate
//      b) one operand of the relop is equivalent to the attribute
//         on which the index is
//      c) the other operand of the relop references only attributes
//         from the "outer" relation (in the join).

int Apred_t::index_joinable (const Aattr_t *indexattr, 
			     const Aset_t<Aattr_t> &outer_eqattrs) const
{
  if (!indexattr) return 0;

  if (leftattrs == 0) return 0;	// this is not the right kind of predicate.
				// condition (a)

  Aattr_t *left = 0, *right = 0;


  if (_ptree.arg (0)->func ()->is_attref ())
    {
      Aattr_t *left = leftattrs->FirstElement ();
      if (Aare_attrs_pseudo_equal (left, indexattr))
				// condition (b)
	if (rightattrs->IsSubsetOf (outer_eqattrs))
				// condition (c)
	  return 1;
    }
  
  if (_ptree.arg (1)->func ()->is_attref ())
    {
      Aattr_t *right = rightattrs->FirstElement ();
      if (Aare_attrs_pseudo_equal (right, indexattr))
				// condition (b)
	  if (leftattrs->IsSubsetOf (outer_eqattrs))
	    return 1;		// condition (c)
    }
  return 0;			// no this predicate cannot be used
				// in an index-nested-loops-join
}


Aset_t<Aattr_t> Aget_eq_closure (Aset_t<Apred_t> &predset)
{
  Aset_t<Aattr_t> eq_closure;

  FOR_EACH_ELEMENT_OF_SET (predset)
    eq_closure.Union (predset.CurrentElement ()->eqattrs ());
  
  return eq_closure;
}
