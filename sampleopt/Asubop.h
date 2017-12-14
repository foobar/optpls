#ifndef ASUBOP_H
#define ASUBOP_H

class Asubop_t {
private:
  int _number;			// index into array of sub-ops in Aquery_t
  Apred_t &_pred;		// part of pred that is represented by this.

public:


  // functions for the Aset_t<Asubop_t> class
  static int TotalNumber (void);
  static Aattr_t *NthElement (int N);
  int Number (void) const {return _number;}
};


// inline functions


#endif /* ASUBOP_H */
