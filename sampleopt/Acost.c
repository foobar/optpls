#include <Aphynode.h>

#include <Acost.h>
#include <Aopinc.h>
#include <Aphyprop.h>

#include <Arel.h>
#include <Aattr.h>

double memory_per_operator = 125e0; // in terms of pages

const double page_size = 4096e0;
const double effective_page_size = 4008e0;
const double page_io_cost = 20e-3;
const double latency_and_transfer_cost = 10e-3;
const double cost_per_instruction = 5e-8;
const double copy_cost = 1e0*cost_per_instruction;
const double compare_cost = 2e0*cost_per_instruction;
const double index_fan_out = 179e0;
const double min_car = 1;
const double disk_cylinders = 220e0;
const double hash_cost = 5e1*cost_per_instruction;
const double buffer_cost = 1000e0; // in terms of pages
const double cluster_size = 8e0; // in terms of pages
const double fan_out = floor (memory_per_operator/cluster_size);
const double build = 4e0*cost_per_instruction;
const double avg_length_hash_chain = 3e0;
const double ptrs_per_node = 243e0;
const double infinite_cardinality = 1e20; // wonder what this is for...
const double log_index_fan_out = log (index_fan_out);
const double instrs_per_page = 1000e0;
const double instrs_per_and = 100e0;

template <class T> 
T Amin (T t1, T t2)
{
  return t1 < t2 ? t1 : t2;
}

long int Ainmem_obj_size (Acat_t::attr_type_t otypeid)
{
  long int cat_size = Aglob_vars()->cat->objsize (otypeid);
  long int num_refs = Aglob_vars()->cat->numrefs (otypeid);
  return cat_size + 64 + 2*num_refs*12;
}

long int Ainmem_obj_size (Aptree_t *operation)
{
  Acat_t::attr_type_t otypeid;
  if (operation->is_relref ())
    {
      Arelref_t *relref = (Arelref_t *)operation->func ();
      otypeid = relref->rel ()->otypeid ();
    }
  else if (operation->is_attref ())
    {
      Aattref_t *attref = (Aattref_t *)operation->func ();
      otypeid = attref->attr ()->type ();
    }

  return Ainmem_obj_size (otypeid);
}

inline double Anumpages_compute (double card, long int size)
{
  double objs_per_page = effective_page_size/size;
  if (objs_per_page < 1)
    return card*ceil (1.0/objs_per_page);
  else 
    return ceil (card/floor (objs_per_page));
}

double Anumpages_on_disk (Acat_t::attr_type_t typ, double cardinality)
{
  long int typ_size = Ainmem_obj_size (typ);
  double num_nodes = ceil (cardinality/(double)ptrs_per_node);
  double member_accesses = Anumpages_compute (cardinality, typ_size);
  return num_nodes+member_accesses;
}

int Adistance_to_inmem_obj (Aptree_t *operation, Aset_t<Aptree_t> hierarchy)
{
  assert (operation->is_attref () ||
	  operation->func ()->type () == Afunc_t::tupleref);
  Aptree_t *depends_on = operation->arg (0);
  
  if (hierarchy.HasElement (depends_on))
    return Adistance_to_inmem_obj (depends_on, hierarchy) + 1;
  else
    return 1;
}

int Aget_depth_of_hierarchy (Aset_t<Aptree_t> &hierarchy)
{
  int max_depth = -1;
  FOR_EACH_ELEMENT_OF_SET (hierarchy)
    {
      int curr_depth = Adistance_to_inmem_obj (hierarchy.CurrentElement (),
					       hierarchy);
      if (max_depth < curr_depth) max_depth = curr_depth;
    }
  return max_depth;
}


void Acost_t::compute (Afilescan_t *algo, Aphynode_t *node)
{
  double cardinality = node->GetParent ()->GetLogProps ()->numtuples ();
  Acat_t::attr_type_t typ = 
    ((Aget_t *)node->GetParent ()->GetOp ())->rel ()->otypeid ();
  double numpages = Anumpages_on_disk (typ, cardinality);
  
  _cost = numpages*page_io_cost + 
    numpages*instrs_per_page*cost_per_instruction;

  assert (_cost >= 0);
}

void Acost_t::compute (Aindexscan_t *algo, Aphynode_t *node)
{
  Acat_t::relid_t base_set = Aglob_vars()->cat->base_set (algo->idxid ());
  Acat_t::attr_type_t base_set_type = Aglob_vars()->cat->otypeid (base_set);
  double cardinality = node->GetParent ()->GetLogProps ()->numtuples ();

  double numpages = Anumpages_on_disk (base_set_type, cardinality);
  
  double idxcardinality = Aglob_vars()->cat->numtuples (algo->idxid ());
  double numidxpages = ceil (log (idxcardinality)/log_index_fan_out) +
    ceil (cardinality/index_fan_out - 1);

  double cost_read_from_disk = numpages*page_io_cost +
    instrs_per_page*numpages*cost_per_instruction;
  
  double cost_read_index_pages = numidxpages*page_io_cost +
    instrs_per_page*numidxpages*cost_per_instruction;

  int num_ands_plus_one = 1;	// for indexscan...
  double cost_eval_pred = (double)(num_ands_plus_one)*
    instrs_per_and*cost_per_instruction*cardinality;

  _cost = cost_read_from_disk+cost_read_index_pages+cost_eval_pred;

  assert (_cost >= 0);
}

void Acost_t::compute (Afilter_t *algo, Aphynode_t *node)
{
  int num_ands_plus_one = 
    ((Aselect_t *)node->GetParent ()->GetOp ())->operations ().Cardinality ();

//  double cardinality = node->Input ()->GetLogProps ()->numtuples ();

  // this is what o3db does
  double cardinality = node->GetLogProps ()->numtuples ();


  _cost = (double)(num_ands_plus_one)*
    instrs_per_and*cardinality*cost_per_instruction;
  
  _cost += node->Input ()->GetPhyProps ()->GetCost ()._cost;
  assert (_cost >= 0);
}

void Acost_t::compute (Aassembly_t *algo, Aphynode_t *node)
{
  Alogprop_t *my_logprops = node->GetLogProps ();
  Aphyprop_t *my_phyprops = node->GetPhyProps ();

  long int my_size = my_phyprops->inmem_ass_obj_size ();
  
  int window_size = (int)floor (memory_per_operator*page_size/(0.5*my_size));
  if (window_size <= 0) window_size = 1;

  Aset_t<Aptree_t> ops_to_assemble = algo->operations ();

  int depth_of_hierarchy = Aget_depth_of_hierarchy (ops_to_assemble);
  double size_of_input_stream = node->Input ()->GetLogProps ()->numtuples ();
  int components_to_assemble = ops_to_assemble.Cardinality ();
  double open_refs = Amin ((double)window_size*(double)components_to_assemble/
			   (double)depth_of_hierarchy, size_of_input_stream);
  double total_fetches = 0;
  
  FOR_EACH_ELEMENT_OF_SET (ops_to_assemble)
    {
      double extent_cardinality = infinite_cardinality;
      Aptree_t *oprn = ops_to_assemble.CurrentElement ();
      if (oprn->is_relref ())
	{
	  assert (oprn->func ()->type () == Afunc_t::tupleref);
	  Arel_t *rel = ((Arelref_t *)oprn->func ())->rel ();
	  if (rel->has_extent_or_set ())
	    extent_cardinality = rel->numtuples ();
	}
      else if (oprn->is_attref ())
	{
	  Aattr_t *attr = ((Aattref_t *)oprn->func ())->attr ();
	  Acat_t::relid_t attr_extent = attr->attr_extent ();
	  if (!Aglob_vars()->cat->relid_is_invalid (attr_extent))
	    extent_cardinality = Aglob_vars()->cat->numtuples (attr_extent);
	}
      else
	assert (!"huh?");

      total_fetches += (int) Amin (extent_cardinality, size_of_input_stream);
    }

  assert (total_fetches > 0);

  double prob_no_seek = 1.0 - pow ((1.0-(1.0/disk_cylinders)), open_refs);
  double io_cost = total_fetches*prob_no_seek*latency_and_transfer_cost +
    total_fetches*(1.0-prob_no_seek)*page_io_cost;
  double cpu_cost = total_fetches*instrs_per_page*cost_per_instruction;
  
  _cost = io_cost + cpu_cost;

  _cost += node->Input ()->GetPhyProps ()->GetCost ()._cost;
  assert (_cost >= 0);
}

void Acost_t::compute (Ahh_join_t *algo, Aphynode_t *node)
{
  Aphyprop_t *leftphyprops = node->LeftInput ()->GetPhyProps ();
  Aphyprop_t *rightphyprops = node->RightInput ()->GetPhyProps ();

  long int Rsize = leftphyprops->inmem_ass_obj_size ();
  long int Ssize = rightphyprops->inmem_ass_obj_size ();

  double Rcard = node->LeftInput ()->GetLogProps ()->numtuples ();
  double Scard = node->RightInput ()->GetLogProps ()->numtuples ();

  double Rpages = Anumpages_compute (Rcard, Rsize);
  double Spages = Anumpages_compute (Scard, Ssize);
  
  double recursion_levels = log (Rpages/memory_per_operator)/log (fan_out);
  if (recursion_levels < 0.0) recursion_levels = 0.0;

  double twice_recursion_levels = recursion_levels * 2;
  double recursion_levels_plus_one = recursion_levels + 1;

  double total_pages = Rpages + Spages;
  double total_ios = twice_recursion_levels*total_pages/cluster_size;
  double io_cost = total_ios*page_io_cost;

  double cpu_cost = 
    twice_recursion_levels*total_pages*page_size*copy_cost +
    recursion_levels_plus_one*(Rcard+Scard)*hash_cost +
    total_ios*instrs_per_page*cost_per_instruction +
    recursion_levels_plus_one*avg_length_hash_chain*Scard*compare_cost +
    recursion_levels_plus_one*Rsize*Rcard*build;

  _cost = io_cost + cpu_cost;

  _cost += 
    leftphyprops->GetCost ()._cost + rightphyprops->GetCost ()._cost;
				// cost of computing the inputs.

  assert (_cost >= 0);
}

void Acost_t::compute (Ahhptr_join_t *algo, Aphynode_t *node)
{
  Aphyprop_t *leftphyprops = node->LeftInput ()->GetPhyProps ();
  Aphyprop_t *rightphyprops = node->RightInput ()->GetPhyProps ();

  long int Rsize = leftphyprops->inmem_ass_obj_size ();
  long int Ssize = rightphyprops->inmem_ass_obj_size ();

  double Rcard = node->LeftInput ()->GetLogProps ()->numtuples ();
  double Scard = node->RightInput ()->GetLogProps ()->numtuples ();

  double Rpages = Anumpages_compute (Rcard, Rsize);
  double Spages = Anumpages_compute (Scard, Ssize);
  
  double recursion_levels = log (Rpages/memory_per_operator)/log (fan_out);
  if (recursion_levels < 0.0) recursion_levels = 0.0;

  double twice_recursion_levels = recursion_levels * 2;
  double recursion_levels_plus_one = recursion_levels + 1;

  double total_pages = Rpages + Spages;
  double total_ios = twice_recursion_levels*Rpages/cluster_size +
    Amin (Spages, Rcard);
  double io_cost = total_ios*page_io_cost;

  double cpu_cost = 
    twice_recursion_levels*Rpages*page_size*copy_cost +
    (recursion_levels_plus_one*Rcard+Scard)*hash_cost +
    total_ios*instrs_per_page*cost_per_instruction +
    avg_length_hash_chain*Scard*compare_cost +
    recursion_levels_plus_one*Rsize*Rcard*build;

  _cost = io_cost + cpu_cost;

  _cost += leftphyprops->GetCost ()._cost;
				// cost of the right input is not counted.

  assert (_cost >= 0);
}

void Acost_t::compute (Aunnestalgo_t *algo, Aphynode_t *node)
{
  _cost = node->Input ()->GetPhyProps ()->GetCost ()._cost;
				// cost of computing input

  Aptree_t *oprn = 
    ((Aunnestop_t *)node->GetParent ()->GetOp ())->
    operations ().FirstElement ();

  long int avg_set_size;
  if (oprn->is_attref ())
    {
      avg_set_size = 
	((Aattref_t *)oprn->func ())->attr ()->avg_set_size ();
    }
  else if (oprn->is_tupleref ())
    {
      avg_set_size =
	Aglob_vars()->cat->
	avg_set_size (((Atupleref_t *)oprn->func ())->attrid ());
    }
  
  _cost += ((double)(avg_set_size - 1))*
    node->GetLogProps ()->numtuples ()*
    copy_cost;

  assert (_cost >= 0);
}

void Acost_t::compute (Aoutputalgo_t *algo, Aphynode_t *node)
{
  _cost = node->Input ()->GetPhyProps ()->GetCost ()._cost;
				// cost of computing input

  assert (_cost >= 0);
}


void Acost_t::write (ostream &os) const
{
  os << _cost;
}
