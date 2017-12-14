#include <assert.h>

#include <Aphyprop.h>
#include <Aquery.h>
#include <Aphynode.h>
#include <Aopinc.h>
#include <Aglob_vars.h>
#include <Aset.h>

extern long int Ainmem_obj_size (Aptree_t *oprn);

long int Ainmem_ass_obj_size (Aset_t<Aptree_t> &operations)
{
  long int size = 0;
  FOR_EACH_ELEMENT_OF_SET (operations)
    {
      Aptree_t *oprn = operations.CurrentElement ();
      if (oprn->is_relref () || oprn->is_attref ())
	size += Ainmem_obj_size (oprn);
    }
  return size;
}

Aphyprop_t::Aphyprop_t (Afilescan_t *algo, Aphynode_t *node)
{
  node->SetPhyProps (this);
  Aptree_t *relref_oprn = ((Aget_t *)node->GetParent ()->GetOp ())->relref ();
  _inmem_ass_obj_size = Ainmem_obj_size (relref_oprn);
  _cost.compute (algo, node);
}

Aphyprop_t::Aphyprop_t (Aindexscan_t *algo, Aphynode_t *node)
{
  node->SetPhyProps (this);

  Alognode_t *select_node = 
    ((Aidx_collapse_t *)node->GetParent ()->GetOp ())->select_node ();

  for (Alognode_t *tmpnode = select_node->Input ();
       tmpnode->GetOp ()->GetNumber () != Aget;
       tmpnode = tmpnode->Input ())
    {
      assert (tmpnode->GetOp ()->GetNumber () == Amat);
      _ops_not_in_memory.Union (((Amat_t*)tmpnode->GetOp ())->operations ());
    }

  Aset_t<Aptree_t> ops_inmem = node->GetLogProps ()->operations ();
  ops_inmem.Minus (_ops_not_in_memory);
  _inmem_ass_obj_size = Ainmem_ass_obj_size (ops_inmem);
  _cost.compute (algo, node);
}

Aphyprop_t::Aphyprop_t (Aassembly_t *algo, Aphynode_t *node)
{
  node->SetPhyProps (this);
  Aphyprop_t *inprops = node->Input ()->GetPhyProps ();

  Aset_t<Aptree_t> &ops_assembled = algo->operations ();
  
  _ops_not_in_memory = inprops->_ops_not_in_memory;
  _ops_not_in_memory.Minus (ops_assembled);

  _inmem_ass_obj_size = 
    inprops->_inmem_ass_obj_size + Ainmem_ass_obj_size (ops_assembled);
  
  _cost.compute (algo, node);
}

Aphyprop_t::Aphyprop_t (Afilter_t *algo, Aphynode_t *node)
{
  node->SetPhyProps (this);
  Aphyprop_t *inprops = node->Input ()->GetPhyProps ();
  _ops_not_in_memory = inprops->_ops_not_in_memory;
  _inmem_ass_obj_size = inprops->_inmem_ass_obj_size;
  _cost.compute (algo, node);
}


Aphyprop_t::Aphyprop_t (Ahh_join_t *algo, Aphynode_t *node)
{
  node->SetPhyProps (this);
  Aphyprop_t *leftprops = node->LeftInput ()->GetPhyProps ();
  Aphyprop_t *rightprops = node->RightInput ()->GetPhyProps ();
  
  
  _ops_not_in_memory = leftprops->_ops_not_in_memory;
  _ops_not_in_memory.Union (rightprops->_ops_not_in_memory);

  _inmem_ass_obj_size = 
    leftprops->_inmem_ass_obj_size + rightprops->_inmem_ass_obj_size;

  Ajoin_t *joinop = (Ajoin_t *)node->GetParent ()->GetOp ();
  Aset_t<Aptree_t> common_ops = joinop->undone_tuplerefs ();
  common_ops.Union (joinop->operations ());
  _inmem_ass_obj_size -= Ainmem_ass_obj_size (common_ops);

  _cost.compute (algo, node);
}

Aphyprop_t::Aphyprop_t (Ahhptr_join_t *algo, Aphynode_t *node)
{
  node->SetPhyProps (this);
  Aphyprop_t *leftprops = node->LeftInput ()->GetPhyProps ();
  Aphyprop_t *rightprops = node->RightInput ()->GetPhyProps ();

  _ops_not_in_memory = leftprops->_ops_not_in_memory;
  _ops_not_in_memory.Union (rightprops->_ops_not_in_memory);

  _inmem_ass_obj_size = 
    leftprops->_inmem_ass_obj_size + rightprops->_inmem_ass_obj_size;

  Ajoin_t *joinop = (Ajoin_t *)node->GetParent ()->GetOp ();
  Aset_t<Aptree_t> common_ops = joinop->undone_tuplerefs ();
  common_ops.Union (joinop->operations ());
  _inmem_ass_obj_size -= Ainmem_ass_obj_size (common_ops);

  _cost.compute (algo, node);
}

Aphyprop_t::Aphyprop_t (Aunnestalgo_t *algo, Aphynode_t *node)
{
  node->SetPhyProps (this);
  _inmem_ass_obj_size = node->Input ()->GetPhyProps ()->_inmem_ass_obj_size;
  _cost.compute (algo, node);
}

Aphyprop_t::Aphyprop_t (Aoutputalgo_t *algo, Aphynode_t *node)
{
  node->SetPhyProps (this);
  _inmem_ass_obj_size = node->Input ()->GetPhyProps ()->_inmem_ass_obj_size;
  _cost.compute (algo, node);
}

int Aphyprop_t::IsEqualTo (const Aphyprop_t *other) const
{
  if (!_ops_not_in_memory.IsEqualTo (other->_ops_not_in_memory))
    return 0;
  return 1;			// they are equal.
}

int Aphyprop_t::Hash (const Alogprop_t *logprops) const
{
  int hashval = 0;

  hashval += _ops_not_in_memory.Hash ();

  return hashval;
}

int Aphyprop_t::IsInteresting (const Alogprop_t *logprops) const
{
  return 0;
}
