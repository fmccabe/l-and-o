/*
  Header file for  string management in the L&O engine
  Copyright (c) 2016, 2017. Francis G. McCabe

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
  except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the
  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied. See the License for the specific language governing
  permissions and limitations under the License.
*/

#ifndef _ENGINE_STR_H_
#define _ENGINE_STR_H_

/* Symbol structure */
typedef struct _string_record_ {
  ptrI class;                             // == stringClass
  long size;                              // Length of the string
  char data[ZEROARRAYSIZE];               // The string contents
} stringRec, *strBuffPo;

extern ptrI stringClass;

void initStringClass(void);

static inline logical IsString(ptrI o) {
  return HasClass(o, stringClass);
}

static inline logical isString(objPo p) {
  return hasClass(p, stringClass);
}

static inline strBuffPo stringV(ptrI x) {
  assert(IsString(x));
  return (strBuffPo) objV(x);
}

static inline char * stringVal(strBuffPo p) {
  assert(isString((objPo) p));

  return p->data;
}

static inline char * StringVal(strBuffPo p) {
  assert(isString((objPo) p));

  return p->data;
}

static inline long stringLen(strBuffPo p) {
  assert(isString((objPo) p));
  return p->size;
}

static inline uinteger stringHash(objPo p) {
  clssPo class = classOf(p);

  assert(hasClass(p, stringClass));

  specialClassPo sClass = (specialClassPo) class;
  return sClass->hashFun(sClass, p);
}

ptrI allocateString(heapPo H, const char *buff, long len);
ptrI allocateCString(heapPo H, const char *buff);

retCode copyString2Buff(char *buffer, long bLen, strBuffPo s);
retCode strPrepare(char * tgt, long tLen, char * src, long sLen, codePoint pad, logical left, long width);
retCode closeOutString(ioPo f, heapPo P, ptrPo tgt);

retCode explodeString(processPo P, char *text, long length, ptrPo a);
retCode implodeString(ptrPo l, ioPo out);
#endif
