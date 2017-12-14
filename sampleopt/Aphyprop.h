#ifndef APHYPROP_H
#define APHYPROP_H

#include <Aopdefs.h>
#include <Aattr.h>
#include <Acost.h>
#include <Aptree.h>

class Alogprop_t;
class Aphynode_t;

class Aphyprop_t {
private:
  Acost_t _cost;
  Aset_t<Aptree_t> _ops_not_in_memory;
				// operations which the Alogprops thinks
				// are completed but which are not in
				// memory (due to idx_collapse)
				// these will have to be brought in by
				// the assembly enforcer.
  long int _inmem_ass_obj_size;	// size of assembled obj in memory

public:
  ~Aphyprop_t (void) {}
  Aphyprop_t (Aphyprop_t *other) {*this = *other;}

  Aphyprop_t (Afilescan_t *, Aphynode_t *);
  Aphyprop_t (Aindexscan_t *, Aphynode_t *);
  Aphyprop_t (Aassembly_t *, Aphynode_t *);
  Aphyprop_t (Afilter_t *, Aphynode_t *);
  Aphyprop_t (Ahh_join_t *, Aphynode_t *);
  Aphyprop_t (Ahhptr_join_t *, Aphynode_t *);
  Aphyprop_t (Aunnestalgo_t *, Aphynode_t *);
  Aphyprop_t (Aoutputalgo_t *, Aphynode_t *);

  Aset_t<Aptree_t> &ops_not_in_memory (void) {return _ops_not_in_memory;}
  long int inmem_ass_obj_size (void) const {return _inmem_ass_obj_size;}
  void need_inmem (Aset_t<Aptree_t> &need_inmem);
  Acost_t GetCost () const {return _cost;}
  int IsEqualTo (const Aphyprop_t *other) const;
  int Hash (const Alogprop_t *logprop) const;
  int IsInteresting (const Alogprop_t *logprop) const;
};

/***************************************************************************

  inline functions

***************************************************************************/
inline void Aphyprop_t::need_inmem (Aset_t<Aptree_t> &need_inmem)
{
  _ops_not_in_memory.Minus (need_inmem);
}

#endif /* APHYPROP_H */
