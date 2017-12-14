#ifndef AMISCOPS_H
#define AMISCOPS_H

// miscellaneous operators and algorithms.
// aggrop, outputop, sort.

#include <Aop.h>
#include <Aalgo.h>

#include <Aattr.h>

#include <Alogprop.h>
#include <Aphyprop.h>

// the aggregate and group by operators

class Aaggrop_t : public Aunaryop_t {
private:
public:
  ~Aaggrop_t (void) {}
  Aaggrop_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate (void) const;

  Alogprop_t *MakeLogProps (Alognode_t *node);
};

class Aaggralgo_t : public Aunaryalgo_t {
private:
public:
  ~Aaggralgo_t (void) {}
  Aaggralgo_t (Alist_t<Aenforcer_t> *enforcers);
  Aalgo_t *Duplicate (void) const;

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  Aphyprop_t *Constraint (Alognode_t *, Aphynode_t *, int inputnum = 0);
  Alist_t<Aunaryalgo_t> Clones (Alognode_t *, Aphynode_t *);
  int CanBeApplied (Alognode_t *, Aphynode_t *);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};



// the output operator

class Aoutputop_t : public Aunaryop_t {
private:
public:
  ~Aoutputop_t (void) {}
  Aoutputop_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate (void) const;

  Alogprop_t *MakeLogProps (Alognode_t *node);
};

class Aoutputalgo_t : public Aunaryalgo_t {
private:
public:
  ~Aoutputalgo_t (void) {}
  Aoutputalgo_t (Alist_t<Aenforcer_t> *enforcers);
  Aalgo_t *Duplicate (void) const;

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  Aphyprop_t *Constraint (Alognode_t *, Aphynode_t *, int inputnum = 0);
  int CanBeApplied (Alognode_t *, Aphynode_t *);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};

// the sort enforcer
class Asort_t : public Aenforcer_t {
private:
  Aattr_t *_sortorder;
public: 
  ~Asort_t (void) {}
  Asort_t (void);
  Aalgo_t *Duplicate (void) const;

  void set_sortorder (Aattr_t *s) {_sortorder = s;}
  Aattr_t *sortorder (void) {return _sortorder;}

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  Alist_t<Aphynode_t> Enforce (Aphynode_t *, Aphyprop_t *);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};


/***************************************************************************

  inline functions

***************************************************************************/

inline Aaggrop_t::Aaggrop_t (Alist_t<Aalgo_t> algos) 
  : Aunaryop_t ("aggrop", Aaggrop, algos) {}

inline Aop_t *Aaggrop_t::Duplicate (void) const
{
  Aaggrop_t *a = new Aaggrop_t (GetListOfAlgorithms ()); 
  assert (a); *a = *this; return a;
}

inline Alogprop_t *Aaggrop_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node); assert (l); return l;
}

inline Aaggralgo_t::Aaggralgo_t (Alist_t<Aenforcer_t> *enforcers)
  : Aunaryalgo_t ("aggralgo", Aaggralgo, enforcers) {}

inline Aalgo_t *Aaggralgo_t::Duplicate (void) const
{
  Aaggralgo_t *a = new Aaggralgo_t (enforcers); 
  assert (a); *a = *this; return a;
}

inline Aphyprop_t *Aaggralgo_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}

inline Aoutputop_t::Aoutputop_t (Alist_t<Aalgo_t> algos)
  : Aunaryop_t ("outputop", Aoutputop, algos) {}

inline Aop_t *Aoutputop_t::Duplicate (void) const
{
  Aoutputop_t *a = new Aoutputop_t (GetListOfAlgorithms ()); 
  assert (a); *a = *this; return a;
}

inline Alogprop_t *Aoutputop_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node); assert (l); return l;
}

inline Aoutputalgo_t::Aoutputalgo_t (Alist_t<Aenforcer_t> *enforcers)
  : Aunaryalgo_t ("outputalgo", Aoutputalgo, enforcers) {}

inline Aalgo_t *Aoutputalgo_t::Duplicate (void) const
{
  Aoutputalgo_t *a = new Aoutputalgo_t (enforcers); 
  assert (a); *a = *this; return a;
}

inline Aphyprop_t *Aoutputalgo_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}

inline Asort_t::Asort_t (void) : Aenforcer_t ("sort", Asort) 
{
  _sortorder = 0;
}

inline Aalgo_t *Asort_t::Duplicate (void) const
{
  Asort_t *a = new Asort_t; assert (a); *a = *this; return a;
}

inline Aphyprop_t *Asort_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}

#endif /* AMISCOPS_H */
