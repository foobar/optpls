#include <iostream.h>
#include <stdlib.h>

#include "Aop.h"
#include "Alognode.h"
#include "Asearch.h"

// Apply operator to take input and create new nodes in the search tree
void Aop_t::Apply (Alognode_t *lognode)
{
  Alist_t<Aop_t> clones = Clones ();
  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (clones)
    {
      Alognode_t *newnode = new Alognode_t (clones.Element ());
      assert (newnode);
      Aglob_vars ()->search->NewNode (newnode);
    }
}


void
Aunaryop_t::Apply (Alognode_t *input)
{
  Alist_t<Aunaryop_t> clones = Clones (input);
		// clones is the list of operator objects
		// all of which apply this operator
		// to this input, but with different parameters
		// (e.g. different predicates.)
		// parameters are part of the user supplied data
		// members for each operator.

  FOR_EACH_ELEMENT_OF_LIST (clones)
    {
      Alognode_t *newnode = 
	new Alognode_t (clones.Element (), input);
      assert (newnode);
      Aglob_vars()->search->NewNode (newnode);
		// apply each clone to each input and register the resultant
		// lognode 
    }
}

/***************************************************************************

  Clones

  this function is supposed to return the list of operator objects
  all of which represent different ways of applying this operator
  to this input, but with different parameters (e.g. with different
  select predicates). 

  if this operator cannot be applied at all to this input then this
  function is expected to return an empty list.

  NOTE: these represent differences in the parameters of the operator
  which will result in (possibly) different outputs; this has nothing
  to do with physically different methods of implementing a given
  operator (with given parameters) all of which MUST give the same
  logical output (with possibly different physical properties).

  thus "apply select with predicate a = b" and "apply select with predicate
  a = d" can be two different clones for the select operator; while
  "apply index scan using predicate a = b and index on a" and
  "apply sequential scan using predicate a = b" are two different physical
  methods for implementing the first clone of this example.

  the DBI is supposed to provide a Clones function with the operator
  s/he defines. In case one is not provided, then it defaults to this one
  which essentially says that there is only one way of applying this
  operator to this input.

***************************************************************************/


Alist_t<Aop_t>
Aop_t::Clones (Alognode_t **inputs)
{
  Alist_t<Aop_t> clones;
  clones.Insert (Duplicate ());
  return clones;
}

Alist_t<Aunaryop_t> 
Aunaryop_t::Clones (Alognode_t *input)
{
  Alist_t<Aunaryop_t> clones;
  clones.Insert ((Aunaryop_t *)Duplicate ());
  return clones;
}

// this is a private function used by Abinop_t::Apply
// it effectively performs a depth first search of the search tree
// for suitable nodes which can be paired with "input."
void 
Abinop_t::DfsNode (Alognode_t *input, Alognode_t *othernode)
{
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

// find all the nodes in the search tree which can (should)
// be combined up with the given node using this operator
// to give rise to a new node in the search tree.
void
Abinop_t::Apply (Alognode_t *input)
{
  Alist_t<Alognode_t> list = Aglob_vars()->search->GetListOfRootNodes ();
  FOR_EACH_ELEMENT_OF_LIST (list)
    DfsNode (input, list.Element ());
}

// see comment at Aunaryop_t::Clones.
Alist_t<Abinop_t> 
Abinop_t::Clones (Alognode_t *leftinput, 
			   Alognode_t *rightinput)
{
  Alist_t<Abinop_t> clones;
  clones.Insert ((Abinop_t *)Duplicate ());
  return clones;
}
