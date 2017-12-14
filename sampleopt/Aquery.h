#ifndef AQUERY_H
#define AQUERY_H

#include <iostream.h>

#include <Aset.h>
#include <Alist.h>
#include <Aattr.h>
#include <Arel.h>
#include <Apred.h>

class Bastnode_t;
class Bquery_stmt_op_t;

class Aquery_t {
  friend class Bquery_stmt_op_t;
  friend class Brqg_t;
private:
  // these two classes Aquery_t::rel_t and Aquery_t::attr_t are
  // required to store information about attr's and rel's in this
  // particular query. (it is not easy to store this information
  // in the Arel_t and Aattr_t classes.)
  class rel_t {
  public:
    Arel_t rel;		// relation which is being represented
    Aset_t<Apred_t> *selects;	// selects on this relation only.
				// can be done during a filescan.
    Apred_t *oid_ref;		// an oid reference for this relation
				// if any. there can be only one...
    Aset_t<Arel_t> *conn_rels;	// says which relations are connected to this
				// relation through predicate... this
				// info is used to avoid cartesian products

    rel_t (void) {oid_ref = 0;}
    ~rel_t (void) {delete selects; delete conn_rels;}
#ifdef A_ZIPPY    
    rel_t (const rel_t &r) {this->rel = r.rel; selects = r.selects;
			    oid_ref = r.oid_ref; conn_rels = r.conn_rels;}
#endif /* A_ZIPPY */
  };
  class attr_t {
  public:
    Aattr_t attr;		// attr which is being represented
    Aset_t<Apred_t> *indexselects; 
				// selects on this attribute which can
				// be used in an index scan.
    Aset_t<Aattr_t> *eqattrs;	// set of attr's equivalent to this one.
    attr_t (void) {indexselects = 0; eqattrs = 0;}
    ~attr_t (void) {delete indexselects; delete eqattrs;}
#ifdef A_ZIPPY 
    attr_t (const attr_t &a) {this->attr = a.attr; eqattrs = a.eqattrs;
			      indexselects = a.indexselects;}
#endif /* A_ZIPPY */
  };

private:
  Aset_t<Aptree_t> *_relop_ops;	// set of relop operations
  Aset_t<Aptree_t> *_join_ops;	// set of (maybe) join operations
  Aset_t<Aptree_t> *_attref_ops; 
				// set of attref operations
  Aset_t<Aptree_t> *_relref_ops; 
				// set of relref operations
  Aset_t<Aptree_t> *_tupleref_ops;	
				// set of tupleref operations
  Aset_t<Aptree_t> *_tuplevar_ops;	
				// set of tuplevar operations
  Aset_t<Aptree_t> *_mat_ops;	// set of operations which can be applied
				// by a mat or a get operator.
				// essentially attref + tupleref.

  int _numrels;			// number of relations 
  rel_t *_rels;			// array of relations
  int _numattrs;		// number of attributes
  attr_t *_attrs;		// array of attributes
  int _numpreds;		// number of predicates
  Apred_t *_preds;		// array of predicates
  int _numoperations;		// number of operations
  Aptree_t *_operations;	// array of operations
  Aset_t<Aptree_t> *_op_tuprefs;// set of tuplerefs associated with
				// any given operation.
  Aset_t<Aptree_t> *_op_mats;	// set of mat_ops associated with
				// any given operation.
  Aattr_t *_groupby;		// attribute in group by clause
  Aattr_t *_orderby;		// attribute in order by clause
  int _ascending;		// true if order by is ascending
  
  Aset_t<Aattr_t> *_target_attrs;
				// attributes which figure in the target list
  Aset_t<Aptree_t> *_target_ops;// operations which figure in the target list
  Alist_t<Aptree_t> _target_list;
				// list of items in target list
  int _is_select_star;		// true if query is a "select *" query
  int _has_aggregates;		// true if query has aggregates in targetlist

private:
  void compute_operations_array (void);
  void compute_eqattrs (void);
  void compute_conn_rels (void);
  void compute_rel_selects (void);
  void compute_indexselects (void);
  void compute_target_attrs (void);

public:
  Aquery_t (void);
  ~Aquery_t (void);

  void init (void);

  int numrels (void) const {return _numrels;}
  Arel_t *rel (int N);
  Arel_t *rel (const char *name) const;
  const Aset_t<Arel_t> &conn_rels (const Arel_t *rel) const;
  Apred_t *oid_ref (const Arel_t *r) const;
  const Aset_t<Apred_t> &relselects (const Arel_t *r) const; 

  int numattrs (void) const {return _numattrs;}
  Aattr_t *attr (int N) const;
  Aattr_t *attr (Arel_t *relptr, const char *attrname) const;
  Aattr_t *attr (const char *relname, const char *attrname) const;
  const Aset_t<Aattr_t> &get_eqattrs (const Aattr_t *a) const;
  int are_attrs_equivalent (const Aattr_t *a1, const Aattr_t *a2) const;

  int numpreds (void) const {return _numpreds;}
  Apred_t *pred (int N) const {return &_preds[N];}

  int numoperations (void) const {return _numoperations;}
  Aptree_t *operation (int N) const {return &_operations[N];}

  const Aset_t<Aptree_t> &relop_ops (void) const {return *_relop_ops;}
  const Aset_t<Aptree_t> &join_ops (void) const {return *_join_ops;}
  const Aset_t<Aptree_t> &relref_ops (void) const {return *_relref_ops;}
  const Aset_t<Aptree_t> &tuplevar_ops (void) const {return *_tuplevar_ops;}
  const Aset_t<Aptree_t> &tupleref_ops (void) const {return *_tupleref_ops;}
  const Aset_t<Aptree_t> &attref_ops (void) const {return *_attref_ops;}
  const Aset_t<Aptree_t> &mat_ops (void) const {return *_mat_ops;}

  const Aset_t<Aptree_t> &op_tuprefs (Aptree_t *oprn) const;
  const Aset_t<Aptree_t> &op_mats (Aptree_t *oprn) const;

  Aattr_t *groupby (void) const {return _groupby;}
  Aattr_t *orderby (void) const {return _orderby;}

  int has_empty_where_clause (void) const {return _numpreds == 0;}
  int has_aggr_in_target_list (void) const {return _has_aggregates;}
  Alist_t<Aptree_t> target_list (void) const {return _target_list;}
  Aset_t<Aptree_t> target_ops (void) const {return *_target_ops;}
  const Aset_t<Aattr_t> &target_attrs (void) const {return *_target_attrs;}
  int is_select_star (void) const {return _is_select_star;}
};

/***************************************************************************

  inline functions

***************************************************************************/

inline Arel_t *Aquery_t::rel (int N) 
{
  return &_rels[N].rel;
}

inline Apred_t *Aquery_t::oid_ref (const Arel_t *r) const
{
  return _rels[r->Number ()].oid_ref;
}


inline const Aset_t<Apred_t> &Aquery_t::relselects (const Arel_t *r) const
{
  return *_rels[r->Number ()].selects;
}

inline Aattr_t *Aquery_t::attr (int N) const
{
  return &_attrs[N].attr;
}


inline const Aset_t<Aattr_t> &Aquery_t::get_eqattrs (const Aattr_t *a) const
{
  return *_attrs[a->Number ()].eqattrs;
}

inline int Aquery_t::are_attrs_equivalent (const Aattr_t *a1, 
					   const Aattr_t *a2) const
{
  if (!(a1 && a2)) return 0;	// if either of them is NULL, return FALSE.
  else return get_eqattrs (a1).HasElement (a2);
}

inline const Aset_t<Arel_t> &Aquery_t::conn_rels (const Arel_t *r) const
{
  return *_rels[r->Number ()].conn_rels;
}

inline const Aset_t<Aptree_t> &Aquery_t::op_tuprefs (Aptree_t *oprn) const
{
  return _op_tuprefs[oprn->Number ()];
}


inline const Aset_t<Aptree_t> &Aquery_t::op_mats (Aptree_t *oprn) const
{
  return _op_mats[oprn->Number ()];
}

#endif /* AQUERY_H */
