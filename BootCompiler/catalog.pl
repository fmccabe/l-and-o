:- module(catalog,[locateCatalog/2,catalog//1,resolveCatalog/3,catalogBase/2]).

:- use_module(resource).
:- use_module(misc).
:- use_module(uri).
:- use_module(parseUtils).

locateCatalog(Uri,Cat) :-
  resolveURI(Uri,relUri(rel(["catalog"]),noQuery),CatURI),
  locateResource(CatURI,Chars),
  parseCatalog(Chars,CatURI,Cat),!.

resolveCatalog(cat(Cat),Nm,Uri) :-
  is_member(entries(Map),Cat),
  is_member(base(Base),Cat),
  is_member(entry(Nm,U),Map),!,
  resolveURI(Base,U,Uri).
resolveCatalog(cat(Cat),Nm,Uri) :-
  is_member(default(Deflt),Cat),!,
  is_member(base(Base),Cat),
  resolveURI(Base,Deflt,DefltUri),
  locateCatalog(DefltUri,DefltCat),
  resolveCatalog(DefltCat,Nm,Uri).

parseCatalog(Chrs,Uri,Cat) :-
  phrase(tokens(Toks),Chrs),
  phrase(catalog(C),Toks),
  defaultBase(C,Uri,Cat),!.

defaultBase(cat(Stmts),Fl,cat(NStmts)) :-
  replace(Stmts,base(_),base(Fl),NStmts).

catalogBase(cat(Stmts),Base) :-
  is_member(base(Base),Stmts).

catalog(cat(Stmts)) -->
  [catalog, lbrce], catStmts(Stmts), [rbrce].

catStmts([St|More]) --> catStmt(St), catStmts(More).
catStmts([]) --> [].

catStmt(entries(Contents)) --> [content, lbrce], contents(Contents), [rbrce].
catStmt(base(BaseUri)) --> [base, colon], string(Base), { parseURI(Base,BaseUri) }, [term].
catStmt(version(V)) --> [version, colon], version(V), [term].
catStmt(default(CatUri)) --> [default, colon], string(U), { parseURI(U,CatUri) }, [term].

contents([Entry|More]) --> entry(Entry), contents(More).
contents([]) --> [].

entry(entry(Key,Uri)) --> package(Key), [thin_arrow], string(U), [term], {parseURI(U,Uri)}.

package(pkg(Pkg,V)) --> [iden(Id)], suffixes(Suf), version(V), { stringify([Id|Suf], ".", P), string_chars(Pkg,P)}.

suffixes([Id|More]) --> [period], [iden(Id)], suffixes(More).
suffixes([]) --> [].

version(v(V)) --> [hash, iden(F)], vSegs(Vs), { stringify([F|Vs],".", V)}.
version(defltVersion) --> [].

vSegs([V|More]) --> [period], iden(iden(V)), vSegs(More).
vSegs([]) --> [].

stringify(L,S,O) :-
  interleave(L,S,I),
  concatStrings(I,O).

string(S) --> [string(S)].

% Tokenization

tokens(Toks) --> spaces, moreToks(Toks).

moreToks([]) --> at_end.
moreToks([Tok|More]) --> token(Tok), tokens(More).

token(catalog) --> [c,a,t,a,l,o,g].
token(content) --> [c,o,n,t,e,n,t].
token(version) --> [v,e,r,s,i,o,n].
token(base) --> [b,a,s,e].
token(default) --> [d,e,f,a,u,l,t].
token(product) --> [p,r,o,d,u,c,t].
token(is) --> [i,s].
token(lbrce) --> ['{'].
token(rbrce) --> ['}'].
token(thin_arrow) --> ['-','>'].
token(T) --> ['.'], ((at_end ; [' '] ; ['\n']) -> {T=term} ; {T=period}).
token(colon) --> [':'].
token(hash) --> ['#'].
token(string(Text)) --> ['"'], stringText(Seq), ['"'], { string_chars(Text,Seq) }.
token(iden(Text)) --> iden(Text).

stringText([]) --> [].
stringText([C|More]) --> ['\\'], quote(C), stringText(More).
stringText([C|More]) --> [C], { C \= '"' }, stringText(More).

quote('\n') --> 'n'.
quote('"') --> '"'.
quote('''') --> ''''.
quote('\t') --> t.
quote(C) --> [C].
