#include <Bparsetree.h>
#include <Bopcodes.h>
#include <Aquery.h>

void Bop_t::pred_to_list (Bastnode_t &node, Alist_t<Bastnode_t> &list)
{
  list.Insert (&node);
}

void Band_op_t::pred_to_list (Bastnode_t &node, Alist_t<Bastnode_t> &list)
{
  for (int i=0; i<node.nargs(); i++)
    node.arg (i).pred_to_list (list);
}

void Bquery_stmt_op_t::convert (Bastnode_t &node, Aquery_t &query)
{
  Bastnode_t &select_clause = node.arg (0);
  Bastnode_t &from_clause = node.arg (1);
  Bastnode_t &where_clause = node.arg (2);
  Bastnode_t &groupby_clause = node.arg (3);
  Bastnode_t &orderby_clause = node.arg (4);


  // first the from clause
  int i;
  Alist_t<Bastnode_t> relref_list = Bparser_state()->relref_list ();
  query._numrels = relref_list.Length ();
  assert (query._numrels > 0);
  query._rels = new Aquery_t::rel_t[query._numrels];
  assert (query._rels);
  i = 0;
  FOR_EACH_ELEMENT_OF_LIST (relref_list)
    {
      Bastnode_t *relref = relref_list.Element ();
      Acat_t::relid_t relid = relref->typeinfo.relid;
      Acat_t::attr_type_t otypeid = relref->typeinfo.attr_type;
      char rel_var[A_MAXPATHLEN];
      relref->get_rel_var (rel_var);
      int is_real_relation = 
	relref->type () == Bastnode_t::RelName ? 1 : 0;
      query._rels[i].rel.init (i, relid, otypeid, rel_var, is_real_relation);
      i++;
    }

  // the target list
  if (select_clause.opcode () == Bastnode_t::select_star)
    query._is_select_star = 1;
  else
    {
      Bastnode_t &target_list = select_clause.arg (0);
      for (i=0; i<target_list.nargs (); i++)
	{
	  Aptree_t *ptree = new Aptree_t;
	  assert (ptree);
	  *ptree = target_list.arg (i).ast_to_ptree ();
	  query._target_list.InsertAtEnd (ptree);
	}
    }

  // the set of predicates
  if (where_clause.opcode () == Bastnode_t::empty)
    query._numpreds = 0;
  else
    {
      Alist_t<Bastnode_t> pred_list;
      where_clause.arg (0).pred_to_list (pred_list);
      query._numpreds = pred_list.Length ();
      assert (query._numpreds > 0);
      query._preds = new Apred_t[query._numpreds];
      assert (query._preds);
      i = 0;
      FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (pred_list)
				// check if this is right: Navin.
	{
	  Aptree_t ptree = pred_list.Element ()->ast_to_ptree ();
	  query._preds[i].init (i, ptree);
	  i++;
	}
    }
  
  if (query._is_select_star)
    {
      query._numattrs = 0;	// initialize
      for (int i=0; i<query._numrels; i++)
	query._numattrs += query.rel (i)->numattrs ();

      query._attrs = new Aquery_t::attr_t[query._numattrs];
      assert (query._attrs);
      int attrnum = 0;
      for (i=0; i<query._numrels; i++)
	{
	  Arel_t *rel = query.rel (i);
	  int num_relattrs = rel->numattrs ();
	  for (int j=0; j<num_relattrs; j++)
	    {
	      char attrname[A_MAXNAMELEN];
	      rel->nth_attrname (j, attrname);
	      Acat_t::attrid_t attrid = 
		Aglob_vars()->cat->attrid_byrel (rel->relid (), attrname);
	      assert (!Aglob_vars()->cat->attrid_is_invalid (attrid));
	      query._attrs[attrnum].attr.init (attrnum, rel, attrid);
	      attrnum++;
	    }
	}
    }
  else
    {
      Alist_t<Bastnode_t> attr_list = Bparser_state()->attr_list ();
      query._numattrs = attr_list.Length ();
      if (query._numattrs > 0)
	{
	  query._attrs = new Aquery_t::attr_t[query._numattrs];
	  assert (query._attrs);
	  i = 0;
	  FOR_EACH_ELEMENT_OF_LIST (attr_list)
	    {
	      Bastnode_t &attref_node = *attr_list.Element ();
	      Acat_t::attrid_t attrid = attref_node.typeinfo.attrid;
	      char rel_var[A_MAXPATHLEN];
	      attref_node.arg (0).get_rel_var (rel_var);
	      Arel_t *relptr = query.rel (rel_var);
				// find the relation record
	      assert (relptr);	// the relation will exist.
	      query._attrs[i].attr.init (i, relptr, attrid);
	      i++;
	    }
	}
    }
  // now the group by clause
  assert (groupby_clause.nargs () <= 1); 
  if (groupby_clause.nargs () == 1)
    {
      Bastnode_t &attref_node = groupby_clause.arg (0);
      char rel_var[A_MAXPATHLEN];
      attref_node.arg (0).get_rel_var (rel_var);
      const char *attrname = 
	Aglob_vars()->cat->attrname (attref_node.typeinfo.attrid);
      Aattr_t *attrptr = query.attr (rel_var, attrname);
      assert (attrptr);
      query._groupby = attrptr;
    }

  // now the order by clause
  assert (orderby_clause.nargs () <= 1);
  if (orderby_clause.nargs () == 1)
    {
      Bastnode_t &attref_node = orderby_clause.arg (0).arg (0);
      char rel_var[A_MAXPATHLEN];
      attref_node.arg (0).get_rel_var (rel_var);
      const char *attrname = 
	Aglob_vars()->cat->attrname (attref_node.typeinfo.attrid);
      Aattr_t *attrptr = query.attr (rel_var, attrname);
      assert (attrptr);
      query._orderby = attrptr;

      query._ascending = 
	orderby_clause.arg (0).opcode () == Bastnode_t::asc ? 1 : 0;
    }
}

Aptree_t Bop_t::ast_to_ptree (Bastnode_t &node)
{
  Afunc_t *func;
  if (this == Bastnode_t::equal)
    func = new Aclose_relop_t (Afunc_t::eq);
  else if (this == Bastnode_t::not_equal)
    func = new Aclose_relop_t (Afunc_t::neq);

  else if (this == Bastnode_t::less)
    func = new Aopen_relop_t (Afunc_t::lt);
  else if (this == Bastnode_t::less_equal)
    func = new Aopen_relop_t (Afunc_t::leq);
  else if (this == Bastnode_t::greater)
    func = new Aopen_relop_t (Afunc_t::gt);
  else if (this == Bastnode_t::greater_equal)
    func = new Aopen_relop_t (Afunc_t::geq);

  else if (this == Bastnode_t::between)
    func = new Arange_t (Afunc_t::between);
  else if (this == Bastnode_t::not_between)
    func = new Arange_t (Afunc_t::not_between);

  else if (this == Bastnode_t::not)
    func = new Anot_t ();
  else if (this == Bastnode_t::and)
    func = new Aboolfunc_t (Afunc_t::and);
  else if (this == Bastnode_t::or)
    func = new Aboolfunc_t (Afunc_t::or);

  else 
    assert (!"unknown relop");

  assert (func);


  Aptree_t ptree;

  int nargs = node.nargs ();
  Aptree_t *args;
  if (nargs > 0)
    {
      Aptree_t args[nargs];
      for (int i=0; i<nargs; i++)
	args[i] = node.arg (i).ast_to_ptree ();
      
      ptree.init (func, nargs, args);
    }
  else
    ptree.init (func);

  return ptree;
}

Aptree_t Bbool_expr_op_t::ast_to_ptree (Bastnode_t &node)
{
  return node.arg (0).ast_to_ptree ();
}

Aptree_t Boid_ref_op_t::ast_to_ptree (Bastnode_t &node)
{
  const char *rel_var = node.arg (0).typeinfo.rel_var;
  Arel_t *rel = Aglob_vars()->query->rel (rel_var);
  assert (rel);

  Afunc_t *oid_ref_func = new Aoidref_t (rel, node.arg (1).text ());
  Aptree_t ptree;
  ptree.init (oid_ref_func);
  return ptree;
}

Aptree_t Bliteral_op_t::ast_to_ptree (Bastnode_t &node)
{
  return node.arg (0).ast_to_ptree ();
}

Aptree_t Blitval_op_t::ast_to_ptree (Bastnode_t &node)
{
  Acat_t::attr_type_t attr_type = 
    Aglob_vars()->cat->str_to_attr_type (opname);
  
  Afunc_t *func = new Aliteral_t (attr_type, node.adt_val ());
  assert (func);
  Aptree_t ptree;
  ptree.init (func);
  return ptree;
}

Aptree_t Brelref_op_t::ast_to_ptree (Bastnode_t &node)
{
  char path[A_MAXPATHLEN];
  node.get_rel_var (path);
  Arel_t *rel = Aglob_vars()->query->rel (path);
  assert (rel->is_real_rel ());

  Afunc_t *func = new Atuplevar_t (rel);
  assert (func);
  Aptree_t ptree;
  ptree.init (func);
  return ptree;
}

Aptree_t Breference_op_t::ast_to_ptree (Bastnode_t &node)
{
  Aptree_t relref_tree = node.arg (0).ast_to_ptree ();

  char path[A_MAXPATHLEN];
  node.get_rel_var (path);
  Arel_t *rel = Aglob_vars()->query->rel (path);
  assert (rel);

  Acat_t::attrid_t attrid = node.typeinfo.attrid;
  Afunc_t *func = new Atupleref_t (rel, attrid);
  assert (func);
  Aptree_t ptree;
  ptree.init (func, 1, &relref_tree);
  return ptree;
}

Aptree_t Battref_op_t::ast_to_ptree (Bastnode_t &node)
{
  Aptree_t relref_tree = node.arg (0).ast_to_ptree ();
  const char *attrname = node.arg (1).text ();

  Afunc_t *func = new Aattref_t (attrname);
  assert (func);
  Aptree_t ptree;
  ptree.init (func, 1, &relref_tree);
  return ptree;
}

Aptree_t Binvoke_method_op_t::ast_to_ptree (Bastnode_t &node)
{
  Afunc_t *func = new Af_apply_t (node.typeinfo.funcid,
				  node.arg (1).text ());
  assert (func);

  int nargs = node.arg (2).nargs () + 1;
  Aptree_t args[nargs];
  args[0] = node.arg (0).ast_to_ptree ();
  for (int i=1; i<nargs; i++)
    args[i] = node.arg (2).arg (i-1).ast_to_ptree ();

  Aptree_t ptree;
  ptree.init (func, nargs, args);
  return ptree;
}


Aptree_t Bfunc_call_op_t::ast_to_ptree (Bastnode_t &node)
{
  Afunc_t *func = new Af_apply_t (node.typeinfo.funcid,
				  node.arg (0).text ());
  assert (func);

  Aptree_t ptree;

  int nargs = node.arg (1).nargs ();
  if (nargs > 0)
    {
      Aptree_t args[nargs];
      for (int i=0; i<nargs; i++)
	args[i] = node.arg (1).arg (i).ast_to_ptree ();

      ptree.init (func, nargs, args);
    }
  else
    ptree.init (func);

  return ptree;
}

Aptree_t Barith_op_t::ast_to_ptree (Bastnode_t &node)
{
  Afunc_t *func = new Af_apply_t (node.typeinfo.funcid,
				  op_str);
  assert (func);

  Aptree_t ptree;
  int nargs = node.nargs ();
  Aptree_t args[nargs];
  for (int i=0; i<nargs; i++)
    args[i] = node.arg (i).ast_to_ptree ();
  
  ptree.init (func, nargs, args);
}

Aptree_t Bconst_expr_op_t::ast_to_ptree (Bastnode_t &node)
{
  assert (node.adt_val () != 0);
  Afunc_t *func = new Aliteral_t (node.typeinfo.attr_type,
				  node.adt_val ());
  assert (func);
  Aptree_t ptree;
  ptree.init (func);
  return ptree;
}

void Brelref_op_t::get_rel_var (const Bastnode_t &node, char *rel_var) const
{
  strcpy (rel_var, node.typeinfo.rel_var);
}

void Brel_var_op_t::get_rel_var (const Bastnode_t &node, char *rel_var) const
{
  strcpy (rel_var, node.typeinfo.rel_var);
}

void Brel_name_op_t::get_rel_var (const Bastnode_t &node, char *rel_var) const
{
  strcpy (rel_var, node.typeinfo.rel_var);
}

void Breference_op_t::get_rel_var (const Bastnode_t &node, char *rel_var) const
{
  assert (node.arg (0).type () == Bastnode_t::Relref &&
	  node.arg (1).type () == Bastnode_t::Ident);
  node.arg (0).get_rel_var (rel_var);
  strcat (rel_var, ".");
  strcat (rel_var, node.arg (1).text ());
  return;
}
