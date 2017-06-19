/******************************************************
*     PIL System
*     $Id: setheo2block.pl,v 1.1.1.1 2001/07/16 18:16:09 schumann Exp $
*
*     (c) Johann Schumann, Andreas Wolf
*     TU Muenchen and NASA Ames
* 	SWI Prolog port of the Eclipse version of
*	setheo2block (A. Wolf)
*
*     $Log: setheo2block.pl,v $
*     Revision 1.1.1.1  2001/07/16 18:16:09  schumann
*     new CVS project
*
*****************************************************/

:- dynamic(proof/3),
   dynamic(block_proof/3),
   dynamic(r_tree/3),
   dynamic(r_connect/2),
   dynamic(main_con/2),
   dynamic(e_connect/2),
   dynamic(rem/1),
   dynamic(to_r_connect/2).

:-op(500,fx,not).

start_working:-
	%   	see('TMP-kerb-1.pr3'), 
    get_file,
    transform_proof,
    write_proof,
    halt. 
    
get_file:-
    repeat,
    read(X),
    (X=end_of_file; (assert(X), fail)).
    
write_proof:-
    block_proof(Handle,Key,Info),
    printf("proof(%QDMTw,%QDMTw,(%QDMTw)).\n",[Handle,Key,Info]),
    fail.
write_proof.

/**********************************/
/* AUXs */
/**********************************/
printf(X,[H,K,I]) :-
	write('proof('),
	write(H),
	write(','),		
	write(K),		
	write(',('),		
	write(I),		
	write(')).\n').
	
setval(Flag,Val) :-
	flag(Flag,_,Val).
getval(Flag,Val) :-
	flag(Flag,Val,Val).
incval(Flag) :-
	flag(Flag,Val,Val+1).

/**********************************/
transform_proof:-
    setval(node_counter,0),
    setval(proof_counter,0),
    construct_reductions_tree,
    construct_initial_tree(Pos),
    translate_reductions_tree(none,Pos),
    translate_extensions.

nextnode(N):-
    incval(node_counter),
    getval(node_counter,N),
    !.

nextproof(sub(Job,N)):-
    incval(proof_counter),
    getval(proof_counter,N),
    proof([Job,_],_,_),
    !.

construct_reductions_tree:-
    get_reductions,
    get_factorizations,
    get_extensions,
    order_r_tree(none,none,none),
    move_facs_up,
    control_facs_up,
    disconnect_lower_reds, 
    !.
    
get_reductions:-
    proof(Lower_node,status,red(Upper_node)),
    once((r_tree(red,Upper_node,Lower_node);
          assert(r_tree(red,Upper_node,Lower_node))
        )),
    fail.
get_reductions:-
    !.

get_factorizations:-
    proof(Image_node,status,fac(Original_node)),
    once((r_tree(fak,Original_node,Image_node);
          assert(r_tree(fak,Original_node,Image_node))
        )),
    once((r_tree(fak,Original_node,Original_node);
          assert(r_tree(fak,Original_node,Original_node))
        )),
    fail.
get_factorizations:-
    !.
    
get_extensions:-
    proof(Node,status,ext),
    once((proof(Node,predecessors,[Contra,_]);
          proof(Node,predecessors,[Contra])
        )), 
    proof(Contra,status,contra),
    proof(Contra,contents,Contra_form),
    proof(Contra,predecessors,[Axiom_node]),
    proof(Axiom_node,status,axiom(Axiom_name)),
    proof(Axiom_node,contents,Axiom_form),
    once((r_tree(ext,Node,[Contra_form,Axiom_name,Axiom_form]);
          assert(r_tree(ext,Node,[Contra_form,Axiom_name,Axiom_form]))
        )),
    fail.        
get_extensions:-
    !.

order_r_tree(Root,Upper,Ex):-
    ((Root=none,proof(Node,contents,not(query)));
     proof(Node,predecessors,[Root]);
     proof(Node,predecessors,[_,Root])),
    order_r_tree_2(Node,Upper,NUpper),
    order_r_tree_3(Node,Ex,Nex),
    order_r_tree(Node,NUpper,Nex),
    fail.
order_r_tree(_,_,_):-
    !.    

order_r_tree_2(Node,Upper,Node):-
    r_tree(fak,Node,Node),
    (r_connect(Upper,Node);assert(r_connect(Upper,Node))),
    (main_con(Upper,Node);assert(main_con(Upper,Node))),
    !.
order_r_tree_2(Node,Upper,Node):-
    r_tree(fak,Node1,Node),
    (r_connect(Upper,Node1);assert(r_connect(Upper,Node1))),
    !.
order_r_tree_2(Node,Upper,Node):-
    r_tree(red,Node,_),
    (r_connect(Upper,Node);assert(r_connect(Upper,Node))),
    !.
order_r_tree_2(Node,Upper,Node):-
    r_tree(red,_,Node),
    (r_connect(Upper,Node);assert(r_connect(Upper,Node))),
    !.
order_r_tree_2(_,Upper,Upper):-
    !.
order_r_tree_3(Node,Ex,Node):-
    r_tree(ext,Node,_),
    (e_connect(Ex,Node);assert(e_connect(Ex,Node))),
    !.    
order_r_tree_3(_,Ex,Ex):-
    !.

move_facs_up:-
    remarking_faks,
    repeat,
    get_upper_fac(Node),
    move_facs_up_1(Node),
    Node=ready,
    !.

control_facs_up:-
    rem(_),
    write("SETHEO-2-BLOCK: WARNING! Factorization Order!"),
    nl, 
    !.    
control_facs_up.

remarking_faks:-
    r_tree(fak,Node,Node),
    assert(rem(Node)),
    fail.
remarking_faks:-!.    

get_upper_fac(Node):-
    rem(Node),
    upper_fac(Node),
    !.
get_upper_fac(ready).


upper_fac(Node):-
    rem(X),
    not(X=Node),
    r_d_connected(X,Node),
    !,fail.
upper_fac(_):-!.

move_facs_up_1(ready).
move_facs_up_1(Node):-
    main_con(Oneup,Node),
    get_lowest_red(Red,Oneup,Node),
    control_correct(Node,Red),
    disconnect_facs(Node),
    asserta(r_connect(Red,Node)),
    retract(rem(Node)),
    !.
    
get_lowest_red(Upper,X,Node):-
    r_d_connected(Upper,X),
    r_tree(red,Upper,Lower),
    r_d_connected(Node,Lower),
    !.
get_lowest_red(none,_,_):-
    !.
    
r_d_connected(U,U).
r_d_connected(U,L):-
    r_connect(X,L),
    r_d_connected(U,X).
    
control_correct(Node,Red):-
    r_connect(X,Node),
    not(r_d_connected(Red,X)),
    write("SETHEO-2-BLOCK: WARNING! Factorization Error!"),
    nl,
    !.
control_correct(_,_):-!.

disconnect_facs(Node):-
    r_connect(X,Node),
    retract(r_connect(X,Node)),
    fail.
disconnect_facs(_):-
    !.
    
disconnect_lower_reds:-
    r_tree(red,_,Node),
    retract(r_connect(_,Node)),
    fail.            
disconnect_lower_reds:-
    !.

construct_initial_tree([Sub1,Node3]):-
    proof([Job,_],_,_),
/*    assert(block_proof([Job,global],system,setheo)),
    assert(block_proof([Job,global],control,proved)),
    assert(block_proof([Job,global],control,ready)), */  /* im Rahmenscript */
    (proof([Job,global],goal,Goal);Goal=goal),
    assert(block_proof([Job,global],goal,Goal)),
    nextnode(Node1),
    nextproof(Sub1), 
    nextnode(Node2), 
    nextnode(Node3), 
    assert(block_proof([Job,Node1],contents,query)),
    assert(block_proof([Job,Node1],predecessors,[])),    
    assert(block_proof([Job,Node1],status,subproof(Sub1))),
    assert(block_proof([Job,Node1],control,rule([indirect],[[Sub1,Node2],[Sub1,Node3]]))),
    assert(block_proof([Sub1,Node2],contents,not(query))),
    assert(block_proof([Sub1,Node2],predecessors,[])),    
    assert(block_proof([Sub1,Node2],status,assumption([]))),
    assert(block_proof([Sub1,Node3],contents,contradiction)),
    assert(block_proof([Sub1,Node3],predecessors,[[Sub1,Node2]])),    
    assert(block_proof([Sub1,Node3],status,proved)),
    assert(to_r_connect([Sub1,Node3],none)),
    !.
    
translate_reductions_tree(Rold,[Sub,Anode]):-
    r_connect(Rold,Rnode),
    translate_reductions_tree_1(Rnode,[Sub,Anode],[Sub1,N2]),
    translate_reductions_tree(Rnode,[Sub1,N2]),
    fail.
translate_reductions_tree(_,_):-
    !.    
    
translate_reductions_tree_1(Rnode,[Sub,Anode],[Sub1,N2]):-
    proof(Rnode,contents,Formula),
    negate(Formula,Negformula,Rule),
    nextnode(Node),
    assert(block_proof([Sub,Node],contents,Negformula)),
    retract(block_proof([Sub,Anode],predecessors,L1)),
    assert(block_proof([Sub,Anode],predecessors,[[Sub,Node]])),
    assert(block_proof([Sub,Node],predecessors,L1)),
    nextproof(Sub1),
    assert(block_proof([Sub,Node],status,subproof(Sub1))),
    nextnode(N1),
    nextnode(N2),
    assert(block_proof([Sub,Node],control,rule([Rule],[[Sub1,N1],[Sub1,N2]]))),
    assert(block_proof([Sub1,N1],contents,Formula)),
    assert(block_proof([Sub1,N1],status,assumption([]))),
    assert(block_proof([Sub1,N1],predecessors,[])),
    assert(block_proof([Sub1,N2],contents,contradiction)),
    assert(block_proof([Sub1,N2],status,proved)),
    assert(to_r_connect([Sub1,N2],Rnode)),
    assert(block_proof([Sub1,N2],predecessors,[[Sub1,N1]])),
    !.
        
negate(F,N,indirect):-
    F=not(N),
    !.    
negate(F,N,in(not)):-
    N=not(F),
    !.

translate_extensions:-
    get_task(Task),
    integrate_extension(Task),
    complete_task(Task),
    fail.
translate_extensions:-
    !.

get_task(none).
get_task(T):-
    r_connect(_,T).

integrate_extension(Task):-
    e_connect(Task,NTask),
    integrate_extension_1(NTask,Task),
    fail.
integrate_extension(Task):-
    integrate_extension_2(Task,Task),
    !.
integrate_extension_1(Task,_):-
    r_connect(_,Task),
    !.
integrate_extension_1(Task,Main):-
    e_connect(Task,NTask),
    integrate_extension_1(NTask,Main),
    fail.
integrate_extension_1(Task,Main):-
    integrate_extension_2(Task,Main),
    !.
integrate_extension_2(none,_):-
    !.
integrate_extension_2(Task,Main):-
    to_r_connect([Sub,Pos],Main),
    proof(Task,contents,Lit),
    negate(Lit,Conclusion,_),
    r_tree(ext,Task,[Contra,Axiom,Axform]),
    get_premises(Task,Poslist,[Sub,Pos]),
    integrate_extension_3([Sub,Pos],Contra,Conclusion,Axiom,Axform,Poslist),
    !.
integrate_extension_3([Sub,Pos],_,Conclusion,Axiom,_,[]):-      
    block_proof([Px,N2],control,axiom(Axiom)),
    nextnode(N1),
    retract(block_proof([Sub,Pos],predecessors,Pred)),
    assert(block_proof([Sub,Pos],predecessors,[[Sub,N1]])),
    assert(block_proof([Sub,N1],predecessors,Pred)),
    assert(block_proof([Sub,N1],contents,Conclusion)),
    assert(block_proof([Sub,N1],status,proved)),
    assert(block_proof([Sub,N1],control,rule([contrapositive],[[Px,N2]]))),
    !.
integrate_extension_3([Sub,Pos],_,Conclusion,Axiom,Axform,[]):-      
    nextnode(N1),
    nextnode(N2),
    nextproof(Px),
    retract(block_proof([Sub,Pos],predecessors,Pred)),
    assert(block_proof([Sub,Pos],predecessors,[[Sub,N1]])),
    assert(block_proof([Sub,N1],predecessors,Pred)),
    assert(block_proof([Px,N2],predecessors,[])),
    assert(block_proof([Sub,N1],contents,Conclusion)),
    assert(block_proof([Sub,N1],status,proved)),
    assert(block_proof([Sub,N1],control,rule([contrapositive],[[Px,N2]]))),
    assert(block_proof([Px,N2],contents,Axform)),
    assert(block_proof([Px,N2],status,proved)),
    assert(block_proof([Px,N2],control,axiom(Axiom))),
    !.
integrate_extension_3([Sub,Pos],Contra,Conclusion,Axiom,_,Poslist):-      
    block_proof([Px,N3],control,axiom(Axiom)),
    nextnode(N1),
    nextnode(N2),
    retract(block_proof([Sub,Pos],predecessors,Pred)),
    assert(block_proof([Sub,Pos],predecessors,[[Sub,N1]])),
    assert(block_proof([Sub,N1],predecessors,[[Sub,N2]])),
    assert(block_proof([Sub,N2],predecessors,Pred)),
    assert(block_proof([Sub,N1],contents,Conclusion)),
    assert(block_proof([Sub,N1],status,proved)),
    Usedlist=[[Sub,N2]|Poslist], 
    assert(block_proof([Sub,N1],control,rule([in(and),out(imp)],Usedlist))),
    assert(block_proof([Sub,N2],contents,Contra)),
    assert(block_proof([Sub,N2],status,proved)),
    assert(block_proof([Sub,N2],control,rule([contrapositive],[[Px,N3]]))),
    !.
integrate_extension_3([Sub,Pos],Contra,Conclusion,Axiom,Axform,Poslist):-      
    nextnode(N1),
    nextnode(N2),
    nextnode(N3),
    nextproof(Px),
    retract(block_proof([Sub,Pos],predecessors,Pred)),
    assert(block_proof([Sub,Pos],predecessors,[[Sub,N1]])),
    assert(block_proof([Sub,N1],predecessors,[[Sub,N2]])),
    assert(block_proof([Sub,N2],predecessors,Pred)),
    assert(block_proof([Px,N3],predecessors,[])),
    assert(block_proof([Sub,N1],contents,Conclusion)),
    assert(block_proof([Sub,N1],status,proved)),
    Usedlist=[[Sub,N2]|Poslist], 
    assert(block_proof([Sub,N1],control,rule([in(and),out(imp)],Usedlist))),
    assert(block_proof([Sub,N2],contents,Contra)),
    assert(block_proof([Sub,N2],status,proved)),
    assert(block_proof([Sub,N2],control,rule([contrapositive],[[Px,N3]]))),
    assert(block_proof([Px,N3],contents,Axform)),
    assert(block_proof([Px,N3],status,proved)),
    assert(block_proof([Px,N3],control,axiom(Axiom))),
    !.
    
complete_task(Task):-
    to_r_connect([Sub,Node],Task),
    block_proof([Sub,Root],predecessors,[]),
    block_proof([Sub,Node],predecessors,[Pred]),
    assert(block_proof([Sub,Node],control,rule([in(contradiction),since],[[Sub,Root],Pred]))),
    !.
    

/* ACHTUNG!!! Dasjenige Alternativglied, das im ME-Baum zum Astabschluss 
   benutzt wird, MUSS(!) als erster Fakt unter allen solchen 
   Alternativgliedern des Praedikates proof/3 des Inputbeweises zugreifbar
   sein.
*/

get_premises(Task,Poslist,Above):-
    setof(X,premise(Task,Above,X),Poslist),
    !.
get_premises(_,[],_):-
    !.    

premise(Task,Above,Premise):-
    proof(NotThat,predecessors,[Task]),
    !,
    premise1(Task,Above,Premise,NotThat).
premise1(Task,Above,Premise,NotThat):-
    proof(Opos,predecessors,[Task]),
    not(Opos=NotThat),
    premise2(Opos,Above,Premise).
premise1(Task,Above,Premise,_):-
    proof(Opos,predecessors,[_,Task]),
    premise2(Opos,Above,Premise).
premise1(Task,Above,Premise,_):-
    proof(Opos,predecessors,[Task,_]),
    premise2(Opos,Above,Premise).
premise2(Opos,Above,Premise):-
    proof(Opos,contents,NForm),
    negate(NForm,Form,_),
    block_usable(Form,Above,Premise),
    !.
    
block_usable(Form,Above,Node):-
    block_usable_1(Above,Node),
    block_proof(Node,contents,Form).
block_usable(Form,Above,_):-
    write("SETHEO-2-BLOCK: WARNING! Usable formula "),
    write(Form),
    write(" above "),
    write(Above),
    write(" not found!"),
    nl,
    !,fail.
block_usable_1(Above,Node):-
    block_proof(Above,predecessors,[Node]).
block_usable_1([S,P],Node):-
    block_proof([S,P],predecessors,[]),
    block_proof(Node1,status,subproof(S)),
    block_proof(Node1,predecessors,[Node]).
block_usable_1(Above,Node):-
    block_proof(Above,predecessors,[Node1]),
    block_usable_1(Node1,Node).  
block_usable_1([S,P],Node):-
    block_proof([S,P],predecessors,[]),
    block_proof(Node1,status,subproof(S)),
    block_proof(Node1,predecessors,[Node2]),
    block_usable_1(Node2,Node).













