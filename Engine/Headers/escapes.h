/* 
  This is where you define a new escape function so that the compiler and
  the run-time system can see it
  Copyright (c) 2016. Francis G. McCabe

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
  except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the
  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied. See the License for the specific language governing
  permissions and limitations under the License.

 */

/* Declare standard symbols and constructors */

#define processState "t'#processState'"
#define errorType "t'#exception'"
#define threadType "t'#thread'"
#define thingType "t'#thing'"
#define fileType "t'lo.io#fileType'"
#define filePerm "t'go.io#filePerm'"
#define debugType "t'go.debug#debugger'"

/* Define the standard escapes */
escape(exit,0,True,False,"F1+iv","terminate L&O engine")
escape(_command_line,1,False,False,"F0LS","command line arguments")
escape(_command_opts,3,False,False,"F0LT2SS","command line options")

escape(_unify,4,False,False,":k't'P2?k't'?k't'","unification")
escape(_identical,5,False,False,":k't'P2+k't'+k't'","test for identicality")

escape(var,7,False,False,":k't'P1?k't'","test for variable")
escape(nonvar,8,False,False,":k't'P1?k't'","test for non-variable")

// escape(_call,11,True,False,"p4+s+s+i+LS","dynamic call")

// escape(_defined,12,True,False,"P3+s+s+i","test for defined name")

escape(_int_plus,13,False,False,"F2+i+ii","add two integers")
escape(_int_minus,14,False,False,"F2+i+ii","subtract two integers")
escape(_int_times,15,False,False,"F2+i+ii","multiply two integers")
escape(_int_div,16,False,False,"F2+i+ii","divide two integers")

escape(_flt_plus,13,False,False,"F2+f+ff","add two floats")
escape(_flt_minus,14,False,False,"F2+f+ff","subtract two floats")
escape(_flt_times,15,False,False,"F2+f+ff","multiply two floats")
escape(_flt_div,16,False,False,"F2+f+ff","divide two floats")

escape(_int_abs,24,False,False,"F1+ii","integer absolute value")
escape(_flt_abs,24,False,False,"F1+ff","float absolute value")

escape(_int_lt,27,False,False,"P2+i+i","integer less than")
escape(_int_ge,27,False,False,"P2+i+i","integer greater or equal")

escape(_flt_lt,27,False,False,"P2+f+f","float less than")
escape(_flt_ge,27,False,False,"P2+f+f","float greater or equal")

escape(_int2flt,25,False,False,"F1+if","convert integer to float")
escape(_flt2int,26,False,False,"F1+fi","convert float to integer")

escape(_flt_hash,27,False,False,"F1+fi","compute hash of float")

escape(_pwr,25,False,False,"F2+f+ff","raise X to the power Y")

/*  escape(sqrt,26,False,False,":k'u'NF1+k'u'f","square root")
  escape(exp,27,False,False,":k'u'NF1+k'u'k'u'","exponential")
  escape(log,28,False,False,":k'u'NF1+k'u'f","logarithm")
  escape(log10,29,False,False,":k'u'NF1+k'u'f","10-based logarithm")
  escape(pi,30,False,False,"F0f","return PI")
  escape(sin,31,False,False,":k'u'NF1+k'u'f","sine")
  escape(cos,32,False,False,":k'u'NF1+k'u'f","cosine")
  escape(tan,33,False,False,":k'u'NF1+k'u'f","tangent")
  escape(asin,34,False,False,":k'u'NF1+k'u'f","arc sine")
  escape(acos,35,False,False,":k'u'NF1+k'u'f","arc cosine")
  escape(atan,36,False,False,":k'u'NF1+k'u'f","arc tangent")

  escape(srand,37,False,False,":1Np1+k'u'","set random seed")
  escape(rand,38,False,False,":k'u'NF1+k'u'f","random # generator")
  escape(irand,39,False,False,"F1+ii","generate random integer")

  escape(ldexp,40,False,False,"F2+N+Nf","raise x to 2**y")
  escape(frexp,41,False,False,"P3+f-f-i","split x into mant and exp")
  escape(modf,42,False,False,"P3+f-f-f","split x into int and frac")
*/
  escape(_band,43,False,False,"F2+i+ii","bitwise and two integers")
  escape(_bor,44,False,False,"F2+i+ii","bitwise or two integers")
  escape(_bxor,45,False,False,"F2+i+ii","bitwise xor two integers")
  escape(_blsl,46,False,False,"F2+i+ii","logical left shift")
  escape(_blsr,47,False,False,"F2+i+ii","logical right shift")
  escape(_basr,47,False,False,"F2+i+ii","arithmetic right shift")
  escape(_bnot,48,False,False,"F1+ii","bitwise negate number")
  escape(_nthb,49,False,False,"P2+i+i","is nth bit set?")

/*
  escape(trunc,49,False,False,":k'u'NF1+k'u'k'u'","truncate to nearest integer")
  escape(floor,50,False,False,":k'u'NF1+k'u'k'u'","truncate to lower integer")
  escape(ceil,51,False,False,"F1+Ni","truncate to next integer")
  escape(itrunc,52,False,False,"F1+Ni","truncate to 64 bit integer")
  escape(integral,53,False,False,"P1+N","test if number is integral")
  escape(n2float,54,False,False,":k'u'NF1+k'u'f","float a number")
  
  escape(ground,59,False,False,":k't'AP1+k't'","test for grounded")

  escape(_suspend,64,False,False,":k'u'A:k'v'AP2+k'u'+k'v'","suspend if variable not bound")

  escape(_assert,70,False,False,":k'u'Ap2+s+k'u'","assert a term")
  escape(_retract,71,False,False,"p1+s","remove assertion")

  escape(_term,75,False,False,":k'u'AF1+k'u's","define an assertion")
  escape(_is,76,False,False,":k'u'AP2+s+k'u'","invoke an assertion")
  escape(_remove,77,False,False,"p1+s","retract a definition")

  // Create a new object -- clone a term to make an object
  escape(_newObject,80,False,False,":k'u'AF1+k'u'k'u'","create a new object")

  // Property management
 // escape(_setProp,81,False,False,":k'u'AP3+"thingType"+s+k'u'","set a property on a symbol")
 // escape(_getProp,82,False,False,":k'u'AP3+"thingType"+s-k'u'","get a symbol property")
 // escape(_delProp,83,False,False,":k'u'AP2+"thingType"+s","delete a property from a symbol")

  // Term construction
  escape(_univ,84,False,False,":k'u'A:k'v'AF2+s+Lk'u'k'v'","weird function to construct terms")  

  // Lock management
  escape(_acquireLock,91,False,False,":k't'AP2+k't'+N","acquire lock")
  escape(_waitLock,92,False,False,":k't'AP2+k't'+N","release and wait on a lock")
  escape(_releaseLock,93,False,False,":k't'AP1+k't'","release a lock")

  // Sha function
  escape(_sha1,102,False,False,"F1+LiLi","compute hash of a byte string")

  escape(_openURL,110,True,False,"F4+S+S+S+iO","open a URL")
*/
  escape(_readFileContents,111,True,False,"F1+SLi","Get the contents of a file as a list of integer codes")
  escape(_writeFileContents,112,True,False,"P2+S+Li","write a file from a list of integer codes")
  escape(_getCwd,113,True,False,"F0S","return url of current working directory")
/*

  escape(_openInFile,111,True,False,"F2+S+iO","open input file")
  escape(_openOutFile,112,True,False,"F2+S+iO","open output file")
  escape(_openAppendFile,113,True,False,"F2+S+iO","open output file")
  escape(_openAppendIOFile,114,True,False,"F2+S+iO","open output file")
  escape(_checkRoot,115,True,False,"P2+S+S","check url against root URL")
  escape(_mergeURL,116,True,False,"F2+S+SS","merge URLs")
  escape(_createURL,117,True,False,"F4+S+S+S+iO","create a URL")
  escape(_popen,118,True,False,"p7+S+LS+LT2sS+O+O+O+i","open a pipe")
  
  escape(_close,119,True,False,"p1+O","close file")
  escape(_eof,120,True,False,"P1+O","end of file test")
  escape(_ready,121,True,False,"P1+O","file ready test")
  escape(_inchars,122,True,False,"F2+O+iS","read block string")
  escape(_inbytes,123,True,False,"F2+O+iLi","read block of bytes")
  escape(_inchar,124,True,False,"F1+Oc","read single character")
  escape(_inbyte,125,True,False,"F1+Oi","read single byte")
  escape(_inline,126,True,False,"F2+O+SS","read a line")
  escape(_intext,127,True,False,"F2+O+SS","read until matching character")
  escape(_outch,128,True,False,"p2+O+i","write a single character")
  escape(_outbyte,129,True,False,"p2+O+i","write a single byte")
  escape(_outtext,130,True,False,"p2+O+S","write a string as a block")
  escape(_outsym,131,True,False,"p2+O+s","write a symbol")
  escape(_stdfile,132,True,False,"F1+iO","standard file descriptor")
  escape(_fposition,135,True,False,"F1+Oi","report current file position")
  escape(_fseek,136,True,False,"p2+O+i","seek to new file position")
  escape(_flush,137,True,False,"p1+O","flush the I/O buffer")
  escape(_flushall,138,True,False,"p0","flush all files")
  escape(_setfileencoding,140,True,False,"p2+O+i", "set file encoding on file")
    
  escape(_classload,141,True,False,"p4+S+s+s-Ls","load class file")

*/
  escape(_display,141,False,False,"P2+T3iii+S","Display a term on console")
  escape(_logmsg,142,False,False,"P1+S","log a message in logfile or console")
/*  
  // Socket handling functions
  escape(_connect,143,True,False,"p5+S+i+i-O-O","connect to remote host")
  escape(_listen,144,True,False,"p2+i-O","listen on a port")
  escape(_accept,145,True,False,"p7+O-O-O-S-S-i+i","accept connection")
  //  escape(_udpPort,146,True,False,"p2NO","estabish a UDP port")
  //  escape(_udpGet,147,True,False,"p4OSSN","read a UDP datagram")
  //  escape(_udpSend,148,True,False,"p4OSSN","send a UDP datagram")
  escape(hosttoip,150,False,False,"F1+SLS","IP address of host")
  escape(iptohost,151,False,False,"F1+SS","host name from IP")
  
  escape(_cwd,152,False,False,"F0S","return current working directory")
  escape(_cd,153,False,False,"p1+S","change current working directory")
  escape(_rm,154,True,False,"p1+S","remove file")
  escape(_mv,155,True,False,"p2+S+S","rename file")
  escape(_mkdir,156,True,False,"p2+S+i","create directory")
  escape(_rmdir,157,True,False,"p1+S","delete directory")
  escape(_chmod,158,True,False,"p2+S+i","change mode of a file or directory")
  escape(_ls,159,True,False,"F1+SLT2S"fileType"","report on contents of a directory")
  escape(_file_present,160,True,False,"P1+S","test for file")
  escape(_file_mode,161,True,False,"F1+Si","report modes of a file")
  escape(_file_type,162,True,False,"F1+S"fileType,"report on the type of a file")
  escape(_file_size,163,True,False,"F1+Si","report on the size of a file")
  escape(_file_date,164,True,False,"p4+S-f-f-f","report on file access time and modification times")

// Timing and delaying
  escape(delay,170,False,False,"p1+N","delay for period of time")  
  escape(sleep,171,False,False,"p1+N","sleep until a definite time")  
  escape(now,172,False,False,"F0f","current time")  
  escape(today,173,False,False,"F0i","time at midnight")  
  escape(ticks,174,False,False,"F0f","used CPU time")
  escape(_time2date,175,False,False,"P012+N-i-i-i-i-i-i-f-f-S", "convert a time to a date")
  escape(_time2utc,176,False,False,"P012+N-i-i-i-i-i-i-f-f-S", "convert a time to UTC date")
  escape(_date2time,177,False,False,"F007+i+i+i+i+i+N+Nf", "convert a date to a time")
  escape(_utc2time,178,False,False,"F010+N+N+N+N+N+N+N+Nf", "convert a UTC date to a time")

*/
 // Character class escapes

  escape(_isCcChar,180,False,False,"P1+i","is Other, control char")
  escape(_isCfChar,181,False,False,"P1+i","is Other, format char")
  escape(_isCnChar,182,False,False,"P1+i","is Other, unassigned char")
  escape(_isCoChar,183,False,False,"P1+i","is Other, private char")
  escape(_isCsChar,184,False,False,"P1+i","is Other, surrogate char")
  escape(_isLlChar,185,False,False,"P1+i","is Letter, lowercase char")
  escape(_isLmChar,186,False,False,"P1+i","is Letter, modifier char")
  escape(_isLoChar,187,False,False,"P1+i","is Letter, other char")
  escape(_isLtChar,188,False,False,"P1+i","is Letter, title char")
  escape(_isLuChar,189,False,False,"P1+i","is Letter, uppercase char")
  escape(_isMcChar,190,False,False,"P1+i","is Mark, spacing char")
  escape(_isMeChar,191,False,False,"P1+i","is Mark, enclosing char")
  escape(_isMnChar,192,False,False,"P1+i","is Mark, nonspacing char")
  escape(_isNdChar,193,False,False,"P1+i","is Number, decimal digit")
  escape(_isNlChar,194,False,False,"P1+i","is Number, letter char")
  escape(_isNoChar,195,False,False,"P1+i","is Number, other char")
  escape(_isPcChar,196,False,False,"P1+i","is Punctuation, connector")
  escape(_isPdChar,197,False,False,"P1+i","is Punctuation, dash char")
  escape(_isPeChar,198,False,False,"P1+i","is Punctuation, close char")
  escape(_isPfChar,199,False,False,"P1+i","is Punctuation, final quote")
  escape(_isPiChar,200,False,False,"P1+i","is Punctuation, initial quote")
  escape(_isPoChar,201,False,False,"P1+i","is Punctuation, other char")
  escape(_isPsChar,202,False,False,"P1+i","is Punctuation, open char")
  escape(_isScChar,203,False,False,"P1+i","is Symbol, currency char")
  escape(_isSkChar,204,False,False,"P1+i","is Symbol, modifier char")
  escape(_isSmChar,205,False,False,"P1+i","is Symbol, math char")
  escape(_isSoChar,206,False,False,"P1+i","is Symbol, other char")
  escape(_isZlChar,207,False,False,"P1+i","is Separator, line char")
  escape(_isZpChar,208,False,False,"P1+i","is Separator, para char")
  escape(_isZsChar,209,False,False,"P1+i","is Separator, space char")

  escape(_isLetterChar,210,False,False,"P1+i","is letter char")
  escape(_digitCode,211,False,False,"F1+ii","convert char to num")

// String and symbol handling escapes
  escape(_int2str,214,False,False,"F4+i+i+i+iS","format an integer as a string")
  escape(_flt2str,215,False,False,"F5+f+i+i+l+lS","format a floating as a string")

  escape(_str_lt,216,False,False,"P2+S+S","String 1 is less than string 2")
  escape(_str_ge,217,False,False,"P2+S+S","String 1 is greater than or equals to string 2")

  escape(_str_hash,218,False,False,"F1+Si","Compute hash of string")

/*
  escape(_stringOf,216,False,False,":k't'AF3+k't'+i+iS","convert value to a string")
  escape(_trim,217,False,False,"F2+S+iS","trim a string to a width")
*/
  escape(explode,218,False,False,"F1+SLi","convert string to list of code points")
  escape(implode,219,False,False,"F1+LiS","convert list of code points to string")
/*
  escape(getenv,230,False,False,"F2+S+SS","get an environment variable")
  escape(setenv,231,True,False,"P2+S+S","set an environment variable")
  escape(envir,232,False,False,"F0LT2SS","return entire environment")
  escape(getlogin,233,False,False,"F0S","return user's login")

/ Process manipulation
  escape(_fork,240,False,False,"p1+"threadType,"fork new process")
  escape(_thread,241,False,False,"F0"threadType"","report thread of current process")
  escape(kill,242,True,False,"p1+"threadType ,"kill off a process")
  escape(thread_state,243,False,False,"F1+"threadType processState"","state of process")
  escape(waitfor,244,False,False,"p1+"threadType,"wait for other thread to terminate")
  escape(_assoc,246,False,False,":k't'AP2+k't'+p0","associate a goal with a var")
  escape(_shell,247,True,False,"p4+S+LS+LT2SS-i","Run a shell cmd")

  escape(_ins_debug,254,False,False,"p0","set instruction-level")
  escape(_stackTrace,255,False,False,"p0","Print a stack trace")
*/

#undef processState
#undef errorType
#undef threadType
#undef thingType
#undef fileType
#undef filePerm
#undef debugType
