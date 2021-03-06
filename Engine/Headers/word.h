/* 
  Main memory layout definitions for the L&O engine
  Copyright (c) 2016, 2017. Francis G. McCabe

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
  except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the
  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied. See the License for the specific language governing
  permissions and limitations under the License.
 */

#ifndef _ENGINE_WORD_H_
#define _ENGINE_WORD_H_

#include "config.h"
#include "logical.h"		/* import a definition of true and false */
#include "integer.h"
#include "retcode.h"
#include "io.h"
#include <math.h>
#include <assert.h>
#include <stdlib.h>

/* Standard constant definitions */

/* This is used for many internal fixed char buffers */
#ifndef MAX_SYMB_LEN
#define MAX_SYMB_LEN 1024
#endif

/* This is used for some internal stack buffers */
#ifndef MAX_DEPTH
#define MAX_DEPTH 512
#endif

/* This is used for some internal tables */
#ifndef MAX_TABLE
#define MAX_TABLE 2048
#endif

/* This is used for message buffers */
#ifndef MAX_MSG_LEN
#define MAX_MSG_LEN 1024
#endif

#ifndef LO_REGS			    /* do we know how many registers? */
#define LO_REGS 64		  /* should'nt be more than #bits in an integer*/
#endif

/* BE VERY CAREFUL if you change this .... */
/* Definition of a cell/pointer for the L&O run-time engine
 * The value pointed at is preceded by a descriptor word which contains
 * a tag and possible attribute value
 */

/* As with nearly every Prolog system out there, the L&O engine uses a tagged
   pointer architecture. However, this one uses both tagged pointer and tagged 
   cell. This is to minimize the in-pointer tags (to 2 bits) and to maximize 
   potential for expansion.

   The two pointer bits are in the bottom (lsb) of the pointer, they tell us:

   00 - a variable, or a variable-variable reference
   01 - object, pointer is a pointer to a tagged structure
   10 - forward pointer, GC has moved the object somewhere
*/

/* The idea is this:
   A ptrPo points to a ptrI, which is a pointer with a tag in the lower 2 bits.
   A stripped ptrI becomes an objPo, which is normally a pointer to a structure.

   The first word in a structure must its class, whose class is class. This is
   the sentinel for the beginning of all structures.

   The special case is variable, which may point *into* structure as well as
   to the beginning of the structure. In the case that a variable points
   into a structure, it is the result of a variable-variable binding.

   The first word in the structure is actually a pointer to a class structure.
   This class structure is used to decode the following sequence of words.

   There are two cases of structure:
   1. A term class constructor which consists of N tagged pointer words. The arity
   N comes from the class structure.
   2. A special object. The class structure has functions that define the layout
   of the object. Examples include symbols, integer, float, char, code, dynamic objects

   A ptrI is defined to be an integer which is long enough to hold a pointer.
   The assumption that we make is that a pointer is at least 4 bytes, but is typically 8.
*/
typedef PTRINT ptrI, *ptrPo;    /* PTRINT normally generated during config */
typedef struct _general_record_ *objPo;  /* a core pointer */

typedef struct _class_record_ *clssPo;
typedef struct _program_record_ *programPo;
typedef struct _special_class_ *specialClassPo;

typedef struct _heap_rec_ *heapPo;

#define TAG_SHFT 2
#define TAG_MASK ((1<<TAG_SHFT)-1)
#define PTR_MASK (~TAG_MASK)

typedef enum {
  varTg = 0,                            // A variable pointer
  objTg = 1,                            // An object
  fwdTg = 2        // A forwarded structure
} ptrTg;

#define ptg(p) ((ptrTg)(((ptrI)(p))&TAG_MASK))
#define isvar(p) (ptg(p)==varTg)
#define isobj(p) (ptg(p)==objTg)
#define isfwd(p) (ptg(p)==fwdTg)

#define ptrP(p, t)  (((ptrI)(p))|(t))
#define varP(p)  ptrP(p,varTg)
#define objP(p)  ptrP(p,objTg)
#define fwdP(p)  ptrP(p,fwdTg)

#define objV(p) ((objPo)(((ptrI)(p))&PTR_MASK))

typedef struct _general_record_ {
  ptrI class;        // basic object has a class
  ptrI args[ZEROARRAYSIZE];
} GeneralRec;

// Some typedefs to help with working with classes

typedef long (*classSizeFun)(specialClassPo class, objPo o);

typedef uinteger (*classHashFun)(specialClassPo class, objPo o);

typedef comparison (*classCompFun)(specialClassPo class, objPo o1, objPo o2);

typedef retCode (*classOutFun)(specialClassPo class, ioPo out, objPo o1);

typedef retCode (*specialHelperFun)(ptrPo arg, void *c);

typedef retCode (*classScanFun)(specialClassPo class, specialHelperFun helper, void *c, objPo o);

typedef objPo (*classCpyFun)(specialClassPo class, objPo dst, objPo src);


/*
 * A program label structure
 */

typedef struct _program_label_ {
  long arity;                   // Arity of program
  char name[ZEROARRAYSIZE];     // Program's print name
} PrgLabel, *prgLabelPo;


comparison compPrgLabel(prgLabelPo p1, prgLabelPo p2);
uinteger hashPrgLabel(prgLabelPo p);

// The sign of a general_record points to a class object ...
typedef struct _class_record_ {
  ptrI class;               /* == classClass */
  uinteger hash;            /* the hash code for this class */
  PrgLabel lbl;             // The class label
} clssRec;

// Special classes mostly refer to special system objects
typedef struct _special_class_ {
  ptrI class;                  /* == specialClass */
  classSizeFun sizeFun;        /* Function to compute size of object */
  classCompFun compFun;        /* Function to compare two values */
  classOutFun outFun;          /* Function to write a value */
  classCpyFun copyFun;         /* Function to copy special object */
  classScanFun scanFun;        /* Function to scan object */
  classHashFun hashFun;        /* Function to compute hash function */
  ptrI program;                /* Program that responds to this type */
  char name[ZEROARRAYSIZE];    /* the class's print name */
} specialClassRec;

#define PTRSZE sizeof(ptrI)

#define ALIGNPTR(count, size) (((count+size-1)/size)*(size))
#define CellCount(size) (ALIGNPTR(size,PTRSZE)/PTRSZE)
#ifndef ALIGNED
#define ALIGNED(ptr, size) (((((ptrI)ptr)+size-1)/size)*(size)==(ptrI)ptr)
#endif

extern ptrI classClass, specialClass, programClass;

static inline logical isClass(ptrI p) {
  return (logical) (((clssPo) objV(p))->class == classClass);
}

static inline logical isSpecialClass(clssPo cl) {
  return (logical) (cl->class == specialClass);
}

static inline logical IsSpecialClass(ptrI p) {
  return isSpecialClass((clssPo) objV(p));
}

static inline logical isTermClass(objPo cl) {
  return (logical) (cl->class == classClass);
}

static inline logical IsTermClass(ptrI p) {
  return isTermClass(objV(p));
}

#define isObjct(o) (isClass((o)->class))

static inline logical isObjBase(ptrPo p) {
  if (isobj(*p)) {
    objPo o = objV(*p);

    return (logical) (o->class == classClass);
  }
  else
    return False;
}

static inline logical isSpecialObject(objPo o) {
  return IsSpecialClass(o->class);
}

static inline specialClassPo sClassOf(objPo o) {
  ptrI class = o->class;
  assert(IsSpecialClass(class));
  return (specialClassPo) objV(class);
}

/*
 * Find the class of an object
 */
static inline clssPo classOf(objPo p) {
  return (clssPo) objV(p->class);
}

static inline clssPo ClassOf(ptrI X) {
  return classOf(objV(X));
}

static inline logical hasClass(objPo p,ptrI class)
{
  return (logical)(p->class==class);
}

static inline logical HasClass(ptrI x,ptrI class)
{
  return (logical)(isobj(x) && hasClass(objV(x),class));
}

static inline long objectSize(objPo p) {
  clssPo class = classOf(p);

  assert(class->class == classClass);

  return class->lbl.arity + 1;
}

static inline long specialSize(objPo p) {
  specialClassPo spClass = (specialClassPo) objV(p->class);

  assert(spClass->class == specialClass);

  return spClass->sizeFun(spClass, p);
}

static inline ptrI specialProgram(objPo p) {
  specialClassPo spClass = (specialClassPo) objV(p->class);

  assert(spClass->class == specialClass);

  return spClass->program;
}

static inline long objectArity(objPo o) {
  clssPo class = classOf(o);

  return class->lbl.arity;
}

static inline long classArity(clssPo class)
{
  return class->lbl.arity;
}

static inline ptrPo objectArgs(objPo o)
{
  assert(isObjct(o));

  return o->args;
}

static inline ptrPo nthArg(objPo o, long ix) {
  assert(ix >= 0 && ix < objectArity(o));

  return objectArgs(o) + ix;
}

static inline void updateArg(objPo o, long ix, ptrI val) {
  assert(ix >= 0 && ix < objectArity(o));

  ptrPo args = objectArgs(o);

  args[ix] = val;
}

static inline char * objectClassName(objPo p) {
  clssPo class = classOf(p);
  return class->lbl.name;
}

static inline char * className(clssPo class) {
  return class->lbl.name;
}

static inline prgLabelPo classLabel(clssPo clss){
  return &clss->lbl;
}

static inline uinteger objectHash(objPo p) {
  return ((clssPo) objV(p->class))->hash;
}

static inline integer classSize(clssPo clss){
  return clss->lbl.arity + 1;
}

extern logical IsBinOp(ptrPo p, ptrI key, ptrPo a1, ptrPo a2);

extern ptrI newClassDef(char * name, long arity);
extern ptrI newClassDf(const char *name, long arity);

extern ptrI newSpecialClass(const char *name,
                            classSizeFun sizeFun, classCompFun compFun,
                            classOutFun outFun, classCpyFun copyFun,
                            classScanFun scanFun, classHashFun hashFun);

extern void initClass(void);

extern void standardClasses(void);

extern void installClass(clssPo class);

extern ptrI classPresent(char * name);


#endif
