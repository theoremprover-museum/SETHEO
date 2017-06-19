/* A Bison parser, made by GNU Bison 2.0.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LABEL = 258,
     NL = 259,
     NUMBER = 260,
     EQU = 261,
     DW = 262,
     DS = 263,
     START = 264,
     ORG = 265,
     OPTIM = 266,
     NOOPT = 267,
     INCLUDE = 268,
     CLAUSE = 269,
     RED = 270,
     SYMB = 271,
     PRED = 272,
     STRING = 273,
     TVOID = 274,
     TVAR = 275,
     TCONST = 276,
     TGLOBAL = 277,
     TGTERM = 278,
     TNGTERM = 279,
     TCSTRVAR = 280,
     TCREF = 281,
     TEOSTR = 282,
     TCRTERM = 283,
     T_i_alloc = 284,
     T_i_nalloc = 285,
     T_i_dealloc = 286,
     T_i_ndealloc = 287,
     T_i_fold_up = 288,
     T_i_reg_fold_up = 289,
     T_i_call = 290,
     T_i_stop = 291,
     T_i_break = 292,
     T_i_jmp = 293,
     T_i_jmpz = 294,
     T_i_jmpg = 295,
     T_i_unvoid = 296,
     T_i_unconst = 297,
     T_i_unvar = 298,
     T_i_ungterm = 299,
     T_i_unngterm = 300,
     T_i_unglobl = 301,
     T_i_orbranch = 302,
     T_i_porbranch = 303,
     T_i_fail = 304,
     T_i_reduct = 305,
     T_i_ureduct = 306,
     T_i_proceed = 307,
     T_i_add = 308,
     T_i_mul = 309,
     T_i_sub = 310,
     T_i_div = 311,
     T_i_lod = 312,
     T_i_sto = 313,
     T_i_glod = 314,
     T_i_gsto = 315,
     T_i_lconst = 316,
     T_i_assign = 317,
     T_i_genreg = 318,
     T_i_genxreg = 319,
     T_i_genureg = 320,
     T_i_cgen = 321,
     T_i_ptree = 322,
     T_i_out = 323,
     T_i_tell = 324,
     T_i_told = 325,
     T_i_eqpred = 326,
     T_i_eq_built = 327,
     T_i_neq_built = 328,
     T_i_equ_unif = 329,
     T_i_isunifiable = 330,
     T_i_isnotunifiable = 331,
     T_i_pre_cut = 332,
     T_i_cut = 333,
     T_i_is_var = 334,
     T_i_is_nonvar = 335,
     T_i_is_number = 336,
     T_i_is_const = 337,
     T_i_is_compl = 338,
     T_i_rand_reorder = 339,
     T_i_setrand = 340,
     T_i_galloc = 341,
     T_i_size = 342,
     T_i_tdepth = 343,
     T_i_tdepth3 = 344,
     T_i_getdepth = 345,
     T_i_getinf = 346,
     T_i_getmaxinf = 347,
     T_i_getlocinf = 348,
     T_i_set_depth = 349,
     T_i_set_inf = 350,
     T_i_set_maxinf = 351,
     T_i_set_locinf = 352,
     T_i_functor = 353,
     T_i_arg = 354,
     T_i_genlemma = 355,
     T_i_genulemma = 356,
     T_i_checklemma = 357,
     T_i_checkulemma = 358,
     T_i_addtoindex = 359,
     T_i_checkindex = 360,
     T_i_uselemma = 361,
     T_i_dumplemma = 362,
     T_i_getnrlemmata = 363,
     T_i_delrange = 364,
     T_i_path2list = 365,
     T_i_assume = 366,
     T_i_init_counters = 367,
     T_i_getcounter = 368,
     T_i_set_counter = 369,
     T_i_get_maxfvars = 370,
     T_i_set_maxfvars = 371,
     T_i_get_maxtc = 372,
     T_i_set_maxtc = 373,
     T_i_get_maxsgs = 374,
     T_i_set_maxsgs = 375,
     T_i_lpo = 376,
     T_i_lpoeq = 377,
     T_i_kbo = 378,
     T_i_sig = 379,
     T_i_precall = 380,
     T_i_isempty = 381,
     T_i_issubset = 382,
     T_i_ismember = 383,
     T_i_card = 384,
     T_i_mkempty = 385,
     T_i_addtoset = 386,
     T_i_remfromset = 387,
     T_i_selnth = 388,
     T_i_selnext = 389,
     T_i_postunify = 390,
     T_i_argsize = 391,
     T_i_preunify = 392,
     T_i_array_assign = 393,
     T_i_array_glod = 394,
     T_i_array_gsto = 395
   };
#endif
#define LABEL 258
#define NL 259
#define NUMBER 260
#define EQU 261
#define DW 262
#define DS 263
#define START 264
#define ORG 265
#define OPTIM 266
#define NOOPT 267
#define INCLUDE 268
#define CLAUSE 269
#define RED 270
#define SYMB 271
#define PRED 272
#define STRING 273
#define TVOID 274
#define TVAR 275
#define TCONST 276
#define TGLOBAL 277
#define TGTERM 278
#define TNGTERM 279
#define TCSTRVAR 280
#define TCREF 281
#define TEOSTR 282
#define TCRTERM 283
#define T_i_alloc 284
#define T_i_nalloc 285
#define T_i_dealloc 286
#define T_i_ndealloc 287
#define T_i_fold_up 288
#define T_i_reg_fold_up 289
#define T_i_call 290
#define T_i_stop 291
#define T_i_break 292
#define T_i_jmp 293
#define T_i_jmpz 294
#define T_i_jmpg 295
#define T_i_unvoid 296
#define T_i_unconst 297
#define T_i_unvar 298
#define T_i_ungterm 299
#define T_i_unngterm 300
#define T_i_unglobl 301
#define T_i_orbranch 302
#define T_i_porbranch 303
#define T_i_fail 304
#define T_i_reduct 305
#define T_i_ureduct 306
#define T_i_proceed 307
#define T_i_add 308
#define T_i_mul 309
#define T_i_sub 310
#define T_i_div 311
#define T_i_lod 312
#define T_i_sto 313
#define T_i_glod 314
#define T_i_gsto 315
#define T_i_lconst 316
#define T_i_assign 317
#define T_i_genreg 318
#define T_i_genxreg 319
#define T_i_genureg 320
#define T_i_cgen 321
#define T_i_ptree 322
#define T_i_out 323
#define T_i_tell 324
#define T_i_told 325
#define T_i_eqpred 326
#define T_i_eq_built 327
#define T_i_neq_built 328
#define T_i_equ_unif 329
#define T_i_isunifiable 330
#define T_i_isnotunifiable 331
#define T_i_pre_cut 332
#define T_i_cut 333
#define T_i_is_var 334
#define T_i_is_nonvar 335
#define T_i_is_number 336
#define T_i_is_const 337
#define T_i_is_compl 338
#define T_i_rand_reorder 339
#define T_i_setrand 340
#define T_i_galloc 341
#define T_i_size 342
#define T_i_tdepth 343
#define T_i_tdepth3 344
#define T_i_getdepth 345
#define T_i_getinf 346
#define T_i_getmaxinf 347
#define T_i_getlocinf 348
#define T_i_set_depth 349
#define T_i_set_inf 350
#define T_i_set_maxinf 351
#define T_i_set_locinf 352
#define T_i_functor 353
#define T_i_arg 354
#define T_i_genlemma 355
#define T_i_genulemma 356
#define T_i_checklemma 357
#define T_i_checkulemma 358
#define T_i_addtoindex 359
#define T_i_checkindex 360
#define T_i_uselemma 361
#define T_i_dumplemma 362
#define T_i_getnrlemmata 363
#define T_i_delrange 364
#define T_i_path2list 365
#define T_i_assume 366
#define T_i_init_counters 367
#define T_i_getcounter 368
#define T_i_set_counter 369
#define T_i_get_maxfvars 370
#define T_i_set_maxfvars 371
#define T_i_get_maxtc 372
#define T_i_set_maxtc 373
#define T_i_get_maxsgs 374
#define T_i_set_maxsgs 375
#define T_i_lpo 376
#define T_i_lpoeq 377
#define T_i_kbo 378
#define T_i_sig 379
#define T_i_precall 380
#define T_i_isempty 381
#define T_i_issubset 382
#define T_i_ismember 383
#define T_i_card 384
#define T_i_mkempty 385
#define T_i_addtoset 386
#define T_i_remfromset 387
#define T_i_selnth 388
#define T_i_selnext 389
#define T_i_postunify 390
#define T_i_argsize 391
#define T_i_preunify 392
#define T_i_array_assign 393
#define T_i_array_glod 394
#define T_i_array_gsto 395




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



