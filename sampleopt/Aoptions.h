#ifndef AOPTIONS_H
#define AOPTIONS_H

class Aoptimizeroptions_t {
 public:
  int left_deep_only;
  int right_deep_only;
  int delay_cartesian_products;
  int do_merge_join;
  int do_hash_join;
  int do_idx_heuristic;
  int no_equivalences;
  int all_operator_trees;
  int accuracy;
  int dont_split_lists;

  int dont_use_assembly;

  int do_exhaustive_selects;
  int do_select_collapse;
  int do_mat_collapse;
  int do_path_indexes;
  int do_mat_to_join;
  int do_inverse_links;


  Aoptimizeroptions_t (void) {
    left_deep_only = right_deep_only = 0;
    delay_cartesian_products = 1;
    do_merge_join = do_hash_join = do_idx_heuristic = no_equivalences = 0;
    all_operator_trees = accuracy = dont_split_lists = 0;
    do_exhaustive_selects = 1;
    do_select_collapse = 1;
    do_mat_collapse = 1;
    do_path_indexes = 1;
    do_mat_to_join = 1;
    do_inverse_links = 1;
    dont_use_assembly = 0;
  }
};


#endif /* AOPTIONS_H */
