#ifndef ACOST_H
#define ACOST_H

#include <Aopdefs.h>
#include <iostream.h>

class Aphynode_t;

class Acost_t {
private:
  double _cost;
public:
  Acost_t (void) {_cost = -1;}
  ~Acost_t (void) {}
  
  int operator< (const Acost_t &other) {return _cost < other._cost;}
  int operator<= (const Acost_t &other) {return _cost <= other._cost;}
  int operator> (const Acost_t &other) {return _cost > other._cost;}
  int operator>= (const Acost_t &other) {return _cost >= other._cost;}
  int operator== (const Acost_t &other) {return _cost == other._cost;}
  int operator!= (const Acost_t &other) {return _cost != other._cost;}

  void compute (Afilescan_t *algo, Aphynode_t *node);
  void compute (Aindexscan_t *algo, Aphynode_t *node);
  void compute (Afilter_t *algo, Aphynode_t *node);
  void compute (Aassembly_t *algo, Aphynode_t *node);
  void compute (Ahh_join_t *algo, Aphynode_t *node);
  void compute (Ahhptr_join_t *algo, Aphynode_t *node);
  void compute (Aunnestalgo_t *algo, Aphynode_t *node);
  void compute (Aoutputalgo_t *algo, Aphynode_t *node);

  void write (ostream &os) const;
};

inline ostream &operator<< (ostream &os, const Acost_t &cost)
{
  cost.write (os); return os;
}

#endif /* ACOST_H */
