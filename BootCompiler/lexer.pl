:- module(lexer,[nextToken/3,allTokens/2,locOfToken/2,subTokenize/3,isToken/1,dispToken/2]).
:- use_module(operators).
:- use_module(errors).

/* tokenState(text,currLine,currOff,currPos) */

hedChar(tokenState([Ch|_],_,_,_),Ch).

hedHedChar(tokenState([_,Ch|_],_,_,_),Ch).

hedHedHedChar(tokenState([_,_,Ch|_],_,_,_),Ch).

initSt(Txt,tokenState(Txt,1,1,1)).
initSt(Chars,LineNo,Column,Base,tokenState(Chars,LineNo,Column,Base)).
isTerminal(tokenState([],_,_,_)).

nxtSt(tokenState(['\n'|T],L,_,CP),tokenState(T,LN,1,CP1)) :- succ(L,LN), succ(CP,CP1).
nxtSt(tokenState([Ch|T],L,O,P),tokenState(T,L,O1,P1)) :- Ch\='\n',succ(O,O1),succ(P,P1).
nxtSt(tokenState([],L,O,P),tokenState([],L,O,P)).

nextSt(tokenState(['\n'|T],L,_,CP),tokenState(T,LN,1,CP1),'\n') :- succ(L,LN), succ(CP,CP1).
nextSt(tokenState([Ch|T],L,O,P),tokenState(T,L,O1,P1),Ch) :- Ch\='\n',succ(O,O1),succ(P,P1).

nxtNxtSt(St,St2) :- nxtSt(St,St1), nxtSt(St1,St2).

lookingAt(St,Nxt,Test,Lc) :- lookingAt(St,Nxt,Test), makeLoc(St,Nxt,Lc).

lookingAt(St,St,[]).
lookingAt(St,NxSt,[Ch|M]) :- nextSt(St,St1,Ch), lookingAt(St1,NxSt,M).

makeLoc(tokenState(_,Ln,O,Ps),tokenState(_,_,_,Pe), loc(Ln,O,Ps,P)) :- P is Pe-Ps.

isToken(T) :- locOfToken(T,_).

locOfToken(lpar(Lc),Lc).
locOfToken(rpar(Lc),Lc).
locOfToken(lbra(Lc),Lc).
locOfToken(rbra(Lc),Lc).
locOfToken(lbrce(Lc),Lc).
locOfToken(rbrce(Lc),Lc).
locOfToken(idTok(_,Lc),Lc).
locOfToken(idQTok(_,Lc),Lc).
locOfToken(lqpar(Lc),Lc).
locOfToken(rqpar(Lc),Lc).
locOfToken(integerTok(_,Lc),Lc).
locOfToken(floatTok(_,Lc),Lc).
locOfToken(stringTok(_,Lc),Lc).
locOfToken(term(Lc),Lc).
locOfToken(terminal,missing).

dispToken(lpar(_),['(']).
dispToken(rpar(_),[')']).
dispToken(lbra(_),['[']).
dispToken(rbra(_),[']']).
dispToken(lbrce(_),['{']).
dispToken(rbrce(_),['}']).
dispToken(idQTok(Id,_),St) :- string_chars(Id,St).
dispToken(idTok(Id,_),St) :- string_chars(Id,St).
dispToken(lqpar(_),['<','|']).
dispToken(rqpar(_),['|','>']).
dispToken(integerTok(Ix,_),Str) :- number_string(Ix,St),string_chars(St,Str).
dispToken(floatTok(Dx,_),Str) :- number_string(Dx,St),string_chars(St,Str).
dispToken(stringTok(St,_),Str) :- string_chars(St,Str).
dispToken(term(_),['.',' ']).
dispToken(terminal,[]).

isSpace(' ').
isSpace('\t').
isSpace('\n').

isDigit('0',0).
isDigit('1',1).
isDigit('2',2).
isDigit('3',3).
isDigit('4',4).
isDigit('5',5).
isDigit('6',6).
isDigit('7',7).
isDigit('8',8).
isDigit('9',9).

isHexDigit(Ch,D) :- isDigit(Ch,D).
isHexDigit('a',10).
isHexDigit('b',11).
isHexDigit('c',12).
isHexDigit('d',13).
isHexDigit('e',14).
isHexDigit('f',15).

skipToNx(St,NxSt) :- hedChar(St,Ch), isSpace(Ch), nxtSt(St,St1), skipToNx(St1,NxSt).
skipToNx(St,NxSt) :- hedChar(St,'-'), hedHedChar(St,'-'), hedHedHedChar(St,Ch), isSpace(Ch), lineComment(St,St2), skipToNx(St2,NxSt).
skipToNx(St,NxSt) :- hedChar(St,'/'), hedHedChar(St,'*'), nxtNxtSt(St,St1), blockComment(St1,St2), skipToNx(St2,NxSt).
skipToNx(St,St).

lineComment(St,NxSt) :- nextSt(St,NxSt,'\n').
lineComment(St,NxSt) :- nextSt(St,St1,_), lineComment(St1,NxSt).
lineComment(St,St).

blockComment(St,NxSt) :- hedChar(St,'*'), hedHedChar(St,'/'), nxtNxtSt(St,NxSt).
blockComment(St,NxSt) :- nxtSt(St,St1), blockComment(St1,NxSt).

nxTok(St,NxSt,integerTok(Code,Lc)) :- lookingAt(St,St1,['0','c'],_), charRef(St1,NxSt,Ch), char_code(Ch,Code), makeLoc(St,NxSt,Lc).
nxTok(St,NxSt,integerTok(Hx,Lc)) :- lookingAt(St,St1,['0','x'],_), readHex(St1,NxSt,0,Hx), makeLoc(St,NxSt,Lc).
nxTok(St,NxSt,Tk) :- hedChar(St,Ch), isDigit(Ch,_), readNumber(St,NxSt,Tk).
nxTok(St,NxSt,idQTok(Id,Lc)) :- nextSt(St,St1,''''), readQuoted(St1,NxSt,Id), makeLoc(St,NxSt,Lc).
nxTok(St,NxSt,stringTok([Seg],Lc)) :- lookingAt(St,St1,['"','"','"']), stringBlob(St1,St2,Txt),lookingAt(St2,NxSt,['"','"','"']),string_chars(Seg,Txt),makeLoc(St1,St2,Lc).
nxTok(St,NxSt,Str) :- nextSt(St,St1,'"'), readString(St1,NxSt,Str).
nxTok(St,NxSt,idTok(Id,Lc)) :- hedChar(St,Ch), idStart(Ch), readIden(St,NxSt,Id), makeLoc(St,NxSt,Lc).
nxTok(St,NxSt,lpar(Lc)) :- lookingAt(St,NxSt,['('],Lc).
nxTok(St,NxSt,rpar(Lc)) :- lookingAt(St,NxSt,[')'],Lc).
nxTok(St,NxSt,lbra(Lc)) :- lookingAt(St,NxSt,['['],Lc).
nxTok(St,NxSt,rbra(Lc)) :- lookingAt(St,NxSt,[']'],Lc).
nxTok(St,NxSt,lqbrce(Lc)) :- lookingAt(St,NxSt,['{','.'],Lc).
nxTok(St,NxSt,rqbrce(Lc)) :- lookingAt(St,NxSt,['.','}'],Lc).
nxTok(St,NxSt,lbrce(Lc)) :- lookingAt(St,NxSt,['{'],Lc).
nxTok(St,NxSt,rbrce(Lc)) :- lookingAt(St,NxSt,['}'],Lc).
nxTok(St,NxSt,lqpar(Lc)) :- lookingAt(St,NxSt,['<','|'],Lc).
nxTok(St,NxSt,rqpar(Lc)) :- lookingAt(St,NxSt,['|','>'],Lc).
nxTok(St,NxSt,term(Lc)) :- lookingAt(St,NxSt,['.',' '],Lc).
nxTok(St,NxSt,term(Lc)) :- lookingAt(St,NxSt,['.','\t'],Lc).
nxTok(St,NxSt,term(Lc)) :- lookingAt(St,NxSt,['.','\n'],Lc).
nxTok(St,NxSt,term(Lc)) :- nextSt(St,NxSt,'.'), nextSt(NxSt,_,'}'), makeLoc(St,NxSt,Lc).
nxTok(St,NxSt,idTok(Id,Lc)) :- nextSt(St,St1,Ch), follows('',Ch,_), followGraph(Ch,Id,St1,NxSt), !, makeLoc(St,NxSt,Lc).
nxTok(St,NxSt,Tk) :-
  nextSt(St,St1,Ch),
  makeLoc(St,St1,Lc),
  reportError("invalid character: %s",[[Ch]],Lc),
  skipToNx(St1,St2),
  nxTok(St2,NxSt,Tk).
nxTok(St,St,terminal) :- isTerminal(St).

readNumber(St,NxSt,Tk) :- readNumber(St,St,NxSt,1,Tk).

readNumber(St0,St,NxSt,Sgn,Tk) :- readNatural(St,St1,0,D), readMoreNumber(St0,St1,NxSt,Sgn,D,Tk).

readNatural(St,NxSt,D,N) :- nextSt(St,St1,Ch), isDigit(Ch,Dx), D1 is D*10+Dx, readNatural(St1,NxSt,D1,N).
readNatural(St,St,D,D).

readDecimal(St,NxSt,Ng) :- nextSt(St,St1,'-'), readNatural(St1,NxSt,0,N), Ng is -N.
readDecimal(St,NxSt,N) :- readNatural(St,NxSt,0,N).

readMoreNumber(St0,St,NxSt,Sgn,D,floatTok(FP,Lc)) :- nextSt(St,St1,'.'), hedChar(St1,Dg), isDigit(Dg,_), readFraction(St1,St2,D,0.1,Fr), Mant is Sgn*Fr, readExponent(St2,NxSt,Mant,FP), makeLoc(St0,NxSt,Lc).
readMoreNumber(St0,St,St,Sgn,D,integerTok(FP,Lc)) :- FP is Sgn*D, makeLoc(St0,St,Lc).

readFraction(St,NxSt,SF,Fx,Fr) :- nextSt(St,St1,Ch), isDigit(Ch,D), SF1 is SF+D*Fx, Fx1 is Fx/10, readFraction(St1,NxSt,SF1,Fx1,Fr).
readFraction(St,St,Fr,_,Fr).

readExponent(St,NxSt,M,FP) :- nextSt(St,St1,'e'), readDecimal(St1,NxSt,E), FP is M*10**E.
readExponent(St,St,M,M).

readHex(St,NxSt,SF,Hx) :- nextSt(St,St1,D), isHexDigit(D,H), !, NF is SF*16+H, readHex(St1,NxSt,NF,Hx).
readHex(St,St,SF,SF).

charRef(St,Nxt,Chr) :- nextSt(St,St1,'\\'), nextSt(St1,St2,Ch), backslashRef(St2,Nxt,Ch,Chr).
charRef(St,Nxt,Chr) :- nextSt(St,Nxt,Chr).

backslashRef(St,St,'a','\a').
backslashRef(St,St,'b','\b').
backslashRef(St,St,'e','\e').
backslashRef(St,St,'t','\t').
backslashRef(St,St,'n','\n').
backslashRef(St,St,'r','\r').
backslashRef(St,Nxt,'u',Hx) :- hexChar(St,Nxt,0,Hx).
backslashRef(St,St,Ch,Ch).

hexChar(St,NxSt,SF,SF) :- nextSt(St,NxSt,';').
hexChar(St,NxSt,SF,Hx) :- nextSt(St,St1,D), isHexDigit(D,H), NF is SF*16+H, hexChar(St1,NxSt,NF,Hx).

readQuoted(St,NxSt,Id) :- readUntil(St,NxSt,'''',Chrs), string_chars(Id,Chrs).

readUntil(St,NxSt,Stop,[]) :- nextSt(St,NxSt,Stop).
readUntil(St,NxSt,Stop,[Ch|Chrs]) :- charRef(St,St1,Ch), readUntil(St1,NxSt,Stop,Chrs).

readString(St,NxSt,stringTok(Str,Lc)) :- readMoreString(St,St2,Str), makeLoc(St,St2,Lc), nextSt(St2,NxSt,'"').

readMoreString(St,St,[]) :- hedChar(St,'"').
readMoreString(St,NxSt,[Seg|Segments]) :-
      nextSt(St,St1,'\\'),
      hedChar(St1,'('),!,
      interpolation(St1,St2,Seg),
      readMoreString(St2,NxSt,Segments).
readMoreString(St,NxSt,Segments) :-
      readStr(St,St1,Chars),!,
      string_chars(Seg,Chars),
      makeLoc(St,St1,Lc),
      appendSegment(Seg,Lc,Segments,More),
      readMoreString(St1,NxSt,More).

appendSegment("",_,Segments,Segments).
appendSegment(Seg,Lc,[segment(Seg,Lc)|More],More) :-
  \+Seg="".

readStr(St,St,[]) :- hedChar(St,'"'). /* Terminated by end of string */
readStr(St,St,[]) :- hedChar(St,'\\'), hedHedChar(St,'('). /* or an interpolation marker */
readStr(St,St1,[]) :- nextSt(St,St1,'\n'), !,
  makeLoc(St,St1,Lc),
  reportError("new line found in string",[],Lc).
readStr(St,NxSt,[Ch|Seg]) :- charRef(St,St1,Ch), readStr(St1,NxSt,Seg).

stringBlob(St,St,[]) :- lookingAt(St,_,['"','"','"']).
stringBlob(St,NxSt,[Ch|L]) :- nextSt(St,St1,Ch), stringBlob(St1,NxSt,L).

interpolation(St,NxSt,interpolate(Text,Fmt,Lc)) :-
    bracketCount(St,St1,[],Text),
    readFormat(St1,NxSt,FmtChars),
    string_chars(Fmt,FmtChars),
    makeLoc(St,NxSt,Lc).

bracketCount(St,NxSt,Stk,Chrs) :- nextSt(St,St1,Ch), bracketCount(St,St1,NxSt,Ch,Stk,Chrs).

bracketCount(_,St1,NxSt,Cl,[Cl|Stk],[Cl|Chrs]) :- nextSt(St1,St2,Ch), bracketCount(St1,St2,NxSt,Ch,Stk,Chrs).
bracketCount(_,St1,NxSt,'(',Stk,['('|Chrs]) :- nextSt(St1,St2,Ch), bracketCount(St1,St2,NxSt,Ch,[')'|Stk],Chrs).
bracketCount(_,St1,NxSt,'[',Stk,['['|Chrs]) :- nextSt(St1,St2,Ch), bracketCount(St1,St2,NxSt,Ch,[']'|Stk],Chrs).
bracketCount(_,St1,NxSt,'{',Stk,['{'|Chrs]) :- nextSt(St1,St2,Ch), bracketCount(St1,St2,NxSt,Ch,['}'|Stk],Chrs).
bracketCount(St,_,St,_,[],[]). /* stop if the bracket stack is empty */
bracketCount(_,St1,NxSt,Ch,Stk,[Ch|Chrs]) :- nextSt(St1,St2,Chr), bracketCount(St1,St2,NxSt,Chr,Stk,Chrs).

readFormat(St,NxSt,Chrs) :- nextSt(St,St1,':'), readUntil(St1,NxSt,';',Chrs).
readFormat(St,St,[]) :- \+ hedChar(St,':').

idStart(L) :- char_type(L,alpha).
idStart('_').

idChar(X) :- idStart(X).
idChar(X) :- isDigit(X,_).

readIden(St,NxSt,Id) :- readId(St,NxSt,II), string_chars(Id,II).

readId(St,NxSt,[F|M]) :- nextSt(St,St1,F), idStart(F), readMoreId(St1,NxSt,M).

readMoreId(St,NxSt,[C|M]):- nextSt(St,St1,C), idChar(C), readMoreId(St1,NxSt,M).
readMoreId(St,St,[]).

followGraph(Ch,Id,St,NxSt) :- nextSt(St,St1,NxCh), follows(Ch,NxCh,NxId), followGraph(NxId,Id,St1,NxSt).
followGraph(Ch,Id,St,St) :- final(Ch,Id).

nextToken(St,NxSt,Tk) :- skipToNx(St,St1), nxTok(St1,NxSt,Tk).

allTokens(Txt,Toks) :- initSt(Txt,St), tokenize(St,_,Toks), !.

tokenize(St,NxSt,Toks) :- skipToNx(St,St1), nxTokenize(St1,NxSt,Toks).

nxTokenize(St,St,[]) :- isTerminal(St).
nxTokenize(St,NxSt,[Tok|More]) :- nxTok(St,St1,Tok), !, tokenize(St1,NxSt,More).

subTokenize(loc(LineNo,Column,Base,_),Chars,Toks) :-
    initSt(Chars,LineNo,Column,Base,St),
    nxTokenize(St,NxSt,Toks),!,
    (isTerminal(NxSt) ; writef("Extra characters: %w\n",[NxSt])).
