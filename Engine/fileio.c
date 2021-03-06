/* 
  File I/O functions
  Copyright (c) 2016, 2017. Francis G. McCabe

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
  except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the
  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied. See the License for the specific language governing
  permissions and limitations under the License.
*/

#include <string.h>
#include <stdlib.h>

#include <unistd.h>

#include <sys/stat.h>
#include <limits.h>

#include "lo.h"
#include "fileio.h"
#include "term.h"
#include "formioP.h"
#include "pipe.h"
#include "tpl.h"

ptrI filePtrClass;              // The file class structure

static retCode cellMsg(ioPo f, void *data, long depth, long precision, logical alt);
static retCode stringMsg(ioPo f, void *data, long depth, long precision, logical alt);

static void clearFilePointer(ptrI p);

static long flSizeFun(specialClassPo class, objPo o);
static comparison flCompFun(specialClassPo class, objPo o1, objPo o2);
static retCode flOutFun(specialClassPo class, ioPo out, objPo o);
static retCode flScanFun(specialClassPo class, specialHelperFun helper, void *c, objPo o);
static objPo flCopyFun(specialClassPo class, objPo dst, objPo src);
static uinteger flHashFun(specialClassPo class, objPo o);

typedef struct {
  ptrI class; // == filePtrClass
  ioPo file;
} fileRec, *fPo;

void initFileIo(void) {
  installMsgProc('w', cellMsg);  /* extend outMsg to cope with cell structures */
  installMsgProc('S', stringMsg); // Special function for writing string values
}

void initFiles(void) {
  filePtrClass = newSpecialClass("lo.core#file", flSizeFun, flCompFun,
                                 flOutFun, flCopyFun, flScanFun, flHashFun);
}

static long flSizeFun(specialClassPo class, objPo o) {
  return CellCount(sizeof(fileRec));
}

static comparison flCompFun(specialClassPo class, objPo o1, objPo o2) {
  if (o1->class == filePtrClass && o2->class == filePtrClass) {
    fPo f1 = (fPo) o1;
    fPo f2 = (fPo) o2;

    if (f1 == f2)
      return same;
    else
      return incomparible;
  } else
    return incomparible;
}

static retCode flOutFun(specialClassPo class, ioPo out, objPo o) {
  assert(o->class == filePtrClass);

  fPo f = (fPo) o;

  return outMsg(out, "%U[%x]", fileName(f->file), f->file);
}

static retCode flScanFun(specialClassPo class, specialHelperFun helper, void *c, objPo o) {
  return Ok;
}

static objPo flCopyFun(specialClassPo class, objPo dst, objPo src) {
  long size = flSizeFun(class, src);
  memmove((void *) dst, (void *) src, size * sizeof(ptrI));

  return (objPo) (((ptrPo) dst) + size);
}

static uinteger flHashFun(specialClassPo class, objPo o) {
  assert(o->class == filePtrClass);

  return (uinteger) ((PTRINT) (((fPo) o)->file));
}

/* check the permission bits on a file */
retCode filePerms(char *file, unsigned long *mode) {
  struct stat buf;

  if (stat(file, &buf) == -1)
    return Error;
  else {
    if (mode != NULL)
      *mode = buf.st_mode;
    return Ok;
  }
}

/* return True if a file is really an executable program or not.
   Not checked out for windows at this time
   */
logical executableFile(char *file) {
  uid_t id = geteuid();
  gid_t grp = getegid();

  struct stat buf;

  if (stat(file, &buf) == -1 || !S_ISREG(buf.st_mode))
    return False;

  if (buf.st_mode & S_IXOTH)  /* anyone can execute this file */
    return True;
  if (buf.st_mode & S_IXGRP) {
    if (buf.st_gid == grp)
      return True;
    else {
      int ngroups = getgroups(0, NULL); /* The number of supplementary groups */

      if (ngroups > 0) {
        gid_t *groups = (gid_t *) calloc((size_t) ngroups, sizeof(gid_t));
        int i;

        getgroups(ngroups, groups);

        for (i = 0; i < ngroups; i++)
          if (groups[i] == buf.st_gid) {
            free(groups);
            return True;
          }

        free(groups);
      }
    }
  }
  if (buf.st_mode & S_IXUSR)
    return (logical) (id == buf.st_uid);
  return False;
}

ioEncoding pickEncoding(ptrI k) {
  switch (integerVal(intV(k))) {
    case 0:return rawEncoding;
    case 3:return utf8Encoding;
    default:return unknownEncoding;
  }
}

/*
 * openInFile(fd,fm,encoding)
 * 
 * openInFile a file to read
 */

retCode g__openInFile(processPo P, ptrPo a) {
  switchProcessState(P, wait_io);

  ptrI t1 = deRefI(&a[1]);

  if (!IsString(t1))
    return liberror(P, "__openInFile", eSTRNEEDD);
  else if (isvar(deRefI(&a[2])))
    return liberror(P, "__openInFile", eINVAL);

  ioPo file = openInFile(stringVal(stringV(t1)), pickEncoding(deRefI(&a[2])));

  ptrI t2 = allocFilePtr(file);    /* return open file descriptor */
  setProcessRunnable(P);

  if (file == NULL)
    return liberror(P, "__openInFile", eNOFILE);

  return equal(P, &t2, &a[3]);
}

/*
 * openOutFile(fd,fm,enc)
 * 
 * openOutFile a file to write to
 */

retCode g__openOutFile(processPo P, ptrPo a) {
  switchProcessState(P, wait_io);

  ptrI t1 = deRefI(&a[1]);

  if (!IsString(t1))
    return liberror(P, "__openOutFile", eSTRNEEDD);
  else if (isvar(deRefI(&a[2])))
    return liberror(P, "__openOutFile", eINVAL);

  ioPo file = newOutFile(stringVal(stringV(t1)), pickEncoding(deRefI(&a[2])));
  setProcessRunnable(P);

  if (file == NULL)
    return liberror(P, "__openOutFile", eNOFILE);

  ptrI t2 = allocFilePtr(file);    /* return open file descriptor */
  return equal(P, &t2, &a[3]);
}

/*
 * openAppenFile(fd,fm)
 * 
 * openAppendFile a file to append to 
 */

retCode g__openAppendFile(processPo P, ptrPo a) {
  switchProcessState(P, wait_io);

  ptrI t1 = deRefI(&a[1]);

  if (!IsString(t1))
    return liberror(P, "__openAppendFile", eSTRNEEDD);
  else if (isvar(deRefI(&a[2])))
    return liberror(P, "__openAppendFile", eINVAL);

  ioPo file = openAppendFile(stringVal(stringV(t1)), pickEncoding(deRefI(&a[2])));
  setProcessRunnable(P);

  if (file == NULL)
    return liberror(P, "__openAppendFile", eNOFILE);

  ptrI t2 = allocFilePtr(file);    /* return open file descriptor */
  return equal(P, &t2, &a[3]);
}

/*
 * openAppendIOFile(fd,fm)
 * 
 * openAppendIOFile a file to read/append to 
 */

retCode g__openAppendIOFile(processPo P, ptrPo a) {
  switchProcessState(P, wait_io);

  ptrI t1 = deRefI(&a[1]);
  ioPo file = NULL;

  if (!IsString(t1))
    return liberror(P, "__openAppendIOFile", eSTRNEEDD);
  else if (isvar(deRefI(&a[2])))
    return liberror(P, "__openAppendIOFile", eINVAL);

  file = openInOutAppendFile(stringVal(stringV(t1)), pickEncoding(deRefI(&a[2])));
  setProcessRunnable(P);

  if (file == NULL)
    return liberror(P, "__openAppendIOFile", eNOFILE);

  ptrI t2 = allocFilePtr(file); /* return open file descriptor */
  return equal(P, &t2, &a[3]);
}


// Open a file and return the file pointers associated with the input and the output channels

retCode g__popen(processPo P, ptrPo a) {
  switchProcessState(P, wait_io);

  ptrI Cmd = deRefI(&a[1]);
  ptrI Ags = deRefI(&a[2]);
  ptrI Env = deRefI(&a[3]);
  long argCount = ListLen(Ags);
  long envCount = ListLen(Env);

  if (!IsString(Cmd))
    return liberror(P, "_popen", eSTRNEEDD);
  else if (isvar(Ags) || argCount < 0 || isvar(Env) || envCount < 0)
    return liberror(P, "_popen", eINSUFARG);
  else {
    long len = stringLen(stringV(Cmd)) + 1;
    char cmd[3 * len];
    ioPo inPipe, outPipe, errPipe;
    strncpy(cmd,  stringVal(stringV(Cmd)), NumberOf(cmd));

    if (access(cmd, F_OK | R_OK | X_OK) != 0) {
      setProcessRunnable(P);
      return liberror(P, "_popen", eNOTFND);
    } else if (!executableFile(cmd)) {
      setProcessRunnable(P);
      return liberror(P, "_popen", eNOPERM);
    } else {
      char **argv = (char **) calloc((size_t) (argCount + 2), sizeof(char *));
      char **envp = (char **) calloc((size_t) (envCount + 1), sizeof(char *));
      long i;

      argv[0] = cmd;

      for (i = 1; IsList(Ags); i++, Ags = deRefI(listTail(objV(Ags)))) {
        ptrI Arg = deRefI(listHead(objV(Ags)));

        if (!IsString(Arg)) {
          setProcessRunnable(P);
          return liberror(P, "_popen", eINSUFARG);
        } else {
          strBuffPo ap = stringV(Arg);
          char * arg = stringVal(ap);
          long al = stringLen(ap);

          argv[i] = strndup((const char *) arg, (size_t) al);
        }
      }

      argv[i] = NULL;

      for (i = 0; IsList(Env); i++, Env = deRefI(listTail(objV(Env)))) {
        ptrPo l = listHead(objV(Env));
        ptrI envKey;
        ptrI envVal;
        if (!isTuplePair(l, &envKey, &envVal) || !IsString(envKey)) {
          setProcessRunnable(P);
          return liberror(P, "__popen", eINVAL);
        } else {
          strBuffPo kp = stringV(envKey);
          strBuffPo vp = stringV(envVal);
          long al = stringLen(kp) + stringLen(vp) + 4;
          char buffer[al];

          strMsg(buffer, al, "%S = %S", kp, vp);

          envp[i] = strdup((const char *) buffer);
        }
      }

      envp[i] = NULL;

      switch (openPipe(argv[0], argv, envp, &inPipe, &outPipe, &errPipe, utf8Encoding)) {
        case Ok: {
          ptrI in = allocFilePtr(inPipe); /* return open file descriptor */
          rootPo root = gcAddRoot(&P->proc.heap, &in);
          ptrI out = allocFilePtr(outPipe);
          ptrI errIn;
          retCode ret;

          setProcessRunnable(P);
          gcAddRoot(&P->proc.heap, &out);

          errIn = allocFilePtr(errPipe);

          gcRemoveRoot(&P->proc.heap, root);

          ret = equal(P, &in, &a[4]);

          if (ret == Ok)
            ret = equal(P, &out, &a[5]);

          if (ret == Ok)
            ret = equal(P, &errIn, &a[6]);

          return ret;
        }
        default: {
          for (i = 0; i < argCount; i++)
            free(argv[i]);    /* release the strings we allocated */
          for (i = 0; i < envCount; i++)
            free(envp[i]);    /* release the strings we allocated */

          setProcessRunnable(P);
          return liberror(P, "__popen", eIOERROR);
        }
      }
    }
  }
}

/* Set the encoding on a file */
retCode g__setfileencoding(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);
  ptrI e = deRefI(&a[2]);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__setfileencoding", eINVAL);
  else if (!IsInt(e))
    return liberror(P, "__setfileencoding", eINVAL);
  else {
    ioPo file = filePtr(t1);

    switchProcessState(P, wait_io);
    setEncoding(O_IO(file), pickEncoding(e));
    setProcessRunnable(P);
    return Ok;
  }
}

/*
 * fclose()
 * 
 * close file or pipe 
 */

retCode g__close(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__close", eINVAL);
  else {
    ioPo file = filePtr(t1);

    if (file != NULL) {
      clearFilePointer(t1);
      switchProcessState(P, wait_io);
      retCode ret = closeFile(file);
      setProcessRunnable(P);
      return ret;
    } else
      return Ok;
  }
}

retCode g__stdfile(processPo P, ptrPo a) {
  long fno = integerVal(intV(deRefI(&a[1])));
  ptrI res;

  switch (fno) {
    case 0:res = allocFilePtr(OpenStdin());  /* return stdin file descriptor */
      break;
    case 1:res = allocFilePtr(OpenStdout());  /* return stdout file descriptor */
      break;
    case 2:res = allocFilePtr(OpenStderr());  /* return stderr file descriptor */
      break;
    default:return liberror(P, "__stdfile", eNOPERM);
  }
  return equal(P, &res, &a[2]);
}

/*
 * eof()
 * returns "true" if the file associated with process is at the end of file
 * This function reads character forward in order to position the EOF flag.
 * the character is then un-read by ungetc()
 */
retCode g__end_of_file(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__eof", eINVAL);
  else {
    ioPo file = filePtr(t1);

    if (isReadingFile(file) != Ok)
      return liberror(P, "__eof", eNOPERM);

    switchProcessState(P, wait_io);

    retCode ret = isFileAtEof(file);

    setProcessRunnable(P);

    if (ret == Eof)
      return Ok;
    else
      return Fail;
  }
}

/*
 * ready()
 * returns "true" if the file is ready -- i.e., an eof test would not suspend
 */
retCode g__ready(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__ready", eINVAL);
  else {
    ioPo file = filePtr(t1);

    if (isReadingFile(file) != Ok)
      return liberror(P, "__ready", eNOPERM);

    switchProcessState(P, wait_io);

    retCode ret = isInReady(file);

    setProcessRunnable(P);

    if (ret == Ok)
      return Ok;
    else
      return Fail;
  }
}

/*
 * position()
 * returns the position of the file 
 */
retCode g__fposition(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__fposition", eINVAL);
  else {
    ioPo file = filePtr(t1);

    if (isReadingFile(file) == Ok) {
      ptrI pos = allocateInteger(&P->proc.heap, inCPos(file));
      return equal(P, &a[2], &pos);
    } else if (isWritingFile(file) == Ok) {
      ptrI pos = allocateInteger(&P->proc.heap, outCPos(file));
      return equal(P, &a[2], &pos);
    } else
      return liberror(P, "__fposition", eINVAL);
  }
}

/*
 * seek(p)
 * resets the position of the file 
 */
retCode g__fseek(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);
  ptrI t2 = deRefI(&a[2]);
  objPo o2 = objV(t2);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__fseek", eINVAL);
  else if (isvar(t2))
    return liberror(P, "__fseek", eINSUFARG);
  else if (!isInteger(o2))
    return liberror(P, "__fseek", eINVAL);
  else {
    ioPo file = filePtr(t1);
    integer pos = integerVal((integerPo) o2);

    again:
    switchProcessState(P, wait_io);
    retCode ret = ioSeek(file, pos);
    setProcessRunnable(P);

    switch (ret) {
      case Ok:return Ok;
      case Fail:return Fail;
      case Interrupt:
        if (checkForPause(P))
          goto again;
        else
          return liberror(P, "__fseek", eINTRUPT);
      default:return liberror(P, "__fseek", eIOERROR);
    }
  }
}

/*
 * inbytes(file,count)
 * 
 * get a block of bytes from file attached to process, returned as a list of numbers
 * Complicated 'cos it allows suspension during the read of a block of bytes
 */

retCode g__inbytes(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);
  objPo t2 = objV(deRefI(&a[2]));
  long count;

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__inbytes", eINVAL);
  else if (!isInteger(t2) || (count = integerVal((integerPo) t2)) <= 0)
    return liberror(P, "__inbytes", eINTNEEDD);
  else {
    ioPo file = filePtr(t1);
    long ix = 0;
    char buff[MAX_SYMB_LEN];
    char *buffer = (count > NumberOf(buff) ?
                    (char *) malloc(sizeof(char) * count) :
                    buff);
    long remaining = count;

    if (isReadingFile(file) != Ok)
      return liberror(P, "__inbytes", eNOPERM);

    while (remaining > 0) {
      long bytesRead;
      again:        /* come back in case of interrupt */
      switchProcessState(P, wait_io);
      retCode ret = inBytes(file, (byte*)&buffer[ix], remaining, &bytesRead);
      setProcessRunnable(P);

      switch (ret) {
        case Eof: {
          if (bytesRead == 0) {
            if (ix == 0)
              return liberror(P, "__inbytes", eEOF);
            else {
              ix += bytesRead;
              remaining -= bytesRead;
              break;
            }
          } else {
            ix += bytesRead;
            remaining -= bytesRead;
            break;
          }
        }
        case Ok:        /* We are able to stuff successfully */
          ix += bytesRead;
          remaining -= bytesRead;
          continue;
        case Interrupt:
          if (checkForPause(P))
            goto again;
          else {
            return liberror(P, "__inbytes", eINTRUPT);
          }
        default:return liberror(P, "__inbytes", eIOERROR);
      }
    }

    /* grab the result */
    ptrI reslt = emptyList;
    explodeString(P, buffer, ix, &reslt);

    if (buffer != buff)
      free(buffer);

    return equal(P, &a[3], &reslt);
  }
}

/*
 * _get_file(file)
 *
 * Read everything from a file. Convenient, but expensive because it buffers the contents.
 */

retCode g__get_file(processPo P, ptrPo a) {
  switchProcessState(P, wait_io);

  ptrI t1 = deRefI(&a[1]);
  ptrI t2 = deRefI(&a[2]);

  if (!IsString(t1))
    return liberror(P, "_get_file", eSTRNEEDD);
  if (!isvar(t2))
    return liberror(P, "_get_file", eVARNEEDD);

  ioPo file = openInFile(stringVal(stringV(t1)), utf8Encoding);
  setProcessRunnable(P);

  long count = 0;

  if (isReadingFile(file) != Ok)
    return liberror(P, "_get_file", eNOPERM);

  ioPo outBuff = O_IO(newStringBuffer());
  byte buffer[1024];

  retCode ret = Ok;

  while (ret == Ok) {
    long bytesRead;
    again:        /* come back in case of interrupt */
    switchProcessState(P, wait_io);
    ret = inBytes(file, buffer, NumberOf(buffer), &bytesRead);
    setProcessRunnable(P);

    switch (ret) {
      case Eof: {
        if (bytesRead == 0) {
          if (count == 0) {
            closeFile(file);
            closeFile(outBuff);
            return liberror(P, "_get_file", eEOF);
          } else {
            count += bytesRead;
            continue;
          }
        } else {
          count += bytesRead;
          continue;
        }
      }
      case Ok:        /* We are able to stuff successfully */
        count += bytesRead;
        outBlock(outBuff, buffer, bytesRead);
        continue;
      case Interrupt:
        if (checkForPause(P))
          goto again;
        else {
          closeFile(file);
          closeFile(outBuff);
          return liberror(P, "_get_file", eINTRUPT);
        }
      default:closeFile(file);
        closeFile(outBuff);
        return liberror(P, "_get_file", eIOERROR);
    }
  }

  /* grab the result */
  long length;
  char * text = getTextFromBuffer(&length, O_BUFFER(outBuff));
  ptrI reslt = allocateString(&P->proc.heap, text, length);

  closeFile(file);
  closeFile(outBuff);

  return equal(P, &a[2], &reslt);
}

/*
 * Put the contents of a char * into a file.
 */

retCode g__put_file(processPo P, ptrPo a) {

  ptrI t1 = deRefI(&a[1]);

  if (!IsString(t1))
    return liberror(P, "_put_file", eSTRNEEDD);

  ptrI t2 = deRefI(&a[2]);

  if (!isString(objV(t2)))
    return liberror(P, "_put_file", eSTRNEEDD);

  switchProcessState(P, wait_io);

  ioPo file = newOutFile(stringVal(stringV(t1)), utf8Encoding);

  retCode ret = outUStr(file, stringVal(stringV(t2)));
  setProcessRunnable(P);

  closeFile(file);

  switch (ret) {
    case Ok:return Ok;
    case Interrupt:      /* should never happen */
      return liberror(P, "_put_file", eINTRUPT);
    default:return liberror(P, "_put_file", eIOERROR);
  }
}

/*
 * inchars(file,count)
 * 
 * get a block of characters from file attached to process, returned as a string
 */

retCode g__inchars(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);
  objPo t2 = objV(deRefI(&a[2]));
  long count;

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__inchars", eINVAL);
  else if (!isInteger(t2) || (count = integerVal((integerPo) t2)) <= 0)
    return liberror(P, "__inchars", eINTNEEDD);

  else {
    ioPo file = filePtr(t1);
    long ix = 0;
    char buff[MAX_SYMB_LEN];
    char * buffer = (count > NumberOf(buff) ?
                     (char *) malloc(sizeof(char) * count) :
                     buff);

    if (isReadingFile(file) != Ok)
      return liberror(P, "__inchars", eNOPERM);

    for (; ix < count; ix++) {
      codePoint ch;
      again:        /* come back in case of interrupt */
      switchProcessState(P, wait_io);

      switch (inChar(file, &ch)) {          /* Attempt to read a character */
        case Eof:
          if (ix == 0) {      /* attempt to read past EOF */
            setProcessRunnable(P);
            return liberror(P, "__inchars", eEOF);
          } else {
            count = ix;
            break;      /* results in an early exit */
          }
        case Ok: {    /* We are able to read byte successfully */
          if (appendCodePoint(buffer, &ix, count, ch) != Ok)
            return liberror(P, "__inchars", eFAIL);

          continue;
        }
        case Interrupt:
          if (checkForPause(P))
            goto again;
          else {
            setProcessRunnable(P);
            return liberror(P, "__inchars", eINTRUPT);
          }
        default:setProcessRunnable(P);
          return liberror(P, "__inchars", eIOERROR);
      }
    }

    setProcessRunnable(P);

    /* grab the result */
    ptrI reslt = allocateString(&P->proc.heap, buffer, count);

    if (buffer != buff)
      free(buffer);

    return equal(P, &a[3], &reslt);
  }
}

/*
 * inchar(count)
 * 
 * get a single character from file attached to process, 
 * returned as a single character
 */

retCode g__inchar(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__inchar", eINVAL);
  else {
    ioPo file = filePtr(t1);
    codePoint ch;

    if (isReadingFile(file) != Ok)
      return liberror(P, "__inchar", eNOPERM);

    again:
    switchProcessState(P, wait_io);
    retCode ret = inChar(file, &ch);
    ptrI Ch = allocateInteger(&P->proc.heap, ch);
    setProcessRunnable(P);

    switch (ret) {
      case Ok:return funResult(P, a, 2, Ch);
      case Eof:setProcessRunnable(P);
        return liberror(P, "__inchar", eEOF); /* eof error */
      case Interrupt:
        if (checkForPause(P))
          goto again;      /* try again after GC is finished */
        else {
          setProcessRunnable(P);
          return liberror(P, "__inchar", eINTRUPT);
        }
      default:setProcessRunnable(P);
        return liberror(P, "__inchar", eINVAL); /* other error */
    }
  }
}

/*
 * inbyte
 * 
 * get a single character from file attached to process, 
 * returned as a single integer
 */

retCode g__inbyte(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__inbyte", eINVAL);
  else {
    ioPo file = filePtr(t1);
    byte ch;

    if (isReadingFile(file) != Ok)
      return liberror(P, "__inbyte", eNOPERM);
    again:
    switchProcessState(P, wait_io);
    retCode ret = inByte(file, &ch);
    setProcessRunnable(P);

    switch (ret) {
      case Ok: {
        ptrI ans = allocateInteger(&P->proc.heap, ch);

        return equal(P, &a[2], &ans);
      }
      case Interrupt:
        if (checkForPause(P))
          goto again;      /* try again */
        else
          return liberror(P, "__inbyte", eINTRUPT);
      case Eof:return liberror(P, "__inbyte", eEOF); /* eof error */
      default:return liberror(P, "__inbyte", eIOERROR); /* other error */
    }
  }
}

/*
 * inline(file,match)
 * 
 * get a single line from a file, returned as a string
 * get all characters until either EOF or a newline is found
 */
retCode g__inline(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__inline", eINVAL);
  else {
    ioPo file = filePtr(t1);
    codePoint chr;

    if (isReadingFile(file) != Ok)
      return liberror(P, "__inline", eNOPERM);

    ioPo str = O_IO(newStringBuffer());

    while (True) {
      switchProcessState(P, wait_io);
      retCode ret = inChar(file, &chr);
      setProcessRunnable(P);

      switch (ret) {
        case Eof:
          if (bufferSize(O_BUFFER(str)) == 0) {
            closeFile(str);
            return liberror(P, "__inline", eEOF);    /* cant read past the end of the file */
          } else {
            long len;
            char * buff = getTextFromBuffer(&len, O_BUFFER(str));
            ptrI reslt = allocateString(&P->proc.heap, buff, len); /* grab the result */

            closeFile(str);    /* we are done reading */
            return equal(P, &a[3], &reslt);
          }
        case Ok:
          if (chr == '\n') {
            ptrI RR = kvoid;              // This is clumsy, but necessary
            rootPo root = gcAddRoot(&P->proc.heap, &RR);
            ret = closeOutString(str, &P->proc.heap, &RR);
            if (ret == Ok)
              ret = equal(P, &RR, &a[3]);
            gcRemoveRoot(&P->proc.heap, root);
            return ret;
          } else {
            outChar(str, chr);
            continue;
          }
        case Interrupt:
          if (checkForPause(P))
            continue;      /* We were interrupted for a GC */
          else
            return liberror(P, "__inline", eINTRUPT);
        default:return liberror(P, "__inline", eIOERROR);
      }
    }
  }
}

/*
 * intext(file,search)
 * 
 * return all available text, until either of EOF is true or one of the characters in search is found
 */
retCode g__intext(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);
  ptrI t2 = deRefI(&a[2]);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__intext", eINVAL);
  else if (isvar(t2))
    return liberror(P, "__intext", eINSUFARG);
  else if (IsString(t2) != Ok)
    return liberror(P, "__intext", eSTRNEEDD);
  else {
    ioPo file = filePtr(t1);
    codePoint chr;

    strBuffPo sp = stringV(t2);
    long tlen = stringLen(sp);
    char term[tlen + 1];

    copyString2Buff(term, NumberOf(term), sp);

    if (isReadingFile(file) != Ok)
      return liberror(P, "__intext", eNOPERM);

    ioPo str = O_IO(newStringBuffer());

    while (True) {
      switchProcessState(P, wait_io);
      retCode ret = inChar(file, &chr);
      setProcessRunnable(P);

      switch (ret) {
        case Eof:
          if (bufferSize(O_BUFFER(str)) == 0) {
            closeFile(str);
            return liberror(P, "__intext", eEOF);    /* cant read past the end of the file */
          } else {
            long len;
            char * buff = getTextFromBuffer(&len, O_BUFFER(str));
            ptrI reslt = allocateString(&P->proc.heap, buff, (size_t) len); /* grab the result */

            closeFile(str);    /* we are done reading */

            return equal(P, &a[3], &reslt);
          }
        case Ok:
          if (uniIndexOf(term, (long) tlen, 00, chr) >= 0) { /* have we found a terminating byte? */
            ptrI RR = kvoid;              // This is clumsy, but necessary
            rootPo root = gcAddRoot(&P->proc.heap, &RR);
            ret = closeOutString(str, &P->proc.heap, &RR);
            if (ret == Ok)
              ret = equal(P, &RR, &a[3]);
            gcRemoveRoot(&P->proc.heap, root);
            return ret;
          } else if (chr == EOF_CHAR && bufferSize(O_BUFFER(str)) == 0) {
            long len;
            char * buff = getTextFromBuffer(&len, O_BUFFER(str));
            ptrI result = allocateString(&P->proc.heap, buff, len); /* grab the result */

            closeFile(str);    /* we are done reading */

            return equal(P, &a[3], &result);
          } else
            outChar(str, chr);
          continue;
        case Interrupt:
          if (checkForPause(P))
            continue;      /* We were interrupted for a GC */
          else
            return liberror(P, "__intext", eINTRUPT);
        default:return liberror(P, "__intext", eIOERROR);
      }
    }
  }
}

/*
 * outch(file,ch)
 * 
 * write a single character on file controlled by process
 */

retCode g__outch(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__outch", eINVAL);
  else {
    ioPo file = filePtr(t1);
    ptrI tc = deRefI(&a[2]);

    if (isWritingFile(file) != Ok)
      return liberror(P, "__outch", eNOPERM);
    else {
      again:
      switchProcessState(P, wait_io);
      retCode ret = outChar(file, (codePoint) IntVal(tc));
      setProcessRunnable(P);

      switch (ret) {
        case Ok:
          if (IntVal(tc) == '\n')
            flushFile(file);
          return Ok;
        case Interrupt:
          if (checkForPause(P))
            goto again;
          else
            return liberror(P, "__outch", eINTRUPT);
        default:return liberror(P, "__outch", eIOERROR);
      }
    }
  }
}

/*
 * outbyte(file,ch)
 * 
 * write a single byte on file controlled by process
 */

retCode g__outbyte(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__outbyte", eINVAL);
  else {
    ioPo file = filePtr(t1);
    objPo t2 = objV(deRefI(&a[2]));

    if (isWritingFile(file) != Ok)
      return liberror(P, "__outbyte", eNOPERM);
    else if (isvar(deRefI(&a[1])) || !isInteger(t2))
      return liberror(P, "__outbyte", eINTNEEDD);
    else {
      again:
      switchProcessState(P, wait_io);
      retCode ret = outChar(file, (codePoint) integerVal((integerPo) t2));
      setProcessRunnable(P);

      switch (ret) {
        case Ok:return Ok;
        case Interrupt:
          if (checkForPause(P))
            goto again;
          else
            return liberror(P, "__outbyte", eINTRUPT);
        default:return liberror(P, "__outbyte", eIOERROR);
      }
    }
  }
}

retCode g__outbytes(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);
  ptrI t2 = deRefI(&a[2]);
  objPo o2 = objV(t2);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__outbytes", eINVAL);
  else if (!isGroundTerm(&t2))
    return liberror(P, "__outbytes", eINSUFARG);
  else {
    ioPo file = filePtr(t1);

    if (isWritingFile(file) != Ok)
      return liberror(P, "__outbytes", eNOPERM);
    else {
      bufferPo buff = newStringBuffer();
      retCode ret = implodeString(&t2, O_IO(buff));
      if (ret == Ok) {
        switchProcessState(P, wait_io);
        long len;
        char * s = getTextFromBuffer(&len, buff);

        ret = outText(file, s, len);

        setProcessRunnable(P);
        closeFile(O_IO(buff));

        switch (ret) {
          case Ok:return Ok;
          case Interrupt:      /* should never happen */
            return liberror(P, "_outbytes", eINTRUPT);
          default:return liberror(P, "_outbytes", eIOERROR);
        }
      } else
        return ret;
    }
  }
}

/*
 * outtext(file,str)
 * 
 * write a string on file
 */

retCode g__outtext(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__outtext", eINVAL);
  else {
    ioPo file = filePtr(t1);

    if (isWritingFile(file) != Ok)
      return liberror(P, "__outtext", eNOPERM);
    else if (IsString(a[2]) != Ok)
      return liberror(P, "__outtext", eSTRNEEDD);
    else {
      switchProcessState(P, wait_io);
      strBuffPo sp = stringV(deRefI(&a[2]));
      char * s = stringVal(sp);
      long len = stringLen(sp);

      retCode ret = outText(file, s, len);

      setProcessRunnable(P);

      switch (ret) {
        case Ok:return Ok;
        case Interrupt:      /* should never happen */
          return liberror(P, "__outtext", eINTRUPT);
        default:return liberror(P, "__outtext", eIOERROR);
      }
    }
  }
}

/* Write a term in default notation onto a file channel */
retCode g__outterm(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__outterm", eINVAL);
  else {
    ioPo file = filePtr(t1);

    if (isWritingFile(file) != Ok)
      return liberror(P, "__outterm", eNOPERM);

    switchProcessState(P, wait_io);
    retCode ret = outCell(file, &a[2], INT_MAX / 4, 0, False);
    setProcessRunnable(P);

    switch (ret) {
      case Ok:return Ok;
      case Interrupt:      /* should never happen */
        return liberror(P, "__outterm", eINTRUPT);
      default:return liberror(P, "__outterm", eIOERROR);
    }
  }
}

/* Post a message in the log */
retCode g__logmsg(processPo P, ptrPo a) {
  switchProcessState(P, wait_io);

  ptrI t1 = deRefI(&a[1]);

  if (!IsString(t1))
    return liberror(P, "__logmsg", eSTRNEEDD);
  else {
    retCode ret = logMsg(logFile, "%S", &t1);
    setProcessRunnable(P);

    switch (ret) {
      case Ok:return Ok;
      case Interrupt:      /* should never happen */
        return liberror(P, "__logmsg", eINTRUPT);
      default:return liberror(P, "__logmsg", eIOERROR);
    }
    return Ok;
  }
}

/*
 * flush(file)
 * flush remaining buffered output of an output stream. 
 */

retCode g__flush(processPo P, ptrPo a) {
  ptrI t1 = deRefI(&a[1]);
  objPo o1 = objV(t1);

  if (!hasClass(o1, filePtrClass))
    return liberror(P, "__flush", eINVAL);
  else {
    ioPo file = filePtr(t1);
    retCode ret;

    if (isFileOpen(file) != Ok)
      return liberror(P, "__flush", eNOPERM);

    if (isWritingFile(file) != Ok)
      return liberror(P, "__flush", eIOERROR);

    switchProcessState(P, wait_io);

    while ((ret = flushFile(file)) == Fail);
    setProcessRunnable(P);

    if (ret == Error)
      return liberror(P, "__flush", eIOERROR);
    else
      return Ok;
  }
}

retCode g__flushall(processPo P, ptrPo a) {
  switchProcessState(P, wait_io);
  flushOut();
  setProcessRunnable(P);
  return Ok;
}

/* This must be installed into outMsg */
static retCode cellMsg(ioPo f, void *data, long depth, long precision, logical alt) {
  ptrPo ptr = (ptrPo) data;

  if (ptr != NULL) {
    if (precision <= 0)
      precision = 32767;
    outCell(f, ptr, depth, (int) precision, alt);
  } else
    outStr(f, "(NULL)");
  return Ok;
}

static retCode stringMsg(ioPo f, void *data, long depth, long precision, logical alt) {
  ptrPo ptr = (ptrPo) data;

  if (ptr != NULL && IsString(*ptr)) {
    if (precision <= 0)
      precision = 32767;
    strBuffPo s = stringV(*ptr);
    char * str = stringVal(s);
    long len = stringLen(s);

    return outText(f, str, len);
  } else
    outStr(f, "(NULL)");
  return Ok;
}

ptrI allocFilePtr(ioPo file) {
  fPo f = (fPo) allocateSpecial(&globalHeap, filePtrClass);

  f->file = file;
  return objP(f);
}

ioPo filePtr(ptrI p) {
  objPo o = objV(p);
  assert(hasClass(o, filePtrClass));
  return ((fPo) o)->file;
}

static void clearFilePointer(ptrI p) {
  objPo o = objV(p);
  assert(hasClass(o, filePtrClass));

  ((fPo) o)->file = NULL;
}


