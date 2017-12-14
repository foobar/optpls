#include "Bparsetree.h"
#include <Aquery.h>
#include <Atimer.h>
#include <Asearch.h>

void Bop_t::execute (Bastnode_t &node)
{
  if (this == Bastnode_t::utility ||
      this == Bastnode_t::update ||
      this == Bastnode_t::query)
    node.arg (0).execute ();
  else if (this == Bastnode_t::empty)
    return;
  else
    assert (!"Unknown op to execute");
}

void Bcreate_idx_op_t::execute (Bastnode_t &node)
{
  int clustered = this == Bastnode_t::create_clus_idx ? 1 : 0;

  const char *idx_name = node.arg (0).text ();
  const char *rel_name = node.arg (1).text ();
  assert (node.arg (2).nargs () == 1);
  const char *attr_name = node.arg (2).arg (0).text ();

  Aglob_vars()->cat->create_idx (idx_name, rel_name, 
				 attr_name, clustered, Acat_t::btree ());
				// do something about the index type here...
}

void Bcreate_table_op_t::execute (Bastnode_t &node)
{
//   const char *rel_name = node.arg (0).text ();
//   int num_attrs = node.arg (1).nargs ();

//   assert (num_attrs > 0);
//   Acat_t::attrec_t attrs[num_attrs];

//   for (int i=0; i<num_attrs; i++)
//     {
//       const char *attr_name = node.arg (1).arg (i).arg (0).text ();
//       const char *type_name = node.arg (1).arg (i).arg (1).arg (0).text ();
//       Acat_t::attr_type_t type = 
// 	Aglob_vars()->cat->str_to_attr_type (type_name);
//       assert (type != Acat_t::bogus_adt ());
//       Aglob_vars()->cat->init_attrec (attrs[i], attr_name, type);
// 				// do something about the length here???
//     }

//   Aglob_vars()->cat->create_rel (rel_name, num_attrs, attrs);
}

void Bload_table_op_t::execute (Bastnode_t &node)
{
  const char *rel_name = node.arg (0).text ();
  int num_files = node.arg (1).nargs ();
  assert (num_files > 0);
  const char *filenames[num_files];

  for (int i=0; i<num_files; i++)
    filenames[i] = node.arg (1).arg (i).arg (0).text ();

  Aglob_vars()->cat->load_rel (rel_name, num_files, filenames);
}

void Bdrop_table_op_t::execute (Bastnode_t &node)
{
  const char *rel_name = node.arg (0).text ();
  Aglob_vars()->cat->drop_rel (rel_name);
}


void Bcreate_db_op_t::execute (Bastnode_t &node)
{
  const char *db_name = node.arg (0).text ();
  Aglob_vars()->cat->create_db (db_name);
}


void Bopen_db_op_t::execute (Bastnode_t &node)
{
  const char *db_name = node.arg (0).text ();
  Aglob_vars()->cat->open_db (db_name);
}


void Bclose_db_op_t::execute (Bastnode_t &node)
{
  const char *db_name = node.arg (0).text ();
  Aglob_vars()->cat->close_db (db_name);
}

void Bquery_stmt_op_t::execute (Bastnode_t &node)
{
  Atimer_t timer;
  struct timeval opt_time, exe_time;

  convert (node, *Aglob_vars()->query);
  Aglob_vars()->query->init ();

//  for (int i=0; i<Aglob_vars()->query->numrels (); i++)
//    {
//      Arel_t *rel = Aglob_vars()->query->rel (i);
//      cout << '[' << i << "] " 
//	   << (rel->has_extent_or_set () ? rel->name () : "NoExtent") << "::" 
//	   << rel->rel_var () << endl;
//    }

//  for (i=0; i<Aglob_vars()->query->numpreds (); i++)
//    cout << '[' << i << "] " 
//	 << Aglob_vars()->query->pred (i)->ptree () << endl;
//  for (i=0; i<Aglob_vars()->query->numoperations (); i++)
//    cout << '[' << i << "] " 
//	 << *Aglob_vars()->query->operation (i) << endl;
  
//  cout << endl << endl;

  Aopalgo_t opalgo;
  Aglob_vars()->opalgo = &opalgo;

//  timer.restart ();
  Arusage_t rusage;
  Aphynode_t *access_plan = Aglob_vars()->search->Optimize ();
  rusage.freeze ();
  
//   cout
//       << setw (7) << Aglob_vars()->search->lognode_ctr.tot ()
//       << setw (7) << Aglob_vars()->search->lognode_ctr.max ()
//       << setw (7) << Aglob_vars()->search->phynode_ctr.tot ()
//       << setw (7) << Aglob_vars()->search->phynode_ctr.max () << endl;
//   cout
//       << setw (7) << Aglob_vars()->search->subopt_lognode_ctr.tot ()
//       << setw (7) << Aglob_vars()->search->subopt_lognode_ctr.max ()
//       << setw (7) << Aglob_vars()->search->subopt_phynode_ctr.tot ()
//       << setw (7) << Aglob_vars()->search->subopt_phynode_ctr.max () << endl;

  Bexecute_plan (access_plan);
  cout << "output" << endl;
  cout << "------------------------------------------------" << endl;
  cout << "Estimated cost (secs.) : " 
       << access_plan->GetPhyProps ()->GetCost () << endl;
  cout << rusage << endl;
}

