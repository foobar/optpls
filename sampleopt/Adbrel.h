#ifndef ADBREL_H
#define ADBREL_H

#include <Aop.h>
#include <Aalgo.h>

#include <Arel.h>

#include <Alogprop.h>
#include <Aphyprop.h>

class Adbrel_t : public Aop_t {
private:
  Arel_t *_rel;

public:
  ~Adbrel_t (void) {}
  Adbrel_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate (void) const;

  int Arity (void) const {return 0;}
  void Apply (Alognode_t *node = 0);
  Alogprop_t *MakeLogProps (Alognode_t *node);
  Alist_t<Aop_t> Clones (Alognode_t **inputs = 0);

  Arel_t *rel (void) const {return _rel;}
};

class Aheapfile_t : public Aalgo_t {
private:
public:
  ~Aheapfile_t (void) {}
  Aheapfile_t (void);
  Aalgo_t *Duplicate (void) const;

  int Arity (void) const {return 0;}
  void MakePhyNodes (Alognode_t *node);
  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};

class Aindex_t : public Aalgo_t {
private:
  Aattr_t *_indexattr;

public:
  ~Aindex_t (void) {}
  Aindex_t (void);
  Aalgo_t *Duplicate (void) const;

  void set_indexattr (Aattr_t *a) {_indexattr = a;}
  Aattr_t *indexattr (void) {return _indexattr;}
  
  int Arity (void) const {return 0;}
  void MakePhyNodes (Alognode_t *node);
  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  Alist_t<Aalgo_t> Clones (Alognode_t *node, Aphynode_t **inputs = 0);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};

/***************************************************************************

  inline functions

***************************************************************************/

inline Adbrel_t::Adbrel_t (Alist_t<Aalgo_t> algos) 
  : Aop_t ("dbrel", Adbrel, algos)
{
  _rel = 0;
}


inline Aop_t *Adbrel_t::Duplicate (void) const 
{
  Adbrel_t *d = new Adbrel_t (GetListOfAlgorithms ()); 
  assert (d); *d = *this; return d;
}

inline Alogprop_t *Adbrel_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node); assert (l); return l;
}

inline Aheapfile_t::Aheapfile_t (void) : Aalgo_t ("heapfile", Aheapfile) {}

inline Aalgo_t *Aheapfile_t::Duplicate (void) const
{
  Aheapfile_t *h = new Aheapfile_t; assert (h); return h;
}

inline Aphyprop_t *Aheapfile_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}

inline Aindex_t::Aindex_t (void) : Aalgo_t ("index", Aindex) 
{
  _indexattr = 0; 
}

inline Aalgo_t *Aindex_t::Duplicate (void) const
{
  Aindex_t *i = new Aindex_t; assert (i); i->_indexattr = _indexattr; return i;
}

inline Aphyprop_t *Aindex_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}

#endif /* ADBREL_H */
