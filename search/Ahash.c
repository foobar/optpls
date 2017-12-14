#include "Ahash.h"
#include "Alognode.h"
#include "Alogprop.h"
#include "Aphynode.h"
#include "Aphyprop.h"
#include "Aglob_vars.h"

#include <iostream.h>
#include <stdlib.h>

/***************************************************************************

  BEWARE!!!

  do not make any changes to this file unless you are very
  very sure of what you are doing.

***************************************************************************/
/***************************************************************************

  PRUNING.

  whenever a new physical node is created 
  there is potential for ii_cost based pruning.

  remember that each physical node stands for an access plan
  (for whatever partial operator tree it is supposed to be
  implementing).

  so if there exists a physical node in our search tree which
  produces equivalent output as the new node (i.e. all the 
  logical properties are same AND the physical properties are also 
  the same or the physical properties are NOT INTERESTING) then 
  we keep only the the less expensive one around
  to be considered for further optimization. the more expensive
  one is pruned out. we call this an EXACT MATCH.
  in this case, if the new node is less expensive then it
  REPLACEs the older one in the search tree or if it is more 
  expensive then (since it is not useful to anyone) it commits SUICIDE
  by deleting itself.
  
  if no exact match is found we continue with pruning.

  if there is a physical node whose logical properties are the
  same as the new node and its physical properties are not
  but its physical properties are NOT INTERESTING
  and it is more expensive than the new node then that node
  can be pruned out (because the new node can provide everything
  that the old one provided (and more) at a lesser ii_cost).
  the desicion of whether the physical properties of a node are
  INTERESTING or not is left to the DBI (database implementor)
  through the use of the Aphynode_t::IsInteresting member funtion.

  in such a situation we call the new node a REPLACER (because
  it is going to remove/KILL the older node from the search tree) and
  the new node a REPLACEE.

  the situation is reversed if the physical properties if the new
  node are NOT INTERESTING. in that case if there is any already
  existing node which has the same logical properties (obviously
  it has INTERESTING physical properties) and is less expensive than
  the new node, then the new node need not be kept around for the 
  later phase of optimization. hence the new node commits SUICIDE. 

***************************************************************************/

/***************************************************************************

  HASH TABLE

  this is the data structure which makes it all work.
  to implement the pruning explained above we need a to
  be able to efficiently find
  1) the EXACT MATCH (if any) of a give physical node
  2) all REPLACERs of a physical node
  3) a REPLACEE (if any) of the physical node

  for this we have a elaborate hashing mechanism.
  a node can go to two possible buckets in the hash table.
  the first hash value is calculated using the logical properties
  AND the physical properties of the node. this bucket will
  be used for looking for EXACT MATCHes of INTERESTING nodes.
  the other hash value is calculated using just the logical properties.
  this is used to store those nodes which dont have INTERESTING
  physical properties. (hence) this is also the place to look
  for REPLACEEs. 
  when a new node is created and inserted in the hash table, 
  there are two possibilites:
  1) it is NOT INTERESTING: in this case, the logical properties
  are used to calculate the hash value and the node is inserted in
  that bucket.
  1) it is INTERESTING: in this case, the logical+physical hashvalue
  is calculated and the node is inserted in that bucket.
  in addition the logical properties are also used to calculate the
  other hash value associated and this node is inserted in the 
  listofreplacers which is maintained at that location.
  
***************************************************************************/

// we use bucket chaining for hashing
// each link of the chain in the hashtable is a Ahashnode_t

Ahashnode_t::Ahashnode_t (Aphynode_t *newnode)
{
  node = 0;
  logprops = 0;
  NewPhyNode (newnode);
  if (newnode->IsInteresting ())
    listofreplacers = 0; // an interesting node cannot have any replacers.
  else
    listofreplacers = new Alist_t<Ahashnode_t>;
}

void 
Ahashnode_t::NewPhyNode (Aphynode_t *newnode)
{
  if (node == 0)
    {
      delete logprops; // this was allocated using 
		       // LogProps::Duplicate
    }

  assert (newnode);
  node = newnode;
  phyprops = newnode->GetPhyProps ();
  logprops = newnode->GetLogProps ();
}

// this creates a dummy node whose sole purpose is to store
// the listofreplacers. this node will be inserted in the place where
// UNINTERESTING physical nodes usually go in the hashtable.
// this dummy might be required in the case where it becomes
// necessary to store a list of replacers even though there is
// no UNINTERESTING node currently in the hashtable.
Ahashnode_t::Ahashnode_t (Alogprop_t *newlogprops)
{
  node = 0;	// because this is a dummy node
  phyprops = 0;
  listofreplacers = new Alist_t<Ahashnode_t>;

  logprops = newlogprops->Duplicate ();
		// we dont want to use the "newlogprops" as
		// they are because they are likely to be
		// deleted (by the logical node).
}

Ahashnode_t::~Ahashnode_t (void)
{
  if (listofreplacers)
    listofreplacers->DeleteList ();
  delete listofreplacers;

  if (node == 0)
    {
      delete logprops; // this was allocated using 
		       // Alogprop_t::Duplicate
    }
}


/***************************************************************************

  A note on hashing.
  
  since the hashing is required to locate nodes with equivalent
  logical properties and physical properties and since these
  classes are supplied by the DBI, the hash function has to
  be implemented by the DBI.

  to do this, we require the DBI to provide us with two functions
  1) the Alogprop_t.Hash () function.
     the DBI is expected to guarantee that whenever two 
     instances of the Alogprop_t class are deemed to be
     equivalent by the Alogprop_t.IsEqualTo function then
     their Alogprop_t.Hash () values must be equal.
     further, the DBI should attempt to write this function such that
     different logprops objects return different values.
  2) the Aphyprop_t.Hash (Alogprop_t *) function.
     here two (logprops,phyprops) should give
     the same hash values if they are equivalent according to the
     Alogprop_t.IsEqualTo and the Aphyprop_t.IsEqualTo
     functions.

  these hashed numbers are rehashed by our hashtable to produce an integer
  from 0 to (arraysize - 1). in case of nodes which are not INTERESTING, 
  the Alogprop_t.Hash value is used for rehashing (because the
  physical properties are not interesting). for other nodes the
  Aphyprop_t.Hash value is used for rehashing since the physical
  properties are also important in this case.

***************************************************************************/


void 
Ahashtable_t::EmptyHashTable (void)
{
  for (int i=0; i<arraysize; i++)
    {
      FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (hasharray[i])
	delete hasharray[i].Element ();
    }
}

Ahashtable_t::Ahashtable_t (int thearraysize)
{
  arraysize = thearraysize;
  hasharray = new Alist_t<Ahashnode_t> [arraysize];
}

// here hashnumber is the physicalhashnumber if the node
// is Interesting. otherwise it is the logicalhashnumber
Ahashnode_t *
Ahashtable_t::FindMatch (Aphynode_t *node, int hashnumber)
{
  int index = Rehash (hashnumber);

  Alist_t<Ahashnode_t> list = hasharray[index];
  FOR_EACH_ELEMENT_OF_LIST (list)
    {
      Ahashnode_t *hashnode = list.Element ();


		// else statements which dont do anything have been 
		// explicitly coded in this section because it is believed
		// that they make the code more understandable.
		// they are required to get an exact idea of what
		// is happening here.

      if (node->GetLogProps ()->IsEqualTo (hashnode->logprops))
        if (!node->IsInteresting ()) // just the logical properties matter
          if (!hashnode->node)	     // this is a dummy
            return hashnode;	     // dummy and uninteresting match.
          else if (!hashnode->node->IsInteresting ()) 
            return hashnode; // two uninteresting nodes meet each other
          else
            ;	// uninteresting and interesting dont match.
		// i have kept this null statement in here just
		// to make this fact explicit.
	else if (hashnode->phyprops) // both are interesting.
	  if (hashnode->phyprops->IsEqualTo 
              (node->GetPhyProps ()))
            return hashnode; // they are exactly equivalent
          else
            ;	// both have interesting but different properties.
        else
          ;	// hashnode is a dummy but node is interesting. no match.
      else
        ;	// logical properties dont match. no match.
    }
  
  return 0;
}


// find the replacee of this node (i.e. a node which can possibly
// be replaced by this node). if a replacee does not exist, create
// a dummy replacee.
Ahashnode_t *
Ahashtable_t::FindReplacee (Aphynode_t *node, int hashnumber)
{
  int index = Rehash (hashnumber);
  Alogprop_t *logprops = node->GetLogProps ();

  Alist_t<Ahashnode_t> list = hasharray[index];

  FOR_EACH_ELEMENT_OF_LIST (list)
    {
      Ahashnode_t *hashnode = list.Element ();

      if (logprops->IsEqualTo (hashnode->logprops))
	if (hashnode->phyprops == 0 || 
	    !hashnode->node->IsInteresting ())
	  return hashnode; // this is your replacee for you
    }
  

  // there was no replacee. so we will have to create one.

  Ahashnode_t *newhashnode = new Ahashnode_t (logprops);
  hasharray[index].Insert (newhashnode);
  return newhashnode;
}


// here hashnumber is the physicalnumber if the phynode
// is Interesting. otherwise it is the logicalnumber.
void
Ahashtable_t::Insert (Ahashnode_t *hashnode, int hashnumber)
{
  int index = Rehash (hashnumber);
  hasharray[index].Insert (hashnode); // insert anywhere in the list.
}





void
Ahashid_t::InitializeHashTable (void)
{
  return;
}

void 
Ahashid_t::EmptyHashTable (void)
{
  Aglob_vars()->hashtable->EmptyHashTable ();
  return;
}

Ahashid_t::Ahashid_t (Aphynode_t *newnode)
{
  node = newnode;
  exactmatch = replacee = 0; replacers = 0;

  Alogprop_t *logprops = newnode->GetLogProps ();
  logicalhashnumber = logprops->Hash ();
  physicalhashnumber = 
    newnode->GetPhyProps ()->Hash (logprops);
}

Aphynode_t *
Ahashid_t::GetExactMatch (void)
{
  
  exactmatch = 
    Aglob_vars()->hashtable->FindMatch (node, node->IsInteresting () ? 
				  physicalhashnumber : logicalhashnumber); 
				// if the physical node is
				// interesting then use the
				// physicalhashnumber for
				// hashing. otherwise use
				// the logicalhashnumber.
				     
  if (exactmatch)
    return exactmatch->node; // this might be 0 in case exactmatch was a dummy
  else
    return 0;
}

void
Ahashid_t::Replace (void)
{
  assert (exactmatch);
  exactmatch->NewPhyNode (node); // just replace that one by this.
}

void
Ahashid_t::Insert (void)
{
  if (exactmatch) // this means that the exactmatch was a dummy
		  // so replace the stuff in the dummy with some real things
    {
      exactmatch->NewPhyNode (node); 
      return;
    }
  
  // else make a new node and fill it up with information
  exactmatch = new Ahashnode_t (node);
  Aglob_vars()->hashtable->Insert (exactmatch, // and insert in hash table
		    node->IsInteresting () ? physicalhashnumber : 
		    logicalhashnumber); // at the right place

  if (!node->IsInteresting ()) // nothing more to be done.
			       // if the node is not interesting.
    return; 

  replacee = Aglob_vars()->hashtable->FindReplacee (node, logicalhashnumber);
		// if it is interesting then it has a replacee somewhere.
  replacee->listofreplacers->Insert (exactmatch);
		// and it has to be added to the list of replacers
		// of that node.
}

Aphynode_t *
Ahashid_t::GetNextReplacer (void)
{
  if (!replacers) // if this is the first time then fetch
		  // the replacers from the hashtable
    {
      
      if ((replacers = exactmatch->listofreplacers) == 0)
	return 0; // this node is interesting. cannot be replaced. 
      else if (replacers->IsEmpty ())
	return 0; // it is replacable, but there are no replacers now.
      else
	{
	  Aglob_vars()->end_of_replacers = replacers->StartTraversal ();
	  return replacers->Element ()->node;
	}
    }
  else
    {
      assert (!replacers->IsEmpty ());
      Aglob_vars()->end_of_replacers = replacers->Advance (); // goto next replacer.
      if (Aglob_vars()->end_of_replacers)
	return 0; // no more replacers
      else
	return replacers->Element ()->node;
    }
  return 0;	// to satisfy picky compilers.
}

// this is the case where a node has been inserted but it finds out
// that there is someone who can replace it.
// so it decides to commit suicide.
void Ahashid_t::Suicide (void)
{
  assert (exactmatch->node); // this cannot be zero.

  exactmatch->logprops = exactmatch->logprops->Duplicate ();
		// because the old logprops will
		// be deleted by the logical node.

  exactmatch->node = 0; 
  exactmatch->phyprops = 0;
}

// gets a physical node which can be replaced by this node.
// we assume that there can be only one such node at any
// given time...
Aphynode_t *
Ahashid_t::GetReplacee (void)
{
  if (replacee) // if we have already located it (during insert)
    return replacee->node;
  
  if (!node->IsInteresting ())
    return 0; // there is nothing that this guy can replace

  // if you reach here then you have to locate the replacee

  replacee = Aglob_vars()->hashtable->FindReplacee (node, logicalhashnumber);
  return replacee->node;
}

void
Ahashid_t::KillReplacee (void)
{
  assert (replacee);
  assert (replacee->node); // this cannot be zero.

  replacee->logprops = replacee->logprops->Duplicate ();
		// because the old logprops will
		// be deleted by the logical node.

  replacee->node = 0; 
  replacee->phyprops = 0;
}

  
