/* The time has come, the walross said to talk of many things */
/* * * * * *  H O R N  * * * * *  without reductionsteps  * * */
ln00001:
	.clause 0,1
	nalloc	lcb000001
	call
	printtree
	stop
sl000001:
	.dw	lcb000001
lcb000001:
	.dw	ln00001
	.dw	const	10000
	.dw	const	2
	.dw	const	0
	.dw	const	1
	.dw	const	0
	.dw	const	0
	.dw	const	1
	.dw	const	1
	.dw	const	1
	.dw	av000001
	.dw	sl000001
	.dw	or000001
av000001:
/* * * * * * * * * *  starting next original clause * * * * * */
	.clause 1,1
lp00101:
	alloc	lcb001001
	call
	dealloc
sl001001:
	.dw	lcb001002
lcb001001:
	.dw	lp00101
	.dw	const	0
	.dw	const	0
	.dw	const	1
	.dw	const	1
	.dw	const	0
	.dw	const	0
	.dw	const	1
	.dw	const	1
	.dw	const	1
	.dw	av001001
	.dw	sl001001
	.dw	0
av001001:
lcb001002:
	.dw	lcbfailure
	.dw	const	10092
	.dw	const	0
	.dw	const	1
	.dw	const	2
	.dw	const	0
	.dw	const	0
	.dw	const	0
	.dw	const	0
	.dw	const	0
	.dw	av001002
	.dw	0
	.dw	or001002
av001002:
/* * * * * * * * * *  starting next original clause * * * * * */
	.clause 2,1
lp00201:
	nalloc	lcb002001
	ndealloc
sl002001:
lcb002001:
	.dw	lp00201
	.dw	const	92
	.dw	const	0
	.dw	const	2
	.dw	const	1
	.dw	const	0
	.dw	const	0
	.dw	const	0
	.dw	const	0
	.dw	const	0
	.dw	av002001
	.dw	sl002001
	.dw	0
av002001:
/* * * * * * * * * * * WEAK UNIFICATION GRAPH * * * * * * * * */
or000001:
	orbranch	lab001
	.optim
lab001:
	.dw	lcb001001
	.dw	0
	.noopt
or001001:
	orbranch	lab002
	.optim
lab002:
	.dw	lcb000001
	.dw	0
	.noopt
or001002:
	orbranch	lab003
	.optim
lab003:
	.dw	lcb002001
	.dw	0
	.noopt
failure:
	fail
lcbfailure:
	.dw	failure
fcb:
	.dw	const 2
	.dw	const 2
	.dw	const 3
	.dw	const 0
	.dw	const 0
	.dw	const 1
	.dw	const 0
	.dw	const 1
	.dw	const 1
	.dw	const 1
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 1
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
	.dw	const 0
/* * * * * *  S Y M B O L T A B L E * * * * * * * * * * * * * */
	.symb	"query__",	pred, 0
	.symb	"[",	const, 2
	.symb	"[]",	const, 0
	.symb	"+",	const, 2
	.symb	"-",	const, 2
	.symb	"*",	const, 2
	.symb	"/",	const, 2
	.symb	"-_",	const, 1
	.symb	"sem_and",	const, 2
	.symb	"sem_or",	const, 2
	.symb	":=",	pred, 2
	.symb	":is",	pred, 2
	.symb	"is",	pred, 2
	.symb	"$unif",	pred, 2
	.symb	"$mkglob",	pred, 2
	.symb	"$greater",	pred, 2
	.symb	"$less",	pred, 2
	.symb	"$leq",	pred, 2
	.symb	"$geq",	pred, 2
	.symb	"$equal",	pred, 2
	.symb	"$nequal",	pred, 2
	.symb	"=/=",	pred, 2
	.symb	"$equconstr",	pred, 2
	.symb	"$disconstr",	pred, 2
	.symb	"$conjconstr",	pred, 2
	.symb	"$monitor",	pred, 0
	.symb	"$eqpred",	pred, 1
	.symb	"$write",	pred, 1
	.symb	"$fail",	pred, 0
	.symb	"$stop",	pred, 0
	.symb	"$printtree",	pred, 0
	.symb	"$eq",	pred, 2
	.symb	"$neq",	pred, 2
	.symb	"$isvar",	pred, 1
	.symb	"$isnonvar",	pred, 1
	.symb	"$isconst",	pred, 1
	.symb	"$isnumber",	pred, 1
	.symb	"$iscompl",	pred, 1
	.symb	"$unify",	pred, 2
	.symb	"$unifiable",	pred, 2
	.symb	"$notunifiable",	pred, 2
	.symb	"$size",	pred, 2
	.symb	"$tdepth",	pred, 2
	.symb	"$tdepth3",	pred, 3
	.symb	"$initcounters",	pred, 1
	.symb	"$getcounter",	pred, 2
	.symb	"$setcounter",	pred, 2
	.symb	"$setrand",	pred, 1
	.symb	"$getdepth",	pred, 1
	.symb	"$setdepth",	pred, 1
	.symb	"$getinf",	pred, 1
	.symb	"$setinf",	pred, 1
	.symb	"$getmaxinf",	pred, 1
	.symb	"$setmaxinf",	pred, 1
	.symb	"$setlocinf",	pred, 1
	.symb	"$getlocinf",	pred, 1
	.symb	"$functor",	pred, 3
	.symb	"$arg",	pred, 3
	.symb	"$negate",	pred, 2
	.symb	"$getmaxfvars",	pred, 1
	.symb	"$setmaxfvars",	pred, 1
	.symb	"$getmaxtc",	pred, 1
	.symb	"$setmaxtc",	pred, 1
	.symb	"$getmaxsgs",	pred, 1
	.symb	"$setmaxsgs",	pred, 1
	.symb	"$dumplemma",	pred, 0
	.symb	"$checkindex",	pred, 1
	.symb	"$addtoindex",	pred, 1
	.symb	"$genlemma",	pred, 2
	.symb	"$genulemma",	pred, 3
	.symb	"$checklemma",	pred, 1
	.symb	"$checkulemma",	pred, 2
	.symb	"$addlemma",	pred, 1
	.symb	"$addulemma",	pred, 2
	.symb	"$uselemma",	pred, 1
	.symb	"$getnrlemmata",	pred, 1
	.symb	"$foldupparam",	pred, 1
	.symb	"$assumehead",	pred, 0
	.symb	"$delrange",	pred, 2
	.symb	"$path2list",	pred, 1
	.symb	"$assume",	pred, 0
	.symb	"$precut",	pred, 0
	.symb	"$cut",	pred, 0
	.symb	"$lpo",	pred, 3
	.symb	"@$lpo",	pred, 3
	.symb	"$lpoeq",	pred, 3
	.symb	"@$lpoeq",	pred, 3
	.symb	"$kbo",	pred, 3
	.symb	"@$kbo",	pred, 3
	.symb	"$sig",	pred, 1
	.symb	"$tell",	pred, 1
	.symb	"$told",	pred, 0
	.symb	"p",	pred, 0
