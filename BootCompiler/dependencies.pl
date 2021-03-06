:- module(dependencies,[dependencies/6]).

:- use_module(topsort).
:- use_module(abstract).
:- use_module(errors).
:- use_module(misc).
:- use_module(keywords).
:- use_module(wff).

dependencies(Els,Groups,Public,Annots,Imports,Other) :-
  collectDefinitions(Els,Dfs,Public,Annots,Imports,Other),
  allRefs(Dfs,[],AllRefs),
  collectThetaRefs(Dfs,AllRefs,Annots,Defs),
  topsort(Defs,Groups,misc:same).
  % showGroups(Groups).

collectDefinitions([St|Stmts],Defs,P,A,I,Other) :-
  collectDefinition(St,Stmts,S0,Defs,D0,P,P0,A,A0,I,I0,Other,O0,dependencies:nop),
  collectDefinitions(S0,D0,P0,A0,I0,O0).
collectDefinitions([],[],[],[],[],[]).

collectDefinition(St,Stmts,Stmts,Defs,Defs,P,P,A,A,[St|I],I,Other,Other,_) :-
  isImport(St).
collectDefinition(St,Stmts,Stmts,Defs,Defs,P,P,A,A,I,I,[St|Other],Other,_) :-
  isUnary(St,"assert",_).
collectDefinition(St,Stmts,Stmts,Defs,Defs,P,P,A,A,I,I,[St|Other],Other,_) :-
  isUnary(St,"show",_).
collectDefinition(St,Stmts,Stmts,Defs,Defs,P,Px,[(V,St)|A],A,I,I,Other,Other,Export) :-
  isBinary(St,":",L,_),
  isIden(L,V),
  call(Export,var(V),P,Px).
collectDefinition(St,Stmts,Stx,Defs,Dfx,P,P,A,Ax,I,Ix,O,Ox,_) :-
  isUnary(St,"private",Inner),
  collectDefinition(Inner,Stmts,Stx,Defs,Dfx,P,_,A,Ax,I,Ix,O,Ox,dependencies:nop).
collectDefinition(St,Stmts,Stx,Defs,Dfx,P,Px,A,Ax,I,Ix,O,Ox,_) :-
  isUnary(St,"public",Inner),
  collectDefinition(Inner,Stmts,Stx,Defs,Dfx,P,Px,A,Ax,I,Ix,O,Ox,dependencies:export).
collectDefinition(St,Stmts,Stmts,[(Nm,Lc,[St])|Defs],Defs,P,Px,A,A,I,I,O,O,Export) :-
  isUnary(St,Lc,"contract",Inner),
  contractName(Inner,Nm),
  call(Export,Nm,P,Px).
collectDefinition(St,Stmts,Stmts,[(Nm,Lc,[St])|Defs],Defs,P,Px,A,A,I,I,O,O,Export) :-
  isImplementationStmt(St,Lc,_,_,Con,_),!,
  implementationName(Con,Nm),
  call(Export,Nm,P,Px).
collectDefinition(St,Stmts,Stmts,Defs,Defs,Px,Px,A,A,I,I,O,O,_) :-
  isBinary(St,"@",_,_).
collectDefinition(St,Stmts,Stmts,Defs,Defs,Px,Px,A,A,I,I,O,O,_) :-
  isUnary(St,"@",_).
collectDefinition(St,Stmts,Stx,[(Nm,Lc,[St|Defn])|Defs],Defs,P,Px,A,A,I,I,O,O,Export) :-
  ruleName(St,Nm,Kind),
  locOfAst(St,Lc),
  collectDefines(Stmts,Kind,Stx,Nm,Defn),
  call(Export,Nm,P,Px).
collectDefinition(St,Stmts,Stmts,Defs,Defs,P,P,A,A,I,I,O,O,_) :-
  locOfAst(St,Lc),
  reportError("Cannot fathom %s",[St],Lc).

export(Nm,[Nm|P],P).
nop(_,P,P).

isImport(St) :-
  isUnary(St,"public",I),!,
  isImport(I).
isImport(St) :-
  isUnary(St,"private",I),!,
  isImport(I).
isImport(St) :-
  isUnary(St,"import",_).

ruleName(St,Name,Mode) :-
  isQuantified(St,_,B),!,
  ruleName(B,Name,Mode).
ruleName(St,Name,Mode) :-
  isBinary(St,"|:",_,R),
  ruleName(R,Name,Mode).
ruleName(St,Nm,con) :-
  isUnary(St,"contract",I),
  contractName(I,Nm),!.
ruleName(St,Nm,impl) :-
  isImplementationStmt(St,_,_,_,Con,_),!,
  implementationName(Con,Nm),!.
ruleName(St,Nm,type) :-
  isUnary(St,"type",I),
  ruleName(I,Nm,type).
ruleName(St,tpe(Nm),type) :-
  isBinary(St,"<~",L,_),
  typeName(L,Nm).
ruleName(St,var(Nm),value) :-
  headOfRule(St,Hd),
  headName(Hd,Nm).

contractName(St,Nm) :-
  isQuantified(St,_,B),
  contractName(B,Nm).
contractName(St,Nm) :-
  isBinary(St,"|:",_,R),
  contractName(R,Nm).
contractName(St,Nm) :-
  isBinary(St,"::=",L,_),
  contractName(L,Nm).
contractName(St,con(Nm)) :-
  isSquare(St,Nm,_).

%% Thus must mirror the definition in types.pl
implementationName(St,imp(Nm)) :-
  implementedContractName(St,Nm).

implementedContractName(Sq,INm) :-
  isSquare(Sq,Nm,A),
  appStr(Nm,S0,S1),
  marker(conTract,M),
  surfaceNames(A,M,S1,[]),
  string_chars(INm,S0).

surfaceNames([],_,S,S).
surfaceNames([T|_],Sep,S0,Sx) :-
  isBinary(T,Lc,"->>",L,_),!,
  tupleize(L,"()",Lc,tuple(_,_,Els)),
  surfaceNames(Els,Sep,S0,Sx).
surfaceNames([T|L],Sep,S0,Sx) :-
  surfaceName(T,SN),
  appStr(Sep,S0,S1),
  appStr(SN,S1,S2),
  surfaceNames(L,Sep,S2,Sx).

surfaceName(N,Nm) :-
  isIden(N,Nm).
surfaceName(N,Nm) :-
  isSquare(N,Nm,_).
surfaceName(T,Nm) :-
  isTuple(T,_,A),
  length(A,Ar),
  swritef(Nm,"()%d",[Ar]).

collectDefines([St|Stmts],Kind,OSt,Nm,[St|Defn]) :-
  ruleName(St,Nm,Kind),
  collectDefines(Stmts,Kind,OSt,Nm,Defn).
collectDefines([St|Stmts],Kind,[St|OSt],Nm,Defn) :-
  collectDefines(Stmts,Kind,OSt,Nm,Defn).
collectDefines(Stmts,_,Stmts,_,[]).

headOfRule(St,Hd) :-
  isBinary(St,"=",Hd,_).
headOfRule(St,Hd) :-
  isBinary(St,"=>",Hd,_).
headOfRule(St,Hd) :-
  isBinary(St,":-",L,_),
  headOfRule(L,Hd).
headOfRule(St,Hd) :-
  isBinary(St,"<=",Hd,_).
headOfRule(St,Hd) :-
  isBraceTerm(St,_,Hd,_),!.
headOfRule(St,Hd) :-
  isBinary(St,"-->",H,_),
  (isBinary(H,",",Hd,_) ; H=Hd),!.
headOfRule(St,St) :-
  isRound(St,Nm,_), \+ isRuleKeyword(Nm).

headName(Head,Nm) :-
  isRoundTerm(Head,Op,_),
  headName(Op,Nm).
headName(Head,Nm) :-
  isBrace(Head,Nm,_).
headName(Name,Nm) :-
  isName(Name,Nm),
  \+isKeyword(Nm).
headName(tuple(_,"()",[Name]),Nm) :-
  headName(Name,Nm).

typeName(Tp,Nm) :-
  isBinary(Tp,"|:",_,R),
  typeName(R,Nm).
typeName(Tp,Nm) :- isSquare(Tp,Nm,_), \+ isKeyword(Nm).
typeName(Tp,Nm) :- isName(Tp,Nm), \+ isKeyword(Nm).

allRefs([(N,_,_)|Defs],SoFar,AllRefs) :-
  allRefs(Defs,[N|SoFar],AllRefs).
allRefs([],SoFar,SoFar).

collectThetaRefs([],_,_,[]).
collectThetaRefs([(Defines,Lc,Def)|Defs],AllRefs,Annots,[(Defines,Refs,Lc,Def)|Dfns]) :-
  collectStmtRefs(Def,AllRefs,Annots,Refs,[]),
  collectThetaRefs(Defs,AllRefs,Annots,Dfns).

collectStmtRefs([],_,_,Refs,Refs).
collectStmtRefs([St|Stmts],All,Annots,SoFar,Refs) :-
  collRefs(St,All,Annots,SoFar,S0),
  collectStmtRefs(Stmts,All,Annots,S0,Refs).

collRefs(St,All,Annots,SoFar,Refs) :-
  isQuantified(St,_,B),
  collRefs(B,All,Annots,SoFar,Refs).
collRefs(St,All,Annots,SoFar,Rest) :-
  isBinary(St,"|:",L,Inner),
  collConstraints(L,All,SoFar,R0),
  collRefs(Inner,All,Annots,R0,Rest).
collRefs(St,All,Annots,SoFar,Refs) :-
  isBinary(St,"=",H,Exp),
  collectAnnotRefs(H,All,Annots,SoFar,R0),
  collectHeadRefs(H,All,R0,R1),
  collectExpRefs(Exp,All,R1,Refs).
collRefs(St,All,Annots,SoFar,Refs) :-
  isBinary(St,"=>",H,Exp),
  collectAnnotRefs(H,All,Annots,SoFar,R0),
  collectHeadRefs(H,All,R0,R1),
  collectExpRefs(Exp,All,R1,Refs).
collRefs(St,All,Annots,SoFar,Refs) :-
  isBinary(St,":-",L,C),
  isBinary(L,"=>",H,R),
  collectAnnotRefs(H,All,Annots,SoFar,R0),
  collectHeadRefs(H,All,R0,R1),
  collectCondRefs(C,All,R1,R2),
  collectExpRefs(R,All,R2,Refs).
collRefs(St,All,Annots,SoFar,Refs) :-
  isBinary(St,"-->",H,Body),
  collectAnnotRefs(H,All,Annots,SoFar,R0),
  collectGrHeadRefs(H,All,R0,R1),
  collectNTRefs(Body,All,R1,Refs).
collRefs(St,All,Annots,SoFar,Refs) :-
  isBinary(St,"<=",H,Exp),
  collectAnnotRefs(H,All,Annots,SoFar,R0),
  collectHeadRefs(H,All,R0,R1),
  collectLabelRefs(Exp,All,R1,Refs).
collRefs(St,All,Annots,SoFar,Refs) :-
  isBraceTerm(St,_,H,Els),
  collectAnnotRefs(H,All,Annots,SoFar,R0),
  collectHeadRefs(H,All,R0,R1),
  collectClassRefs(Els,All,R1,Refs).
collRefs(St,All,Annots,SoFar,Refs) :-
  isBinary(St,":-",H,Exp),
  collectAnnotRefs(H,All,Annots,SoFar,R0),
  collectHeadRefs(H,All,R0,R1),
  collectCondRefs(Exp,All,R1,Refs).
collRefs(St,All,_,R0,Refs) :-
  isBinary(St,"<~",_,Tp),
  collectTypeRefs(Tp,All,R0,Refs).
collRefs(St,All,_,R0,Rx) :-
  isImplementationStmt(St,_,_,Cx,_,T),
  collectContractRefs(Cx,All,R0,R1),
  collectExpRefs(T,All,R1,Rx).

collRefs(St,All,Annots,SoFar,Refs) :-
  collectAnnotRefs(St,All,Annots,SoFar,R0),
  collectHeadRefs(St,All,R0,Refs).

collectHeadRefs(Hd,All,R0,Refs) :-
  isBinary(Hd,"@@",L,R),
  collectHeadRefs(L,All,R0,R1),
  collectCondRefs(R,All,R1,Refs).
collectHeadRefs(Hd,All,R0,Refs) :-
  isRoundTerm(Hd,_,A),
  collectPtnListRefs(A,All,R0,Refs).
collectHeadRefs(_,_,R,R).

collectGrHeadRefs(Hd,All,R0,Refs) :-
  isBinary(Hd,",",L,R),!,
  collectHeadRefs(L,All,R0,R1),
  collectPtnRefs(R,All,R1,Refs).
collectGrHeadRefs(Hd,All,R0,Refs) :-
  collectHeadRefs(Hd,All,R0,Refs).

collectNTRefs(T,All,R,Refs) :-
  isSquareTuple(T,_,Els),
  collectExpListRefs(Els,All,R,Refs).
collectNTRefs(T,All,R,Refs) :-
  isRoundTuple(T,_,Els),
  collectNTRefList(Els,All,R,Refs).
collectNTRefs(T,All,R,Refs) :-
  isBraceTuple(T,_,Els),
  collectCondListRefs(Els,All,R,Refs).
collectNTRefs(T,All,Rf,Refs) :-
  isBinary(T,"|",L,R),
  isBinary(L,"?",LL,LR),
  collectNTRefs(LL,All,Rf,R0),
  collectNTRefs(LR,All,R0,R1),
  collectNTRefs(R,All,R1,Refs).
collectNTRefs(T,All,Rf,Refs) :-
  isBinary(T,"|",L,R),
  collectNTRefs(L,All,Rf,R0),
  collectNTRefs(R,All,R0,Refs).
collectNTRefs(T,All,Rf,Refs) :-
  isBinary(T,",",L,R),
  collectNTRefs(L,All,Rf,R0),
  collectNTRefs(R,All,R0,Refs).
collectNTRefs(T,All,Rf,Refs) :-
  isUnary(T,"!",L),
  collectNTRefs(L,All,Rf,Refs).
collectNTRefs(T,All,Rf,Refs) :-
  isUnary(T,"\\+",L),
  collectNTRefs(L,All,Rf,Refs).
collectNTRefs(T,All,Rf,Refs) :-
  isUnary(T,"+",L),
  collectNTRefs(L,All,Rf,Refs).
collectNTRefs(T,All,Rf,Refs) :-
  isBinary(T,"@@",L,R),
  collectNTRefs(L,All,Rf,R0),
  collectCondRefs(R,All,R0,Refs).
collectNTRefs(T,All,Rf,Refs) :-
  isBinary(T,"=",L,R),
  collectExpRefs(L,All,Rf,R0),
  collectExpRefs(R,All,R0,Refs).
collectNTRefs(T,All,Rf,Refs) :-
  isBinary(T,"\\=",L,R),
  collectExpRefs(L,All,Rf,R0),
  collectExpRefs(R,All,R0,Refs).
collectNTRefs(T,All,Rf,Refs) :-
  isRoundTerm(T,_,O,A),
  collectExpRefs(O,All,Rf,R0),
  collectExpListRefs(A,All,R0,Refs).
collectNTRefs(T,_,Rf,Rf) :-
  isString(T,_).
collectNTRefs(T,_,Rf,Rf) :-
  isIden(T,_,"eof").

collectNTRefList([],_,R,R).
collectNTRefList([C|L],All,R,Refs) :-
  collectNTRefs(C,All,R,R0),
  collectNTRefList(L,All,R0,Refs).

collectClassRefs(Defs,All,SoFar,Refs) :-
  locallyDefined(Defs,All,Rest),
  collectStmtRefs(Defs,Rest,[],SoFar,Refs).

collectAnnotRefs(H,All,Annots,SoFar,Refs) :-
  headName(H,Nm),
  is_member((Nm,Annot),Annots),!,
  isBinary(Annot,":",_,Tp),
  collectTypeRefs(Tp,All,SoFar,Refs).
collectAnnotRefs(_,_,_,Refs,Refs).

collConstraints(C,All,SoFar,Refs) :-
  isBinary(C,",",L,R),
  collConstraints(L,All,SoFar,R0),
  collConstraints(R,All,R0,Refs).
collConstraints(C,All,[con(Nm)|Refs],Refs) :-
  isSquare(C,_,Nm,_),
  is_member(con(Nm),All).
collConstraints(_,_,Refs,Refs).

locallyDefined([],All,All).
locallyDefined([St|Stmts],All,Rest) :-
  removeLocalDef(St,All,A0),
  locallyDefined(Stmts,A0,Rest).

removeLocalDef(St,All,Rest) :-
  ruleName(St,Nm,_),
  subtract(Nm,All,Rest).
removeLocalDef(_,All,All).

collectCondRefs(C,A,R0,Refs) :-
  isBinary(C,",",L,R),
  collectCondRefs(L,A,R0,R1),
  collectCondRefs(R,A,R1,Refs).
collectCondRefs(C,A,R0,Refs) :-
  isBinary(C,"|",L,R),
  isBinary(L,"?",T,Th),
  collectCondRefs(T,A,R0,R1),
  collectCondRefs(Th,A,R1,R2),
  collectCondRefs(R,A,R2,Refs).
collectCondRefs(C,A,R0,Refs) :-
  isBinary(C,"|",L,R),
  collectCondRefs(L,A,R0,R1),
  collectCondRefs(R,A,R1,Refs).
collectCondRefs(C,A,R0,Refs) :-
  isBinary(C,"*>",L,R),
  collectCondRefs(L,A,R0,R1),
  collectCondRefs(R,A,R1,Refs).
collectCondRefs(C,A,R0,Refs) :-
  isUnary(C,"!",R),
  collectCondRefs(R,A,R0,Refs).
collectCondRefs(C,A,R0,Refs) :-
  isUnary(C,"\\+",R),
  collectCondRefs(R,A,R0,Refs).
collectCondRefs(C,A,R0,Refs) :-
  isBinary(C,"%%",L,R),
  isBinary(R,"~",St,Re),
  collectNTRefs(L,A,R0,R1),
  collectExpRefs(St,A,R1,R2),
  collectExpRefs(Re,A,R2,Refs).
collectCondRefs(C,A,R0,Refs) :-
  isBinary(C,"%%",L,R),
  collectNTRefs(L,A,R0,R1),
  collectExpRefs(R,A,R1,Refs).
collectCondRefs(C,A,R0,Refs) :-
  isTuple(C,[Inner]),
  collectCondRefs(Inner,A,R0,Refs).
collectCondRefs(C,A,R0,Refs) :-
  collectExpRefs(C,A,R0,Refs).

collectCondListRefs([],_,R,R).
collectCondListRefs([C|L],All,R,Refs) :-
  collectCondRefs(C,All,R,R0),
  collectCondListRefs(L,All,R0,Refs).

collectExpRefs(E,A,R0,Refs) :-
  isBinary(E,":",L,R),
  collectExpRefs(L,A,R0,R1),
  collectTypeRefs(R,A,R1,Refs).
collectExpRefs(V,A,[var(Nm)|Refs],Refs) :-
  isName(V,Nm),
  is_member(var(Nm),A).
collectExpRefs(T,A,R0,Refs) :-
  isRoundTerm(T,O,Args),
  collectExpRefs(O,A,R0,R1),
  collectExpListRefs(Args,A,R1,Refs).
collectExpRefs(T,A,R0,Refs) :-
  isTuple(T,Els),
  collectExpListRefs(Els,A,R0,Refs).
collectExpRefs(T,A,R0,Refs) :-
  isSquareTuple(T,Lc,Els),
  collectExpRefs(name(Lc,"[]"),A,R0,R1),
  collectExpRefs(name(Lc,",.."),A,R1,R2), % special handling for list notation
  collectExpListRefs(Els,A,R2,Refs).
collectExpRefs(app(_,Op,Args),All,R,Refs) :-
  collectExpRefs(Op,All,R,R0),
  collectExpRefs(Args,All,R0,Refs).
collectExpRefs(T,All,R,Refs) :-
  isBraceTuple(T,_,Els),
  collectClassRefs(Els,All,R,Refs).
collectExpRefs(T,All,R,Refs) :-
  isSquareTerm(T,Op,A),
  collectExpRefs(Op,All,R,R0),
  collectIndexRefs(A,All,R0,Refs).
collectExpRefs(T,All,R0,Refs) :-
  isBinary(T,"=>",L,R),
  collectExpRefs(L,All,R0,R1),
  collectExpRefs(R,All,R1,Refs).
collectExpRefs(T,All,R0,Refs) :-
  isBinary(T,":-",L,R),
  collectExpRefs(L,All,R0,R1),
  collectCondRefs(R,All,R1,Refs).
collectExpRefs(T,All,R0,Refs) :-
  isBinary(T,"-->",L,R),
  collectExpRefs(L,All,R0,R1),
  collectNTRefs(R,All,R1,Refs).
collectExpRefs(_,_,Refs,Refs).

collectExpListRefs([],_,Refs,Refs).
collectExpListRefs([E|L],A,R0,Refs) :-
  collectExpRefs(E,A,R0,R1),
  collectExpListRefs(L,A,R1,Refs).

collectIndexRefs([A],All,R,Refs) :-
  isBinary(A,_,"->",Ky,Vl),!,
  collectExpRefs(Ky,All,R,R0),
  collectExpRefs(Vl,All,R0,Refs).
collectIndexRefs([A],All,R,Refs) :-
  isUnary(A,_,"\\+",Ky),!,
  collectExpRefs(Ky,All,R,Refs).
collectIndexRefs(A,All,R,Refs) :-
  collectExpListRefs(A,All,R,Refs).

collectPtnListRefs([],_,Refs,Refs).
collectPtnListRefs([E|L],A,R0,Refs) :-
  collectPtnRefs(E,A,R0,R1),
  collectPtnListRefs(L,A,R1,Refs).

collectPtnRefs(P,A,R0,Refs) :-
  isBinary(P,"@@",L,R),
  collectPtnRefs(L,A,R0,R1),
  collectCondRefs(R,A,R1,Refs).
collectPtnRefs(P,A,R0,Refs) :-
  isBinary(P,":",L,R),
  collectPtnRefs(L,A,R0,R1),
  collectTypeRefs(R,A,R1,Refs).
collectPtnRefs(V,A,[var(Nm)|Refs],Refs) :-
  isIden(V,Nm),
  is_member(var(Nm),A).
collectPtnRefs(app(_,Op,Args),All,R0,Refs) :-
  collectExpRefs(Op,All,R0,R1),
  collectPtnRefs(Args,All,R1,Refs).
collectPtnRefs(tuple(_,_,Args),All,R0,Refs) :-
  collectPtnListRefs(Args,All,R0,Refs).
collectPtnRefs(_,_,Refs,Refs).

collectTypeRefs(V,All,SoFar,Refs) :-
  isIden(V,Nm),
  collectTypeName(Nm,All,SoFar,Refs).
collectTypeRefs(T,All,SoFar,Refs) :-
  isSquare(T,Nm,A),
  collectTypeName(Nm,All,SoFar,R0),
  collectTypeList(A,All,R0,Refs).
collectTypeRefs(St,_,SoFar,SoFar) :-
  isBinary(St,"@",_,_).
collectTypeRefs(St,_,SoFar,SoFar) :-
  isUnary(St,"@",_,_).
collectTypeRefs(T,All,SoFar,Refs) :-
  isBinary(T,"=>",L,R),
  isTuple(L,A),
  collectArgList(A,All,SoFar,R0),
  collectTypeRefs(R,All,R0,Refs).
collectTypeRefs(T,All,SoFar,Refs) :-
  isBinary(T,"<=>",L,R),
  isTuple(L,A),
  collectArgList(A,All,SoFar,R0),
  collectTypeRefs(R,All,R0,Refs).
collectTypeRefs(T,All,SoFar,Refs) :-
  isBinary(T,"-->",L,R),
  isTuple(L,A),
  collectArgList(A,All,SoFar,R0),
  collectTypeRefs(R,All,R0,Refs).
collectTypeRefs(T,All,SoFar,Rest) :-
  isBinary(T,"|:",L,R),
  collConstraints(L,All,SoFar,R0),
  collectTypeRefs(R,All,R0,Rest).
collectTypeRefs(T,All,SoFar,Rest) :-
  isBinary(T,"->>",L,R),
  collectTypeRefs(L,All,SoFar,R0),
  collectTypeRefs(R,All,R0,Rest).
collectTypeRefs(C,All,SoFar,Refs) :-
  isBinary(C,",",L,R),
  collectTypeRefs(L,All,SoFar,R0),
  collectTypeRefs(R,All,R0,Refs).
collectTypeRefs(T,All,SoFar,Refs) :-
  isBraceTerm(T,_,L,[]),
  isTuple(L,A),
  collectArgList(A,All,SoFar,Refs).
collectTypeRefs(T,All,SoFar,Refs) :-
  isBraceTuple(T,_,A),
  collectFaceTypes(A,All,SoFar,Refs).
collectTypeRefs(T,All,SoFar,Refs) :-
  isQuantified(T,_,A),
  collectTypeRefs(A,All,SoFar,Refs).
collectTypeRefs(T,All,SoFar,Refs) :-
  isTuple(T,Args),
  collectTypeList(Args,All,SoFar,Refs).

collectTypeList([],_,Refs,Refs).
collectTypeList([Tp|List],All,SoFar,Refs) :-
  collectTypeRefs(Tp,All,SoFar,R0),
  collectTypeList(List,All,R0,Refs).

collectArgList([],_,Refs,Refs).
collectArgList([Tp|List],All,SoFar,Refs) :-
  collectArgRefs(Tp,All,SoFar,R0),
  collectArgList(List,All,R0,Refs).

collectArgRefs(T,All,SoFar,Refs) :-
  isUnary(T,"^",I),
  collectArgRefs(I,All,SoFar,Refs).
collectArgRefs(T,All,SoFar,Refs) :-
  isUnary(T,"?",I),
  collectArgRefs(I,All,SoFar,Refs).
collectArgRefs(T,All,SoFar,Refs) :-
  isUnary(T,"^?",I),
  collectArgRefs(I,All,SoFar,Refs).
collectArgRefs(T,All,SoFar,Refs) :-
  isUnary(T,"?^",I),
  collectArgRefs(I,All,SoFar,Refs).
collectArgRefs(T,All,SoFar,Refs) :-
  collectTypeRefs(T,All,SoFar,Refs).

collectFaceTypes([],_,Refs,Refs).
collectFaceTypes([T|List],All,SoFar,Refs) :-
  isBinary(T,":",_,Tp),
  collectTypeRefs(Tp,All,SoFar,R0),
  collectFaceTypes(List,All,R0,Refs).

collectTypeName(Nm,All,[tpe(Nm)|SoFar],SoFar) :-
  is_member(tpe(Nm),All),!.
collectTypeName(_,_,Refs,Refs).

collectContractRefs([],_,R,R).
collectContractRefs([C|L],All,R,Rx) :-
  collConstraints(C,All,R,R1),
  collectContractRefs(L,All,R1,Rx).

collectLabelRefs(Lb,All,R0,Refs) :- collectExpRefs(Lb,All,R0,Refs).

showGroups([]).
showGroups([G|M]) :-
  reportMsg("Group:",[]),
  showGroup(G),
  showGroups(M).
showGroup([]).
showGroup([(Def,Lc,_)|M]) :-
  reportMsg("Def %s",[Def],Lc),
  showGroup(M).

showRefs(Msg,Refs) :-
  reportMsg("%s references: %s",[Msg,Refs]).
