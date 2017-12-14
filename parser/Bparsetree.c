#include "Bparsetree.h"
#include <assert.h>
#include <strstream.h>

void Bastnode_t::init (type_t t, opcode_t o, int n, Bastnode_t *a, int l)
{
  _type = t;
  _opcode = o;
  _line = l;
  
  _nargs = n;
  assert (nargs () >= 0);
  if (nargs () > 0)
    {
      _args = new Bastnode_t [nargs ()];
      assert (_args);
    }
  else
    _args = 0;

  for (int i=0; i<nargs (); i++)
    _args[i] = a[i];

  _adt_val = 0;
}


void Bastnode_t::delete_tree (void)
{
  for (int i=0; i<nargs (); i++)
    arg (i).delete_tree ();
  
  delete [] _args;
  _nargs = 0; _args = 0;

  delete _adt_val;
  _adt_val = 0;
}

Bastnode_t &Bastnode_t::flatten (void)
{
  // flatten a left-deep tree into a list.

  // first count the number of children.
  int num_child = 0;
  for (Bastnode_t *tmp = this; tmp->nargs () == 2; tmp = &tmp->arg (0))
    num_child++;
  
  assert (tmp->nargs () == 1);	// the leaf node.
  num_child++;

  Bastnode_t *new_args = new Bastnode_t[num_child];
  assert (new_args);
  
  int i = num_child - 1;	// we need to preserve the order of the list
				// hence i goes from num_child-1 to 0.
  Bastnode_t *cur_args = _args;
  int cur_nargs = _nargs;
  while (cur_nargs == 2)
    {
      new_args[i] = cur_args[1];	

      // deleting stuff which is no longer needed.
      Bastnode_t *to_be_deleted = cur_args; 
      cur_args = to_be_deleted[0]._args;
      cur_nargs = to_be_deleted[0]._nargs;
      delete [] to_be_deleted;
      i--;
    }
  
  assert (cur_nargs == 1 && i == 0);
  new_args[i] = cur_args[0];
  delete [] cur_args;

  _args = new_args;
  _nargs = num_child;

  return *this;
}


Bparser_state_t _Bparser_state;
void Bparser_state_t::set_error (int line_no, char *msg)
{
  _error = 1;
  
  ostrstream error_stream (_error_str, sizeof (_error_str));
  error_stream << "Error: line " << line_no << ", " << msg << '\0';
}

void Bparser_state_t::add_uniq_attr (Bastnode_t *attr)
{
  assert (attr->opcode () == Bastnode_t::attref);
  char path1[A_MAXPATHLEN];
  attr->arg (0).get_rel_var (path1);

  FOR_EACH_ELEMENT_OF_LIST (attrs)
   {
      const char *attrname1 = 
	Aglob_vars()->cat->attrname (attrs.Element ()->typeinfo.attrid);
      const char *attrname2 =
	Aglob_vars()->cat->attrname (attr->typeinfo.attrid);

      if (strcmp (attrname1, attrname2) == 0)
	{
	  char path2[A_MAXPATHLEN];
	  attrs.Element ()->arg (0).get_rel_var (path2);

	  if (strcmp (path1, path2) == 0)
	    return;			// because its already there
	}
   }

  attrs.Insert (attr);
}

void Bparser_state_t::add_uniq_relref (Bastnode_t *relref)
{
  assert (relref->type () == Bastnode_t::Relref ||
	  relref->type () == Bastnode_t::RelName);
  
  char path1[A_MAXPATHLEN];
  char path2[A_MAXPATHLEN];
  relref->get_rel_var (path1);

  FOR_EACH_ELEMENT_OF_LIST (relrefs)
   {
     relrefs.Element ()->get_rel_var (path2);
     if (strcmp (path1, path2) == 0)
       return;			// because its already there
   }

  relrefs.Insert (relref);
}
