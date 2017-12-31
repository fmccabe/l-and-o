/* Automatically generated, do not edit */

:-module(escapes,[isEscape/2,escapeType/2]).

escapeType("_exit",predType([(inMode,type("lo.core*integer"))])).
escapeType("_command_line",funType([],typeExp(tpFun("lo.core*list",1),[type("lo.core*string")]))).
escapeType("_command_opts",funType([],typeExp(tpFun("lo.core*list",1),[tupleType([type("lo.core*string"),type("lo.core*string")])]))).
escapeType("_unify",univType(kVar("t"),predType([(biMode,kVar("t")),(biMode,kVar("t"))]))).
escapeType("_identical",univType(kVar("t"),predType([(biMode,kVar("t")),(biMode,kVar("t"))]))).
escapeType("var",univType(kVar("t"),predType([(biMode,kVar("t"))]))).
escapeType("ground",univType(kVar("t"),predType([(biMode,kVar("t"))]))).
escapeType("_call",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*integer")),(inMode,typeExp(tpFun("lo.core*list",1),[typeExp(tpFun("lo.core*list",1),[type("lo.core*string")])]))])).
escapeType("_defined",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*integer"))])).
escapeType("_int_plus",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_int_minus",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_int_times",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_int_div",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_int_mod",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_flt_plus",funType([(inMode,type("lo.core*float")),(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("_flt_minus",funType([(inMode,type("lo.core*float")),(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("_flt_times",funType([(inMode,type("lo.core*float")),(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("_flt_div",funType([(inMode,type("lo.core*float")),(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("_flt_mod",funType([(inMode,type("lo.core*float")),(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("_int_abs",funType([(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_flt_abs",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("_int_lt",predType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))])).
escapeType("_int_ge",predType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))])).
escapeType("_flt_lt",predType([(inMode,type("lo.core*float")),(inMode,type("lo.core*float"))])).
escapeType("_flt_ge",predType([(inMode,type("lo.core*float")),(inMode,type("lo.core*float"))])).
escapeType("_int2flt",funType([(inMode,type("lo.core*integer"))],type("lo.core*float"))).
escapeType("_flt2int",funType([(inMode,type("lo.core*float"))],type("lo.core*integer"))).
escapeType("_flt_hash",funType([(inMode,type("lo.core*float"))],type("lo.core*integer"))).
escapeType("_pwr",funType([(inMode,type("lo.core*float")),(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("sqrt",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("exp",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("log",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("log10",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("pi",funType([],type("lo.core*float"))).
escapeType("sin",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("cos",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("tan",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("asin",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("acos",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("atan",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("trunc",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("floor",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("ceil",funType([(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("integral",predType([(inMode,type("lo.core*float"))])).
escapeType("srand",predType([(inMode,type("lo.core*float"))])).
escapeType("rand",funType([],type("lo.core*float"))).
escapeType("irand",funType([(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_ldexp",funType([(inMode,type("lo.core*float")),(inMode,type("lo.core*float"))],type("lo.core*float"))).
escapeType("_frexp",predType([(inMode,type("lo.core*float")),(outMode,type("lo.core*float")),(outMode,type("lo.core*integer"))])).
escapeType("_modf",predType([(inMode,type("lo.core*float")),(outMode,type("lo.core*float")),(outMode,type("lo.core*float"))])).
escapeType("_band",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_bor",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_bxor",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_blsl",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_blsr",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_basr",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_bnot",funType([(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_nthb",predType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))])).
escapeType("_suspend",univType(kVar("u"),predType([(biMode,kVar("u")),(inMode,predType([]))]))).
escapeType("_get_file",funType([(inMode,type("lo.core*string"))],type("lo.core*string"))).
escapeType("_put_file",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*string"))])).
escapeType("_cwd",funType([],type("lo.core*string"))).
escapeType("_cd",predType([(inMode,type("lo.core*string"))])).
escapeType("_rm",predType([(inMode,type("lo.core*string"))])).
escapeType("_mv",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*string"))])).
escapeType("_mkdir",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*integer"))])).
escapeType("_rmdir",predType([(inMode,type("lo.core*string"))])).
escapeType("_isdir",predType([(inMode,type("lo.core*string"))])).
escapeType("_chmod",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*integer"))])).
escapeType("_ls",funType([(inMode,type("lo.core*string"))],typeExp(tpFun("lo.core*list",1),[type("lo.core*string")]))).
escapeType("_file_mode",funType([(inMode,type("lo.core*string"))],type("lo.core*integer"))).
escapeType("_file_present",predType([(inMode,type("lo.core*string"))])).
escapeType("_file_type",funType([(inMode,type("lo.core*string"))],type("lo.core*integer"))).
escapeType("_file_size",funType([(inMode,type("lo.core*string"))],type("lo.core*integer"))).
escapeType("_file_modified",funType([(inMode,type("lo.core*string"))],type("lo.core*integer"))).
escapeType("_file_date",predType([(inMode,type("lo.core*string")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer"))])).
escapeType("_openInFile",funType([(inMode,type("lo.core*string")),(inMode,type("lo.core*integer"))],type("lo.io*fileHandle"))).
escapeType("_openOutFile",funType([(inMode,type("lo.core*string")),(inMode,type("lo.core*integer"))],type("lo.io*fileHandle"))).
escapeType("_openAppendFile",funType([(inMode,type("lo.core*string")),(inMode,type("lo.core*integer"))],type("lo.io*fileHandle"))).
escapeType("_openAppendIOFile",funType([(inMode,type("lo.core*string")),(inMode,type("lo.core*integer"))],type("lo.io*fileHandle"))).
escapeType("_popen",predType([(inMode,type("lo.core*string")),(inMode,typeExp(tpFun("lo.core*list",1),[type("lo.core*string")])),(inMode,typeExp(tpFun("lo.core*list",1),[tupleType([type("lo.core*string"),type("lo.core*string")])])),(inMode,type("lo.io*fileHandle")),(inMode,type("lo.io*fileHandle")),(inMode,type("lo.io*fileHandle"))])).
escapeType("_close",predType([(inMode,type("lo.io*fileHandle"))])).
escapeType("_end_of_file",predType([(inMode,type("lo.io*fileHandle"))])).
escapeType("_ready",predType([(inMode,type("lo.io*fileHandle"))])).
escapeType("_inchars",funType([(inMode,type("lo.io*fileHandle")),(inMode,type("lo.core*integer"))],type("lo.core*string"))).
escapeType("_inbytes",funType([(inMode,type("lo.io*fileHandle")),(inMode,type("lo.core*integer"))],typeExp(tpFun("lo.core*list",1),[type("lo.core*integer")]))).
escapeType("_inchar",funType([(inMode,type("lo.io*fileHandle"))],type("lo.core*integer"))).
escapeType("_inbyte",funType([(inMode,type("lo.io*fileHandle"))],type("lo.core*integer"))).
escapeType("_inline",funType([(inMode,type("lo.io*fileHandle"))],type("lo.core*string"))).
escapeType("_intext",funType([(inMode,type("lo.io*fileHandle")),(inMode,type("lo.core*string"))],type("lo.core*string"))).
escapeType("_outch",predType([(inMode,type("lo.io*fileHandle")),(inMode,type("lo.core*integer"))])).
escapeType("_outbyte",predType([(inMode,type("lo.io*fileHandle")),(inMode,type("lo.core*integer"))])).
escapeType("_outbytes",predType([(inMode,type("lo.io*fileHandle")),(inMode,typeExp(tpFun("lo.core*list",1),[type("lo.core*integer")]))])).
escapeType("_outtext",predType([(inMode,type("lo.io*fileHandle")),(inMode,type("lo.core*string"))])).
escapeType("_stdfile",funType([(inMode,type("lo.core*integer"))],type("lo.io*fileHandle"))).
escapeType("_fposition",funType([(inMode,type("lo.io*fileHandle"))],type("lo.core*integer"))).
escapeType("_fseek",predType([(inMode,type("lo.io*fileHandle")),(inMode,type("lo.core*integer"))])).
escapeType("_flush",predType([(inMode,type("lo.io*fileHandle"))])).
escapeType("_flushall",predType([])).
escapeType("_setfileencoding",predType([(inMode,type("lo.io*fileHandle")),(inMode,type("lo.core*integer"))])).
escapeType("_install_pkg",funType([(inMode,type("lo.core*string"))],typeExp(tpFun("lo.core*list",1),[tupleType([type("lo.core*string"),type("lo.core*string")])]))).
escapeType("_pkg_is_present",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*string")),(outMode,type("lo.core*string")),(outMode,type("lo.core*string"))])).
escapeType("_logmsg",predType([(inMode,type("lo.core*string"))])).
escapeType("_connect",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer")),(inMode,type("lo.io*fileHandle")),(inMode,type("lo.io*fileHandle"))])).
escapeType("_listen",predType([(inMode,type("lo.core*integer")),(inMode,type("lo.io*fileHandle"))])).
escapeType("_accept",predType([(inMode,type("lo.core*integer")),(inMode,type("lo.io*fileHandle")),(inMode,type("lo.io*fileHandle")),(inMode,type("lo.core*string")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*string"))])).
escapeType("_udpPort",predType([(inMode,type("lo.core*integer")),(inMode,type("lo.io*udpHandle"))])).
escapeType("_udpGet",predType([(inMode,type("lo.io*udpHandle")),(inMode,type("lo.core*string")),(outMode,type("lo.core*string")),(outMode,type("lo.core*integer"))])).
escapeType("_udpSend",predType([(inMode,type("lo.io*udpHandle")),(inMode,type("lo.core*string")),(outMode,type("lo.core*string")),(outMode,type("lo.core*integer"))])).
escapeType("_udpClose",predType([(inMode,type("lo.io*udpHandle"))])).
escapeType("hosttoip",funType([(inMode,type("lo.core*string"))],typeExp(tpFun("lo.core*list",1),[type("lo.core*string")]))).
escapeType("iptohost",funType([(inMode,type("lo.core*string"))],type("lo.core*string"))).
escapeType("_delay",predType([(inMode,type("lo.core*float"))])).
escapeType("_sleep",predType([(inMode,type("lo.core*float"))])).
escapeType("_now",funType([],type("lo.core*float"))).
escapeType("_today",funType([],type("lo.core*float"))).
escapeType("_ticks",funType([],type("lo.core*float"))).
escapeType("_time2date",predType([(inMode,type("lo.core*float")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*float")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer"))])).
escapeType("_time2utc",predType([(inMode,type("lo.core*float")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*float")),(outMode,type("lo.core*integer")),(outMode,type("lo.core*integer"))])).
escapeType("_date2time",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*float")),(inMode,type("lo.core*integer"))],type("lo.core*float"))).
escapeType("_utc2time",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*float")),(inMode,type("lo.core*integer"))],type("lo.core*float"))).
escapeType("_isCcChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isCfChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isCnChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isCoChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isCsChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isLlChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isLmChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isLoChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isLtChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isLuChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isMcChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isMeChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isMnChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isNdChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isNlChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isNoChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isPcChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isPdChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isPeChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isPfChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isPiChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isPoChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isPsChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isScChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isSkChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isSmChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isSoChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isZlChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isZpChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isZsChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_isLetterChar",predType([(inMode,type("lo.core*integer"))])).
escapeType("_digitCode",funType([(inMode,type("lo.core*integer"))],type("lo.core*integer"))).
escapeType("_int2str",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*string"))).
escapeType("_flt2str",funType([(inMode,type("lo.core*float")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*logical"))],type("lo.core*string"))).
escapeType("_int_format",funType([(inMode,type("lo.core*integer")),(inMode,type("lo.core*string"))],type("lo.core*string"))).
escapeType("_flt_format",funType([(inMode,type("lo.core*float")),(inMode,type("lo.core*string"))],type("lo.core*string"))).
escapeType("_str2flt",funType([(inMode,type("lo.core*string"))],type("lo.core*float"))).
escapeType("_str2int",funType([(inMode,type("lo.core*string"))],type("lo.core*integer"))).
escapeType("_str_lt",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*string"))])).
escapeType("_str_ge",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*string"))])).
escapeType("_str_hash",funType([(inMode,type("lo.core*string"))],type("lo.core*integer"))).
escapeType("_str_len",funType([(inMode,type("lo.core*string"))],type("lo.core*integer"))).
escapeType("_str_gen",funType([(inMode,type("lo.core*string"))],type("lo.core*string"))).
escapeType("_stringOf",univType(kVar("t"),funType([(inMode,kVar("t")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*string")))).
escapeType("_trim",funType([(inMode,type("lo.core*string")),(inMode,type("lo.core*integer"))],type("lo.core*string"))).
escapeType("explode",funType([(inMode,type("lo.core*string"))],typeExp(tpFun("lo.core*list",1),[type("lo.core*integer")]))).
escapeType("implode",funType([(inMode,typeExp(tpFun("lo.core*list",1),[type("lo.core*integer")]))],type("lo.core*string"))).
escapeType("_str_find",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*string")),(inMode,type("lo.core*integer")),(biMode,type("lo.core*integer"))])).
escapeType("_sub_str",funType([(inMode,type("lo.core*string")),(inMode,type("lo.core*integer")),(inMode,type("lo.core*integer"))],type("lo.core*string"))).
escapeType("_str_split",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*integer")),(biMode,type("lo.core*string")),(biMode,type("lo.core*string"))])).
escapeType("_str_concat",funType([(inMode,type("lo.core*string")),(inMode,type("lo.core*string"))],type("lo.core*string"))).
escapeType("_str_start",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*string"))])).
escapeType("_str_multicat",funType([(inMode,typeExp(tpFun("lo.core*list",1),[type("lo.core*string")]))],type("lo.core*string"))).
escapeType("getenv",funType([(inMode,type("lo.core*string")),(inMode,type("lo.core*string"))],type("lo.core*string"))).
escapeType("setenv",predType([(inMode,type("lo.core*string")),(inMode,type("lo.core*string"))])).
escapeType("envir",funType([],typeExp(tpFun("lo.core*list",1),[tupleType([type("lo.core*string"),type("lo.core*string")])]))).
escapeType("getlogin",funType([],type("lo.core*string"))).
escapeType("_fork",funType([(inMode,predType([]))],type("lo.thread*thread"))).
escapeType("_thread",funType([],type("lo.thread*thread"))).
escapeType("kill",predType([(inMode,type("lo.thread*thread"))])).
escapeType("thread_state",funType([(inMode,type("lo.thread*thread"))],type("lo.thread*processState"))).
escapeType("waitfor",predType([(inMode,type("lo.thread*thread"))])).
escapeType("_shell",funType([(inMode,type("lo.core*string")),(inMode,typeExp(tpFun("lo.core*list",1),[type("lo.core*string")])),(inMode,typeExp(tpFun("lo.core*list",1),[tupleType([type("lo.core*string"),type("lo.core*string")])]))],type("lo.core*integer"))).
escapeType("_newLock",funType([],type("lo.thread*lock"))).
escapeType("_acquireLock",predType([(inMode,type("lo.thread*lock")),(inMode,type("lo.core*float"))])).
escapeType("_waitLock",predType([(inMode,type("lo.thread*lock")),(inMode,type("lo.core*float"))])).
escapeType("_releaseLock",predType([(inMode,type("lo.thread*lock"))])).
escapeType("_ins_debug",predType([])).
escapeType("_stackTrace",predType([])).
isEscape("_exit",true).
isEscape("_command_line",false).
isEscape("_command_opts",false).
isEscape("_unify",false).
isEscape("_identical",false).
isEscape("var",false).
isEscape("ground",false).
isEscape("_call",true).
isEscape("_defined",false).
isEscape("_int_plus",false).
isEscape("_int_minus",false).
isEscape("_int_times",false).
isEscape("_int_div",false).
isEscape("_int_mod",false).
isEscape("_flt_plus",false).
isEscape("_flt_minus",false).
isEscape("_flt_times",false).
isEscape("_flt_div",false).
isEscape("_flt_mod",false).
isEscape("_int_abs",false).
isEscape("_flt_abs",false).
isEscape("_int_lt",false).
isEscape("_int_ge",false).
isEscape("_flt_lt",false).
isEscape("_flt_ge",false).
isEscape("_int2flt",false).
isEscape("_flt2int",false).
isEscape("_flt_hash",false).
isEscape("_pwr",false).
isEscape("sqrt",false).
isEscape("exp",false).
isEscape("log",false).
isEscape("log10",false).
isEscape("pi",false).
isEscape("sin",false).
isEscape("cos",false).
isEscape("tan",false).
isEscape("asin",false).
isEscape("acos",false).
isEscape("atan",false).
isEscape("trunc",false).
isEscape("floor",false).
isEscape("ceil",false).
isEscape("integral",false).
isEscape("srand",false).
isEscape("rand",false).
isEscape("irand",false).
isEscape("_ldexp",false).
isEscape("_frexp",false).
isEscape("_modf",false).
isEscape("_band",false).
isEscape("_bor",false).
isEscape("_bxor",false).
isEscape("_blsl",false).
isEscape("_blsr",false).
isEscape("_basr",false).
isEscape("_bnot",false).
isEscape("_nthb",false).
isEscape("_suspend",true).
isEscape("_get_file",true).
isEscape("_put_file",true).
isEscape("_cwd",true).
isEscape("_cd",true).
isEscape("_rm",true).
isEscape("_mv",true).
isEscape("_mkdir",true).
isEscape("_rmdir",true).
isEscape("_isdir",true).
isEscape("_chmod",true).
isEscape("_ls",true).
isEscape("_file_mode",true).
isEscape("_file_present",true).
isEscape("_file_type",true).
isEscape("_file_size",true).
isEscape("_file_modified",true).
isEscape("_file_date",true).
isEscape("_openInFile",true).
isEscape("_openOutFile",true).
isEscape("_openAppendFile",true).
isEscape("_openAppendIOFile",true).
isEscape("_popen",true).
isEscape("_close",true).
isEscape("_end_of_file",true).
isEscape("_ready",true).
isEscape("_inchars",true).
isEscape("_inbytes",true).
isEscape("_inchar",true).
isEscape("_inbyte",true).
isEscape("_inline",true).
isEscape("_intext",true).
isEscape("_outch",true).
isEscape("_outbyte",true).
isEscape("_outbytes",true).
isEscape("_outtext",true).
isEscape("_stdfile",true).
isEscape("_fposition",true).
isEscape("_fseek",true).
isEscape("_flush",true).
isEscape("_flushall",true).
isEscape("_setfileencoding",true).
isEscape("_install_pkg",true).
isEscape("_pkg_is_present",true).
isEscape("_logmsg",true).
isEscape("_connect",true).
isEscape("_listen",true).
isEscape("_accept",true).
isEscape("_udpPort",true).
isEscape("_udpGet",true).
isEscape("_udpSend",true).
isEscape("_udpClose",true).
isEscape("hosttoip",false).
isEscape("iptohost",false).
isEscape("_delay",true).
isEscape("_sleep",true).
isEscape("_now",true).
isEscape("_today",true).
isEscape("_ticks",true).
isEscape("_time2date",false).
isEscape("_time2utc",false).
isEscape("_date2time",false).
isEscape("_utc2time",false).
isEscape("_isCcChar",false).
isEscape("_isCfChar",false).
isEscape("_isCnChar",false).
isEscape("_isCoChar",false).
isEscape("_isCsChar",false).
isEscape("_isLlChar",false).
isEscape("_isLmChar",false).
isEscape("_isLoChar",false).
isEscape("_isLtChar",false).
isEscape("_isLuChar",false).
isEscape("_isMcChar",false).
isEscape("_isMeChar",false).
isEscape("_isMnChar",false).
isEscape("_isNdChar",false).
isEscape("_isNlChar",false).
isEscape("_isNoChar",false).
isEscape("_isPcChar",false).
isEscape("_isPdChar",false).
isEscape("_isPeChar",false).
isEscape("_isPfChar",false).
isEscape("_isPiChar",false).
isEscape("_isPoChar",false).
isEscape("_isPsChar",false).
isEscape("_isScChar",false).
isEscape("_isSkChar",false).
isEscape("_isSmChar",false).
isEscape("_isSoChar",false).
isEscape("_isZlChar",false).
isEscape("_isZpChar",false).
isEscape("_isZsChar",false).
isEscape("_isLetterChar",false).
isEscape("_digitCode",false).
isEscape("_int2str",false).
isEscape("_flt2str",false).
isEscape("_int_format",false).
isEscape("_flt_format",false).
isEscape("_str2flt",false).
isEscape("_str2int",false).
isEscape("_str_lt",false).
isEscape("_str_ge",false).
isEscape("_str_hash",false).
isEscape("_str_len",false).
isEscape("_str_gen",false).
isEscape("_stringOf",false).
isEscape("_trim",false).
isEscape("explode",false).
isEscape("implode",false).
isEscape("_str_find",false).
isEscape("_sub_str",false).
isEscape("_str_split",false).
isEscape("_str_concat",false).
isEscape("_str_start",false).
isEscape("_str_multicat",false).
isEscape("getenv",true).
isEscape("setenv",true).
isEscape("envir",false).
isEscape("getlogin",false).
isEscape("_fork",true).
isEscape("_thread",true).
isEscape("kill",true).
isEscape("thread_state",true).
isEscape("waitfor",true).
isEscape("_shell",true).
isEscape("_newLock",true).
isEscape("_acquireLock",true).
isEscape("_waitLock",true).
isEscape("_releaseLock",true).
isEscape("_ins_debug",true).
isEscape("_stackTrace",true).
