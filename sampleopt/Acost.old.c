#include <Aphynode.h>

#include <Acost.h>
#include <Aopinc.h>
#include <Aphyprop.h>

#include <Arel.h>
#include <Aattr.h>

void Acost_t::compute (Afilescan_t *algo, Aphynode_t *node)
{
  _cost = node->Input ()->GetPhyProps ()->GetCost ()._cost;
				// cost of computing the input
  _cost += node->Input ()->GetLogProps ()->numpages (); 
				// cost of reading in the input pages
  _cost += node->GetLogProps ()->numpages ();
				// cost of writing out the output pages
  assert (_cost >= 0);
}

void Acost_t::compute (Aindexscan_t *algo, Aphynode_t *node)
{
  Arel_t *rel = ((Aselect_t *)node->GetParent ()->GetOp ())->rel ();
  Aattr_t *indexattr = ((Aindex_t *)node->Input ()->GetAlgo ())->indexattr ();

  _cost = node->Input ()->GetPhyProps ()->GetCost ()._cost;
				// cost of computing the input.

  double totalpages = (double) 
    (indexattr->is_clustered () ? rel->numpages () : rel->numtuples ());
				// clustered index ==> each page read only once
				// unclustered ==> one page read per tuple
  double idx_selectivity = 
    algo->indexpred ()->selectivity (node->GetParent ());
  
  _cost += totalpages * idx_selectivity;
  _cost += node->GetLogProps ()->numpages ();
				// plus cost of writing out the pages
  assert (_cost >= 0);
}


void Acost_t::compute (Aloopsjoin_t *algo, Aphynode_t *node)
{
  Aphyprop_t *outerphyprops = node->LeftInput ()->GetPhyProps ();
  Aphyprop_t *innerphyprops = node->RightInput ()->GetPhyProps ();

  Alogprop_t *outerlogprops = node->LeftInput ()->GetLogProps ();
  Alogprop_t *innerlogprops = node->RightInput ()->GetLogProps ();


  double outertuples = outerlogprops->numtuples ();
  double innertuples = innerlogprops->numtuples ();

  _cost = 
    node->LeftInput ()->GetPhyProps ()->GetCost ()._cost + 
    node->RightInput ()->GetPhyProps ()->GetCost ()._cost;
				// cost of computing the inputs.

  double outerpages = node->LeftInput ()->GetLogProps ()->numpages ();
  double innerpages = node->RightInput ()->GetLogProps ()->numpages ();
  _cost += outerpages + outerpages * innerpages;
				// scan inner once for each page of the outer.
				//
				// i should actually use the value of 
				// the blocksize since this is going 
				// be a block-nested-loops.

  _cost += node->GetLogProps ()->numpages ();
				// cost of writing the output
  assert (_cost >= 0);
}

void Acost_t::compute (Anlidxjoin_t *algo, Aphynode_t *node)
{
  Aphyprop_t *outerphyprops = node->LeftInput ()->GetPhyProps ();
  Aphyprop_t *innerphyprops = node->RightInput ()->GetPhyProps ();

  Alogprop_t *outerlogprops = node->LeftInput ()->GetLogProps ();
  Alogprop_t *innerlogprops = node->RightInput ()->GetLogProps ();

  double outerpages = outerlogprops->numpages ();
  double innerpages = innerlogprops->numpages ();

  double outertuples = outerlogprops->numtuples ();
  double innertuples = innerlogprops->numtuples ();

  _cost = outerphyprops->GetCost ()._cost + innerphyprops->GetCost ()._cost;
				// cost of computing the inputs.

  Apred_t *idxjoin_pred = algo->pred ();
  assert (idxjoin_pred);
  assert (node->RightInput ()->GetAlgo ()->GetNumber () == Aindex);
  Aattr_t *indexattr = 
    ((Aindex_t *)node->RightInput ()->GetAlgo ())->indexattr ();
  assert (indexattr);

  double idx_selectivity = idxjoin_pred->selectivity (node->GetParent ());
  double inner_cost_per_outer_tuple = 
    indexattr->is_clustered () ? innerpages*idx_selectivity*2 :
    innertuples*idx_selectivity*2;
				// i am assuming there there are 2 page
				// references per tuple required for a
				// non clustered index scan.
				// for a clustered index i assume two
				// page references per page of inner
				//
				// 2 pages == one index page + one data page.

  _cost += outerpages + outertuples*inner_cost_per_outer_tuple;
				// cost of doing the join

  _cost += node->GetLogProps ()->numpages ();
				// cost of writing out the output.
  assert (_cost >= 0);
}

void Acost_t::compute (Amergejoin_t *algo, Aphynode_t *node)
{
  double outerpages = node->LeftInput ()->GetLogProps ()->numpages ();
  double innerpages = node->RightInput ()->GetLogProps ()->numpages ();

  _cost = 
    node->LeftInput ()->GetPhyProps ()->GetCost ()._cost + 
    node->RightInput ()->GetPhyProps ()->GetCost ()._cost;
				// cost of computing the inputs.

  _cost += outerpages + innerpages;
				// cost of the mergejoin
  _cost += node->GetLogProps ()->numpages ();
				// cost of writing out the output.
  assert (_cost >= 0);
}

void Acost_t::compute (Ahashjoin_t *algo, Aphynode_t *node)
{
  double outerpages = node->LeftInput ()->GetLogProps ()->numpages ();
  double innerpages = node->RightInput ()->GetLogProps ()->numpages ();

  _cost = 
    node->LeftInput ()->GetPhyProps ()->GetCost ()._cost + 
    node->RightInput ()->GetPhyProps ()->GetCost ()._cost;
				// cost of computing the inputs.

  _cost += 3*(outerpages + innerpages);
				// cost of the mergejoin
  _cost += node->GetLogProps ()->numpages ();
				// cost of writing out the output.
  assert (_cost >= 0);
}

void Acost_t::compute (Aaggralgo_t *algo, Aphynode_t *node)
{
  _cost = node->Input ()->GetPhyProps ()->GetCost ()._cost;
				// cost of computing input
  _cost += node->Input ()->GetLogProps ()->numpages ();
				// reading in input pages
  _cost += node->GetLogProps ()->numpages ();
				// writing out output pages.
  assert (_cost >= 0);
}

void Acost_t::compute (Aoutputalgo_t *algo, Aphynode_t *node)
{
  _cost = node->Input ()->GetPhyProps ()->GetCost ()._cost;
				// cost of computing input

// let us ignore these costs because they are common to 
// all algorithms
//
//
//  _cost += node->Input ()->GetLogProps ()->numpages ();
//				// reading in input pages
//  _cost += node->GetLogProps ()->numpages ();
//				// shipping output pages.

  assert (_cost >= 0);
}

void Acost_t::compute (Asort_t *algo, Aphynode_t *node)
{
  _cost = node->Input ()->GetPhyProps ()->GetCost ()._cost;
				// cost of computing input
  double numpages = node->Input ()->GetLogProps ()->numpages ();

  _cost += 4*numpages; 
				// cost of the sort.
  _cost += node->GetLogProps ()->numpages ();
				// writing out the output pages.
  assert (_cost >= 0);
}

void Acost_t::write (ostream &os) const
{
  os << _cost;
}
