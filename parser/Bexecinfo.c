#include <Bexecinfo.h>
#include <Alognode.h>
#include <Aphynode.h>
#include <Alogprop.h>
#include <Aphyprop.h>
#include <Aopinc.h>

#include <Bbogus.h>

#include <iostream.h>
#include <strstream.h>
#include <Bprojlist.h>




Bexec_info_t::Bexec_info_t (void)
{
  _node = 0; _indent = 0; _topnode = 0;
  _res_filename[0] = '\0'; _error = 0;
  _num_inputs = 0; _inputs = 0;
  _algo = 0; _op = 0;
  _tmp_flag = 0;
}

Bexec_info_t::~Bexec_info_t (void)
{
  assert (_inputs == 0);	// should be deleted by parent when
				// it no longer needs them.
  if (_tmp_flag)			
    {
      // Aglob_vars()->cat->drop_rel (_res_filename);
      if (Aglob_vars()->print_plan)
	cout << "deleted file " << _res_filename << endl;
    }
}

void Bexec_info_t::init (Aphynode_t *n, Aset_t<Aattr_t> &pattrs, int t, int i)
{
  assert (_node = n);
  _algo = _node->GetAlgo ();
  _op = _node->GetParent ()->GetOp ();
  _indent = i;
  _proj_attrs = pattrs;
  _topnode = t;

  _suffix[0] = '\0';
//  if (_op->GetNumber () == Adbrel) // then it might need a suffix
//    add_suffix (_suffix, ((Adbrel_t *)_op)->rel ());

  _num_inputs = algo ()->Arity ();
  if (_num_inputs > 0)
    _inputs = new Bexec_info_t[_num_inputs];
}

void Aget_tmp_filename (char *filename, int max_chars)
{
  ostrstream ostr (filename, max_chars);
  ostr << "tmp" << Aglob_vars()->filename_counter++ << '\0';
  return;
}

void Bexec_info_t::output_in_tmp_file (void)
{
  Aget_tmp_filename (_res_filename, sizeof (_res_filename));
  
  if (!_topnode)
    _tmp_flag = 1;		// this is a temporary file. hence, if
				// this operator is not the last operator
				// to be applied then this file is deletable.
}

Aptree_t Bexec_info_t::set_to_ptree (const Aset_t<Apred_t> &c_preds)
{
  Aset_t<Apred_t> preds = c_preds;
				// just to make sure that we dont
				// clobber c_preds.
  Aptree_t ptree;

  // take care of empty sets
  if (preds.IsEmpty ())
    {
      Afunc_t *nullfunc = new Afunc_t (Afunc_t::empty);
      assert (nullfunc);
      ptree.init (nullfunc);	// the empty ptree.
      return ptree;
    }

  // take care of singleton sets.
  Apred_t *first_pred = preds.FirstElement ();
  preds.Delete (first_pred);
  ptree = first_pred->rewrite (*this);

  // now take care of the rest of the set.
  FOR_EACH_ELEMENT_OF_SET (preds)
    {
      Aptree_t left_ptree = ptree;
      Aptree_t right_ptree = preds.CurrentElement ()->rewrite (*this);
      Aptree_t args[2] = {left_ptree, right_ptree};
      Aboolfunc_t *and_func = new Aboolfunc_t (Afunc_t::and);
      assert (and_func);
      ptree.init (and_func, 2, args);
    }

  return ptree;  
}

Arelattr_t *Bexec_info_t::make_relattr (const Aattr_t *a) const
{
  // search for this attribute in the inputs
//   for (int i=0; i<num_inputs (); i++)
//     {
//       Aset_t<Aattr_t> attrs = input (i).output_attrs ();
//       if (attrs.HasElement (a)) // we found the attribute
// 	{
// 	  char tmp_relname[A_MAXNAMELEN];
// 	  strncpy (tmp_relname, input (i).res_filename (),
// 		   sizeof (tmp_relname));
	  
// 	  const char *actual_relname = a->rel ()->name ();

// 	  char attrname[A_MAXNAMELEN];
// 	  strncpy (attrname, a->name (), sizeof (attrname));
// 	  char suffix[A_MAXNAMELEN];
// 	  suffix[0] = '\0';

// 	  if (strcmp (actual_relname, tmp_relname) != 0)
// 	    add_suffix (attrname, a->rel ());
// 	  else
// 	    add_suffix (suffix, a->rel ());

// 	  Arelattr_t *relattr = new Arelattr_t (tmp_relname, attrname, suffix);
// 	  assert (relattr);
// 	  return relattr;
// 	}
//     }
//   assert (!"attr not found in any input");
  return 0;			// to keep the compiler happy.
}


void Bexec_info_t::make_projlist (ProjList &projlist) const
{
//   Aset_t<Aattr_t> out_attrs = output_attrs ();
//   Aset_t<Aattr_t> pattrs = _proj_attrs;
// 				// to avoid problems with "const"
//   if (_topnode)
//     {
//       if (Aglob_vars()->query->is_select_star ())
// 	{
// 	  int numrels = Aglob_vars()->query->numrels ();
	  
// 	  int projlist_size = 0;
// 	  for (int i=0; i<numrels; i++)
// 	    projlist_size += Aglob_vars()->query-> rel (i)->numattrs ();
	  
// 	  projlist.initialize (projlist_size);
	  
// 	  for (i=0; i<numrels; i++)
// 	    {
// 	      Arel_t *rel = Aglob_vars()->query->rel (i);
// 	      const char *rel_var = rel->rel_var ();
// 	      int numattrs = rel->numattrs ();
// 	      for (int j=0; j<numattrs; j++)
// 		{
// 		  char attrname[A_MAXNAMELEN];
// 		  rel->nth_attrname (j, attrname);
// 		  Aattr_t *attr = Aglob_vars()->query->attr (rel_var,
// 							     attrname);
// 		  Arelattr_t *relattr = make_relattr (attr);
// 		  Aptree_t *ptree = new Aptree_t; 
// 		  assert (ptree); // will be deleted by ~projlist
// 		  ptree->init (relattr);
		  
// 		  char out_attrname[A_MAXNAMELEN];
// 		  strncpy (out_attrname, attr->name (), sizeof (out_attrname));
// 		  add_suffix (out_attrname, rel);

// 		  projlist.put (ptree, out_attrname);
// 		}
// 	    }
// 	}
//       else
// 	{

// 	  Alist_t<Aptree_t> target_list = Aglob_vars()->query->target_list ();
// 	  projlist.initialize (target_list.Length ());
// 	  int target_num=0;
// 	  FOR_EACH_ELEMENT_OF_LIST (target_list)
// 	    {
// 	      Aptree_t *orig_ptree = target_list.Element ();
// 	      Aptree_t *ptree = new Aptree_t;
// 	      assert (ptree);
// 	      *ptree = orig_ptree->copy_tree ();
// 	      ptree->replace_eqattrs (out_attrs);
// 	      ptree->put_suffixes (*this);

// 	      char out_attrname[A_MAXNAMELEN];
// 	      orig_ptree->generate_name (out_attrname, target_num);
	  
// 	      projlist.put (ptree, out_attrname);
// 	      target_num++;
// 	    }
// 	}
//     }
//   else
//     if (Aglob_vars()->query->is_select_star ())
//       projlist.initialize (0);	// null projlist.
//     else
//       {
// 	projlist.initialize (pattrs.Cardinality ());
// 	FOR_EACH_ELEMENT_OF_SET (pattrs)
// 	  {
// 	    Aattr_t *a = pattrs.CurrentElement ();
// 	    Aset_t<Aattr_t> eqattrs = Aglob_vars()->query->get_eqattrs (a);
// 	    Aset_t<Aattr_t> intersect = ASetIntersect (eqattrs, out_attrs);
// 	    assert (!intersect.IsEmpty ());
// 	    Aattr_t *real_attr = intersect.FirstElement ();

// 	    Arelattr_t *relattr = make_relattr (real_attr);
// 	    Aptree_t *ptree = new Aptree_t; // will be deleted by ~projlist.
// 	    assert (ptree);
// 	    ptree->init (relattr);

// 	    char out_attrname[A_MAXNAMELEN];
// 	    strncpy (out_attrname, real_attr->name (), sizeof (out_attrname));
// 	    add_suffix (out_attrname, real_attr->rel ());
	
// 	    projlist.put (ptree, out_attrname);
// 	  }
//       }
}

void 
Bexec_info_t::compute_inputs_for_select_operator (void)
{
  // Aset_t<Apred_t> preds = ((Aselect_t *)_op)->preds ();

  // attributes required from input := attributes needed to to be output
  //                                  +attributes needed for select predicates

  Aset_t<Aattr_t> inproj; 
//  Aset_t<Aattr_t> inproj = 
//    ASetUnion (_proj_attrs,
//	       ASetIntersect (Aget_eq_closure (preds), output_attrs ()));

  _inputs[0].init (node ()->Input (), inproj, 0, _indent + 2);
  _inputs[0].execute ();	// compute the inputs.

  // check for error.
  if (_inputs[0].error ())
    _error = _inputs[0]._error;
}

void
Bexec_info_t::compute_inputs_for_join_operator (void)
{
//  Aset_t<Apred_t> preds = ((Ajoin_t *)_op)->preds ();
//  Aset_t<Aattr_t> left_attrs = _node->LeftInput ()->GetLogProps ()->attrs ();
//  Aset_t<Aattr_t> right_attrs = _node->RightInput ()->GetLogProps ()->attrs ();

  Aset_t<Aattr_t> leftproj;
//  Aset_t<Aattr_t> leftproj = ASetUnion (
//    ASetIntersect (_proj_attrs, left_attrs),
//    ASetIntersect (Aget_eq_closure (preds), left_attrs));
  Aset_t<Aattr_t> rightproj;
//  Aset_t<Aattr_t> rightproj = ASetUnion (
//    ASetIntersect (_proj_attrs, right_attrs),
//    ASetIntersect (Aget_eq_closure (preds), right_attrs));

  // compute the left input
  _inputs[0].init (node ()->LeftInput (), leftproj, 0, _indent + 2);
  _inputs[0].execute ();

  if (_inputs[0].error ())
    {
      _error = _inputs[0]._error;
      return;
    }

  // compute the right input.
  _inputs[1].init (node ()->RightInput (), rightproj, 0, _indent + 2);
  _inputs[1].execute ();

  if (_inputs[1].error ())
    _error = _inputs[1]._error;
}

void 
Bexec_info_t::compute_inputs_for_simple_operator (void)
{
  int top = _algo->GetNumber () == Aoutputalgo ? 1 : 0;
    
  _inputs[0].init (node ()->Input (), _proj_attrs, top, _indent + 2);
  _inputs[0].execute ();	// compute the inputs.

  // check for error.
  if (_inputs[0].error ())
    _error = _inputs[0]._error;
}

void Bexecute_plan (Aphynode_t *node)
{
  Aset_t<Aattr_t> projects;
  // Aset_t<Aattr_t> projects = Aglob_vars()->query->target_attrs ();
  Bexec_info_t exec_info;
  exec_info.init (node, projects, 1, 0);
  exec_info.execute ();
  exec_info.delete_inputs ();
}
