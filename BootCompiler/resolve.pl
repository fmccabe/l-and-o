:- module(resolve,[overload/4,overloadOthers/3]).

:- use_module(misc).
:- use_module(errors).
:- use_module(types).
:- use_module(freshen).
:- use_module(canon).

overload(Defs,Dict,RDict,RDefs) :-
  declareImplementations(Defs,Dict,RDict),
  overloadDefs(Defs,RDict,RDefs).

overloadDefs(D,Dict,RD) :-
  overloadList(D,overloadDef,Dict,RD).

overloadDef(function(Lc,Nm,Tp,Cx,Eqns),Dict,RF) :-
  overloadFunction(Lc,Nm,Tp,Cx,Eqns,Dict,RF).
overloadDef(predicate(Lc,Nm,Tp,Cx,Cls),Dict,RP) :-
  overloadPredicate(Lc,Nm,Tp,Cx,Cls,Dict,RP).
overloadDef(D,Dict,RD) :-
  D = defn(_,_,_,_,_,_),
  overloadDefn(D,Dict,RD).
overloadDef(E,Dict,RE) :-
  E = enum(_,_,_,_,_,_),
  overloadEnum(E,Dict,RE).
overloadDef(class(Lc,Nm,Tp,Cx,Rules,Face),Dict,RC) :-
  overloadClass(Lc,Nm,Tp,Cx,Rules,Face,Dict,RC).
overloadDef(grammar(Lc,Nm,Tp,Cx,Rules),Dict,RG) :-
  overloadGrammar(Lc,Nm,Tp,Cx,Rules,Dict,RG).
overloadDef(T,_,T) :-
  T = typeDef(_,_,_,_).
overloadDef(C,_,C) :-
  C = contract(_,_,_,_).
overloadDef(implementation(Lc,INm,ImplName,Spec,OCx,AC,ThDefs,BodyDefs,Types,Others),Dict,RI) :-
  overloadImplementation(Lc,INm,ImplName,Spec,OCx,AC,ThDefs,BodyDefs,Types,Others,Dict,RI).

overloadFunction(Lc,Nm,Tp,[],Eqns,Dict,function(Lc,Nm,Tp,[],REqns)) :-
  overloadEquations(Eqns,Dict,[],REqns).
overloadFunction(Lc,Nm,Tp,Cx,Eqns,Dict,function(Lc,Nm,Tp,[],REqns)) :-
  defineCVars(Lc,Cx,Dict,CVars,FDict),
  overloadEquations(Eqns,FDict,CVars,REqns).

overloadEquations(Eqns,Dict,Extra,REqns) :-
  overloadList(Eqns,overloadEquation(Extra),Dict,REqns).

overloadEquation(Extra,equation(Lc,Nm,Args,Cond,Exp),Dict,equation(Lc,Nm,RArgs,RCond,RExp)) :-
  resolveTerms(Args,Dict,RA),
  concat(Extra,RA,RArgs),
  resolveCond(Cond,Dict,RCond),
  resolveTerm(Exp,Dict,RExp).

overloadPredicate(Lc,Nm,Tp,[],Cls,Dict,predicate(Lc,Nm,Tp,[],RCls)) :-
  overloadClauses(Cls,Dict,[],RCls).
overloadPredicate(Lc,Nm,Tp,Cx,Cls,Dict,predicate(Lc,Nm,Tp,[],RCls)) :-
  defineCVars(Lc,Cx,Dict,CVars,FDict),
  overloadClauses(Cls,FDict,CVars,RCls).

overloadClauses(Eqns,Dict,REqns) :-
  overloadList(Eqns,overloadClause,Dict,REqns).

overloadClause(clause(Lc,Nm,Args,Cond,Body),Dict,Extra,clause(Lc,Nm,RArgs,RCond,RBody)) :-
  resolveTerms(Args,Dict,RA),
  concat(Extra,RA,RArgs),
  resolveCond(Cond,Dict,RCond),
  resolveCond(Body,Dict,RBody).
overloadClause(strong(Lc,Nm,Args,Cond,Body),Dict,Extra,strong(Lc,Nm,RArgs,RCond,RBody)) :-
  resolveTerms(Args,Dict,RA),
  concat(Extra,RA,RArgs),
  resolveCond(Cond,Dict,RCond),
  resolveCond(Body,Dict,RBody).

overloadGrammar(Lc,Nm,Tp,[],Rules,Dict,grammar(Lc,Nm,Tp,[],RRules)) :-
  overloadGrRules(Rules,Dict,[],RRules).
overloadGrammar(Lc,Nm,Tp,Cx,Rules,Dict,grammar(Lc,Nm,Tp,[],RRules)) :-
  defineCVars(Lc,Cx,Dict,CVars,FDict),
  overloadGrRules(Rules,FDict,CVars,RRules).

overloadGrRules(Rules,Dict,RRules) :-
  overloadList(Rules,overloadGrRule,Dict,RRules).

overloadGrRule(grammarRule(Lc,Nm,Args,PB,Body),Dict,Extra,grammarRule(Lc,Nm,RArgs,RPB,RBody)) :-
  resolveTerms(Args,Dict,RA),
  concat(Extra,RA,RArgs),
  resolveGr(PB,Dict,RPB),
  resolveGr(Body,Dict,RBody).

defineCVars(_,[],Dict,[],Dict).
defineCVars(Lc,[Con|Cx],Dict,[NV|CVars],FDict) :-
  implementationName(Con,ImplNm),
  genVar(ImplNm,Lc,NV),
  defineCVars(Lc,Cx,[(ImplNm,NV)|Dict],CVars,FDict).

resolveTerm(v(Lc,Nm),_,v(Lc,Nm)).
resolveTerm(intLit(Ix),_,intLit(Ix)).
resolveTerm(floatLit(Ix),_,floatLit(Ix)).
resolveTerm(stringLit(Sx),_,stringLit(Sx)).
resolveTerm(pkgRef(Lc,Pkg,Rf),_,pkgRef(Lc,Pkg,Rf)).
resolveTerm(dot(Rc,Fld),Dict,dot(RRc,Fld)) :- resolveTerm(Rc,Dict,RRc).
resolveTerm(enum(Lc,Rf),_,enum(Lc,Rf)).
resolveTerm(tuple(Lc,Args),Dict,tuple(Lc,RArgs)) :-
  resolveTerms(Args,Dict,RArgs).
resolveTerm(dict(Lc,Entries),Dict,dict(Lc,REntries)) :-
  resolveEntries(Entries,Dict,REntries).
resolveTerm(where(Trm,Cond),Dict,where(RTrm,RCond)) :-
  resolveTerm(Trm,Dict,RTrm),
  resolveCond(Cond,Dict,RCond).
resolveTerm(conditional(Cond,Then,Else),Dict,conditional(RCond,RThen,RElse)) :-
  resolveCond(Cond,Dict,RCond),
  resolveTerm(Then,Dict,RThen),
  resolveTerm(Else,Dict,RElse).
resolveTerm(apply(over(Lc,T,Cx),Args),Dict,apply(OverOp,NArgs)) :-
  resolveTerms(Args,Dict,RArgs),
  resolveContracts(Lc,Cx,Dict,DTerms),
  overloadRef(Lc,T,DTerms,RArgs,OverOp,NArgs).
resolveTerm(apply(Op,Args),Dict,apply(ROp,RArgs)) :-
  resolveTerm(Op,Dict,ROp),
  resolveTerms(Args,Dict,RArgs).
resolveTerm(over(Lc,T,Cx),Dict,apply(OverOp,NArgs)) :-
  resolveContracts(Lc,Cx,Dict,DTerms),
  overloadRef(Lc,T,DTerms,[],OverOp,NArgs).
resolveTerm(mtd(Lc,Nm),_,v(Lc,Nm)) :-
  reportError("cannot find implementation for %s",[Nm],Lc).

overloadList([],_,_,[]):-!.
overloadList([T|L],C,D,[RT|RL]) :-
  call(C,T,D,RT),
  overloadList(L,C,D,RL).

resolveTerms(L,D,RL) :-
  overloadList(L,resolve:resolveTerm,D,RL).

resolveCond(true(Lc),_,true(Lc)).
resolveCond(false(Lc),_,false(Lc)).
resolveCond(conj(L,R),Dict,conj(RL,RR)) :-
  resolveCond(L,Dict,RL),
  resolveCond(R,Dict,RR).
resolveCond(disj(Lc,L,R),Dict,disj(Lc,RL,RR)) :-
  resolveCond(L,Dict,RL),
  resolveCond(R,Dict,RR).
resolveCond(forall(Lc,L,R),Dict,forall(Lc,RL,RR)) :-
  resolveCond(L,Dict,RL),
  resolveCond(R,Dict,RR).
resolveCond(conditional(Lc,T,L,R),Dict,conditional(Lc,RT,RL,RR)) :-
  resolveCond(T,Dict,RT),
  resolveCond(L,Dict,RL),
  resolveCond(R,Dict,RR).
resolveCond(one(Lc,T),Dict,one(Lc,RT)) :-
  resolveCond(T,Dict,RT).
resolveCond(neg(Lc,T),Dict,neg(Lc,RT)) :-
  resolveCond(T,Dict,RT).
resolveCond(match(Lc,L,R),Dict,match(Lc,RL,RR)) :-
  resolveTerm(L,Dict,RL),
  resolveTerm(R,Dict,RR).
resolveCond(unify(Lc,L,R),Dict,unify(Lc,RL,RR)) :-
  resolveTerm(L,Dict,RL),
  resolveTerm(R,Dict,RR).
resolveCond(phrase(Lc,T,S,R),Dict,phrase(Lc,RT,RS,RR)) :-
  resolveGr(T,Dict,RT),
  resolveTerm(S,Dict,RS),
  resolveTerm(R,Dict,RR).
resolveCond(phrase(Lc,T,S),Dict,phrase(Lc,RT,RS)) :-
  resolveGr(T,Dict,RT),
  resolveTerm(S,Dict,RS).
resolveCond(show(Lc,E),Dict,show(Lc,RE)) :-
  resolveTerm(E,Dict,RE).
resolveCond(call(Lc,Op,Args),Dict,call(Lc,ROp,RArgs)) :-
  resolveTerm(Op,Dict,ROp),
  resolveTerms(Args,Dict,RArgs).
resolveCond(over(Lc,T,Cx),Dict,apply(OverOp,NArgs)) :-
  resolveContracts(Lc,Cx,Dict,DTerms),
  overloadRef(Lc,T,DTerms,[],OverOp,NArgs).
resolveCond(call(Lc,P,Args),Dict,call(Lc,RP,RArgs)) :-
  resolveTerm(P,Dict,RP),
  resolveTerms(Args,Dict,RArgs).

resolveGr(stringLit(St),_,stringLit(St)).
resolveGr(terminals(Lc,Terms),Dict,terminals(Lc,RTerms)) :-
  resolveTerms(Terms,Dict,RTerms).
resolveGr(eof(Lc),_,eof(Lc)).
resolveGr(conj(Lc,L,R),Dict,conj(Lc,RL,RR)) :-
  resolveGr(L,Dict,RL),
  resolveGr(R,Dict,RR).
resolveGr(disj(Lc,L,R),Dict,disj(Lc,RL,RR)) :-
  resolveGr(L,Dict,RL),
  resolveGr(R,Dict,RR).
resolveGr(conditional(Lc,T,L,R),Dict,conditional(Lc,RT,RL,RR)) :-
  resolveCond(T,Dict,RT),
  resolveGr(L,Dict,RL),
  resolveGr(R,Dict,RR).
resolveGr(one(Lc,L),Dict,one(Lc,RL)) :-
  resolveGr(L,Dict,RL).
resolveGr(neg(Lc,L),Dict,neg(Lc,RL)) :-
  resolveGr(L,Dict,RL).
resolveGr(ahead(Lc,L),Dict,ahead(Lc,RL)) :-
  resolveGr(L,Dict,RL).
resolveGr(guard(Lc,L,T),Dict,guard(Lc,RL,RT)) :-
  resolveCond(T,Dict,RT),
  resolveGr(L,Dict,RL).
resolveGr(goal(Lc,T),Dict,goal(Lc,RT)) :-
  resolveCond(T,Dict,RT).
resolveGr(dip(Lc,V,T),Dict,dip(Lc,RV,RT)) :-
  resolveTerm(V,Dict,RV),
  resolveCond(T,Dict,RT).
resolveGr(call(Lc,Op,Args),Dict,call(Lc,ROp,RArgs)) :-
  resolveTerm(Op,Dict,ROp),
  resolveTerms(Args,Dict,RArgs).
resolveGr(over(Lc,T,Cx),Dict,call(OverOp,NArgs)) :-
  resolveContracts(Lc,Cx,Dict,DTerms),
  overloadRef(Lc,T,DTerms,[],OverOp,NArgs).
resolveGr(call(Lc,P,Args),Dict,call(Lc,RP,RArgs)) :-
  resolveTerm(P,Dict,RP),
  resolveTerms(Args,Dict,RArgs).

overloadRef(_,mtd(_,Nm),[DT],RArgs,dot(DT,Nm),RArgs).
overloadRef(_,v(Lc,Nm),DT,RArgs,v(Lc,Nm),Args) :- concat(DT,RArgs,Args).

resolveContracts(_,[],_,[]).
resolveContracts(Lc,[Con|C],Dict,[CV|Vs]) :-
  resolveContract(Lc,Con,Dict,CV),
  resolveContracts(Lc,C,Dict,Vs).

resolveContract(Lc,C,Dict,Over) :-
  implementationName(C,ImpNm),
  findImplementation(ImpNm,Dict,Impl),!,
  resolve(Impl,Lc,Dict,Over).
resolveContract(Lc,C,_,v(Lc,ImpNm)) :-
  implementationName(C,ImpNm),
  reportError("no implementation known for %s",[C],Lc).

resolve(v(Lc,Nm),_,_,v(Lc,Nm)) :-!.
resolve(I,Lc,Dict,Over) :-
  freshenContract(I,_,Con),
  resolveDependents(Con,Dict,ImplNm,[],Args),
  formOver(v(Lc,ImplNm),Args,Over).

resolveDependents(constrained(C,O),Dict,ImplNm,As,Args) :-
  resolveContract(O,Dict,A),
  resolveDependents(C,Dict,ImplNm,[A|As],Args).
resolveDependents(C,_,ImplNm,Args,Args) :-
  implementationName(C,ImplNm).

formOver(V,[],V).
formOver(V,Args,apply(V,Args)).

genVar(Nm,Lc,v(Lc,NV)) :-
  genstr(Nm,NV).

declareImplementations([],Dict,Dict).
declareImplementations([implementation(_,_,ImplName,Spec,_,_,_,_,_,_)|Defs],Dict,RDict) :-
  declareImplementations(Defs,[(ImplName,Spec)|Dict],RDict).
declareImplementations([_|Defs],Dict,RDict) :-
  declareImplementations(Defs,Dict,RDict).

findImplementation(ImplName,Dict,Spec) :-
  is_member((ImplName,Spec),Dict).

overloadImplementation(Lc,INm,ImplName,Spec,OCx,AC,ThDefs,Face,Types,Others,Dict,
    impl(Lc,INm,ImplName,Arity,Spec,OCx,ImplBody,faceType(Face))) :-
  defineCVars(Lc,AC,Dict,CVars,FDict),
  overload(ThDefs,FDict,FODict,RThDefs),
  overloadOthers(Others,FODict,ROthers),
  length(CVars,Arity),
  (CVars=[] -> Hd = enum(Lc,ImplName) ; Hd = apply(v(Lc,ImplName),CVars)),
  ImplBody = implBody(Lc,Hd,RThDefs,ROthers,Types).

overloadOthers(Other,Dict,OOthers) :-
  overloadList(Other,resolve:overloadOther,Dict,OOthers).

overloadOther(show(Lc,Show),Dict,show(Lc,RShow)) :-
  resolveTerm(Show,Dict,RShow).
overloadOther(assertion(Lc,Cond),Dict,assertion(Lc,RCond)) :-
  resolveCond(Cond,Dict,RCond).

