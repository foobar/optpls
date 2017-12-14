#ifndef AOP_H
#define AOP_H

#include <Aopdefs.h> // supplied by DBI

#include <stdlib.h>

#include <Alist.h>

class Alogprop_t; // to take care of 
class Aalgo_t;	    // the forward 
class Alognode_t;	    // references.

/***************************************************************************

  THE OPERATOR BASE CLASS

  from this base class will be derived classes for all the operators
  in the algebra.
  we have derived classes Aunaryop_t and Abinop_t
  to factor out common code for these classes of operators.

  associated with each operator we have a listofalgorithms which
  can implement this operator.
  everytime a new algorithm is defined it is expected to call
  the AddAlgorithm function and add itself to the list of algorithms
  which implement that operator.

***************************************************************************/

class Aop_t {
private:
  char *name;	// name of the operator. DBI-supplied.
  AopNumber number; // a unique number associated with this operator.
			    // we believe that this function will be useful
			    // for writing code where the DBI wants to do
			    // a switch based on operator "type"

  Alist_t<Aalgo_t> listofalgorithms; // this is a list of instances
					  // of the algorithms which can
					  // be used to implement this
					  // operator.


public:
  virtual ~Aop_t (void) {}
  Aop_t (char *newname, AopNumber n, Alist_t<Aalgo_t> l) {
    name = newname; number = n; listofalgorithms = l;
  }

  virtual Aop_t *Duplicate (void) const = 0;
		// this virtual function should return a newly
		// allocated object which is an exact replica of this one.
		// read comment in file ./README to find out why it is
		// required. the DBI has to provide this function

  char *GetName (void) {return name;}
  int GetNumber (void) {return number;}
  Alist_t<Aalgo_t> GetListOfAlgorithms (void) const {return listofalgorithms;}

  virtual void Apply (Alognode_t *);
		// this function is supposed to create logical nodes
		// corresponding to the application of this operator to
		// (all possible) sets of inputs, one of which is the
		// given node. default versions have been provided for
		// Aunaryop_t and for Abinop_t.

  virtual Alist_t<Aop_t> Clones (Alognode_t **inputs = 0);
		// this is supposed to give a list of operator objects
		// which represent different ways of applying this operator
		// to these inputs, but with different parameters
		// DBI is supposed to redefine this function if s/he 
		// doesnt like the default behaviour

  virtual int Arity (void) const = 0;	
		// e.g. 1 for unary and 2 for binary ops.


  virtual Alogprop_t *MakeLogProps (Alognode_t *) = 0;
		// make the logical properties for the given logical node
		// this just involves a call to the Alogprop_t
		// with the right parameters.
};



class Aunaryop_t : public Aop_t {
private:
public:
  virtual ~Aunaryop_t (void) {}
  Aunaryop_t (char *name, AopNumber n, Alist_t<Aalgo_t> a)
    : Aop_t (name, n, a) {}

  int Arity (void) const {return 1;} // because its a unary operator

  virtual void Apply (Alognode_t *input); 
		// Apply operator to given input to create a new
		// node in the search tree

  virtual Alist_t<Aunaryop_t> Clones (Alognode_t *input);
		// this is supposed to give a list of operator objects
		// which represent different ways of applying this operator
		// to this input, but with different parameters
		// DBI is supposed to redefine this function if s/he 
		// doesnt like the default behaviour
		// see comment at function definition in Aop.c
};

class Abinop_t : public Aop_t {
private:
  void DfsNode (Alognode_t *, Alognode_t *);
		// private function used by Abinop_t::Apply

public:
  virtual ~Abinop_t (void) {}
  Abinop_t (char *name, AopNumber n, Alist_t<Aalgo_t> a)
    : Aop_t (name, n, a) {}

  int Arity (void) const {return 2;} // because it a dyadic operator.

  virtual void Apply (Alognode_t *input); 
		// apply this opeartor to the given input. this input
		// will serve as one of the inputs for this operator.
		// the other inputs will have to be found in the search tree.
		// the FindOtherInputs function does this job.

  virtual Alist_t<Abinop_t> Clones (Alognode_t *leftinput,
				       Alognode_t *rightinput);
		// this is supposed to give a list of operator objects
		// which represent different ways of applying this operator
		// to this input, but with different parameters
		// DBI is supposed to redefine this function if s/he 
		// doesnt like the default behaviour
		// see comment at the definition of the
		// Aunaryop_t::Clones function in Aop.c
};

#endif /* AOP_H */
