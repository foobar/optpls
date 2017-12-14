#ifndef AATTR_H
#define AATTR_H

#include <Aglob_vars.h>
#include <Acat.h>
#include <Aset.h>

class Arel_t;

class Aattr_t {
  friend class Acat_t;
private:
  int _number;			// index into array of attributes in Aquery_t
  Arel_t *_rel;			// relation whose attribute this is
  Acat_t::attrid_t _attrid;	// for use by the Acat_t class

public:
  void init (int n, Arel_t *r, Acat_t::attrid_t a);

  const char *name (void) const;
  Arel_t *rel (void) const {return _rel;}
  Acat_t::attr_type_t type (void) const;
  Acat_t::relid_t attr_extent (void) const;

  int is_numeric (void) const;
  int is_reference (void) const;
  int is_key (void) const;
  int is_set_valued (void) const;
  int avg_set_size (void) const;
  
  int is_equal_to (const Aattr_t *other) const;
  int hash (void) const;

  // these functions are written so for the Aset_t<Aattr_t> class
  static int TotalNumber (void);
  static Aattr_t *NthElement (int N);
  int Number (void) const {return _number;}
};


Aset_t<Aattr_t> Aget_eq_closure (Aset_t<Aattr_t> &attrset);
				// this function returns a set of attributes
				// which contains all attributes which are
				// equivalent to some attribute in "attrset"




// inline functions
inline void Aattr_t::init (int n, Arel_t *r, Acat_t::attrid_t a)
{
  _number = n; _rel = r; _attrid = a;
}

inline const char *Aattr_t::name (void) const 
{
  return Aglob_vars()->cat->attrname (_attrid);
}

inline int Aattr_t::is_equal_to (const Aattr_t *other) const
{
  return _number == other->_number;
}

inline int Aattr_t::hash (void) const
{
  return _number;
}

inline Acat_t::attr_type_t Aattr_t::type (void) const
{
  return Aglob_vars()->cat->attr_type (_attrid);
}

inline Acat_t::relid_t Aattr_t::attr_extent (void) const
{
  return Aglob_vars()->cat->attr_extent (_attrid);
}

inline int Aattr_t::is_numeric (void) const
{
  return Aglob_vars()->cat->is_numeric (_attrid);
}

inline int Aattr_t::is_reference (void) const
{
  return Aglob_vars()->cat->is_reference (_attrid);
}

inline int Aattr_t::is_key (void) const
{
  return Aglob_vars()->cat->is_key (_attrid);
}


inline int Aattr_t::is_set_valued (void) const
{
  return Aglob_vars()->cat->is_set_valued (_attrid);
}

inline int Aattr_t::avg_set_size (void) const
{
  return Aglob_vars()->cat->avg_set_size (_attrid);
}

#endif /* AATTR_H */
