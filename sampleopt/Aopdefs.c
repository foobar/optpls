#include <Aopdefs.h>
#include <Aopinc.h>
#include <Aoptions.h>
#include <fstream.h>

static inline int Areadnf (istream *is, const char *expected_name)
{
  if (is == 0)
    return 1;

  char name[A_MAXNAMELEN]; int flag; 
  *is >> ws >> name >> flag;
  assert (strcmp (name, expected_name) == 0);
  return flag;
}

Aopalgo_t::Aopalgo_t (void)
{
  // allocate the enforcers.
  assert (assembly = new Aassembly_t);

  // allocate the enforcer arrays
  assert (enforcer_array = new Alist_t<Aenforcer_t>[2]);
  enforcer_array[0].InsertAtEnd (assembly);
  enforcer_array[1].InsertAtEnd (assembly);

  // allocate the operators and the algorithm

  ifstream infile_obj (".opalgos");
  ifstream *infile = 0;
  if (!!infile_obj) infile = &infile_obj;

  // get
  assert (filescan = new Afilescan_t);
  get_algos.InsertAtEnd (filescan);
  assert (get = new Aget_t (get_algos));
  
  // mat_collapse
  assert (assembly = new Aassembly_t);
  if (Areadnf (infile, assembly->GetName ()) &&
      Aglob_vars()->oopt->dont_use_assembly)
    mat_collapse_algos.InsertAtEnd (assembly);
  assert (mat_collapse = new Amat_collapse_t (mat_collapse_algos));
  if (Areadnf (infile, mat_collapse->GetName ()) &&
      Aglob_vars()->oopt->do_mat_collapse)
    all_operators.InsertAtEnd (mat_collapse);


  // select_collapse
  assert (filter = new Afilter_t (enforcer_array));
  if (Areadnf (infile, filter->GetName ()))
    select_collapse_algos.InsertAtEnd (filter);
  assert (select_collapse = new Aselect_collapse_t (select_collapse_algos));
  if (Areadnf (infile, select_collapse->GetName ()) &&
      Aglob_vars()->oopt->do_select_collapse)
    all_operators.InsertAtEnd (select_collapse);


  // idx_collapse
  assert (indexscan = new Aindexscan_t);
  if (Areadnf (infile, indexscan->GetName ()))
    idx_collapse_algos.InsertAtEnd (indexscan);
  assert (idx_collapse = new Aidx_collapse_t (idx_collapse_algos));
  if (Areadnf (infile, idx_collapse->GetName ()))
    all_operators.InsertAtEnd (idx_collapse);

  // mat
  if (Areadnf (infile, assembly->GetName ()) &&
      !Aglob_vars()->oopt->dont_use_assembly)
    mat_algos.InsertAtEnd (assembly);
  assert (mat = new Amat_t (mat_algos));
  if (Areadnf (infile, mat->GetName ()))
    all_operators.InsertAtEnd (mat);

  // select
  if (Areadnf (infile, filter->GetName ()))
    select_algos.InsertAtEnd (filter);
  assert (select = new Aselect_t (select_algos));
  if (Areadnf (infile, select->GetName ()))
    all_operators.InsertAtEnd (select);

  // join
  assert (hh_join = new Ahh_join_t (enforcer_array));
  if (Areadnf (infile, hh_join->GetName ()))
    join_algos.InsertAtEnd (hh_join);
  assert (hhptr_join = new Ahhptr_join_t (enforcer_array));
  if (Areadnf (infile, hhptr_join->GetName ()))
    join_algos.InsertAtEnd (hhptr_join);
  assert (join = new Ajoin_t (join_algos));
  if (Areadnf (infile, join->GetName ()))
    all_operators.InsertAtEnd (join);

  // unnest
  assert (unnestalgo = new Aunnestalgo_t (enforcer_array));
  if (Areadnf (infile, unnestalgo->GetName ()))
    unnest_algos.InsertAtEnd (unnestalgo);
  assert (unnestop = new Aunnestop_t (unnest_algos));
  if (Areadnf (infile, unnestop->GetName ()))
    all_operators.InsertAtEnd (unnestop);

  // output
  assert (outputalgo = new Aoutputalgo_t (enforcer_array));
  if (Areadnf (infile, outputalgo->GetName ()))
    output_algos.InsertAtEnd (outputalgo);
  assert (outputop = new Aoutputop_t (output_algos));
  if (Areadnf (infile, outputop->GetName ()))
    all_operators.InsertAtEnd (outputop);
}

Aopalgo_t::~Aopalgo_t (void)
{
  get_algos.DeleteList ();
  mat_algos.DeleteList ();
  mat_collapse_algos.DeleteList ();
  select_algos.DeleteList ();
  select_collapse_algos.DeleteList ();
  idx_collapse_algos.DeleteList ();
  join_algos.DeleteList ();
  unnest_algos.DeleteList ();
  output_algos.DeleteList ();

  delete get; delete mat; delete mat_collapse;
  delete select; delete select_collapse; delete idx_collapse; 
  delete join; delete unnestop; delete outputop; 

  delete filescan; delete assembly; 
  delete filter; delete indexscan;
  delete hh_join; delete hhptr_join; 
  delete unnestalgo; delete outputalgo;

  enforcer_array[0].DeleteList ();
  enforcer_array[1].DeleteList ();

  delete [] enforcer_array;
}
