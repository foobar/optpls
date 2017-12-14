#ifndef BOPCODES_H
#define BOPCODES_H

#include <Aconsts.h>
#include <Acat.h>
#include <Aptree.h>
#include <Alist.h>
class Bastnode_t;

class Bop_t {
protected:
  const char *opname;		// for debugging purposes
public:
  Bop_t (const char *name) : opname (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual void execute (Bastnode_t &node);
  virtual void pred_to_list (Bastnode_t &node, Alist_t<Bastnode_t> &list);
  virtual Aptree_t ast_to_ptree (Bastnode_t &node);
				// last two functions defined in Bconvert.c
  int are_args_literal (Bastnode_t &node) const;
  virtual void get_rel_var (const Bastnode_t &node, 
			    char *rel_var) const {assert (0);}
};

class Bcreate_idx_op_t : public Bop_t {
public:
  Bcreate_idx_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual void execute (Bastnode_t &node);
};


class Bcreate_table_op_t : public Bop_t {
public:
  Bcreate_table_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual void execute (Bastnode_t &node);
};

class Bload_table_op_t : public Bop_t {
public:
  Bload_table_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual void execute (Bastnode_t &node);
};

class Bdrop_table_op_t : public Bop_t {
public:
  Bdrop_table_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual void execute (Bastnode_t &node);
};

class Bcreate_db_op_t : public Bop_t {
public:
  Bcreate_db_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual void execute (Bastnode_t &node);
};

class Bopen_db_op_t : public Bop_t {
public:
  Bopen_db_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual void execute (Bastnode_t &node);
};

class Bclose_db_op_t : public Bop_t {
public:
  Bclose_db_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual void execute (Bastnode_t &node);
};

class Bquery_stmt_op_t : public Bop_t {
public:
  Bquery_stmt_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual void execute (Bastnode_t &node);
  virtual void convert (Bastnode_t &node, Aquery_t &query);
};


class Bfrom_list_op_t : public Bop_t {
public:
  Bfrom_list_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
};

class Brel_var_op_t : public Bop_t {
public:
  Brel_var_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual void get_rel_var (const Bastnode_t &node, char *rel_var) const;
};

class Brel_name_op_t : public Bop_t {
public:
  Brel_name_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual void get_rel_var (const Bastnode_t &node, char *rel_var) const;
};

class Brelop_op_t : public Bop_t {
private:
  Acat_t::relop_t relop_code;
public:
  Brelop_op_t (const char *name, Acat_t::relop_t rc);
  virtual void typechk (Bastnode_t &node);
};

class Band_op_t : public Bop_t {
public:
  Band_op_t (const char *name) : Bop_t (name) {}
  virtual void pred_to_list (Bastnode_t &node, Alist_t<Bastnode_t> &list);
				// defined in Bconvert.c
};


class Bbetween_op_t : public Bop_t {
public:
  Bbetween_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
};

class Bbool_expr_op_t : public Bop_t {
public:
  Bbool_expr_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual Aptree_t ast_to_ptree (Bastnode_t &node);
};

class Bconst_expr_op_t : public Bop_t {
public:
  Bconst_expr_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual Aptree_t ast_to_ptree (Bastnode_t &node);
};

class Barith_op_t : public Bop_t {
private:
  char op_str[2];
public:
  Barith_op_t (const char *name, const char *str);
  virtual void typechk (Bastnode_t &node);
  virtual Aptree_t ast_to_ptree (Bastnode_t &node);
};

class Battr_or_rel_name_op_t : public Bop_t {
public:
  Battr_or_rel_name_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
};

class Brelref_op_t : public Bop_t {
public:
  Brelref_op_t (const char *name) : Bop_t (name) {}
  virtual void get_rel_var (const Bastnode_t &node, char *rel_var) const;
  virtual Aptree_t ast_to_ptree (Bastnode_t &node);
};

class Breference_op_t : public Bop_t {
public:
  Breference_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual void get_rel_var (const Bastnode_t &node, char *rel_var) const;
  virtual Aptree_t ast_to_ptree (Bastnode_t &node);
};

class Bliteral_op_t : public Bop_t {
public:
  Bliteral_op_t (const char *name) : Bop_t (name) {}
  virtual Aptree_t ast_to_ptree (Bastnode_t &node);
};

class Binvoke_method_op_t : public Bop_t {
public:
  Binvoke_method_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual Aptree_t ast_to_ptree (Bastnode_t &node);
};

class Bfunc_call_op_t : public Bop_t {
public:
  Bfunc_call_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual Aptree_t ast_to_ptree (Bastnode_t &node);
};

class Boid_ref_op_t : public Bop_t {
public:
  Boid_ref_op_t (const char *name) : Bop_t (name) {}
  virtual Aptree_t ast_to_ptree (Bastnode_t &node);
};

class Boid_op_t : public Bop_t {
public:
  Boid_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
};

class Brel_oid_op_t : public Bop_t {
public:
  Brel_oid_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
};

class Battref_op_t : public Bop_t {
public:
  Battref_op_t (const char *name) : Bop_t (name) {}
  virtual void typechk (Bastnode_t &node);
  virtual Aptree_t ast_to_ptree (Bastnode_t &node);
};

class Blitval_op_t : public Bop_t {
private:
public:
  Blitval_op_t (const char *name);
  virtual void typechk (Bastnode_t &node);
  virtual Aptree_t ast_to_ptree (Bastnode_t &node);
};

/***************************************************************************

  inline functions

***************************************************************************/

inline Brelop_op_t::Brelop_op_t (const char *name, Acat_t::relop_t rc) 
  : Bop_t (name) 
{
  relop_code = rc;
}

inline Barith_op_t::Barith_op_t (const char *name, const char *str)
  : Bop_t (name) 
{
  strncpy (op_str, str, sizeof (op_str));
}

inline Blitval_op_t::Blitval_op_t (const char *name)
  : Bop_t (name) {}

#endif /* BOPCODES_H */
