#ifndef AREL_H
#define AREL_H

#include <string.h>
#include <assert.h>

#include <Aconsts.h>
#include <Acat.h>
#include <Aglob_vars.h>
#include <Aattr.h>
#include <Aset.h>


class Arel_t {
  friend class Acat_t;		
private:
  int _number;			// index into array of relations in Aquery_t.
  Acat_t::relid_t _relid;	// id of the extent.
  Acat_t::attr_type_t _otypeid;	// type of objects in this extent.
  char *_rel_var;		// name of the relation variable (if any)
				// associated with this (in the query).
				// example: select * from Emp E.
				//    here Emp is name of the relation
				//    and E is the relation variable.
  Aset_t<Aattr_t> *_attrs;	// attributes in this relation
  int _is_real_rel;		// TRUE if this appears in the from clause
				// false if it is an extent for a
				// "reference" attribute

public:
  Arel_t (void) {_attrs = 0;}
  ~Arel_t (void) {delete _attrs;}
  void init (int n, Acat_t::relid_t r, Acat_t::attr_type_t t, const char *v,
	     int is_real_relation = 1);

  int is_real_rel (void) const;
  int has_extent_or_set (void) const;
  const char *name (void) const;
  const char *rel_var (void) const {return _rel_var;}	
  Acat_t::relid_t relid (void) const {return _relid;}
  Acat_t::attr_type_t otypeid (void) const {return _otypeid;}
  const Aset_t<Aattr_t> &attrs (void) const {return *_attrs;}
  void allocate_attrs (void);	// allocate space for _attrs.
  void insert_attr (Aattr_t *a); // insert this attr into _attrs.

  int is_equal_to (const Arel_t *rel) const;

  long int numtuples (void) const;
  int tupsize (void) const;
  int numattrs (void) const;	// number of attrs this relation has
  void nth_attrname (int n, char *attrname) const; 
				// name of the nth attr.

  // functions for the Aset_t<Arel_t> class
  static int TotalNumber (void);
  static Arel_t *NthElement (int N); 
  int Number (void) const {return _number;}
}; 


/***************************************************************************

  inline functions

***************************************************************************/
inline void Arel_t::init (int n, Acat_t::relid_t r, Acat_t::attr_type_t t,
			  const char *v, int is_real_relation)
{
  _number = n; _relid = r; _otypeid = t; 
  _rel_var = new char [strlen (v) + 1]; assert (_rel_var);
  strcpy (_rel_var, v);
  _is_real_rel = is_real_relation;
}

inline int Arel_t::is_equal_to (const Arel_t *rel) const 
{
  return _number == rel->_number;
}

inline int Arel_t::is_real_rel (void) const
{
  return _is_real_rel;
}

inline int Arel_t::has_extent_or_set (void) const
{
  return !Aglob_vars()->cat->relid_is_invalid (_relid);
}

inline const char *Arel_t::name (void) const 
{
  return Aglob_vars()->cat->relname (_relid);
}

inline int Arel_t::numattrs (void) const
{
  return Aglob_vars()->cat->numattrs (_relid);
}

inline void Arel_t::nth_attrname (int n, char *attrname) const
{
  Aglob_vars()->cat->nth_attrname (_relid, n, attrname);
}

inline void Arel_t::allocate_attrs (void)
{
  _attrs = new Aset_t<Aattr_t>;
  assert (_attrs);
}

inline void Arel_t::insert_attr (Aattr_t *a)
{
  _attrs->Insert (a);
}

inline long int Arel_t::numtuples (void) const
{
  return Aglob_vars()->cat->numtuples (_relid);
}

inline int Arel_t::tupsize (void) const
{
  return Aglob_vars()->cat->tupsize (_relid);
}

#endif /* AREL_H */
