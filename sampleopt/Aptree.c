#include "Aptree.h"
#include "Afunc.h"

Aptree_t Aptree_t::_null_pred_tree = Aptree_t::make_null_pred_tree ();

void Aptree_t::init (Afunc_t *f, int n, const Aptree_t *a)
{
  // assume that user is in charge of deleting the 
  // old _func and _args.

  _func = f;
  assert ((_nargs = n) >= 0);
  if (_nargs > 0)
    {
      assert (_args = new Aptree_t [_nargs]);
      for (int i=0; i<_nargs; i++)
	new_arg (i, a[i]);
    }
}

void Aptree_t::delete_tree (void)
{
  delete _func;
  for (int i=0; i<_nargs; i++)
    _args[i].delete_tree ();
}

int Aptree_t::is_equijoin (Aattr_t *&a1, Aattr_t *&a2) const
{
  if (_func->type () == Afunc_t::eq &&
      _args[0]._func->type () == Afunc_t::attref &&
      _args[1]._func->type () == Afunc_t::attref)
    {
      a1 = ((Aattref_t *)_args[0]._func)->attr ();
      a2 = ((Aattref_t *)_args[1]._func)->attr ();
      return 1;			// yes it is an equijoin.
    }
  return 0;			// no its not an equijoin
}

int Aptree_t::TotalNumber (void)
{
  return Aglob_vars()->query->numoperations ();
}

Aptree_t *Aptree_t::NthElement (int N)
{
  return Aglob_vars()->query->operation (N);
}

// check for equality.
int Aptree_t::is_equal_to (const Aptree_t &p) const
{
  if (!func ()->is_equal_to (*p.func ()))
    return 0;
  
  if (Number () > 0 && p.Number () > 0)
    return Number () == p.Number ();

  if (nargs () != p.nargs ())
    return 0;			// not equal!

  for (int i=0; i<nargs (); i++)
    if (!arg (i)->is_equal_to (*p.arg (i)))
      return 0;			// not equal!

  return 1;			// the funcs are equal and the args are equal.
}

void Aptree_t::insert_unique_operation (Alist_t<Aptree_t> &op_list) 
{
  int sizeoflist = 0;		// if we fall through to the end of this
				// function, where we actually insert the
				// ptree in the list, then sizeoflist will
				// contain the number of elements in the list
				// and will thus indicate the number to give
				// the list.
  FOR_EACH_ELEMENT_OF_LIST (op_list)
    {
      sizeoflist++;
      Aptree_t *p = op_list.Element ();
      if (is_equal_to (*p))
	{
	  _number  = p->Number ();
	  return;		// its already there. no need to insert.
	}
    }
  
  _number = sizeoflist;
  op_list.InsertAtEnd (this);
}


void Aptree_t::make_operations_list (Alist_t<Aptree_t> &op_list) 
{
  if (is_literal ())
    {
      _number = 0;
      return;			// literals are not operations.
				// hence they are not inserted.
    }

  for (int i=0; i<nargs (); i++)
    arg (i)->make_operations_list (op_list);

  insert_unique_operation (op_list);
}
