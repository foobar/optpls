
/*  A Bison parser, made from Bparser.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	t_integer	258
#define	t_real	259
#define	t_string	260
#define	t_identifier	261
#define	t_oid	262
#define	t_create	263
#define	t_drop	264
#define	t_load	265
#define	t_open	266
#define	t_close	267
#define	t_db	268
#define	t_table	269
#define	t_clustered	270
#define	t_index	271
#define	t_on	272
#define	t_delete	273
#define	t_insert	274
#define	t_into	275
#define	t_as	276
#define	t_values	277
#define	t_update	278
#define	t_set	279
#define	t_select	280
#define	t_from	281
#define	t_where	282
#define	t_group	283
#define	t_order	284
#define	t_by	285
#define	t_asc	286
#define	t_dsc	287
#define	t_intersect	288
#define	t_minus	289
#define	t_union	290
#define	t_all	291
#define	t_distinct	292
#define	t_null	293
#define	t_is	294
#define	t_between	295
#define	t_overlaps	296
#define	t_equal	297
#define	t_not_equal	298
#define	t_less	299
#define	t_less_equal	300
#define	t_greater	301
#define	t_greater_equal	302
#define	t_lparen	303
#define	t_rparen	304
#define	t_lbracket	305
#define	t_rbracket	306
#define	t_comma	307
#define	t_arrow	308
#define	t_and	309
#define	t_or	310
#define	t_dot	311
#define	t_not	312
#define	t_plus	313
#define	t_star	314
#define	t_div	315
#define	t_unary_minus	316

	
#include "Bparsetree.h"

#define YYSTYPE Bastnode_t

void yyerror (char *);
extern int yylex (void);
extern char *yytext;


#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		220
#define	YYFLAG		-32768
#define	YYNTBASE	62

#define YYTRANSLATE(x) ((unsigned)(x) <= 316 ? yytranslate[x] : 110)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     6,     8,     9,    19,    23,    30,    36,
    40,    44,    48,    52,    56,    58,    59,    64,    71,    77,
    83,    85,    89,    93,    97,   103,   112,   114,   116,   118,
   121,   125,   126,   128,   130,   132,   136,   139,   141,   145,
   147,   150,   151,   154,   156,   160,   164,   170,   177,   180,
   184,   188,   192,   194,   196,   198,   200,   202,   204,   206,
   209,   212,   215,   219,   223,   227,   231,   233,   235,   239,
   246,   251,   253,   257,   258,   260,   262,   266,   268,   272,
   274,   276,   278,   280,   281,   285,   287,   291,   292,   296,
   298,   302,   305,   306,   308,   310,   311,   315,   317,   320,
   321,   325,   327,   331,   333,   335,   337,   341,   343,   345,
   347,   349,   351,   353,   355,   359,   362,   364,   368,   371,
   373,   375
};

static const short yyrhs[] = {    63,
     0,    64,     0,    66,     0,    69,     0,     0,     8,    65,
    16,   100,    17,   100,    48,   104,    49,     0,     9,    16,
   100,     0,     8,    14,   100,    48,   106,    49,     0,    10,
    14,   100,    26,    98,     0,     9,    14,   100,     0,     8,
    13,   100,     0,     9,    13,   100,     0,    11,    13,   100,
     0,    12,    13,   100,     0,    15,     0,     0,    18,    26,
   100,    77,     0,    19,    20,   100,    95,    22,   109,     0,
    19,    20,   100,    95,    69,     0,    23,   100,    24,    67,
    77,     0,    68,     0,    67,    52,    68,     0,    97,    42,
    80,     0,    97,    42,    38,     0,    71,    74,    77,    87,
    89,     0,    48,    69,    49,    70,    48,    69,    49,    93,
     0,    33,     0,    34,     0,    35,     0,    25,    59,     0,
    25,    72,    73,     0,     0,    36,     0,    37,     0,    80,
     0,    73,    52,    80,     0,    26,    75,     0,    76,     0,
    75,    52,    76,     0,   100,     0,   100,   100,     0,     0,
    27,    78,     0,    80,     0,    80,    79,    80,     0,    84,
    42,   103,     0,    80,    40,    86,    54,    86,     0,    80,
    57,    40,    86,    54,    86,     0,    57,    78,     0,    78,
    54,    78,     0,    78,    55,    78,     0,    48,    78,    49,
     0,    42,     0,    43,     0,    44,     0,    45,     0,    46,
     0,    47,     0,    41,     0,    57,    41,     0,    58,    80,
     0,    34,    80,     0,    80,    58,    80,     0,    80,    34,
    80,     0,    80,    59,    80,     0,    80,    60,    80,     0,
    85,     0,   100,     0,    80,    56,   100,     0,    80,    56,
   100,    48,    82,    49,     0,   100,    48,    82,    49,     0,
   100,     0,    81,    56,   100,     0,     0,    83,     0,    80,
     0,    83,    52,    80,     0,     7,     0,   100,    53,     7,
     0,   101,     0,   102,     0,   103,     0,    80,     0,     0,
    28,    30,    88,     0,    81,     0,    88,    52,    81,     0,
     0,    29,    30,    90,     0,    91,     0,    90,    52,    91,
     0,    81,    92,     0,     0,    31,     0,    32,     0,     0,
    29,    30,    94,     0,   101,     0,    94,   101,     0,     0,
    50,    96,    51,     0,    97,     0,    96,    52,    97,     0,
   100,     0,   101,     0,    99,     0,    98,    52,    99,     0,
   100,     0,   103,     0,     6,     0,     3,     0,     4,     0,
     5,     0,   105,     0,   104,    52,   105,     0,    97,    92,
     0,   107,     0,   106,    52,   107,     0,   100,   108,     0,
   100,     0,    86,     0,   109,    52,    86,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    37,    39,    44,    49,    54,    61,    74,    80,    87,    93,
    99,   104,   109,   114,   121,   127,   135,   141,   149,   156,
   165,   171,   180,   187,   195,   202,   211,   216,   221,   228,
   234,   248,   253,   259,   267,   273,   282,   288,   294,   303,
   308,   317,   322,   330,   336,   343,   350,   357,   364,   370,
   377,   384,   390,   396,   402,   408,   414,   420,   426,   432,
   440,   446,   452,   459,   466,   473,   480,   486,   492,   499,
   507,   516,   522,   531,   537,   543,   549,   558,   564,   572,
   576,   580,   586,   594,   600,   606,   612,   621,   627,   633,
   639,   648,   660,   666,   672,   680,   686,   692,   698,   707,
   713,   723,   729,   738,   743,   750,   756,   765,   771,   779,
   787,   795,   803,   815,   821,   830,   842,   848,   857,   866,
   873,   879
};

static const char * const yytname[] = {   "$","error","$undefined.","t_integer",
"t_real","t_string","t_identifier","t_oid","t_create","t_drop","t_load","t_open",
"t_close","t_db","t_table","t_clustered","t_index","t_on","t_delete","t_insert",
"t_into","t_as","t_values","t_update","t_set","t_select","t_from","t_where",
"t_group","t_order","t_by","t_asc","t_dsc","t_intersect","t_minus","t_union",
"t_all","t_distinct","t_null","t_is","t_between","t_overlaps","t_equal","t_not_equal",
"t_less","t_less_equal","t_greater","t_greater_equal","t_lparen","t_rparen",
"t_lbracket","t_rbracket","t_comma","t_arrow","t_and","t_or","t_dot","t_not",
"t_plus","t_star","t_div","t_unary_minus","StartState","Command","Utility","OptClustered",
"Update","AttrUpdateList","AttrUpdate","Query","SetOp","SelectClause","AllDist",
"ExprList","FromClause","RelNameList","RelName","WhereClause","Predicate","Relop",
"Expr","Attref","OptArgList","ArgList","OidRef","Literal","ConstExpr","GroupBy",
"AttrefList","OrderBy","SortOrderList","SortOrder","OptAscDsc","SetOrderBy",
"IntegerList","OptColList","ColList","Col","FilenameList","Filename","Ident",
"Integer","Real","String","IdxAttrList","IdxAttref","ColTypenameList","ColTypename",
"Typename","ValueList",""
};
#endif

static const short yyr1[] = {     0,
    62,    63,    63,    63,    63,    64,    64,    64,    64,    64,
    64,    64,    64,    64,    65,    65,    66,    66,    66,    66,
    67,    67,    68,    68,    69,    69,    70,    70,    70,    71,
    71,    72,    72,    72,    73,    73,    74,    75,    75,    76,
    76,    77,    77,    78,    78,    78,    78,    78,    78,    78,
    78,    78,    79,    79,    79,    79,    79,    79,    79,    79,
    80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
    80,    81,    81,    82,    82,    83,    83,    84,    84,    85,
    85,    85,    86,    87,    87,    88,    88,    89,    89,    90,
    90,    91,    92,    92,    92,    93,    93,    94,    94,    95,
    95,    96,    96,    97,    97,    98,    98,    99,    99,   100,
   101,   102,   103,   104,   104,   105,   106,   106,   107,   108,
   109,   109
};

static const short yyr2[] = {     0,
     1,     1,     1,     1,     0,     9,     3,     6,     5,     3,
     3,     3,     3,     3,     1,     0,     4,     6,     5,     5,
     1,     3,     3,     3,     5,     8,     1,     1,     1,     2,
     3,     0,     1,     1,     1,     3,     2,     1,     3,     1,
     2,     0,     2,     1,     3,     3,     5,     6,     2,     3,
     3,     3,     1,     1,     1,     1,     1,     1,     1,     2,
     2,     2,     3,     3,     3,     3,     1,     1,     3,     6,
     4,     1,     3,     0,     1,     1,     3,     1,     3,     1,
     1,     1,     1,     0,     3,     1,     3,     0,     3,     1,
     3,     2,     0,     1,     1,     0,     3,     1,     2,     0,
     3,     1,     3,     1,     1,     1,     3,     1,     1,     1,
     1,     1,     1,     1,     3,     2,     1,     3,     2,     1,
     1,     3
};

static const short yydefact[] = {     5,
    16,     0,     0,     0,     0,     0,     0,     0,    32,     0,
     1,     2,     3,     4,     0,     0,     0,    15,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   110,     0,    33,
    34,    30,     0,     0,     0,    42,    11,     0,     0,    12,
    10,     7,     0,    13,    14,    42,   100,     0,   111,   112,
   113,     0,     0,    31,    35,    67,    68,    80,    81,    82,
     0,    37,    38,    40,     0,    84,     0,     0,     0,    17,
     0,     0,    42,    21,     0,   104,   105,    62,    61,     0,
     0,     0,     0,     0,     0,    74,    27,    28,    29,     0,
     0,    41,    78,     0,     0,    43,    44,     0,    68,     0,
    88,     0,     0,   117,     0,     9,   106,   108,   109,     0,
   102,     0,    19,     0,    20,     0,    36,    64,    69,    63,
    65,    66,    76,     0,    75,     0,    39,     0,    49,     0,
     0,     0,    59,    53,    54,    55,    56,    57,    58,     0,
     0,     0,     0,     0,     0,    25,   120,   119,     8,     0,
     0,     0,   101,     0,    83,   121,    18,    22,    24,    23,
    74,    71,     0,     0,    52,    50,    51,     0,     0,    60,
    45,    46,    79,    86,    85,    72,     0,   118,     0,   107,
   103,     0,     0,    77,    96,     0,     0,     0,     0,    93,
    89,    90,    93,     0,   114,   122,    70,     0,    26,    47,
     0,    73,    87,    94,    95,    92,     0,   116,     6,     0,
     0,    48,    91,   115,    97,    98,    99,     0,     0,     0
};

static const short yydefgoto[] = {   218,
    11,    12,    19,    13,    73,    74,    14,    90,    15,    33,
    54,    36,    62,    63,    66,    96,   141,   155,   190,   124,
   125,    98,    56,   156,   101,   175,   146,   191,   192,   206,
   199,   215,    72,   110,    75,   106,   107,    57,    58,    59,
    60,   194,   195,   103,   104,   148,   157
};

static const short yypact[] = {   182,
    78,   134,    14,    21,    47,     4,    44,    56,    45,   -19,
-32768,-32768,-32768,-32768,    53,    56,    56,-32768,    54,    56,
    56,    56,    56,    56,    56,    56,    56,-32768,    60,-32768,
-32768,-32768,    42,    77,    56,    93,-32768,    97,    56,-32768,
-32768,-32768,   125,-32768,-32768,    93,   103,   131,-32768,-32768,
-32768,    42,    42,   113,    43,-32768,   118,-32768,-32768,-32768,
   123,   120,-32768,    56,    32,   142,    56,   156,     5,-32768,
   131,    73,    -2,-32768,   132,-32768,-32768,-32768,-32768,    42,
    42,    56,    42,    42,    42,    42,-32768,-32768,-32768,   127,
    56,-32768,-32768,    32,    32,   -12,    71,   135,    -4,   146,
   149,    56,    91,-32768,    56,   137,-32768,-32768,-32768,    57,
-32768,    42,-32768,   131,-32768,    17,    43,    65,   136,    65,
-32768,-32768,    43,   154,   143,   -19,-32768,    84,-32768,    32,
    32,    42,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   119,
    42,   174,   191,    56,   176,-32768,-32768,-32768,-32768,    56,
   160,     5,-32768,   131,    43,-32768,   152,-32768,-32768,    43,
    42,-32768,    42,   161,-32768,-32768,-32768,   155,    42,-32768,
    43,-32768,-32768,   157,   159,-32768,    56,-32768,   131,-32768,
-32768,    42,   163,    43,   185,    42,   162,    56,    56,    40,
   165,-32768,   130,   100,-32768,-32768,-32768,   189,-32768,-32768,
    42,-32768,   157,-32768,-32768,-32768,    56,-32768,-32768,   131,
   212,-32768,-32768,-32768,   212,-32768,-32768,   220,   221,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,   108,    -7,-32768,-32768,-32768,
-32768,-32768,-32768,   133,   -20,   -62,-32768,   102,  -137,    62,
-32768,-32768,-32768,  -128,-32768,-32768,-32768,-32768,    18,    33,
-32768,-32768,-32768,-32768,   -69,-32768,    75,    -8,   -47,-32768,
   -64,-32768,    19,-32768,    81,-32768,-32768
};


#define	YYLAST		265


static const short yytable[] = {    29,
    77,   111,    34,   168,   109,     9,   174,    37,    38,    51,
    28,    40,    41,    42,    43,    44,    45,    46,    47,    49,
    50,    51,    28,    77,    65,    70,    64,    23,    10,    26,
    68,   128,   129,    24,    49,    50,    51,    28,    93,    76,
   187,   130,   131,    86,    49,    50,    51,    28,   143,   114,
    52,   203,   115,   196,   159,    92,    99,   200,   102,    25,
   108,    28,    76,    27,   113,    52,    77,   166,   167,    39,
   204,   205,   212,   119,    53,    52,    81,   172,    35,    94,
    30,    31,    64,    48,   181,    99,    99,   109,    95,    53,
    16,    17,    18,   147,   112,   188,   151,     9,    82,    53,
    83,    84,    85,    32,    81,    76,    77,   153,   154,   193,
   132,   133,   134,   135,   136,   137,   138,   139,   164,    65,
    10,    99,    99,    84,    85,    61,    82,   140,    83,    84,
    85,    77,   165,    49,    55,   176,    28,   130,   131,   149,
   193,   102,   150,   108,    67,    76,    20,    21,   209,    22,
    69,   210,    71,    78,    79,    87,    88,    89,   169,   170,
   204,   205,    77,   216,    80,    86,    97,   217,   176,   100,
    76,    91,   105,   116,   126,   144,   142,   145,    51,   202,
   176,   117,   118,   161,   120,   121,   122,   123,   152,     1,
     2,     3,     4,     5,   163,    97,    97,   173,   176,     6,
     7,    76,   162,   182,     8,   177,     9,   179,   186,   185,
   189,   197,   188,   198,    49,   201,   207,   160,   211,   219,
   220,   158,   183,   127,   213,   208,   180,     0,   214,    10,
   178,    97,    97,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   171,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   123,     0,   184
};

static const short yycheck[] = {     8,
    48,    71,    10,   132,    69,    25,   144,    16,    17,     5,
     6,    20,    21,    22,    23,    24,    25,    26,    27,     3,
     4,     5,     6,    71,    27,    46,    35,    14,    48,    26,
    39,    94,    95,    13,     3,     4,     5,     6,     7,    48,
   169,    54,    55,    48,     3,     4,     5,     6,    53,    52,
    34,   189,    73,   182,    38,    64,    65,   186,    67,    13,
    69,     6,    71,    20,    72,    34,   114,   130,   131,    16,
    31,    32,   201,    82,    58,    34,    34,   142,    26,    48,
    36,    37,    91,    24,   154,    94,    95,   152,    57,    58,
    13,    14,    15,   102,    22,    56,   105,    25,    56,    58,
    58,    59,    60,    59,    34,   114,   154,    51,    52,   179,
    40,    41,    42,    43,    44,    45,    46,    47,   126,    27,
    48,   130,   131,    59,    60,    49,    56,    57,    58,    59,
    60,   179,    49,     3,    33,   144,     6,    54,    55,    49,
   210,   150,    52,   152,    48,   154,    13,    14,    49,    16,
    26,    52,    50,    52,    53,    33,    34,    35,    40,    41,
    31,    32,   210,   211,    52,    48,    65,   215,   177,    28,
   179,    52,    17,    42,    48,    30,    42,    29,     5,   188,
   189,    80,    81,    48,    83,    84,    85,    86,    52,     8,
     9,    10,    11,    12,    52,    94,    95,     7,   207,    18,
    19,   210,    49,    52,    23,    30,    25,    48,    54,    49,
    52,    49,    56,    29,     3,    54,    52,   116,    30,     0,
     0,   114,   161,    91,   207,   193,   152,    -1,   210,    48,
   150,   130,   131,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   141,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   161,    -1,   163
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */


/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int
yyparse(YYPARSE_PARAM)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
{ Bparser_state()->query = yyvsp[0]; ;
    break;}
case 2:
{ 
			  yyval.init (Bastnode_t::Command, Bastnode_t::utility,
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 3:
{ 
			  yyval.init (Bastnode_t::Command, Bastnode_t::update,
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 4:
{ 
			  yyval.init (Bastnode_t::Command, Bastnode_t::query,
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 5:
{ 
			  yyval.init (Bastnode_t::Command, Bastnode_t::empty,
			           0, 0, 0); 
			;
    break;}
case 6:
{
			  Bastnode_t::opcode_t opcode;
			  if (yyvsp[-7].opcode () == Bastnode_t::clustered)
			    opcode = Bastnode_t::create_clus_idx;
			  else
			    opcode = Bastnode_t::create_idx;

			  Bastnode_t args[3] = {yyvsp[-5], yyvsp[-3], yyvsp[-1].flatten ()};
			  yyval.init (Bastnode_t::Utility, opcode,
			           3, args, yyvsp[-8].line ());
			;
    break;}
case 7:
{
			  yyval.init (Bastnode_t::Utility, Bastnode_t::drop_idx,
			           1, &yyvsp[0], yyvsp[-2].line ());
			;
    break;}
case 8:
{
			  Bastnode_t args[2] = {yyvsp[-3], yyvsp[-1].flatten ()};
			  yyval.init (Bastnode_t::Utility, 
			           Bastnode_t::create_table,
			           2, args, yyvsp[-5].line ());
			;
    break;}
case 9:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0].flatten ()};
			  yyval.init (Bastnode_t::Utility, Bastnode_t::load_table,
			           2, args, yyvsp[-4].line ());
			;
    break;}
case 10:
{
			  yyval.init (Bastnode_t::Utility, Bastnode_t::drop_table,
			           1, &yyvsp[0], yyvsp[-2].line ());
			;
    break;}
case 11:
{ 
			  yyval.init (Bastnode_t::Utility, Bastnode_t::create_db,
			           1, &yyvsp[0], yyvsp[-2].line ());
			;
    break;}
case 12:
{ 
			  yyval.init (Bastnode_t::Utility, Bastnode_t::drop_db,
			           1, &yyvsp[0], yyvsp[-2].line ());
			;
    break;}
case 13:
{ 
			  yyval.init (Bastnode_t::Utility, Bastnode_t::open_db,
			           1, &yyvsp[0], yyvsp[-2].line ());
			;
    break;}
case 14:
{ 
			  yyval.init (Bastnode_t::Utility, Bastnode_t::close_db,
			           1, &yyvsp[0], yyvsp[-2].line ());
			;
    break;}
case 15:
{
			  yyval.init (Bastnode_t::OptClustered, 
			           Bastnode_t::clustered,
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 16:
{
			  yyval.init (Bastnode_t::OptClustered, 
			           Bastnode_t::empty,
			           0, 0, -1);
			;
    break;}
case 17:
{
			  Bastnode_t args[2] = {yyvsp[-1], yyvsp[0]};
			  yyval.init (Bastnode_t::Update, Bastnode_t::delete_stmt,
			           2, args, yyvsp[-3].line ());
			;
    break;}
case 18:
{
			  Bastnode_t args[3] = {yyvsp[-3], yyvsp[-2].flatten (), 
			                        yyvsp[0].flatten ()};
			  yyval.init (Bastnode_t::Update, 
			           Bastnode_t::insert_values,
			           3, args, yyvsp[-5].line ());
			;
    break;}
case 19:
{
			  Bastnode_t args[3] = {yyvsp[-2], yyvsp[-1].flatten (), yyvsp[0]};
			  yyval.init (Bastnode_t::Update, 
			           Bastnode_t::insert_query,
			           3, args, yyvsp[-4].line ());
			;
    break;}
case 20:
{
			  Bastnode_t args[3] = {yyvsp[-3], yyvsp[-1].flatten (), yyvsp[0]};
			  yyval.init (Bastnode_t::Update, Bastnode_t::update_stmt,
			           3, args, yyvsp[-4].line ());
			;
    break;}
case 21:
{
			  yyval.init (Bastnode_t::AttrUpdateList, 
			           Bastnode_t::attr_update_list,
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 22:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::AttrUpdateList, 
			           Bastnode_t::attr_update_list,
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 23:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::AttrUpdate, 
			           Bastnode_t::attr_assign,
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 24:
{
			  yyval.init (Bastnode_t::AttrUpdate, 
			           Bastnode_t::attr_null,
			           1, &yyvsp[-2], yyvsp[-2].line ());
			;
    break;}
case 25:
{
			  Bastnode_t args[5] = {yyvsp[-4], yyvsp[-3], yyvsp[-2], yyvsp[-1], yyvsp[0]};
			  yyval.init (Bastnode_t::Query, Bastnode_t::query_stmt,
			           5, args, yyvsp[-4].line ());
			;
    break;}
case 26:
{
			  Bastnode_t args[3] = {yyvsp[-6], yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::Query, yyvsp[-4].opcode (),
			           3, args, yyvsp[-7].line ()); 
			;
    break;}
case 27:
{
			  yyval.init (Bastnode_t::SetOp, Bastnode_t::set_xsect,
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 28:
{
			  yyval.init (Bastnode_t::SetOp, Bastnode_t::set_minus,
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 29:
{
			  yyval.init (Bastnode_t::SetOp, Bastnode_t::set_union,
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 30:
{
			  yyval.init (Bastnode_t::SelectClause,
			           Bastnode_t::select_star, 
			           0, 0, yyvsp[-1].line ());
			;
    break;}
case 31:
{
			  Bastnode_t::opcode_t opcode;
			  if (yyvsp[-1].opcode () == Bastnode_t::distinct)
			    opcode = Bastnode_t::select_dist;
			  else
			    opcode = Bastnode_t::select_all;

			  Bastnode_t args[1] = {yyvsp[0].flatten ()};
			  yyval.init (Bastnode_t::SelectClause, opcode, 
			           1, args, yyvsp[-2].line ()); 
			;
    break;}
case 32:
{
			  yyval.init (Bastnode_t::AllDist, Bastnode_t::empty, 
				   0, 0, -1);
			;
    break;}
case 33:
{
			  yyval.init (Bastnode_t::AllDist,
			           Bastnode_t::all, 
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 34:
{
			  yyval.init (Bastnode_t::AllDist,
			           Bastnode_t::distinct, 
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 35:
{
			  yyval.init (Bastnode_t::ExprList,
			           Bastnode_t::expr_list,
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 36:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::ExprList,
			           Bastnode_t::expr_list, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 37:
{
			  yyval = yyvsp[0].flatten ();
			;
    break;}
case 38:
{
			  yyval.init (Bastnode_t::FromClause,
			           Bastnode_t::from_list, 
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 39:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::FromClause,
			           Bastnode_t::from_list, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 40:
{
			  yyval.init (Bastnode_t::RelName, Bastnode_t::rel_name,
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 41:
{
			  Bastnode_t args[2] = {yyvsp[-1], yyvsp[0]};
			  yyval.init (Bastnode_t::RelName,
			           Bastnode_t::rel_var, 
			           2, args, yyvsp[-1].line ());
			;
    break;}
case 42:
{
			  yyval.init (Bastnode_t::WhereClause, Bastnode_t::empty, 
			           0, 0, -1);
			;
    break;}
case 43:
{
			  yyval.init (Bastnode_t::WhereClause,
			           Bastnode_t::predicate, 
			           1, &yyvsp[0], yyvsp[-1].line ());
			;
    break;}
case 44:
{
			  yyval.init (Bastnode_t::Predicate,
			           Bastnode_t::bool_expr, 
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 45:
{
			  Bastnode_t::opcode_t opcode = yyvsp[-1].opcode ();
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::Predicate, opcode, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 46:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::Predicate,
			           Bastnode_t::oid_ref, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 47:
{
			  Bastnode_t args[3] = {yyvsp[-4], yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::Predicate,
			           Bastnode_t::between, 
			           3, args, yyvsp[-4].line ());
			;
    break;}
case 48:
{
			  Bastnode_t args[3] = {yyvsp[-5], yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::Predicate,
			           Bastnode_t::not_between, 
			           3, args, yyvsp[-5].line ());
			;
    break;}
case 49:
{
			  yyval.init (Bastnode_t::Predicate,
			           Bastnode_t::not, 
			           1, &yyvsp[0], yyvsp[-1].line ());
			;
    break;}
case 50:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::Predicate,
			           Bastnode_t::and, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 51:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::Predicate,
			           Bastnode_t::or, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 52:
{
			  yyval = yyvsp[-1];
			;
    break;}
case 53:
{
			  yyval.init (Bastnode_t::Relop,
			           Bastnode_t::equal, 
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 54:
{
			  yyval.init (Bastnode_t::Relop,
			           Bastnode_t::not_equal, 
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 55:
{
			  yyval.init (Bastnode_t::Relop,
			           Bastnode_t::less, 
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 56:
{
			  yyval.init (Bastnode_t::Relop,
			           Bastnode_t::less_equal, 
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 57:
{
			  yyval.init (Bastnode_t::Relop,
			           Bastnode_t::greater, 
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 58:
{
			  yyval.init (Bastnode_t::Relop,
			           Bastnode_t::greater_equal, 
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 59:
{
			  yyval.init (Bastnode_t::Relop,
			           Bastnode_t::overlaps, 
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 60:
{
			  yyval.init (Bastnode_t::Relop,
			           Bastnode_t::not_overlaps, 
			           0, 0, yyvsp[-1].line ());
			;
    break;}
case 61:
{
			  yyval.init (Bastnode_t::Expr,
			           Bastnode_t::unary_plus, 
			           1, &yyvsp[0], yyvsp[-1].line ());
			;
    break;}
case 62:
{
			  yyval.init (Bastnode_t::Expr,
			           Bastnode_t::unary_minus, 
			           1, &yyvsp[0], yyvsp[-1].line ());
			;
    break;}
case 63:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::Expr,
			           Bastnode_t::plus, 
			           2, args, yyvsp[-1].line ());
			;
    break;}
case 64:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::Expr,
			           Bastnode_t::minus, 
			           2, args, yyvsp[-1].line ());
			;
    break;}
case 65:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::Expr,
			           Bastnode_t::times, 
			           2, args, yyvsp[-1].line ());
			;
    break;}
case 66:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::Expr,
			           Bastnode_t::div, 
			           2, args, yyvsp[-1].line ());
			;
    break;}
case 67:
{
			  yyval.init (Bastnode_t::Expr,
			           Bastnode_t::literal, 
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 68:
{
			  yyval.init (Bastnode_t::Expr,
				   Bastnode_t::attr_or_rel_name, 
				   1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 69:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::Expr,
			           Bastnode_t::reference, 
			           2, args, yyvsp[0].line ());
			;
    break;}
case 70:
{
			  Bastnode_t args[3] = {yyvsp[-5], yyvsp[-3], yyvsp[-1]};
			  yyval.init (Bastnode_t::Expr,
			           Bastnode_t::invoke_method, 
			           3, args, yyvsp[-3].line ());
			;
    break;}
case 71:
{
			  Bastnode_t args[2] = {yyvsp[-3], yyvsp[-1]};
			  yyval.init (Bastnode_t::Expr,
			           Bastnode_t::func_call, 
			           2, args, yyvsp[-3].line ());
			;
    break;}
case 72:
{
			  yyval.init (Bastnode_t::Attref, 
				   Bastnode_t::attr_or_rel_name,
				   1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 73:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::Attref,
			           Bastnode_t::attref, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 74:
{
			  yyval.init (Bastnode_t::ArgList,
			           Bastnode_t::arg_list, 
			           0, 0, -1);
			;
    break;}
case 75:
{
			  yyval = yyvsp[0].flatten ();
			;
    break;}
case 76:
{
			  yyval.init (Bastnode_t::ArgList,
			           Bastnode_t::arg_list, 
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 77:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::ArgList,
			           Bastnode_t::arg_list, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 78:
{
			  yyval.init (Bastnode_t::OidRef,
			           Bastnode_t::oid, 
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 79:
{
			  yyval.init (Bastnode_t::OidRef,
			           Bastnode_t::rel_oid, 
			           1, &yyvsp[-2], yyvsp[-2].line ());
			;
    break;}
case 80:
{
			  yyval.assign (yyvsp[0], Bastnode_t::Literal);
			;
    break;}
case 81:
{
			  yyval.assign (yyvsp[0], Bastnode_t::Literal);
			;
    break;}
case 82:
{
			  yyval.assign (yyvsp[0], Bastnode_t::Literal);
			;
    break;}
case 83:
{
			  yyval.init (Bastnode_t::ConstExpr, 
				   Bastnode_t::const_expr,
				   1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 84:
{
			  yyval.init (Bastnode_t::GroupBy,
			           Bastnode_t::groupby_list, 
			           0, 0, -1);
			;
    break;}
case 85:
{
			  yyval = yyvsp[0].flatten ();
			;
    break;}
case 86:
{
			  yyval.init (Bastnode_t::GroupBy,
			           Bastnode_t::groupby_list, 
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 87:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[-1]};
			  yyval.init (Bastnode_t::GroupBy,
			           Bastnode_t::groupby_list, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 88:
{
			  yyval.init (Bastnode_t::OrderBy,
			           Bastnode_t::orderby_list, 
			           0, 0, -1);
			;
    break;}
case 89:
{
			  yyval = yyvsp[0].flatten ();
			;
    break;}
case 90:
{
			  yyval.init (Bastnode_t::OrderBy,
			           Bastnode_t::orderby_list, 
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 91:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::OrderBy,
			           Bastnode_t::orderby_list, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 92:
{
			  Bastnode_t::opcode_t opcode;
			  if (yyvsp[0].opcode () == Bastnode_t::dsc)
			    opcode = Bastnode_t::dsc;
			  else
			    opcode = Bastnode_t::asc;
			  yyval.init (Bastnode_t::SortOrder, opcode, 
			           1, &yyvsp[-1], yyvsp[-1].line ());
			;
    break;}
case 93:
{
			  yyval.init (Bastnode_t::OptAscDsc,
			           Bastnode_t::empty, 
			           0, 0, -1);
			;
    break;}
case 94:
{
			  yyval.init (Bastnode_t::OptAscDsc,
			           Bastnode_t::asc, 
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 95:
{
			  yyval.init (Bastnode_t::OptAscDsc,
			           Bastnode_t::dsc, 
			           0, 0, yyvsp[0].line ());
			;
    break;}
case 96:
{
			  yyval.init (Bastnode_t::SetOrderBy,
			           Bastnode_t::set_order_by_list, 
			           0, 0, -1);
			;
    break;}
case 97:
{
			  yyval = yyvsp[0].flatten ();
			;
    break;}
case 98:
{
			  yyval.init (Bastnode_t::SetOrderBy,
			           Bastnode_t::set_order_by_list, 
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 99:
{
			  Bastnode_t args[2] = {yyvsp[-1], yyvsp[0]};
			  yyval.init (Bastnode_t::SetOrderBy,
			           Bastnode_t::set_order_by_list, 
			           2, args, yyvsp[-1].line ());
			;
    break;}
case 100:
{
			  yyval.init (Bastnode_t::OptColList,
			           Bastnode_t::col_list, 
			           0, 0, -1);
			;
    break;}
case 101:
{
			/* these should actually be l_paren and r_paren
			   but that would meant that the language is not 
			   LL(1) andt i dont have the enthusiasm to waste 
			   time over this issue */
			  yyval = yyvsp[-1].flatten ();
			;
    break;}
case 102:
{
			  yyval.init (Bastnode_t::OptColList,
			           Bastnode_t::col_list, 
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 103:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::OptColList,
			           Bastnode_t::col_list, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 104:
{
			  yyval.init (Bastnode_t::Col, Bastnode_t::col_name,
				   1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 105:
{
			  yyval.init (Bastnode_t::Col, Bastnode_t::col_number,
				   1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 106:
{
			  yyval.init (Bastnode_t::FilenameList,
			           Bastnode_t::filename_list, 
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 107:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::FilenameList,
			           Bastnode_t::filename_list, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 108:
{
			  yyval.init (Bastnode_t::Filename,
				   Bastnode_t::filename_id,
				   1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 109:
{
			  yyval.init (Bastnode_t::Filename,
				   Bastnode_t::filename_str,
				   1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 110:
{
			  yyval.init (Bastnode_t::Ident, Bastnode_t::identifier,
				   0, 0, yyvsp[0].line ());
			  yyval.set_text (yytext);
			;
    break;}
case 111:
{
			  yyval.init (Bastnode_t::Integer, Bastnode_t::integer,
				   0, 0, yyvsp[0].line ());
			  yyval.set_text (yytext);
			;
    break;}
case 112:
{
			  yyval.init (Bastnode_t::Real, Bastnode_t::real,
				   0, 0, yyvsp[0].line ());
			  yyval.set_text (yytext);
			;
    break;}
case 113:
{
			  yyval.init (Bastnode_t::String, Bastnode_t::string,
				   0, 0, yyvsp[0].line ());
				/*  getting rid of the double quotes */
			  int string_length = strlen (yytext);
			  yytext[string_length-1] = '\0';
			  yyval.set_text (yytext+1);
			;
    break;}
case 114:
{
			  yyval.init (Bastnode_t::IdxAttrList,
			           Bastnode_t::idx_attr_list, 
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 115:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::IdxAttrList,
			           Bastnode_t::idx_attr_list, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 116:
{
			  Bastnode_t::opcode_t opcode;
			  if (yyvsp[0].opcode () == Bastnode_t::dsc)
			    opcode = Bastnode_t::dsc_idx_attref;
			  else
			    opcode = Bastnode_t::asc_idx_attref;
			  yyval.init (Bastnode_t::IdxAttref, opcode, 
			           1, &yyvsp[-1], yyvsp[-1].line ());
			;
    break;}
case 117:
{
			  yyval.init (Bastnode_t::ColTypenameList,
			           Bastnode_t::col_typename_list, 
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 118:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::ColTypenameList,
			           Bastnode_t::col_typename_list, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
case 119:
{
			  Bastnode_t args[2] = {yyvsp[-1], yyvsp[0]};
			  yyval.init (Bastnode_t::ColTypename,
			           Bastnode_t::col_typename, 
			           2, args, yyvsp[-1].line ());
			;
    break;}
case 120:
{
			  yyval.init (Bastnode_t::Typename, Bastnode_t::typename,
				   1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 121:
{
			  yyval.init (Bastnode_t::ValueList,
			           Bastnode_t::value_list, 
			           1, &yyvsp[0], yyvsp[0].line ());
			;
    break;}
case 122:
{
			  Bastnode_t args[2] = {yyvsp[-2], yyvsp[0]};
			  yyval.init (Bastnode_t::ValueList,
			           Bastnode_t::value_list, 
			           2, args, yyvsp[-2].line ());
			;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */


  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
		

void yyerror (char *s) {return;}
