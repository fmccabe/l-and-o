/*
  String handling functions for the L&O engine
  Copyright (c) 2016, 2017. Francis G. McCabe

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
  except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the
  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied. See the License for the specific language governing
  permissions and limitations under the License.
*/

#include "config.h"		/* pick up standard configuration header */
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "lo.h"
#include "term.h"

static long strSizeFun(specialClassPo class, objPo o);
static comparison strCompFun(specialClassPo class, objPo o1, objPo o2);
static retCode strOutFun(specialClassPo class, ioPo out, objPo o);
static retCode strScanFun(specialClassPo class, specialHelperFun helper, void *c, objPo o);
static objPo strCopyFun(specialClassPo class, objPo dst, objPo src);
static uinteger strHashFun(specialClassPo class, objPo o);

void initStringClass(void) {
  stringClass = newSpecialClass("lo.core#string", strSizeFun, strCompFun,
                                strOutFun, strCopyFun, strScanFun, strHashFun);
}

static long strSizeFun(specialClassPo class, objPo o) {
  assert(o->class == stringClass);
  stringPo s = (stringPo) o;

  return CellCount(sizeof(stringRec) + (s->size + 1) * sizeof(byte));
}

static comparison strCompFun(specialClassPo class, objPo o1, objPo o2) {
  if (o1 == o2)
    return same;
  else if (o1->class == stringClass && o2->class == stringClass) {
    stringPo s1 = (stringPo) o1;
    stringPo s2 = (stringPo) o2;

    long l1 = stringLen(s1);
    long l2 = stringLen(s2);

    if (l1 != l2)
      return incomparible;
    else if(memcmp(stringVal(s1),stringVal(s2),(size_t)(l1*sizeof(byte)))==0)
      return same;
  } else
    return incomparible;
}

static retCode strOutFun(specialClassPo class, ioPo out, objPo o) {
  stringPo s = (stringPo) o;
  string str = StringVal(s);
  long len = stringLen(s);
  retCode r = outChar(out, '\'');

  long pos = 0;

  while (pos < len && r == Ok) {
    codePoint ch;
    r = nxtPoint(str, &pos, len, &ch);
    if (r == Ok)
      r = wStringChr(out, ch);
    if (r == Ok)
      r = outChar(out, '\'');
  }
  return r;
}

static retCode strScanFun(specialClassPo class, specialHelperFun helper, void *c, objPo o) {
  return Ok;
}

static objPo strCopyFun(specialClassPo class, objPo dst, objPo src) {
  long size = strSizeFun(class, src);
  memmove((void *) dst, (void *) src, size * sizeof(ptrI));

  return (objPo) (((ptrPo) dst) + size);
}

static uinteger strHashFun(specialClassPo class, objPo o) {
  assert(o->class == stringClass);

  stringPo s = (stringPo) o;
  return uniHash(StringVal(s));
}

ptrI allocateCString(heapPo H, const char *m) {
  return allocateString(H, (string) m, strlen(m));
}

ptrI allocateString(heapPo H, string m, long count) {
  size_t len = CellCount(sizeof(stringRec) + (count + 1) * sizeof(byte));
  stringPo new = (stringPo) allocate(H, len);

  new->class = stringClass;
  new->size = count;
  memcpy(new->data,m,(size_t)(count*sizeof(byte)));
  return objP(new);
}

retCode copyString2Buff(byte *buffer, long bLen, stringPo s) {
  long sLen = stringLen(s);
  long len = min(sLen, bLen - 1);

  memcpy(buffer, stringVal(s), len * sizeof(byte));
  buffer[len] = '\0';

  if (sLen < bLen)
    return Ok;
  else
    return Eof;
}

retCode g__stringOf(processPo P, ptrPo a) {
  ptrI Data = deRefI(&a[1]);
  ptrI Width = deRefI(&a[2]);
  ptrI Prec = deRefI(&a[3]);
  heapPo H = &P->proc.heap;

  if (isvar(Width) || !isInteger(objV(Width)))
    return liberror(P, "__stringOf", eINTNEEDD);
  else if (isvar(Prec) || !isInteger(objV(Prec)))
    return liberror(P, "__stringOf", eINTNEEDD);
  else {
    long width = (long) integerVal(intV(Width));
    long prec = (long) integerVal(intV(Prec));
    string buffer = (prec < 0 ?
                     (byte *) malloc(sizeof(byte) * (-prec + 1))
                              : NULL);
    ioPo str = O_IO(newStringBuffer());
    retCode ret = outCell(str, &Data, prec == 0 ? INT_MAX / 4 : prec, 0, False);

    if (ret != Ok) {
      closeFile(str);
      if (buffer != NULL)
        free(buffer);
      return liberror(P, "__stringOf", eINVAL);
    } else {
      long len;
      string txt = getTextFromBuffer(&len, O_BUFFER(str));

      if (width != 0) {
        long sLen = labs(width) + 1;
        byte text[sLen];

        if (width > 0) {                    /* right padded */
          string p;
          long w = width - len;

          uniNCpy(text, sLen, txt, len);
          p = text + len;
          while (w-- > 0)
            *p++ = ' ';                   /* pad out with spaces */
          *p = 0;
        } else {
          string p = text;
          long w = -width - (integer) len;
          while (w-- > 0)
            *p++ = ' ';                   /* left pad with spaces */
          if (labs(width) > len)
            uniNCpy(p, sLen - (p - text), txt, len);
          else
            uniNCpy(p, sLen - (p - text), txt + len + width, -width);
        }

        ptrI txtList = allocateString(H, text, labs(width));

        closeFile(str);  /* close the file down */
        if (buffer != NULL)
          free(buffer);
        return funResult(P, a, 4, txtList);
      } else {
        ptrI txtList = kvoid;
        rootPo root = gcAddRoot(&P->proc.heap, &txtList);
        ret = closeOutString(str, &P->proc.heap, &txtList);

        gcRemoveRoot(&P->proc.heap, root);
        if (buffer != NULL)
          free(buffer);

        if (ret == Ok)
          return equal(P, &a[4], &txtList);
        else
          return ret;
      }
    }
  }
}

/*
 Trim a string to be a particular width
 negative width = right justified 
*/
retCode g__trim(processPo P, ptrPo a) {
  ptrI Data = deRefI(&a[1]);
  ptrI Width = deRefI(&a[2]);

  if (isvar(Width) || !isInteger(objV(Width)))
    return liberror(P, "__trim", eINTNEEDD);
  else if (isvar(Data))
    return liberror(P, "__trim", eINSUFARG);
  else if (!IsString(Data))
    return liberror(P, "__trim", eINVAL);
  else {
    long width = integerVal(intV(Width));
    stringPo D = stringV(Data);
    string data = stringVal(D);
    long len = stringLen(D);
    long awidth = labs(width);
    heapPo H = &P->proc.heap;

    if (width == 0 || awidth == len)
      return equal(P, &a[1], &a[3]);      /* just return the string itself */
    else {
      byte buff[MAX_SYMB_LEN];
      string buffer = (width > NumberOf(buff) ? (byte *) malloc(sizeof(byte) * awidth) : buff);

      if (width < 0) {                      /* right justified */
        long cnt = len - awidth;           /* how much we have to step into the string */
        string l = &data[cnt];

        cnt = awidth - len;               /* the number of pad characters */
        while (cnt > 0)
          buffer[--cnt] = ' ';
        cnt = awidth - len;
        if (cnt < 0)
          cnt = 0;
        strncpy((char *) &buffer[cnt], (char *) l, awidth - cnt);     // plop in the string contents
      } else {
        strncpy((char *) buffer, (char *) data, awidth);

        if (len < width) {
          for (long i = len; i < width; i++)
            buffer[i] = ' ';
        }
      }

      {
        ptrI txtList = allocateString(H, buffer, awidth);

        if (buffer != buff)
          free(buffer);

        return funResult(P, a, 3, txtList);
      }
    }
  }
}

// Prepare a string to be formatted in a differently sized field

retCode strPrepare(string tgt, long tLen, string src, long sLen, codePoint pad, logical left, long width) {
  long i, j;

  if (width == 0)
    width = sLen;

  if (width >= tLen)
    return Error;      /* requested width too large */

  long gaps = width - sLen;    /* How many gap fillers */

  if (left) {        /* left aligned */
    if (gaps < 0) {        /* We have to trim, lose trailing */
      for (i = 0; i < width; i++)
        tgt[i] = src[i];
      tgt[i] = '\0';      /* terminate */
    } else {
      for (i = 0; i < sLen; i++)
        tgt[i] = src[i];
      while (i < width)
        appendCodePoint(tgt, &i, tLen, pad);
      tgt[i] = '\0';
    }
  } else {          /* right aligned */
    if (gaps < 0) {
      for (j = 0, i = sLen + gaps; i < sLen; i++, j++) /* lose the left part of the source */
        tgt[j] = src[i];
      tgt[j] = '\0';
    } else {        /* extra pad on the left */
      for (j = 0, i = gaps; i > 0; i--)
        appendCodePoint(tgt, &j, tLen, pad);
      for (i = 0; i < sLen; j++, i++)
        tgt[j] = src[i];
      tgt[j] = '\0';
    }
  }
  return Ok;
}

retCode g_explode(processPo P, ptrPo a) {
  ptrI Str = deRefI(&a[1]);

  if (isvar(Str))
    return liberror(P, "explode", eINSUFARG);
  else if (!IsString(Str))
    return liberror(P, "explode", eINVAL);
  else {
    stringPo s = stringV(Str);
    string src = stringVal(s);
    long strLen = stringLen(s);

    byte buff[MAX_SYMB_LEN];
    string buffer = (strLen > NumberOf(buff) ? (byte *) malloc(sizeof(byte) * strLen) : buff);

    strncpy((char *) buffer, (char *) src, strLen); // Copy out the string in case of GC

    ptrI out = emptyList;
    ptrI el = kvoid;
    heapPo H = &P->proc.heap;
    rootPo root = gcAddRoot(H, &out);

    gcAddRoot(H, &el);

    long pos = strLen;

    while (pos > 0) {
      codePoint ch;
      if (prevPoint(buffer, &pos, &ch) != Ok)
        return liberror(P, "explode", eINVAL);
      el = allocateInteger(H, ch);
      out = consLsPair(H, el, out);
    }

    if (buffer != buff)
      free(buffer);

    gcRemoveRoot(H, root);
    return funResult(P, a, 2, out);
  }
}

retCode explodeString(processPo P, byte *text, long length, ptrPo a) {
  byte buff[MAX_SYMB_LEN];
  string buffer = (length > NumberOf(buff) ? (byte *) malloc(sizeof(byte) * length) : buff);
  strncpy((char *) buffer, (char *) text, length); // Copy out the string in case of GC

  *a = emptyList;
  ptrI el = kvoid;
  heapPo H = &P->proc.heap;
  rootPo root = gcAddRoot(H, a);

  gcAddRoot(H, &el);

  long pos = length;

  while (pos > 0) {
    codePoint ch;
    if (prevPoint(buffer, &pos, &ch) != Ok)
      return Error;
    el = allocateInteger(H, ch);
    *a = consLsPair(H, el, *a);
  }

  if (buffer != buff)
    free(buffer);

  gcRemoveRoot(H, root);
  return Ok;
}

retCode g_implode(processPo P, ptrPo a) {
  ptrI Ls = deRefI(&a[1]);

  if (!isGroundTerm(&Ls))
    return liberror(P, "implode", eINSUFARG);
  else {
    long sLen = 4 * ListLen(Ls) + 1; // Over estimate of string size.
    long pos = 0;
    byte text[sLen];

    while (IsList(Ls)) {
      ptrPo h = listHead(objV(Ls));
      ptrI C = deRefI(h);

      if (isobj(C) && IsInt(C)) {
        if (pos < sLen) {
          appendCodePoint(text, &pos, sLen, (codePoint) IntVal(C));
          Ls = deRefI(h + 1);
        } else
          return liberror(P, "implode", eINVAL);
      }
    }
    switchProcessState(P, in_exclusion);

    ptrI result = allocateString(&P->proc.heap, text, pos);

    setProcessRunnable(P);
    return funResult(P, a, 2, result);
  }
}

retCode implodeString(ptrPo l, ioPo out) {
  ptrI Ls = deRefI(l);
  retCode ret = Ok;

  while (ret == Ok && IsList(Ls)) {
    ptrPo h = listHead(objV(Ls));
    ptrI C = deRefI(h);

    if (isobj(C) && IsInt(C)) {
      codePoint ch = (codePoint) IntVal(C);

      ret = outChar(out, ch);

      Ls = deRefI(h + 1);
    } else
      return Error;
  }
  return ret;
}

retCode g__str_len(processPo P, ptrPo a) {
  ptrI a1 = deRefI(&a[1]);
  ptrI a2 = deRefI(&a[2]);

  if (isvar(a1) || !isString(objV(a1)))
    return liberror(P, "_str_len", eSTRNEEDD);
  else {
    stringPo str = stringV(a1);
    integer slen = stringLen(str);

    if (isvar(a2)) {
      ptrI R = allocateInteger(&P->proc.heap, slen);

      bindVar(P, deRef(&a[2]), R);
      return Ok;
    } else if (integerVal(intV(a2)) == slen)
      return Ok;
    else
      return Fail;
  }
}

retCode g__str_hash(processPo P, ptrPo a) {
  ptrI a1 = deRefI(&a[1]);
  ptrI a2 = deRefI(&a[2]);

  if (isvar(a1) || !isString(objV(a1)))
    return liberror(P, "_str_hash", eSTRNEEDD);
  else {
    stringPo str = stringV(a1);
    string s = stringVal(str);
    long len = stringLen(str);

    integer hash = uniNHash(s, len);

    if (isvar(a2)) {
      ptrI R = allocateInteger(&P->proc.heap, hash);

      bindVar(P, deRef(&a[2]), R);
      return Ok;
    } else if (integerVal(intV(a2)) == hash)
      return Ok;
    else
      return Fail;
  }
}

retCode g__str_concat(processPo P, ptrPo a) {
  ptrI a1 = deRefI(&a[1]);
  ptrI a2 = deRefI(&a[2]);

  if (isvar(a1) || isvar(a2))
    return liberror(P, "_str_concat", eSTRNEEDD);
  else if (!isString(objV(a1)) || !isString(objV(a2)))
    return liberror(P, "_str_concat", eINVAL);
  else {
    stringPo str1 = stringV(a1);
    string s1 = stringVal(str1);
    stringPo str2 = stringV(a2);
    string s2 = stringVal(str2);
    long slen1 = stringLen(str1);
    long slen2 = stringLen(str2);
    long tlen = slen1 + slen2;
    byte catted[tlen + 1];

    uniCpy(catted, tlen + 1, s1);
    uniCpy(&catted[slen1], slen2 + 1, s2);

    ptrI rslt = allocateString(&P->proc.heap, catted, tlen);

    return funResult(P, a, 3, rslt);
  }
}

retCode g__str_multicat(processPo P, ptrPo a) {
  ptrI a1 = deRefI(&a[1]);
  ptrI a2 = deRefI(&a[2]);

  if (isvar(a1))
    return liberror(P, "_str_multicat", eSTRNEEDD);
  else if (!isGroundTerm(&a1))
    return liberror(P, "_str_multicat", eINVAL);
  else if (!isvar(a2))
    return liberror(P, "_str_multicat", eVARNEEDD);
  else {
    bufferPo b = newStringBuffer();
    ptrI Ls = a1;
    retCode ret = Ok;

    while (ret == Ok && IsList(Ls)) {
      ptrPo h = listHead(objV(Ls));
      ptrI C = deRefI(h);

      if (IsString(C)) {
        stringPo s = stringV(C);

        ret = outText(O_IO(b), stringVal(s), stringLen(s));
        Ls = deRefI(h + 1);
      }
    }
    long len;
    string txt = getTextFromBuffer(&len, b);
    ptrI rslt = allocateString(&P->proc.heap, txt, len);
    closeFile(O_IO(b));
    return funResult(P, a, 2, rslt);
  }
}

retCode g__str_lt(processPo P, ptrPo a) {
  ptrI a1 = deRefI(&a[1]);
  ptrI a2 = deRefI(&a[2]);

  if (isvar(a1) || isvar(a2))
    return liberror(P, "_str_lt", eSTRNEEDD);
  else if (!isString(objV(a1)) || !isString(objV(a2)))
    return liberror(P, "_str_lt", eINVAL);
  else {
    string s1 = stringVal(stringV(a1));
    string s2 = stringVal(stringV(a2));

    if (uniCmp(s1, s2) == smaller)
      return Ok;
    else
      return Fail;
  }
}

retCode g__str_ge(processPo P, ptrPo a) {
  ptrI a1 = deRefI(&a[1]);
  ptrI a2 = deRefI(&a[2]);

  if (isvar(a1) || isvar(a2))
    return liberror(P, "_str_ge", eSTRNEEDD);
  else if (!isString(objV(a1)) || !isString(objV(a2)))
    return liberror(P, "_str_ge", eINVAL);
  else {
    string s1 = stringVal(stringV(a1));
    string s2 = stringVal(stringV(a2));

    if (uniCmp(s1, s2) != smaller)
      return Ok;
    else
      return Fail;
  }
}

retCode g__str_start(processPo P, ptrPo a) {
  ptrI a1 = deRefI(&a[1]);
  ptrI a2 = deRefI(&a[2]);

  if (isvar(a1) || isvar(a2))
    return liberror(P, "_str_start", eSTRNEEDD);
  else if (!isString(objV(a1)) || !isString(objV(a2)))
    return liberror(P, "_str_start", eINVAL);
  else {
    stringPo str = stringV(a1);
    stringPo tgt = stringV(a2);

    string s1 = stringVal(str);
    string s2 = stringVal(tgt);

    if (uniNCmp(s1, s2, stringLen(tgt)) == same)
      return Ok;
    else
      return Fail;
  }
}

retCode g__str_find(processPo P, ptrPo a) {
  ptrI a1 = deRefI(&a[1]);
  ptrI a2 = deRefI(&a[2]);
  ptrI a3 = deRefI(&a[3]);
  ptrI a4 = deRefI(&a[4]);

  if (isvar(a1) || !isString(objV(a1)) || isvar(a2) || !isString(objV(a2)) || isvar(a3) || !isInteger(objV(a3)))
    return liberror(P, "_str_find", eINSUFARG);
  else {
    stringPo src = stringV(a1);
    stringPo tgt = stringV(a2);
    integer start = integerVal(intV(a3));

    integer pos = uniSearch(stringVal(src), stringLen(src), start, stringVal(tgt), stringLen(tgt));

    if (pos < 0)
      return Fail;
    else if (isvar(a4)) {
      ptrI R = allocateInteger(&P->proc.heap, pos);

      bindVar(P, deRef(&a[2]), R);
      return Ok;
    } else if (integerVal(intV(a2)) == pos)
      return Ok;
    else
      return Fail;
  }
}

retCode g__str_split(processPo P, ptrPo a) {
  ptrI a1 = deRefI(&a[1]);
  ptrI a2 = deRefI(&a[2]);
  ptrI a3 = deRefI(&a[3]);
  ptrI a4 = deRefI(&a[4]);

  if (isvar(a1) || !isString(objV(a1)) || isvar(a2) || !isInteger(objV(a2)))
    return liberror(P, "_str_split", eINSUFARG);
  else {
    stringPo S = stringV(a1);
    string src = stringVal(S);
    integer start = integerVal(intV(a2));
    long len = stringLen(S);

    if (start < 0 || start > len)
      return Fail;
    else {
      if (isvar(a3)) {
        ptrI left = allocateString(&P->proc.heap, src, start);
        bindVar(P, deRef(&a[3]), left);
      } else {
        string left = stringVal(stringV(a3));
        if (uniNCmp(src, left, start) != same)
          return Fail;
      }
      if (isvar(a4)) {
        ptrI right = allocateString(&P->proc.heap, &src[start], len - start);
        bindVar(P, deRef(&a[4]), right);
        return Ok;
      } else {
        string right = stringVal(stringV(a4));

        if (uniNCmp(&src[start], right, len - start) == same)
          return Ok;
        else
          return Fail;
      }
    }
  }
}

retCode g__sub_str(processPo P, ptrPo a) {
  ptrI a1 = deRefI(&a[1]);
  ptrI a2 = deRefI(&a[2]);
  ptrI a3 = deRefI(&a[3]);
  ptrI a4 = deRefI(&a[4]);

  if (isvar(a1) || !isString(objV(a1)) || isvar(a2) || !isInteger(objV(a2)) || isvar(a3) || !isInteger(objV(a3)))
    return liberror(P, "_sub_str", eINSUFARG);
  else {
    stringPo str = stringV(a1);
    string src = stringVal(str);
    long len = stringLen(str);
    integer start = integerVal(intV(a2));
    integer end = integerVal(intV(a3));

    end = min(end, len);

    if (start < 0 || start > len || end < start)
      return Fail;
    else if (isvar(a4)) {
      ptrI sub = allocateString(&P->proc.heap, &src[start], end - start);
      bindVar(P, deRef(&a[4]), sub);
      return Ok;
    } else {
      string sub = stringVal(stringV(a4));
      if (uniNCmp(&src[start], sub, end - start) != same)
        return Fail;
      else
        return Ok;
    }
  }
}

retCode g__str_gen(processPo P, ptrPo a) {
  ptrI a1 = deRefI(&a[1]);
  ptrI a2 = deRefI(&a[2]);

  if (isvar(a1) || !isString(objV(a1)))
    return liberror(P, "_str_gen", eINSUFARG);
  else if (!isvar(a2))
    return liberror(P, "_str_gen", eVARNEEDD);
  else {
    string prefix = stringVal(stringV(a1));
    byte buff[128];
    strMsg(buff, NumberOf(buff), "%s%ld", prefix, random());

    ptrI sub = allocateString(&P->proc.heap, buff, uniStrLen(buff));
    bindVar(P, deRef(&a[4]), sub);
    return Ok;
  }
}

retCode closeOutString(ioPo f, heapPo H, ptrPo tgt) {
  long len;
  string buff = getTextFromBuffer(&len, O_BUFFER(f));
  ptrI str = allocateString(H, buff, len);

  *deRef(tgt) = str;

  return closeFile(f);
}
