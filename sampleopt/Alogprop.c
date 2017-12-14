#include <assert.h>

#include <Alogprop.h>
#include <Aquery.h>
#include <Aopinc.h>
#include <Alognode.h>
#include <math.h>
#include <string.h>

Aset_t<Aptree_t> Acompute_need_unnesting (Aset_t<Aptree_t> &oprns);


Alogprop_t *Alogprop_t::Duplicate (void) const
{
  Alogprop_t *l = new Alogprop_t;
  assert (l);
  *l = *this;			
  if (_index_path)
    {
      l->_index_path = new char[strlen (_index_path)+1];
      assert (l->_index_path);
      strcpy (l->_index_path, _index_path);
    }
  return l;
}

Alogprop_t::Alogprop_t (Aget_t *op, Alognode_t *node)
{
  node->SetLogProps (this);
  Arel_t *r = op->rel ();

  const char *relname = r->name ();
  _index_path = 0;
  if (Aglob_vars()->cat->matches_index (relname))
    {
      _index_path = new char [strlen (relname) + 1];
      assert (_index_path);
      strcpy (_index_path, relname);
    }

  outputop_applied = 0;
  _is_interesting = _index_path ? 1 : 0;

  Aptree_t::init_operations_set (_operations);
  _operations.Union (op->operations ()); 

  _need_unnesting = Acompute_need_unnesting (op->operations ());
  _need_unnesting.Delete (op->relref ());
				// if we are reading the extent
				// of a set_valued attribute, then
				// you should not apply unnest to it!!
  

  _numtuples = ceil ((double)r->numtuples ());
}

Alogprop_t::Alogprop_t (Amat_t *op, Alognode_t *node)
{
  node->SetLogProps (this);
  Alogprop_t *inprops = node->Input ()->GetLogProps ();

  outputop_applied = 0;

  _index_path = 0;
  if (Aglob_vars()->oopt->do_path_indexes &&
      inprops->_index_path)
				// no point if this is an Amat_collapse
    {
      Atupleref_t *tupleref = (Atupleref_t *)op->tupleref ()->func ();
      const char *last_component = tupleref->component ();
      assert (last_component);
      _index_path = new char [strlen (inprops->_index_path) +
                              strlen (last_component) + 2];
      assert (_index_path);
      strcpy (_index_path, inprops->_index_path);
      strcat (_index_path, ".");
      strcat (_index_path, last_component);
      if (!Aglob_vars()->cat->matches_index (_index_path))
	{
	  delete _index_path; _index_path = 0;
	}
    }


  _operations = inprops->_operations;
  _operations.Union (op->operations ()); 

  _need_unnesting = inprops->_need_unnesting;
  _need_unnesting.Union (Acompute_need_unnesting (op->operations ()));

  if (Aglob_vars()->oopt->do_mat_collapse)
    _is_interesting = 1;	// all mats are intersting because of collapse.
  else
    _is_interesting = _index_path ? 1 : 0;

  _numtuples = inprops->_numtuples;
}

Alogprop_t::Alogprop_t (Amat_collapse_t *op, Alognode_t *node)
{
  node->SetLogProps (this);

  assert (Aglob_vars()->oopt->do_mat_collapse);
  
  *this = *op->matnode ()->GetLogProps ();
  _is_interesting = 0;		// not intersting!
  _index_path = 0;		
}

Alogprop_t::Alogprop_t (Aselect_t *op, Alognode_t *node)
{
  node->SetLogProps (this);
  Alogprop_t *inprops = node->Input ()->GetLogProps ();

  _operations = inprops->_operations;
  _operations.Union (op->operations ()); 

  _need_unnesting = inprops->_need_unnesting;
  _need_unnesting.Union (Acompute_need_unnesting (op->operations ()));

  outputop_applied = 0;
  _index_path = 0;
  assert (op->operations ().Cardinality () == 1);
  Aptree_t *oprn = op->operations ().FirstElement ();
  if (inprops->_index_path)
    {
      if (oprn->is_relop () && oprn->arg (0)->is_attref () &&
	  oprn->arg (1)->is_literal ())
	{
	  const char *attrname = 
	    ((Aattref_t *)oprn->arg (0)->func ())->attr ()->name ();
	  _index_path = new char [strlen (inprops->_index_path) +
	                          strlen (attrname) + 2];
	  assert (_index_path);
	  strcpy (_index_path, inprops->_index_path);
	  strcat (_index_path, ".");
	  strcat (_index_path, attrname);
	  if (!Aglob_vars()->cat->matches_index (_index_path))
	    {
	      delete _index_path; _index_path = 0;
	    }
	}
    }

  _is_interesting = 0;
  if (Aglob_vars()->oopt->do_select_collapse && op->multi_select ())
    _is_interesting = 1;

  if (_index_path)
    _is_interesting = 1;

  if (Aglob_vars()->oopt->do_select_collapse &&
      node->Input ()->GetOp ()->GetNumber () == Aselect)
    {
      Aselect_t *input_select = (Aselect_t *)node->Input ()->GetOp ();
      Aptree_t *input_select_oprn = 
	input_select->operations ().FirstElement ();
      if (oprn->Number () < input_select_oprn->Number ())
	_is_interesting = 1;	// we do select collapses only if the
				// select operations in the chain are
				// in increasing order. this prevents
				// duplicate collapses...
    }

  double selectivity = oprn->selectivity (node);
  _numtuples = ceil (inprops->_numtuples *selectivity);
}

Alogprop_t::Alogprop_t (Aselect_collapse_t *op, Alognode_t *node)
{
  node->SetLogProps (this);
  
  assert (Aglob_vars()->oopt->do_select_collapse);

  *this = *op->select_node ()->GetLogProps ();
  _is_interesting = 0;		// not intersting!
  _index_path = 0;		
}

Alogprop_t::Alogprop_t (Aidx_collapse_t *op, Alognode_t *node)
{
  node->SetLogProps (this);
  _operations = op->operations ();
  _index_path = 0;

  _is_interesting = 0;
  Alogprop_t *select_props = op->select_node ()->GetLogProps ();

  // input sizes remain unchanged.
  _numtuples = select_props->_numtuples;
}

Alogprop_t::Alogprop_t (Ajoin_t *op, Alognode_t *node)
{
  node->SetLogProps (this);
  Alogprop_t *leftprops = node->LeftInput ()->GetLogProps ();
  Alogprop_t *rightprops = node->RightInput ()->GetLogProps ();
  
  outputop_applied = 0;

  
  _index_path = 0;
  _is_interesting = 0;

  _operations = leftprops->_operations;
  _operations.Union (rightprops->_operations);
  _operations.Union (op->operations ());

  _need_unnesting = leftprops->_need_unnesting;
  _need_unnesting.Union (rightprops->_need_unnesting);
  _need_unnesting.Union (Acompute_need_unnesting (op->operations ()));

  _undone_tuplerefs = leftprops->_undone_tuplerefs;
  _undone_tuplerefs.Union (rightprops->_undone_tuplerefs);
  _undone_tuplerefs.Union (op->undone_tuplerefs ());
  _undone_tuplerefs.Minus (op->operations ());
				// undone = stuff left undone by left +
				//          stuff left undone by right +
				//          stuff left undone in this join -
				//          stuff done by this join.


  assert (op->operations ().Cardinality () == 1);
  Aptree_t *oprn = op->operations ().FirstElement ();

  double selectivity = oprn->selectivity (node);
  _numtuples = 
    ceil (leftprops->_numtuples * rightprops->_numtuples * selectivity);
}

Alogprop_t::Alogprop_t (Aunnestop_t *op, Alognode_t *node)
{
  node->SetLogProps (this);
  *this = *node->Input ()->GetLogProps ();
  _need_unnesting.Minus (op->operations ());
}

Alogprop_t::Alogprop_t (Aoutputop_t *op, Alognode_t *node)
{
  node->SetLogProps (this);
  *this = *node->Input ()->GetLogProps ();
  _index_path = 0;
  _is_interesting = 0;
  outputop_applied = 1;
}


int Alogprop_t::IsEqualTo (const Alogprop_t *other) const
{
  if (!_operations.IsEqualTo (other->_operations)) return 0;
  if (!_undone_tuplerefs.IsEqualTo (other->_undone_tuplerefs)) return 0;
  if (!_need_unnesting.IsEqualTo (other->_need_unnesting)) return 0;
//  if (!outputop_applied == other->outputop_applied) return 0;
  return 1;
}

int Alogprop_t::Hash (void) const
{
  int hashval = 0;
  
  hashval += _operations.Hash ();
  hashval += _undone_tuplerefs.Hash ();
  hashval += _need_unnesting.Hash ();
//  hashval += outputop_applied ? 1 : 0;

  return hashval;
}


Aset_t<Aptree_t> Acompute_need_unnesting (Aset_t<Aptree_t> &oprns)
{
  Aset_t<Aptree_t> result;
  FOR_EACH_ELEMENT_OF_SET (oprns)
    {
      Aptree_t *oprn = oprns.CurrentElement ();
      int is_set_valued = 0;
      if (oprn->is_attref ())
	{
	  is_set_valued = 
	    ((Aattref_t *)oprn->func ())->attr ()->is_set_valued ();
	}
      else if (oprn->is_tupleref ())
	{
	  is_set_valued =
	    Aglob_vars()->cat->
	    is_set_valued (((Atupleref_t *)oprn->func ())->attrid ());
	}
      if (is_set_valued)
	result.Insert (oprn);
    }
  return result;
}

int Alogprop_t::IsInteresting (void) const
{
  int random_number = rand ();
  int random_percent = random_number % 100;
  if (random_percent < Aglob_vars()->oopt->all_operator_trees)
    return 1;
  return _is_interesting;
}


