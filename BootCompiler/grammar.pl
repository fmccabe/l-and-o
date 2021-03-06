:- module(grammar,[parse/3]).

:- use_module(operators).
:- use_module(abstract).
:- use_module(location).
:- use_module(errors).
:- use_module(lexer).
:- use_module(wff).

parse(Tks,T,RTks) :-
    term(Tks,2000,T,Tks1,Lst),!,
    checkTerminator(Lst,Tks1,RTks).

term(Tks,Pr,T,Toks,Lst) :-
    termLeft(Tks,Pr,Left,LftPr,Tks1,LLst),!,
    termRight(Tks1,Pr,LftPr,Left,T,Toks,LLst,Lst).

termLeft([idTok(Id,Lc),rpar(Lcy)|Toks],_,name(Lc,Id),0,[rpar(Lcy)|Toks],id).
termLeft([idTok(Id,Lcx)|Tks],Pr,Left,LftPr,Toks,Lst) :-
    prefixOp(Id,LftPr,OpRightPr),
    \+ lookAhead(rpar(_),Tks),
    LftPr =< Pr, !,
    term(Tks,OpRightPr,Arg,Toks,Lst),
    locOfAst(Arg,Lcy),
    mergeLoc(Lcx,Lcy,Lc),
    unary(Lc,Id,Arg,Left).
termLeft(Tks,_,T,0,Toks,Lst) :- term0(Tks,T,Toks,Lst).

termRight([idTok(Id,_)|Tks],Pr,LPr,Left,T,Toks,_,Lst) :-
  infixOp(Id,InfOpL,InfOpPr,InfOpR), InfOpPr =< Pr, LPr =< InfOpL,
  postfixOp(Id,PostOpL,PostOpPr), PostOpPr =< Pr, LPr =< PostOpL,
  legalNextRight(Tks,InfOpR), !,
  term(Tks,InfOpR, Right, Tks1,LLst),
  locOfAst(Left,Lcx),
  locOfAst(Right,Lcy),
  mergeLoc(Lcx,Lcy,Lc),
  binary(Lc,Id,Left,Right,NewLeft),
  termRight(Tks1,Pr,InfOpPr,NewLeft,T,Toks,LLst,Lst).
termRight([idTok(Id,Lcy)|Tks],Pr,LPr,Left,T,Toks,_,Lst) :-
  postfixOp(Id,PostOpL,PostOpPr), PostOpPr =< Pr, LPr =< PostOpL,
  \+legalNextRight(Tks,PostOpPr),!,
  locOfAst(Left,Lcx),
  mergeLoc(Lcx,Lcy,Lc),
  unary(Lc,Id,Left,NewLeft),
  termRight(Tks,Pr,PostOpPr,NewLeft,T,Toks,id,Lst).
termRight([idTok(Id,_)|Tks],Pr,LPr,Left,T,Toks,_,Lst) :-
  infixOp(Id,InfOpL,InfOpPr,InfOpR), InfOpPr =< Pr, LPr =< InfOpL,
  term(Tks,InfOpR, Right, Tks1,LLst),
  locOfAst(Left,Lcx),
  locOfAst(Right,Lcy),
  mergeLoc(Lcx,Lcy,Lc),
  binary(Lc,Id,Left,Right,NewLeft),
  termRight(Tks1,Pr,InfOpPr,NewLeft,T,Toks,LLst,Lst).
termRight(Toks,_,_,Left,Left,Toks,Lst,Lst).

legalNextRight([idTok(I,_)|_],Pr) :- ( prefixOp(I,PPr,_), PPr=<Pr ; \+ isOperator(I,_)) , !.
legalNextRight([idQTok(_,_)|_],_).
legalNextRight([lpar(_)|_],_).
legalNextRight([lbra(_)|_],_).
legalNextRight([lbrce(_)|_],_).
legalNextRight([lqbrce(_)|_],_).
legalNextRight([lqpar(_)|_],_).
legalNextRight([stringTok(_,_)|_],_).
legalNextRight([integerTok(_,_)|_],_).
legalNextRight([floatTok(_,_)|_],_).

term0([stringTok(St,Lc)|Toks],Str,Toks,id) :-
  handleInterpolation(St,Lc,Str).
term0([integerTok(In,Lc)|Toks],integer(Lc,In),Toks,id).
term0([floatTok(Fl,Lc)|Toks],float(Lc,Fl),Toks,id).
term0([lbrce(Lc0),rbrce(Lc2)|Toks],tuple(Lc,"{}",[]),Toks,rbrce) :-
  mergeLoc(Lc0,Lc2,Lc).
term0([lbrce(Lcx)|Tks],tuple(Lc,"{}",Seq),Toks,rbrce) :-
  terms(Tks,Tks2,Seq),
  checkToken(Tks2,Toks,rbrce(Lcy),Lcy,"missing close brace, got %s, left brace at %s",[Lcx]),
  mergeLoc(Lcx,Lcy,Lc).
term0([lqbrce(Lc0),rqbrce(Lc2)|Toks],tuple(Lc,"{..}",[]),Toks,rbrce) :-
  mergeLoc(Lc0,Lc2,Lc).
term0([lqbrce(Lcx)|Tks],tuple(Lc,"{..}",Seq),Toks,rbrce) :-
  terms(Tks,Tks2,Seq),
  checkToken(Tks2,Toks,rqbrce(Lcy),Lcy,"missing close brace, got %s, left brace at %s",[Lcx]),
  mergeLoc(Lcx,Lcy,Lc).
term0([lqpar(Lcx)|Tks],unary(Lc,"<||>",T),Toks,rpar) :-
  term(Tks,2000,T,Tks2,_),
  checkToken(Tks2,Toks,rpar(Lcy),Lcy,"missing close quote, got %s",[]),
  mergeLoc(Lcx,Lcy,Lc).
term0(Tks,T,Toks,Lst) :- term00(Tks,Op,RTks,LLst), termArgs(RTks,Op,T,Toks,LLst,Lst).

term00([idTok(I,Lc)|Toks],T,Toks,id) :-
      (isOperator(I,_), \+lookAhead(rpar(_),Toks), !, reportError("unexpected operator: '%s'",[I],Lc),T=void(Lc);
      T = name(Lc,I)).
term00([idQTok(I,Lc)|Toks],name(Lc,I),Toks,id).
term00([lpar(Lc0),rpar(Lc2)|Toks],tuple(Lc,"()",[]),Toks,rpar) :-
  mergeLoc(Lc0,Lc2,Lc).
term00([lpar(Lcx)|Tks],T,Toks,rpar) :-
  term(Tks,2000,Seq,Tks2,_),
  checkToken(Tks2,Toks,rpar(Lcy),Lcy,"missing close parenthesis, got %s, left paren at %s",[Lcx]),
  mergeLoc(Lcx,Lcy,Lc),
  tupleize(Seq,Lc,"()",T).
term00([lbra(Lc0),rbra(Lc2)|Toks],tuple(Lc,"[]",[]),Toks,rbra) :-
  mergeLoc(Lc0,Lc2,Lc).
term00([lbra(Lcx)|Tks],T,Toks,rbra) :-
  term(Tks,2000,Seq,Tks2,_),
  checkToken(Tks2,Toks,rbra(Lcy),Lcy,"mising close bracket, got %s, left bracket at %s",[Lcx]),
  mergeLoc(Lcx,Lcy,Lc),
  tupleize(Seq,Lc,"[]",T).

termArgs([],T,T,[],Lst,Lst).
termArgs([lpar(_),rpar(Lcy)|Tks],Op,T,Toks,_,Lst) :-
    locOfAst(Op,Lcx),
    mergeLoc(Lcx,Lcy,Lc),
    apply(Lc,Op,tuple(Lc,"()",[]),NOP),
    termArgs(Tks,NOP,T,Toks,rpar,Lst).
termArgs([lpar(_)|Tks],Op,T,Toks,_,Lst) :-
    locOfAst(Op,Lcx),
    term(Tks,2000,Seq,Tks2,LLst),
    checkToken(Tks2,Tks3,rpar(Lcy),Lcy,"missing close parenthesis, got %s, left paren at %s",[Lcx]),
    mergeLoc(Lcx,Lcy,Lc),
    tupleize(Seq,Lc,"()",Args),
    apply(Lc,Op,Args,NOP),
    termArgs(Tks3,NOP,T,Toks,LLst,Lst).
termArgs([lbra(_),rbra(Lcy)|Tks],Op,T,Toks,_,Lst) :-
    locOfAst(Op,Lcx),
    mergeLoc(Lcx,Lcy,Lc),
    apply(Lc,Op,tuple(Lc,"[]",[]),NOP),
    termArgs(Tks,NOP,T,Toks,rbra,Lst).
termArgs([lbra(_)|Tks],Op,T,Toks,_,Lst) :-
    locOfAst(Op,Lcx),
    term(Tks,2000,Seq,Tks2,_),
    checkToken(Tks2,Tks3,rbra(Lcy),Lcy,"missing close bracket, got %s, left bracket at %s",[Lcx]),
    mergeLoc(Lcx,Lcy,Lc),
    tupleize(Seq,Lc,"[]",Args),
    apply(Lc,Op,Args,NOP),
    termArgs(Tks3,NOP,T,Toks,rbra,Lst).
termArgs([lbrce(_),rbrce(Lcy)|Tks],Op,T,Tks,_,rbrce) :-
    locOfAst(Op,Lcx),
    mergeLoc(Lcx,Lcy,Lc),
    apply(Lc,Op,tuple(Lc,"{}",[]),T).
termArgs([lbrce(_)|Tks],Op,T,Toks,_,rbrce) :-
    locOfAst(Op,Lcx),
    terms(Tks,Tks2,Seq),
    checkToken(Tks2,Toks,rbrce(Lcy),Lcy,"missing close brace, got %s, left brace at %s",[Lcx]),
    mergeLoc(Lcx,Lcy,Lc),
    apply(Lc,Op,tuple(Lc,"{}",Seq),T).
termArgs([idTok(".",_),idTok(Fld,LcF)|Tks],Op,T,Toks,_,Lst) :-
    locOfAst(Op,Lcx),
    mergeLoc(Lcx,LcF,Lc),
    binary(Lc,".",Op,name(LcF,Fld),NOP),
    termArgs(Tks,NOP,T,Toks,id,Lst).
termArgs(Toks,T,T,Toks,Lst,Lst).

terms([],[],[]).
terms([rbrce(Lc)|Toks],[rbrce(Lc)|Toks],[]) :- !.
terms(Tks,Toks,[T|R]) :-
    parse(Tks,T,Tks2),
    terms(Tks2,Toks,R).

lookAhead(Tk,[Tk|_]).

checkToken([Tk|Toks],Toks,Tk,_,_,_) :- !.
checkToken([Tk|Toks],Toks,_,Lc,Msg,Extra) :- locOfToken(Tk,Lc), reportError(Msg,[Tk|Extra],Lc).

checkTerminator(_,[],[]).
checkTerminator(_,Toks,Toks) :- Toks = [rbrce(_)|_].
checkTerminator(_,[term(_)|Toks],Toks) .
checkTerminator(rbrce,Toks,Toks).
checkTerminator(_,[Tk|Tks],RTks) :-
  locOfToken(Tk,Lc),
  reportError("missing terminator, got %s",[Tk],Lc),
  scanForTerminator(Tks,RTks).

scanForTerminator([term(_)|Tks],Tks).
scanForTerminator([],[]).
scanForTerminator([_|Tk],Tks) :-
  scanForTerminator(Tk,Tks).

handleInterpolation([segment(Str,Lc)],_,string(Lc,Str)).
handleInterpolation([],Lc,string(Lc,"")).
handleInterpolation(Segments,Lc,Term) :-
  stringSegments(Segments,Inters),
  unary(Lc,"ssSeq",tuple(Lc,"[]",Inters),Fltn),
  unary(Lc,"formatSS",Fltn,Term).

stringSegments([],[]).
stringSegments([Seg|More],[H|T]) :- stringSegment(Seg,H),!, stringSegments(More,T).

stringSegment(segment(Str,Lc),S) :-
  unary(Lc,"ss",string(Lc,Str),S).
stringSegment(interpolate(Text,"",Lc),Disp) :-
  subTokenize(Lc,Text,Toks),
  term(Toks,2000,Term,TksX,_),
  unary(Lc,"disp",Term,Disp),
  ( TksX = [] ; lookAhead(ATk,TksX),locOf(ATk,ALc),reportError("extra tokens in string interpolation",[],ALc)).
stringSegment(interpolate(Text,Fmt,Lc),Disp) :-
  subTokenize(Lc,Text,Toks),
  term(Toks,2000,Term,TksX,_),
  binary(Lc,"frmt",Term,string(Lc,Fmt),Disp),
  ( TksX = [] ; lookAhead(ATk,TksX),locOf(ATk,ALc),reportError("extra tokens in string interpolation",[],ALc)).
