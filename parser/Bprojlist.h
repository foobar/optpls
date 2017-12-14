#ifndef BPROJLIST_H
#define BPROJLIST_H

#include <Aconsts.h>
#include <assert.h>

class Aptree_t;

class ProjList {
public:
  class projelem_t {
  public:
    char out_attrname[A_MAXNAMELEN];
    Aptree_t *ptree;
  };

private:
  int num_elems;
  projelem_t *elems;
  int curr_elem;

public:
  ProjList (void) {num_elems = curr_elem = 0; elems = 0;}
  ~ProjList (void) {delete [] elems;}
  void initialize (int n);
  void put (Aptree_t *ptree, char *out_attrname);
  const projelem_t &get (int elem_num);
  int num (void) const {return num_elems;}
  int null_projlist (void) const {return num_elems == 0;}
  void check (void) const {assert (curr_elem == num_elems);}
};


#endif /* BPROJLIST_H */
