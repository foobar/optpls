#ifndef AGLOB_VARS_H
#define AGLOB_VARS_H

// all the global variables are stored in this class.
// this is done so as to avoid problems in case this
// optimizer in used in a multi-threaded system
// where global variables are shared among threads.
// in this case, the
// 

class Ahashtable_t;
class Asearch_t;
class Aoptimizeroptions_t;
class Acat_t;
class Aquery_t;
class Aopalgo_t;
class Bparser_state_t;

class Aglob_vars_t {
 public:
  // global variables required by APG
  Ahashtable_t *hashtable;	// APG internal hashtable
  int end_of_replacers;		// APG internal variable
  int end_of_set;		// used by the Aset_t class
  int end_of_list;		// used by the Alist_t class

  Asearch_t *search;		// search strategy object. used by APG
  Aoptimizeroptions_t *oopt;	// option controlling the optimizer
  Aopalgo_t *opalgo;		// operators and algorithms


  // global variable used by the optimizer.
  Acat_t *cat;			// the catalog object
  Aquery_t *query;		// the query being optimized

  // global variables required by the parser/typechecker
  Bparser_state_t *parser_state;

  // command line options used by various parts of the program
  int print_plan;
  int dont_execute_queries;
  int no_secondary_joins;
  int print_stats;

  // used by Bexecution.c to generate temporary filenames.
  int filename_counter;

  Aglob_vars_t (void) {
    print_plan = dont_execute_queries = print_stats = 0;
    no_secondary_joins = 1;
    filename_counter = 0;
  }
};

Aglob_vars_t *Aglob_vars ();


/***************************************************************************

  inline functions

***************************************************************************/
extern Aglob_vars_t main_glob_vars;

inline Aglob_vars_t *Aglob_vars (void)
{
  return &main_glob_vars;
}


#endif /* AGLOB_VARS_H */
