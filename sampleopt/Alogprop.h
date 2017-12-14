#ifndef LOGPROP_H
#define LOGPROP_H

#include <Aset.h>
#include <Arel.h>
#include <Aattr.h>
#include <Aptree.h>
#include <Apred.h>
#include <Aopdefs.h>

class Alognode_t;

class Alogprop_t {
 private:
  Aset_t<Aptree_t> _operations;	// operations applied so far
  Aset_t<Aptree_t> _undone_tuplerefs;
				// keeps track of any pointer join
				// predicates which should have been
				// applied but haven't yet been...
  Aset_t<Aptree_t> _need_unnesting;
				// keeps track of which attributes
				// are set valued and havent yet been unnested.

  double _numtuples;		// number of tuples

  int outputop_applied;		// true if output operator has been applied.
				// these two variables are used in the
				// IsEqualTo function. they are very important
  char *_index_path;		// if this tree is part of a
				// get-(mat)*-select combination which can
				// be used for an indexscan then this variable
				// holds the pathname materialized so far...
  int _is_interesting;		// is this an interesting lognode?

  Alogprop_t (void) {}

 public:
  ~Alogprop_t (void) {delete _index_path;}
  Alogprop_t (Aget_t *, Alognode_t *);
  Alogprop_t (Amat_t *, Alognode_t *);
  Alogprop_t (Amat_collapse_t *, Alognode_t *);
  Alogprop_t (Aselect_t *, Alognode_t *);
  Alogprop_t (Aselect_collapse_t *, Alognode_t *);
  Alogprop_t (Aidx_collapse_t *, Alognode_t *);
  Alogprop_t (Ajoin_t *, Alognode_t *);
  Alogprop_t (Aunnestop_t *, Alognode_t *);
  Alogprop_t (Aoutputop_t *, Alognode_t *);

  Alogprop_t *Duplicate (void) const;
  
  Aset_t<Aptree_t> &operations (void) {return _operations;}
  const Aset_t<Aptree_t> &undone_tuplerefs (void) const;
  const Aset_t<Aptree_t> &need_unnesting (void) const;

  double numtuples (void) const {return _numtuples;}
  const char *index_path (void) const {return _index_path;}

  int IsEqualTo (const Alogprop_t *other) const;
  int IsInteresting (void) const;
  int NumOperations (void) const;
  int IsCompleteQuery (void);
  int Hash (void) const;
};


/***************************************************************************

  inline functions

***************************************************************************/
#include <Aoptions.h>

inline const Aset_t<Aptree_t> &Alogprop_t::undone_tuplerefs (void) const 
{
  return _undone_tuplerefs;
}


inline const Aset_t<Aptree_t> &Alogprop_t::need_unnesting (void) const 
{
  return _need_unnesting;
}

inline int Alogprop_t::IsCompleteQuery (void) 
{
  return _operations.IsFull () && _need_unnesting.IsEmpty ();
}

inline int Alogprop_t::NumOperations (void) const {
  return Aglob_vars()->oopt->dont_split_lists ? 0 : _operations.Cardinality ();
}

#endif /* LOGPROP_H */
