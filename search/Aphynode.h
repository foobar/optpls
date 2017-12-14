#ifndef APHYNODE_H
#define APHYNODE_H

#include <Alist.h>
#include <Aalgo.h>
#include <Alognode.h>

class Aphyprop_t; // forward reference

/***************************************************************************

  PHYNODE

  there can be a number of phynodes associated with each logical
  node in a search tree. each Aphynode_t is a representation of a 
  different way of physically computing the same operation (operator tree) 
  described in the given ALogNode. these differences arise due to 
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

class Aphynode_t {
private:
  Aalgo_t *algorithm;
  Aphynode_t **inputs; // array of pointers to input Aphynode_ts
  Aphyprop_t *phyprops;
  Alognode_t *parent;
  int usedflag;	// this flag is true if this node has been used by
		// some other node as an input. in this case we should
		// not delete this node. we just mark it as suboptimal.
  int suboptimal; // this marks nodes which are suboptimal but cannot be
		  // deleted because they are used as inputs to other nodes.
		  // these nodes should not considered as inputs for other
		  // nodes
  int enforcednode; // true if this node is the result of the application
		    // of an enforcer to another. such nodes have to be
		    // handled differently in some cases.

public:
  ~Aphynode_t (void);
  static void DeletePhyNode (Aphynode_t *node);
  Aphynode_t (Alognode_t *parent, Aunaryalgo_t *, Aphynode_t *input);
  Aphynode_t (Alognode_t *parent, Abinalgo_t *, 
	      Aphynode_t *leftinput, Aphynode_t *rightinput);
  Aphynode_t (Alognode_t *parent, Aalgo_t *, Aphynode_t **inputs = 0);
  Aphynode_t (Alognode_t *parent, Aenforcer_t *, Aphynode_t *input);

  Aalgo_t *GetAlgo (void) const {return algorithm;}
  int GetNumInputs (void) const {return algorithm->Arity ();}
  void SetPhyProps (Aphyprop_t *p) {phyprops = p;}

  Aphynode_t *Input (int N = 0) const {return inputs[N];} // get the Nth input
  Aphynode_t *LeftInput (void) const {return inputs[0];}  // special cases
  Aphynode_t *RightInput (void) const {return inputs[1];} // for binary nodes

  Aphyprop_t *GetPhyProps (void) const {return phyprops;}
  Alogprop_t *GetLogProps (void) const {return parent->GetLogProps ();}
  Alognode_t *GetParent (void) const {return parent;}

  int IsInteresting (void) const; // is this physical node interesting.
  int SubOptimal (void) const {return suboptimal;}
		// true if this is a suboptimal node.
  int IsUsed (void) const {return usedflag;}
				// not for general use.
				// used only by Aalgo_t::Apply
  int IsEnforcedNode (void) const {return enforcednode;}
};

#endif /* APHYNODE_H */
