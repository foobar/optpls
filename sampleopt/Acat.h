#ifndef ACAT_H
#define ACAT_H

#include <iostream.h>
#include <assert.h>
#include <Aconsts.h>
#include <math.h>
#include <string.h>
#include <Badt.h>

// catalog information.
class Arel_t;
class Aattr_t;

class Amin_max_val_t {
public:
  double value;			// int's and real's are both converted
				// to double's...
  Amin_max_val_t (double v=0) : value (v) {}
};

class Acat_t {
  friend class Brqg_t;
public:
  typedef int otypeid_t;
  typedef int osetid_t;
  typedef int omethodid_t;

  // relid_t and attrid_t are "pointers" to specific relation and attribute
  // records in the catalog data structure.
  typedef osetid_t relid_t;
  typedef omethodid_t attrid_t;
  typedef osetid_t idxid_t;

  static int relid_is_invalid (const relid_t &relid) {return relid < 0;}
  static void invalidate_relid (relid_t &relid) {relid = -1;}
  static relid_t invalid_relid (void) {return (relid_t)-1;}
  static int attrid_is_invalid (const attrid_t &attrid) {return attrid < 0;}
  static void invalidate_attrid (attrid_t &attrid) {attrid = -1;}
  static attrid_t invalid_attrid (void) {return (attrid_t)-1;}
  static int idxid_is_invalid (const idxid_t &idxid) {return idxid < 0;}
  static void invalidate_idxid (idxid_t &idxid) {idxid = -1;}
  static idxid_t invalid_idxid (void) {return (idxid_t)-1;}


  typedef otypeid_t _attr_type_t;

//  enum _attr_type_t {_integer, _real, _string, _boolean, _reference, 
//		     _bogus_adt};
  enum _idx_type_t {_btree, _noindex};
				// these are not to be used by general public
				// use the following two wrappers instead.
  typedef int funcid_t;
  enum _relop_t {_eq, _neq, _lt, _leq, _gt, _geq, _ovlp, _not_ovlp, _noop};

  typedef _attr_type_t attr_type_t;
//   static attr_type_t integer (void) {return _integer;}
//   static attr_type_t real (void) {return _real;}
//   static attr_type_t string (void) {return _string;}
//   static attr_type_t boolean (void) {return _boolean;}
//   static attr_type_t reference (void) {return _reference;}
  static attr_type_t bogus_adt (void) {return (attr_type_t)-1;}
  attr_type_t str_to_attr_type (const char *str);

  static int is_numeric (attr_type_t a) {assert (0); return 0;}

  typedef _idx_type_t idx_type_t;
  static idx_type_t btree (void) {return _btree;}
  static idx_type_t noindex (void) {return _noindex;}

  typedef _relop_t relop_t;
  static relop_t eq (void) {return _eq;}
  static relop_t neq (void) {return _neq;}
  static relop_t lt (void) {return _lt;}
  static relop_t leq (void) {return _leq;}
  static relop_t gt (void) {return _gt;}
  static relop_t geq (void) {return _geq;}
  static relop_t ovlp (void) {return _ovlp;}
  static relop_t not_ovlp (void) {return _not_ovlp;}


  typedef Badt_t adt_t;
  adt_t *adt_instance (attr_type_t type);
  static double get_numeric_val (adt_t *adtptr);

private:

  class _otype_t {
  public:
    char name[A_MAXNAMELEN];
    int objsize;
    int numrefs;
    int complex;
    int has_extent;
    char extent_name[A_MAXNAMELEN];
    osetid_t extent_id;		// for the above extent_name;

    omethodid_t first_method;	// pointer to the first method associated
				// with this type
    int num_methods;		// number of methods.

    void read (istream &is);
  };

  class _oset_t {
  public:
    char name[A_MAXNAMELEN];
    char typename[A_MAXNAMELEN];
    otypeid_t otypeid;		// for above typename. (not for index).
    long int cardinality;
    int has_index;
    int is_index;
    char idx_pathname[A_MAXPATHLEN];
    char idx_or_set_name[A_MAXNAMELEN];
    osetid_t idx_or_setid;	// for above idx_or_set_name
    char idx_type[10];		// dont know what this is

    void read (istream &is);
  };

  class _omethod_t {
  public:
    char name[A_MAXNAMELEN];
    char owner_typename[A_MAXNAMELEN];
    otypeid_t otypeid;		// for above class
    char ret_typename[A_MAXNAMELEN];
    otypeid_t ret_otypeid;	// for above ret_typename
    int is_set;
    long int set_size;
    int is_ref;
    int is_key;
    int has_inverse;
    char inverse_name[A_MAXNAMELEN];
    omethodid_t inverse_id;	// for above inverse_name
    char inverse_typename[A_MAXNAMELEN];
    otypeid_t inverse_otypeid;	// for above inverse_typename
    void read (istream &is);
  };

private:
  int _numtypes;
  _otype_t *_otypes;
  int _numsets;
  _oset_t *_osets;
  int _nummethods;
  _omethod_t *_omethods;

  void set_pointers (void);
  osetid_t find_set_by_name (const char *name) const;
  void set_first_method_and_num_methods (otypeid_t owner_id,
					 omethodid_t &methodid,
					 int &num_methods) const;
  otypeid_t find_type_by_name (const char *name) const;
  omethodid_t find_method_by_name (const char *name) const;


public:
//  typedef _attrec_t attrec_t;
//  static void init_attrec (attrec_t &attrec, const char *n, 
//			   attr_type_t t, int l = -1) {attrec.init (n, t, l);}

  osetid_t type_extent (otypeid_t t) const {return _otypes[t].extent_id;}
  

public:
  Acat_t (void);
  ~Acat_t (void);

  void read (istream &is);

  relid_t relid (const char *rel_name) const; 
				// get the relid of relation "rel_name"
  attrid_t attrid_bytype (attr_type_t attr_type, const char *attr_name) const;
				// get the attrid of the attr "attr_name"
				// in type indicated by attr_type
  attrid_t attrid_byrel (relid_t relid, const char *attr_name) const;
				// get the attrid of the attr "attr_name"
				// in relation indicated by "relid"
  idxid_t idxid (const char *pathname) const;
				// get idxid of index "pathname".
  idxid_t idxid (relid_t relid, const char *pathname) const;
				// get idxid of index "pathname" on "relid"


  // information about types
  long int objsize (attr_type_t t) const;
  long int numrefs (attr_type_t t) const;

  // information about relations
  const char *relname (relid_t relid) const;
  otypeid_t otypeid (relid_t relid) const;
  long int numtuples (relid_t relid) const;
  long int tupsize (relid_t relid) const;
  int numattrs (relid_t relid) const;
  void nth_attrname (relid_t relid, int n, char *attrname) const;

  // information about attributes
  const char *attrname (attrid_t attrid) const;
  attr_type_t attr_type (attrid_t attrid) const;
  int is_numeric (attrid_t attrid) const;
  int is_reference (attrid_t attrid) const;
  int has_inverse (attrid_t attrid) const;
  int is_key (attrid_t attrid) const;
  int is_set_valued (attrid_t attrid) const;
  int avg_set_size (attrid_t attrid) const;
  relid_t attr_extent (attrid_t attrid) const;
  relid_t refrel (attrid_t attrid) const; 
				// relid of referenced relation if attr is
				// a reference type.

  // information about indexes
  const char *idxname (idxid_t idxid) const;
  int matches_index (const char *path_name) const;
  relid_t base_set (idxid_t idxid) const;
  int is_clustered (idxid_t idxid) const;
  long int uniq_keys (idxid_t idxid) const;
  Amin_max_val_t min_val (idxid_t idxid) const;
  Amin_max_val_t max_val (idxid_t idxid) const;

  // typechecking information
  int check_relop (relop_t relop, attr_type_t left, attr_type_t right) const;
  int check_func_call (const char *func_name, attr_type_t owner,
		       int nargs, attr_type_t *arg_types,
		       funcid_t &funcid, attr_type_t &return_type) const;
  static int func_exec (funcid_t f, adt_t *owner, 
			adt_t *retval, int nargs, adt_t **args);
  static int is_aggregate (funcid_t f);
  static int is_method (funcid_t f);

  // utilities. creating databases/relations etc.
  int create_db (const char *name);
  int open_db (const char *name);
  int close_db (const char *name);
//  int create_rel (const char *name, int num_attrs, attrec_t *attrs);
  int create_idx (const char *idx_name, const char *rel_name, 
		  const char *attr_name, int clustered, idx_type_t idx_type);
  int drop_rel (const char *name);
  int load_rel (const char *rel_name, int num_files, const char **load_files);
}; 

/***************************************************************************

  inline functions

***************************************************************************/
inline long int Acat_t::objsize (attr_type_t t) const
{
  assert (t >= 0);
  return _otypes[t].objsize;
}

inline long int Acat_t::numrefs (attr_type_t t) const
{
  assert (t >= 0);
  return _otypes[t].numrefs;
}

inline const char *Acat_t::relname (relid_t relid) const
{
  assert (!relid_is_invalid (relid));
  return _osets[relid].name;
}

inline Acat_t::otypeid_t Acat_t::otypeid (relid_t relid) const
{
  assert (!relid_is_invalid (relid));
  return _osets[relid].otypeid;
}

inline Acat_t::attrid_t Acat_t::attrid_byrel (relid_t relid, 
					      const char *attr_name) const
{
  assert (!relid_is_invalid (relid));
  return attrid_bytype (otypeid (relid), attr_name);
}

inline long int Acat_t::numtuples (relid_t relid) const
{
  assert (!relid_is_invalid (relid));
  return _osets[relid].cardinality;
}

inline long int Acat_t::tupsize (relid_t relid) const
{
  assert (!relid_is_invalid (relid));
  return _otypes[otypeid (relid)].objsize;
}

inline int Acat_t::numattrs (relid_t relid) const
{
  assert (!relid_is_invalid (relid));
  return _otypes[otypeid (relid)].num_methods;
}

inline const char *Acat_t::attrname (attrid_t attrid) const
{
  return _omethods[attrid].name;
}

inline void Acat_t::nth_attrname (relid_t relid, int n, char *aname) const
{
  strcpy (aname, attrname(_otypes[otypeid (relid)].first_method+n));
}

inline Acat_t::attr_type_t Acat_t::attr_type (attrid_t attrid) const
{
  return _omethods[attrid].ret_otypeid;
}

inline int Acat_t::is_reference (attrid_t attrid) const
{
  return _omethods[attrid].is_ref;
}

inline int Acat_t::has_inverse (attrid_t attrid) const
{
  return _omethods[attrid].has_inverse;
}

inline int Acat_t::is_key (attrid_t attrid) const
{
  return _omethods[attrid].is_key;
}

inline Acat_t::relid_t Acat_t::attr_extent (attrid_t attrid) const
{
  return type_extent (attr_type (attrid));
}

inline int Acat_t::is_numeric (attrid_t attrid) const
{
  assert (0);
//  attr_type_t a = attr_type (attrid);
//  return a == integer () || a == real ();
  return 0;
}

inline Acat_t::relid_t Acat_t::base_set (idxid_t idxid) const
{
  assert (idxid > 0 && _osets[idxid].is_index);
  return _osets[idxid].idx_or_setid;
}

inline const char *Acat_t::idxname (idxid_t idxid) const
{
  assert (!idxid_is_invalid (idxid));
  return _osets[idxid].name;
}

inline int Acat_t::is_clustered (idxid_t idxid) const
{
  assert (0);
  return 0;
}

inline long int Acat_t::uniq_keys (idxid_t idxid) const
{
  assert (0); return (0);
//  return _idxs[idxid].uniq_keys;
}

inline Amin_max_val_t Acat_t::min_val (idxid_t idxid) const
{
  assert (0); return Amin_max_val_t (0);
//  return _idxs[idxid].min_val;
}

inline Amin_max_val_t Acat_t::max_val (idxid_t idxid) const
{
  assert (0); return Amin_max_val_t (0);
//  return _idxs[idxid].max_val;
}

inline Acat_t::relid_t Acat_t::refrel (attrid_t attrid) const
{
  return attr_extent (attrid);	// the extent associated with
				// this attr (assuming it is
				// a reference attr).
}


inline double Acat_t::get_numeric_val (Acat_t::adt_t *adtptr)
{
  assert (0);
  return adtptr->get_numeric_val ();
}

inline int Acat_t::is_aggregate (funcid_t f)
{
  return 0;
}

inline int Acat_t::is_set_valued (attrid_t a) const
{
  return _omethods[a].is_set;
}

inline int Acat_t::avg_set_size (attrid_t a) const
{
  return _omethods[a].set_size;
}

inline int Acat_t::is_method (funcid_t f)
{
  return 0;
}

#endif /* ACAT_H */
