#include "Asearch.h"
#include <iostream.h>
#include <stdlib.h>
#include <assert.h>
#include <Ahash.h>

#include <Aoptions.h>
#include <Aphyprop.h>

inline int is_real_lognode (Alognode_t *node)
{
  return node->GetOp ()->GetNumber () != Aidx_collapse;
}

// private function used by ~Asearch_t to
// delete the whole search tree under a given logical node.
static void 
ADeleteTree (Alognode_t *lognode)
{
  do {
    Alognode_t *child = lognode->Children ().PopFirstElement ();
    if (child) ADeleteTree (child);
  } while (!lognode->Children ().IsEmpty ());

  do {
    Aphynode_t *phynode = lognode->GetPhyNodes ().PopFirstElement ();
    if (phynode) delete phynode; // we cannot call Aphynode_t::DeletePhyNode
				 // from here because it could (ultimately)
				 // cause lognode to be deleted which is
				 // undesirable.
  } while (!lognode->GetPhyNodes ().IsEmpty ());

  delete lognode;
}

// to be called before each query is optimized
void Asearch_t::Initialize (void)
{
  Ahashid_t::InitializeHashTable ();
}

// to be called after each query done.
void Asearch_t::CleanUp (void)
{
  do {
    Alognode_t *lognode = listofrootnodes.PopFirstElement ();
    if (lognode) ADeleteTree (lognode);
  } while (!listofrootnodes.IsEmpty ());

  for (int i=0; i<Amax_operations; i++)
    assert (listofunexpandednodes[i].IsEmpty ());

  Ahashid_t::EmptyHashTable ();
}

void 
Asearch_t::NewNode (Alognode_t *node) 
{	
#ifdef ADEBUG
  if (is_real_lognode (node))
    lognode_ctr.add_node ();
#endif
  
  if (node->CanDelete ())
    {    
#ifdef ADEBUG
      if (is_real_lognode (node))
	lognode_ctr.delete_node ();
#endif
      delete node;
    }
  else 
    {
      int num_operations = node->GetLogProps ()->NumOperations ();
      assert (num_operations < Amax_operations &&
	num_operations >= unexpanded_nodes_array_index);
      listofunexpandednodes[num_operations].InsertAtEnd (node);
    }

  node->expanded = 0;		// not yet expanded
}

void 
Asearch_t::AddNodeToTree (Alognode_t *node) 
{
  // this function is called after the node has been expanded...
  if (node->GetOp ()->Arity () == 0)
    listofrootnodes.Insert (node); 
				// it is added either as a root node 
  else
    node->Input ()->AddChild (node); 
				// or as a child of an already existing node.

  node->expanded = 1;		// yes it has been expanded.
}


void
Asearch_t::DeleteLogNode (Alognode_t *lognode) 
{
  int num_operations = lognode->GetLogProps ()->NumOperations ();
  listofunexpandednodes[num_operations].FindAndDelete (lognode); 
  listofrootnodes.FindAndDelete (lognode); 

  if (optimumnode == lognode) 
    {
      // this means that a new guy who is better just got created
      // and will replace lognode as the optimumnode and soon as its
      // expansion process is complete
      optimumnode = 0;
    }

#ifdef ADEBUG
  if (is_real_lognode (lognode))
    lognode_ctr.delete_node ();
#endif
}

void
Asearch_t::ExpandNode (Alognode_t *node) 
{
  int expand_node = 1;

  if (node->GetLogProps ()->IsCompleteQuery ())
    {
      if (!node->GetPhyNodes ().IsEmpty ())
	{
#ifdef ADEBUG
	  if (optimumnode && !optimumnode->GetPhyNodes ().IsEmpty ())
	    {
				// optimumnode can have an empty phynodes list
				// because it might be an Interesting nodes
				// whose phynodes got pruned by the phynodes
				// by the phynodes of "node"

	      Aphynode_t *opt_plan = node->GetPhyNodes ().FirstElement ();
	      Aphynode_t *not_opt_plan = 
		optimumnode->GetPhyNodes ().FirstElement ();
	      assert (opt_plan->GetPhyProps ()->GetCost () <
		      not_opt_plan->GetPhyProps ()->GetCost ());
	    }
#endif
	  optimumnode = node;
				// remember that there can be only one
				// final node.  and that is the optimumnode.
				// this is because the hashing will prune
				// out all others
	}

      if (!node->GetLogProps ()->IsInteresting ())
	expand_node = 0;
    }

  if (expand_node)
    {
      Alist_t<Aop_t> &listofoperators = GetListOfOperatorsToApply (node);
      FOR_EACH_ELEMENT_OF_LIST (listofoperators)
	listofoperators.Element ()->Apply (node);
    }

  AddNodeToTree (node);
  return;
}


Aphynode_t *
Asearch_t::Optimize (void)
{
  optimumnode = 0;

#ifdef ADEBUG
  lognode_ctr.reset ();
  phynode_ctr.reset ();
  subopt_lognode_ctr.reset ();
  subopt_phynode_ctr.reset ();
#endif

  MakeInitialTree (); // make the atomic nodes

  do {
    do {
      Alognode_t *lognode = 
	listofunexpandednodes[unexpanded_nodes_array_index].PopFirstElement ();
      if (lognode) ExpandNode (lognode);
    } while (!listofunexpandednodes[unexpanded_nodes_array_index].IsEmpty ());
  } while (++unexpanded_nodes_array_index < Amax_operations);

  assert (optimumnode);
  int numoptimalplans = optimumnode->NumPhyNodes ();
  assert (numoptimalplans == 1);
  
  Aphynode_t *best_access_plan = optimumnode->GetPhyNodes ().FirstElement ();
		// the optimum logical node has only one element and that
		// is the best access plan

  return best_access_plan;
}
