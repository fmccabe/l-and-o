/*
  Unicode encoding and decoding functions
  Copyright (c) 2016, 2017. Francis G. McCabe

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
  except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the
  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied. See the License for the specific language governing
  permissions and limitations under the License.
*/

#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "unicodeP.h"

retCode nxtPoint(char *src, long *start, long end, codePoint *code) {
  long pos = *start;

  if (pos < end) {
    char b = src[pos++];

    if (b <= 0x7f) {
      *code = (codePoint) b;
      *start = pos;
      return Ok;
    } else if (UC80(b)) {
      if (pos < end) {
        char nb = src[pos++];
        codePoint ch = (codePoint) (UX80(b) << 6 | UXR(nb));

        if (ch < 0x7ff) {
          *code = ch;
          *start = pos;
          return Ok;
        } else
          return Error;
      } else
        return Eof;
    } else if (UC800(b)) {
      if (pos + 2 < end) {
        char md = src[pos++];
        char up = src[pos++];

        codePoint ch = (codePoint) ((UX800(b) << 12) | (UXR(md) << 6) | (UXR(up)));

        if (ch >= 0x800 && ch <= 0xffff) {
          *code = ch;
          *start = pos;
          return Ok;
        } else
          return Error;
      } else
        return Eof;
    } else if (UC1000(b)) {
      if (pos + 3 < end) {
        char md = src[pos++];
        char up = src[pos++];
        char lo = src[pos++];

        codePoint ch = (codePoint) ((UX1000(b) << 18) | (UXR(md) << 12) | (UXR(up) << 6) | (UXR(lo)));

        if (ch >= 0x1000 && ch <= 0x1fffff) {
          *code = ch;
          *start = pos;
          return Ok;
        } else
          return Error;
      } else
        return Eof;
    } else
      return Error;
  } else
    return Eof;
}

retCode prevPoint(char *src, long *start, codePoint *code) {
  long pos = *start;

  if (pos > 0) {
    char b = src[--pos];

    if (b <= 0x7f) {
      *code = (codePoint) b;
      *start = pos;
      return Ok;
    } else {
      codePoint pt = 0;
      int factor = 0;
      while (UCR(b)) {
        pt = pt | (UXR(b) << factor);
        factor += 6;
        b = src[--pos];
      }
      if (UC80(b)) {
        *code = pt | (UX80(b) << factor);
        *start = pos;
        return Ok;
      } else if (UC800(b)) {
        *code = pt | (UX800(b) << factor);
        *start = pos;
        return Ok;
      } else if (UC1000(b)) {
        *code = pt | (UX1000(b) << factor);
        *start = pos;
        return Ok;
      } else
        return Error;
    }
  } else
    return Eof;
}

long countCodePoints(char *src, long start, long end) {
  long count = 0;

  while (start < end) {
    codePoint ch;

    if (nxtPoint(src, &start, end, &ch) == Ok)
      count++;
    else
      return count;
  }
  return count;
}

int codePointSize(codePoint ch) {
  if (ch > 0 && ch <= 0x7f)
    return 1;
  else if (ch <= 0x7ff)
    return 2;
  else if (ch >= 0x800 && ch <= 0xffff)
    return 3;
  else
    return 4;
}

long uniCodeCount(char *src) {
  long end = uniByteLen(src);

  return countCodePoints(src, 0, end);
}

long advanceCodePoint(char *src, long start, long end, long count) {
  while (count-- > 0 && start < end) {
    codePoint ch;
    if (nxtPoint(src, &start, end, &ch) == Ok)
      continue;
    else
      return -1;
  }
  return start;
}

codePoint nextCodePoint(char *src, long *start, long end) {
  codePoint ch;
  if (nxtPoint(src, start, end, &ch) == Ok)
    return ch;
  else
    return (codePoint) 0;
}

unsigned long uniStrLen(const char *s) {
  char *str = (char *) s;
  unsigned long count = 0;
  while (*str++ != 0)
    count++;
  return count;
}

logical isUniIdentifier(char *str) {
  long pos = 0;
  long end = uniByteLen(str);
  logical first = True;

  while (pos < end) {
    codePoint ch;
    if (nxtPoint(str, &pos, end, &ch) == Ok) {
      if (!(isLetterChar(ch) || (!first && isNdChar(ch))))
        return False;
      first = False;
    } else
      return False;
  }
  return first ? False : True; // empty strings are not identifiers
}

long uniByteLen(const char *s) {
  long len = 0;
  char *p = (char *) s;

  assert(s != NULL);

  while (*p++ != 0)
    len++;
  return len;
}

retCode uniCat(char *dest, long len, const char *src) {
  int pos = 0;
  char *tst = (char *) src;

  while (pos < len - 1 && dest[pos] != 0)
    pos++;

  while (pos < len - 1 && *src != 0)
    dest[pos++] = *tst++;
  dest[pos++] = 0;

  if (pos < len)
    return Ok;
  else
    return Eof;
}

retCode uniTackOn(char *dest, long len, codePoint ch) {
  long pos = 0;
  while (pos < len - 1 && dest[pos] != 0)
    pos++;

  if (appendCodePoint(dest, &pos, len, ch) == Ok) {
    if (pos < len) {
      dest[pos++] = 0;
      return Ok;
    } else
      return Eof;
  } else
    return Error;
}

retCode uniAppend(char *dest, long *pos, long len, char *src) {
  for (; *src != 0 && *pos < len;)
    dest[(*pos)++] = *src++;
  if (*pos < len - 1) {
    dest[*pos] = 0;
    return Ok;
  } else {
    return Eof;
  }
}

retCode uniNAppend(char *dest, long *pos, long len, char *src, long sLen) {
  for (long sx = 0; sx < sLen && *pos < len;)
    dest[(*pos)++] = *src++;
  if (*pos < len - 1) {
    dest[*pos] = 0;
    return Ok;
  } else {
    return Eof;
  }
}

retCode appendCodePoint(char *dest, long *pos, long len, codePoint ch) {
  if (ch <= 0x7f) {
    if ((*pos) < len - 1) {
      dest[(*pos)++] = (byte) ((ch) & 0x7f);
      return Ok;
    } else
      return Eof;
  } else if (ch <= 0x7ff) {
    if ((*pos) < len - 2) {
      dest[(*pos)++] = (byte) ((((ch) >> 6) & 0x1f) | U80);
      dest[(*pos)++] = (byte) (UXR(ch) | UR);
      return Ok;
    } else
      return Eof;
  } else if (ch >= 0x800 && ch <= 0xffff) {
    if ((*pos) < len - 3) {
      dest[(*pos)++] = (byte) ((((ch) >> 12) & 0xf) | U800);
      dest[(*pos)++] = (byte) (UXR(ch >> 6) | UR);
      dest[(*pos)++] = (byte) (UXR(ch) | UR);
      return Ok;
    } else
      return Eof;
  } else if (ch >= 0x10000 && ch <= 0x1fffff) {
    if ((*pos) < len - 4) {
      dest[(*pos)++] = (byte) ((((ch) >> 18) & 0xf) | U1000);
      dest[(*pos)++] = (byte) (UXR(ch >> 12) | UR);
      dest[(*pos)++] = (byte) (UXR(ch >> 6) | UR);
      dest[(*pos)++] = (byte) (UXR(ch) | UR);
      return Ok;
    } else
      return Eof;
  } else
    return Error;
}

retCode uniReverse(char *dest, long len) {
  for (long ix = 0; ix < len / 2; ix++) {
    char b = dest[ix];
    dest[ix] = dest[len - ix - 1];
    dest[len - ix - 1] = b;
  }
  return Ok;
}

retCode uniCpy(char *dest, long len, const char *src) {
  int pos = 0;
  char *s = (char *) src;

  while (pos < len - 1 && *src != 0)
    dest[pos++] = *s++;
  dest[pos] = 0;
  return pos < len ? Ok : Eof;
}

retCode uniNCpy(char *dest, long len, const char *src, long sLen) {
  long pos = 0;
  long max = (sLen < len - 1 ? sLen : len - 1);
  char *s = (char *) src;

  while (pos < max && *src != 0)
    dest[pos++] = *s++;
  dest[pos] = 0;
  return pos < len ? Ok : Eof;
}

comparison uniCmp(char *s1, char *s2) {
  long pos = 0;
  assert(s1 != NULL && s2 != NULL);

  while (s1[pos] == s2[pos]) {
    if (s1[pos] == 0)
      return same;
    pos++;
  }

  if (s1[pos] < s2[pos] || s1[pos] == 0)
    return smaller;
  else
    return bigger;
}

logical uniIsTail(char *s1, char *s2) {
  long len = 0;
  char *eS1 = uniEndStr(s1);

  while (*s2 != 0) {
    s2++;
    len++;
  }

  while (eS1 > s1 && len-- > 0) {
    if (*--eS1 != *--s2)
      return False;
  }
  return True;
}

retCode uniInsert(char *dest, long len, const char *src) {
  long iLen = uniStrLen(src);
  long dLen = uniStrLen(dest) + 1;

  assert(iLen + dLen < len);

  if (iLen + dLen < len) {
    long end = dLen + iLen;
    long pos = dLen;
    while (--pos > 0)              /* Shuffle up the old text */
      dest[--end] = dest[pos];

    for (pos = 0; pos < iLen; pos++)
      dest[pos] = src[pos];

    return Ok;
  }
  return Error;                  /* Bomb out */
}

comparison uniNCmp(char *s1, char *s2, long l) {
  long pos = 0;
  while (pos < l && s1[pos] == s2[pos]) {
    if (s1[pos] == 0)
      return same;
    pos++;
  }
  if (pos < l) {
    if (s2[pos] > s1[pos])
      return bigger;
    else
      return smaller;
  } else
    return same;
}

/* Tack on an ASCII string to the end of a unicode string */
/* This is only necessary 'cos C is not codePoint friendle */
retCode uniTack(char *dest, long len, const char *src) {
  int pos = 0;
  char *s = (char *) src;

  while (pos < len - 1 && dest[pos] != 0)
    pos++;

  while (pos < len - 1 && *s != 0)
    dest[pos++] = (byte) *s++;
  if (pos < len - 1)
    dest[pos++] = 0;

  return pos < len ? Ok : Eof;
}

long uniIndexOf(char *s, long len, long from, codePoint c) {
  long pos = from;

  while (pos < len) {
    codePoint ch;
    from = pos;
    if (nxtPoint(s, &pos, len, &ch) == Ok) {
      if (ch == c)
        return from;
    }
  }
  return -1;
}

long uniLastIndexOf(char *s, long len, codePoint c) {
  long lx = -1;
  long pos = 0;

  while (pos < len) {
    codePoint ch;
    long nxt = pos;
    if (nxtPoint(s, &nxt, len, &ch) == Ok) {
      if (ch == c) {
        lx = pos;
      }
      pos = nxt;
    }
  }
  return lx;
}

char *uniSubStr(char *s, long len, long from, long cnt, char *buff, long bLen) {
  char *src = &s[from];
  long ix;
  for (ix = 0; ix < cnt && ix < bLen; ix++) {
    buff[ix] = src[ix];
    if (src[ix] == 0)
      break;
  }
  if (ix < bLen)
    buff[ix] = '\0';
  return buff;
}

char *uniSearchAny(char *s, long len, char *term) {
  long pos = 0;
  long termSize = uniStrLen(term);

  while (pos < len) {
    codePoint ch;
    if (nxtPoint(term, &pos, termSize, &ch) == Ok) {
      long index = uniIndexOf(s, len, 0, ch);
      if (index >= 0)
        return &s[index];
    }
  }
  return NULL;
}

codePoint uniSearchDelims(char *s, long len, char *t) {
  long tlen = uniStrLen(t);
  long tSize = countCodePoints(t, 0, tlen);

  codePoint terms[tSize];
  long ti = 0;

  for (long tx = 0; tx < tSize;) {
    terms[ti++] = nextCodePoint(t, &tx, tSize);
  }

  for (long ix = 0; ix < len;) {
    codePoint ch = nextCodePoint(s, &ix, len);
    for (long dx = 0; dx < tSize; dx++) {
      if (terms[dx] == ch) {
        terms[dx] = 0;
        break;
      }
    }
  }

  for (long dx = 0; dx < tSize; dx++) {
    if (terms[dx] != 0)
      return terms[dx];
  }
  return 0;
}

// This is a poor algorithm. Fix me with Boyer-Moore or better
long uniSearch(char *src, long len, long start, char *tgt, long tlen) {
  long pos = start;

  while (pos < len - tlen) {
    if (uniNCmp(&src[pos], tgt, tlen) == same)
      return pos;
    else
      pos++;
  }
  return -1;
}

char *uniLast(char *s, long l, codePoint c) {
  long last = uniLastIndexOf(s, l, c);

  if (last >= 0)
    return &s[last];
  else
    return NULL;
}

logical uniIsLit(char *s1, char *s2) {
  long pos = 0;
  while (s2[pos] != 0 && s1[pos] == s2[pos])
    pos++;

  return (logical) (s2[pos] == 0 && s1[pos] == 0);
}

logical uniIsLitPrefix(char *s1, char *s2) {
  long pos = 0;
  while (s2[pos] != '\0' && s1[pos] == s2[pos])
    pos++;

  return (logical) (s2[pos] == 0);
}

uinteger uniHash(const char *name) {
  register uinteger hash = 0;
  char *s = (char *) name;

  while (*s) {
    hash = hash * 37 + *s++;
  }

  return hash;
}

uinteger uniNHash(const char *name, long len) {
  register uinteger hash = 0;
  char *s = (char *) name;

  for (long ix = 0; ix < len; ix++)
    hash = hash * 37 + *s++;

  return hash;
}

char *uniEndStr(char *s) {
  while (*s != 0)
    s++;
  return s;
}

retCode uniLower(char *s, long sLen, char *d, long dLen) {
  long sPos = 0;
  long dPos = 0;

  while (sPos < sLen && dPos < dLen) {
    codePoint ch;
    if (nxtPoint(s, &sPos, sLen, &ch) == Ok) {
      appendCodePoint(d, &dPos, dLen, lowerOf(ch));
    } else
      return Error;
  }
  if (dPos < dLen - 1) {
    d[dPos] = 0;
    return Ok;
  } else
    return Eof;
}

char *uniDuplicate(char *s) {
  size_t len = uniStrLen(s);
  char *copy = (char *) malloc((len + 1) * sizeof(byte));

  memcpy(copy, s, len + 1);
  return copy;
}
