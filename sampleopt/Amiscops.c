#include <Amiscops.h>
#include <Aquery.h>

#include <Alognode.h>
#include <Aphynode.h>
#include <Asearch.h>


#include <iostream.h>
#include <stdlib.h>

Aphyprop_t *Aaggralgo_t::Constraint (Alognode_t *lognode, 
				     Aphynode_t *input,
				     int inputnum)
{
  Aattr_t *groupby = Aglob_vars()->query->groupby ();

  if (groupby)			// if there is a group by clause
    {
      Aphyprop_t *reqd_props = new Aphyprop_t (input->GetPhyProps ());
      reqd_props->set_sortorder (groupby);
      return reqd_props;
    }
  else
    return 0;			// no constraint...
}

Alist_t<Aunaryalgo_t> Aaggralgo_t::Clones (Alognode_t *lognode,
					   Aphynode_t *input)
{
  Alist_t<Aunaryalgo_t> clones;
  
  if (!Aglob_vars()->query->groupby () ||
      input->GetPhyProps ()->is_grouped ())
    clones.Insert ((Aunaryalgo_t *)Duplicate ());
				// if there is a group by clause then
				// insist that the input be grouped...

  return clones;
}

int Aaggralgo_t::CanBeApplied (Alognode_t *lognode, Aphynode_t *input)
{
  // this function should always return 1 because the input
  // constraint can always be satisfied by using the sort enforcer
  assert (!Aglob_vars()->query->groupby () ||
	  input->GetPhyProps ()->is_grouped ());
  return 1;
}

Aphyprop_t *Aoutputalgo_t::Constraint (Alognode_t *lognode, 
				       Aphynode_t *input,
				       int inputnum)
{
  // just check that there is no problem with group by clauses
  Aattr_t *groupby = Aglob_vars()->query->groupby ();
  if (groupby &&
      !Aglob_vars()->query->has_aggr_in_target_list ())
				// if there is a group by clause and there
				// are aggregates in target list
    {
      cerr << "Query has group by but no aggregates. I cant handle this" 
	<< endl;
      exit (1);
    }

  // i am assuming that group by and order by dont appear in the
  // same query (the typechecker disallows such a query...)
  // so, now forget about the group by clause

  Aattr_t *orderby = Aglob_vars()->query->orderby ();
  if (orderby)
    {
      Aphyprop_t *reqd_props = new Aphyprop_t (input->GetPhyProps ());
      assert (reqd_props);
      reqd_props->set_sortorder (orderby);
      return reqd_props;
    }

  
  return 0;			// no constraint...
}

int Aoutputalgo_t::CanBeApplied (Alognode_t *lognode, Aphynode_t *input)
{
  // this function should always return 1 because the input
  // constraint can always be satisfied by using the sort enforcer
  assert (!Aglob_vars()->query->orderby () ||
	  Aglob_vars()->query->are_attrs_equivalent 
	  (input->GetPhyProps ()->sortorder (),
	   Aglob_vars()->query->orderby ()));
  return 1;
}


Alist_t<Aphynode_t> Asort_t::Enforce (Aphynode_t *node,
				      Aphyprop_t *reqd_props)
{
  Alist_t<Aphynode_t> enforced_outputs;

  _sortorder = reqd_props->sortorder ();
  Aphynode_t *newnode = new Aphynode_t (node->GetParent(), this, node);
  assert (newnode);
  enforced_outputs.Insert (newnode);
  return enforced_outputs;
}

