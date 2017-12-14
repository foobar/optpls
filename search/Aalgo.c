#include <iostream.h>
#include <stdlib.h>

#include "Aalgo.h"
#include "Ahash.h"
#include "Aphynode.h"
#include "Aphyprop.h"
#include "Asearch.h"
#include "Aoptions.h"

/***************************************************************************

  PRUNING

  whenever a new physical node is created 
  there is potential for ii_cost based pruning.

  remember that each physical node stands for an access plan
  (for whatever partial operator tree it is supposed to be
  implementing).

  so if there exists a physical node in our search tree which
  produces the the same output as the new node (i.e. all the 
  logical properties are same AND the physical properties are also 
  the same) then we keep only the the less expensive one around
  to be considered for further optimization. the more expensive
  one is pruned out. we call this an EXACT MATCH.
  in this case, if the new node is less expensive then it
  REPLACEs the older one in the search tree or if it is more 
  expensive then (since it is not useful to anyone) it commits SUICIDE
  by deleting itself.
  
  if no exact match is found we continue with pruning.

  if there is a physical node whose logical properties are the
  same as the new node and its physical properties are not
  but its physical properties are SUBSUMED by the new node
  and it is more expensive than the new node then that node
  can be pruned out (because the new node can provide everything
  that the old one provided (and more) at a lesser ii_cost).
  by physical properties of one node being SUBSUMED by another
  node we mean that either the physical properties of the former
  are all present in the latter also (a subset relationship)
  or the physical properties of the former are NOT INTERESTING.
  the desicion of whether the physical properties of a node are
  SUBSUMED by those of another is left to the DBI (database
  implementor).
  for example, a physical node which produces a relation in
  sorted order SUBSUMES a physical node which produces a relation
  in random order or even one which produces a relation in a 
  sorted order which is NOT INTERESTING.
  in such a situation we call the new node a REPLACER (because
  it is going to remove/KILL the older node from the search tree) and
  the new node a REPLACEE.

  the situation is reversed if the physical properties if the new
  node are SUBSUMED by those of an already existing node. in that
  case if the latter node is less expensive then the new node need
  not be kept around for the later phased of optimization. hence the
  new node commits SUICIDE.

***************************************************************************/

inline int mcond (int condition, int anti)
{
  return anti ? !condition : condition;
}

void
Aalgo_t::Prune (Aphynode_t *phynode)
{
  if (!phynode)
    return;

  Ahashid_t hashid (phynode);
  Aphynode_t *othernode;

  int accuracy_num = rand () % 100;
  int anti_prune = accuracy_num < Aglob_vars ()->oopt->accuracy ? 1 : 0;

#ifdef ADEBUG
  Aglob_vars()->search->phynode_ctr.add_node ();
#endif
  
  // if there is an exact match, delete the lower ii_cost one
  if (othernode = hashid.GetExactMatch ()) 
    {
#ifdef ADEBUG
      Aglob_vars()->search->phynode_ctr.delete_node ();
#endif
      if (mcond (phynode->GetPhyProps ()->GetCost () < 
		 othernode->GetPhyProps ()->GetCost (), anti_prune))
	{hashid.Replace (); Aphynode_t::DeletePhyNode (othernode);}
      else
	{Aphynode_t::DeletePhyNode  (phynode); return;}
    }

  else // insert yourself as a representative of this type.
      hashid.Insert ();

  // if there is any REPLACER with a lower ii_cost, then delete yourself
  while (othernode = hashid.GetNextReplacer ()) 
    if (othernode->GetPhyProps ()->GetCost () < 
	phynode->GetPhyProps ()->GetCost ())
      {
	hashid.Suicide (); 
	Aphynode_t::DeletePhyNode  (phynode); 
#ifdef ADEBUG
	Aglob_vars()->search->phynode_ctr.delete_node ();
#endif
	return;
      }


  // if there is someone whom you can replace AND it has a higher ii_cost
  // delete the sucker.
  if (othernode = hashid.GetReplacee ()) // if there is a replacee
      if (phynode->GetPhyProps ()->GetCost () < 
	  othernode->GetPhyProps ()->GetCost ())
	{
	  hashid.KillReplacee (); 
	  Aphynode_t::DeletePhyNode  (othernode);
#ifdef ADEBUG
	  Aglob_vars()->search->phynode_ctr.delete_node ();
#endif
	}

  return;
}

/***************************************************************************

  Clones

  this function is supposed to return the list of algorithm objects
  all of which represent different ways of applying this algorithm
  to these inputs, but with different parameters.

  if this algorithm cannot be applied at all to these inputs then this
  function is expected to return an empty list.

  the DBI is supposed to provide a Clones function with the algorithm
  s/he defines. In case one is not provided, then it defaults to this one
  which essentially says that there is only one way of applying this
  operator to this input. 

  -----

  first the Clones function is called.
  then for each clone, the Constraints are checked.
  to enforce the constraints, the enforcers are used.
  then CanBeApplied function is used to check if the
   enforcers have done their work.
  finally, if some combination of inputs passes through all
   this, then the new phynode is created.

***************************************************************************/

Alist_t<Aalgo_t>
Aalgo_t::Clones (Alognode_t *lognode, Aphynode_t **inputs)
{
  Alist_t<Aalgo_t> clones;
  clones.Insert (Duplicate ());
  return clones;
}


/***************************************************************************

  an algorithm can have constraints on its inputs.
  i.e. it might require that the Aphynode_t which serves as
  its Nth input should satisfy certain physical properties.
  
  the Constraint function tells us what those constraints are.
  the Constraint function takes a Aphynode_t which will
  serve as the Nth input and returns the Aphyprop_t
  which are required of that node if it has to serve as the Nth input.

  each algorithm has stored with it the Aenforcer_ts (which are
  special cases of Aunaryalgo_ts) 
  required to enforce the required physical properties on its
  inputs. there is an array of list of enforcers: one list of enforcers
  for each input. this is because 1) each input may have separate 
  constraints. 2) Each constraint may be enforced by more than one
  enforcer.
  
  The EnforceNthConstraint makes sure that he Nth input 
  satisfies the required constraints. It calls the Constraint 
  function to find out what physical properties are required and
  then called each of the enforcers associated with the Nth input.
  each enforcer returns a list of Aphynode_ts which might result
  from the application of that enforcer to the Nth input node.
  these resultant Aphynode_ts are used as the actual inputs of
  this algorithm.
  
***************************************************************************/


Alist_t<Aphynode_t>
Aalgo_t::EnforceNthConstraint (Alognode_t *lognode, Aphynode_t *input, int N)
{
  Alist_t<Aphynode_t> resultantnodes;
  Aphyprop_t *requiredproperties;

  if (requiredproperties = Constraint (lognode, input, N))
    {
      if (enforcers == 0)
	return resultantnodes; // no enforcers provided. cant do anything.

      Alist_t<Aenforcer_t> list = enforcers[N];
      FOR_EACH_ELEMENT_OF_LIST (list) // try each enforcer in turn
	{
	  Alist_t<Aphynode_t> tmp =           
	    list.Element ()->Enforce (input, requiredproperties);
	  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (tmp) // application of an
							  // enforcer can
							  // result in more
							  // than one node
	    resultantnodes.Insert (tmp.Element ()); 
	}
      delete requiredproperties; // allocated using "new" in Constraint
    }
  else
    {
      // it already satisfies the input constraints OR 
      // this node cannot be used as an input to this algorithm at all
      resultantnodes.Insert (input);
    }

  return resultantnodes;
}

Alist_t<Aunaryalgo_t> 
Aunaryalgo_t::Clones (Alognode_t *lognode, Aphynode_t *input)
{
  Alist_t<Aunaryalgo_t> clones;
  clones.Insert ((Aunaryalgo_t *)Duplicate ());
  return clones;
}

// create all the physical nodes that can result from the
// application of this algorithms to the inputs
// of lognode.
void 
Aunaryalgo_t::MakePhyNodes (Alognode_t *lognode)
{
  // dont change this to use the GetNthPhyNode function
  // remember that will be O(n^2).
  Alist_t<Aphynode_t> inputlist = 
    lognode->Input ()->GetPhyNodes ();


  FOR_EACH_ELEMENT_OF_LIST (inputlist)
    {
      if (!inputlist.Element ()->SubOptimal ())
	Apply (lognode, inputlist.Element ());
		// Apply will apply this algorithm to the given input
		// and insert the resultant Aphynode_t(s) into the
		// lognode.phynodes list after pruning.
		// apply only if the node is not suboptimal.
      
      }
}

void
Aunaryalgo_t::Apply (Alognode_t *lognode, Aphynode_t *input)
{
  Alist_t<Aunaryalgo_t> clones = Clones (lognode, input);
				// find the clones
  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (clones)
    {				// for each clone
      Aunaryalgo_t *clone = clones.Element ();
      Alist_t<Aphynode_t> enforcedinputs = 
	clone->EnforceNthConstraint (lognode, input, 0); 
				// enforce constraints
      int not_first = 0;	// hack to avoid unnecessary Duplicates. 
      FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (enforcedinputs)
	if (clone->CanBeApplied (lognode, enforcedinputs.Element ()))
	  {
	  Aphynode_t *node = enforcedinputs.Element ();
	  if (clone->CanBeApplied (lognode, node))
	    {
	      Aunaryalgo_t *algo = 
		(Aunaryalgo_t *)(not_first++ ? clone->Duplicate () : clone);
	      Prune (new Aphynode_t (lognode, algo, node));
	    }

	  if (node->IsEnforcedNode ())
	    if (node->IsUsed ())
	      node->GetParent ()->AddEnforcedNode (node);
	    else
	      Aphynode_t::DeletePhyNode (node);
	  }
      
      if (!not_first) delete clone;
    }
}


// this is different from Aunaryalgo_t::Apply because of the fact
// that physical nodes created by an enforcer should not be Pruned
//
// An enforcer does not need an apply function
//
// void
// Aenforcer_t::Apply (Alognode_t *lognode, Aphynode_t *input)
// {
//   Alist_t<Aunaryalgo_t> clones = Clones (lognode, input);
// 				// find the clones
//   FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (clones)
//     {				// for each clone
//       Aunaryalgo_t *clone = clones.Element ();
//       Alist_t<Aphynode_t> enforcedinputs = 
// 	clone->EnforceNthConstraint (lognode, input, 0); 
// 				// enforce constraints
//       int not_first = 0;	// hack to avoid unnecessary Duplicates. 
//       FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (enforcedinputs)
// 	if (clone->CanBeApplied (lognode, enforcedinputs.Element ()))
// 	  {
// 	  Aphynode_t *node = enforcedinputs.Element ();
// 	  if (clone->CanBeApplied (lognode, node))
// 	    {
// 	      Aunaryalgo_t *algo = 
// 		(Aunaryalgo_t *)(not_first++ ? clone->Duplicate () : clone);
// 	      new Aphynode_t (lognode, algo, node);
// 	    }

// 	  if (node->IsEnforcedNode ())
// 	    if (node->IsUsed ())
// 	      node->GetParent ()->AddEnforcedNode (node);
// 	    else
// 	      Aphynode_t::DeletePhyNode (node);

// 	  }
      
//       if (!not_first) delete clone;
//     }
// }


Alist_t<Abinalgo_t> 
Abinalgo_t::Clones (Alognode_t *lognode, 
			    Aphynode_t *leftinput,
			    Aphynode_t *rightinput)
{
  Alist_t<Abinalgo_t> clones;
  clones.Insert ((Abinalgo_t *)Duplicate ());
  return clones;
}

void
Abinalgo_t::MakePhyNodes (Alognode_t *lognode)
{
  Alist_t<Aphynode_t> leftinputlist = lognode->LeftInput ()->GetPhyNodes ();
  Alist_t<Aphynode_t> rightinputlist = lognode->RightInput ()->GetPhyNodes ();

  FOR_EACH_ELEMENT_OF_LIST (leftinputlist)
    if (!leftinputlist.Element ()->SubOptimal ())
      FOR_EACH_ELEMENT_OF_LIST (rightinputlist)
	{
	  if (!rightinputlist.Element ()->SubOptimal ())
	    Apply (lognode, leftinputlist.Element (), 
		   rightinputlist.Element ());
	}
}


void
Abinalgo_t::Apply (Alognode_t *lognode, 
		   Aphynode_t *leftinput, Aphynode_t *rightinput)
{
  Alist_t<Abinalgo_t> clones = Clones (lognode, leftinput, rightinput);
				// get the clones
  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (clones)
    {				// for each clone
      Abinalgo_t *clone = clones.Element ();
      Alist_t<Aphynode_t> enforcedleftinputs = 
	clone->EnforceNthConstraint (lognode, leftinput, 0);
      Alist_t<Aphynode_t> enforcedrightinputs = 
	clone->EnforceNthConstraint (lognode, rightinput, 1);
  
      int not_first = 0;	// hack to avoid unnecessary Duplicate()'s.

      FOR_EACH_ELEMENT_OF_LIST (enforcedleftinputs)
	FOR_EACH_ELEMENT_OF_LIST (enforcedrightinputs)
	if (clone->CanBeApplied (lognode, enforcedleftinputs.Element (),
				 enforcedrightinputs.Element ()))
	  {
	    Abinalgo_t *algo = 
	      (Abinalgo_t *)(not_first++ ? clone->Duplicate () : clone);
	    Prune (new Aphynode_t (lognode, algo,
				   enforcedleftinputs.Element (), 
				   enforcedrightinputs.Element ()));
	  }

      // now delete the enforced nodes that were not used and
      // add those that were used to the listofenforcednodes in
      // the Asearch_t object.
      FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (enforcedleftinputs)
	{
	  Aphynode_t *node = enforcedleftinputs.Element ();
	  if (node->IsEnforcedNode ())
	    if (node->IsUsed ())
	      node->GetParent ()->AddEnforcedNode (node);
	    else
	      Aphynode_t::DeletePhyNode (node);
	}

      FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (enforcedrightinputs)
	{
	  Aphynode_t *node = enforcedrightinputs.Element ();
	  if (node->IsEnforcedNode ())
	    if (node->IsUsed ())
	      node->GetParent ()->AddEnforcedNode (node);
	    else
	      Aphynode_t::DeletePhyNode (node);
	}

      if (!not_first) delete clone;
    }
}

