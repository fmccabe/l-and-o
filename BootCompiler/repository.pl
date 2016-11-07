:- module(repository,[openRepository/2,
          locatePackage/6,locatePackage/5,
          openPackageAsStream/5,
          addPackage/5,addPackage/6,
          packagePresent/7]).

% Implement a file-based repository.

:- use_module(uri).
:- use_module(misc).
:- use_module(resource).
:- use_module(parseUtils).

openRepository(Root,repo(Root,Manifest)) :-
  resolveFile(Root,"manifest",MF),
  access_file(MF,read),
  readFile(MF,Chrs),
  parseManifest(Chrs,Manifest),!.
openRepository(Root,repo(Root,man([]))) :-
  resolveFile(Root,"manifest",MF),
  access_file(MF,write),!.

locatePackage(repo(Root,Man),Pkg,V,Vers,U,Text) :-
  locateVersion(Man,Pkg,V,Vers,U,Fn),
  resolveFile(Root,Fn,FileNm),
  readFile(FileNm,Text).

locatePackage(repo(Root,Man),Pkg,Vers,U,Text) :-
  locateVersion(Man,Pkg,defltVersion,Vers,U,Fn),
  resolveFile(Root,Fn,FileNm),
  readFile(FileNm,Text).

resolveFile(Root,Fl,MF) :-
  parseURI(Fl,Rel),
  resolveURI(Root,Rel,F),
  getUriPath(F,MF).

openPackageAsStream(repo(Root,Man),Pkg,Vers,U,Stream) :-
  locateVersion(Man,Pkg,defltVersion,Vers,U,fl(Fn)),
  resolveFile(Root,Fn,Fl),
  open(Fl,read,Stream).

locateVersion(man(Entries),Pkg,Vers,Act,U,Fn) :-
  is_member(entry(Pkg,V),Entries),
  getVersion(Vers,Act,V,U,Fn).

getVersion(Vers,Vers,V,U,Fn) :- is_member((Vers,U,Fn),V),!.
getVersion(defltVersion,Act,V,U,Fn) :- is_member((Act,U,Fn),V),!.

addPackage(Repo,U,Pkg,Text,Rx) :-
  addPackage(Repo,U,Pkg,defltVersion,Text,Rx).
addPackage(repo(Root,Man),U,Pkg,Vers,Text,repo(Root,NM)) :-
  packageHash(Pkg,Vers,Hash),
  string_concat(Pkg,Hash,Fn),
  resolveFile(Root,Fn,FileNm),
  writeFile(FileNm,Text),!,
  addToManifest(Man,U,pkg(Pkg),Vers,fl(Fn),NM),
  flushManifest(Root,NM).

packageHash(Pkg,defltVersion,Hash) :-
  stringHash(0,Pkg,H),
  hashSixtyFour(H,Hash).
packageHash(Pkg,v(V),Hash) :-
  stringHash(0,Pkg,H1),
  stringHash(H1,V,H2),
  hashSixtyFour(H2,Hash).

packagePresent(repo(Root,Man),Pkg,V,Vers,SrcFn,SrcWhen,When) :-
  locateVersion(Man,Pkg,V,Vers,U,fl(Fn)),
  resolveFile(Root,Fn,FileNm),
  access_file(FileNm,read),
  time_file(FileNm,When),
  getUriPath(U,SrcFn),
  time_file(SrcFn,SrcWhen).

flushManifest(Root,M) :-
  showManifest(M,Chrs,[]),
  resolveFile(Root,"manifest",Fn),
  string_chars(Text,Chrs),
  writeFile(Fn,Text).

%% Each end-point directory has a manifest file in it.
% The role of the manifest is to map URIs to files

readManifest(Path,Manifest) :-
  readFile(Path,Chars),
  parseManifest(Chars,Manifest).

parseManifest(Chrs,M) :-
  phrase(manifest(M),Chrs).

manifest(man(E)) --> manifst, lbrce, entries(E), rbrce,spaces.

% An entry looks like:
% packageName : { (version:file)* }

entries([Entry|More]) --> entry(Entry), entries(More).
entries([]) --> [].

entry(entry(Package,Versions)) --> package(Package), colon, lbrce, versions(Versions), rbrce.

manifst --> spaces, ['m','a','n','i','f','e','s','t'].

colon --> spaces, [':'].

lbrce --> spaces, ['{'].

rbrce --> spaces, ['}'].

equals --> spaces, ['='].

package(pkg(Pkg)) --> spaces, iden(Id), suffixes(Suf), { flatten([Id|Suf], P), string_chars(Pkg,P)}.

suffixes([['.'|Id]|More]) --> ['.'], iden(Id), suffixes(More).
suffixes([]) --> [].

fileName(fl(Fn)) --> spaces, fileSeg(F), segments(Rest), { flatten([F|Rest],Fl), string_chars(Fn,Fl)}.

fileSeg([Ch|Mo]) --> (alpha(Ch) ; digit(Ch) ; period(Ch)), fileSeg(Mo).
fileSeg([]) --> [].

segments([['/'|Seg]|More]) --> ['/'], fileSeg(Seg), segments(More).
segments([]) --> \+['/'].

period('.') --> ['.'].

versions([V|More]) --> version(V), versions(More).
versions([]) --> [].

version((V,U,F)) --> versionName(V), equals, fileName(F), ['['], uri(U), [']'], !.

versionName(defltVersion) --> spaces, ['*'].
versionName(v(V)) --> spaces, fileSeg(S), { string_chars(V,S) }. %% do more later

showManifest(man(E),O,Ox) :- 
  appStr("manifest",O,O1),
  appStr("{\n",O1,O2),
  showEntries(E,O2,O3),
  appStr("}\n",O3,Ox).

showEntries([],O,O).
showEntries([E|M],O,Ox) :-
  showEntry(E,O,O1),
  showEntries(M,O1,Ox).

showEntry(entry(Pkg,Versions),O,Ox) :-
  appStr("  ",O,O0),
  showPkg(Pkg,O0,O1),
  appStr(":{\n",O1,O2),
  showVersions(Versions,O2,O3),
  appStr("  }\n",O3,Ox).

showPkg(pkg(Pkg),O,Ox) :-
  appStr(Pkg,O,Ox).

showVersions([],O,O).
showVersions([V|M],O,Ox) :-
  showVersion(V,O,O1),
  showVersions(M,O1,Ox).

showVersion((V,U,F),O,Ox) :-
  appStr("    ",O,O1),
  showV(V,O1,O2),
  appStr("=",O2,O3),
  showFileName(F,O3,O4),
  appStr("[",O4,O5),
  showUri(U,O5,O6),
  appStr("]\n",O6,Ox).

showV(v(V),O,Ox) :-
  appStr(V,O,Ox).
showV(defltVersion,O,Ox) :-
  appStr("*",O,Ox).

showFileName(fl(Nm),O,Ox) :-
  appStr(Nm,O,Ox).

addToManifest(M,U,Pkg,FileName,NM) :-
  addToManifest(M,U,Pkg,defltVersion,FileName,NM).
addToManifest(man(M),U,Pkg,Version,FileName,man(NM)) :-
  addEntry(M,U,Pkg,Version,FileName,NM).

addEntry([],U,Pkg,Version,FileName,[(entry(Pkg,[(Version,U,FileName)]))]).
addEntry([entry(Pkg,Vers)|E],U,Pkg,Version,FileName,[entry(Pkg,NV)|E]) :- !,
  addVersion(Vers,U,Version,FileName,NV).
addEntry([E|M],U,Pkg,Version,FileName,[E|R]) :-
  addEntry(M,U,Pkg,Version,FileName,R).

addVersion([],U,Vers,FileNm,[(Vers,U,FileNm)]).
addVersion([(Vers,_,_)|V],U,Vers,FileNm,[(Vers,U,FileNm)|V]) :- !. % replace version
addVersion([V|M],U,Vers,FileNm,[V|R]) :- addVersion(M,U,Vers,FileNm,R).
