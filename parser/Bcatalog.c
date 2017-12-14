#include <Acat.h>
#include <assert.h>
#include <string.h>

#include <Bbogus.h>

void Bboolread (istream &is, int &boolval)
{
  char boolstr[10], *str = boolstr;
  is >> ws >> str;
  if (strncmp (boolstr, "Y", sizeof (boolstr)) == 0)
    boolval = 1;
  else if (strncmp (boolstr, "N", sizeof (boolstr)) == 0)
    boolval = 0;
  else
    assert (!"neither Y nor N");
}

// reading bogus catalog data from bogus file.
Acat_t::osetid_t Acat_t::find_set_by_name (const char *name) const
{
  for (int i=0; i<_numsets; i++)
    if (strncmp (_osets[i].name, name, sizeof (_osets[i].name)) == 0)
      return (osetid_t)i;
  assert (!"set not found");
  return (osetid_t) -1;
}

Acat_t::otypeid_t Acat_t::find_type_by_name (const char *name) const
{
  for (int i=0; i<_numtypes; i++)
    if (strncmp (_otypes[i].name, name, sizeof (_otypes[i].name)) == 0)
      return (otypeid_t)i;
  assert (!"type not found");
  return (otypeid_t) -1;
}

Acat_t::omethodid_t Acat_t::find_method_by_name (const char *name) const
{
  for (int i=0; i<_nummethods; i++)
    if (strncmp (_omethods[i].name, name, sizeof (_omethods[i].name)) == 0)
      return (omethodid_t)i;
  assert (!"method not found");
  return (omethodid_t) -1;
}

void Acat_t::set_first_method_and_num_methods (otypeid_t owner_id,
					       omethodid_t &methodid,
					       int &num_methods) const
{
  enum status_t {finding_first, counting, sanity_check};
  status_t status = finding_first;
  num_methods = 0;

  for (int i=0; i<_nummethods; i++)
    switch (status)
      {
      case finding_first:
	if (_omethods[i].otypeid == owner_id)
	  {
	    methodid = (omethodid_t)i;
	    status = counting;
	    num_methods++;
	  }
	break;

      case counting:
	if (_omethods[i].otypeid == owner_id)
	  num_methods++;
	else
	  status = sanity_check;
	break;

      case sanity_check:
	if (_omethods[i].otypeid == owner_id)
	  assert (!"sanity check failed");
	break;
      default:
	assert (!"huh?!");
	break;
      }
  
  assert (!_otypes[owner_id].complex || status != finding_first);
}

void Acat_t::set_pointers (void)
{
  int i;
  for (i=0; i<_numtypes; i++)
    {
      _otype_t &thetype = _otypes[i];
      if (thetype.has_extent)
	thetype.extent_id = find_set_by_name (thetype.extent_name);
      else
	thetype.extent_id = -1;
    }
  
  for (i=0; i<_numsets; i++)
    {
      _oset_t &set = _osets[i];
      set.otypeid = find_type_by_name (set.typename);
      if (set.is_index || set.has_index)
	set.idx_or_setid = find_set_by_name (set.idx_or_set_name);
      else
	set.idx_or_setid = -1;
    }

  for (i=0; i<_nummethods; i++)
    {
      _omethod_t &method = _omethods[i];
      method.otypeid = find_type_by_name (method.owner_typename);
      method.ret_otypeid = find_type_by_name (method.ret_typename);
      if (method.has_inverse)
	{
	  method.inverse_id = find_method_by_name (method.inverse_name);
	  method.inverse_otypeid = find_type_by_name (method.inverse_typename);
	}
      else
	{
	  method.inverse_id = -1;
	  method.inverse_otypeid = -1;
	}
    }

  for (i=0; i<_numtypes; i++)
    set_first_method_and_num_methods ((otypeid_t)i,
				      _otypes[i].first_method,
				      _otypes[i].num_methods);
}

void Acat_t::read (istream &is)
{
  int i;

  // the types
  is >> ws >> _numtypes;
  assert (_numtypes > 0);
  _otypes = new _otype_t[_numtypes];
  assert (_otypes);
  for (i=0; i<_numtypes; i++)
    _otypes[i].read (is);

  // the sets and indexes
  is >> ws >> _numsets;
  assert (_numsets > 0);
  _osets = new _oset_t[_numsets];
  assert (_osets);
  for (i=0; i<_numsets; i++)
    _osets[i].read (is);

  // the methods (attrs and refs)
  is >> ws >> _nummethods;
  assert (_nummethods > 0);
  _omethods = new _omethod_t[_nummethods];
  assert (_omethods);
  for (i=0; i<_nummethods; i++)
    _omethods[i].read (is);

  set_pointers ();
}

void Acat_t::_otype_t::read (istream &is)
{
  is >> ws >> name >> objsize >> numrefs;
  Bboolread (is, complex);
  Bboolread (is, has_extent);
  is >> ws >> extent_name;
}

void Acat_t::_oset_t::read (istream &is)
{
  is >> ws >> name >> typename >> cardinality;
  Bboolread (is, has_index);
  Bboolread (is, is_index);
  is >> ws >> idx_pathname >> ws >> idx_or_set_name >> ws >> idx_type;
}

void Acat_t::_omethod_t::read (istream &is)
{
  is >> ws >> name >> ws >> owner_typename >> ws >> ret_typename;
  Bboolread (is, is_set);
  // is_set = 0;
  is >> set_size;

  // read the is_ref field
  char is_ref_str[10], *s = is_ref_str;
  is >> ws >> s;
  if (strncmp (is_ref_str, "Link", sizeof (is_ref_str)) == 0)
    is_ref = 1;
  else if (strncmp (is_ref_str, "Attr", sizeof (is_ref_str)) == 0)
    is_ref = 0;
  else
    assert (!"neither link nor attr!");

  Bboolread (is, is_key);
  Bboolread (is, has_inverse);
  
  is >> ws >> inverse_name >> ws >> inverse_typename;
}
