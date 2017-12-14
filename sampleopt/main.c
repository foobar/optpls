#include "Acat.h"
#include "Aquery.h"
#include "Aphyprop.h"
#include <Aoptions.h>
#include <Aglob_vars.h>
#include <Aopdefs.h>
#include <Atimer.h>

#include <Asearch.h>
#include <Aphynode.h>
#include <Ahash.h>


#include <fstream.h>
#include <assert.h>
#include <stdlib.h>
#include <iomanip.h>
#include <stdio.h> // for getopt stuff... 

#include <Bbogus.h>
#include <Bparsetree.h>
// #include <tcl.h>

int use_tcl = 0;

extern "C" {
  int gettimeofday (struct timeval *, struct timezone *);
}

extern double memory_per_operator;


static int generate_seed (void)
{
  struct timezone tz;
  tz.tz_minuteswest = tz.tz_dsttime = 0;
  struct timeval cur_time;
  assert (gettimeofday (&cur_time, &tz) == 0);
  int usec = (int)cur_time.tv_usec;

  int pid = (int) getpid ();

  return usec + pid;
}



void queries_from_parser (void)
{
  Aglob_vars()->search->Initialize ();

  Aquery_t query;
  Aglob_vars()->query = &query;

  char str[1024];
  // cout << "Query: " << flush;
  cin >> ws;
  cin.get (str, sizeof (str), ';');
  char semi_colon;
  cin >> semi_colon;

  Bparser_state()->input_string = str;

  extern void Brestart_lexical_analyzer (void);
  Brestart_lexical_analyzer ();
  extern int yyparse (void);
  if (yyparse ())
    {cout << "parsing failed" << endl; return;}
  else
     ; // cout << "parsing succeeded" << endl;

  Bparser_state()->query.typechk ();
  if (Bparser_state()->error ())
    {cout << Bparser_state()->error_str () << endl; return;}
  else
    ; // cout << "typechk succeeded" << endl;

  Bparser_state()->query.execute ();

  return;			// for now.

}

// returns true if quit...
void generate_and_execute (int num_rels, int num_preds, int unparse)
{
  Atimer_t timer;
  struct timeval startup, setup, opttime;

  Aglob_vars()->search->Initialize ();

  Aquery_t query;
  Aglob_vars()->query = &query;

  Brqg_t rqg (query);

  rqg.generate (num_rels, num_preds);

  if (unparse)
    {
      cout << "The query was: " << endl;
      rqg.unparse (cout);
    }

  timer.time (startup);
  timer.restart ();
  query.init ();

  Aopalgo_t opalgo;		// the opalgo object cannot be allocated
				// before the query is read in ...
  Aglob_vars()->opalgo = &opalgo;


  timer.time (setup);
  timer.restart ();

  Aphynode_t *access_plan = Aglob_vars()->search->Optimize ();
  timer.time (opttime);
  timer.restart ();

  if (Aglob_vars()->print_stats)
    cout 
//      << setw (10) << startup 
//      << setw (10) << setup
      << setw (7) << Aglob_vars()->search->subopt_lognode_ctr.tot ()
      << setw (7) << Aglob_vars()->search->subopt_lognode_ctr.max ()
      << setw (7) << Aglob_vars()->search->subopt_phynode_ctr.tot ()
      << setw (7) << Aglob_vars()->search->subopt_phynode_ctr.max ()

      << setw (7) << Aglob_vars()->search->lognode_ctr.tot ()
      << setw (7) << Aglob_vars()->search->lognode_ctr.max ()
      << setw (7) << Aglob_vars()->search->phynode_ctr.tot ()
      << setw (7) << Aglob_vars()->search->phynode_ctr.max ()
      << ' '
      << setw (10) << access_plan->GetPhyProps ()->GetCost ()
      << ' '
      << setw (10) << opttime << endl;

  Bexecute_plan (access_plan);

  Aglob_vars()->search->CleanUp ();

  return;
}

int my_optind = 0;
char *my_optarg = 0;
struct optstruct_t {
  char *optstring;
  int optchar;
};
struct optstruct_t my_options [] = {
  {"no_assembly", 'a'},
  {"left_deep", 'b'},
  {"bushy", 'Z'},
  {"right_deep", 'B'},
  {"cartesian", 'c'},
  {"select_pushdown", 'E'},
  {"no_path_index", 'I'},
  {"no_mat_to_join", 'J'},
  {"ssBottomup", 'w'},
  {"ssTransformative", 'W'},
  {"ssSA", 'x'},
  {"ssII", 'X'},
  {"ss2PO", 'y'},
  {"ssAstar", 'Y'},
  {"", EOF} 
};

int my_getopt (int argc, char *argv[], char *)
{
  my_optind++;
  if (my_optind >= argc)
    return EOF;
  
  char *option_argv = argv[my_optind];
  if (option_argv[0] != '-')
    return EOF;

  char *option_string = option_argv + 1;

  for (int i=0; my_options[i].optstring[0] != '\0'; i++)
    {
      if (strcmp (my_options[i].optstring, option_string) == 0)
	return my_options[i].optchar;
    }

  if (option_string[1] != '\0')
    {
      cout << "unknown option " << option_string << endl;
      return '?';
    }

  return option_string[0];
}

Aglob_vars_t main_glob_vars;
int main (int argc, char *argv[])
{
  Aoptimizeroptions_t oopt;
  int num_queries = 1, num_rels = 3, num_preds = -1, 
    generate = 0, unparse = 0, header = 0;


 // command line options

  int c;
  extern int my_optind, my_opterr;
  extern char *my_optarg;
  while ((c = my_getopt (argc, argv, 
		      "aAbBcCdeEhHiIJlLmM:n:N:p:rRs:SuvVZ")) != EOF)
    switch (c)
      {
      case 'a': // access_plan
	Aglob_vars()->print_plan = 1;
	break;
      case 'A': // dont use the assembly algorithm
	oopt.dont_use_assembly = 1;
	break;
      case 'b': // dont do bushy_trees, do left deep only
	oopt.left_deep_only = 1;
	break;
      case 'B': // dont do bushy_trees, do right deep only
	oopt.right_deep_only = 1;
	break;
      case 'c': // cartesian_products
	oopt.delay_cartesian_products = 0;
	break;
      case 'C': // dont do mat-collapse
	oopt.do_mat_collapse = 0;
	break;
      case 'd': // dont_execute_queries 
	Aglob_vars()->dont_execute_queries = 1;
	break;
      case 'e': // turn off equivalence classes
	oopt.no_equivalences = 1;
	break;
      case 'E': // dont do exhaustive select positioning
	oopt.do_exhaustive_selects = 0;
	break;
      case 'h': // hash_join
	oopt.do_hash_join = 1;	
	break;
      case 'H': // print the header
	header = 1;
	break;
      case 'i': // index heuristic
	oopt.do_idx_heuristic = 1;
	break;
      case 'I': // dont do path indexes
	oopt.do_path_indexes = 0;
	break;
      case 'J': // dont do mat to join conversion
	oopt.do_mat_to_join = 0;
	break;
      case 'l': // dont split the unexpanded node lists
	oopt.dont_split_lists = 1;
	break;
      case 'L': // dont do select collapse
	oopt.do_select_collapse = 0;
	break;
      case 'm': // merge_join
	oopt.do_merge_join = 1;
	break;
      case 'M': // set the memory size (per operator)
	memory_per_operator = atoi (my_optarg);
	break;
      case 'n': // -n <num>  number of relations
	num_rels = atoi (my_optarg);
	break;
      case 'N': // -N <num>  number of queries 
	num_queries = atoi (my_optarg);
	break;
      case 'p': // -p <num>   number of predicates
	num_preds = atoi (my_optarg);
	break;
      case 'r': // randomize
	srand (generate_seed ());
	break;
      case 'R': // dont read query from file
	generate = 1;
	break;
//       case 's': // -seed <num>   set the seed of the random number generator
// 	srand (atoi (my_optarg));
// 	break;
      case 's': // run setup. 
	use_tcl = 1;
	break;
      case 'S': // secondary joins
	Aglob_vars()->no_secondary_joins = 0;
	break;
      case 'u': // unparse
	unparse = 1;
	break;
      case 'v': // verbose mode. (print stats).
	Aglob_vars()->print_stats = 1;
	break;
      case 'V': // dont use inverse links
	oopt.do_inverse_links = 0;
	break;
      case 'w': // do nothing
	oopt.all_operator_trees = 0;
	oopt.all_operator_trees = 0;
	break;
      case 'W': // do nothing
	oopt.all_operator_trees = 10;
	oopt.all_operator_trees = 0;
	break;
      case 'x': // do nothing
	oopt.all_operator_trees = 90;
	oopt.all_operator_trees = 20;
	break;
      case 'X': // do nothing
	oopt.all_operator_trees = 80;
	oopt.all_operator_trees = 30;
	break;
      case 'y': // do nothing
	oopt.all_operator_trees = 70;
	oopt.all_operator_trees = 5;
	break;
      case 'Y': // do nothing
	oopt.all_operator_trees = -1;
	oopt.all_operator_trees = 60;
	break;
      case 'Z': // dummy argument. ignored.
	break;
      default:
	cerr << "usage: " << argv[0] << " [options]" << endl
	  << "where options are" << endl
	     << " -a\t\tprint the access plan" << endl
	     << " -A\t\tdont use the assembly algorithm" << endl
	     << " -b\t\tdont do bushy trees, do left deep only" << endl
	     << " -B\t\tdont do bushy trees, do right deep only" << endl
	     << " -c\t\ttry cartesian products too" << endl
	     << " -C\t\tdont do mat collapse" << endl
	     << " -d\t\tdont execute queries" << endl
	     << " -e\t\tturn off equivalence classes" << endl
	     << " -E\t\tdont do exhaustive selects" << endl
	     << " -h\t\tuse hash join as a join algorithm too" << endl
	     << " -H\t\tprint the header" << endl
	     << " -i\t\timplement the index join heuristic" << endl
	     << " -I\t\tdont use path indexes" << endl
	     << " -l\t\tdont split the unexpanded nodes lists" << endl
	     << " -L\t\tdont do select collapse" << endl
	     << " -m\t\tuse merge join as a join algorithm too" << endl
	     << " -M\t\tset the memory size per operator (pages)" << endl
	     << " -n <num>\tnumber of relations to join." << endl
	     << " -N <num>\tnumber of queries to run" << endl
	     << " -p <num>\tnumber of predicates in where clause" << endl
	     << " -r\t\trandomize the random number generator" << endl
	     << " -R\t\tread query from file \".query\"" << endl
	     << " -s <num>\tset seed for random number generator" << endl
	     << " -S\t\tgenerate secondary join predicates too" << endl
	     << " -u\t\tunparse and print the generated query" << endl
	     << " -v\t\tverbose mode. print statistics and stuff." << endl
	     << " -V\t\tdont use inverse links." << endl
	     << " -Z\t\tdummy argument. this is ignored." << endl;
	exit (1);
	break;
      }

  // setting up the stuff

  Acat_t catalog;
  Asearch_t search;
  Ahashtable_t hashtable (1021);
  Bparser_state_t parser_state;

  Aglob_vars()->cat = &catalog;
  Aglob_vars()->search = &search;
  Aglob_vars()->oopt = &oopt;
  if (getenv ("allops")) Aglob_vars()->oopt->all_operator_trees = 1;

  Aglob_vars()->hashtable = &hashtable;
  Aglob_vars()->parser_state = &parser_state;

  // set up the catalogs
  ifstream catfile (".catalog");
  assert (!!catfile);
  catalog.read (catfile);

  
  // do queries.

  if (num_preds < 0) num_preds = num_rels - 1; 
				// useful default
  if (header)
    cout
//      << setw (10) << "Startup" 
//      << setw (10) << "Setup" 
      << setw (7) << "SL_tot"
      << setw (7) << "SL_max"
      << setw (7) << "SP_tot"
      << setw (7) << "SP_max"
      << setw (7) << "TotLog"
      << setw (7) << "MaxLog"
      << setw (7) << "TotPhy"
      << setw (7) << "MaxPhy"
      << " "
      << setw (10) << "Cost"
      << " "
      << setw (10) << "OptTime" << endl;

//  if (use_tcl)
//    setup_tcl ();

  if (generate)
    for (int i=0; i<num_queries; i++)
      generate_and_execute (num_rels, num_preds, unparse);
  else
    queries_from_parser ();

  return 0;
}

