#ifndef ASELECT_H
#define ASELECT_H

#include <Aop.h>
#include <Aalgo.h>

#include <Arel.h>
#include <Apred.h>
#include <Aset.h>

#include <Alogprop.h>
#include <Aphyprop.h>

class Aselect_t : public Aunaryop_t {
private:
  Arel_t *_rel;
  Aset_t<Apred_t> _preds;

public:
  ~Aselect_t (void) {}
  Aselect_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate () const;

  Arel_t *rel (void) const {return _rel;}
  Aset_t<Apred_t> &preds (void) {return _preds;}

  Alist_t<Aunaryop_t> Clones (Alognode_t *input);
  Alogprop_t *MakeLogProps (Alognode_t *node);
};

class Afilescan_t : public Aunaryalgo_t {
private:
public:
  ~Afilescan_t (void) {}
  Afilescan_t (void);
  Aalgo_t *Duplicate (void) const;

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  Alist_t<Aunaryalgo_t> Clones (Alognode_t *lognode, Aphynode_t *input);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};

class Aindexscan_t : public Aunaryalgo_t {
private:
  Apred_t *_indexpred;

public:
  ~Aindexscan_t (void) {}
  Aindexscan_t (void);
  Aalgo_t *Duplicate (void) const;

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  Alist_t<Aunaryalgo_t> Clones (Alognode_t *lognode, Aphynode_t *input);
  Apred_t *indexpred (void) const {return _indexpred;}
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};

/***************************************************************************

  inline functions

***************************************************************************/

inline Aselect_t::Aselect_t (Alist_t<Aalgo_t> algos)
  : Aunaryop_t ("select", Aselect, algos) {}

inline Aop_t *Aselect_t::Duplicate (void) const
{
  Aselect_t *s = new Aselect_t (GetListOfAlgorithms ()); 
  assert (s); *s = *this; return s;
}

inline Alogprop_t *Aselect_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node);
  assert (l); return l;
}

inline Afilescan_t::Afilescan_t (void)
  : Aunaryalgo_t ("filescan", Afilescan) {}

inline Aalgo_t *Afilescan_t::Duplicate (void) const
{
  Afilescan_t *f = new Afilescan_t; assert (f); return f;
}

inline Aphyprop_t *Afilescan_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}

inline Aindexscan_t::Aindexscan_t (void) 
  : Aunaryalgo_t ("indexscan", Aindexscan) 
{
}

inline Aalgo_t *Aindexscan_t::Duplicate (void) const
{
  Aindexscan_t *i = new Aindexscan_t; assert (i); return i;
}

inline Aphyprop_t *Aindexscan_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}

#endif /* ASELECT_H */
