#include <Asearch.h>
#include <Aquery.h>
#include <Aglob_vars.h>
#include <Alogprop.h>
#include <Aopalgos.h>

#include <assert.h>

void Asearch_t::MakeInitialTree (void)
{
  optimumnode = 0;
  
  Aglob_vars()->opalgo->get->Apply ();
}

Alist_t<Aop_t> &Asearch_t::GetListOfOperatorsToApply (Alognode_t *node)
{
//  Alist_t<Aop_t> emptylist;

//  if (node->GetOp ()->GetNumber () == Aoutputop)
//    return emptylist;		// no more operators to be applied.
//  else
    return Aglob_vars()->opalgo->all_operators;
}
