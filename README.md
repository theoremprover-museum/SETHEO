# SETHEO

SETHEO is a theorem prover for first-order logic based on some variant
of the connection method. The inference machine is implemented in C
using PROLOG technology and features preprocessing, search pruning,
factorization, lemma generation, the application of proof schemata
etc. At the first international competition among theorem provers, the
CADE ATP System Competition, or CASC, in 1996 SETHEO won first-place.


## This repository contains:

* "traditional" SETHEO (C version with SAM machine)
	* compiles under Linux and MacOS
	* includes:
		plop:   FOL -> LOP
		inwasm
		sam
	* for compilation and example: src/SETHEO/README.txt
	* SETHEO manual under: doc/PDF   with sources (not sure if the latex)

* PIL (BAN-logic for crypto-protocol verification using SETHEO)
	* src/PIL

* TODO:
	* later competitive SETHEO versions
	* short description
	* bibliography
