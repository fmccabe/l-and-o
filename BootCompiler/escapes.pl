/* Automatically generated, do not edit */

:-module(escapes,[isEscape/1,escapeType/2]).

escapeType("exit",funType([in(type("lo.arith*integer"))],voidType)).
escapeType("_command_line",funType([],typeExp("lo.list*list",[type("lo.string*string")]))).
escapeType("_command_opts",funType([],typeExp("lo.list*list",[tupleType([type("lo.string*string"),type("lo.string*string")])]))).
escapeType("_unify",univType(kVar("t"),predType([inout(kVar("t")),inout(kVar("t"))]))).
escapeType("_identical",univType(kVar("t"),predType([in(kVar("t")),in(kVar("t"))]))).
escapeType("var",univType(kVar("t"),predType([inout(kVar("t"))]))).
escapeType("nonvar",univType(kVar("t"),predType([inout(kVar("t"))]))).
escapeType("_int_plus",funType([in(type("lo.arith*integer")),in(type("lo.arith*integer"))],type("lo.arith*integer"))).
escapeType("_int_minus",funType([in(type("lo.arith*integer")),in(type("lo.arith*integer"))],type("lo.arith*integer"))).
escapeType("_int_times",funType([in(type("lo.arith*integer")),in(type("lo.arith*integer"))],type("lo.arith*integer"))).
escapeType("_int_div",funType([in(type("lo.arith*integer")),in(type("lo.arith*integer"))],type("lo.arith*integer"))).
escapeType("_flt_plus",funType([in(type("lo.arith*float")),in(type("lo.arith*float"))],type("lo.arith*float"))).
escapeType("_flt_minus",funType([in(type("lo.arith*float")),in(type("lo.arith*float"))],type("lo.arith*float"))).
escapeType("_flt_times",funType([in(type("lo.arith*float")),in(type("lo.arith*float"))],type("lo.arith*float"))).
escapeType("_flt_div",funType([in(type("lo.arith*float")),in(type("lo.arith*float"))],type("lo.arith*float"))).
escapeType("_int_abs",funType([in(type("lo.arith*integer"))],type("lo.arith*integer"))).
escapeType("_flt_abs",funType([in(type("lo.arith*float"))],type("lo.arith*float"))).
escapeType("_int2flt",funType([in(type("lo.arith*integer"))],type("lo.arith*float"))).
escapeType("_flt2int",funType([in(type("lo.arith*float"))],type("lo.arith*integer"))).
escapeType("_pwr",funType([in(type("lo.arith*float")),in(type("lo.arith*float"))],type("lo.arith*float"))).
escapeType("_display",predType([in(tupleType([type("lo.arith*integer"),type("lo.arith*integer"),type("lo.arith*integer")])),in(topType)])).
escapeType("_isCcChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isCfChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isCnChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isCoChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isCsChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isLlChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isLmChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isLoChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isLtChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isLuChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isMcChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isMeChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isMnChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isNdChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isNlChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isNoChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isPcChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isPdChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isPeChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isPfChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isPiChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isPoChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isPsChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isScChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isSkChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isSmChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isSoChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isZlChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isZpChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isZsChar",predType([in(type("lo.arith*integer"))])).
escapeType("_isLetterChar",predType([in(type("lo.arith*integer"))])).
escapeType("_digitCode",funType([in(type("lo.arith*integer"))],type("lo.arith*integer"))).
escapeType("_int2str",funType([in(type("lo.arith*integer")),in(type("lo.arith*integer")),in(type("lo.arith*integer")),in(type("lo.arith*integer"))],type("lo.string*string"))).
escapeType("_flt2str",funType([in(type("lo.arith*float")),in(type("lo.arith*integer")),in(type("lo.arith*integer")),in(type("lo.logical*logical")),in(type("lo.logical*logical"))],type("lo.string*string"))).
escapeType("explode",funType([in(type("lo.string*string"))],typeExp("lo.list*list",[type("lo.arith*integer")]))).
escapeType("implode",funType([in(typeExp("lo.list*list",[type("lo.arith*integer")]))],type("lo.string*string"))).
isEscape("exit").
isEscape("_command_line").
isEscape("_command_opts").
isEscape("_unify").
isEscape("_identical").
isEscape("var").
isEscape("nonvar").
isEscape("_int_plus").
isEscape("_int_minus").
isEscape("_int_times").
isEscape("_int_div").
isEscape("_flt_plus").
isEscape("_flt_minus").
isEscape("_flt_times").
isEscape("_flt_div").
isEscape("_int_abs").
isEscape("_flt_abs").
isEscape("_int2flt").
isEscape("_flt2int").
isEscape("_pwr").
isEscape("_display").
isEscape("_isCcChar").
isEscape("_isCfChar").
isEscape("_isCnChar").
isEscape("_isCoChar").
isEscape("_isCsChar").
isEscape("_isLlChar").
isEscape("_isLmChar").
isEscape("_isLoChar").
isEscape("_isLtChar").
isEscape("_isLuChar").
isEscape("_isMcChar").
isEscape("_isMeChar").
isEscape("_isMnChar").
isEscape("_isNdChar").
isEscape("_isNlChar").
isEscape("_isNoChar").
isEscape("_isPcChar").
isEscape("_isPdChar").
isEscape("_isPeChar").
isEscape("_isPfChar").
isEscape("_isPiChar").
isEscape("_isPoChar").
isEscape("_isPsChar").
isEscape("_isScChar").
isEscape("_isSkChar").
isEscape("_isSmChar").
isEscape("_isSoChar").
isEscape("_isZlChar").
isEscape("_isZpChar").
isEscape("_isZsChar").
isEscape("_isLetterChar").
isEscape("_digitCode").
isEscape("_int2str").
isEscape("_flt2str").
isEscape("explode").
isEscape("implode").
