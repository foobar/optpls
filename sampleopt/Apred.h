#ifndef APRED_H
#define APRED_H

#include <Aset.h>
#include <Aptree.h>
#include <Arel.h>
#include <Aattr.h>

class Alognode_t;
class Bexec_info_t;

class Apred_t {
public:
  enum type_t {singleattr, singlerel, equijoin, oidref, general, bogus};
private:
  Aptree_t _ptree;
  type_t _type;
  int _number;			// index into array of pred's in Aquery_t
  Aset_t<Arel_t> *_rels;	// relations referenced in this predicate
  Aset_t<Aattr_t> *_attrs;	// attributes referenced in this predicate
  Aset_t<Aattr_t> *_eqattrs;	// eq-closure of above set of attributes

  // these members are required for the index_joinable function.
  Aset_t<Aattr_t> *leftattrs;
  Aset_t<Aattr_t> *rightattrs;

public:
  Apred_t (void);
  ~Apred_t (void);
  void init (int num, Aptree_t &p);

  type_t type (void) const {return _type;}
  const Aptree_t &ptree (void) const {return _ptree;}
  const Aset_t<Arel_t> &rels (void) {return *_rels;}
  const Aset_t<Aattr_t> &attrs (void) {return *_attrs;}
  const Aset_t<Aattr_t> &eqattrs (void) {return *_eqattrs;}
  int is_equal_to (const Apred_t &other) const;

  void replace_relattrs (void) {_ptree.replace_relattrs ();}
  void update_sets (void);
  Aptree_t rewrite (const Bexec_info_t &exec_info) const;
				// defined in bogus/Brewrite.c
  void make_operations_list (Alist_t<Aptree_t> &oplist);
  
  int is_equijoin (Aattr_t *&a1, Aattr_t *&a2) const;
  int index_scannable (const Aattr_t *a) const; 
				// returns true if this pred can
				// be used to scan an index on "a".
  int index_joinable (const Aattr_t *a, 
		      const Aset_t<Aattr_t> &outer_eqattrs) const;
				// returns true if this pred can be
				// used in an index-join

  double selectivity (const Alognode_t *lognode) const;

  // functions for the Aset_t<Apred_t> class
  static int TotalNumber (void);
  static Apred_t *NthElement (int N);
  int Number (void) const {return _number;}
};

Aset_t<Aattr_t> Aget_eq_closure (Aset_t<Apred_t> &predset);
                                // this function returns the union of all
				// _eqattrs of each pred in predset.

class Alognode_t;
double Aselectivity (Aset_t<Apred_t> &predset, Alognode_t *node);
				// finds the selectivity of a set of
				// select predicates...
				// this function defined in Aselectivity.c


/***************************************************************************

  inline functions

***************************************************************************/
inline Apred_t::Apred_t (void) 
{
  _rels = 0; _attrs = _eqattrs = leftattrs = rightattrs = 0;
}

inline Apred_t::~Apred_t (void) 
{
  delete _rels; delete _attrs; delete _eqattrs;
  delete leftattrs; delete rightattrs;
}

inline int Apred_t::is_equal_to (const Apred_t &other) const
{
  return _number == other._number;
}

inline double Apred_t::selectivity (const Alognode_t *lognode) const
{
  return _ptree.seldesc (lognode).selectivity;
}

inline int Apred_t::is_equijoin (Aattr_t *&a1, Aattr_t *&a2) const
{
  return _ptree.is_equijoin (a1, a2);
}

inline void Apred_t::make_operations_list (Alist_t<Aptree_t> &oplist)
{
  _ptree.make_operations_list (oplist);
}

#endif /* APRED_H */
