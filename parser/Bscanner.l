%{

#define YYSTYPE Bastnode_t

#include <Bparsetree.h>
#include <By.tab.h>
#include <assert.h>
#include <string.h>
#include <iostream.h>
#include <iomanip.h>


#undef YY_INPUT
#define YY_INPUT(buf,result,maxsize) \
		result = strlen (Bparser_state()->input_string); \
		if (result > maxsize) result = maxsize; \
		strncpy (buf, Bparser_state()->input_string, maxsize); \
		Bparser_state()->input_string += result;

int Bline;

%}

underscore	_
dollar		\$
slash		\/
dot		\.

digit		[0-9]
alpha		[a-zA-Z]
nondigit	{alpha}|{underscore}|{dollar}
character	{nondigit}|{digit}
optsign		[\+\-]?

integer		{optsign}{digit}+
frac		{dot}{integer}
mantissa	{integer}|{integer}{dot}|{integer}{frac}|{frac}
exponent        [Ee]{integer}
real		{mantissa}{exponent}?

string		\"([^\"]|(\"\"))*\"

comment		{slash}{slash}[^\n]*

identifier	{nondigit}{character}*

%%

[ ]		{}
\t		{}		
\n		{Bline++;}
{comment}	{}

"create"	{yylval.init (Bline); return t_create;}
"load"		{yylval.init (Bline); return t_load;}
"drop"		{yylval.init (Bline); return t_drop;}
"open"		{yylval.init (Bline); return t_open;}
"close"		{yylval.init (Bline); return t_close;}
"db"		{yylval.init (Bline); return t_db;}
"table"		{yylval.init (Bline); return t_table;}
"relation"	{yylval.init (Bline); return t_table;}
"extent"	{yylval.init (Bline); return t_table;}
"clustered"	{yylval.init (Bline); return t_clustered;}
"index"		{yylval.init (Bline); return t_index;}
"delete"	{yylval.init (Bline); return t_delete;}
"on"		{yylval.init (Bline); return t_on;}
"insert"	{yylval.init (Bline); return t_insert;}
"into"		{yylval.init (Bline); return t_into;}
"values"	{yylval.init (Bline); return t_values;}
"update"	{yylval.init (Bline); return t_update;}
"set"		{yylval.init (Bline); return t_set;}
"as"		{yylval.init (Bline); return t_as;}
"select"	{yylval.init (Bline); return t_select;}
"from"		{yylval.init (Bline); return t_from;}
"where"		{yylval.init (Bline); return t_where;}
"group"		{yylval.init (Bline); return t_group;}
"order"		{yylval.init (Bline); return t_order;}
"by"		{yylval.init (Bline); return t_by;}
"asc"		{yylval.init (Bline); return t_asc;}
"desc"		{yylval.init (Bline); return t_dsc;}
"intersect"	{yylval.init (Bline); return t_intersect;}
"minus"		{yylval.init (Bline); return t_minus;}
"union"		{yylval.init (Bline); return t_union;}
"all"		{yylval.init (Bline); return t_all;}
"distinct"	{yylval.init (Bline); return t_distinct;}
"not"		{yylval.init (Bline); return t_not;}
"null"		{yylval.init (Bline); return t_null;}
"and"		{yylval.init (Bline); return t_and;}
"or"		{yylval.init (Bline); return t_or;}
"is"		{yylval.init (Bline); return t_is;}
"between"	{yylval.init (Bline); return t_between;}
"oid"		{yylval.init (Bline); return t_oid;}
"overlaps"	{yylval.init (Bline); return t_overlaps;}
"="		{yylval.init (Bline); return t_equal;}
"<>"		{yylval.init (Bline); return t_not_equal;}
"!="		{yylval.init (Bline); return t_not_equal;}
"<"		{yylval.init (Bline); return t_less;}
"<="		{yylval.init (Bline); return t_less_equal;}
">"		{yylval.init (Bline); return t_greater;}
">="		{yylval.init (Bline); return t_greater_equal;}
"("		{yylval.init (Bline); return t_lparen;}
")"		{yylval.init (Bline); return t_rparen;}
"["		{yylval.init (Bline); return t_lbracket;}
"]"		{yylval.init (Bline); return t_rbracket;}
","		{yylval.init (Bline); return t_comma;}
"."		{yylval.init (Bline); return t_dot;}
"->"		{yylval.init (Bline); return t_arrow;}
"+"		{yylval.init (Bline); return t_plus;}
"-"		{yylval.init (Bline); return t_minus;}
"*"		{yylval.init (Bline); return t_star;}
"/"		{yylval.init (Bline); return t_div;}
{integer}	{yylval.init (Bline); return t_integer;}
{real}		{yylval.init (Bline); return t_real;}
{string}	{yylval.init (Bline); return t_string;}
{identifier}	{yylval.init (Bline); return t_identifier;}

.		{assert (!"Illegal Characters"); return 0;}

%%

void Brestart_lexical_analyzer (void) {
// and the end of a query, the scanner gets a YY_NULL 
// and, unfortunately, decides to remember it so that
// yylex returns 0 on all subsequent calls.
// we need some way of preventing this behaviour.
// i.e. we need to return a YY_NULL at the end of
// each query but, then at the start of the next
// query we also need the scanner to start all over
// again. so this is a hack to achieve that.
   
// if someone has a better way of doing this, 
// go ahead, be my guest!
yy_init = 1;
Bparser_state()->reset ();
}

int yywrap (void) {return 1;}
