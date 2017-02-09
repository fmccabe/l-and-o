/* 
 Class management module
 (c) 2000-2017 F.G. McCabe

 Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 except in compliance with the License. You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software distributed under the
 License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied. See the License for the specific language governing
 permissions and limitations under the License.
 */

#include "config.h"		/* pick up standard configuration header */
#include <string.h>
#include <stdlib.h>

#include "lo.h"
#include "manifestP.h"
#include "thread.h"

#ifndef MAX_FILE_LEN
#define MAX_FILE_LEN 2048
#endif

#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN 4096
#endif

/* Dictionary of known classes ... */
static hashPo classes, specialClasses;

/* Standard constructor templates */
ptrI classClass;
ptrI specialClass;
ptrI programClass;

/* Standard classes */
ptrI emptyList;  //  the empty list
ptrI nilClass;  //  The empty list class
ptrI listClass;  //  the non-empty list

ptrI thingClass;  //  root type/class
ptrI thingProg;  //  The thing program label

ptrI integerClass;  //  The integers
ptrI floatClass;  //  F.P. numbers
ptrI symbolClass;  //  Standard symbol class
ptrI stringClass;

ptrI trueClass, falseClass;  //  True and False

static long clSizeFun(specialClassPo class, objPo o);
static comparison clCompFun(specialClassPo class, objPo o1, objPo o2);
static retCode clOutFun(specialClassPo class, ioPo out, objPo o);
static retCode clScanFun(specialClassPo class, specialHelperFun helper, void *c, objPo o);
static uinteger clHashFun(specialClassPo class, objPo o);
static objPo clCopyFun(specialClassPo class, objPo dst, objPo src);

static long spSizeFun(specialClassPo class, objPo o);
static comparison spCompFun(specialClassPo class, objPo o1, objPo o2);
static retCode spOutFun(specialClassPo class, ioPo out, objPo o);
static retCode spScanFun(specialClassPo class, specialHelperFun helper, void *c, objPo o);
static objPo spCopyFun(specialClassPo class, objPo dst, objPo src);
static uinteger spHashFun(specialClassPo class, objPo o);

void initClass() {
  classes = NewHash(256, (hashFun) uniHash, (compFun) uniCmp, NULL);
  specialClasses = NewHash(256, (hashFun) uniHash, (compFun) uniCmp, NULL);

  // The first class record has to be created carefully
  specialClass = newSpecialClass("#special", spSizeFun, spCompFun, spOutFun, spCopyFun, spScanFun, spHashFun);
  specialClassPo sClass = (specialClassPo) objV(specialClass);

  classClass = newSpecialClass("#class", clSizeFun, clCompFun, clOutFun, clCopyFun, clScanFun, clHashFun);
  specialClassPo class = (specialClassPo) objV(classClass);

  class->class = specialClass; /* class is a special class */
  sClass->class = specialClass; /* specialClass is a special class! */
}

// Return the size of a class object
static long clSizeFun(specialClassPo class, objPo o) {
  assert(o != NULL);
  assert(o->class == classClass);

  clssPo cl = (clssPo) o;
  return CellCount(sizeof(clssRec) + sizeof(byte) * (uniStrLen(cl->name) + 1));
}

static comparison clCompFun(specialClassPo class, objPo o1, objPo o2) {
  if (o1 == o2)
    return same;
  else
    return incomparible;
}

static retCode clOutFun(specialClassPo class, ioPo out, objPo o) {
  clssPo cl = (clssPo) o;

  return outMsg(out, "%U/%d", cl->name, cl->arity);
}

static retCode clScanFun(specialClassPo class, specialHelperFun helper, void *c, objPo o) {
  return Ok;
}

static objPo clCopyFun(specialClassPo class, objPo dst, objPo src) {
  long size = clSizeFun(class, src);
  memmove((void *) dst, (void *) src, size * sizeof(ptrI));

  return (objPo) (((ptrPo) dst) + size);
}

static uinteger clHashFun(specialClassPo class, objPo o) {
  clssPo cl = (clssPo) o;

  return cl->hash;
}

static long spSizeFun(specialClassPo class, objPo o) {
  assert(o->class == specialClass);

  specialClassPo cl = (specialClassPo) o;

  return CellCount(sizeof(specialClassRec) + sizeof(byte) * (strlen((char *) cl->name) + 1));
}

static comparison spCompFun(specialClassPo class, objPo o1, objPo o2) {
  if (o1 == o2)
    return same;
  else
    return incomparible;
}

static retCode spOutFun(specialClassPo class, ioPo out, objPo o) {
  specialClassPo cl = (specialClassPo) o;

  return outMsg(out, "%U", cl->name);
}

static retCode spScanFun(specialClassPo class, specialHelperFun helper, void *c, objPo o) {
  return Ok;
}

static objPo spCopyFun(specialClassPo class, objPo dst, objPo src) {
  long size = spSizeFun(class, src);
  memmove((void *) dst, (void *) src, size * sizeof(ptrI));

  return (objPo) (((ptrPo) dst) + size);
}

static uinteger spHashFun(specialClassPo class, objPo o) {
  specialClassPo cl = (specialClassPo) o;

  return uniHash(cl->name);
}

// This is called to set up the standard classes in the system
void standardClasses(void) {
  kvoid = newEnumSym("lo.core#void");

  thingClass = newClassDf("lo.core#thing", 0);
  thingProg = newProgLbl("lo.core#thing", 3);

  emptyList = newEnumSym("lo.core#[]");
  nilClass = newClassDf("lo.core#[]", 0);
  listClass = newClassDf("lo.core#,..", 2);

  suspClass = newClassDf("#suspension", 3);
  varClass = newClassDf("#var", 1);

  trueClass = newEnumSym("lo.core#true");
  falseClass = newEnumSym("lo.core#false");

  initCodeClass();
  initSymbolClass();
  initStringClass();
  initDynamicClass();
  initIntegerClass();
  initFloatClass();
  initThreadClass();
  initProcessClss();
}

ptrI newClassDef(const string name, long arity) {
  ptrI def = (ptrI) hashGet(classes, (void *) name);

  if (objV(def) == NULL) {
    long symlen = uniStrLen(name);
    long len = CellCount(sizeof(clssRec) + sizeof(byte) * (symlen + 1));
    clssPo new = (clssPo) permAllocate(len);

    new->class = classClass;
    new->hash = uniHash(name);
    new->arity = arity;

    memcpy(new->name, name, (symlen + 1) * sizeof(byte));

    def = objP(new);

    hashPut(classes, className(new), (void *) def);
    return def;
  } else {
    assert(classArity((clssPo) objV(def)) == arity);
    return def;
  }
}

void installClass(clssPo class) {
  string name = className(class);
  ptrI cl = (ptrI) hashGet(classes, name);

  if (objV(cl) == NULL)
    hashPut(classes, name, (void *) objP(class));
}

ptrI classPresent(string name) {
  return (ptrI) hashGet(classes, name);
}

ptrI newClassDf(const char *name, long arity) {
  return newClassDef((string) name, arity);
}

ptrI newEnumSym(const char *fun) {
  ptrI eClass = newClassDf(fun, 0);

  return objP(allocateObject(&globalHeap, eClass));
}

ptrI newEnumSymbol(const string fun) {
  ptrI eClass = newClassDef(fun, 0);

  return objP(allocateObject(&globalHeap, eClass));
}

ptrI newSpecialClass(const char *name, classSizeFun sizeFun, classCompFun compFun, classOutFun outFun,
                     classCpyFun copyFun, classScanFun scanFun, classHashFun hashFun) {
  size_t slen = strlen(name);
  byte buff[slen + 1];

  strncpy((char *) buff, name, slen);

  specialClassPo new = (specialClassPo) malloc(
    sizeof(specialClassRec) + (uniStrLen(buff) + 1) * sizeof(byte));

  new->class = specialClass;
  new->sizeFun = sizeFun;
  new->compFun = compFun;
  new->outFun = outFun;
  new->copyFun = copyFun;
  new->scanFun = scanFun;
  new->hashFun = hashFun;

  memcpy(&new->name, buff, (uniStrLen(buff) + 1) * sizeof(byte));

  hashPut(specialClasses, &new->name, (void *) objP(new));

  if (name[0] != '#')
    new->program = defineSpecialProg(name);
  else
    new->program = 0;

  return objP(new);
}

/* remove all entries from the class directory */
typedef struct {
  globalGcPo G;
  hashPo newDict;
} DInfoRec;

static retCode remClass(void *n, void *r, void *c) {
  DInfoRec *I = (DInfoRec *) c;
  ptrI S = (ptrI) r;

  hashRemove(classes, n);

  objPo o = objV(S);

  /* This fragment allows code to be garbage collected - except for code loaded as part of a package */
  if (oldGeneration(o))
    hashPut(I->newDict, className((clssPo) objV(S)), (void *) S); /* put symbol directly into the new dictionary */
  else
    scanPtr(I->G, S); /* We keep defined programs FIXME */

  return Ok;
}

static retCode markSpecialClass(void *n, void *r, void *c) {
  DInfoRec *I = (DInfoRec *) c;
  specialClassPo spClass = (specialClassPo) objV(r);

  spClass->program = scanPtr(I->G, spClass->program); /* We keep special programs */

  return Ok;
}

void markStandardClasses(globalGcPo G) {
  DInfoRec help = {G, NewHash(256, (hashFun) uniHash, (compFun) uniCmp, NULL)};
  hashPo currClassTable = classes;

  classes = help.newDict;
  ProcessTable(remClass, currClassTable, &help);

  thingClass = scanPtr(G, thingClass);

  emptyList = scanPtr(G, emptyList);
  nilClass = scanPtr(G, nilClass);
  listClass = scanPtr(G, listClass);
  errorClass = scanPtr(G, errorClass);

  /*
   integerClass = scanPtr(G,integerClass);
   floatClass = scanPtr(G,floatClass);
   symbolClass = scanPtr(G,symbolClass);

   filePtrClass = scanPtr(G,filePtrClass);
   threadClass = scanPtr(G,threadClass);
   procClass = scanPtr(G,procClass);

   dynamicClass = scanPtr(G,dynamicClass);
   */

  suspClass = scanPtr(G, suspClass);
  varClass = scanPtr(G, varClass);
  trueClass = scanPtr(G, trueClass);
  falseClass = scanPtr(G, falseClass);

  ProcessTable(markSpecialClass, specialClasses, &help);
}

static string computeClassFileName(string path, long pathLen, string className, string version, string fn,
                                   long fLen);

retCode g__ensure_loaded(processPo P, ptrPo a) {
  ptrI pname = deRefI(&a[1]);

  if (isvar(pname))
    return liberror(P, "__ensure_loaded", eINSUFARG);
  else if (!IsSymb(pname))
    return liberror(P, "__ensure_loaded", eINVAL);
  else {
    if (isLoaded(pname))
      return Ok;
    else {
      heapPo H = &P->proc.heap;

      switchProcessState(P, wait_io); /* Potentially nec. to wait */

      retCode ret = loadPkg(stringVal(stringV(pname)), stringVal(stringV(deRefI(&a[2]))), P->proc.errorMsg,
                            NumberOf(P->proc.errorMsg));
      setProcessRunnable(P);

      switch (ret) {
        case Error: {
          byte msg[MAX_MSG_LEN];

          strMsg(msg, NumberOf(msg), "__ensure_loaded: %#w in %#w", &a[2], &a[1]);
          return raiseError(P, msg, eNOTFND);
        }
        case Eof: {
          byte msg[MAX_MSG_LEN];

          strMsg(msg, NumberOf(msg), "__ensure_loaded: %#w in %#w", &a[2], &a[1]);
          return raiseError(P, msg, eNOFILE);
        }
        case Ok:
          return Ok;
        case Fail:
          return Fail;
        case Space:
          outMsg(logFile, "Out of heap space, increase and try again\n%_");
          return liberror(P, "__ensure_loaded", eSPACE);
        default:
          return liberror(P, "__ensure_loaded", eINVAL);
      }
    }
  }
}


