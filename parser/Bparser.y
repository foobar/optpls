%{	
#include "Bparsetree.h"

#define YYSTYPE Bastnode_t

void yyerror (char *);
extern int yylex (void);
extern char *yytext;

%}

%token t_integer t_real t_string
%token t_identifier 
%token t_oid
%token t_create t_drop t_load t_open t_close t_db t_table 
%token t_clustered t_index t_on t_delete t_insert t_into
%token t_as t_values t_update t_set 
%token t_select t_from t_where t_group t_order t_by t_asc t_dsc
%token t_intersect t_minus t_union
%token t_all t_distinct
%token t_null t_is t_between 
%token t_overlaps t_equal t_not_equal 
%token t_less t_less_equal t_greater t_greater_equal
%token t_lparen t_rparen t_lbracket t_rbracket
%token t_comma t_arrow

%left t_and t_or 
%left t_dot
%left t_not

%left t_plus t_minus
%left t_star t_div
%left t_unary_minus


%%
StartState	:	Command { Bparser_state()->query = $1; }

Command		:	Utility
			{ 
			  $$.init (Bastnode_t::Command, Bastnode_t::utility,
			           1, &$1, $1.line ());
			}
		|	Update
			{ 
			  $$.init (Bastnode_t::Command, Bastnode_t::update,
			           1, &$1, $1.line ());
			}
		|	Query
			{ 
			  $$.init (Bastnode_t::Command, Bastnode_t::query,
			           1, &$1, $1.line ());
			}
		|	
			{ 
			  $$.init (Bastnode_t::Command, Bastnode_t::empty,
			           0, 0, 0); 
			} 
		;

Utility		:	t_create OptClustered t_index Ident t_on Ident
			t_lparen IdxAttrList t_rparen
			{
			  Bastnode_t::opcode_t opcode;
			  if ($2.opcode () == Bastnode_t::clustered)
			    opcode = Bastnode_t::create_clus_idx;
			  else
			    opcode = Bastnode_t::create_idx;

			  Bastnode_t args[3] = {$4, $6, $8.flatten ()};
			  $$.init (Bastnode_t::Utility, opcode,
			           3, args, $1.line ());
			}
		|	t_drop t_index Ident
			{
			  $$.init (Bastnode_t::Utility, Bastnode_t::drop_idx,
			           1, &$3, $1.line ());
			}
		
		|	t_create t_table Ident t_lparen ColTypenameList t_rparen
			{
			  Bastnode_t args[2] = {$3, $5.flatten ()};
			  $$.init (Bastnode_t::Utility, 
			           Bastnode_t::create_table,
			           2, args, $1.line ());
			}
		|	t_load t_table Ident t_from FilenameList
			{
			  Bastnode_t args[2] = {$3, $5.flatten ()};
			  $$.init (Bastnode_t::Utility, Bastnode_t::load_table,
			           2, args, $1.line ());
			}
		|	t_drop t_table Ident
			{
			  $$.init (Bastnode_t::Utility, Bastnode_t::drop_table,
			           1, &$3, $1.line ());
			}

		|	t_create t_db Ident
			{ 
			  $$.init (Bastnode_t::Utility, Bastnode_t::create_db,
			           1, &$3, $1.line ());
			}
		|	t_drop t_db Ident
			{ 
			  $$.init (Bastnode_t::Utility, Bastnode_t::drop_db,
			           1, &$3, $1.line ());
			}
		|	t_open t_db Ident
			{ 
			  $$.init (Bastnode_t::Utility, Bastnode_t::open_db,
			           1, &$3, $1.line ());
			}
		|	t_close t_db Ident
			{ 
			  $$.init (Bastnode_t::Utility, Bastnode_t::close_db,
			           1, &$3, $1.line ());
			}
		;

OptClustered	:	t_clustered
			{
			  $$.init (Bastnode_t::OptClustered, 
			           Bastnode_t::clustered,
			           0, 0, $1.line ());
			}
		|	
			{
			  $$.init (Bastnode_t::OptClustered, 
			           Bastnode_t::empty,
			           0, 0, -1);
			}
		;

Update		:	t_delete t_from Ident WhereClause
			{
			  Bastnode_t args[2] = {$3, $4};
			  $$.init (Bastnode_t::Update, Bastnode_t::delete_stmt,
			           2, args, $1.line ());
			}
		|	t_insert t_into Ident OptColList t_values ValueList
			{
			  Bastnode_t args[3] = {$3, $4.flatten (), 
			                        $6.flatten ()};
			  $$.init (Bastnode_t::Update, 
			           Bastnode_t::insert_values,
			           3, args, $1.line ());
			}
		|	t_insert t_into Ident OptColList Query
			{
			  Bastnode_t args[3] = {$3, $4.flatten (), $5};
			  $$.init (Bastnode_t::Update, 
			           Bastnode_t::insert_query,
			           3, args, $1.line ());
			}
		|	t_update Ident t_set AttrUpdateList WhereClause
			{
			  Bastnode_t args[3] = {$2, $4.flatten (), $5};
			  $$.init (Bastnode_t::Update, Bastnode_t::update_stmt,
			           3, args, $1.line ());
			}
		;


AttrUpdateList	:	AttrUpdate
			{
			  $$.init (Bastnode_t::AttrUpdateList, 
			           Bastnode_t::attr_update_list,
			           1, &$1, $1.line ());
			}
		|	AttrUpdateList t_comma AttrUpdate
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::AttrUpdateList, 
			           Bastnode_t::attr_update_list,
			           2, args, $1.line ());
			}
		;

AttrUpdate	:	Col t_equal Expr
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::AttrUpdate, 
			           Bastnode_t::attr_assign,
			           2, args, $1.line ());
			}
		|	Col t_equal t_null
			{
			  $$.init (Bastnode_t::AttrUpdate, 
			           Bastnode_t::attr_null,
			           1, &$1, $1.line ());
			}
		;

Query		:	SelectClause FromClause WhereClause
			GroupBy OrderBy
			{
			  Bastnode_t args[5] = {$1, $2, $3, $4, $5};
			  $$.init (Bastnode_t::Query, Bastnode_t::query_stmt,
			           5, args, $1.line ());
			}
		|	t_lparen Query t_rparen SetOp t_lparen Query t_rparen
			SetOrderBy
			{
			  Bastnode_t args[3] = {$2, $6, $8};
			  $$.init (Bastnode_t::Query, $4.opcode (),
			           3, args, $1.line ()); 
			}
		;

SetOp		:	t_intersect
			{
			  $$.init (Bastnode_t::SetOp, Bastnode_t::set_xsect,
			           0, 0, $1.line ());
			}
		|	t_minus
			{
			  $$.init (Bastnode_t::SetOp, Bastnode_t::set_minus,
			           0, 0, $1.line ());
			}
		|	t_union
			{
			  $$.init (Bastnode_t::SetOp, Bastnode_t::set_union,
			           0, 0, $1.line ());
			}
		;

SelectClause	:	t_select t_star
			{
			  $$.init (Bastnode_t::SelectClause,
			           Bastnode_t::select_star, 
			           0, 0, $1.line ());
			}
		|	t_select AllDist ExprList
			{
			  Bastnode_t::opcode_t opcode;
			  if ($2.opcode () == Bastnode_t::distinct)
			    opcode = Bastnode_t::select_dist;
			  else
			    opcode = Bastnode_t::select_all;

			  Bastnode_t args[1] = {$3.flatten ()};
			  $$.init (Bastnode_t::SelectClause, opcode, 
			           1, args, $1.line ()); 
			}
		;

AllDist		:	
			{
			  $$.init (Bastnode_t::AllDist, Bastnode_t::empty, 
				   0, 0, -1);
			}
		|	t_all
			{
			  $$.init (Bastnode_t::AllDist,
			           Bastnode_t::all, 
			           0, 0, $1.line ());
			}
		|	t_distinct
			{
			  $$.init (Bastnode_t::AllDist,
			           Bastnode_t::distinct, 
			           0, 0, $1.line ());
			}
		;

ExprList	:	Expr
			{
			  $$.init (Bastnode_t::ExprList,
			           Bastnode_t::expr_list,
			           1, &$1, $1.line ());
			}
		|	ExprList t_comma Expr
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::ExprList,
			           Bastnode_t::expr_list, 
			           2, args, $1.line ());
			}
		;

FromClause	:	t_from RelNameList
			{
			  $$ = $2.flatten ();
			}
		;

RelNameList	:	RelName
			{
			  $$.init (Bastnode_t::FromClause,
			           Bastnode_t::from_list, 
			           1, &$1, $1.line ());
			}
		|	RelNameList t_comma RelName
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::FromClause,
			           Bastnode_t::from_list, 
			           2, args, $1.line ());
			}
		;

RelName		:	Ident
			{
			  $$.init (Bastnode_t::RelName, Bastnode_t::rel_name,
			           1, &$1, $1.line ());
			}
		|	Ident Ident
			{
			  Bastnode_t args[2] = {$1, $2};
			  $$.init (Bastnode_t::RelName,
			           Bastnode_t::rel_var, 
			           2, args, $1.line ());
			}
		;

WhereClause	:	
			{
			  $$.init (Bastnode_t::WhereClause, Bastnode_t::empty, 
			           0, 0, -1);
			}
		|	t_where Predicate
			{
			  $$.init (Bastnode_t::WhereClause,
			           Bastnode_t::predicate, 
			           1, &$2, $1.line ());
			}
		;

Predicate	:	Expr
			{
			  $$.init (Bastnode_t::Predicate,
			           Bastnode_t::bool_expr, 
			           1, &$1, $1.line ());
			}
		|	Expr Relop Expr
			{
			  Bastnode_t::opcode_t opcode = $2.opcode ();
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::Predicate, opcode, 
			           2, args, $1.line ());
			}
		|	OidRef t_equal String
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::Predicate,
			           Bastnode_t::oid_ref, 
			           2, args, $1.line ());
			}
		|	Expr t_between ConstExpr t_and ConstExpr
			{
			  Bastnode_t args[3] = {$1, $3, $5};
			  $$.init (Bastnode_t::Predicate,
			           Bastnode_t::between, 
			           3, args, $1.line ());
			}
		|	Expr t_not t_between ConstExpr t_and ConstExpr
			{
			  Bastnode_t args[3] = {$1, $4, $6};
			  $$.init (Bastnode_t::Predicate,
			           Bastnode_t::not_between, 
			           3, args, $1.line ());
			}
		|	t_not Predicate
			{
			  $$.init (Bastnode_t::Predicate,
			           Bastnode_t::not, 
			           1, &$2, $1.line ());
			}
		|	Predicate t_and Predicate
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::Predicate,
			           Bastnode_t::and, 
			           2, args, $1.line ());
			}
		|	Predicate t_or Predicate
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::Predicate,
			           Bastnode_t::or, 
			           2, args, $1.line ());
			}
		|	t_lparen Predicate t_rparen
			{
			  $$ = $2;
			}
		;

Relop		:	t_equal
			{
			  $$.init (Bastnode_t::Relop,
			           Bastnode_t::equal, 
			           0, 0, $1.line ());
			}
		|	t_not_equal
			{
			  $$.init (Bastnode_t::Relop,
			           Bastnode_t::not_equal, 
			           0, 0, $1.line ());
			}
		|	t_less
			{
			  $$.init (Bastnode_t::Relop,
			           Bastnode_t::less, 
			           0, 0, $1.line ());
			}
		|	t_less_equal
			{
			  $$.init (Bastnode_t::Relop,
			           Bastnode_t::less_equal, 
			           0, 0, $1.line ());
			}
		|	t_greater
			{
			  $$.init (Bastnode_t::Relop,
			           Bastnode_t::greater, 
			           0, 0, $1.line ());
			}
		|	t_greater_equal
			{
			  $$.init (Bastnode_t::Relop,
			           Bastnode_t::greater_equal, 
			           0, 0, $1.line ());
			}
		|	t_overlaps
			{
			  $$.init (Bastnode_t::Relop,
			           Bastnode_t::overlaps, 
			           0, 0, $1.line ());
			}
		|	t_not t_overlaps
			{
			  $$.init (Bastnode_t::Relop,
			           Bastnode_t::not_overlaps, 
			           0, 0, $1.line ());
			}
		;

Expr		:	t_plus Expr  %prec t_unary_minus
			{
			  $$.init (Bastnode_t::Expr,
			           Bastnode_t::unary_plus, 
			           1, &$2, $1.line ());
			}
		|	t_minus Expr %prec t_unary_minus
			{
			  $$.init (Bastnode_t::Expr,
			           Bastnode_t::unary_minus, 
			           1, &$2, $1.line ());
			}
		|	Expr t_plus Expr
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::Expr,
			           Bastnode_t::plus, 
			           2, args, $2.line ());
			}
		| 	Expr t_minus Expr
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::Expr,
			           Bastnode_t::minus, 
			           2, args, $2.line ());
			}
		|	Expr t_star Expr
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::Expr,
			           Bastnode_t::times, 
			           2, args, $2.line ());
			}
		|	Expr t_div Expr
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::Expr,
			           Bastnode_t::div, 
			           2, args, $2.line ());
			}
		|	Literal
			{
			  $$.init (Bastnode_t::Expr,
			           Bastnode_t::literal, 
			           1, &$1, $1.line ());
			}
		|	Ident
			{
			  $$.init (Bastnode_t::Expr,
				   Bastnode_t::attr_or_rel_name, 
				   1, &$1, $1.line ());
			}
		|	Expr t_dot Ident
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::Expr,
			           Bastnode_t::reference, 
			           2, args, $3.line ());
			}
		|	Expr t_dot Ident t_lparen OptArgList t_rparen
			{
			  Bastnode_t args[3] = {$1, $3, $5};
			  $$.init (Bastnode_t::Expr,
			           Bastnode_t::invoke_method, 
			           3, args, $3.line ());
			}
			/* object.func (args) */
		|	Ident t_lparen OptArgList t_rparen
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::Expr,
			           Bastnode_t::func_call, 
			           2, args, $1.line ());
			}
		;

Attref		:	Ident
			{
			  $$.init (Bastnode_t::Attref, 
				   Bastnode_t::attr_or_rel_name,
				   1, &$1, $1.line ());
			}
		|	Attref t_dot Ident
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::Attref,
			           Bastnode_t::attref, 
			           2, args, $1.line ());
			}
		;

OptArgList	:	
			{
			  $$.init (Bastnode_t::ArgList,
			           Bastnode_t::arg_list, 
			           0, 0, -1);
			}
		|	ArgList
			{
			  $$ = $1.flatten ();
			}
		;

ArgList		:	Expr
			{
			  $$.init (Bastnode_t::ArgList,
			           Bastnode_t::arg_list, 
			           1, &$1, $1.line ());
			}
		|	ArgList t_comma Expr
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::ArgList,
			           Bastnode_t::arg_list, 
			           2, args, $1.line ());
			}
		;

OidRef		:	t_oid
			{
			  $$.init (Bastnode_t::OidRef,
			           Bastnode_t::oid, 
			           0, 0, $1.line ());
			}
		|	Ident t_arrow t_oid
			{
			  $$.init (Bastnode_t::OidRef,
			           Bastnode_t::rel_oid, 
			           1, &$1, $1.line ());
			}
		;

Literal		:	Integer
			{
			  $$.assign ($1, Bastnode_t::Literal);
			}
		|	Real
			{
			  $$.assign ($1, Bastnode_t::Literal);
			}
		|	String
			{
			  $$.assign ($1, Bastnode_t::Literal);
			}
		;

ConstExpr	:	Expr
			{
			  $$.init (Bastnode_t::ConstExpr, 
				   Bastnode_t::const_expr,
				   1, &$1, $1.line ());
			}
		;

GroupBy		:	
			{
			  $$.init (Bastnode_t::GroupBy,
			           Bastnode_t::groupby_list, 
			           0, 0, -1);
			}
		|	t_group t_by AttrefList
			{
			  $$ = $3.flatten ();
			}
		;

AttrefList	:	Attref
			{
			  $$.init (Bastnode_t::GroupBy,
			           Bastnode_t::groupby_list, 
			           1, &$1, $1.line ());
			}
		|	AttrefList t_comma Attref
			{
			  Bastnode_t args[2] = {$1, $2};
			  $$.init (Bastnode_t::GroupBy,
			           Bastnode_t::groupby_list, 
			           2, args, $1.line ());
			}
		;

OrderBy		:	
			{
			  $$.init (Bastnode_t::OrderBy,
			           Bastnode_t::orderby_list, 
			           0, 0, -1);
			}
		|	t_order t_by SortOrderList
			{
			  $$ = $3.flatten ();
			}
		;

SortOrderList	:	SortOrder
			{
			  $$.init (Bastnode_t::OrderBy,
			           Bastnode_t::orderby_list, 
			           1, &$1, $1.line ());
			}
		|	SortOrderList t_comma SortOrder
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::OrderBy,
			           Bastnode_t::orderby_list, 
			           2, args, $1.line ());
			}
		;

SortOrder	:	Attref OptAscDsc
			{
			  Bastnode_t::opcode_t opcode;
			  if ($2.opcode () == Bastnode_t::dsc)
			    opcode = Bastnode_t::dsc;
			  else
			    opcode = Bastnode_t::asc;
			  $$.init (Bastnode_t::SortOrder, opcode, 
			           1, &$1, $1.line ());
			}
		;

OptAscDsc	:	
			{
			  $$.init (Bastnode_t::OptAscDsc,
			           Bastnode_t::empty, 
			           0, 0, -1);
			}
		|	t_asc
			{
			  $$.init (Bastnode_t::OptAscDsc,
			           Bastnode_t::asc, 
			           0, 0, $1.line ());
			}
		|	t_dsc
			{
			  $$.init (Bastnode_t::OptAscDsc,
			           Bastnode_t::dsc, 
			           0, 0, $1.line ());
			}
		;

SetOrderBy	:	
			{
			  $$.init (Bastnode_t::SetOrderBy,
			           Bastnode_t::set_order_by_list, 
			           0, 0, -1);
			}
		|	t_order t_by IntegerList
			{
			  $$ = $3.flatten ();
			}
		;

IntegerList	:	Integer
			{
			  $$.init (Bastnode_t::SetOrderBy,
			           Bastnode_t::set_order_by_list, 
			           1, &$1, $1.line ());
			}
		|	IntegerList Integer
			{
			  Bastnode_t args[2] = {$1, $2};
			  $$.init (Bastnode_t::SetOrderBy,
			           Bastnode_t::set_order_by_list, 
			           2, args, $1.line ());
			}
		;

OptColList	:	
			{
			  $$.init (Bastnode_t::OptColList,
			           Bastnode_t::col_list, 
			           0, 0, -1);
			}
		|	t_lbracket ColList t_rbracket
			{
			/* these should actually be l_paren and r_paren
			   but that would meant that the language is not 
			   LL(1) andt i dont have the enthusiasm to waste 
			   time over this issue */
			  $$ = $2.flatten ();
			}
		;

ColList		:	Col
			{
			  $$.init (Bastnode_t::OptColList,
			           Bastnode_t::col_list, 
			           1, &$1, $1.line ());
			}
		|	ColList t_comma Col
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::OptColList,
			           Bastnode_t::col_list, 
			           2, args, $1.line ());
			}
		;

Col		:	Ident
			{
			  $$.init (Bastnode_t::Col, Bastnode_t::col_name,
				   1, &$1, $1.line ());
			}
		|	Integer
			{
			  $$.init (Bastnode_t::Col, Bastnode_t::col_number,
				   1, &$1, $1.line ());
			}
		;

FilenameList	:	Filename
			{
			  $$.init (Bastnode_t::FilenameList,
			           Bastnode_t::filename_list, 
			           1, &$1, $1.line ());
			}
		|	FilenameList t_comma Filename
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::FilenameList,
			           Bastnode_t::filename_list, 
			           2, args, $1.line ());
			}
		;

Filename	:	Ident
			{
			  $$.init (Bastnode_t::Filename,
				   Bastnode_t::filename_id,
				   1, &$1, $1.line ());
			}
		|	String
			{
			  $$.init (Bastnode_t::Filename,
				   Bastnode_t::filename_str,
				   1, &$1, $1.line ());
			}
		;

Ident		:	t_identifier
			{
			  $$.init (Bastnode_t::Ident, Bastnode_t::identifier,
				   0, 0, $1.line ());
			  $$.set_text (yytext);
			}
		;

Integer		:	t_integer
			{
			  $$.init (Bastnode_t::Integer, Bastnode_t::integer,
				   0, 0, $1.line ());
			  $$.set_text (yytext);
			}
		;

Real		:	t_real
			{
			  $$.init (Bastnode_t::Real, Bastnode_t::real,
				   0, 0, $1.line ());
			  $$.set_text (yytext);
			}
		;

String		:	t_string
			{
			  $$.init (Bastnode_t::String, Bastnode_t::string,
				   0, 0, $1.line ());
				/*  getting rid of the double quotes */
			  int string_length = strlen (yytext);
			  yytext[string_length-1] = '\0';
			  $$.set_text (yytext+1);
			}
		;


IdxAttrList	:	IdxAttref
			{
			  $$.init (Bastnode_t::IdxAttrList,
			           Bastnode_t::idx_attr_list, 
			           1, &$1, $1.line ());
			}
		|	IdxAttrList t_comma IdxAttref
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::IdxAttrList,
			           Bastnode_t::idx_attr_list, 
			           2, args, $1.line ());
			}
		;

IdxAttref	:	Col OptAscDsc
			{
			  Bastnode_t::opcode_t opcode;
			  if ($2.opcode () == Bastnode_t::dsc)
			    opcode = Bastnode_t::dsc_idx_attref;
			  else
			    opcode = Bastnode_t::asc_idx_attref;
			  $$.init (Bastnode_t::IdxAttref, opcode, 
			           1, &$1, $1.line ());
			}
		;

ColTypenameList	:	ColTypename
			{
			  $$.init (Bastnode_t::ColTypenameList,
			           Bastnode_t::col_typename_list, 
			           1, &$1, $1.line ());
			}
		|	ColTypenameList t_comma ColTypename
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::ColTypenameList,
			           Bastnode_t::col_typename_list, 
			           2, args, $1.line ());
			}
		;

ColTypename	:	Ident Typename
			{
			  Bastnode_t args[2] = {$1, $2};
			  $$.init (Bastnode_t::ColTypename,
			           Bastnode_t::col_typename, 
			           2, args, $1.line ());
			}
		;

Typename	:	Ident
			{
			  $$.init (Bastnode_t::Typename, Bastnode_t::typename,
				   1, &$1, $1.line ());
			}
		;

ValueList	: 	ConstExpr
			{
			  $$.init (Bastnode_t::ValueList,
			           Bastnode_t::value_list, 
			           1, &$1, $1.line ());
			}
		|	ValueList t_comma ConstExpr
			{
			  Bastnode_t args[2] = {$1, $3};
			  $$.init (Bastnode_t::ValueList,
			           Bastnode_t::value_list, 
			           2, args, $1.line ());
			}
		;

%%		

void yyerror (char *s) {return;}
