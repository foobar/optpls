#include <fstream.h>
#include <assert.h>

#include "Aquery.h"
#include "Aptree.h"
#include "Aconsts.h"
#include "Aoptions.h"

Aquery_t::Aquery_t (void)
{
    _numrels =
      _numattrs =
      _numpreds =
      _numoperations =
      _is_select_star =
      _has_aggregates = 0;

    _rels = 0;
    _attrs = 0;
    _preds = 0;
    _operations = 0;
    _groupby = 0;
    _orderby = 0;
    _target_attrs = 0;

    _relop_ops = _join_ops = _relref_ops = _tuplevar_ops = _tupleref_ops 
      = _attref_ops = _mat_ops = _target_ops = 0;
}

Aquery_t::~Aquery_t (void)
{
  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (_target_list)
    delete _target_list.Element ();

  delete [] _rels;
  delete [] _attrs;
  delete [] _preds;
  delete [] _operations;
  delete _target_attrs;

  delete _relop_ops;
  delete _join_ops;
  delete _relref_ops;
  delete _tuplevar_ops;
  delete _tupleref_ops;
  delete _attref_ops;
  delete _mat_ops;
  delete _target_ops;
}


Arel_t *Aquery_t::rel (const char *name) const
{
  for (int i=0; i<_numrels; i++)
    if (strcmp (_rels[i].rel.rel_var (), name) == 0)
      return &_rels[i].rel;
  return 0;			// failure.
}

Aattr_t *Aquery_t::attr (Arel_t *relptr, const char *attrname) const
{
  assert (relptr);
  for (int i=0; i<_numattrs; i++)
    if (_attrs[i].attr.rel ()->is_equal_to (relptr) &&
	strcmp (_attrs[i].attr.name (), attrname) == 0)
      return &_attrs[i].attr;

  return 0;			// failure
}

Aattr_t *Aquery_t::attr (const char *relname, const char *attrname) const
{
  Arel_t *relptr = rel (relname);
  if (!relptr) return 0;		// failure
  
  return attr (relptr, attrname);
}

void Aquery_t::init (void)
{
  int i, j;

  for (i=0; i<_numrels; i++)
    rel(i)->allocate_attrs ();
  for (i=0; i<_numattrs; i++)
    attr (i)->rel ()->insert_attr (attr (i));
				// these two loops are used to initialize
				// the query.rel(i)._attrs array, so that
				// it contains the set of attributes belonging
				// to that relation.


  for (i=0; i<_numpreds; i++)
    _preds[i].replace_relattrs (); 
  FOR_EACH_ELEMENT_OF_LIST (_target_list)
    _target_list.Element ()->replace_relattrs ();
				// replace the "rel_name.attr_name"'s
				// in the ptrees by references to
				// Aattr_t objects.
  
  compute_operations_array ();

  compute_eqattrs ();		// set up the equivalence classes of attr's.
				// i.e. compute the query._attrs[i].eqattrs

  for (i=0; i<_numpreds; i++)
    _preds[i].update_sets ();	// update the sets of attr's and rel's
				// in each pred.

  compute_conn_rels ();		// set up the information about which
				// relations are connected to each other.
				// i.e. compute the query._rels[i].conn_rels.
				// this is used to figure out which
				// joins are cartesian products and which
				// are not.

  compute_rel_selects ();	// find out which selects are applicable
				// to a given relation. in other words,
				// compute the query._rels[i].selects sets.
				// these can be viewed as "sargable" selects.

  compute_indexselects ();	// find out which selects can be applied
				// to an index (if one exists) on an attribute
				// in other words, compute the
				// query._attrs[i].indexselects sets.

  compute_target_attrs ();	// compute the set of attributes which have
				// been referenced in the target list.
}

void Aquery_t::compute_operations_array (void)
{
  Alist_t<Aptree_t> oplist;
  Aptree_t null_pred_tree = Aptree_t::null_pred_tree ();
				// this has ptree._number == 0.
				// this will be the 0th element of our
				// operations array and will signify all
				// ptrees which are not considered operations
				// i.e. literals.

  oplist.Insert (&null_pred_tree);

  for (int i=0; i<numpreds (); i++)
    pred (i)->make_operations_list (oplist);
  FOR_EACH_ELEMENT_OF_LIST (_target_list)
    _target_list.Element ()->make_operations_list (oplist);
  // should i be doing "group by" and "order by" things here too?

  _numoperations = oplist.Length ();
  assert (_numoperations > 0);	// at least the null pred tree should be there.
  _operations = new Aptree_t[_numoperations];
  assert (_operations);
  int op_num = 0;		// used for sanity checking.
  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (oplist)
    {
      _operations[op_num] = *oplist.Element ();
      assert (_operations[op_num].Number () == op_num);
      op_num++;
    }

  _relop_ops = new Aset_t<Aptree_t>; assert (_relop_ops);
  _join_ops = new Aset_t<Aptree_t>; assert (_join_ops);
  _relref_ops = new Aset_t<Aptree_t>; assert (_relref_ops);
  _tuplevar_ops = new Aset_t<Aptree_t>; assert (_tuplevar_ops);
  _tupleref_ops = new Aset_t<Aptree_t>; assert (_tupleref_ops);
  _attref_ops = new Aset_t<Aptree_t>; assert (_attref_ops);
  _mat_ops = new Aset_t<Aptree_t>; assert (_mat_ops);

  _op_tuprefs = new Aset_t<Aptree_t>[_numoperations]; assert (_op_tuprefs);
  _op_mats = new Aset_t<Aptree_t>[_numoperations]; assert (_op_mats);

  for (i=0; i<_numoperations; i++)
    {
      Aptree_t *oprn = &_operations[i];
      if (oprn->is_attref ())
	{
	  _attref_ops->Insert (oprn);
	  _mat_ops->Insert (oprn);
	  _op_mats[oprn->arg (0)->Number ()].Insert (oprn);
	}
      else if (oprn->is_relref ())
	{
	  if (((Arelref_t *)oprn->func ())->rel ()->has_extent_or_set ())
	    _relref_ops->Insert (oprn);
				// this is a proper relref only if
				// the rel can be scanned from a set.

	  if (oprn->func ()->type () == Afunc_t::tuplevar)
	    _tuplevar_ops->Insert (oprn);
	  else
	    {
	      _mat_ops->Insert (oprn);
	      _op_mats[oprn->arg (0)->Number ()].Insert (oprn);

	      _tupleref_ops->Insert (oprn);
	      _op_tuprefs[oprn->arg (0)->Number ()].Insert (oprn);
				// associate this oprn with the
				// set of tuplerefs of its parent
	    }
	}
      else if (oprn->is_relop ())
	{
	  assert (oprn->func ()->type () == Afunc_t::eq);
				// refuse to handle other relops.
	  _relop_ops->Insert (oprn);

	  if (!oprn->arg (0)->is_literal () && !oprn->arg (1)->is_literal ())
	    _join_ops->Insert (oprn);
				// maybe.
				// if neither side is a literal then this
				// has a high chance of being a join op.
	}
    }
}

// set up the equivalence classes of attr's.
// i.e. compute the query._attrs[i].eqattrs
void Aquery_t::compute_eqattrs (void)
{
  // initializations.
  for (int i=0; i<_numattrs; i++)
    {
      _attrs[i].eqattrs = new Aset_t<Aattr_t>;
      assert (_attrs[i].eqattrs);
      _attrs[i].eqattrs->Insert (attr (i));
    }

  // at the end of this for loop; the query.attrs[i].eqattrs
  // for each attr will contain a set of attributes belonging
  // to its equivalence class.
  for (int j=0; j<_numpreds; j++)
    {
      Aattr_t *a1, *a2;
      if (_preds[j].is_equijoin (a1, a2))
	{
	  // merge the eqattrs (equivalence sets) of a1 and a2.
	  Aset_t<Aattr_t> *merged_set = 
	    &_attrs[a1->Number ()].eqattrs->
	    Union (*_attrs[a2->Number ()].eqattrs); 
	  
	  // now update the eqattr sets of all attr's in the merged_set.
	  FOR_EACH_ELEMENT_OF_SET (*merged_set)
	    {
	      Aattr_t *a = merged_set->CurrentElement ();
	      *_attrs[a->Number ()].eqattrs = *merged_set;
	    }
	}
    }
}

// set up the information about which
// relations are connected to each other.
// ie.e compute the query._rels[i].conn_rels.
// this is used to figure out which
// joins are cartesian products and which
// are not.
void Aquery_t::compute_conn_rels (void)
{
  // initializations.
  for (int i=0; i<_numrels; i++)
    {
      _rels[i].conn_rels = new Aset_t<Arel_t>;
      assert (_rels[i].conn_rels);
      _rels[i].conn_rels->Insert (rel (i));
    }

  // here we do a naive evaluation of the transitive closure
  // of the connectivity graph.
  int there_was_a_change;
  do {
    there_was_a_change = 0;
    for (int i=0; i<_numrels; i++)
      for (int j=0; j<_numpreds; j++)
	{
	  Aset_t<Arel_t> predrels = _preds[j].rels ();
				// all the relations in this predicate
	  Aset_t<Arel_t> intersection = 
	    ASetIntersect (predrels, *_rels[i].conn_rels);
	  if (!intersection.IsEmpty () &&
	      !intersection.IsEqualTo (predrels))
	    {
	      _rels[i].conn_rels->Union (predrels);
	      there_was_a_change = 1;
	    }
	}
  } while (there_was_a_change);
}

// find out which selects are applicable
// to a given relation. in other words,
// compute the queyr._rels[i].selects sets.
// these can be viewed as "sargable" selects.
void Aquery_t::compute_rel_selects (void)
{
  int i;
  for (i=0; i<_numrels; i++)
    assert (_rels[i].selects = new Aset_t<Apred_t>);
				// initial allocation of space
  
  for (i=0; i<_numpreds; i++)
    switch (_preds[i].type ())
      {
      case Apred_t::singleattr:
	// in case of a singleattr pred, insert this pred in the select set
	// of each relation of each attribute in its eqattrs set.
	{
	  Aset_t<Aattr_t> pred_eqattrs;
	  if (Aglob_vars()->oopt->no_equivalences)
	    pred_eqattrs = _preds[i].attrs ();
	  else
	    pred_eqattrs = _preds[i].eqattrs ();

	  FOR_EACH_ELEMENT_OF_SET (pred_eqattrs)
	    {
	      int relnum = pred_eqattrs.CurrentElement ()->rel ()->Number ();
	      _rels[relnum].selects->Insert (&_preds[i]);
	    }
	}
	break;

      case Apred_t::singlerel:
      case Apred_t::oidref:
	{
	  Aset_t<Arel_t> pred_rels = _preds[i].rels ();
	  int relnum = pred_rels.FirstElement ()->Number ();
	  _rels[relnum].selects->Insert (&_preds[i]);
	}
	break;
      }
}


void Aquery_t::compute_indexselects (void)
{
//   for (int i=0; i<_numattrs; i++)
//     {
//       assert (_attrs[i].indexselects = new Aset_t<Apred_t>);
//       if (attr(i)->is_indexed ())
// 	for (int j=0; j<_numpreds; j++)
// 	  if (_preds[j].index_scannable (attr (i)))
// 	    _attrs[i].indexselects->Insert (&_preds[j]);
//     }
}

// compute the set of attributes which have
// been referenced in the target list.
void Aquery_t::compute_target_attrs (void)
{
  Aset_t<Arel_t> bogus;
  _target_attrs = new Aset_t<Aattr_t>;
  assert (_target_attrs);
  _target_ops = new Aset_t<Aptree_t>;
  assert (_target_ops);
  
  FOR_EACH_ELEMENT_OF_LIST (_target_list)
    {
      _target_list.Element ()->update_sets (bogus, *_target_attrs);
      _target_ops->Insert (_target_list.Element ());
    }
}
