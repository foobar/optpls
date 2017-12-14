#include "Alognode.h"
#include "Aphynode.h"
#include "Aalgo.h"

#include <Alogprop.h> // user must provide this file
#include <Aphyprop.h>
#include "Asearch.h"

#include <iostream.h>
#include <stdlib.h>


/***************************************************************************

  LOGICAL NODES.

  a search tree is made up of logical nodes and physical nodes.

  A logical node represents a part of an operator tree which can be
  used in the evaluation of the given query.
  the logical node has a pointer the operator which is being applied,
  and the logical nodes which serve as inputs to it.
  
  There is also a pointer to Alogprop_t of the output
  of the partial operator tree associated with the logical node.
  the Alogprop_t class is supplied by the DBI.
  
  a logical node also has pointers to all the physical nodes
  associated with this logical node. i.e. each physical node
  represents a different possible physical implementation which
  can be used to evaluate the partial operator tree associated
  with this logical node.

  Also there is a list of children of this logical node.
  Children are those logical nodes which result from the application
  of some Operator::Apply to this logical node.

  ***************************************************************************/
// returns TRUE if this lognode can be removed from the search tree.
// i.e. it is suboptimal and should not be considered for further expand's.
int Alognode_t::CanRemoveFromSearch (void)
{
  return phynodes.IsEmpty () && IsDone () && 
    !GetLogProps ()->IsInteresting ();
}


// returns true if this log node can be safely deleted
int Alognode_t::CanDelete (void)
{
  return CanRemoveFromSearch () && suboptimal_phynodes.IsEmpty ();
}


Alognode_t::~Alognode_t (void)
{
  while (!children.IsEmpty ())
    delete children.FirstElement ();
				// these children are sub-optimal.
				// they dont have any phynodes and dont
				// have any hopes of getting a phynode
				// later. they are just around because
				// they are "interesting". so delete them.

  while (!dependent_nodes.IsEmpty ())
    {
      Alognode_t *dep_node = dependent_nodes.FirstElement ();
      Aglob_vars()->search->DeleteLogNode (dep_node);
      delete dep_node;		// list of log nodes that MUST be deleted
				// when this node is deleted. i.e. nodes
				// that use this node as an input and
				// will have to be forcefully deleted if
				// this node is suboptimal.
				// Note: in general, if this node is suboptimal
				// nodes that use this node as an input
				// will get automatically deleted. but this is
				// not true if they are interesting. in this
				// case we have to delete them forcefully...
    }


  if (!phynodes.IsEmpty () || !IsDone ())
    {
      cerr << "Alognode_ts.~Alognode_t: deleting useful node...\n";
      exit (1);
    }

  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (suboptimal_phynodes)
    delete suboptimal_phynodes.Element ();
				// if suboptimal_phynodes is not empty,
				// then it means that we have completed the
				// optimization and now we are cleaning
				// up our data structures...

  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (enforcednodes)
    delete enforcednodes.Element ();
				// same as above

  int op_arity = op->Arity ();
  if (op_arity > 0)
    {
      inputs[0]->DeleteChild (this); // delete self from children of parent
      if (GetLogProps ()->IsInteresting ())
	for (int i=0; i<op_arity; i++) inputs[i]->DeleteDependent (this);
    }

  delete op;
  delete logprops;
  delete [] inputs;		 
}

Alognode_t::Alognode_t (Aunaryop_t *newoperator, 
			Alognode_t *input)
{
  done = 0;
  op = newoperator;
  
  inputs = new Alognode_t * [1]; // inputs is an array of 1 element
  inputs [0] = input; 

  logprops = newoperator->MakeLogProps (this);
  if (GetLogProps ()->IsInteresting ()) input->AddDependent (this);

  MakePhyNodes ();
  done = 1;
}


Alognode_t::Alognode_t (Abinop_t *newoperator, 
			Alognode_t *leftinput,
			Alognode_t *rightinput)
{
  done = 0;
  op = newoperator;

  inputs = new Alognode_t * [2]; // array of two logical node pointers
  inputs[0] = leftinput;
  inputs[1] = rightinput;

  logprops = newoperator->MakeLogProps (this);
  if (GetLogProps ()->IsInteresting ()) 
    {
      leftinput->AddDependent (this);
      rightinput->AddDependent (this);
    }

  MakePhyNodes ();
  done = 1;
}

// this should be used for Nullary Aop_ts (operators that take
// no inputs). i.e. this should be used for setting up the initial
// search tree.
Alognode_t::Alognode_t (Aop_t *newoperator, 
			Alognode_t **newinputs)
{
  done = 0;
  op = newoperator;

  inputs = newinputs;

  logprops = newoperator->MakeLogProps (this);
  if (GetLogProps ()->IsInteresting ())
    for (int i=0; i<op->Arity (); i++)
      inputs[0]->AddDependent (this);

  MakePhyNodes ();
  done = 1;
}

void 
Alognode_t::DeletePhyNode (Aphynode_t *node)
{
  phynodes.FindAndDelete (node);

  if (node->IsEnforcedNode ())
    enforcednodes.FindAndDelete (node);

  if (node->SubOptimal ())
    suboptimal_phynodes.Insert (node);

  else if (CanRemoveFromSearch ())
    {
      Aglob_vars()->search->DeleteLogNode (this);
      // if all the physical nodes of this logical node are deleted
      // and if there is no hope of new ones being added then
      // delete this logical node from the search tree.
      
      if (suboptimal_phynodes.IsEmpty ())
	delete this;
      // if this node has a phynode which is suboptimal then it should not
      // be deleted. but it will be a useless lognode, if all its
      // phynodes are suboptimal.

#ifdef ADEBUG
      else
	Aglob_vars()->search->subopt_lognode_ctr.add_node ();
#endif
    }
}

void 
Alognode_t::DeleteSubOptimalPhyNode (Aphynode_t *node)
{
  assert (node->SubOptimal ());
  suboptimal_phynodes.FindAndDelete (node);
  if (CanDelete ())
    {
      delete this;		// this means that it was a suboptimal
				// lognode sitting around just for this
				// suboptimal phynode. hence delete it.
#ifdef ADEBUG
      Aglob_vars()->search->subopt_lognode_ctr.delete_node ();
#endif
    }
}

Alist_t<Aphynode_t> &
Alognode_t::MakePhyNodes (void)
{
  Alist_t<Aalgo_t> list = op->GetListOfAlgorithms ();
  FOR_EACH_ELEMENT_OF_LIST (list)
    {
      Aalgo_t *algorithm = list.Element ();
      algorithm->MakePhyNodes (this);
    }
  return phynodes;
}
