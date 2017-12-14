#ifndef AJOIN_H
#define AJOIN_H

#include <Aop.h>
#include <Aalgo.h>

#include <Arel.h>
#include <Apred.h>
#include <Aset.h>

#include <Alogprop.h>
#include <Aphyprop.h>

class Ajoin_t : public Abinop_t {
private:
  Aset_t<Apred_t> _preds;
  Aset_t<Aattr_t> _equijoin_attrs;

  void find_applicable_preds (Alogprop_t *, Alogprop_t *);

public:
  ~Ajoin_t (void) {}
  Ajoin_t (Alist_t<Aalgo_t> algos);
  Aop_t *Duplicate (void) const;
  
  Aset_t<Aattr_t> &equijoin_attrs (void) {return _equijoin_attrs;}
  Aset_t<Apred_t> &preds (void) {return _preds;}

  Alist_t<Abinop_t> Clones (Alognode_t *leftinput, 
			    Alognode_t *rightinput);
  Alogprop_t *MakeLogProps (Alognode_t *node);
  void Apply (Alognode_t *input); // redefined because we dont like the default
//void find_applicable_selects (Alogprop_t *leftprops, Alogprop_t *rightprops);
};

class Aloopsjoin_t : public Abinalgo_t {
private:
public:
  ~Aloopsjoin_t (void) {}
  Aloopsjoin_t (void);
  Aalgo_t *Duplicate (void) const;

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  Alist_t<Abinalgo_t> Clones (Alognode_t *node, 
			      Aphynode_t *leftinput,
			      Aphynode_t *rightinput);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};

class Anlidxjoin_t : public Abinalgo_t {
private:
  Apred_t *_pred;

public:
  ~Anlidxjoin_t (void) {}
  Anlidxjoin_t (void);
  Aalgo_t *Duplicate (void) const;

  Apred_t *pred (void) const {return _pred;}

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  Alist_t<Abinalgo_t> Clones (Alognode_t *node, 
			      Aphynode_t *leftinput,
			      Aphynode_t *rightinput);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};

class Amergejoin_t : public Abinalgo_t {
private:
  Apred_t *_pred;		// the pred on which to merge join.
public:
  ~Amergejoin_t (void) {}
  Amergejoin_t (Alist_t<Aenforcer_t> *enforcers);
  Aalgo_t *Duplicate (void) const;
  Apred_t *pred (void) const {return _pred;}

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  Aphyprop_t *Constraint (Alognode_t *, Aphynode_t *, int inputnum = 0);
  Alist_t<Abinalgo_t> Clones (Alognode_t *node, 
			      Aphynode_t *leftinput,
			      Aphynode_t *rightinput);
  int CanBeApplied (Alognode_t *node, Aphynode_t *leftinput, 
		    Aphynode_t *rightinput);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};

class Ahashjoin_t : public Abinalgo_t {
private:
  Apred_t *_pred;		// the pred on which to hash join
public:
  ~Ahashjoin_t (void) {}
  Ahashjoin_t (void);
  Aalgo_t *Duplicate (void) const;
  Apred_t *pred (void) const {return _pred;}

  Aphyprop_t *MakePhyProps (Aphynode_t *node);
  Alist_t<Abinalgo_t> Clones (Alognode_t *node, 
			      Aphynode_t *leftinput,
			      Aphynode_t *rightinput);
  void Execute (Bexec_info_t &exec_info); 
				// defined in Bexecution.c
};
  
/***************************************************************************

  inline functions

***************************************************************************/

inline Ajoin_t::Ajoin_t (Alist_t<Aalgo_t> algos) 
  : Abinop_t ("join", Ajoin, algos)
{}

inline Aop_t *Ajoin_t::Duplicate (void) const
{
  Ajoin_t *j = new Ajoin_t (GetListOfAlgorithms ()); 
  assert (j); *j = *this; return j;
}

inline Alogprop_t *Ajoin_t::MakeLogProps (Alognode_t *node)
{
  Alogprop_t *l = new Alogprop_t (this, node); assert (l); return l;
}

inline Aloopsjoin_t::Aloopsjoin_t (void)
  : Abinalgo_t ("loopsjoin", Aloopsjoin) {}

inline Aalgo_t *Aloopsjoin_t::Duplicate (void) const
{
  Aloopsjoin_t *l = new Aloopsjoin_t; assert (l); *l = *this; return l;
}

inline Aphyprop_t *Aloopsjoin_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}


inline Anlidxjoin_t::Anlidxjoin_t (void)
  : Abinalgo_t ("nlidxjoin", Anlidxjoin) 
{
  _pred = 0;
}

inline Aalgo_t *Anlidxjoin_t::Duplicate (void) const
{
  Anlidxjoin_t *i = new Anlidxjoin_t; assert (i); *i = *this; return i;
}

inline Aphyprop_t *Anlidxjoin_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}

inline Amergejoin_t::Amergejoin_t (Alist_t<Aenforcer_t> *enforcers)
  : Abinalgo_t ("mergejoin", Amergejoin, enforcers) {}

inline Aalgo_t *Amergejoin_t::Duplicate (void) const
{
  Amergejoin_t *m = new Amergejoin_t (enforcers); 
  assert (m); *m = *this; return m;
}

inline Aphyprop_t *Amergejoin_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}


inline Ahashjoin_t::Ahashjoin_t (void) 
  : Abinalgo_t ("hashjoin", Ahashjoin) {}

inline Aalgo_t *Ahashjoin_t::Duplicate (void) const
{
  Ahashjoin_t *h = new Ahashjoin_t; assert (h); *h = *this; return h;
}

inline Aphyprop_t *Ahashjoin_t::MakePhyProps (Aphynode_t *node)
{
  Aphyprop_t *p = new Aphyprop_t (this, node); assert (p); return p;
}
#endif /* AJOIN_H */
