'#pkg'("n7o7'()7'n2o2'pkg's'lo'e'*'n9o9'()9'n2o2'import'e'public'n2o2'pkg's'lo.trie'e'*'n2o2'import'e'public'n2o2'pkg's'lo.sets'e'*'n2o2'import'e'public'n2o2'pkg's'lo.collection'e'*'n2o2'import'e'public'n2o2'pkg's'lo.bits'e'*'n2o2'import'e'public'n2o2'pkg's'lo.coerce'e'*'n2o2'import'e'public'n2o2'pkg's'lo.io'e'*'n2o2'import'e'public'n2o2'pkg's'lo.index'e'*'n2o2'import'e'public'n2o2'pkg's'lo.list'e'*'n2o2'import'e'public'n2o2'pkg's'lo.core'e'*'s\"I2'flatten'FT1t'lo.core*ss'LS'formatSS'FT1t'lo.core*ss'S\"s'I0'n0o0'()0'n0o0'()0'n0o0'()0'").
'lo@init'() :- !.
'lo@flttnList'('lo.core#[]', XRest, XRest) :- !.
'lo@flttnList'('lo.core#,..'(XE, XR), XRest, XX713) :- !,
    'lo@flttnList'(XR, XRest, XX712),
    'lo@flttn'(XE, XX712, XX713).
'lo@flttnList'(_, _, _) :- raise_exception('error'("flttnList", 21, 3, 26)).
'lo@flttn'('lo.core#ss'(XS), XRest, 'lo.core#,..'(XS, XRest)) :- !.
'lo@flttn'('lo.core#ssSeq'(XL), XRest, XX725) :- !,
    'lo@flttnList'(XL, XRest, XX725).
'lo@flttn'('lo.core#sc'(XC), XRest, 'lo.core#,..'(XX732, XRest)) :- !,
    'implode'('lo.core#,..'(XC, 'lo.core#[]'), XX732).
'lo@flttn'(_, _, _) :- raise_exception('error'("flttn", 16, 3, 31)).
'lo@flatten'(XD, XX738) :- !,
    'lo@flttn'(XD, 'lo.core#[]', XX738).
'lo@flatten'(_, _) :- raise_exception('error'("flatten", 13, 3, 25)).
'lo@formatSS'(XS, XX742) :- !,
    'lo@flatten'(XS, XX741),
    '_str_multicat'(XX741, XX742).
'lo@formatSS'(_, _) :- raise_exception('error'("formatSS", 25, 3, 40)).
'lo^flttnList'('_call%3'(XV281, XV282, XV283), 'lo^flttnList', _) :- 'lo@flttnList'(XV281, XV282, XV283).
'lo^flttn'('_call%3'(XV284, XV285, XV286), 'lo^flttn', _) :- 'lo@flttn'(XV284, XV285, XV286).
'lo^flatten'('_call%2'(XV287, XV288), 'lo^flatten', _) :- 'lo@flatten'(XV287, XV288).
'lo^formatSS'('_call%2'(XV289, XV290), 'lo^formatSS', _) :- 'lo@formatSS'(XV289, XV290).