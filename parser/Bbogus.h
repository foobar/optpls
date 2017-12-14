#ifndef BBOGUS_H
#define BBOGUS_H

#include <Aquery.h>
#include <Aptree.h>
#include <Afunc.h>
#include <assert.h>
#include <strstream.h>

#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

extern "C" {
  int rand (void);
}

class Aphynode_t;

class Brqg_t {
 private:
  Aquery_t &q;
  int generate_seed (void);
  long int random_number (long int high, long int low = 0);
  int random_attr (int relnum, Acat_t::attr_type_t t = Acat_t::bogus_adt ());
  Aptree_t random_ptree (void);

 public:
  Brqg_t (Aquery_t &query) : q (query) {}
  void generate (int nrels, int npreds);
				// generate a random query
  void parse (istream &is);	// parse query from in-stream
  void unparse (ostream &os) const;
};


extern void Bexecute_plan (Aphynode_t *node);
				// defined in Bexecinfo.c

#endif /* BBOGUS_H */
