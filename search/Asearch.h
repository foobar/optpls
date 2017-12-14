#ifndef ASEARCH_H
#define ASEARCH_H

#include <iostream.h>

#include <Aglob_vars.h>


#include <string.h>
#include <Alist.h>
#include <Aop.h>
#include <Alognode.h>
#include <Aphynode.h>

#define ADEBUG // normally this should be OFF.

#ifdef ADEBUG

class Anodecounter_t {
private:
  int tot_nodes;		// total number of nodes created
  int cur_nodes;		// number of nodes in tree currently
  int max_nodes;		// maximum nodes coexisting at a given time
public:
  Anodecounter_t (void) {reset ();}
  ~Anodecounter_t (void) {}

  void reset (void) {tot_nodes = cur_nodes = max_nodes = 0;}
  void add_node (void) {
    tot_nodes++; cur_nodes++;if (cur_nodes>max_nodes) max_nodes=cur_nodes;
  }
  void delete_node (void) {cur_nodes--;}
  void write (ostream &os) const {
    os << "Total: " << tot_nodes << "\tMax: " << max_nodes;
  }

  int tot (void) const {return tot_nodes;}
  int max (void) const {return max_nodes;}
};

inline ostream &operator<< (ostream &os, const Anodecounter_t &n)
{
  n.write (os); return os;
}
#endif

const int Amax_operations = 100;

class Asearch_t {
private:
  Alist_t<Alognode_t> listofrootnodes; 
				// should be renamed
				// listofatomicnodes someday.
  Alist_t<Alognode_t> listofunexpandednodes[Amax_operations];
				// array of lists.
				// each list contains a list of unexpanded
				// nodes with that many operations.
  int unexpanded_nodes_array_index;
				// index into above array.
				// keeps track of which list is being
				// expanded currently

  Alognode_t *optimumnode;		   

public:
  
#ifdef ADEBUG
  Anodecounter_t lognode_ctr;	// some performance statistics
  Anodecounter_t phynode_ctr;	
  Anodecounter_t subopt_lognode_ctr;
  Anodecounter_t subopt_phynode_ctr;
#endif

  Asearch_t (void) {unexpanded_nodes_array_index = 0;}
  ~Asearch_t (void) {}

  void NewNode (Alognode_t *node);
  void AddNodeToTree (Alognode_t *node);

  Alist_t<Alognode_t> &GetListOfRootNodes (void) {return listofrootnodes;}
  void DeleteLogNode (Alognode_t *lognode);	// remove from tree.
  void ExpandNode (Alognode_t *);			
  Aphynode_t *Optimize (void);

  void MakeInitialTree (void); // DBI is expected to write this function
  Alist_t<Aop_t> &GetListOfOperatorsToApply (Alognode_t *);
		// DBI is expected to write this function

  void Initialize (void);	// call before each query
  void CleanUp (void);		// call after each query
};       

#endif /* ASEARCH_H */
