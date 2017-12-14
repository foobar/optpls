#ifndef ASELDESC_H
#define ASELDESC_H

#include <Acat.h>

class Apred_t;

/***************************************************************************

  the Aseldesc_t class is a selectivity descriptor.
      this class represents stores information about an sub-expression or
  sub-predicate required by the optimizer to estimate the selectivity
  of the predicate in which it is contained.
      in case of an attribute reference it will store distribution
  information like min-value, max-value, number-of-unique-keys etc. for
  spatial attributes it will keep the universe (bounding box).
      in case of boolean expressions (predicates) it will keep the
  selectivity of the predicate.

***************************************************************************/
class Aseldesc_t {
public:
  // for boolean expressions/ predicates
  double selectivity;

  // for attribute references and objects
  long int cardinality;
  long int uniq_keys;
  Amin_max_val_t min_val;
  Amin_max_val_t max_val;

  // if nothing is know about this
  int nothing_known;

  Aseldesc_t (void) {nothing_known = 0;}
  ~Aseldesc_t (void) {}
};

#endif /* ASELDESC_H */
