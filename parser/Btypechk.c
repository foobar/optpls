#include "Bparsetree.h"
#include <Acat.h>

#define Berrchk(x) if ((x, Bparser_state()->error ())) return

int Bop_t::are_args_literal (Bastnode_t &node) const
{
  for (int i=0; i<node.nargs (); i++)
    if (node.arg (i).adt_val () == 0)
      return 0;			// no its not a literal.
  return 1;			// yes they are all literals.
}

void Bop_t::typechk (Bastnode_t &node)
{
  for (int i=0; i<node.nargs (); i++)
    Berrchk (node.arg (i).typechk ());
}

void Bquery_stmt_op_t::typechk (Bastnode_t &node)
{
  Berrchk (node.arg (1).typechk ()); // the from clause
  Berrchk (node.arg (0).typechk ()); // the select clause
  Berrchk (node.arg (2).typechk ()); // the where clause
  Berrchk (node.arg (3).typechk ()); // the group by clause
  Berrchk (node.arg (4).typechk ()); // the order by clause
}

void Bfrom_list_op_t::typechk (Bastnode_t &node)
{
  // first typechk the children
  Berrchk (Bop_t::typechk (node));

  // now check that no relation names are repeated.
  for (int i=0; i<node.nargs (); i++)
    for (int j=0; j<i; j++)
      if (strcmp (node.arg (i).typeinfo.rel_var,
		  node.arg (j).typeinfo.rel_var) == 0)
	Berrchk (Bparser_state()->set_error 
		 (node.arg (i).line (), 
		  "relation name/var repeated in from clause"));

  Bparser_state()->from_clause = &node;
}

void Brel_var_op_t::typechk (Bastnode_t &node)
{
  const char *relname = node.arg (0).text ();
  node.typeinfo.relid = Aglob_vars()->cat->relid (relname);
  node.typeinfo.attr_type = Aglob_vars()->cat->otypeid (node.typeinfo.relid);
  if (Acat_t::relid_is_invalid (node.typeinfo.relid))
    Berrchk (Bparser_state()->set_error 
	     (node.line (), "non existent relation in from clause"));

  const char *rel_var = node.arg (1).text ();
  strncpy (node.typeinfo.rel_var, rel_var, sizeof (node.typeinfo.rel_var));
  Bparser_state()->add_uniq_relref (&node);
}

void Brel_name_op_t::typechk (Bastnode_t &node)
{
  const char *relname = node.arg (0).text ();
  node.typeinfo.relid = Aglob_vars()->cat->relid (relname);
  node.typeinfo.attr_type = Aglob_vars()->cat->otypeid (node.typeinfo.relid);
  if (Acat_t::relid_is_invalid (node.typeinfo.relid))
    Berrchk (Bparser_state()->set_error 
	     (node.line (), "non existent relation in from clause"));

  const char *rel_var = node.arg (0).text ();
  strncpy (node.typeinfo.rel_var, rel_var, sizeof (node.typeinfo.rel_var));
  Bparser_state()->add_uniq_relref (&node);
}


void Brelop_op_t::typechk (Bastnode_t &node)
{
  Berrchk (Bop_t::typechk (node));

  if (Aglob_vars()->cat->check_relop (relop_code,
				      node.arg (0).typeinfo.attr_type,
				      node.arg (1).typeinfo.attr_type) != 0)
    Berrchk (Bparser_state()->set_error 
	     (node.line (), "Incompatible types for rel-op"));

  if (are_args_literal (node))
    ;				// compute the result

//  node.typeinfo.attr_type = Acat_t::boolean ();
}

void Bbool_expr_op_t::typechk (Bastnode_t &node)
{
  Berrchk (node.arg (0).typechk ());
 
//  if (node.typeinfo.attr_type != Acat_t::boolean ())
//    Berrchk (Bparser_state()->set_error 
//	     (node.line (), "Predicate is not boolean"));
}

void Bbetween_op_t::typechk (Bastnode_t &node)
{
  Berrchk (Bop_t::typechk (node));

  if (Aglob_vars()->cat->check_relop (Acat_t::leq (),
				      node.arg (1).typeinfo.attr_type,
				      node.arg (0).typeinfo.attr_type) != 0 ||
      Aglob_vars()->cat->check_relop (Acat_t::leq (),
				      node.arg (0).typeinfo.attr_type,
				      node.arg (2).typeinfo.attr_type) != 0)
    Berrchk (Bparser_state()->set_error 
	     (node.line (), "Incompatible types for between operator"));

  if (are_args_literal (node))
    ;				// compute the result

//  node.typeinfo.attr_type = Acat_t::boolean ();
}

void Bconst_expr_op_t::typechk (Bastnode_t &node)
{
  Berrchk (node.arg (0).typechk ());
  
  if (node.arg (0).adt_val () != 0)
    Berrchk (Bparser_state()->set_error
	     (node.arg (0).line (), "constant expression expected"));

  node.typeinfo = node.arg (0).typeinfo;
  node.set_adt_val (node.arg (0).adt_val ());
				// remember that adt_t::delete_data ()
				// is idempotent...
}

void Barith_op_t::typechk (Bastnode_t &node)
{
  Berrchk (Bop_t::typechk (node));

  int nargs = node.nargs ();
  Acat_t::attr_type_t arg_types[nargs];
  for (int i=0; i<node.nargs (); i++)
    arg_types[i] = node.arg (i).typeinfo.attr_type;


  if (Aglob_vars()->cat->check_func_call (op_str, Acat_t::bogus_adt (), 
					  node.nargs (), arg_types, 
					  node.typeinfo.funcid, 
					  node.typeinfo.attr_type) != 0)
    Berrchk (Bparser_state()->set_error
	     (node.line (), "Incompatible types for arith op"));

  if (are_args_literal (node))
    {
      Acat_t::adt_t *adt;
      Acat_t::adt_t *arg_vals[nargs];
      for (int i=0; i<nargs; i++)
	arg_vals[i] = node.arg (i).adt_val ();
      Aglob_vars()->cat->func_exec (node.typeinfo.funcid, 0, // no owner
				    adt, nargs, arg_vals);
      assert (adt);
      node.set_adt_val (adt);
    }
}

void Battr_or_rel_name_op_t::typechk (Bastnode_t &node)
{
  // we get something of the type Whatever : attr_or_rel_name (Ident)
  // where Whatever = Expr or Attref
  // we have to convert this into
  // Relref : relref ( ) or
  // Relref : reference (Relref Ident)
  // Whatever : attref (Relref Ident)


  Bastnode_t &from_clause = *Bparser_state()->from_clause;
  const char *name = node.arg (0).text ();
  int i;

  // look for relations
  int found_rel = 0;
  Acat_t::relid_t relid;
  const char *rel_var;
  for (i=0; i<from_clause.nargs (); i++)
    if (strcmp (from_clause.arg (i).typeinfo.rel_var, name) == 0)
      {
	relid = from_clause.arg (i).typeinfo.relid;
	assert (!Aglob_vars()->cat->relid_is_invalid (relid));
	rel_var = from_clause.arg (i).typeinfo.rel_var;
	found_rel++;
      }


  // look for attributes
  Acat_t::attrid_t attrid;
  int found_attr = 0;
  for (i=0; i<from_clause.nargs (); i++)
    {
      Acat_t::relid_t tmp_relid = from_clause.arg (i).typeinfo.relid;
      assert (!Aglob_vars()->cat->relid_is_invalid (tmp_relid));
      const char *tmp_rel_var = from_clause.arg (i).typeinfo.rel_var;
      Acat_t::attrid_t tmp_attrid = 
	Aglob_vars ()->cat->attrid_byrel (tmp_relid, name);
      if (!Acat_t::attrid_is_invalid (tmp_attrid))
	{
	  attrid = tmp_attrid;
	  relid = tmp_relid;
	  rel_var = tmp_rel_var;
	  found_attr++;
	}
    }
  
  int num_found = found_rel + found_attr;
  if (num_found == 0)
    {
      Berrchk (Bparser_state()->set_error
	       (node.line (), "Unknown relation/attribute name"));
    }
  else if (num_found > 1)
    {
      Berrchk (Bparser_state()->set_error
	       (node.line (), "Ambiguous relation/attribute name"));
    }

  if (found_rel > 0)
    {
      // we are converting it into a "Relref : relref ( )" node.
      node.typeinfo.relid = relid;
      node.typeinfo.attr_type = Aglob_vars()->cat->otypeid (relid);
      strncpy (node.typeinfo.rel_var, rel_var, sizeof (node.typeinfo.rel_var));
      node.delete_tree ();
      node.init (Bastnode_t::Relref, Bastnode_t::relref, 0, 0, node.line ());
      Bparser_state()->add_uniq_relref (&node);
    }
  else
    {
      Bastnode_t relref_node;
      relref_node.init (Bastnode_t::Relref, Bastnode_t::relref, 
			0, 0, node.line ());
      relref_node.typeinfo.relid = relid;
      node.typeinfo.attr_type = Aglob_vars()->cat->otypeid (relid);
      strncpy (relref_node.typeinfo.rel_var, rel_var,
	       sizeof (relref_node.typeinfo.rel_var));



      Acat_t::attr_type_t attr_type = Aglob_vars()->cat->attr_type (attrid);
      if (Aglob_vars()->cat->is_reference (attrid))
	{
	  // then we convert this node into a Relref : reference (Relref Ident)

	  node.typeinfo.attrid = attrid;
	  node.typeinfo.attr_type = attr_type;
	  node.typeinfo.relid = Aglob_vars()->cat->refrel (attrid);
	  // strncpy (node.typeinfo.rel_var, rel_var, 
	  // sizeof (node.typeinfo.rel_var));
	  // i think this is not required.

	  Bastnode_t args[2] = {relref_node, node.arg (0)};
	  node.init (Bastnode_t::Relref, Bastnode_t::reference,
		     2, args, node.line ());
	  Bparser_state()->add_uniq_relref (&node);
	}
      else
	{
	  // now we convert it into an Whatever : attref (Relref Ident) 

	  node.typeinfo.attrid = attrid;
	  node.typeinfo.attr_type = attr_type;
	  
	  Bastnode_t args[2] = {relref_node, node.arg (0)};
	  node.init (node.type (), Bastnode_t::attref, 2, args, node.line ());

	  Bparser_state()->add_uniq_attr (&node);
	}
    }
}

void Breference_op_t::typechk (Bastnode_t &node)
{
  // what we get in here is Expr : reference (Something Ident)
  // where Something is either Expr or Relref
  // if Something is Expr we reject this as "not implemented"
  // otherwise we have to convert it into
  // Relref : reference (Relref Ident) or
  // Expr : attref (Relref Ident) 

  assert (node.type () == Bastnode_t::Expr);
  Berrchk (node.arg (0).typechk ());
  const char *name = node.arg (1).text ();

  if (node.arg (0).type () == Bastnode_t::Relref)
    {
      Acat_t::attr_type_t ref_type = node.arg (0).typeinfo.attr_type;
      Acat_t::attrid_t attrid = 
	Aglob_vars()->cat->attrid_bytype (ref_type, name);
      if (Acat_t::attrid_is_invalid (attrid))
	Berrchk (Bparser_state()->set_error 
		 (node.arg (1).line (), "attribute not found"));

      Acat_t::attr_type_t attr_type = Aglob_vars()->cat->attr_type (attrid);
      if (Aglob_vars()->cat->is_reference (attrid))
	{
	  // then we convert this node into a Relref : reference (Relref Ident)

	  node.typeinfo.attrid = attrid;
	  node.typeinfo.attr_type = attr_type;
	  node.typeinfo.relid = Aglob_vars()->cat->refrel (attrid);
	  // strncpy (node.typeinfo.rel_var, rel_var, 
	  // sizeof (node.typeinfo.rel_var));
	  // i think this is not required.

	  Bastnode_t args[2] = {node.arg (0), node.arg (1)};
	  node.init (Bastnode_t::Relref, Bastnode_t::reference,
		     2, args, node.line ());
	  Bparser_state()->add_uniq_relref (&node);
	}
      else
	{
	  // now we convert it into an Expr : attref (Relref Ident) 

	  node.typeinfo.attrid = attrid;
	  node.typeinfo.attr_type = attr_type;
	  
	  Bastnode_t args[2] = {node.arg (0), node.arg (1)};
	  node.init (node.type (), Bastnode_t::attref, 2, args, node.line ());

	  Bparser_state()->add_uniq_attr (&node);
	}

      return;			// we are done
    }

  // check that for the given type of expression this kind of a reference
  // is allowed.
  assert (!"no expression references allowed");
}

void Binvoke_method_op_t::typechk (Bastnode_t &node)
{
  Berrchk (node.arg (0).typechk ()); // check the expression
  Berrchk (node.arg (2).typechk ()); // check the arglist

  int nargs = node.arg (2).nargs ();
  Acat_t::attr_type_t arg_types[nargs];

  for (int i=0; i<nargs; i++)
    arg_types[i] = node.arg (2).arg (i).typeinfo.attr_type;

  if (Aglob_vars()->cat->check_func_call (node.text (),
					  node.arg (0).typeinfo.attr_type,
					  nargs,
					  arg_types,
					  node.typeinfo.funcid,
					  node.typeinfo.attr_type) != 0)
    Berrchk (Bparser_state()->set_error 
	     (node.line (), "Function not found (argument type mismatch?)"));

  if (are_args_literal (node))
    {
      Acat_t::adt_t *adt;
      Acat_t::adt_t *owner_adt = node.arg (0).adt_val ();
      Acat_t::adt_t *arg_vals[nargs];
      for (int i=0; i<nargs; i++)
	arg_vals[i] = node.arg (2).arg (i).adt_val ();
      Aglob_vars()->cat->func_exec (node.typeinfo.funcid, owner_adt,
				    adt, nargs, arg_vals);
      assert (adt);
      node.set_adt_val (adt);
    }
}

void Bfunc_call_op_t::typechk (Bastnode_t &node)
{
  Berrchk (node.arg (1).typechk ()); // check the arglist

  int nargs = node.arg (1).nargs ();
  Acat_t::attr_type_t arg_types[nargs];

  for (int i=0; i<nargs; i++)
    arg_types[i] = node.arg (1).arg (i).typeinfo.attr_type;

  if (Aglob_vars()->cat->check_func_call (node.text (),
					  Acat_t::bogus_adt (),
					  nargs,
					  arg_types,
					  node.typeinfo.funcid,
					  node.typeinfo.attr_type) != 0)
    Berrchk (Bparser_state()->set_error 
	     (node.line (), "Function not found (argument type mismatch?)"));

  if (are_args_literal (node))
    {
      Acat_t::adt_t *adt;
      Acat_t::adt_t *arg_vals[nargs];
      for (int i=0; i<nargs; i++)
	arg_vals[i] = node.arg (1).arg (i).adt_val ();
      Aglob_vars()->cat->func_exec (node.typeinfo.funcid, 0, // no owner
				    adt, nargs, arg_vals);
      assert (adt);
      node.set_adt_val (adt);
    }
  
}

void Battref_op_t::typechk (Bastnode_t &node)
{
  // what we get in here will be Attref : attref (Relref Ident)
  // we might leave it as it is
  // or we might convert it to Relref : reference (Relref Ident) 

  Berrchk (node.arg (0).typechk ());
  const char *name = node.arg (1).text ();

  assert (node.arg (0).type () == Bastnode_t::Relref);


  Acat_t::attr_type_t ref_type = node.arg (0).typeinfo.attr_type;
  // const char *rel_var = node.arg (0).typeinfo.rel_var;
  Acat_t::attrid_t attrid = Aglob_vars()->cat->attrid_bytype (ref_type, name);
  
  if (Acat_t::attrid_is_invalid (attrid))
    Berrchk (Bparser_state()->set_error 
	     (node.arg (1).line (), "attribute not found"));

  Acat_t::attr_type_t attr_type = Aglob_vars()->cat->attr_type (attrid);
  if (Aglob_vars()->cat->is_reference (attrid))
    {
      // then we have to convert this to a Relref : reference (Relref Ident)

      node.typeinfo.attrid = attrid;
      node.typeinfo.attr_type = attr_type;
      node.typeinfo.relid = Aglob_vars()->cat->refrel (attrid);

      Bastnode_t args[2] = {node.arg (0), node.arg (1)};
      node.init (Bastnode_t::Relref, Bastnode_t::reference, 
		 2, args, node.line ());
      Bparser_state()->add_uniq_relref (&node);
    }
  else
    {
      assert (node.opcode () == Bastnode_t::attref);
      // it is ok. no problems.

      node.typeinfo.attrid = attrid;
      node.typeinfo.attr_type = attr_type;

      Bparser_state()->add_uniq_attr (&node);
    }
}

void Boid_op_t::typechk (Bastnode_t &node)
{
  Bastnode_t &from_clause = *Bparser_state()->from_clause;
  if (from_clause.nargs () != 1)
    Berrchk (Bparser_state()->set_error 
	     (node.line (), "ambiguous oid reference"));

  node.typeinfo.relid = from_clause.arg (0).typeinfo.relid;
  strncpy (node.typeinfo.rel_var, 
	   from_clause.arg (0).typeinfo.rel_var,
	   sizeof (node.typeinfo.rel_var));
}

void Brel_oid_op_t::typechk (Bastnode_t &node)
{
  // we are still not supporting c1.polygon.oid

  const char *rel_var = node.arg (0).text ();
  Bastnode_t &from_clause = *Bparser_state()->from_clause;
  for (int i=0; i<from_clause.nargs (); i++)
    if (strcmp (from_clause.arg (i).arg (1).text (), 
		rel_var) == 0)
      {
	node.typeinfo.relid = from_clause.arg (i).typeinfo.relid;
	strncpy (node.typeinfo.rel_var, 
		 from_clause.arg (i).typeinfo.rel_var,
		 sizeof (node.typeinfo.rel_var));
	return;
      }

  Berrchk (Bparser_state()->set_error 
	   (node.line (), "unknown rel in oid reference"));
}

void Blitval_op_t::typechk (Bastnode_t &node)
{
  Acat_t::attr_type_t attr_type = 
    Aglob_vars()->cat->str_to_attr_type (opname);
  Acat_t::adt_t *adt_val = Aglob_vars()->cat->adt_instance (attr_type);
  adt_val->ascii_to_mem (node.text ());
  node.set_adt_val (adt_val);

  node.typeinfo.attr_type = attr_type;
}

void Bcreate_idx_op_t::typechk (Bastnode_t &node)
{
  const char *rel_name = node.arg (1).text ();
  Acat_t::relid_t relid = Aglob_vars()->cat->relid (rel_name);
  if (Acat_t::relid_is_invalid (relid))
    Berrchk (Bparser_state()->set_error 
	     (node.arg (1).line (), "no such relation"));
  if (node.arg (2).nargs () != 1)
    Berrchk (Bparser_state()->set_error 
	     (node.arg (2).line (), "index must be on (only) one attribute"));

  const char *attr_name = node.arg (2).arg (0).text ();
  Acat_t::attrid_t attrid = Aglob_vars()->cat->attrid_byrel (relid, attr_name);
  if (Acat_t::attrid_is_invalid (attrid))
    Berrchk (Bparser_state()->set_error 
	     (node.arg (2).line (), "no such attribute"));
}

void Bcreate_table_op_t::typechk (Bastnode_t &node)
{
  const char *rel_name = node.arg (0).text ();
  
  Acat_t::relid_t relid = Aglob_vars()->cat->relid (rel_name);
  if (!Acat_t::relid_is_invalid (relid))
    Berrchk (Bparser_state()->set_error 
	     (node.arg (0).line (), "relation already exists"));

  if (node.arg (1).nargs () <= 0)
    Berrchk (Bparser_state()->set_error 
	     (node.arg (0).line (), "relation with 0 attributes???"));
}

void Bload_table_op_t::typechk (Bastnode_t &node)
{
  const char *rel_name = node.arg (0).text ();
  
  Acat_t::relid_t relid = Aglob_vars()->cat->relid (rel_name);
  if (Acat_t::relid_is_invalid (relid))
    Berrchk (Bparser_state()->set_error 
	     (node.arg (0).line (), "no such relation"));

  if (node.arg (1).nargs () <= 0)
    Berrchk (Bparser_state()->set_error 
	     (node.arg (0).line (), "0 load files??"));
}

void Bdrop_table_op_t::typechk (Bastnode_t &node)
{
  const char *rel_name = node.arg (0).text ();
  
  Acat_t::relid_t relid = Aglob_vars()->cat->relid (rel_name);
  if (Acat_t::relid_is_invalid (relid))
    Berrchk (Bparser_state()->set_error 
	     (node.arg (0).line (), "no such relation"));
}

void Bcreate_db_op_t::typechk (Bastnode_t &node)
{
}

void Bopen_db_op_t::typechk (Bastnode_t &node)
{
}

void Bclose_db_op_t::typechk (Bastnode_t &node)
{
}
