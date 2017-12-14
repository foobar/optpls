#include <Bexecinfo.h>
#include <Aquery.h>
#include <Aptree.h>
#include <Afunc.h>
#include <Alognode.h>
#include <Aphynode.h>
#include <Alogprop.h>
#include <Aphyprop.h>
#include <Aopinc.h>


#include <strstream.h>


void Afunc_t::replace_eqattrs (const Aptree_t &ptree, 
			       const Aset_t<Aattr_t> &attrs) 
{
  for (int i=0; i<ptree.nargs(); i++)
    ptree.arg(i)->replace_eqattrs (attrs);
}

void Aattref_t::replace_eqattrs (const Aptree_t &ptree,
				 const Aset_t<Aattr_t> &attrs) 
{
  Aset_t<Aattr_t> eqattrs = Aglob_vars()->query->get_eqattrs (_attr);
  Aset_t<Aattr_t> intersect = ASetIntersect (eqattrs, attrs);

  assert (!intersect.IsEmpty ());
  _attr = intersect.FirstElement ();
				// the replace ment is complete.
}




void Afunc_t::put_suffixes (Aptree_t &ptree,
			       const Bexec_info_t &exec_info) 
{
  for (int i=0; i<ptree.nargs(); i++)
    ptree.arg(i)->put_suffixes (exec_info);
}

void Aattref_t::put_suffixes (Aptree_t &ptree,
			      const Bexec_info_t &exec_info) 
{
  ptree._func = exec_info.make_relattr (_attr);
  // no change needed for ptree._nargs and ptree._args

  delete this;
}


static void Arewrite_nlidxjoin_pred (const Apred_t &pred, 
				     const Bexec_info_t &exec_info,
				     Aptree_t &newtree)
{
//   Aindex_t *idx = (Aindex_t *)exec_info.node ()->RightInput ()->GetAlgo ();
//   Aattr_t *indexattr = idx->indexattr ();
//   // lets take a dirty short cut here.
//   if (((Anlidxjoin_t *)exec_info.algo ())->pred ()->is_equal_to (pred))
//     {				// this pred is going to be used as the
// 				// index pred. so we must make sure that
// 				// one one side of the relop is the indexattr
// 				// and the other refers only to attrs from
// 				// the outer extent...
//       Aset_t<Aattr_t> indexattr_set;
//       indexattr_set.Insert (indexattr);
//       Aset_t<Aattr_t> outer_attrs = 
// 	exec_info.node ()->LeftInput ()->GetLogProps ()->attrs ();

//       Aptree_t *leftptree = newtree.arg (0);
//       Aptree_t *rightptree = newtree.arg (1);


//       if (leftptree->is_attref ())
// 	{
// 	  Aattr_t *a = ((Aattref_t *)leftptree->func ())->attr ();
// 	  if (Aglob_vars()->query->are_attrs_equivalent (a, indexattr))
// 	    {
// 	      leftptree->replace_eqattrs (indexattr_set);
// 	      rightptree->replace_eqattrs (outer_attrs);
// 	      return;
// 	    }
// 	}

//       // if we fall through then rightptree must be the indexattribute
//       assert (rightptree->is_attref ());
//       Aattr_t *a = ((Aattref_t *)rightptree->func ())->attr ();
//       assert (Aglob_vars()->query->are_attrs_equivalent (a, indexattr));
//       rightptree->replace_eqattrs (indexattr_set);
//       leftptree->replace_eqattrs (outer_attrs);
//     }
//   else
//     newtree.replace_eqattrs (exec_info.output_attrs ());
}

Aptree_t Apred_t::rewrite (const Bexec_info_t &exec_info) const
{
  Aptree_t newtree = ptree ().copy_tree ();

//   if (exec_info.op ()->GetNumber () != Ajoin)
//     newtree.replace_eqattrs (exec_info.output_attrs ());
//   else if (type () == equijoin)
//     {
//       Aset_t<Aattr_t> left_attrs = 
// 	exec_info.node ()->LeftInput ()->GetLogProps ()->attrs ();
//       Aset_t<Aattr_t> right_attrs = 
// 	exec_info.node ()->RightInput ()->GetLogProps ()->attrs ();
//       newtree.arg (0)->replace_eqattrs (left_attrs);
//       newtree.arg (1)->replace_eqattrs (right_attrs);
//     }
//   else if (exec_info.algo ()->GetNumber () == Anlidxjoin)
//       Arewrite_nlidxjoin_pred (*this, exec_info, newtree);
//   else
//     newtree.replace_eqattrs (exec_info.output_attrs ());

//   newtree.put_suffixes (exec_info);
  return newtree;
}


void Afunc_t::generate_name (const Aptree_t &ptree,
			     char *name, int num) const
{
  ostrstream name_stream (name, A_MAXNAMELEN);
  name_stream << "no name" << '%' << num << '\0';
}

void Aattref_t::generate_name (const Aptree_t &ptree,
				char *name, int num) const
{
  ostrstream name_stream (name, A_MAXNAMELEN);
  name_stream << _attr->name () << '$' << _attr->rel ()->rel_var () 
	      << '%' << num << '\0';
}

void Af_apply_t::generate_name (const Aptree_t &ptree,
				char *gen_name, int num) const
{
  ostrstream name_stream (gen_name, A_MAXNAMELEN);
  if (Aglob_vars()->cat->is_method (funcid ()))
    if (ptree.arg (0)->func ()->type () == Afunc_t::attref)
      {
	Aattr_t *attr = ((Aattref_t *)ptree.arg (0)->func ())->attr ();
	name_stream << attr->name () << '.' 
	  << name () << '$' << attr->rel ()->rel_var () << '%' << num << '\0';
      }
    else
      name_stream << "expr" << '.' << name () << '%' << num << '\0';
  else
    name_stream << name () << '%' << num << '\0';
}
