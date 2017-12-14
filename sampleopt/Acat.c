#include "Acat.h"

#include "Arel.h"
#include "Aattr.h"

#include <assert.h>
#include <string.h>


Acat_t::Acat_t (void)
{
  _numtypes = _numsets = _nummethods = 0;
  _otypes = 0;
  _osets = 0;
  _omethods = 0;
}

Acat_t::~Acat_t (void)
{
  delete [] _otypes;
  delete [] _osets;
  delete [] _omethods;
}


Acat_t::relid_t Acat_t::relid (const char *rel_name) const
{
  for (int i=0; i<_numsets; i++)
    if (!_osets[i].is_index &&
	strncmp (_osets[i].name, rel_name, sizeof (_osets[i].name)) == 0)
      return (relid_t)i;
  return invalid_relid ();	// relation not found
}

Acat_t::attrid_t Acat_t::attrid_bytype (attr_type_t attr_type, 
					const char *attr_name) const
{
  int start_attrid = _otypes[attr_type].first_method;
  int end_attrid = start_attrid + _otypes[attr_type].num_methods;
  for (int i= start_attrid; i<end_attrid; i++)
    if (strncmp (_omethods[i].name, attr_name, 
		 sizeof (_omethods[i].name)) == 0)
      return (attrid_t)i;
  return invalid_attrid ();	// attribute not found
}


Acat_t::idxid_t Acat_t::idxid (relid_t relid, const char *pathname) const
{
  assert (!relid_is_invalid (relid));
  _oset_t &set = _osets[relid];

  const char *only_pathname = strchr (pathname, '.');
  assert (only_pathname); 
  only_pathname++;		// to go beyond the .

  if (set.has_index &&
      strncmp (set.idx_pathname, only_pathname, 
	       sizeof (set.idx_pathname)) == 0)
    return set.idx_or_setid;
  else
    return invalid_idxid ();
}

Acat_t::idxid_t Acat_t::idxid (const char *pathname) const
{
  for (int i=0; i<_numsets; i++)
    {
      idxid_t ret_idxid = idxid ((relid_t)i, pathname);
      if (!idxid_is_invalid (ret_idxid))
	return ret_idxid;
    }

  return invalid_idxid ();	// index not found
}


static int Aindex_prefix_match (const char *idxpath, const char *prefixpath)
{
  const char *ip = idxpath, *pp = prefixpath;

  while (*ip && *ip == *pp)
    ip++, pp++;

  if (*pp == '\0' && (*ip == '.' || *ip == '\0'))
    return 1;			// yes this is a match
  else
    return 0;
}

int Acat_t::matches_index (const char *pathname) const
{
  const char *only_pathname = strchr (pathname, '.');
  int rel_name_len;
  if (only_pathname)
    {
      rel_name_len = (only_pathname - pathname);
      only_pathname++;		    
    }
  else
    rel_name_len = strlen (pathname);

  // now rel_name_len says that the first "rel_name_len" characters of
  // pathname correspond to the relation anme
  //
  // if only_pathname is NULL then we are
  // just trying to find a relation that
  // has and index and whose name is "pathname"
  //
  // else we are trying to find a relation whose
  // name is in the first "rel_name_len" characters of pathname
  // and whose pathname is given by "only_pathname"

  for (int i=0; i<_numsets; i++)
    {
      _oset_t &set = _osets[i];
      if (set.has_index &&
	  memcmp (set.name, pathname, rel_name_len) == 0)
	if (only_pathname)
	  {
	    if (Aindex_prefix_match (set.idx_pathname, only_pathname))
	      return 1;
	  }
	else
	  return 1;		// this is a relname which has an index
    }
  return 0;			// no match found.
}

int Acat_t::check_relop (Acat_t::relop_t relop, 
			 attr_type_t left, attr_type_t right) const
{
  return 0;			// no error
}

int Acat_t::check_func_call (const char *func_name, attr_type_t owner,
			     int nargs, attr_type_t *arg_types,
			     funcid_t &funcid, attr_type_t &return_type) const
{
  return 0;			// no error
}

int Acat_t::create_db (const char *name)
{
}
int Acat_t::open_db (const char *name)
{
}
int Acat_t::close_db (const char *name)
{
}

//int Acat_t::create_rel (const char *name, int num_attrs, attrec_t *attrs)
//{
//}
int Acat_t::create_idx (const char *idx_name, 
			const char *rel_name, const char *attr_name,
			int clustered, idx_type_t idx_type)
{
}
int Acat_t::drop_rel (const char *name)
{
}
int Acat_t::load_rel (const char *rel_name, 
		      int num_files, 
		      const char **load_files)
{
}

Acat_t::attr_type_t Acat_t::str_to_attr_type (const char *str)
{
  for (int i=0; i<_numtypes; i++)
    if (strncmp (str, _otypes[i].name, sizeof (_otypes[i].name)) == 0)
      return (attr_type_t)i;
  return bogus_adt ();
}

Acat_t::adt_t *Acat_t::adt_instance (Acat_t::attr_type_t type)
{
  adt_t *adt;
  if (strncmp (_otypes[type].name, "integer", 
	       sizeof (_otypes[type].name)) == 0)
    adt = new Binteger_t;
  else if (strncmp (_otypes[type].name, "float", 
		    sizeof (_otypes[type].name)) == 0)
    adt = new Breal_t;
  else if (strncmp (_otypes[type].name, "real", 
		    sizeof (_otypes[type].name)) == 0)
    adt = new Breal_t;
  else if (strncmp (_otypes[type].name, "string", 
		    sizeof (_otypes[type].name)) == 0)
    adt = new Bstring_t;
  else
    assert (!"unknownn attr type");

  assert (adt);
  return adt;			// caller assumes responsibility of deleting.
}

int Acat_t::func_exec (Acat_t::funcid_t f, 
				  adt_t *owner, 
				  adt_t *retval,
				  int nargs, adt_t **args)
{
  return 0;
}

