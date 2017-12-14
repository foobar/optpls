#ifndef BPARSETREE_H
#define BPARSETREE_H

#include <Aconsts.h>
#include <string.h>
#include <Bopcodes.h>
#include <Acat.h>
#include <Aglob_vars.h>
#include <Alist.h>
#include <Aptree.h>
#include <Afunc.h>

class Aquery_t;

class Bastnode_t {
public:
  typedef Bop_t *opcode_t;	// an opcode_t is actually a pointer
				// to an object of type Bop_t *.
				// in fact it is a pointer to an object
				// of a type derived from Bop_t.

  // and here are the various opcodes

  static opcode_t bogus, 
    empty,
    utility, update, query, create_idx, create_clus_idx, drop_idx, 
    create_table, load_table, drop_table,
    create_db, drop_db, open_db, close_db, 
    delete_stmt, insert_values, insert_query, update_stmt,
    attr_assign, attr_null, query_stmt, set_xsect, set_minus, set_union,
    select_star, select_all, select_dist, identifier, rel_var, rel_name, 
    predicate, equal, not_equal, less, less_equal, greater, greater_equal, 
    overlaps, not_overlaps, oid_ref, bool_expr, between, not_between,
    not, and, or, unary_plus, unary_minus, plus, minus, times, div,
    literal, reference, invoke_method, func_call, attref, relref, oid, rel_oid,
    integer, real, string, const_expr, asc, dsc, 
    asc_idx_attref, dsc_idx_attref, col_typename,
    clustered, all, distinct,
    attr_update_list, expr_list, from_list, 
    attr_or_rel_name, arg_list, groupby_list, orderby_list,
    set_order_by_list, col_list, col_name, col_number, filename_list,
    filename_id, filename_str, idx_attr_list, col_typename_list, typename,
    value_list;

  enum type_t {
    Bogus, Empty,
    Command, Utility, Update, AttrUpdateList, AttrUpdate, Query, 
    SelectClause, ExprList, FromClause, RelName, WhereClause, Predicate,
    Expr, Relref, Attref, OidRef, ArgList, Literal, ConstExpr, 
    GroupBy, OrderBy, SortOrder, SetOrderBy, OptColList, Col, FilenameList,
    Filename, Ident, IdxAttrList, IdxAttref, ColTypenameList, ColTypename, 
    Typename, ValueList,
    SetOp, AllDist, Relop, AttrefList, SortOrderList, OptAscDsc, IntegerList,
    ColList, OptClustered, RelNameList, Integer, Real, String
  };

  class typeinfo_t {
  public:
    Acat_t::relid_t relid;
    char rel_var[A_MAXNAMELEN];	
    Acat_t::attrid_t attrid;
    Acat_t::funcid_t funcid;
    Acat_t::attr_type_t attr_type;
    typeinfo_t (void);
  };

private:
  type_t _type;
  opcode_t _opcode;

  int _nargs;			// number of arguments
  Bastnode_t *_args;		// array of pointers to arguments

  int _line;
  char *_text;
  Acat_t::adt_t *_adt_val;

public:
  typeinfo_t typeinfo;		// information required for typechecking

  void init (int line_no = 0);
  void init (type_t t, opcode_t o, int n, Bastnode_t *a, int l);

  Bastnode_t (void) {init ();}
  ~Bastnode_t (void) {}
  void delete_tree (void);
  

  opcode_t opcode (void) const {return _opcode;}
  type_t type (void) const {return _type;}
  int nargs (void) const {return _nargs;}
  Bastnode_t &arg (int N) const {return _args[N];}
  int line (void) const {return _line;}
  const char *text (void) const {return _text;}
  void set_text (const char *str);
  Acat_t::adt_t *adt_val (void) const {return _adt_val;}
  void set_adt_val (Acat_t::adt_t *a) {_adt_val = a;}

  void assign (const Bastnode_t &other, type_t new_type = Bogus);
  Bastnode_t &flatten (void); // flatten a list (parse tree).
  void typechk (void) {opcode ()->typechk (*this);}
  void execute (void) {opcode ()->execute (*this);}
  void pred_to_list (Alist_t<Bastnode_t> &list);
  Aptree_t ast_to_ptree (void) {return opcode ()->ast_to_ptree (*this);}
  void get_rel_var (char *rel_var) const;
};


class Bparser_state_t {
private:
  int _error;			// true if there has been an error
  char _error_str[1024];		// the error string
  Alist_t<Bastnode_t> attrs;	// list of all attrs in the query.
  Alist_t<Bastnode_t> relrefs;	// list of all relrefs in the query.

public:
  int Bline;			// line number
  Bastnode_t query;		// the query
  Bastnode_t *from_clause;	// pointer to the from_clause node.
  char *input_string;		// where the query is read from

  void reset (void);		// reset the state at the beginning of a query.
  void set_error (int line_no, char *msg);
  int error (void) const {return _error;}
  const char *error_str (void) const {return _error_str;}
  void add_uniq_attr (Bastnode_t *attr);
  Alist_t<Bastnode_t> attr_list (void) const {return attrs;}
  void add_uniq_relref (Bastnode_t *relref);
  Alist_t<Bastnode_t> relref_list (void) const {return relrefs;}
};

inline Bparser_state_t *Bparser_state(void) 
{
  return Aglob_vars ()->parser_state;
}

/***************************************************************************

  inline functions

***************************************************************************/
inline Bastnode_t::typeinfo_t::typeinfo_t (void)
{
  Acat_t::invalidate_relid (relid);
  Acat_t::invalidate_attrid (attrid);
  attr_type = Acat_t::bogus_adt ();
//  funcid = Acat_t::bogus ();
}

inline void Bastnode_t::init (int line_no)
{
  _type = Bogus; _opcode = bogus; _args = 0; 
  _nargs = 0; _line = line_no;
  _text = 0;
}

inline void Bastnode_t::assign (const Bastnode_t &other, type_t new_type)
{
  *this = other;
  if (new_type != Bogus) _type = new_type;
}

inline void Bastnode_t::set_text (const char *str)
{
  _text = new char [strlen (str) + 1]; assert (_text); strcpy (_text, str);
}

inline void Bastnode_t::pred_to_list (Alist_t<Bastnode_t> &list)
{
  opcode ()->pred_to_list (*this, list);
}

inline void Bastnode_t::get_rel_var (char *rel_var) const
{
  opcode ()->get_rel_var (*this, rel_var);
}

inline void Bparser_state_t::reset (void)
{
  Bline = 1; _error = 0; attrs.DeleteList (); relrefs.DeleteList ();
}



#endif /* BPARSETREE_H */
