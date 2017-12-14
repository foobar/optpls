#include <Bbogus.h>
#include <Aoptions.h>

long int Brqg_t::random_number (long int high, long int low)
{
  long int range = high - low;
  if (range == 0) return low;

  assert (range > 0);

  long int random_num = rand ();
  return (low + (random_num % range));
}

int Brqg_t::random_attr (int relnum, Acat_t::attr_type_t attrtype)
{
//   int attrnum = (int)random_number (Aglob_vars()->cat->_rels[relnum].attrcnt);
  
//   assert (attrnum >= 0);
//   for (int i = -1; attrnum >= 0;)
//     {
//       i++;
//       if (i == q._numattrs) i = 0; // wrap around
//       if (q._attrs[i].attr.rel ()->Number () == relnum)
// 	if (attrtype == Acat_t::bogus_adt ())
// 	  attrnum--;
// 	else if (q._attrs[i].attr.type () == attrtype)
// 	  attrnum--;
//     }
//   return i;
  return 0;
}

Aptree_t Brqg_t::random_ptree (void)
{
//   if (!Aglob_vars()->no_secondary_joins)
//     if (random_number (20) < 1)
//       {
// 	// 5% chance of an OR
// 	Aptree_t left_ptree = random_ptree ();
// 	Aptree_t right_ptree = random_ptree ();

// 	Aboolfunc_t *or_func = new Aboolfunc_t (Afunc_t::or);
// 	assert (or_func);
// 	Aptree_t ptree;
// 	Aptree_t args[2] = {left_ptree, right_ptree};
// 	ptree.init (or_func, 2, args);
// 	return ptree;
//       }

//   Aattr_t *left_attr = q.attr (random_attr ((int)random_number (q._numrels)));
//   Aattref_t *left_func = new Aattref_t (left_attr); assert (left_func);
//   Aptree_t left_ptree; left_ptree.init (left_func, 0);

//   Afunc_t *right_func;
//   Acat_t::attr_type_t left_attr_type = left_attr->type ();

//   Acat_t::adt_t *adt;
//   if (left_attr_type == Acat_t::integer ())
//     {
//       Binteger_t *i = new Binteger_t; 
//       assert (i);
//       i->init ((int)random_number (10000)); 
//       adt = i;
//     }
//   else if (left_attr_type == Acat_t::real ())
//     {
//       Breal_t *r = new Breal_t;
//       assert (r);
//       r->init ((float)random_number (10000)/10000.0);
//       adt = r;
//     }
//   else if (left_attr_type == Acat_t::string ())
//     {
//       Bstring_t *s = new Bstring_t;
//       assert (s);
//       s->ascii_to_mem ("string");
//       adt = s;
//     }
//   else
//     assert (!"default reached");

//   right_func = new Aliteral_t (left_attr_type, adt);
//   assert (right_func);
//   Aptree_t right_ptree; right_ptree.init (right_func);

//   Afunc_t *relop;
//   if (random_number (20) < 1)
//     {
//       // 5% chance of !=
//       assert (relop = new Aclose_relop_t (Afunc_t::neq));
//     }
//   else
//     {
//       switch (random_number (6))
// 	{
// 	case 0:
// 	  assert (relop = new Aopen_relop_t (Afunc_t::lt));
// 	  break;
// 	case 1:
// 	  assert (relop = new Aopen_relop_t (Afunc_t::leq));
// 	  break;
// 	case 2:
// 	  assert (relop = new Aopen_relop_t (Afunc_t::gt));
// 	  break;
// 	case 3:
// 	  assert (relop = new Aopen_relop_t (Afunc_t::geq));
// 	  break;
// 	case 4: case 5:
// 	  assert (relop = new Aclose_relop_t (Afunc_t::eq));
// 	  break;
// 	default:
// 	  assert (!"default reached");
// 	  break;
// 	}
//     }
//   Aptree_t args[2] = {left_ptree, right_ptree};
  Aptree_t ptree; // ptree.init (relop, 2, args);
  return ptree;
}

// generate a random query of "numrels" relations
// and "numpreds" predicates.
void Brqg_t::generate (int nrels, int npreds)
{
  int i;

//   assert ((q._numrels = nrels) > 0);
//   assert (q._rels = new Aquery_t::rel_t[q._numrels]);

//   int cat_numrels = Aglob_vars()->cat->_numrels;

//   // the relations used in the query
//   for (i=0, q._numattrs = 0; i<q._numrels; i++)
//     {
//       char rel_var[20];
//       ostrstream ostr (rel_var, sizeof (rel_var));
//       ostr << "rel" << i << '\0';

//       Acat_t::relid_t relid = (Acat_t::relid_t)random_number (cat_numrels);
//       q._rels[i].rel.init (i, relid, rel_var);
//       q._numattrs += Aglob_vars()->cat->_rels[relid].attrcnt;
//     }

//   // the attributes used in the query.
//   assert (q._attrs = new Aquery_t::attr_t[q._numattrs]);
//   int k=0;
//   for (i=0; i<q._numrels; i++)
//     {
//       int attr_lower_lim = 
// 	Aglob_vars()->cat->_rels[q.rel (i)->relid ()].attrptr;
//       int attr_upper_lim = 
// 	attr_lower_lim + Aglob_vars()->cat->_rels[q.rel (i)->relid ()].attrcnt;

//       for (int j=attr_lower_lim; j<attr_upper_lim; j++)
// 	{
// 	  assert (k<q._numattrs);
// 	  q._attrs[k].attr.init (k, q.rel (i), j);
// 	  k++;
// 	}
//     }
//   assert (k=q._numattrs);

//   // the set of predicates
//   q._numpreds = npreds;
//   assert (q._numpreds >= 0);
//   if (q._numpreds > 0)
//     {
//       assert (q._preds = new Apred_t [q._numpreds]);
      
//       // first the join predicates;
//       for (i=0; 
// 	   i  <  (q._numrels-1 < q._numpreds ? q._numrels-1 : q._numpreds); 
// 	   i++)
// 	{
// 	  int join_rel = (int)random_number (i+1);
// 				// relation with which the i+1_th relation
// 				// will join

// 	  Aattr_t *left_attr = q.attr (random_attr (i+1));
// 	  Aattr_t *right_attr = q.attr (random_attr (join_rel, 
// 						     left_attr->type ()));
      
// 	  Aattref_t *left_func = new Aattref_t (left_attr);
// 	  Aattref_t *right_func = new Aattref_t (right_attr);
// 	  assert (left_func && right_func);

// 	  Aptree_t left_ptree, right_ptree;
// 	  left_ptree.init (left_func);
// 	  right_ptree.init (right_func);

// 	  Afunc_t *relop;
// 	  if (random_number (10) < 9)
// 	    {
// 	      // 90% chance of an equijoin.
// 	      assert (relop = new Aclose_relop_t (Afunc_t::eq));
// 	    }
// 	  else
// 	    {
// 	      switch (random_number (4))
// 		{
// 		case 0:
// 		  assert (relop = new Aopen_relop_t (Afunc_t::lt));
// 		  break;
// 		case 1:
// 		  assert (relop = new Aopen_relop_t (Afunc_t::leq));
// 		  break;
// 		case 2:
// 		  assert (relop = new Aopen_relop_t (Afunc_t::gt));
// 		  break;
// 		case 3:
// 		  assert (relop = new Aopen_relop_t (Afunc_t::geq));
// 		  break;
// 		default:
// 		  assert (!"default reached");
// 		  break;
// 		}
// 	    }
// 	  Aptree_t args[2] = {left_ptree, right_ptree};
// 	  Aptree_t ptree; ptree.init (relop, 2, args);

// 	  q._preds[i].init (i, ptree);
// 	}

//       // fill in the rest of the predicates.
//       for (; i<q._numpreds; i++)
// 	{
// 	  Aptree_t ptree = random_ptree ();
// 	  q._preds[i].init (i, ptree);
// 	}
//     }

//   q._is_select_star = 1;
//   q._groupby = 0;
  
//   if (random_number (10) < 2)
//     {
//       // 20% chance of having an order by clause
//       q._orderby = q.attr (random_attr ((int)random_number (q._numrels)));
//       q._ascending = 1;
//     }
}

