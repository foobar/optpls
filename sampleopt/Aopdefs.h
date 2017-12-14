#ifndef AOPDEFS_H
#define AOPDEFS_H

class Aop_t;
class Aalgo_t;
class Aenforcer_t;

enum AopNumber {
  Aget,
  Amat,				// materialize operator
  Amat_collapse,
  Aselect,
  Aselect_collapse,
  Aidx_collapse,		// collapse to index scan
  Ajoin,
  Aunnestop,
  Aaggrop,			// aggregate operator
  Aoutputop,			// output the result of the query
};

class Aget_t;
class Amat_t;
class Amat_collapse_t;
class Aselect_t;
class Aselect_collapse_t;
class Aidx_collapse_t;
class Ajoin_t;
class Aunnestop_t;
class Aaggrop_t;
class Aoutputop_t;

enum AalgoNumber {
  Afilescan,
  Aindexscan,
  Afilter,
  Aassembly,
  Ahh_join,			// hybrid hash join
  Ahhptr_join,			// hybrid hash pointer join
  Aunnestalgo,
  Aoutputalgo			// output the result of the query
};

class Afilescan_t;
class Aindexscan_t;
class Aassembly_t;
class Afilter_t;
class Ahh_join_t;
class Ahhptr_join_t;
class Aunnestalgo_t;
class Aoutputalgo_t;


#include <Alist.h>

// this class stores the association between the
// operators and the various algorithms used to implement them.
class Aopalgo_t {
public:
  // lists of algorithms associated with various operators
  Alist_t<Aalgo_t> get_algos;
  Alist_t<Aalgo_t> mat_algos;
  Alist_t<Aalgo_t> mat_collapse_algos;
  Alist_t<Aalgo_t> select_algos;
  Alist_t<Aalgo_t> select_collapse_algos;
  Alist_t<Aalgo_t> idx_collapse_algos;
  Alist_t<Aalgo_t> join_algos;
  Alist_t<Aalgo_t> unnest_algos;
  Alist_t<Aalgo_t> output_algos;
  
  // array of lists of enforcers associated with various algorithms
  Alist_t<Aenforcer_t> *enforcer_array;

  // operators and algorithms.
  Aget_t *get;
  Amat_t *mat;
  Amat_collapse_t *mat_collapse;
  Aselect_t *select;
  Aselect_collapse_t *select_collapse;
  Aidx_collapse_t *idx_collapse;
  Ajoin_t *join;
  Aunnestop_t *unnestop;
  Aoutputop_t *outputop;

  Afilescan_t *filescan;
  Aassembly_t *assembly;
  Afilter_t *filter;
  Aindexscan_t *indexscan;
  Ahh_join_t *hh_join;
  Ahhptr_join_t *hhptr_join;
  Aunnestalgo_t *unnestalgo;
  Aoutputalgo_t *outputalgo;

  Alist_t<Aop_t> all_operators;

public:
  Aopalgo_t (void);
  ~Aopalgo_t (void);
};

#endif /* AOPDEFS_H */
