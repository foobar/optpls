#include <Aopalgos.h>
#include <Alognode.h>
#include <Aphynode.h>
#include <Asearch.h>
#include <Aquery.h>
#include <Aptree.h>

static void Acompute_need_inmem (const Aptree_t &ptree,
				 Aset_t<Aptree_t> &need_inmem,
				 Aset_t<Aptree_t> &computed,
				 Aset_t<Aptree_t> &not_inmem,
				 int topnode = 1) 
{
  if (!topnode ||
      (computed.HasElement (&ptree) &&
       not_inmem.HasElement (&ptree) &&
       !ptree.is_literal ()))
    need_inmem.Insert (&ptree); // if it is not topnode, always insert.

  for (int i=0; i<ptree.nargs (); i++)
    {
      if (computed.HasElement (ptree.arg (i)) &&
	  not_inmem.HasElement (ptree.arg (i)) &&
	  !ptree.arg (i)->is_literal ())
	  Acompute_need_inmem (*ptree.arg (i),
			       need_inmem,
			       computed,
			       not_inmem, 0);
    }
}


void Aop_operation_t::compute_need_inmem (Aset_t<Aptree_t> &need_inmem, 
					  Aset_t<Aptree_t> &computed, 
					  Aset_t<Aptree_t> &not_inmem)
{
  if (not_inmem.IsEmpty ()) return; // everything is in memory. no problems.
  FOR_EACH_ELEMENT_OF_SET (_operations)
    {
      Aptree_t *oprn = _operations.CurrentElement ();
      Acompute_need_inmem (*oprn, need_inmem,
			   computed,
			   not_inmem);
    }
}

Aphyprop_t *
Aop_operation_t::inmem_input_constraint (Alognode_t *node,
					 Aphynode_t *input,
					 int inputnum)
{
  Aset_t<Aptree_t> need_inmem;
  Aset_t<Aptree_t> computed = 
    node->Input (inputnum)->GetLogProps ()->operations ();
  compute_need_inmem (need_inmem, 
		      computed,
		      input->GetPhyProps ()->ops_not_in_memory ());
  
  if (!need_inmem.IsEmpty ())
    {
      Aphyprop_t *reqd_props = new Aphyprop_t (input->GetPhyProps ());
      reqd_props->need_inmem (need_inmem);
      return reqd_props;
    }
  else
    return 0;
}


Aset_t<Aptree_t> Aget_select_oprns (Alognode_t *input)
{
  Aset_t<Aptree_t> select_operations;

  Aset_t<Aptree_t> inoperations = input->GetLogProps ()->operations ();
  inoperations.Minus (input->GetLogProps ()->need_unnesting ());
				// cannot apply selections on attributes
				// that need unnesting.
  Aset_t<Aptree_t> relop_ops = Aglob_vars()->query->relop_ops ();
  relop_ops.Minus (inoperations);
				// check only those which havent been
				// done already...
  Aset_t<Aptree_t> undone_tuplerefs = 
    input->GetLogProps ()->undone_tuplerefs ();
  
  Aset_t<Aptree_t> all_select_oprns = relop_ops;
  all_select_oprns.Union (undone_tuplerefs);

  int count = 0;		// count the number of clones
  FOR_EACH_ELEMENT_OF_SET (all_select_oprns)
    {
      Aptree_t *oprn = all_select_oprns.CurrentElement ();

      if (oprn->is_tupleref () ||
				// if its a tupleref ... or ...
	  (inoperations.HasElement (oprn->arg (0)) &&
	   inoperations.HasElement (oprn->arg (1))))
	select_operations.Insert (oprn);
    }

  return select_operations;
}

/***************************************************************************

  Aget

***************************************************************************/

void Aget_t::Apply (Alognode_t *lognode)
{
  Alist_t<Aop_t> clones = Clones ();
  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (clones)
    {
      Alognode_t *newnode = new Alognode_t (clones.Element ());
      assert (newnode);
      Aglob_vars()->search->NewNode (newnode);
    }
}

Alist_t<Aop_t> Aget_t::Clones (Alognode_t **inputs)
{
  Alist_t<Aop_t> clones;

  Aset_t<Aptree_t> relref_ops = Aglob_vars()->query->relref_ops ();
  FOR_EACH_ELEMENT_OF_SET (relref_ops)
    {
      Aptree_t *oprn = relref_ops.CurrentElement ();
      assert (oprn->is_relref ());
      Aget_t *newget = (Aget_t *)Duplicate ();
      newget->_relref = oprn;
      newget->compute_operations ();
      clones.Insert (newget);
    }

  return clones;
}

// find out which operations are going to be applied by this operator
void Aget_t::compute_operations (void)
{
  assert (_relref);
  _operations.Insert (_relref);
  _operations.Union (Aglob_vars()->query->op_mats (_relref));
}

/***************************************************************************

  Amat

  the materialize operator.

***************************************************************************/

Alist_t<Aunaryop_t> Amat_t::Clones (Alognode_t *input)
{
  Alist_t<Aunaryop_t> clones;

  if (input->GetPhyNodes ().IsEmpty ())
    return clones;		// no point in trying to expand
				// a sub-optimal node

  if (!Aglob_vars()->oopt->do_exhaustive_selects)
    {
      Aset_t<Aptree_t> select_oprns = Aget_select_oprns (input);
      if (!select_oprns.IsEmpty ())
	return clones;		// if there are any select predicates
				// applicable and if we are not doing
				// exhaustive select positioning
				// (i.e. we are doing pushdown)
				// then dont apply the materialize
    }

  Aset_t<Aptree_t> inoperations = input->GetLogProps ()->operations ();
  inoperations.Minus (input->GetLogProps ()->need_unnesting ());
				// cannot materialize something that
				// hasnt been unnested yet.
  Aset_t<Aptree_t> tupleref_ops = Aglob_vars()->query->tupleref_ops ();
  tupleref_ops.Intersect (inoperations);
				// we can assemble only those
				// which are present in the input

  FOR_EACH_ELEMENT_OF_SET (tupleref_ops)
    {
      Aptree_t *oprn = tupleref_ops.CurrentElement ();
      Amat_t *newmat = (Amat_t *)Duplicate ();
      newmat->_tuplerefs.Insert (oprn);
      newmat->compute_operations ();
      if (newmat->operations ().IsSubsetOf (inoperations))
	delete newmat;		// no point in applying this materialize
      else
	clones.Insert (newmat);
    }

  return clones;
}

void Amat_t::compute_operations (void)
{
  Aset_t<Aptree_t> mat_ops = Aglob_vars()->query->mat_ops ();
  _operations = Aglob_vars()->query->op_mats (tupleref ());
}

/***************************************************************************

  Amat_collapse

***************************************************************************/

void Amat_collapse_t::Apply (Alognode_t *input)
{
  assert (Aglob_vars()->oopt->do_mat_collapse);

  if (input->GetOp ()->GetNumber () == Amat &&
      input->Input ()->GetOp ()->GetNumber () == Amat)
    {
      Aset_t<Aptree_t> cumulative_ops = 
	((Amat_t *)input->GetOp ())->operations ();
      Aset_t<Aptree_t> cumulative_tuplerefs;
      cumulative_tuplerefs.Insert (((Amat_t *)input->GetOp ())->tupleref ());
      
      for (Alognode_t *mat_node = input->Input ();
	   mat_node->GetOp ()->GetNumber () == Amat;
	   mat_node = mat_node->Input ())
	{
	  Amat_t *matop = (Amat_t *)mat_node->GetOp ();
	  Aset_t<Aptree_t> mat_oprns = matop->operations ();
	  cumulative_ops.Union (mat_oprns);
	  cumulative_tuplerefs.Insert (matop->tupleref ());
	  
	  Amat_collapse_t *newmat = (Amat_collapse_t *)Duplicate ();
	  assert (newmat);
	  newmat->_matnode = input;
	  newmat->_tuplerefs = cumulative_tuplerefs;
	  newmat->_operations = cumulative_ops;

	  // now create the input
	  Alognode_t *newnode = new Alognode_t (newmat, mat_node->Input ());
	  assert (newnode);
	  Aglob_vars ()->search->NewNode (newnode);
	}
    }
}


/***************************************************************************

  Aselect

***************************************************************************/

Alist_t<Aunaryop_t> Aselect_t::Clones (Alognode_t *input)
{
  Alist_t<Aunaryop_t> clones;

  Aset_t<Aptree_t> select_oprns = Aget_select_oprns (input);

  int count = 0;		// count the number of clones
  FOR_EACH_ELEMENT_OF_SET (select_oprns)
    {
      Aptree_t *oprn = select_oprns.CurrentElement ();
      Aselect_t *newselect = (Aselect_t *)Duplicate ();
      newselect->_operations.Insert (oprn);
      newselect->_multi_select = count;
				// we make multi_select TRUE for everything
				// except the first clone.
      clones.Insert (newselect);
      count++;

      if (!Aglob_vars()->oopt->do_exhaustive_selects)
	break;			// if we are not doing exhaustive positioning
				// of selects, then we dont have to try
				// out all different possibilities here.
				// just apply the first one, and get out.
    }

  return clones;
}

/***************************************************************************

  Aselect_collapse

***************************************************************************/

// WARNING: assumes that Aselect_t can apply only a single
// oprn at any given time. Aselect_collapse_t can of course
// apply as many as it wants.

void Aselect_collapse_t::Apply (Alognode_t *input)
{
  assert (Aglob_vars()->oopt->do_select_collapse);

  if (input->GetOp ()->GetNumber () == Aselect &&
      input->Input ()->GetOp ()->GetNumber () == Aselect)
    {
      Aptree_t *tmp;
      Aptree_t *prev_select_oprn =
	((Aselect_t *)input->GetOp ())->operations ().FirstElement ();
				// this variable will always point to the
				// last operation inserted into the
				// cumulative_ops

      Aset_t<Aptree_t> cumulative_ops;
      cumulative_ops.Insert (prev_select_oprn);

      
      for (Alognode_t *select_node = input->Input ();
	   select_node->GetOp ()->GetNumber () == Aselect;
	   select_node = select_node->Input ())
	{
	  Aselect_t *selectop = (Aselect_t *)select_node->GetOp ();
	  Aptree_t *select_oprn = selectop->operations ().FirstElement ();

	  if (prev_select_oprn->Number () < select_oprn->Number ())
	    {
	      prev_select_oprn = select_oprn;
	      cumulative_ops.Insert (prev_select_oprn);
	  
	      Aselect_collapse_t *newselect = 
		(Aselect_collapse_t *)Duplicate ();
	      assert (newselect);
	      newselect->_select_node = input;
	      newselect->_operations = cumulative_ops;

	      // now create the input
	      Alognode_t *newnode = new Alognode_t (newselect, 
						    select_node->Input ());
	      assert (newnode);
	      Aglob_vars ()->search->NewNode (newnode);
	    }
	  else
	    break; // break out of the for loop.
		   // this thing cannot be collapsed further.
	}
    }
}

/***************************************************************************

  Aidx_collapse

***************************************************************************/

void Aidx_collapse_t::Apply (Alognode_t *lognode)
{
  const char *indexpath = lognode->GetLogProps ()->index_path ();
  if (lognode->GetOp ()->GetNumber () == Aselect && indexpath)
    {
      Aidx_collapse_t *newidx = (Aidx_collapse_t *)Duplicate ();
      
      // compute the operations...
      newidx->_operations = lognode->GetLogProps ()->operations ();
				// it is assumed that the indexscan applied
				// all the operations of the original tree
				// some of them will not be in_memory as
				// evidenced by the Aphyprop_t for this node...

      newidx->_select_node = lognode;
      newidx->_idxid = Aglob_vars()->cat->idxid (indexpath);
      assert (!Aglob_vars()->cat->idxid_is_invalid (newidx->_idxid));

      // now create the lognode.
      Alognode_t *newnode = new Alognode_t (newidx);
      assert (newnode);
      Aglob_vars ()->search->NewNode (newnode);
    }
}

/***************************************************************************

  Ajoin

***************************************************************************/

static int Adont_explore (Alognode_t *leftinput, Alognode_t *rightinput)
{
  if ((Aglob_vars()->oopt->left_deep_only ||
       Aglob_vars()->oopt->right_deep_only) &&
      rightinput->GetOp ()->GetNumber () != Aget &&
      leftinput->GetOp ()->GetNumber () != Aget)
    return 1;			// left or right deep only.


  Aset_t<Aptree_t> leftoprns = leftinput->GetLogProps ()->operations ();
  leftoprns.Minus (leftinput->GetLogProps ()->need_unnesting ());
				// cannot apply joins on attributes
				// that need unnesting.
  Aset_t<Aptree_t> rightoprns = rightinput->GetLogProps ()->operations ();
  rightoprns.Minus (rightinput->GetLogProps ()->need_unnesting ());
				// cannot apply joins on attributes
				// that need unnesting.

  Aset_t<Aptree_t> oprns_intersect = leftoprns;
  oprns_intersect.Intersect (rightoprns);
  oprns_intersect.Delete (&Aptree_t::null_pred_tree ());

  FOR_EACH_ELEMENT_OF_SET (oprns_intersect)
    {
      Aptree_t *oprn = oprns_intersect.CurrentElement ();
      if (oprn->func ()->type () != Afunc_t::tupleref)
	return 1;		// dont explore this
				// because the two nodes already
				// have an attribute in common
      
      Aptree_t *relref_oprn = oprn->arg (0);
				// relref_oprn is the relation inside
				// which this current tupleref is situated
				// we require that relref_oprn be present
				// in exactly one input.

      int present_in_left = 
	leftoprns.HasElement (relref_oprn) ? 1 : 0;
      int present_in_right = 
	rightoprns.HasElement (relref_oprn) ? 1 : 0;
      
      if (present_in_left + present_in_right != 1)
	return 1;		// dont explore the join
    }
  return 0;			// explore the join.
}

Alist_t<Abinop_t> Ajoin_t::Clones (Alognode_t *leftinput, 
				   Alognode_t *rightinput)
{
  Alist_t<Abinop_t> clones;

  if (Aglob_vars()->oopt->left_deep_only &&
      rightinput->GetOp ()->GetNumber () != Aget)
    return clones;		// left deep only.

  if (Aglob_vars()->oopt->right_deep_only &&
      leftinput->GetOp ()->GetNumber () != Aget)
    return clones;		// right deep only.


  Aset_t<Aptree_t> leftoprns = leftinput->GetLogProps ()->operations ();
  leftoprns.Minus (leftinput->GetLogProps ()->need_unnesting ());

  Aset_t<Aptree_t> rightoprns = rightinput->GetLogProps ()->operations ();
  rightoprns.Minus (rightinput->GetLogProps ()->need_unnesting ());

  Aset_t<Aptree_t> tupleref_ops = Aglob_vars()->query->tupleref_ops ();
  Aset_t<Aptree_t> &relref_ops = 
    (Aset_t<Aptree_t> &)Aglob_vars()->query->relref_ops ();
				// converting a const to a non-const.
				// will fix this later.

//  Aset_t<Aptree_t> oprns_intersect = leftoprns;
//  oprns_intersect.Intersect (rightoprns);
//  oprns_intersect.Delete (&Aptree_t::null_pred_tree ());
  
//  if (Adont_explore (oprns_intersect))
//    return clones;		// not joinable.
//
// this need not be checked here because the
// Ajoin_t::Apply function does not send us any that dont
// satisfy this...

  tupleref_ops.Intersect (relref_ops);
				// we are interested only in those
				// tuple ref operations which represent
				// real extents.
  tupleref_ops.Intersect (leftoprns);
  tupleref_ops.Intersect (rightoprns);
				// now tupleref_ops constains those
				// tupleref operations which are present
				// in both inputs and which represent
				// real extents.

  // now we have a set of tupleref operation each of which signifies
  // a legal pointer-based join of the two inputs.
  // now we start looking for other (normal) join operations.

  Aset_t<Aptree_t> join_ops = Aglob_vars()->query->join_ops ();
  join_ops.Minus (leftoprns);
  join_ops.Minus (rightoprns); // now join_ops contains the join ops
				// which havent yet been applied.

  for (int eos = join_ops.StartTraversal ();
       !eos;
	)
    {
      Aptree_t *oprn = join_ops.CurrentElement ();
      
      // this is a join operation only if one of its operands
      // is present in the left input and the other is
      // present in the right input.
      int left_present_in_left = 
	leftoprns.HasElement (oprn->arg (0)) ? 1 : 0;
      int left_present_in_right = 
	rightoprns.HasElement (oprn->arg (0)) ? 1 : 0;
      int right_present_in_left = 
	leftoprns.HasElement (oprn->arg (1)) ? 1 : 0;
      int right_present_in_right = 
	rightoprns.HasElement (oprn->arg (1)) ? 1 : 0;

      if (left_present_in_left + left_present_in_right == 1 &&
	  right_present_in_left + right_present_in_right == 1)
	eos = join_ops.Advance ();
      else
	eos = join_ops.DeleteAndAdvance ();	
				// if this doesnt satisfy, then delete it.
    }

  if (Aglob_vars()->oopt->do_mat_to_join)
    join_ops.Union (tupleref_ops);
				// now join_ops contains all the
				// join operations.

  FOR_EACH_ELEMENT_OF_SET (join_ops)
    {
      Aptree_t *oprn = join_ops.CurrentElement ();

      Ajoin_t *newjoin = (Ajoin_t *)Duplicate ();
      newjoin->_operations.Insert (oprn);

      newjoin->_undone_tuplerefs = tupleref_ops;
      newjoin->_undone_tuplerefs.Delete (oprn);
				// tuplerefs which have to be done
				// but which haven't yet been done...
      clones.Insert (newjoin);

      // see if we can do a join using an inverse link...
      if (Aglob_vars()->oopt->do_inverse_links &&
	  oprn->is_tupleref ())
	{
	  Acat_t::attrid_t tupleref_attrid = 
	    ((Atupleref_t *)oprn->func ())->attrid ();
	  if (Aglob_vars()->cat->has_inverse (tupleref_attrid))
	    {
	      Ajoin_t *inverse_newjoin = (Ajoin_t *)Duplicate ();
	      inverse_newjoin->_operations.Insert (oprn);
	      inverse_newjoin->_undone_tuplerefs = 
		newjoin->_undone_tuplerefs;
	      inverse_newjoin->_inverse = 1;
	      clones.Insert (inverse_newjoin);
	    }
	}
    }

  return clones;
}

void 
Ajoin_t::DfsNode (Alognode_t *input, Alognode_t *othernode)
{
  if (Adont_explore (input, othernode))
    return;

  Alist_t<Abinop_t> leftclones = Clones (input, othernode); 

  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (leftclones)
    Aglob_vars()->search->NewNode (new Alognode_t (leftclones.Element (), 
						   input, othernode));

  Alist_t<Abinop_t> rightclones = Clones (othernode, input); 

  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (rightclones)
    Aglob_vars()->search->NewNode (new Alognode_t (rightclones.Element (), 
						othernode, input));
  
  Alist_t<Alognode_t> list = othernode->Children ();
  FOR_EACH_ELEMENT_OF_LIST (list)
    if (list.Element () != input)
      DfsNode (input, list.Element ());
}

void Ajoin_t::Apply (Alognode_t *input)
{
  if (input->GetPhyNodes ().IsEmpty ())
    return;			// no point in trying to expand
				// an node that has no phynodes.

  if (!Aglob_vars()->oopt->do_exhaustive_selects)
    {
      Aset_t<Aptree_t> select_oprns = Aget_select_oprns (input);
      if (!select_oprns.IsEmpty ())
	return;			// if there are any select predicates
				// applicable and if we are not doing
				// exhaustive select positioning
				// (i.e. we are doing pushdown)
				// then dont apply the join
    }

  Alist_t<Alognode_t> list = Aglob_vars()->search->GetListOfRootNodes ();
  FOR_EACH_ELEMENT_OF_LIST (list)
    DfsNode (input, list.Element ());
}

/***************************************************************************

  Aoutputop_t

***************************************************************************/

Alist_t<Aunaryop_t> Aoutputop_t::Clones (Alognode_t *input)
{
  Alist_t<Aunaryop_t> clones;
  
  Aset_t<Aptree_t> inoperations = input->GetLogProps ()->operations ();
  if (inoperations.Complement ().IsEmpty ())
    {
      Aoutputop_t *newoutputop = (Aoutputop_t *)Duplicate ();
      newoutputop->_operations = Aglob_vars()->query->target_ops ();
      clones.Insert (newoutputop);
    }
  
  return clones;
}

/***************************************************************************

  Aunnestop_t

***************************************************************************/

Alist_t<Aunaryop_t> Aunnestop_t::Clones (Alognode_t *input)
{
  Alist_t<Aunaryop_t> clones;
  
  Aset_t<Aptree_t> need_unnesting = input->GetLogProps ()->need_unnesting ();
  FOR_EACH_ELEMENT_OF_SET (need_unnesting)
    {
      Aunnestop_t *newunnestop = (Aunnestop_t *)Duplicate ();
      newunnestop->_operations.Insert (need_unnesting.CurrentElement ());
      clones.Insert (newunnestop);
    }
  
  return clones;
}

/***************************************************************************

  The algorithms

***************************************************************************/

/***************************************************************************

  Afilescan

***************************************************************************/

void Afilescan_t::MakePhyNodes (Alognode_t *lognode)
{
  Alist_t<Aalgo_t> clones = Clones (lognode);
  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (clones)
    assert (new Aphynode_t (lognode, clones.Element ()));
				// the phynode will add itself to the
				// list of phynodes of the parent.
}

/***************************************************************************

  Aindexscan

***************************************************************************/

Alist_t<Aalgo_t> Aindexscan_t::Clones (Alognode_t *lognode, 
				       Aphynode_t **inputs)
{
  Alist_t<Aalgo_t> clones;
  Aindexscan_t *newidx = (Aindexscan_t *)Duplicate (); assert (newidx);
  newidx->_idxid = ((Aidx_collapse_t *)lognode->GetOp ())->idxid ();
  clones.Insert (newidx);
  return clones;
}


void Aindexscan_t::MakePhyNodes (Alognode_t *lognode)
{
  Alist_t<Aalgo_t> clones = Clones (lognode);
  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (clones)
    assert (new Aphynode_t (lognode, clones.Element ()));
				// the phynode will add itself to the
				// list of phynodes of the parent.
}

/***************************************************************************

  Aassembly_t

***************************************************************************/

Alist_t<Aphynode_t> Aassembly_t::Enforce (Aphynode_t *node, 
					  Aphyprop_t *reqd_props)
{
  Alist_t<Aphynode_t> enforced_outputs;

  Aset_t<Aptree_t> need_inmem  = node->GetPhyProps ()->ops_not_in_memory ();
  need_inmem.Minus (reqd_props->ops_not_in_memory ());
				// these are the operations needed in memory

  FOR_EACH_ELEMENT_OF_SET (need_inmem)
    {
      Aptree_t *oprn = need_inmem.CurrentElement ();
      if (oprn->is_attref () || oprn->is_relref ())
	{
	  assert (oprn->func ()->type () != Afunc_t::tuplevar);
	  Aptree_t *relref = oprn->arg (0);
	  _tuplerefs.Insert (relref);
	  _operations.Union (Aglob_vars()->query->op_mats (relref));
	}
    }
  
  Aphynode_t *newnode = new Aphynode_t (node->GetParent (), this, node);
  assert (newnode);
  enforced_outputs.Insert (newnode);
  return enforced_outputs;
}

Alist_t<Aunaryalgo_t> Aassembly_t::Clones (Alognode_t *lognode, 
					   Aphynode_t *input)
{
  Alist_t<Aunaryalgo_t> clones;
  
  Aassembly_t *newassembly = (Aassembly_t *)Duplicate ();
  newassembly->_tuplerefs = ((Amat_t *)lognode->GetOp ())->tuplerefs ();
  newassembly->_operations = ((Amat_t *)lognode->GetOp ())->operations ();
  clones.Insert (newassembly);

  // note that the Aphynodes for these will be created in
  // Aunaryalgo_t and thus the Aphynode_t (Aunaryalgo_t *,...
  // will get called and not Aphynode_t (Aenforcer_t *,...

  return clones;
}

/***************************************************************************

  Afilter

***************************************************************************/

int Afilter_t::CanBeApplied (Alognode_t *node, Aphynode_t *input)
{
  Aset_t<Aptree_t> need_inmem;
  ((Aselect_t *)node->GetOp ())->
    compute_need_inmem (need_inmem,
			node->GetLogProps ()->operations (),
			input->GetPhyProps ()->ops_not_in_memory ());

  return need_inmem.IsEmpty ();
}

Aphyprop_t *Afilter_t::Constraint (Alognode_t *node, Aphynode_t *input, 
				   int inputnum)
{
  return
    ((Aselect_t *)node->GetOp ())->
    inmem_input_constraint (node, input, inputnum);
}

/***************************************************************************

  Ahh_join

***************************************************************************/

int Ahh_join_t::CanBeApplied (Alognode_t *node, 
			      Aphynode_t *leftinput,
			      Aphynode_t *rightinput)
{
  Aset_t<Aptree_t> need_inmem;
  Aset_t<Aptree_t> not_inmem = 
    ASetUnion (leftinput->GetPhyProps ()->ops_not_in_memory (),
	       rightinput->GetPhyProps ()->ops_not_in_memory ());
  ((Ajoin_t *)node->GetOp ())->
    compute_need_inmem (need_inmem,
			node->GetLogProps ()->operations (),
			not_inmem);
  return need_inmem.IsEmpty ();
}

Aphyprop_t *Ahh_join_t::Constraint (Alognode_t *node, Aphynode_t *input, 
				    int inputnum)
{
  return
    ((Ajoin_t *)node->GetOp ())->
    inmem_input_constraint (node, input, inputnum);
}

/***************************************************************************

  Ahhptr_join

***************************************************************************/

Alist_t<Abinalgo_t> Ahhptr_join_t::Clones (Alognode_t *node,
					   Aphynode_t *left_input,
					   Aphynode_t *right_input)
{
  Alist_t<Abinalgo_t> clones;
  Ajoin_t *joinop = (Ajoin_t *)node->GetOp ();
  if (joinop->operations ().Cardinality () != 1)
    return clones;
  
  Aptree_t *oprn = joinop->operations ().FirstElement ();
  if (!oprn->is_tupleref ())
    return clones;		// can be applied only with pointers.

  if (node->RightInput ()->GetOp ()->GetNumber () != Aget)
    return clones;		// rhs must be a get.

  Arel_t *rel = ((Aget_t *)node->RightInput ()->GetOp ())->rel ();
  
  Arel_t *link_destination;
  if (joinop->inverse ())
    link_destination = ((Arelref_t *)oprn->arg (0)->func ())->rel ();
  else
    link_destination = ((Atupleref_t *)oprn->func ())->rel ();
      
  if (rel->is_equal_to (link_destination))
    {
      Ahhptr_join_t *newjoin = (Ahhptr_join_t *)Duplicate ();
      assert (newjoin);
      clones.Insert (newjoin);
    }

  return clones;
}

int Ahhptr_join_t::CanBeApplied (Alognode_t *node, 
				 Aphynode_t *leftinput,
				 Aphynode_t *rightinput)
{
  Aset_t<Aptree_t> need_inmem;
  Aset_t<Aptree_t> not_inmem = 
    ASetUnion (leftinput->GetPhyProps ()->ops_not_in_memory (),
	       rightinput->GetPhyProps ()->ops_not_in_memory ());
  ((Ajoin_t *)node->GetOp ())->
    compute_need_inmem (need_inmem,
			node->GetLogProps ()->operations (),
			not_inmem);
  return need_inmem.IsEmpty ();
}

Aphyprop_t *Ahhptr_join_t::Constraint (Alognode_t *node, Aphynode_t *input, 
				       int inputnum)
{
  return
    ((Ajoin_t *)node->GetOp ())->
    inmem_input_constraint (node, input, inputnum);
}

/***************************************************************************

  Aunnestalgo

***************************************************************************/

int Aunnestalgo_t::CanBeApplied (Alognode_t *node, 
				 Aphynode_t *input)
{
  Aset_t<Aptree_t> need_inmem;
  ((Aunnestop_t *)node->GetOp ())->
    compute_need_inmem (need_inmem,
			node->GetLogProps ()->operations (),
			input->GetPhyProps ()->ops_not_in_memory ());

  return need_inmem.IsEmpty ();
}

Aphyprop_t *Aunnestalgo_t::Constraint (Alognode_t *node, Aphynode_t *input, 
				       int inputnum)
{
  return
    ((Aunnestop_t *)node->GetOp ())->
    inmem_input_constraint (node, input, inputnum);
}


/***************************************************************************

  Aoutputalgo

***************************************************************************/

int Aoutputalgo_t::CanBeApplied (Alognode_t *node, 
				 Aphynode_t *input)
{
  Aset_t<Aptree_t> need_inmem;
  ((Aoutputop_t *)node->GetOp ())->
    compute_need_inmem (need_inmem,
			node->GetLogProps ()->operations (),
			input->GetPhyProps ()->ops_not_in_memory ());

  return need_inmem.IsEmpty ();
}

Aphyprop_t *Aoutputalgo_t::Constraint (Alognode_t *node, Aphynode_t *input, 
				       int inputnum)
{
  return
    ((Aoutputop_t *)node->GetOp ())->
    inmem_input_constraint (node, input, inputnum);
}
