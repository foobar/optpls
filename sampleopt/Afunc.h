#ifndef FUNC_H
#define FUNC_H

#include <iostream.h>
#include <string.h>
#include <assert.h>

#include <Aconsts.h>
#include <Acat.h>
#include <Aattr.h>
#include <Arel.h>
#include <Aseldesc.h>

class Aptree_t;
class Bexec_info_t;
class Alognode_t;

class Afunc_t {
public:
  enum type_t {
    empty = 0,
      and, or, not,
      lt, gt, leq, geq, eq, neq, between, not_between,
      literal, oidref, relattr, attref,
      tuplevar, tupleref,
      f_apply, unknown
  };
private:
  type_t _type;

  virtual int are_data_members_equal (const Afunc_t &f) const {return 1;}

public:
  Afunc_t (type_t t = unknown) {_type = t;}
  type_t type (void) const {return _type;}

  virtual Afunc_t *duplicate (void) const;
  virtual void replace_relattrs (Aptree_t &ptree);
				// this routine replaces all occurrences
				// of "attribute_name" or
				// "rel_name.attribute_name" by reference
				// to the proper Aattr_t objects.
  virtual void update_sets (const Aptree_t &ptree,
			    Aset_t<Arel_t> &rels,
			    Aset_t<Aattr_t> &attrs) const;
				// to "rels" and "attrs" add those
				// relations and attributes which are
				// referenced in this ptree

  virtual void read_args (istream &is, int &nargs, Aptree_t *&args);

  virtual Aseldesc_t seldesc (const Aptree_t &ptree,
			      const Alognode_t *lognode) const;
				// return the selectivity descriptor
				// for this expression... see file
				// ../opt/Aselectivity.h for details

  Aptree_t copy_tree (const Aptree_t &ptree) const;
  virtual void replace_eqattrs (const Aptree_t &ptree,
				const Aset_t<Aattr_t> &attrs);
				// defined in query/Brewrite.c
  virtual void put_suffixes (Aptree_t &ptree, 
				const Bexec_info_t &exec_info);
				// defined in query/Brewrite.c
  virtual void generate_name (const Aptree_t &ptree,
			      char *name, int num) const;
				// defined in query/Brewrite.c

  // identification functions
  int is_literal (void) const {return type () == literal;}
  int is_attref (void) const {return type () == attref;}
  int is_relref (void) const;
  int is_tupleref (void) const {return type () == tupleref;}
  virtual int is_relop (void) const {return 0;}

  virtual void write_args (ostream &os, const Aptree_t &ptree) const;
  int is_equal_to (const Afunc_t &f) const;
};


// the boolean not operator
class Anot_t : public Afunc_t {
private:
public:
  Anot_t (void) : Afunc_t (not) {}
  virtual Afunc_t *duplicate (void) const;
  virtual Aseldesc_t seldesc (const Aptree_t &ptree,
			     const Alognode_t *lognode) const;
};


// boolean operator. "and" "or"
class Aboolfunc_t : public Afunc_t {
private:
public:
  Aboolfunc_t (type_t t) : Afunc_t (t) {}
  virtual Afunc_t *duplicate (void) const;
  virtual Aseldesc_t seldesc (const Aptree_t &ptree,
			      const Alognode_t *lognode) const;
};

// relop. "<" ">" "<=" ">=" "==" "!="
class Arelop_t : public Afunc_t {
private:
public:
  Arelop_t (type_t t) : Afunc_t (t) {}
  virtual Afunc_t *duplicate (void) const;
  int is_relop (void) const {return 1;}
};

// "<" ">" "<=" ">=" are open-ended relops.
class Aopen_relop_t : public Arelop_t {
private:
public:
  Aopen_relop_t (type_t t) : Arelop_t (t) {}
  virtual Afunc_t *duplicate (void) const;
  virtual Aseldesc_t seldesc (const Aptree_t &ptree,
			      const Alognode_t *lognode) const;
};

// "==" and "!=" are close-ended relops.
class Aclose_relop_t : public Arelop_t {
private:
public:
  Aclose_relop_t (type_t t) : Arelop_t (t) {}
  virtual Afunc_t *duplicate (void) const;
  virtual Aseldesc_t seldesc (const Aptree_t &ptree,
			      const Alognode_t *lognode) const;
};


// stands for "between" and "not between" queries.
class Arange_t : public Afunc_t {
private:
public:
  Arange_t (type_t t) : Afunc_t (t) {}
  virtual Afunc_t *duplicate (void) const;
  virtual void read_args (istream &is, int &nargs, Aptree_t *&args);
  virtual void write_args (ostream &os, const Aptree_t &ptree) const;
  virtual Aseldesc_t seldesc (const Aptree_t &ptree,
			      const Alognode_t *lognode) const;
};

// A reference to a relation. This can either be a tuple variable
// like the "c1" in "c1.polygon.shape", or it can be a tuple reference
// like "polygon" in the previous example
class Arelref_t : public Afunc_t {
protected:
  Arel_t *_rel;

  virtual int are_data_members_equal (const Afunc_t &f) const;

public:
  Arelref_t (Afunc_t::type_t t, Arel_t *r) : Afunc_t (t) {_rel = r;}
				// "t" can either be tuplevar or tupleref
  virtual Afunc_t *duplicate (void) const;

  const char *relname (void) const {return rel ()->name ();}
  const char *relvar (void) const {return rel ()->rel_var ();}
  Arel_t *rel (void) const {return _rel;}

  virtual void write_args (ostream &os, const Aptree_t &ptree) const;
  virtual Aseldesc_t seldesc (const Aptree_t &ptree,
			      const Alognode_t *lognode) const;
};

// A tuple variable. e.g. the "c1" in "c1.polygon.shape"
// This has no arguments.
class Atuplevar_t : public Arelref_t {
private:
public:
  Atuplevar_t (Arel_t *r = 0) : Arelref_t (tuplevar, r) {}
  virtual Afunc_t *duplicate (void) const;
  virtual Aseldesc_t seldesc (const Aptree_t &ptree,
			      const Alognode_t *lognode) const;
};

// A tuple reference e.g. the "polygon" in "c1.polygon.shape"
// Has one argument. Argument is the tuplevar or tupleref
// from which this reference originates. (e.g. "city")
class Atupleref_t : public Arelref_t {
private:
  Acat_t::attrid_t _attrid; 
public:
  Atupleref_t (void);
  Atupleref_t (Arel_t *r, Acat_t::attrid_t a);
  virtual Afunc_t *duplicate (void) const;

  const char *full_path (void) const {return relvar ();}
  const char *component (void) const;
  Acat_t::attrid_t attrid (void) const {return _attrid;}
  virtual Aseldesc_t seldesc (const Aptree_t &ptree,
			      const Alognode_t *lognode) const;
};

// an attribute. 
// Has one argument. Argument is the tuplevar or tupleref
// from which this reference originates. (e.g. "city")
class Aattref_t : public Afunc_t {
private:
  char _attrname[A_MAXNAMELEN];	// name of the attribute
  Aattr_t *_attr;		// this will be filled in by
				// replace_relattrs.

  virtual int are_data_members_equal (const Afunc_t &f) const;

public:
  Aattref_t (Aattr_t *a = 0) : Afunc_t (attref) {_attr = a;}
  Aattref_t (const char *aname);

  const char *attrname (void) const {return _attrname;}
  Aattr_t *attr (void) const {return _attr;}
  virtual Afunc_t *duplicate (void) const;
  virtual void update_sets (const Aptree_t &ptree,
			    Aset_t<Arel_t> &rels,
			    Aset_t<Aattr_t> &attrs) const;
  virtual Aseldesc_t seldesc (const Aptree_t &ptree,
			      const Alognode_t *lognode) const;

  virtual void replace_relattrs (Aptree_t &ptree);
  virtual void replace_eqattrs (const Aptree_t &ptree,
				const Aset_t<Aattr_t> &attrs);
				// defined in query/Brewrite.c
  virtual void put_suffixes (Aptree_t &ptree,
			     const Bexec_info_t &exec_info);
				// defined in query/Brewrite.c
  virtual void generate_name (const Aptree_t &ptree,
			      char *name, int num) const;
				// defined in query/Brewrite.c

  virtual void write_args (ostream &os, const Aptree_t &ptree) const; 
};


// unused.
// "relname.attrname". connects relation to its attribute.
class Arelattr_t : public Afunc_t {
private:
  char _relname[A_MAXNAMELEN];	
  char _suffix [A_MAXNAMELEN];
  char _attrname[A_MAXNAMELEN];
public:
  Arelattr_t (void) : Afunc_t (relattr) {}
  Arelattr_t (const char *rname, const char *aname, const char *suff = "");

  const char *relname (void) const {return _relname;}
  const char *suffix (void) const {return _suffix;}
  const char *attrname (void) const {return _attrname;}

  virtual Afunc_t *duplicate (void) const;
  virtual void replace_relattrs (Aptree_t &ptree);
  virtual void read_args (istream &is, int &nargs, Aptree_t *&args);
  virtual void write_args (ostream &os, const Aptree_t &ptree) const;
};

// user defined functions.

class Af_apply_t : public Afunc_t {
private:
  Acat_t::funcid_t _funcid;
  char _name [A_MAXNAMELEN];
  
  virtual int are_data_members_equal (const Afunc_t &f) const;


public:
  Af_apply_t (void) : Afunc_t (f_apply) {}
  Af_apply_t (Acat_t::funcid_t f, const char *n);
  virtual Afunc_t *duplicate (void) const;

  Acat_t::funcid_t funcid (void) const {return _funcid;}
  const char *name (void) const {return _name;}

  virtual void read_args (istream &is, int &nargs, Aptree_t *&args);
  virtual void write_args (ostream &os, const Aptree_t &ptree) const;
  virtual void generate_name (const Aptree_t &ptree,
			      char *name, int num) const;
				// defined in query/Brewrite.c
  virtual Aseldesc_t seldesc (const Aptree_t &ptree,
			      const Alognode_t *lognode) const;
};

// relation.oid = "oid_string"
class Aoidref_t : public Afunc_t {
private:
  Arel_t *_rel;			// relation for which this oid ref is 
  char _oidstr[A_MAXNAMELEN];

  virtual int are_data_members_equal (const Afunc_t &f) const;


public:
  Aoidref_t (Arel_t *r, const char *str);
  const char *oidstr (void) const {return _oidstr;}
  virtual Afunc_t *duplicate (void) const;
  virtual void read_args (istream &is, int &nargs, Aptree_t *&args);
  virtual void write_args (ostream &os, const Aptree_t &ptree) const;
  virtual Aseldesc_t seldesc (const Aptree_t &ptree,
			      const Alognode_t *lognode) const;
};

// literals
class Aliteral_t : public Afunc_t {
private:
  Acat_t::attr_type_t _adt_type;
  Acat_t::adt_t *_adt;

  virtual int are_data_members_equal (const Afunc_t &f) const;

public:
  Aliteral_t (Acat_t::attr_type_t adt_type = Acat_t::bogus_adt (),
	      Acat_t::adt_t *adt_val = 0);
  virtual Afunc_t *duplicate (void) const;

  Acat_t::attr_type_t adt_type (void) const {return _adt_type;}
  Acat_t::adt_t *adt (void) const {return _adt;}

  virtual void read_value (istream &is); // read the value of the literal
  virtual void read_args (istream &is, int &nargs, Aptree_t *&args);
  virtual void write_args (ostream &os, const Aptree_t &ptree) const;
  int is_literal (void) const {return 1;}
  virtual Aseldesc_t seldesc (const Aptree_t &ptree, 
			      const Alognode_t *lognode) const;
};

/***************************************************************************

  inline functions
***************************************************************************/

inline int Afunc_t::is_equal_to (const Afunc_t &f) const
{
  if (type () != f.type ()) return 0; // they cannot be equal.
  return are_data_members_equal (f);
}

inline Afunc_t *Afunc_t::duplicate (void) const 
{
  Afunc_t *newfunc = new Afunc_t; assert (newfunc);
  *newfunc = *this; return newfunc;
}

inline int Afunc_t::is_relref (void) const 
{
  return type () == tupleref || type () == tuplevar;
}

inline Afunc_t *Anot_t::duplicate (void) const 
{
  Anot_t *newfunc = new Anot_t; assert (newfunc);
  *newfunc = *this; return newfunc;
}

inline Afunc_t *Aboolfunc_t::duplicate (void) const 
{
  Aboolfunc_t *newfunc = new Aboolfunc_t (type ()); assert (newfunc);
  *newfunc = *this; return newfunc;
}

inline Afunc_t *Arelop_t::duplicate (void) const 
{
  Arelop_t *newfunc = new Arelop_t (type ()); assert (newfunc);
  *newfunc = *this; return newfunc;
}

inline Afunc_t *Aopen_relop_t::duplicate (void) const 
{
  Aopen_relop_t *newfunc = new Aopen_relop_t (type ()); assert (newfunc);
  *newfunc = *this; return newfunc;
}

inline Afunc_t *Aclose_relop_t::duplicate (void) const 
{
  Aclose_relop_t *newfunc = new Aclose_relop_t (type ()); assert (newfunc);
  *newfunc = *this; return newfunc;
}

inline Afunc_t *Arange_t::duplicate (void) const 
{
  Arange_t *newfunc = new Arange_t (type ()); assert (newfunc);
  *newfunc = *this; return newfunc;
}

inline Afunc_t *Arelref_t::duplicate (void) const
{
  Arelref_t *newfunc = new Arelref_t (type (), _rel); assert (newfunc);
  *newfunc = *this; return newfunc;
}

inline Afunc_t *Atuplevar_t::duplicate (void) const
{
  Atuplevar_t *newfunc = new Atuplevar_t; assert (newfunc);
  *newfunc = *this; return newfunc;
}

inline Atupleref_t::Atupleref_t (void) : Arelref_t (tupleref, 0) 
{
  Aglob_vars()->cat->invalidate_attrid (_attrid);
}

inline Atupleref_t::Atupleref_t (Arel_t *r, Acat_t::attrid_t a) 
  : Arelref_t (tupleref, r),
    _attrid (a)
{
}

inline Afunc_t *Atupleref_t::duplicate (void) const
{
  Atupleref_t *newfunc = new Atupleref_t; assert (newfunc);
  *newfunc = *this; return newfunc;
}

inline const char *Atupleref_t::component (void) const
{
  return Aglob_vars()->cat->attrname (_attrid);
}

inline Aattref_t::Aattref_t (const char *aname) 
  : Afunc_t (attref)
{
  strncpy (_attrname, aname, sizeof (_attrname));
  _attr = 0;			// for now. this will be updated by
				// replace_relattrs.
}

inline Afunc_t *Aattref_t::duplicate (void) const 
{
  Aattref_t *newfunc = new Aattref_t; assert (newfunc);
  *newfunc = *this; return newfunc;
}


// Arelattr_t is no longer relevant
inline Arelattr_t::Arelattr_t (const char *rname, const char *aname, 
			       const char *suff) : Afunc_t (relattr)
{
  strncpy (_relname, rname, sizeof (_relname));
  strncpy (_suffix, suff, sizeof (_suffix));
  strncpy (_attrname, aname, sizeof (_attrname));
}

inline Afunc_t *Arelattr_t::duplicate (void) const 
{
  Arelattr_t *newfunc = new Arelattr_t; assert (newfunc);
  *newfunc = *this; return newfunc;
}

inline Af_apply_t::Af_apply_t (Acat_t::funcid_t f, const char *n)
{
  _funcid = f;
  strncpy (_name, n, sizeof (_name));
}

inline Afunc_t *Af_apply_t::duplicate (void) const 
{
  Af_apply_t *newfunc = new Af_apply_t; assert (newfunc);
  *newfunc = *this; return newfunc;
}

inline Aoidref_t::Aoidref_t (Arel_t *r = 0, const char *str = "")
  : Afunc_t (oidref)
{
  _rel = r;
  strncpy (_oidstr, str, sizeof (_oidstr));
}

inline Afunc_t *Aoidref_t::duplicate (void) const 
{
  Aoidref_t *newfunc = new Aoidref_t; assert (newfunc);
  *newfunc = *this; return newfunc;
}

inline Aliteral_t::Aliteral_t (Acat_t::attr_type_t atype,
			       Acat_t::adt_t *adt_val)
  : Afunc_t (literal)
{
  _adt_type = atype;
  _adt = adt_val;
}

inline Afunc_t *Aliteral_t::duplicate (void) const 
{
  Aliteral_t *newfunc = new Aliteral_t; assert (newfunc);
  *newfunc = *this; return newfunc;
}

istream &operator>> (istream &is, Afunc_t *&func);
ostream &operator<< (ostream &os, const Afunc_t *func);

#endif /* FUNC_H */
