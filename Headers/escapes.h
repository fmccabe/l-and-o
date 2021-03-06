/*
  This is where you define a new escape function so that the compiler and
  the run-time system can see it
  Copyright (c) 2016, 2017. Francis G. McCabe

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
  except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the
  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied. See the License for the specific language governing
  permissions and limitations under the License.

 */

/* Declare standard types used in escapes */

#define processState "t'lo.thread*processState'"
#define threadType "t'lo.thread*thread'"
#define lockType "t'lo.thread*lock'"
#define fileType "t'lo.io*fileHandle'"
#define udpType "t'lo.io*udpHandle'"

/* Define the standard escapes */
escape(_exit,True,False,"P1+i","terminate L&O engine")
escape(_command_line,False,False,"F0LS","command line arguments")
escape(_command_opts,False,False,"F0LT2SS","command line options")

escape(_unify,False,False,":k't'P2?k't'?k't'","unification")
escape(_identical,False,False,":k't'P2?k't'?k't'","test for identicality")

escape(var,False,False,":k't'P1?k't'","test for variable")
escape(ground,False,False,":k't'P1?k't'","test for grounded-ness")

escape(_call,True,False,"P3+S+i+LLS","dynamic call")
escape(_defined,False,False,"P2+S+i","test for defined name")

escape(_int_plus,False,False,"F2+i+ii","add two integers")
escape(_int_minus,False,False,"F2+i+ii","subtract two integers")
escape(_int_times,False,False,"F2+i+ii","multiply two integers")
escape(_int_div,False,False,"F2+i+ii","divide two integers")
escape(_int_mod,False,False,"F2+i+ii","modulo remainder")

escape(_flt_plus,False,False,"F2+f+ff","add two floats")
escape(_flt_minus,False,False,"F2+f+ff","subtract two floats")
escape(_flt_times,False,False,"F2+f+ff","multiply two floats")
escape(_flt_div,False,False,"F2+f+ff","divide two floats")
escape(_flt_mod,False,False,"F2+f+ff","modulo remainder")

escape(_int_abs,False,False,"F1+ii","integer absolute value")
escape(_flt_abs,False,False,"F1+ff","float absolute value")

escape(_int_lt,False,False,"P2+i+i","integer less than")
escape(_int_ge,False,False,"P2+i+i","integer greater or equal")

escape(_flt_lt,False,False,"P2+f+f","float less than")
escape(_flt_ge,False,False,"P2+f+f","float greater or equal")

escape(_int2flt,False,False,"F1+if","convert integer to float")
escape(_flt2int,False,False,"F1+fi","convert float to integer")

escape(_flt_hash,False,False,"F1+fi","compute hash of float")

escape(_pwr,False,False,"F2+f+ff","raise X to the power Y")

escape(sqrt,False,False,"F1+ff","square root")
escape(exp,False,False,"F1+ff","exponential")
escape(log,False,False,"F1+ff","logarithm")
escape(log10,False,False,"F1+ff","10-based logarithm")
escape(pi,False,False,"F0f","return PI")
escape(sin,False,False,"F1+ff","sine")
escape(cos,False,False,"F1+ff","cosine")
escape(tan,False,False,"F1+ff","tangent")
escape(asin,False,False,"F1+ff","arc sine")
escape(acos,False,False,"F1+ff","arc cosine")
escape(atan,False,False,"F1+ff","arc tangent")

escape(trunc,False,False,"F1+ff","truncate to nearest integer")
escape(floor,False,False,"F1+ff","truncate to lower integer")
escape(ceil,False,False,"F1+ff","truncate to next integer")
escape(integral,False,False,"P1+f","test if number is integral")

escape(srand,False,False,"P1+f","set random seed")
escape(rand,False,False,"F0f","random # generator")
escape(irand,False,False,"F1+ii","generate random integer")

escape(_ldexp,False,False,"F2+f+ff","raise x to 2**y")
escape(_frexp,False,False,"P3+f-f-i","split x into mant and exp")
escape(_modf,False,False,"P3+f-f-f","split x into int and frac")

escape(_band,False,False,"F2+i+ii","bitwise and two integers")
escape(_bor,False,False,"F2+i+ii","bitwise or two integers")
escape(_bxor,False,False,"F2+i+ii","bitwise xor two integers")
escape(_blsl,False,False,"F2+i+ii","logical left shift")
escape(_blsr,False,False,"F2+i+ii","logical right shift")
escape(_basr,False,False,"F2+i+ii","arithmetic right shift")
escape(_bnot,False,False,"F1+ii","bitwise negate number")
escape(_nthb,False,False,"P2+i+i","is nth bit set?")

escape(_suspend,True,False,":k'u'P2?k'u'+P0","suspend handler if variable not bound")

/*

  escape(_assert,False,False,":k'u'Ap2sk'u'","assert a term")
  escape(_retract,False,False,"p1s","remove assertion")

  escape(_term,False,False,":k'u'AF1k'u's","define an assertion")
  escape(_is,False,False,":k'u'AP2sk'u'","invoke an assertion")
  escape(_remove,False,False,"p1s","retract a definition")

  // Create a new object -- clone a term to make an object
  escape(_newObject,False,False,":k'u'AF1k'u'k'u'","create a new object")

  // Term construction
  escape(_univ,False,False,":k'u'A:k'v'AF2sLk'u'k'v'","weird function to construct terms")
*/

  escape(_get_file,True,False,"F1+SS","Get the contents of a file as a string")
  escape(_put_file,True,False,"P2+S+S","write a file from a string")
  escape(_cwd,True,False,"F0S","return url of current working directory")
  escape(_cd,True,False,"P1+S","change current working directory")
  escape(_rm,True,False,"P1+S","remove file")
  escape(_mv,True,False,"P2+S+S","rename file")
  escape(_mkdir,True,False,"P2+S+i","create directory")
  escape(_rmdir,True,False,"P1+S","delete directory")
  escape(_isdir,True,False,"P1+S","is directory present")
  escape(_chmod,True,False,"P2+S+i","change mode of a file or directory")
  escape(_ls,True,False,"F1+SLS","return a list of files in a directory")

  escape(_file_mode,True,False,"F1+Si","report modes of a file")
  escape(_file_present,True,False,"P1+S","check presence of a file")
  escape(_file_type,True,False,"F1+Si","report on the type of a file")
  escape(_file_size,True,False,"F1+Si","report on the size of a file")
  escape(_file_modified,True,False,"F1+Si","report on when a file was last modified")
  escape(_file_date,True,False,"P4+S-i-i-i","report on file access time and modification times")

  escape(_openInFile,True,False,"F2+S+i"fileType,"open input file")
  escape(_openOutFile,True,False,"F2+S+i"fileType,"open output file")
  escape(_openAppendFile,True,False,"F2+S+i"fileType,"open output file")
  escape(_openAppendIOFile,True,False,"F2+S+i"fileType,"open output file")

  escape(_popen,True,False,"P6+S+LS+LT2SS+"fileType "+" fileType "+" fileType,"open a pipe")

  escape(_close,True,False,"P1+"fileType,"close file")
  escape(_end_of_file,True,False,"P1+"fileType,"end of file test")
  escape(_ready,True,False,"P1+"fileType,"file ready test")
  escape(_inchars,True,False,"F2+"fileType"+iS","read block string")
  escape(_inbytes,True,False,"F2+"fileType"+iLi","read block of bytes")
  escape(_inchar,True,False,"F1+"fileType"i","read single character")
  escape(_inbyte,True,False,"F1+"fileType"i","read single byte")
  escape(_inline,True,False,"F1+"fileType"S","read a line")
  escape(_intext,True,False,"F2+"fileType"+SS","read until matching character")
  escape(_outch,True,False,"P2+"fileType"+i","write a single character")
  escape(_outbyte,True,False,"P2+"fileType"+i","write a single byte")
  escape(_outbytes,True,False,"P2+"fileType"+Li","write a list of bytes")
  escape(_outtext,True,False,"P2+"fileType"+S","write a string as a block")
  escape(_stdfile,True,False,"F1+i"fileType,"standard file descriptor")
  escape(_fposition,True,False,"F1+"fileType"i","report current file position")
  escape(_fseek,True,False,"P2+"fileType"+i","seek to new file position")
  escape(_flush,True,False,"P1+"fileType,"flush the I/O buffer")
  escape(_flushall,True,False,"P0","flush all files")
  escape(_setfileencoding,True,False,"P2+"fileType"+i", "set file encoding on file")

  escape(_install_pkg,True,False,"F1+SLT2SS","define package from string contents")
  escape(_pkg_is_present,True,False,"P4+S+S-S-S","True if an identified resource is available")

  escape(_logmsg,True,False,"P1+S","log a message in logfile or console")

  // Socket handling functions
  escape(_connect,True,False,"P5+S+i+i+"fileType "+" fileType,"connect to remote host")
  escape(_listen,True,False,"P2+i+"fileType,"listen on a port")
  escape(_accept,True,False,"P6+i+"fileType "+" fileType "+S+i+S","accept connection")

  escape(_udpPort,True,False,"P2+i+"udpType,"estabish a UDP port")
  escape(_udpGet,True,False,"P4+"udpType"+S-S-i","read a UDP datagram")
  escape(_udpSend,True,False,"P4+"udpType"+S-S-i","send a UDP datagram")
  escape(_udpClose,True,False,"P1+"udpType,"close the UDP socket")

  escape(hosttoip,False,False,"F1+SLS","IP address of host")
  escape(iptohost,False,False,"F1+SS","host name from IP")

// Timing and delaying
  escape(_delay,True,False,"P1+f","delay for period of time")
  escape(_sleep,True,False,"P1+f","sleep until a definite time")
  escape(_now,True,False,"F0f","current time")
  escape(_today,True,False,"F0f","time at midnight")
  escape(_ticks,True,False,"F0f","used CPU time")
  escape(_time2date,False,False,"P10+f-i-i-i-i-i-i-f-i-i", "convert a time to a date")
  escape(_time2utc,False,False, "P10+f-i-i-i-i-i-i-f-i-i", "convert a time to UTC date")
  escape(_date2time,False,False,"F7+i+i+i+i+i+f+if", "convert a date to a time")
  escape(_utc2time,False,False,"F7+i+i+i+i+i+f+if", "convert a UTC date to a time")

 // Character class escapes

  escape(_isCcChar,False,False,"P1+i","is Other, control char")
  escape(_isCfChar,False,False,"P1+i","is Other, format char")
  escape(_isCnChar,False,False,"P1+i","is Other, unassigned char")
  escape(_isCoChar,False,False,"P1+i","is Other, private char")
  escape(_isCsChar,False,False,"P1+i","is Other, surrogate char")
  escape(_isLlChar,False,False,"P1+i","is Letter, lowercase char")
  escape(_isLmChar,False,False,"P1+i","is Letter, modifier char")
  escape(_isLoChar,False,False,"P1+i","is Letter, other char")
  escape(_isLtChar,False,False,"P1+i","is Letter, title char")
  escape(_isLuChar,False,False,"P1+i","is Letter, uppercase char")
  escape(_isMcChar,False,False,"P1+i","is Mark, spacing char")
  escape(_isMeChar,False,False,"P1+i","is Mark, enclosing char")
  escape(_isMnChar,False,False,"P1+i","is Mark, nonspacing char")
  escape(_isNdChar,False,False,"P1+i","is Number, decimal digit")
  escape(_isNlChar,False,False,"P1+i","is Number, letter char")
  escape(_isNoChar,False,False,"P1+i","is Number, other char")
  escape(_isPcChar,False,False,"P1+i","is Punctuation, connector")
  escape(_isPdChar,False,False,"P1+i","is Punctuation, dash char")
  escape(_isPeChar,False,False,"P1+i","is Punctuation, close char")
  escape(_isPfChar,False,False,"P1+i","is Punctuation, final quote")
  escape(_isPiChar,False,False,"P1+i","is Punctuation, initial quote")
  escape(_isPoChar,False,False,"P1+i","is Punctuation, other char")
  escape(_isPsChar,False,False,"P1+i","is Punctuation, open char")
  escape(_isScChar,False,False,"P1+i","is Symbol, currency char")
  escape(_isSkChar,False,False,"P1+i","is Symbol, modifier char")
  escape(_isSmChar,False,False,"P1+i","is Symbol, math char")
  escape(_isSoChar,False,False,"P1+i","is Symbol, other char")
  escape(_isZlChar,False,False,"P1+i","is Separator, line char")
  escape(_isZpChar,False,False,"P1+i","is Separator, para char")
  escape(_isZsChar,False,False,"P1+i","is Separator, space char")

  escape(_isLetterChar,False,False,"P1+i","is letter char")
  escape(_digitCode,False,False,"F1+ii","convert char to num")

// String handling escapes
  escape(_int2str,False,False,"F4+i+i+i+iS","format an integer as a string")
  escape(_flt2str,False,False,"F5+f+i+i+i+lS","format a floating as a string")
  escape(_int_format,False,False,"F2+i+SS","format an integer using picture format")
  escape(_flt_format,False,False,"F2+f+SS","format a floating point using picture format")

  escape(_str2flt,False,False,"F1+Sf","parse a string as a float")
  escape(_str2int,False,False,"F1+Si","parse a string as an integer")

  escape(_str_lt,False,False,"P2+S+S","String 1 is less than string 2")
  escape(_str_ge,False,False,"P2+S+S","String 1 is greater than or equals to string 2")

  escape(_str_hash,False,False,"F1+Si","Compute hash of string")
  escape(_str_len,False,False,"F1+Si","return length of string")

  escape(_str_gen,False,False,"F1+SS","Generate a unique string")

  escape(_stringOf,False,False,":k't'F3+k't'+i+iS","Display a general term")
  escape(_trim,False,False,"F2+S+iS","trim a string to a width")

  escape(explode,False,False,"F1+SLi","convert string to list of code points")
  escape(implode,False,False,"F1+LiS","convert list of code points to string")

  escape(_str_find,False,False,"P4+S+S+i?i","find a substring in string")
  escape(_sub_str,False,False,"F3+S+i+iS","extract a substring")
  escape(_str_split,False,False,"P4+S+i?S?S","split a string at a point")
  escape(_str_concat,False,False,"F2+S+SS","Concatenate two strings")
  escape(_str_start,False,False,"P2+S+S","True if second string starts with first")
  escape(_str_multicat,False,False,"F1+LSS","Concatenate a list of strings into one")

  escape(getenv,True,False,"F2+S+SS","get an environment variable")
  escape(setenv,True,False,"P2+S+S","set an environment variable")
  escape(envir,False,False,"F0LT2SS","return entire environment")
  escape(getlogin,False,False,"F0S","return user's login")

// Process manipulation
  escape(_fork,True,False,"F1+P0"threadType,"fork new process")
  escape(_thread,True,False,"F0"threadType"","report thread of current process")
  escape(kill,True,False,"P1+"threadType ,"kill off a process")
  escape(thread_state,True,False,"F1+"threadType processState,"state of process")
  escape(waitfor,True,False,"P1+"threadType,"wait for other thread to terminate")

  escape(_shell,True,False,"F3+S+LS+LT2SSi","Run a shell cmd")

 // Lock management
  escape(_newLock,True,False,"F0"lockType,"create a new lock")
  escape(_acquireLock,True,False,"P2+"lockType"+f","acquire lock")
  escape(_waitLock,True,False,"P2+"lockType"+f","release and wait on a lock")
  escape(_releaseLock,True,False,"P1+"lockType,"release a lock")

  escape(_ins_debug,True,False,"P0","set instruction-level")
  escape(_stackTrace,True,False,"P0","Print a stack trace")

#undef processState
#undef threadType
#undef fileType
#undef lockType
#undef udpType
