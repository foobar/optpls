#include <fstream.h>
#include <assert.h>

#include <Afunc.h>
#include <Aptree.h>
#include <Aquery.h>

#include <Bbogus.h>

/***************************************************************************

  bogus functions to parse Aptree_t and Afunc_t objects from
  an istream.

***************************************************************************/

static void Areadstring (istream &is, char *string, int size)
{
  const char double_quote = '\"';
  char dq;
  is.get (string, size, double_quote); 
  is.get (dq);
  assert (dq == double_quote);  // discard everything upto first double quote.

  is.get (string, size, double_quote);
  is.get (dq);
  assert (dq == double_quote);  // everything upto the next double quote
                                // is the string we are looking for.
}


void Brqg_t::parse (istream &is)
{
  char str1[A_MAXNAMELEN], str2[A_MAXNAMELEN];
  int i;

  // the set of relations in the from clause
  is >> q._numrels;
  assert (q._numrels > 0);
  q._rels = new Aquery_t::rel_t[q._numrels];
  assert (q._rels);
  for (i=0; i<q._numrels; i++)
    {
      char *relname = str1, *rel_var = str2;
      is >> ws >> relname;	// name of the relation
      is >> ws >> rel_var;	// the relation variable
      Acat_t::relid_t relid = Aglob_vars()->cat->relid (relname);
      Acat_t::attr_type_t otypeid = Aglob_vars()->cat->otypeid (relid);
      assert (!Acat_t::relid_is_invalid (relid));
      q._rels[i].rel.init (i, relid, otypeid, rel_var);
    }

  // the set of attributes referenced in the query.
  is >> q._numattrs;
  assert (q._numattrs >= 0);
  if (q._numattrs > 0)
    {
      q._attrs = new Aquery_t::attr_t[q._numattrs];
      assert (q._attrs);
      for (i=0; i<q._numattrs; i++)
	{
	  char *relname = str1, *attrname = str2;
	  is >> ws >> relname >> ws >> attrname;
	  Arel_t *relptr = q.rel (relname); 
				// find the relation record
	  assert (relptr);		// the relation should exist
	  Acat_t::attrid_t attrid = 
	    Aglob_vars()->cat->attrid_byrel (relptr->relid (), attrname);
	  assert (!Acat_t::attrid_is_invalid (attrid));
				// the attribute should exist in catalogs.
	  q._attrs[i].attr.init (i, relptr, attrid);
	}
    }

  // the set of predicates
  is >> q._numpreds;
  assert (q._numpreds >= 0);
  if (q._numpreds > 0)
    {
      q._preds = new Apred_t[q._numpreds];
      assert (q._preds);
      for (i=0; i<q._numpreds; i++)
	{
	  Aptree_t ptree;
	  is >> ptree;
	  q._preds[i].init (i, ptree);
	}
    }

  // the target list
  int num_target_items;
  is >> num_target_items;
  assert (num_target_items >= 0);
  if (num_target_items == 0)
    q._is_select_star = 1;	// assume that this means "select *..."
  else
    for (i=0; i<num_target_items; i++)
      {
	Aptree_t *ptree = new Aptree_t;
	assert (ptree);
	is >> *ptree;		// this is a target list item.
	q._target_list.InsertAtEnd (ptree);
      }


  // the group by clause
  int flag;
  is >> flag;			// says whether there is a group by clause
  if (flag) 
    {
      char *relname = str1, *attrname = str2;
      is >> ws >> relname >> ws >> attrname;
      Aattr_t *attrptr = q.attr (relname, attrname);
      assert (attrptr);
      q._groupby = attrptr;
    }

  // the order by clause
  is >> flag;			// says whether there is an order by clause
  if (flag)
    {
      char *relname = str1, *attrname = str2;
      is >> ws >> relname >> ws >> attrname;
      Aattr_t *attrptr = q.attr (relname, attrname);
      assert (attrptr);
      q._orderby = attrptr;
    }
  is >> q._ascending;
}



void Brqg_t::unparse (ostream &os) const
{
  int i;
  os << q._numrels << " ";
  for (i=0; i<q._numrels; i++)
    os << q.rel (i)->name () << " " << q.rel (i)->rel_var () << " ";
  os << '\n';

  os << q._numattrs << " ";
  for (i=0; i<q._numattrs; i++)
    os << q.attr (i)->rel ()->rel_var () << " " << q.attr (i)->name () << " ";
  os << '\n';

  os << q._numpreds << " ";
  for (i=0; i<q._numpreds; i++)
    os << q.pred (i)->ptree () << '\n';
  os << '\n';

  os << q._target_list.Length () << "\n";
  FOR_EACH_ELEMENT_OF_LIST (q._target_list)
    os << q._target_list.Element () << '\n';
 
  if (!q._groupby)
    os << "0\n";
  else 
    os << "1 " 
       << q._groupby->rel ()->rel_var () << " " 
       << q._groupby->name () << "\n";

  if (!q._orderby)
    os << "0\n";
  else
    os << "1 " 
       << q._orderby->rel ()->rel_var () << " " 
       << q._orderby->name () << "\n";
}

void Aptree_t::read (istream &is)
{
  delete _func; delete [] _args;

  is >> _func;
  _func->read_args (is, _nargs, _args);
}

void write_func (ostream &os, const Afunc_t *func, const Aptree_t &ptree);

void Aptree_t::write (ostream &os) const
{
  write_func (os, _func, *this);
  // os << _func << " ";
  // _func->write_args (os, *this);
}

void Afunc_t::read_args (istream &is, int &nargs, Aptree_t *&args)
{
  is >> nargs;
  args = nargs > 0 ? new Aptree_t[nargs] : 0;
  assert (args);
  
  for (int i=0; i<nargs; i++)
    is >> args[i];
}

void Afunc_t::write_args (ostream &os, const Aptree_t &ptree) const
{
  os << ptree._nargs << " ";
  for (int i=0; i<ptree._nargs; i++)
    os << ptree._args[i] << " ";
}

void Arange_t::read_args (istream &is, int &nargs, Aptree_t *&args)
{
  nargs = 3;
  args = new Aptree_t[nargs];
  assert (args);
  is >> args[0];
  is >> args[1];
  is >> args[2];
}

void Arange_t::write_args (ostream &os, const Aptree_t &ptree) const
{
  os  << *ptree.arg(0) << " " << *ptree.arg(1) << " " << *ptree.arg(2);
}

void Arelattr_t::read_args (istream &is, int &nargs, Aptree_t *&args)
{
  is >> ws >> _relname >> ws >> _attrname;
  _suffix[0] = '\0';		// dont read the suffix.
}

void Arelref_t::write_args (ostream &os, const Aptree_t &ptree) const
{
  os << relvar ();
}

void Arelattr_t::write_args (ostream &os, const Aptree_t &ptree) const
{
  os  << _relname << "(" << _suffix << ") " << _attrname;
}
 
void Af_apply_t::read_args (istream &is, int &nargs, Aptree_t *&args)
{
  is >> ws >> _funcid;
  Afunc_t::read_args (is, nargs, args);
}

void Af_apply_t::write_args (ostream &os, const Aptree_t &ptree) const
{
  os  << _funcid << " ";
  Afunc_t::write_args (os, ptree);
}

void Aattref_t::write_args (ostream &os, const Aptree_t &ptree) const
{
  os << *ptree.arg (0) << "." << attrname ();
}

void Aoidref_t::read_args (istream &is, int &nargs, Aptree_t *&args)
{
  char relname[A_MAXNAMELEN];
  is >> ws >> relname;
  _rel = Aglob_vars()->query->rel (relname);
  assert (_rel);
  
  Areadstring (is, _oidstr, sizeof (_oidstr));
}


void Aoidref_t::write_args (ostream &os, const Aptree_t &ptree) const
{
  os << _rel->rel_var () << " " << _oidstr;
}

void Aliteral_t::read_args (istream &is, int &nargs, Aptree_t *&args)
{
  read_value (is);
}

void Aliteral_t::read_value (istream &is)
{
  char ascii_val[A_MAXADTSTRLEN];
  Areadstring (is, ascii_val, sizeof (ascii_val));

  _adt = Aglob_vars()->cat->adt_instance (_adt_type);
//  if (_adt->ascii_to_mem (ascii_val) != 0)
//    assert (!"cannot convert string to constant of proper type");
  _adt->ascii_to_mem (ascii_val);
}

void Aliteral_t::write_args (ostream &os, const Aptree_t &ptree) const
{
  _adt->print (os);
}
istream &operator>> (istream &is, Afunc_t *&func)
{
  char name[20];
  char *str = name;

  is >> ws >> str;
  if (strcmp (str, "and") == 0)
    func = new Aboolfunc_t (Afunc_t::and);
  else if (strcmp (str, "or") == 0)
    func = new Aboolfunc_t (Afunc_t::or);

  else if (strcmp (str, "=") == 0)
    func = new Aclose_relop_t (Afunc_t::eq);
  else if (strcmp (str, "!=") == 0)
    func = new Aclose_relop_t (Afunc_t::neq);
  else if (strcmp (str, "<") == 0)
    func = new Aopen_relop_t (Afunc_t::lt);
  else if (strcmp (str, ">") == 0)
    func = new Aopen_relop_t (Afunc_t::gt);
  else if (strcmp (str, "<=") == 0)
    func = new Aopen_relop_t (Afunc_t::leq);
  else if (strcmp (str, ">=") == 0)
    func = new Aopen_relop_t (Afunc_t::geq);

  else if (strcmp (str, "between") == 0)
    func = new Arange_t (Afunc_t::between);
  else if (strcmp (str, "not_between") == 0)
    func = new Arange_t (Afunc_t::not_between);

  else if (strcmp (str, "oid") == 0)
    func = new Aoidref_t;

  else if (strcmp (str, "lit") == 0)
    {
      Acat_t::attr_type_t adt_type;
      is >> ws >> str;
      adt_type = Aglob_vars()->cat->str_to_attr_type (str);
      func = new Aliteral_t (adt_type);
    }

  else if (strcmp (str, "relattr") == 0)
    func = new Arelattr_t;
  else if (strcmp (str, "f") == 0)
    func = new Af_apply_t;

  else 
    assert (!"unknown operator");

  assert (func);
  return is;
}


void write_func (ostream &os, const Afunc_t *func, const Aptree_t &ptree)
{
  int i;
  switch (func->type ())
    {
    case Afunc_t::empty:
      os << "empty";
      break;
    case Afunc_t::and:
      os << *ptree.arg (0) << " AND " << *ptree.arg (1);
      break;
    case Afunc_t::or:
      os << *ptree.arg (0) << " AND " << *ptree.arg (1);
      break;
    case Afunc_t::lt:
      os << *ptree.arg (0) << " < " << *ptree.arg (1);
      break;
    case Afunc_t::gt:
      os << *ptree.arg (0) << " > " << *ptree.arg (1);
      break;
    case Afunc_t::leq:
      os << *ptree.arg (0) << " <= " << *ptree.arg (1);
      break;
    case Afunc_t::geq:
      os << *ptree.arg (0) << " >= " << *ptree.arg (1);
      break;
    case Afunc_t::eq:
      os << *ptree.arg (0) << " = " << *ptree.arg (1);
      break;
    case Afunc_t::neq:
      os << *ptree.arg (0) << " <> " << *ptree.arg (1);
      break;
    case Afunc_t::between:
      os << *ptree.arg (0) << " BETWEEN " << *ptree.arg (1) 
	 << " AND " << *ptree.arg (2);
      break;
    case Afunc_t::not_between:
      os << *ptree.arg (0) << " NOT BETWEEN " << *ptree.arg (1) 
	 << " AND " << *ptree.arg (2);
      break;
    case Afunc_t::literal:
      ((Aliteral_t *)func)->adt ()->print (os);
      break;
    case Afunc_t::oidref:
      os << "oidref " << ((Aoidref_t *)func)->oidstr ();
      break;
    case Afunc_t::relattr:
      os << ((Arelattr_t *)func)->relname () << "." 
	 << ((Arelattr_t *)func)->attrname ();
      break;
    case Afunc_t::tuplevar:
      os << "relation " << ((Atuplevar_t *)func)->relname ();
      break;
    case Afunc_t::tupleref:
      os << ((Atupleref_t *)func)->component ();
      break;
    case Afunc_t::attref:
      os << ((Aattref_t *)func)->attrname ()
	 << " (" << ((Arelref_t *)ptree.arg (0)->func ())->relname () << ")";
      break;
    case Afunc_t::f_apply:
      os << ((Af_apply_t *)func)->name () << " (";
      for (i=0; i<ptree.nargs (); i++)
	{
	  if (i>0)
	    os << ", ";
	  os << *ptree.arg (i);
	}
      os << ")";
      break;
    case Afunc_t::unknown:
      os << "unknown";
      break;
    default:
      assert (!"unknown func type");
      break;
    }
}



ostream &operator<< (ostream &os, const Afunc_t *func)
{
  char *str;

  switch (func->type ())
    {
    case Afunc_t::empty:
      str = "empty";
      break;
    case Afunc_t::and:
      str = "and";
      break;
    case Afunc_t::or:
      str = "or";
      break;
    case Afunc_t::lt:
      str = "lt";
      break;
    case Afunc_t::gt:
      str = "gt";
      break;
    case Afunc_t::leq:
      str = "leq";
      break;
    case Afunc_t::geq:
      str = "geq";
      break;
    case Afunc_t::eq:
      str = "eq";
      break;
    case Afunc_t::neq:
      str = "neq";
      break;
    case Afunc_t::between:
      str = "between";
      break;
    case Afunc_t::not_between:
      str = "not_between";
      break;
    case Afunc_t::literal:
      str = "literal";
      break;
    case Afunc_t::oidref:
      str = "oidref";
      break;
    case Afunc_t::relattr:
      str = "relattr";
      break;
    case Afunc_t::tuplevar:
      str = "tuplevar";
      break;
    case Afunc_t::tupleref:
      str = "tupleref";
      break;
    case Afunc_t::attref:
      str = "attref";
      break;
    case Afunc_t::f_apply:
      str = "f_apply";
      break;
    case Afunc_t::unknown:
      str = "unknown";
      break;
    default:
      assert (!"unknown func type");
      break;
    }

  os << str;

  return os;
}


