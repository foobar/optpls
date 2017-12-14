#include <Aselect.h>
#include <Aquery.h>
#include <Adbrel.h>
#include <Alognode.h>
#include <Aphynode.h>
#include <Asearch.h>
#include <Aoptions.h>

Alist_t<Aunaryop_t> Aselect_t::Clones (Alognode_t *input)
{
  Alist_t<Aunaryop_t> clones;

  Arel_t *rel = ((Adbrel_t *)input->GetOp ())->rel ();
  Apred_t *oid_ref = Aglob_vars()->query->oid_ref (rel);

  if (oid_ref)			// if there is an oid_ref apply it immediately
    {
      Aselect_t *newselect = (Aselect_t *)Duplicate ();
      newselect->_rel = rel;
      newselect->_preds.Insert (oid_ref);
      clones.Insert (newselect);
      return clones;		// if there is an oid_ref, apply nothing else
    }
  
  Aset_t<Apred_t> relselects = Aglob_vars()->query->relselects (rel); 
				// sargable selects.
				// select predicates on this relation.
  if (!relselects.IsEmpty ())
    {
      Aselect_t *newselect = (Aselect_t *)Duplicate ();
      newselect->_rel = rel;
      newselect->_preds = relselects;
      clones.Insert (newselect);
    }

  return clones;
}

Alist_t<Aunaryalgo_t> Afilescan_t::Clones (Alognode_t *lognode,
					   Aphynode_t *input)
{
  Alist_t<Aunaryalgo_t> clones;
  if (input->GetAlgo ()->GetNumber () != Aindex)
    clones.Insert ((Aunaryalgo_t *)Duplicate ());	
				// cannot be applied to index nodes.
  return clones;
}

Alist_t<Aunaryalgo_t> Aindexscan_t::Clones (Alognode_t *lognode,
					    Aphynode_t *input)
{
  Alist_t<Aunaryalgo_t> clones;

  if (input->GetAlgo ()->GetNumber () != Aindex)
    return clones;		// cannot be applied to non-index nodes.

  Aattr_t *indexattr = ((Aindex_t *)input->GetAlgo ())->indexattr ();
  assert (indexattr);
  
  // now we have to decide which predicate to use as the index
  // predicate. we will make one clone for each predicate which
  // can be used for the index scan.

  Aset_t<Apred_t> preds = ((Aselect_t *)lognode->GetOp ())->preds ();

  FOR_EACH_ELEMENT_OF_SET (preds)
    {
      Apred_t *p = preds.CurrentElement ();
      if (p->index_scannable (indexattr))
	{
	  Aindexscan_t *newidxscan = (Aindexscan_t *)Duplicate ();
	  newidxscan->_indexpred = p;
	  clones.Insert (newidxscan);
	}
    }
  return clones;
}

