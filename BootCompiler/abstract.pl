:- module(abstract,[locOfAst/2,isAst/1,
      nary/4,binary/5,unary/4,zeroary/3,apply/4,isApply/3,
      isUnary/3,isUnary/4,isBinary/4,isBinary/5,isBinaryTerm/4,
      isTernary/5,ternary/6,
      roundTerm/4,isRound/3,isRoundTerm/3,isRoundTerm/4,isTuple/2,isTuple/3,isRoundTuple/3,
      braceTerm/4,isBrace/3,isBraceTerm/4,isBraceTuple/3,isQBraceTuple/3,
      squareTerm/4,isSquare/3,isSquare/4,isSquareTuple/3,isSquareTuple/2,isSquareTerm/3,isSquareTerm/4,
      isName/2,isIden/1,isIden/2,isIden/3,genIden/2,isString/2,isInteger/2,
      sameTerm/2]).
:- use_module(operators).
:- use_module(keywords).
:- use_module(misc).

apply(Lc,Op,Args,app(Lc,Op,Args)).

isApply(app(_,Op,Args),Nm,Args) :- isIden(Op,Nm).

isTuple(tuple(_,"()",Args),Args).

isTuple(tuple(Lc,"()",Args),Lc,Args).

isRoundTuple(tuple(Lc,"()",Args),Lc,Args).

roundTerm(Lc,Op,Args,app(Lc,name(Lc,Op),tuple(Lc,"()",Args))).

isRound(app(_,name(_,Op),tuple(_,"()",Args)),Op,Args).

isKeyOp(name(_,Op)) :- isKeyword(Op).

isRoundTerm(app(_,Op,tuple(_,"()",Args)),Op,Args) :- \+isKeyOp(Op).

isRoundTerm(app(Lc,Op,tuple(_,"()",Args)),Lc,Op,Args) :- \+isKeyOp(Op).

binary(Lc,Op,L,R,app(Lc,name(Lc,Op),tuple(Lc,"()",[L,R]))).

isBinary(app(_,name(_,Op),tuple(_,"()",[L,R])),Op,L,R).

isBinary(app(Lc,name(_,Op),tuple(_,"()",[L,R])),Lc,Op,L,R).

isBinaryTerm(app(_,Op,tuple(_,"()",[L,R])),Op,L,R).

zeroary(Lc,Op,app(Lc,name(Lc,Op),tuple(Lc,"()",[]))).

unary(Lc,Op,L,app(Lc,name(Lc,Op),tuple(Lc,"()",[L]))).

isUnary(app(_,name(_,Op),tuple(_,"()",[L])),Op,L).

isUnary(app(Lc,name(_,Op),tuple(_,"()",[L])),Lc,Op,L).

isTernary(app(_,name(_,Op),tuple(_,"()",[L,M,R])),Op,L,M,R).

ternary(Lc,Op,L,M,R,app(Lc,name(Lc,Op),tuple(Lc,"()",[L,M,R]))).

nary(Lc,Op,Args,app(Lc,name(Lc,Op),tuple(Lc,"()",Args))).

braceTerm(Lc,Op,Els,app(Lc,Op,tuple(Lc,"{}",Els))).

isBrace(app(_,name(_,Op),tuple(_,"{}",L)),Op,L).

isBraceTerm(app(Lc,Op,tuple(_,"{}",A)),Lc,Op,A).

isBraceTuple(tuple(Lc,"{}",L),Lc,L).

isQBraceTuple(tuple(Lc,"{..}",L),Lc,L).

squareTerm(Lc,Op,Els,app(Lc,name(Lc,Op),tuple(Lc,"[]",Els))).

isSquare(app(_,name(_,Op),tuple(_,"[]",L)),Op,L).

isSquare(app(Lc,name(_,Op),tuple(_,"[]",L)),Lc,Op,L).

isSquareTerm(app(_,Op,tuple(_,"[]",L)),Op,L) :- \+isKeyOp(Op).

isSquareTerm(app(Lc,Op,tuple(_,"[]",L)),Lc,Op,L) :- \+isKeyOp(Op).

isSquareTuple(tuple(_,"[]",A),A).

isSquareTuple(tuple(Lc,"[]",L),Lc,L).

isName(name(_,Nm),Nm).

isIden(N) :- isIden(N,_).

isIden(name(_,Nm),Nm).
isIden(tuple(_,"()",[name(_,Nm)]),Nm).

isIden(name(Lc,Nm),Lc,Nm).
isIden(tuple(Lc,"()",[name(_,Nm)]),Lc,Nm).

genIden(Lc,name(Lc,Id)) :-
  genstr("N",Id).

isString(string(_,St),St).

isInteger(integer(_,Ix),Ix).

isAst(A) :- locOfAst(A,_).

locOfAst(name(Lc,_),Lc).
locOfAst(integer(Lc,_),Lc).
locOfAst(float(Lc,_),Lc).
locOfAst(string(Lc,_),Lc).
locOfAst(tuple(Lc,_,_),Lc).
locOfAst(app(Lc,_,_),Lc).

sameTerm(name(_,Nm),name(_,Nm)).
sameTerm(integer(_,Ix),integer(_,Ix)).
sameTerm(float(_,Dx),float(_,Dx)).
sameTerm(string(_,S),string(_,S)).
sameTerm(tuple(_,T,A),tuple(_,T,B)) :-
  sameTerms(A,B).
sameTerm(app(_,OA,AA),app(_,OB,BA)) :-
  sameTerm(OA,OB),
  sameTerm(AA,BA).

sameTerms([],[]).
sameTerms([A|L1],[B|L2]) :-
  sameTerm(A,B),
  sameTerms(L1,L2).
