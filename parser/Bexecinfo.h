#ifndef BEXECUTION_H
#define BEXECUTION_H

#include <Aset.h>
#include <Aattr.h>
#include <Apred.h>
#include <Aalgo.h>
#include <Alogprop.h>
#include <Aphynode.h>

class Aalgo_t;
class Aop_t;
class Arelattr_t;
class Arel_t;
class ProjList;

class Bexec_info_t {
private:
  Aphynode_t *_node;
  Aalgo_t *_algo;
  Aop_t *_op;
  int _indent;
  Aset_t<Aattr_t> _proj_attrs;
  int _topnode;			// true if this is the top node.
  char _res_filename[A_MAXNAMELEN];
  char _suffix[A_MAXNAMELEN];
  int _tmp_flag;		// true if _res_filename is a temporary file
  
  int _num_inputs;
  Bexec_info_t *_inputs;
  int _error;			// will be true if there was an error

public:
  Bexec_info_t (void);
  ~Bexec_info_t (void);
  void init (Aphynode_t *n, Aset_t<Aattr_t> &pattrs, int topnode, int ind);

  Aphynode_t *node (void) const {return _node;}
  Aalgo_t *algo (void) const {return _algo;}
  Aop_t *op (void) const {return _op;}
  int indent (void) const {return _indent;}
  const char *suffix (void) const {return _suffix;}
  int tmp_flag (void) const {return _tmp_flag;}
  int num_inputs (void) const {return _num_inputs;}
  const Bexec_info_t &input (int N) const {return _inputs[N];}
  const char *res_filename (void) const {return _res_filename;}
  int error (void) const {return _error;}

  void set_error (int e) {_error = e;}
  void set_res_filename (const char *f);
  Arelattr_t *make_relattr (const Aattr_t *attr) const;
  void add_suffix (char *string, const Arel_t *rel) const;
  Aset_t<Aattr_t> output_attrs (void) const;

  void compute_inputs_for_select_operator (void);
  void compute_inputs_for_join_operator (void);
  void compute_inputs_for_simple_operator (void);

  Aptree_t set_to_ptree (const Aset_t<Apred_t> &preds);
  void make_projlist (ProjList &projlist) const;
  void output_in_tmp_file (void);
  void delete_inputs (void) {delete [] _inputs; _inputs = 0;}

  void execute (void) {_algo->Execute (*this);}
};

/***************************************************************************

  inline functions

***************************************************************************/

inline void Bexec_info_t::add_suffix (char *string, const Arel_t *rel) const
{
  strcat (string, "$"); strcat (string, rel->rel_var ());
}

inline Aset_t<Aattr_t> Bexec_info_t::output_attrs (void) const
{
  Aset_t<Aattr_t> set;
  return set;
//  return node ()->GetLogProps ()->attrs ();
}

inline void Bexec_info_t::set_res_filename (const char *f)
{
  strncpy (_res_filename, f, sizeof (_res_filename));
}

#endif /* BEXECUTION_H */
