#include "Aphynode.h"
#include <Aphyprop.h>
#include "Asearch.h"

/***************************************************************************

  PHYNODE

  there can be a number of phynodes associated with each logical
  node in a search tree. each Aphynode_t is a representation of a 
  different way of physically computing the same operation (operator tree) 
  described in the given Alognode_t. these differences arise due to 
  different algorithms existing for computing the operators.

  SUBOPTIMAL NODES
  
  consider a node (n1) which has been used as an input node by
  another node (n2). now, if a third node (n3) is created and it
  happens to be equivalent to node n1 and also happens to be cheaper
  than n1, then we would delete n1. but if, in this situation, it so
  happens that miraculously n2 turns out to be part of the ultimate
  optimal plan (unlikely, but possible in the current scheme) then we
  are in trouble. because n2 will try to access n1, but n1 has already
  been deleted.

  currently i see no easy way out of this problem. so this is what i
  do: each node has a usedflag variable which will be zero if and only
  if a particular node is not used as an input by any other node. and
  when we are trying to delete a node we check if usedflag is 0. if it
  is not we dont delete it. we just remove it from the hashtable and the
  search tree. 

  ***************************************************************************/

void
Aphynode_t::DeletePhyNode (Aphynode_t *node)
{
  if (!node->usedflag)
    {
      if (!node->SubOptimal ())
	node->GetParent ()->DeletePhyNode (node); 
      else
	{
	  node->GetParent ()->DeleteSubOptimalPhyNode (node);
				// it was suboptimal. we have to delete
				// it from the list of suboptimal nodes
				// which the parent maintains.
#ifdef ADEBUG
	  Aglob_vars()->search->subopt_phynode_ctr.delete_node ();
#endif
	}
	
      for (int i=0; i<node->algorithm->Arity (); i++)
	  if (--node->inputs[i]->usedflag == 0 && node->inputs[i]->suboptimal)
	    DeletePhyNode (node->inputs[i]);
      delete node;
    }
  else if (!node->suboptimal)
    {
      node->suboptimal = 1;
      node->GetParent ()->DeletePhyNode (node); 
				// this MUST be called AFTER we set
				// node->suboptimal = 1.
#ifdef ADEBUG
      Aglob_vars()->search->subopt_phynode_ctr.add_node ();
#endif 
    }
  else
    cerr << "why did this happen?" << endl;
}

Aphynode_t::~Aphynode_t (void) 
{
  delete algorithm;
  delete [] inputs;
  delete phyprops;
}


Aphynode_t::Aphynode_t (Alognode_t *newparent, 
			Aunaryalgo_t *newalgorithm,
			Aphynode_t *input)
{
  enforcednode = usedflag = suboptimal = 0;
  parent = newparent;
  inputs = new Aphynode_t *[1]; // array of one Aphynode_t *s
  inputs[0] = input; input->usedflag++;
  parent->AddPhyNode (this); 

  algorithm = newalgorithm;

  phyprops = algorithm->MakePhyProps (this);
}

// enforcers require a slightly different treatment from
// that given to other algorithms. specifically, the
// new node should not be added as a child of the parent.
Aphynode_t::Aphynode_t (Alognode_t *newparent, 
			Aenforcer_t *newalgorithm,
			Aphynode_t *input)
{
  usedflag = suboptimal = 0;
  parent = newparent;
  inputs = new Aphynode_t *[1]; // array of one Aphynode_t *
  inputs[0] = input; input->usedflag++;
  enforcednode = 1;		// yes this is an enforced node

  // dont add this to the list of physical nodes of the parent
  
  algorithm = newalgorithm;

  phyprops = algorithm->MakePhyProps (this);
}


Aphynode_t::Aphynode_t (Alognode_t *newparent, 
			Abinalgo_t *newalgorithm,
			Aphynode_t *leftinput,
			Aphynode_t *rightinput)
{
  enforcednode = usedflag = suboptimal = 0;
  parent = newparent;
  inputs = new Aphynode_t *[2]; // array of two Aphynode_t pointers.
  inputs[0] = leftinput; leftinput->usedflag++;
  inputs[1] = rightinput; rightinput->usedflag++;
  parent->AddPhyNode (this);

  algorithm = newalgorithm;

  phyprops = algorithm->MakePhyProps (this);
}

Aphynode_t::Aphynode_t (Alognode_t *newparent, Aalgo_t *newalgorithm,
			Aphynode_t **newinputs)
{
  enforcednode = usedflag = suboptimal = 0;
  parent = newparent;
  inputs = newinputs;
  parent->AddPhyNode (this);

  for (int i=0; i<newalgorithm->Arity (); i++)
    inputs[i]->usedflag++;

  algorithm = newalgorithm;

  phyprops = algorithm->MakePhyProps (this);
}

int 
Aphynode_t::IsInteresting (void) const
{
  return phyprops->IsInteresting (GetLogProps ());
}
