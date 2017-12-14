#include <Ajoin.h>
#include <Aquery.h>
#include <Adbrel.h>

#include <Alognode.h>
#include <Aphynode.h>
#include <Asearch.h>
#include <Aoptions.h>

Alist_t<Abinop_t> Ajoin_t::Clones (Alognode_t *leftinput, 
				   Alognode_t *rightinput)
{
  Alist_t<Abinop_t> clones;
  
  Alogprop_t *leftprops = leftinput->GetLogProps ();
  Alogprop_t *rightprops = rightinput->GetLogProps ();

  Aset_t<Arel_t> leftrels = leftprops->rels ();
  Aset_t<Arel_t> rightrels = rightprops->rels ();
  
  if (Aglob_vars()->oopt->left_deep_only &&
      rightrels.Cardinality () > 1)
    return clones;		// we want left-deep only

  if (!ASetIntersect (leftrels, rightrels).IsEmpty ())
    return clones;		// cannot join these together.
				// they have common relations.

  find_applicable_preds (leftprops, rightprops);
				// this function will set the
				// values of _equijoin_attrs and _preds.

  if (_preds.IsEmpty () &&
      Aglob_vars()->oopt->delay_cartesian_products &&
      !leftprops->conn_rels ().IsEqualTo (leftrels))
    return clones;		// if there are no join-predicates,
				// and you are delaying cartesian products,
				// then do this join only if there is no
				// alternative

  Ajoin_t *newjoin = (Ajoin_t *)Duplicate ();
  assert (newjoin);
  clones.Insert (newjoin);
  return clones;
}


void Ajoin_t::find_applicable_preds (Alogprop_t *leftprops, 
				       Alogprop_t *rightprops)
{
  _equijoin_attrs.Clear ();
  _preds.Clear ();		// initializing

  Aset_t<Aattr_t> left_eqattrs = leftprops->eqattrs ();
  Aset_t<Aattr_t> right_eqattrs = rightprops->eqattrs ();

  _equijoin_attrs = ASetIntersect (left_eqattrs, right_eqattrs);

  Aset_t<Aattr_t> tmp_equijoin_attrs = _equijoin_attrs;
				// copy for temporary use in this function.

  Aset_t<Aattr_t> output_eqattrs;
  if (Aglob_vars()->oopt->no_equivalences)
    output_eqattrs = ASetUnion (leftprops->attrs (), rightprops->attrs ());
  else
    output_eqattrs = ASetUnion (left_eqattrs, right_eqattrs);
				// eq-closure of attributes in the output
				// of this join

  Aset_t<Apred_t> leftpreds = leftprops->preds ();
  Aset_t<Apred_t> rightpreds = rightprops->preds ();

  Aset_t<Apred_t> leftover_preds = 
    ASetComplement (ASetUnion (leftpreds, rightpreds));
				// these are the preds which have yet
				// to be applied.

  FOR_EACH_ELEMENT_OF_SET (leftover_preds)
    {
      Apred_t *pred = leftover_preds.CurrentElement ();
      Aset_t<Aattr_t> pred_eqattrs;
      if (Aglob_vars()->oopt->no_equivalences)
	pred_eqattrs = pred->attrs ();
      else
	pred_eqattrs = pred->eqattrs ();
      
      if (pred->type () == Apred_t::equijoin &&
	  !Aglob_vars()->oopt->no_equivalences)
	{
	  if (pred_eqattrs.IsSubsetOf (tmp_equijoin_attrs))
	    {	
	      _preds.Insert (pred);
	      tmp_equijoin_attrs.Minus (pred_eqattrs);
				// if we have a query like
				//    select * from a,b,c where a.x = b.x
				//                        and b.x = c.x
				// we have to make sure that
				//   1. if a and c want to join, we can use
				//      one of the join predicates to join
				//      them (with suitable rewriting which
				//      will be done by ii_execution.c)
				//   2. we should not use up both the join
				//      predicates for joining them. one
				//      should be left so that b can be
				//      joined in later.
	    }
	}
      else if (pred_eqattrs.IsSubsetOf (output_eqattrs))
	_preds.Insert (pred);	// this pred can be applied at this point
    }
  if (!Aglob_vars()->oopt->no_equivalences)
    assert (tmp_equijoin_attrs.IsEmpty ());
				// make sure that there is a pred joining
				// each equijoin_attr
}
  

// this functions is used by the Ajoin_t::Apply function
//
// if we are doing left deep only, then explore the other
// node only if its arity is 1. i.e. inner cannot be a
// composite relation.
// on the other hand, if we are doing bushy trees, explore 
// everything in which the sets of relations dont intersect.
static int explore_node (Alognode_t*input, Alognode_t *othernode)
{
  if (Aglob_vars()->oopt->left_deep_only &&
      othernode->GetOp ()->Arity () > 1)
    return 0;			// dont explore this node;
				// it will give rise to a bushy tree
  
  Aset_t<Arel_t> intersect = 
    ASetIntersect (othernode->GetLogProps ()->rels (),
		   input->GetLogProps ()->rels ());
  if (!intersect.IsEmpty ())
    return 0;			// dont explore this node;
				// they have a relation in common
  return 1;
}



// we use this instead of the default Abinop_t::Apply
// because we want only left deep join trees
//
// Note: this is only an efficiency thing. if we 
// remove this function completely, the optimizer will still
// find only left deep trees because of the Clones function
// above which disallows all bushy trees. 
// What we are trying to do here is to make the Apply
// function computationally less expensive by considering only the
// single relation nodes in the searchtree.
void Ajoin_t::Apply (Alognode_t *input)
{
  if (input->GetLogProps ()->numrels () == 1)
    {
      Abinop_t::Apply (input);	// if this node is itself a single relation
				// node, then it can be joined with any
				// other node (as the right input). hence
				// for this special case we revert to
				// Abinop_t::Apply
      return;
    }

  // when this node is not a single relation node, then we save some
  // time by looking at only single relation nodes in the search tree
  // (which will be used as right inputs).
  Alist_t<Alognode_t> list = Aglob_vars ()->search->GetListOfRootNodes ();
  FOR_EACH_ELEMENT_OF_LIST (list)
    {
      Alognode_t *othernode = list.Element ();
      if (explore_node (input, othernode))
	{
	  Alist_t<Alognode_t> queue;
	  queue.Insert (othernode);
				// here we are doing a little breadth first
				// search of all the children of all the
				// dbrel-nodes whose relations are not 
				// already contained in the "input" node.

	  while (othernode = queue.PopFirstElement ())
	    {
	      Alist_t<Abinop_t> clones = Clones (input, othernode);
	      FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (clones)
		{
		  Alognode_t *l = new Alognode_t (clones.Element (), 
						  input, othernode);
		  assert (l);
		  Aglob_vars()->search->NewNode (l);
		}

	      FOR_EACH_ELEMENT_OF_LIST (othernode->Children ())
		{
		  Alognode_t *childnode = othernode->Children ().Element ();
		  if (explore_node (input, childnode))
		    queue.InsertAtEnd (childnode);
		}
	    }
	}
    }
}

Alist_t<Abinalgo_t> Aloopsjoin_t::Clones (Alognode_t *lognode,
					  Aphynode_t *leftinput,
					  Aphynode_t *rightinput)
{
  Alist_t<Abinalgo_t> clones;

  if (rightinput->GetAlgo ()->GetNumber () != Aheapfile)
    return clones;		// we dont want to apply nested loops if
				// the right input is an index. that is
				// the domain of the nlidx join...

  clones.Insert ((Abinalgo_t *)Duplicate ());
  return clones;
}

Alist_t<Abinalgo_t> Anlidxjoin_t::Clones (Alognode_t *lognode,
					  Aphynode_t *leftinput,
					  Aphynode_t *rightinput)
{
  Alist_t<Abinalgo_t> clones;
  Ajoin_t *join = (Ajoin_t *)lognode->GetOp ();

  if (rightinput->GetAlgo ()->GetNumber () != Aindex)
    return clones;		// right input must be an index...

  // we are trying to decide which of the preds to use as the index
  // predicate for the index-nested-loops join.
  // in case there are more than one, we will make one clone
  // of this algo for each.

  Aattr_t *indexattr = ((Aindex_t *)rightinput->GetAlgo ())->indexattr ();
  Aset_t<Apred_t> preds = join->preds ();
  Aset_t<Aattr_t> outer_eqattrs;
  if (Aglob_vars()->oopt->no_equivalences)
    outer_eqattrs = leftinput->GetParent ()->GetLogProps ()->attrs ();
  else
    outer_eqattrs = leftinput->GetParent ()->GetLogProps ()->eqattrs ();

  FOR_EACH_ELEMENT_OF_SET (preds)
    {
      Apred_t *p = preds.CurrentElement ();
      if (p->index_joinable (indexattr, outer_eqattrs))
	{
	  Anlidxjoin_t *newjoin = (Anlidxjoin_t *)Duplicate ();
	  newjoin->_pred = p;
	  clones.Insert (newjoin);
	}
    }
  return clones;
}
  

// the merge join can be applied only if this is an equijoin
// on a single attribute.
Alist_t<Abinalgo_t> Amergejoin_t::Clones (Alognode_t *lognode,
					  Aphynode_t *leftinput,
					  Aphynode_t *rightinput)
{
  Alist_t<Abinalgo_t> clones;

  Aset_t<Apred_t> joinpreds = ((Ajoin_t *)lognode->GetOp ())->preds ();
  
  FOR_EACH_ELEMENT_OF_SET (joinpreds)
    {
      Apred_t *equijoin_pred = joinpreds.CurrentElement ();
      if (equijoin_pred->type () == Apred_t::equijoin)
	{			// can used only for equijoins
	  Amergejoin_t *m = (Amergejoin_t *)Duplicate ();
	  m->_pred = equijoin_pred;
	  clones.Insert (m);
	}
    }

  return clones;
}

Aphyprop_t *Amergejoin_t::Constraint (Alognode_t *lognode, 
				      Aphynode_t *input,
				      int inputnum)
{
  Aset_t<Aattr_t> pred_eqattrs = _pred->eqattrs ();
  Aattr_t *reqd_sortorder = pred_eqattrs.FirstElement ();
  Aattr_t *actual_sortorder = input->GetPhyProps ()->sortorder ();

  if (actual_sortorder)		// if the input is already sorted
    if (Aglob_vars()->query->are_attrs_equivalent (actual_sortorder,
						   reqd_sortorder))
      return 0;			// the constraint is already satisfied

  Aphyprop_t *reqd_props = new Aphyprop_t (input->GetPhyProps ());
  assert (reqd_props);
  reqd_props->set_sortorder (reqd_sortorder);
  
  return reqd_props;		// this will be delete'd by Aalgo_t...
}

int Amergejoin_t::CanBeApplied (Alognode_t *lognode,
				Aphynode_t *leftinput,
				Aphynode_t *rightinput)
{
  // this should always return true because the input constraints of
  // a merge join can always be satisfied using a sort.
  Aset_t<Aattr_t> pred_eqattrs = _pred->eqattrs ();
  Aattr_t *first_attr = pred_eqattrs.FirstElement ();
  assert (Aglob_vars()->query->are_attrs_equivalent 
	  (first_attr, leftinput->GetPhyProps ()->sortorder ()) &&
	  Aglob_vars()->query->are_attrs_equivalent 
	  (first_attr, rightinput->GetPhyProps ()->sortorder ()));

  return 1;
}


Alist_t<Abinalgo_t> Ahashjoin_t::Clones (Alognode_t *lognode,
					 Aphynode_t *leftinput,
					 Aphynode_t *rightinput)
{
  Alist_t<Abinalgo_t> clones;
  Aset_t<Apred_t> joinpreds = ((Ajoin_t *)lognode->GetOp ())->preds ();

  FOR_EACH_ELEMENT_OF_SET (joinpreds)
    {
      Apred_t *joinpred = joinpreds.CurrentElement ();
      if (joinpred->type () == Apred_t::equijoin)
	{
	  Ahashjoin_t *h = (Ahashjoin_t *)Duplicate ();
	  h->_pred = joinpred;
	  clones.Insert (h);	// the hash join can be applied only if 
				// this is an equijoin on a single attribute
	}
    }
  return clones;
}
