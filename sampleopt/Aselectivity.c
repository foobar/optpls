#include <Aseldesc.h>
#include <Afunc.h>
#include <Aptree.h>
#include <Apred.h>
#include <Alognode.h>
#include <Aquery.h>
#include <Alogprop.h>
#include <Aopalgos.h>

const double unknownopenendedselect = 0.33; // default selectivities in 
const double unknownequalityselect = 0.1;   // case no information can be
const double unknownjoinattribute = 0.1;    // gathered from the catalogs
const double unknownrangeselect = 0.25;	    
const double unknownoverlapselect = 0.1;

const double almost_zero = 0.00001;	// to avoid overflow problems.

inline int almost_equal (double d1, double d2) 
{
  double diff = d1 - d2;
  diff = diff < 0 ? -diff : diff;
  return diff < almost_zero;
}

// get the domain associated with an attribute
static Acat_t::relid_t Aget_domain (const Aptree_t &ptree,
				    const Alognode_t *lognode)
{
  // This whole function looks stupid to me.
  // I wonder what the O3DB people had in mind...

  Aattr_t *attr = ((Aattref_t *)ptree.func ())->attr ();
  if (attr->rel ()->is_real_rel () &&
      !attr->is_reference ())
    {
      assert (lognode->GetOp ()->Arity () > 0);
      if (lognode->GetLogProps ()->operations ().HasElement (&ptree))
	return attr->rel ()->relid ();
				// get the base set associated where
				// the path originates
      else
	return Aglob_vars()->cat->type_extent (attr->rel ()->otypeid ());
				// if there is an extent associated
				// with the rel return that
    }
  else
    return attr->attr_extent ();
  
  return Aglob_vars()->cat->invalid_relid ();
				// to satisfy picky compilers.
}

inline char *Aprepend (char *curr, const char *comp)
{
  int len = strlen (comp) + 1;
  char *start = curr - len;
  strcpy (start, comp);
  return start;
}

static Acat_t::relid_t Aget_path_idx (const Aptree_t &ptree,
				      const Alognode_t *lognode)
{
  assert (ptree.is_attref ());
  Aattref_t *attref = (Aattref_t *)ptree.func ();
  
  char idx_pathname[A_MAXPATHLEN];
  char *curr_pathname = idx_pathname + sizeof (idx_pathname);
  char *new_pathname = Aprepend (curr_pathname, attref->attrname ());
  curr_pathname[-1] = '\0';	// null termination.
				// remember, the first time its not a dot.

  char *try_pathname;
  Acat_t::relid_t rel_index = Aglob_vars()->cat->invalid_relid ();
  Acat_t::relid_t ext_index = rel_index;

  Aptree_t *relreftree = ptree.arg (0);
  Arelref_t *relref = (Arelref_t *)relreftree->func ();

  while (relref)
    {
      curr_pathname = new_pathname;
      if (relref->type () == Afunc_t::tuplevar)
	{
	  new_pathname = Aprepend (curr_pathname, relref->relname ());
	  curr_pathname[-1] = '.';
	  if (lognode->GetLogProps ()->operations ().HasElement (relreftree))
	    rel_index = Aglob_vars()->cat->idxid (relref->rel ()->relid (),
						  new_pathname);
	  relref = 0;
	}
      else
	{
	  assert (relref->type () == Afunc_t::tupleref);
	  Atupleref_t *tupleref = (Atupleref_t *)relref;

	  if (!Aglob_vars()->cat->relid_is_invalid (ext_index) &&
	      tupleref->rel ()->has_extent_or_set ())
	    {
	      Acat_t::relid_t extid = tupleref->rel ()->relid ();
	      const char *extname = Aglob_vars()->cat->relname (extid);
	      try_pathname = Aprepend (curr_pathname, extname);
	      curr_pathname[-1] = '.';
	      ext_index = Aglob_vars()->cat->idxid (extid, try_pathname);
	    }
	  new_pathname = Aprepend (curr_pathname, tupleref->component ());
	  curr_pathname[-1] = '.';
	  relreftree = relreftree->arg (0);
	  relref = (Arelref_t *)relreftree->func ();
	}
    }

  if (!Aglob_vars()->cat->relid_is_invalid (rel_index))
    return rel_index;
  return ext_index;
}

static double Aselectivity_of_select (const Aptree_t &ptree,
				      const Alognode_t *lognode)
{
  double selectivity = 0.1;
  double left_idx_card = -1, right_idx_card = -1;
  if (ptree.arg (0)->is_attref ())
    {
      if (((Aattref_t *)ptree.arg (0)->func ())->attr ()->is_key ())
	{
	  Acat_t::relid_t relid = Aget_domain (*ptree.arg (0), lognode);
	  if (!Aglob_vars()->cat->relid_is_invalid (relid))
	    {
	      double cardinality = Aglob_vars()->cat->numtuples (relid);
	      assert (cardinality > 0);
	      return 1.0/cardinality;
	    }
	}
      else			// try using index information
	{
	  Acat_t::relid_t idxid = Aget_path_idx (*ptree.arg (0), lognode);
	  if (!Aglob_vars()->cat->relid_is_invalid (idxid))
	    {
	      left_idx_card = Aglob_vars()->cat->numtuples (idxid);
	      assert (left_idx_card >= 0);
	    }
	}
    }

  if (ptree.arg (1)->is_attref ())
    {
      if (((Aattref_t *)ptree.arg (1)->func ())->attr ()->is_key ())
	{
	  Acat_t::relid_t relid = Aget_domain (*ptree.arg (1), lognode);
	  if (!Aglob_vars()->cat->relid_is_invalid (relid))
	    {
	      double cardinality = Aglob_vars()->cat->numtuples (relid);
	      assert (cardinality > 0);
	      return 1.0/cardinality;
	    }
	}
      else			// try using index information
	{
	  Acat_t::relid_t idxid = Aget_path_idx (*ptree.arg (1), lognode);
	  if (!Aglob_vars()->cat->relid_is_invalid (idxid))
	    {
	      right_idx_card = Aglob_vars()->cat->numtuples (idxid);
	      assert (left_idx_card >= 0);
	    }
	}
    }

  double idx_card = 
    left_idx_card > right_idx_card ? left_idx_card : right_idx_card;
  
  if (idx_card > 0)
    selectivity = 1.0/idx_card;
  else
    selectivity = 0.1;
  
  return selectivity;
}

static double Aselectivity_of_join (const Aptree_t &ptree,
				    const Alognode_t *lognode)
{
  assert (ptree.arg (0)->is_attref () &&
	  ptree.arg (1)->is_attref ());

  Aattr_t *left_attr = ((Aattref_t *)ptree.arg (0)->func ())->attr ();
  Aattr_t *right_attr = ((Aattref_t *)ptree.arg (1)->func ())->attr ();

  Acat_t::relid_t left_relid = Aget_domain (*ptree.arg (0), lognode);
  Acat_t::relid_t right_relid = Aget_domain (*ptree.arg (1), lognode);

//   if (left_relid != right_relid)
//     {
//       cerr << "Warning: relids not equal: " << (int)left_relid << " "
// 	<< (int)right_relid << endl;
//     }
// I wonder why the O3DB people had this
// This seems like a BUGBUG.

  double selectivity;
  if (Aglob_vars()->cat->relid_is_invalid (left_relid))
    selectivity = 0.01;
  else
    {
      double left_card = Aglob_vars()->cat->numtuples (left_relid);
      selectivity = 1.0/left_card;
    }

  return selectivity;
}

double Aselectivity (Aset_t<Apred_t> &predset, Alognode_t *lognode)
{
  double selectivity = 1.0;
  FOR_EACH_ELEMENT_OF_SET (predset)
    selectivity *= predset.CurrentElement ()->selectivity (lognode);
  return selectivity;
}


Aseldesc_t Afunc_t::seldesc (const Aptree_t &ptree,
			     const Alognode_t *lognode) const
{
  assert (!"Afunc_t::seldesc called!");
  Aseldesc_t to_keep_the_compiler_happy;
  return to_keep_the_compiler_happy;
}

Aseldesc_t Anot_t::seldesc (const Aptree_t &ptree,
			    const Alognode_t *lognode) const
{
  Aseldesc_t retval;
  Aseldesc_t s1 = ptree.arg (0)->seldesc (lognode);
  retval.selectivity = 1 - s1.selectivity;

  return retval;
}

Aseldesc_t Aboolfunc_t::seldesc (const Aptree_t &ptree,
				 const Alognode_t *lognode) const
{
  Aseldesc_t s1 = ptree.arg (0)->seldesc (lognode);
  Aseldesc_t s2 = ptree.arg (1)->seldesc (lognode);
  Aseldesc_t retval;

  if (type () == and)
    retval.selectivity = s1.selectivity * s2.selectivity;
  else if (type () == or)
    retval.selectivity = 
      s1.selectivity + s2.selectivity + s1.selectivity * s2.selectivity;
  else
    assert (!"unknown boolean operator");

  return retval;
}

Aseldesc_t Aopen_relop_t::seldesc (const Aptree_t &ptree,
				   const Alognode_t *lognode) const
{
  Aseldesc_t s1 = ptree.arg (0)->seldesc (lognode);
  Aseldesc_t s2 = ptree.arg (1)->seldesc (lognode);

  Aseldesc_t retval;

  assert (!ptree.arg (0)->is_literal ());
				// we dont allow the left-hand-side of a
				// predicate to be a constant.

  if (s1.nothing_known || s2.nothing_known)
    {
      retval.selectivity = unknownopenendedselect;
    }
  else if (!ptree.arg (1)->is_literal ())
    {
      retval.selectivity = unknownopenendedselect;
				// actually we can find out some information
				// about this case. but the calculations
				// are complex. i will try to figure out
				// the whole stuff someday...
    }
  else
    {
      Aliteral_t *lit = ((Aliteral_t *)ptree.arg (1)->func ());
      Acat_t::attr_type_t attr_type = lit->adt_type ();
      if (!Aglob_vars()->cat->is_numeric (attr_type))
	{
	  retval.selectivity = unknownopenendedselect;
				// we cannot use min_value and max_value
				// information for strings.
	}
      else
	{
	  double min_val = s1.min_val.value;
	  double max_val = s1.max_val.value;
	  double middle = s2.min_val.value; 
	  if (min_val == max_val)
	    if (middle == min_val)
	      retval.selectivity = 1;
	    else
	      retval.selectivity = 0;
	  else
	    {
	      if (middle < min_val) middle = min_val;
	      if (middle > max_val) middle = max_val;

	      if (almost_equal (min_val, max_val)) 
				// dont divide by (almost)zero.
		retval.selectivity = 1.0;
	      else if (type () == lt || type () == leq)
		retval.selectivity = (middle-min_val)/(max_val-min_val);
	      else
		retval.selectivity = (max_val-middle)/(max_val-min_val);
				// max_val-min_val cannot be zero.
	    }
	}
    }
  return retval;
}

Aseldesc_t Aclose_relop_t::seldesc (const Aptree_t &ptree,
				    const Alognode_t *lognode) const
{
  double selectivity;
  switch (lognode->GetOp ()->GetNumber ())
    {
    case Aselect:
      selectivity = Aselectivity_of_select (ptree, lognode);
      break;
    case Ajoin:
      selectivity = Aselectivity_of_join (ptree, lognode);
      break;
    default:
      assert (!"selectivity computation only for selects and joins");
      break;
    }

  assert (type () == eq);

  Aseldesc_t retval;
  retval.selectivity = selectivity;
  return retval;
}

// "between" and "not between" predicates
Aseldesc_t Arange_t::seldesc (const Aptree_t &ptree,
			      const Alognode_t *lognode) const
{
  Aseldesc_t s1 = ptree.arg (0)->seldesc (lognode);
				// the attref
  Aseldesc_t s2 = ptree.arg (1)->seldesc (lognode);
				// lower bound
  Aseldesc_t s3 = ptree.arg (2)->seldesc (lognode);
				// upper bound

  double selectivity = unknownrangeselect;
  
  
  // here i should use the min_value and max_value to compute something.
  // sometime i will fix this...

  Aseldesc_t retval;
  retval.selectivity = type () == between ? selectivity : 1.0 - selectivity;
  return retval;
}

Aseldesc_t Arelref_t::seldesc (const Aptree_t &ptree,
			       const Alognode_t *lognode) const
{
  Aseldesc_t retval; retval.nothing_known = 1; return retval;
}

Aseldesc_t Atuplevar_t::seldesc (const Aptree_t &ptree,
				 const Alognode_t *lognode) const
{
  Aseldesc_t retval; retval.nothing_known = 1; return retval;
}

Aseldesc_t Atupleref_t::seldesc (const Aptree_t &ptree,
				 const Alognode_t *lognode) const
{
  if (lognode->GetOp ()->GetNumber () == Aselect)
    cerr << "Warning: tupleref in select" << endl;
  assert (rel ()->has_extent_or_set ());
  double cardinality = rel ()->numtuples ();
  Aseldesc_t retval;
  retval.selectivity = 1.0/cardinality;
  return retval;
}

Aseldesc_t Aattref_t::seldesc (const Aptree_t &ptree,
			       const Alognode_t *lognode) const
{
  Aseldesc_t retval; retval.nothing_known = 1;
  return retval;
}

Aseldesc_t Aoidref_t::seldesc (const Aptree_t &ptree,
			       const Alognode_t *lognode) const
{
  Aseldesc_t retval;
  double numtuples = (double)_rel->numtuples ();
  assert (numtuples > 0);
  retval.selectivity = 1.0/numtuples;
  return retval;
}

Aseldesc_t Aliteral_t::seldesc (const Aptree_t &ptree,
				const Alognode_t *lognode) const
{
  Aseldesc_t retval;
  retval.uniq_keys = retval.cardinality = 1;

  if (Aglob_vars()->cat->is_numeric (adt_type ()))
    retval.max_val.value = retval.min_val.value = 
      Acat_t::get_numeric_val (adt ());
  else
    retval.nothing_known = 1;
  return retval;
}

Aseldesc_t Af_apply_t::seldesc (const Aptree_t &ptree,
				const Alognode_t *lognode) const
{
  Aseldesc_t retval; retval.nothing_known = 1; return retval;
}

