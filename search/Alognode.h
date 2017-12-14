#ifndef ALOGNODE_H
#define ALOGNODE_H

#include <Alist.h>
#include <Aop.h>

class Aphynode_t; // forward reference
class Alogprop_t;


/***************************************************************************

  LOGICAL NODES.

  a search tree is made up of logical nodes and physical nodes.

  A logical node represents a part of an operator tree which can be
  used in the evaluation of the given query.
  the logical node has a pointer the operator which is being applied,
  and the lognodes which serve as inputs to it.
  
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

class Alognode_t {
private:
  Aop_t *op; 
  Alogprop_t *logprops;
  Alognode_t **inputs; // array of input Alognode_ts
  Alist_t<Aphynode_t> phynodes; // list of physical nodes
  Alist_t<Alognode_t> children; // logical nodes generated from this one
  Alist_t<Aphynode_t> suboptimal_phynodes; 
				// see comment in Aphynode.h
  Alist_t<Aphynode_t> enforcednodes;
				// list of nodes created by enforcers
				// being applied to phynodes of this node
  Alist_t<Alognode_t> dependent_nodes;
				// list of log nodes that MUST be deleted
				// when this node is deleted. i.e. nodes
				// that use this node as an input and
				// will have to be forcefully deleted if
				// this node is suboptimal.
				// Note: in general, if this node is suboptimal
				// nodes that use this node as an input
				// will get automatically deleted. but this is
				// not true if they are interesting. in this
				// case we have to delete them forcefully...


  int done;	// a flag indicating whether this logical node is in
		// the process of being created or it is completed.
		// this is used to decide whether a logical node is to be
		// deleted or not when the number of physical nodes becomes 0.
  

  int IsDone (void) const {return done;}

public:
  int expanded;			// true if this node has already been
				// expanded.

public:
  ~Alognode_t (void);
  Alognode_t (Aunaryop_t *, Alognode_t *input);
  Alognode_t (Abinop_t *, Alognode_t *leftinput, 
		  Alognode_t *rightinput);

  Alognode_t (Aop_t *, Alognode_t **inputs = 0);
		// to take care of operators which are neither unary
		// nor binary. an important special case is operators
		// which dont take any input these from the leaves of
		// our operator trees and access plans. the inputs = 0
		// is for that case
  
  Aop_t *GetOp (void) const {return op;}
  Alogprop_t *GetLogProps (void) const {return logprops;}
  void SetLogProps (Alogprop_t *l) {logprops = l;}

  int CanRemoveFromSearch (void); 
				// TRUE if this lognode can be removed
				// from the search tree. i.e. it is
				// suboptimal
  int CanDelete (void);		// TRUE if this lognode can be deleted safely

  Alist_t<Alognode_t> &Children (void) {return children;}
  void AddChild (Alognode_t *newchild) {children.Insert (newchild);}
  void DeleteChild (Alognode_t *child) {children.FindAndDelete (child);}
  void AddDependent (Alognode_t *node) {dependent_nodes.Insert (node);}
  void DeleteDependent (Alognode_t *n) {dependent_nodes.FindAndDelete (n);}
  
  Alist_t<Aphynode_t> &GetPhyNodes (void) {return phynodes;}
  int NumPhyNodes (void) {return phynodes.Length ();}
  void AddPhyNode (Aphynode_t *node) {phynodes.Insert (node);}
  void DeletePhyNode (Aphynode_t *node);
  void DeleteSubOptimalPhyNode (Aphynode_t *node);
  void AddEnforcedNode (Aphynode_t *node) {enforcednodes.Insert (node);}

  Alognode_t *Input (int N = 0) const {return inputs[N];} // get the Nth input

  Alognode_t *LeftInput (void) const {return inputs[0];}  // special cases 
  Alognode_t *RightInput (void) const {return inputs[1];}	// of the above

  Alist_t<Aphynode_t> &MakePhyNodes (void);
};


#endif /* ALOGNODE_H */
