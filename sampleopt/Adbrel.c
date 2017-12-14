#include <Adbrel.h>
#include <Aquery.h>
#include <Alognode.h>
#include <Aphynode.h>
#include <Asearch.h>

void Adbrel_t::Apply (Alognode_t *lognode)
{
  Alist_t<Aop_t> clones = Clones ();
  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (clones)
    {
      Alognode_t *newnode = new Alognode_t (clones.Element ());
      assert (newnode);
      Aglob_vars ()->search->NewNode (newnode);
    }
}

Alist_t<Aop_t> Adbrel_t::Clones (Alognode_t **inputs)
{
  Alist_t<Aop_t> clones;

  for (int i=0; i<Aglob_vars()->query->numrels (); i++)
    {
      Arel_t *r = Aglob_vars()->query->rel (i);
      Adbrel_t *newdbrel = (Adbrel_t *)Duplicate ();
      newdbrel->_rel = r;
      clones.Insert (newdbrel);
    }
  return clones;
}


void Aheapfile_t::MakePhyNodes (Alognode_t *lognode)
{
  Alist_t<Aalgo_t> clones = Clones (lognode);

  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (clones)
    assert (new Aphynode_t (lognode, clones.Element ()));
				// the phynode will add itself to the
				// phynodes of the parent...
}

Alist_t<Aalgo_t> Aindex_t::Clones (Alognode_t *lognode, Aphynode_t **inputs)
{
  Alist_t<Aalgo_t> clones;

  Arel_t *rel = ((Adbrel_t *)lognode->GetOp ())->rel ();
				// relation for which this node is being made

  if (Aglob_vars ()->query->has_empty_where_clause ())
    return clones;		// if the where clause is empty then we
				// prefer a sequential filescan.
  Aset_t<Aattr_t> relattrs = rel->attrs ();
  FOR_EACH_ELEMENT_OF_SET (relattrs)
    {		
				// we make one clone for each indexed attribute
				// of this relation WHICH HAS BEEN REFERENCED
				// in the query. Note that if an attribute is
				// not mentioned anywhere in a query,
				// then an indexed filescan on that attribute 
				// wont be of any help at all! hence there is
				// no need to make an Aindex_t node for that
      Aattr_t *a = relattrs.CurrentElement ();
      if (a->is_indexed ())
	{
	  Aindex_t *newindex = (Aindex_t *)Duplicate ();
	  newindex->_indexattr = a;
	  clones.Insert (newindex);
	}
    }
  return clones;
}

void Aindex_t::MakePhyNodes (Alognode_t *node)
{
  Alist_t<Aalgo_t> clones = Clones (node);

  FOR_EACH_ELEMENT_OF_LIST_DO_WITH_DELETION (clones)
      assert (new Aphynode_t (node, clones.Element ()));
				// it will insert itself into the phynodes
				// of its parent
}
