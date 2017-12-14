#ifndef AOPALGOS_H
#define AOPALGOS_H

#include <Aop.h>
#include <Aalgo.h>
#include <Arel.h>
#include <Alogprop.h>
#include <Aphyprop.h>


/***************************************************************************

  The operators.
  Aget, Aselect, Ajoin and Amat
  and also, Aidx_collapse

***************************************************************************/

// this class tells what operation is being applied by
// an operator.
class Aop_operation_t {
protected:
  Aset_t<Aptree_t> _operations;	// the operations that are being applied
				// by this operator

public:
  Aset_t<Aptree_t> &operations (void) {return _operations;}
  Aptree_t &operation (void) {return *_operations.FirstElement ();}
				// assuming that this operator is
				// applying only a single operation

  void compute_need_inmem (Aset_t<Aptree_t> &need_inmem, 
			   Aset_t<Aptree_t> &computed, 
			   Aset_t<Aptree_t> &not_inmem);
  Aphyprop_t *inmem_input_constraint (Alognode_t *node,
				      Aphynode_t *input, int inputnum);
};

class Aget_t : public Aop_t, public Aop_operation_t {
private:
  Aptree_t *_relref;
  void compute_operations (void);

public:
  ~Aget_t (void) {}
  Aget_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate (void) const;

  int Arity (void) const {return 0;}
  Alogprop_t *MakeLogProps (Alognode_t *node);
  void Apply (Alognode_t *lognode = 0);
  Alist_t<Aop_t> Clones (Alognode_t **inputs = 0);
  Aptree_t *relref (void) const {return _relref;}
  Arel_t *rel (void) const {return ((Arelref_t*)_relref->func ())->rel ();}
};

class Amat_t : public Aunaryop_t, public Aop_operation_t {
private:
  void compute_operations (void);

protected:
  Aset_t<Aptree_t> _tuplerefs;	// tuplerefs that are being assembled.
  Amat_t (char *newname, AopNumber n, Alist_t<Aalgo_t> l);
				// for the Amat_collapse_t operator.

public:
  ~Amat_t (void) {}
  Amat_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate (void) const;

  Alist_t<Aunaryop_t> Clones (Alognode_t *input);
  Alogprop_t *MakeLogProps (Alognode_t *node);
  Aset_t<Aptree_t> &tuplerefs (void) {return _tuplerefs;}
  Aptree_t *tupleref (void);
  Arel_t *rel (void) {return ((Arelref_t*)tupleref ()->func ())->rel ();}
};

class Amat_collapse_t : public Amat_t {
private:
  Alognode_t *_matnode;		// the matnode that was collapsed

public:
  ~Amat_collapse_t (void) {}
  Amat_collapse_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate (void) const;

  Alognode_t *matnode (void) const {return _matnode;}
  Alogprop_t *MakeLogProps (Alognode_t *node);
  void Apply (Alognode_t *lognode);
};

class Aselect_t : public Aunaryop_t, public Aop_operation_t {
private:
  int _multi_select;		// says if there can be another
				// select applying to this immediately.
protected:
  Aselect_t (char *newname, AopNumber n, Alist_t<Aalgo_t> l);
				// for the Aselect_collapse_t operator.
  
public:
  ~Aselect_t (void) {}
  Aselect_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate (void) const;

  int multi_select (void) const {return _multi_select;}
  Alist_t<Aunaryop_t> Clones (Alognode_t *input);
  Alogprop_t *MakeLogProps (Alognode_t *node);
};

class Aselect_collapse_t : public Aselect_t {
private:
  Alognode_t *_select_node;
public:
  ~Aselect_collapse_t (void) {}
  Aselect_collapse_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate (void) const;
  Alognode_t *select_node (void) const {return _select_node;}

  void Apply (Alognode_t *input);
  Alogprop_t *MakeLogProps (Alognode_t *node);
};

// indexscan does not take any inputs!
class Aidx_collapse_t : public Aop_t, public Aop_operation_t {
private:
  Alognode_t *_select_node;	// pointer to the select node
				// which is being collapsed
  Acat_t::idxid_t _idxid;	// id of the index thats being used.

public:
  ~Aidx_collapse_t (void) {}
  Aidx_collapse_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate (void) const;

  Alognode_t *select_node (void) const {return _select_node;}
  Acat_t::idxid_t idxid (void) const {return _idxid;}

  int Arity (void) const {return 0;}
  void Apply (Alognode_t *lognode);
//  Alist_t<Aop_t> Clones (Alognode_t *input);
  Alogprop_t *MakeLogProps (Alognode_t *node);
};
  

class Ajoin_t : public Abinop_t, public Aop_operation_t {
private:
  Aset_t<Aptree_t> _undone_tuplerefs;
  
  int _inverse;			// assumes that this is a tupleref
				// join. and says to use the inverse.

  void DfsNode (Alognode_t *input, Alognode_t *otherinput);

public:
  ~Ajoin_t (void) {_inverse = 0;}
  Ajoin_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate (void) const;
  
  Aset_t<Aptree_t> &undone_tuplerefs (void) {return _undone_tuplerefs;}
  int inverse (void) const {return _inverse;}

  Alist_t<Abinop_t> Clones (Alognode_t *leftinput, 
			    Alognode_t *rightinput);
  Alogprop_t *MakeLogProps (Alognode_t *node);
  void Apply (Alognode_t *input); // redefined because we dont like the default
};

class Aunnestop_t : public Aunaryop_t, public Aop_operation_t {
private:
public:
  ~Aunnestop_t (void) {}
  Aunnestop_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate (void) const;

  Alist_t<Aunaryop_t> Clones (Alognode_t *input);
  Alogprop_t *MakeLogProps (Alognode_t *node);
};

class Aoutputop_t : public Aunaryop_t, public Aop_operation_t {
private:
public:
  ~Aoutputop_t (void) {}
  Aoutputop_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate (void) const;

  Alist_t<Aunaryop_t> Clones (Alognode_t *input);
  Alogprop_t *MakeLogProps (Alognode_t *node);
};
  
/***************************************************************************

  The algorithms
  Afilescan, Aindexscan, Aassembly, Afilter, Ahh_join, Ahhptr_join

***************************************************************************/

class Afilescan_t : public Aalgo_t {
private:
public:
  ~Afilescan_t (void) {}
  Afilescan_t (void);
  Aalgo_t *Duplicate (void) const;

  int Arity (void) const {return 0;}
  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  void MakePhyNodes (Alognode_t *node);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};

// even though index scan is a unary algorithm it
// ignores its input. this is because of the collapse thing.
class Aindexscan_t : public Aalgo_t {
private:
  Acat_t::idxid_t _idxid;
public:
  ~Aindexscan_t (void) {}
  Aindexscan_t (void);
  Aalgo_t *Duplicate (void) const;

  Acat_t::idxid_t idxid (void) const {return _idxid;}

  int Arity (void) const {return 0;}
  void MakePhyNodes (Alognode_t *node);
  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  Alist_t<Aalgo_t> Clones (Alognode_t *lognode, Aphynode_t **inputs = 0);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};

// assembly is a algorithm (for mat) as well as
// an enforcer (for filter, hh_join).
class Aassembly_t : public Aenforcer_t {
private:
  Aset_t<Aptree_t> _operations;	// operations being assembled...
  Aset_t<Aptree_t> _tuplerefs;	// tuplerefs being materialized...

public:
  ~Aassembly_t (void) {}
  Aassembly_t (void);
  Aalgo_t *Duplicate (void) const;

  Aset_t<Aptree_t> &operations (void) {return _operations;}
  Aset_t<Aptree_t> &tuplerefs (void) {return _tuplerefs;}

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  Alist_t<Aphynode_t> Enforce (Aphynode_t *, Aphyprop_t *);
  Alist_t<Aunaryalgo_t> Clones (Alognode_t *lognode, Aphynode_t *input);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};

class Afilter_t : public Aunaryalgo_t {
private:
public:
  ~Afilter_t (void) {}
  Afilter_t (Alist_t<Aenforcer_t> *enforcers);
  Aalgo_t *Duplicate (void) const;

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  int CanBeApplied (Alognode_t *node, Aphynode_t *input);
  Aphyprop_t *Constraint (Alognode_t *, Aphynode_t *, int inputnum = 0);
//  Alist_t<Aunaryalgo_t> Clones (Alognode_t *lognode, Aphynode_t *input);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};


class Ahh_join_t : public Abinalgo_t {
private:
public:
  ~Ahh_join_t (void) {}
  Ahh_join_t (Alist_t<Aenforcer_t> *enforcers);
  Aalgo_t *Duplicate (void) const;

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  int CanBeApplied (Alognode_t *node, Aphynode_t *leftinput,
		    Aphynode_t *rightinput);
  Aphyprop_t *Constraint (Alognode_t *, Aphynode_t *, int inputnum = 0);
//  Alist_t<Abinalgo_t> Clones (Alognode_t *node, 
//			      Aphynode_t *leftinput,
//			      Aphynode_t *rightinput);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};

// A pointer hash join is a way of implementing the join operator
class Ahhptr_join_t : public Abinalgo_t {
private:
public:
  ~Ahhptr_join_t (void) {}
  Ahhptr_join_t (Alist_t<Aenforcer_t> *enforcers);
  Aalgo_t *Duplicate (void) const;

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  int CanBeApplied (Alognode_t *node, Aphynode_t *leftinput,
		    Aphynode_t *rightinput);
  Aphyprop_t *Constraint (Alognode_t *, Aphynode_t *, int inputnum = 0);
  Alist_t<Abinalgo_t> Clones (Alognode_t *node, Aphynode_t *leftinput,
			      Aphynode_t *rightinput);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};

class Aunnestalgo_t : public Aunaryalgo_t {
private:
public:
  ~Aunnestalgo_t (void) {}
  Aunnestalgo_t (Alist_t<Aenforcer_t> *enforcers);
  Aalgo_t *Duplicate (void) const;

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  int CanBeApplied (Alognode_t *node, Aphynode_t *input);
  Aphyprop_t *Constraint (Alognode_t *, Aphynode_t *, int inputnum = 0);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};


class Aoutputalgo_t : public Aunaryalgo_t {
private:
public:
  ~Aoutputalgo_t (void) {}
  Aoutputalgo_t (Alist_t<Aenforcer_t> *enforcers);
  Aalgo_t *Duplicate (void) const;

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  int CanBeApplied (Alognode_t *node, Aphynode_t *input);
  Aphyprop_t *Constraint (Alognode_t *, Aphynode_t *, int inputnum = 0);
//  Alist_t<Aunaryalgo_t> Clones (Alognode_t *node, Aphynode_t *input);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};
/***************************************************************************

  inline functions

***************************************************************************/

inline Aget_t::Aget_t (Alist_t<Aalgo_t> algos) 
  : Aop_t ("get", Aget, algos)
{
  _relref = 0;
}

inline Aop_t *Aget_t::Duplicate (void) const 
{
  Aget_t *d = new Aget_t (GetListOfAlgorithms ()); 
  assert (d); *d = *this; return d;
}

inline Alogprop_t *Aget_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node); assert (l); return l;
}


inline Amat_t::Amat_t (Alist_t<Aalgo_t> algos) 
  : Aunaryop_t ("mat", Amat, algos)
{
}

inline Amat_t::Amat_t (char *newname, AopNumber n, Alist_t<Aalgo_t> l)
  : Aunaryop_t (newname, n, l)
{
}

inline Aop_t *Amat_t::Duplicate (void) const 
{
  Amat_t *d = new Amat_t (GetListOfAlgorithms ()); 
  assert (d); *d = *this; return d;
}

inline Alogprop_t *Amat_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node); assert (l); return l;
}

inline Aptree_t *Amat_t::tupleref (void)
{
//  assert (tuplerefs ().Cardinality () == 1);
  return tuplerefs ().FirstElement ();
}


inline Amat_collapse_t::Amat_collapse_t (Alist_t<Aalgo_t> algos) 
  : Amat_t ("mat_collapse", Amat_collapse, algos)
{
}

inline Aop_t *Amat_collapse_t::Duplicate (void) const 
{
  Amat_collapse_t *d = new Amat_collapse_t (GetListOfAlgorithms ()); 
  assert (d); *d = *this; return d;
}

inline Alogprop_t *Amat_collapse_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node); assert (l); return l;
}



inline Aselect_t::Aselect_t (Alist_t<Aalgo_t> algos) 
  : Aunaryop_t ("select", Aselect, algos)
{
  _multi_select = 0;
}

inline Aselect_t::Aselect_t (char *newname, AopNumber n, Alist_t<Aalgo_t> l)
  : Aunaryop_t (newname, n, l) {_multi_select = 0;}

inline Aop_t *Aselect_t::Duplicate (void) const 
{
  Aselect_t *d = new Aselect_t (GetListOfAlgorithms ()); 
  assert (d); *d = *this; return d;
}

inline Alogprop_t *Aselect_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node); assert (l); return l;
}



inline Aselect_collapse_t::Aselect_collapse_t (Alist_t<Aalgo_t> algos) 
  : Aselect_t ("select_collapse", Aselect_collapse, algos)
{
}

inline Aop_t *Aselect_collapse_t::Duplicate (void) const 
{
  Aselect_collapse_t *d = new Aselect_collapse_t (GetListOfAlgorithms ()); 
  assert (d); *d = *this; return d;
}

inline Alogprop_t *Aselect_collapse_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node); assert (l); return l;
}




inline Aidx_collapse_t::Aidx_collapse_t (Alist_t<Aalgo_t> algos) 
  : Aop_t ("idx_collapse", Aidx_collapse, algos)
{
}

inline Aop_t *Aidx_collapse_t::Duplicate (void) const 
{
  Aidx_collapse_t *d = new Aidx_collapse_t (GetListOfAlgorithms ()); 
  assert (d); *d = *this; return d;
}

inline Alogprop_t *Aidx_collapse_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node); assert (l); return l;
}


inline Ajoin_t::Ajoin_t (Alist_t<Aalgo_t> algos) 
  : Abinop_t ("join", Ajoin, algos)
{
  _inverse = 0;
}

inline Aop_t *Ajoin_t::Duplicate (void) const 
{
  Ajoin_t *d = new Ajoin_t (GetListOfAlgorithms ()); 
  assert (d); *d = *this; return d;
}

inline Alogprop_t *Ajoin_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node); assert (l); return l;
}


inline Aunnestop_t::Aunnestop_t (Alist_t<Aalgo_t> algos) 
  : Aunaryop_t ("unnestop", Aunnestop, algos)
{
}

inline Aop_t *Aunnestop_t::Duplicate (void) const 
{
  Aunnestop_t *d = new Aunnestop_t (GetListOfAlgorithms ()); 
  assert (d); *d = *this; return d;
}

inline Alogprop_t *Aunnestop_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node); assert (l); return l;
}



inline Aoutputop_t::Aoutputop_t (Alist_t<Aalgo_t> algos) 
  : Aunaryop_t ("outputop", Aoutputop, algos)
{
}

inline Aop_t *Aoutputop_t::Duplicate (void) const 
{
  Aoutputop_t *d = new Aoutputop_t (GetListOfAlgorithms ()); 
  assert (d); *d = *this; return d;
}

inline Alogprop_t *Aoutputop_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node); assert (l); return l;
}


inline Afilescan_t::Afilescan_t (void) : Aalgo_t ("filescan", Afilescan) {}

inline Aalgo_t *Afilescan_t::Duplicate (void) const
{
  Afilescan_t *h = new Afilescan_t; assert (h); *h = *this; return h;
}

inline Aphyprop_t *Afilescan_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}



inline Aassembly_t::Aassembly_t (void) 
  : Aenforcer_t ("assembly", Aassembly) {}

inline Aalgo_t *Aassembly_t::Duplicate (void) const
{
  Aassembly_t *h = new Aassembly_t; assert (h); *h = *this; return h;
}

inline Aphyprop_t *Aassembly_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}



inline Afilter_t::Afilter_t (Alist_t<Aenforcer_t> *enforcers) 
  : Aunaryalgo_t ("filter", Afilter, enforcers) {}

inline Aalgo_t *Afilter_t::Duplicate (void) const
{
  Afilter_t *h = new Afilter_t (enforcers); assert (h); *h = *this; return h;
}

inline Aphyprop_t *Afilter_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}



inline Aindexscan_t::Aindexscan_t (void) 
  : Aalgo_t ("indexscan", Aindexscan) {}

inline Aalgo_t *Aindexscan_t::Duplicate (void) const
{
  Aindexscan_t *h = new Aindexscan_t; assert (h); *h = *this; return h;
}

inline Aphyprop_t *Aindexscan_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}



inline Ahh_join_t::Ahh_join_t (Alist_t<Aenforcer_t> *enforcers) 
  : Abinalgo_t ("hh_join", Ahh_join, enforcers) {}

inline Aalgo_t *Ahh_join_t::Duplicate (void) const
{
  Ahh_join_t *h = new Ahh_join_t (enforcers); assert (h); *h = *this; return h;
}

inline Aphyprop_t *Ahh_join_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}



inline Ahhptr_join_t::Ahhptr_join_t (Alist_t<Aenforcer_t> *enforcers) 
  : Abinalgo_t ("hhptr_join", Ahhptr_join, enforcers) {}

inline Aalgo_t *Ahhptr_join_t::Duplicate (void) const
{
  Ahhptr_join_t *h = new Ahhptr_join_t (enforcers); assert (h); 
  *h = *this; return h;
}

inline Aphyprop_t *Ahhptr_join_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}



inline Aunnestalgo_t::Aunnestalgo_t (Alist_t<Aenforcer_t> *enforcers) 
  : Aunaryalgo_t ("unnestalgo", Aunnestalgo, enforcers) {}

inline Aalgo_t *Aunnestalgo_t::Duplicate (void) const
{
  Aunnestalgo_t *h = new Aunnestalgo_t (enforcers); assert (h); 
  *h = *this; return h;
}

inline Aphyprop_t *Aunnestalgo_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}



inline Aoutputalgo_t::Aoutputalgo_t (Alist_t<Aenforcer_t> *enforcers) 
  : Aunaryalgo_t ("outputalgo", Aoutputalgo, enforcers) {}

inline Aalgo_t *Aoutputalgo_t::Duplicate (void) const
{
  Aoutputalgo_t *h = new Aoutputalgo_t (enforcers); assert (h); 
  *h = *this; return h;
}

inline Aphyprop_t *Aoutputalgo_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}

#endif /* AOPALGOS_H */
