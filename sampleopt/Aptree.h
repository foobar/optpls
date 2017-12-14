#ifndef APTREE_H
#define APTREE_H

#include <iostream.h>
// #include <stdarg.h>
#include <Alist.h>
#include <Aset.h>
#include <Arel.h>
#include <Aattr.h>
#include <Afunc.h>

class Alognode_t;
class Bexec_info_t;		

// a predicate tree (Aptree_t) is a parse tree representing
// a predicate or a part of it.
// a ptree is represented as a function (Afunc_t)
// being applied to a number of arguments. 
// each argument is in turn a ptree.
// leaf nodes of the ptree are ptrees whose func's take
// zero arguments.

class Aptree_t {
  friend class Afunc_t;
  friend class Arelattr_t;
  friend class Aattref_t;

private:
  Afunc_t *_func;		// function being applied at this point.
  int _nargs;			// number of arguments
  Aptree_t *_args;		// array of arguments
  Aptree_t *_parent;

  int _number;			// index into array of ptree's (operations)
				// in Aquery_t

  void new_parent (Aptree_t *p) {_parent = p;}
  void new_arg (int n, Aptree_t a) {_args[n] = a; a.new_parent (this);}
  void insert_unique_operation (Alist_t<Aptree_t> &op_list);
  static Aptree_t make_null_pred_tree (void);
  static Aptree_t _null_pred_tree;

public:
  Aptree_t (void);
  void delete_tree (void);
  void init (Afunc_t *f, int n = 0, const Aptree_t *a = 0);

//  void init (Afunc_t *f, int n, va_list arglist);
//  Aptree_t (Afunc_t *f, int n, ...);
//  void init (Afunc_t *f, int n, ...);
				// would love to have this, but cannot make
				// it work with g++

  Afunc_t *func (void) const {return _func;}
  int nargs (void) const {return _nargs;}
  Aptree_t *arg (int n) const {return &_args[n];}

  // utility functions
  void replace_relattrs (void) {_func->replace_relattrs (*this);}
				// this routine replaces all occurrences
				// of "attribute_name" or
				// "rel_name.attribute_name" by reference
				// to the proper Aattr_t objects.
  int is_equijoin (Aattr_t *&a1, Aattr_t *&a2) const;
				// returns true if this is an equijoin
				// predicate. a1 and a2 are set to the
				// left and right predicate resp.
  void update_sets (Aset_t<Arel_t> &rels,
			    Aset_t<Aattr_t> &attrs) const;
				// to "rels" and "attrs" add those
				// relations and attributes which are
				// referenced in this ptree
  void generate_name (char *name, int num) const;

  int is_literal (void) const {return _func->is_literal ();}
  int is_attref (void) const {return _func->is_attref ();}
  int is_relref (void) const {return _func->is_relref ();}
  int is_tupleref (void) const {return _func->is_tupleref ();}
  int is_relop (void) const {return _func->is_relop ();}
  Aseldesc_t seldesc (const Alognode_t *lognode) const;
  double selectivity (const Alognode_t *lognode) const;
  Aptree_t copy_tree (void) const;

  void replace_eqattrs (const Aset_t<Aattr_t> &attrs) const;
  void put_suffixes (const Bexec_info_t &exec_info);

  void read (istream &is);
  void write (ostream &os) const;


  // functions for the Aset_t<Aptree_t> class
  static int TotalNumber (void);
  static Aptree_t *NthElement (int N);
  int Number (void) const {return _number;}

  void make_operations_list (Alist_t<Aptree_t> &op_list);
  int is_equal_to (const Aptree_t &ptree) const;
  static Aptree_t &null_pred_tree (void) {return _null_pred_tree;}
  static void init_operations_set (Aset_t<Aptree_t> &opset);

};  


/***************************************************************************

  inline functions

***************************************************************************/
inline Aptree_t::Aptree_t (void)
{
  _func = 0; _args = _parent = 0; _nargs = 0; _number = -1;
}

inline void Aptree_t::update_sets (Aset_t<Arel_t> &rels, 
				   Aset_t<Aattr_t> &attrs) const
{
  _func->update_sets (*this, rels, attrs);
}

inline Aseldesc_t Aptree_t::seldesc (const Alognode_t *lognode) const
{
  return _func->seldesc (*this, lognode);
}

inline double Aptree_t::selectivity (const Alognode_t *lognode) const
{
  assert (is_relop () || is_relref ());
  Aseldesc_t seldesc_val = seldesc (lognode);
  return seldesc_val.selectivity;
}

inline Aptree_t Aptree_t::copy_tree (void) const
{
  return _func->copy_tree (*this);
}

inline void Aptree_t::replace_eqattrs (const Aset_t<Aattr_t> &attrs) const
{
  _func->replace_eqattrs (*this, attrs);
}

inline void Aptree_t::put_suffixes (const Bexec_info_t &exec_info)
{
  _func->put_suffixes (*this, exec_info);
}

inline void Aptree_t::generate_name (char *name, int num) const 
{
  _func->generate_name (*this, name, num);
}

inline Aptree_t Aptree_t::make_null_pred_tree (void)
{
  Aptree_t null_tree;
  Afunc_t *null_func = new Afunc_t (Afunc_t::empty);
  assert (null_func);
  null_tree.init (null_func);
  null_tree._number = 0;
  return null_tree;
}

inline void Aptree_t::init_operations_set (Aset_t<Aptree_t> &opset)
{
  opset.Insert (&null_pred_tree ());
}

inline ostream &operator<< (ostream &os, const Aptree_t &ptree)
{
  ptree.write (os); return os;
}

inline istream &operator>> (istream &is, Aptree_t &ptree)
{
  ptree.read (is); return is;
}

#endif /* APTREE_H */
