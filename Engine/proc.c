/*
  Process control functions for the L&O run-time system
  Copyright (c) 2016, 2017. Francis G. McCabe

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
  except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the
  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied. See the License for the specific language governing
  permissions and limitations under the License.
*/
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "lo.h"			/* Main april header file */
#include "thred.h"

#ifndef DEFSTACK
#define DEFSTACK 1024    /* default initial size of process stack */
#endif

#ifndef VERIFYDEPTH
#define VERIFYDEPTH 5    /* verify to a depth of 5 */
#endif

#ifdef MEMTRACE
long extendCount = 0;
#endif

typedef struct {
  ptrI class;             /* == procClass */
  processPo process;      /* the process itself */
} procRec, *prPo;

ptrI procClass;

static const char *state_names[] = {"lo.thread#quiescent",
                                    "lo.thread#runnable",
                                    "lo.thread#wait_io",
                                    "lo.thread#wait_term",
                                    "lo.thread#wait_timer",
                                    "lo.thread#wait_lock",
                                    "lo.thread#wait_child",
                                    "lo.thread#wait_rendezvous",
                                    "lo.thread#in_exclusion",
                                    "lo.thread#dead"};

static void inheritProcess(classPo class, classPo request);
static void initProcessClass(classPo class, classPo request);
static void processInit(objectPo o, va_list *args);
static void closeProcess(objectPo o);

ProcessClassRec ProcessClass = {
  {
    (classPo) &ManagedClass,              /* parent class is managed */
    "process",                            /* this is the process class */
    inheritProcess,                       /* deal with inheritance */
    initProcessClass,                     /* Process class initializer */
    O_INHERIT_DEF,                        /* Process object element creation */
    closeProcess,                         /* Process object destruction */
    O_INHERIT_DEF,                        /* erasure */
    processInit,                          /* initialization of a process object */
    sizeof(ProcessObjectRecord),          /* size of a process record */
    NULL,                                  /* pool of values for this class */
    O_INHERIT_DEF,                        /* inherit process hash */
    O_INHERIT_DEF,                        /* inherit process equality */
    PTHREAD_ONCE_INIT,                    /* not yet initialized */
    PTHREAD_MUTEX_INITIALIZER,
  },
  {},
  {
    NULL                                  /* Managed set */
  },
  {
    0                                     /* No live processes yet */
  }
};

classPo processClass = (classPo) &ProcessClass;

/* This is the class identifier for processes */

static void inheritProcess(classPo class, classPo request) {
  //  ProcessClassRec *req = (ProcessClassRec*)request;
  //  ProcessClassRec *template = (ProcessClassRec*)class;
}

static void initProcessClass(classPo class, classPo request) {
  ProcessClassRec *rqClass = (ProcessClassRec *) request;

  rqClass->proc.liveProcesses = 0;
  pthread_key_create(&rqClass->proc.processKey, NULL);  /* create the processKey */
}

static void processInit(objectPo o, va_list *args) {
  ptrI thread = va_arg(*args, ptrI);
  processPo p = O_PROCESS(o);
  ProcessClassRec *pClass = (ProcessClassRec *) o->class;
  callPo c;
  choicePo b;

  /* allocate heap & stack */
  setupHeap(&p->proc.heap, p, DEFSTACK + initStackHeapSize);
  p->proc.heap.end = &p->proc.heap.base[initStackHeapSize]; /* reset max */
  p->proc.sBase = (ptrPo) p->proc.heap.end;

  p->proc.sTop = &p->proc.sBase[DEFSTACK]; /* DEFSTACK is the stack space */

  /* Set up initial conditions in the stack */

  /* Set up an ultimate success frame -- kills the process */
  c = ((callPo) p->proc.sTop) - 1;              /* top frame */
  c->cPC = FirstInstruction(dieProg);       /* a die instruction */
  c->cSB = (choicePo) p->proc.sTop;           /* slashback */
  c->C = (callPo) p->proc.sTop;
  c->cPROG = dieProg;

  b = ((choicePo) c) - 1;
  /* we have a NULL backtrack point */
  b->AX = 0;                            /* No argument registers */
  b->PC = FirstInstruction(dieProg);     // If we exit here, we die
  b->cPC = c->cPC;
  b->B = (choicePo) p->proc.sTop;
  b->cSB = (choicePo) p->proc.sTop;
  b->T = (choicePo) p->proc.sTop;    // If we raise an exception, we log a default message
  b->cPROG = dieProg;
  b->PROG = dieProg;
  b->trail = p->proc.trail = (trailPo) p->proc.sBase;
  b->C = c;                             /* succeed forever */
  b->H = p->proc.heap.create;

  /* fill in the process structure from this */
  p->proc.B = p->proc.SB = p->proc.cSB = b;
  p->proc.T = b;
  p->proc.C = c;
  p->proc.cPROG = b->PROG;
  p->proc.cPC = c->cPC;
  p->proc.trigger = emptyList;    /* no triggers, yet */

  p->proc.state = quiescent;                 /* not yet executing */
  p->proc.pauseRequest = False;  /* This will go true when a pause is requested */
  p->proc.cl = NULL;                         /* no client data yet */

#ifdef EXECTRACE
  p->proc.waitFor = p->proc.cWaitFor = nextBreak;
#endif

  p->proc.thread = thread;

  tryAgain:
  switch (pthread_cond_init(&p->proc.cond, NULL)) {
    case 0:break;
    case EAGAIN:goto tryAgain;
    default:syserr("cannot init lock");
  }

  pClass->proc.liveProcesses++;

  assert(getProcessVal(p->proc.thread) == NULL);

  setProcess(p->proc.thread, p);
}

pthread_t ps_threadID(processPo P) {
  return P->proc.threadID;
}

static void closeProcess(objectPo o) {
  processPo p = O_PROCESS(o);
  ptrI thread = p->proc.thread;
  ProcessClassRec *pClass = (ProcessClassRec *) o->class;

  clearProcess(thread);

  free(p->proc.heap.base);         /* Dispose of the old evaluation stack */
  pClass->proc.liveProcesses--;

  p->proc.state = dead;                 /* mark this process as a zombie */
  p->proc.thread = kvoid;                 /* clear the thread structures */
}

static long prSizeFun(specialClassPo class, objPo o);
static comparison prCompFun(specialClassPo class, objPo o1, objPo o2);
static retCode prOutFun(specialClassPo class, ioPo out, objPo o);
static retCode prScanFun(specialClassPo class, specialHelperFun helper, void *c, objPo o);
static objPo prCopyFun(specialClassPo class, objPo dst, objPo src);
static uinteger prHashFun(specialClassPo class, objPo o);

void initProcessClss(void) {
  procClass = newSpecialClass("#process", prSizeFun, prCompFun,
                              prOutFun, prCopyFun, prScanFun, prHashFun);
}

static long prSizeFun(specialClassPo class, objPo o) {
  return CellCount(sizeof(procRec));
}

static comparison prCompFun(specialClassPo class, objPo o1, objPo o2) {
  if (o1 == o2)
    return same;
  else
    return incomparible;
}

static retCode prOutFun(specialClassPo class, ioPo out, objPo o) {
  return outMsg(out, "process[%x]", o);
}

static retCode prScanFun(specialClassPo class, specialHelperFun helper, void *c, objPo o) {
  return Ok;
}

static objPo prCopyFun(specialClassPo class, objPo dst, objPo src) {
  long size = prSizeFun(class, src);
  memmove((void *) dst, (void *) src, size * sizeof(ptrI));

  return (objPo) (((ptrPo) dst) + size);
}

static uinteger prHashFun(specialClassPo class, objPo o) {
  return (uinteger) o;
}

retCode processProcesses(procProc proc, void *cl) {
  return processAll(processClass, (manageProc) proc, cl);
}

processPo rootProcess(ptrI thread, char * pkg) {
  processPo P = O_PROCESS(newObject(processClass, thread));
  heapPo H = &P->proc.heap;
  rootPo root = gcAddRoot(H, &thread);

  P->proc.A[1] = allocateString(H, pkg, uniStrLen(pkg)); /* class path */
  P->proc.A[2] = cmdLineOptions(H);  /* The command line options */
  P->proc.A[3] = commandLine(H);  /* Command line arguments */

  P->proc.PROG = dieProg;  // Is updated outside for the root process
  P->proc.PC = FirstInstruction(P->proc.PROG);

  gcRemoveRoot(H, root);

  P->proc.state = runnable;
  P->proc.threadID = pthread_self();

  return P;        /* Our root thread is now running */
}

/*
 *  Terminate a process 
 */

void ps_kill(processPo p) {
  if (p != NULL) {
    pthread_t thread = p->proc.threadID;

    pthread_cancel(thread);    /* cancel the thread */
  }
}

process_state ps_state(processPo p) {
  return p->proc.state;
}

void *ps_client(processPo p)  /* Get the process's client information */
{
  return p->proc.cl;
}

void *ps_set_client(processPo p, void *cl) {
  void *c = p->proc.cl;    /* old value */
  p->proc.cl = cl;
  return c;
}

/*********************************************************************/
/*                Display Processes                                  */
/*********************************************************************/
void displayProcess(processPo p) {
  outMsg(logFile, "%w [%s]\n", &p->proc.thread, state_names[p->proc.state]);
  if (!identical(p->proc.trigger, emptyList))
    outMsg(logFile, "Trigger queue = %w\n", &p->proc.trigger);
}

static retCode dispP(managedPo o, void *cl) {
  processPo p = O_PROCESS(o);
  displayProcess(p);
  return Ok;
}

void displayProcesses(void) {
  processAll(processClass, dispP, NULL);
  flushOut();
}

static long findPtr(void **list, long count, void *tgt) {
  long i;
  for (i = 0; i < count; i++)
    if (list[i] == tgt)
      return i;
  return -1;
}

/* Display the whole stack trace of a process */
void stackTrace(processPo p) {
  if (p->proc.state != dead) {    /* only process live processes */
    register callPo C = p->proc.C;
    register callPo lastC = (callPo) p->proc.sTop; /* last call block processed */
    register choicePo B = p->proc.B;
    register choicePo T = p->proc.T;
    register insPo cPC = p->proc.cPC;
    register int len = envSize(p->proc.cPC);
    register trailPo trail = p->proc.trail;
    long cLevel = 0, bLevel = 0;
    callPo callPoints[1024];    /* An arbitrary number of call points */
    choicePo choicePoints[1024];
    long Cix = 0, Bix = 0;      /* Temp inidices into the arrays */

    /* Compute the array of callPoints and choicePoints to make life a little easier */
    callPo Cx = C;
    callPo lastCx = lastC;
    choicePo Bx = B;

    while (Bx < (choicePo) p->proc.sTop || Cx < (callPo) p->proc.sTop) {

      assert((ptrPo) Bx >= p->proc.sBase && (ptrPo) Bx <= p->proc.sTop &&
             (ptrPo) Cx >= p->proc.sBase && (ptrPo) Cx <= p->proc.sTop);
      assert(B->H >= p->proc.heap.base && B->H <= p->proc.heap.create);

      if ((ptrPo) Bx < (ptrPo) Cx) {  /* show a choice point */
        if (Bix < NumberOf(choicePoints))
          choicePoints[Bix++] = Bx;

        if (Bx->C < lastCx) {    /* the choice point's call back is newer */
          Cx = Bx->C;
        }
        Bx = Bx->B;
      } else {        /* show a call entry */
        if (Cix < NumberOf(callPoints))
          callPoints[Cix++] = Cx;
        lastCx = Cx;
        Cx = Cx->C;
      }
    }

    /* Now we do the stack trace itself */

    if (len < 0)         /* a signal that gc is requested before an alloc */
      len = envSize(p->proc.cPC);

    outMsg(logFile, "Stack trace of %w\n", &p->proc.thread);

    while (B < (choicePo) p->proc.sTop || C < (callPo) p->proc.sTop) {

      assert((ptrPo) B >= p->proc.sBase && (ptrPo) B <= p->proc.sTop &&
             (ptrPo) C >= p->proc.sBase && (ptrPo) C <= p->proc.sTop);
      assert(op_code(*cPC) == gcmap);

      if ((ptrPo) B < (ptrPo) C) {  /* show a choice point */
        register integer ar = B->AX, i;
        register ptrPo A = (ptrPo) (B + 1);
        char *sep = "";

        if (B == T) {
          outMsg(logFile, "\ntrap point:%d @ %w[%d]",
                 bLevel++, &B->PROG, B->PC - FirstInstruction(B->PROG));
          T = T->T;
        } else {
          outMsg(logFile, "\nchoice point:%d @ %w[%d]",
                 bLevel++, &B->PROG, B->PC - FirstInstruction(B->PROG));
        }

        for (i = 0; i < ar; i++) {
          outMsg(logFile, "%s%.1w", sep, A++);
          sep = ", ";
        }

        outMsg(logFile, ", Call frame=%d\n",
               findPtr((void **) callPoints, Cix, B->C));

        if (trail != B->trail) {
          outMsg(logFile, "Trail:\n");
          while (trail != B->trail) {
            trail--;
            outMsg(logFile, "%x -> %.1w\n", trail->var, trail->var);
          }
        }

        if (B->C < lastC) {    /* the choice point's call back is newer */
          C = B->C;
          len = envSize(B->cPC);  /* do this before the next step */
        }
        B = B->B;
      } else {        /* show a call entry */
        register int i;
        register ptrPo Y = (ptrPo) C;

        outMsg(logFile, "\ncall frame:%d @ %w[%d], Parent Call frame=%d\n",
               cLevel++, &C->cPROG, C->cPC - FirstInstruction(C->cPROG),
               findPtr((void **) callPoints, Cix, C->C));

        for (i = 0; i < len; i++)
          outMsg(logFile, "Y[%d] = %.1w\n", (i + 1), --Y);

        len = envSize(C->cPC);

        cPC = C->cPC;
        lastC = C;
        C = C->C;
      }
    }

    assert((ptrPo) C == p->proc.sTop && (ptrPo) B == p->proc.sTop);
    //    assert((ptrPo)trail==p->proc.sBase);

    flushFile(logFile);
  }
}

retCode g__stackTrace(processPo P, ptrPo a) {
  stackTrace(P);
  return Ok;
}

retCode g_nop(processPo P, ptrPo a) {
  return Ok;
}

/*
   Process stack re-grow, after running out of stack heap or main stack
*/

static inline ptrPo adjust(ptrPo x, ptrPo base, ptrPo new) {
  return new + (x - base);
}

static inline ptrI adjustI(ptrI x, objPo oHeap, objPo hpLimit, objPo nHeap,
                           ptrPo oStack, ptrPo oTop, ptrPo nTop) {
  switch (ptg(x)) {
    case varTg: {
      ptrPo pp = (ptrPo) x;
      if (pp >= (ptrPo) oHeap && pp < (ptrPo) hpLimit)
        return (ptrI) (nHeap + (pp - (ptrPo) oHeap));
      else if (pp >= oStack && pp < oTop)
        return (ptrI) (nTop - (oTop - pp));
      else
        return x;
    }
    case objTg: {
      objPo pp = objV(x);
      if (pp >= oHeap && pp < hpLimit)
        return ptrP(nHeap + (pp - (objPo) oHeap), ptg(x));
      else if (pp >= (objPo) oStack && pp < (objPo) oTop)
        return ptrP((objPo) nTop - ((objPo) oTop - pp), ptg(x));
      else
        return x;
    }
    default: {                         /* This should never be adjusted dirextly */
      syserr("sig found in stack");
      return x;
    }
  }
}

typedef struct {
  objPo oHeap, hpLimit;
  objPo nHeap;
  ptrPo oStack;
  ptrPo oTop, nTop;
} AdjustRec;

static retCode adjusterHelper(ptrPo a, void *c) {
  AdjustRec *G = (AdjustRec *) c;

  *a = adjustI(*a, G->oHeap, G->hpLimit, G->nHeap, G->oStack, G->oTop, G->nTop);
  return Ok;
}

retCode extendStack(processPo p, int sfactor, int hfactor, long hmin) {
  uinteger osz = (p->proc.sTop - (ptrPo) p->proc.heap.base);
  integer nsz = osz * (sfactor + hfactor) + hmin; /* New stack size */
  ptrPo st = (ptrPo) malloc(sizeof(ptrI) * nsz);
  callPo C = p->proc.C;
  choicePo B = p->proc.B;
  choicePo T = p->proc.T;
  ptrPo nBase = st + (hfactor * osz) + hmin;    // allow at least enough for hmin
  ptrPo oBase = p->proc.sBase;
  ptrPo oTop = p->proc.sTop;
  ptrPo nTop = &st[nsz];
  objPo nHeap = (objPo) st;
  objPo oHeap = p->proc.heap.base;
  objPo oLimit = (objPo)p->proc.heap.create;
  callPo nC = (callPo) adjust((ptrPo) C, oTop, nTop);
  choicePo nB = (choicePo) adjust((ptrPo) B, oTop, nTop);
  choicePo nT = (choicePo) adjust((ptrPo) T, oTop, nTop);
  int len = envSize(p->proc.cPC);

  if (st == NULL)      /* couldnt allocate the larger stack */
    return Error;

#ifdef MEMTRACE
  verifyProc(p);

  if (traceMemory)
    outMsg(logFile, "Growing e-stack of process %#w from %ld to %ld\n",
           &p->proc.thread, osz, nsz);
#endif

  /* Copy the stack heap into the new stack */
  {
    register objPo oH = p->proc.heap.base;
    register objPo nH = nHeap;
    AdjustRec G = {
      oHeap, oLimit, nHeap, oBase, oTop, nTop
    };

    while (oH < oLimit) {
      if (isSpecialObject(oH)) {
        specialClassPo sClass = (specialClassPo) classOf(oH);
        long size = sClass->sizeFun(sClass, oH);

        memmove(nH, oH, size * sizeof(ptrI));
        sClass->scanFun(sClass, adjusterHelper, &G, nH);

        oH += size;
        nH += size;
      } else {
        ptrPo a = objectArgs(oH);
        long arity = objectArity(oH);
        long ix;

        nH->class = oH->class;
        ptrPo nA = objectArgs(nH);

        for (ix = 0; ix < arity; ix++, a++, nA++)
          *nA = adjustI(*a, oHeap, oLimit, nHeap, oBase, oTop, nTop);

        oH += objectSize(oH);
        nH += objectSize(nH);
      }

      continue;
    }

    assert(nH - nHeap == oLimit - oHeap);

    p->proc.heap.create = nH;        /* set up the new heap creation pointer */
  }

  /* Adjust the main argument registers */
  {
    register unsigned int i;

    for (i = 0; i < NumberOf(p->proc.A); i++)
      p->proc.A[i] = adjustI(p->proc.A[i], oHeap, oLimit, nHeap, oBase, oTop, nTop);
  }

  while (B < (choicePo) p->proc.sTop || C < (callPo) p->proc.sTop) {
    assert(B <= (choicePo) p->proc.sTop && C <= (callPo) p->proc.sTop);
    if ((ptrPo) B < (ptrPo) C) {
      register integer ar, i;
      register ptrPo nA = (ptrPo) (nB + 1);
      register ptrPo A = (ptrPo) (B + 1);

      assert((ptrPo) nB < (ptrPo) nC && ((ptrPo) C - (ptrPo) B) == ((ptrPo) nC - (ptrPo) nB));
      assert((ptrPo) B->B <= p->proc.sTop);
      assert((ptrPo) B->C <= p->proc.sTop);
      assert((ptrPo) B->T >= (ptrPo) B);

      nB->AX = ar = B->AX;
      nB->B = (choicePo) adjust((ptrPo) B->B, oTop, nTop);
      nB->H = (objPo)adjust((ptrPo) B->H, (ptrPo) oHeap, (ptrPo) nHeap);
      nB->cSB = (choicePo) adjust((ptrPo) B->cSB, oTop, nTop);
      nB->T = (choicePo) adjust((ptrPo) B->T, oTop, nTop);
      nB->C = (callPo) adjust((ptrPo) B->C, oTop, nTop);
      nB->cPROG = adjustI(B->cPROG, oHeap, oLimit, nHeap, oBase, oTop, nTop);
      nB->PROG = adjustI(B->PROG, oHeap, oLimit, nHeap, oBase, oTop, nTop);
      nB->trail = (trailPo) adjust((ptrPo) B->trail, oBase, nBase);
      nB->cPC = FirstInstruction(nB->cPROG) + (B->cPC - FirstInstruction(B->cPROG));
      nB->PC = FirstInstruction(nB->PROG) + (B->PC - FirstInstruction(B->PROG));
#ifdef EXECTRACE
      nB->waitFor = B->waitFor;
      nB->cWaitFor = B->cWaitFor;
#endif

      for (i = 0; i < ar; i++)
        *nA++ = adjustI(*A++, oHeap, oLimit, nHeap, oBase, oTop, nTop);

      if (B == T) {        /* This is actually a trap point */
        T = T->T;
        nT = nT->T;
      }

      if (B->C < C) {                  /* the choice point's call back is newer */
        C = B->C;
        len = envSize(B->cPC);
        nC = nB->C;
      }

      B = B->B;
      nB = nB->B;                     /* look at the next choice point */
    } else {        /* call is the most recent entry */
      register int i;
      register ptrPo nY = (ptrPo) nC;
      register ptrPo Y = (ptrPo) C;

      assert((ptrPo) nB > (ptrPo) nC && ((ptrPo) C - (ptrPo) B) == ((ptrPo) nC - (ptrPo) nB));
      assert((ptrPo) C->cSB <= p->proc.sTop);
      assert((ptrPo) C->C <= p->proc.sTop);

      nC->cSB = (choicePo) adjust((ptrPo) C->cSB, oTop, nTop);
      nC->C = (callPo) adjust((ptrPo) C->C, oTop, nTop);
      nC->cPROG = adjustI(C->cPROG, oHeap, oLimit, nHeap, oBase, oTop, nTop);
      nC->cPC = FirstInstruction(nC->cPROG) + (C->cPC - FirstInstruction(C->cPROG));
#ifdef EXECTRACE
      nC->cWaitFor = C->cWaitFor;
#endif
      for (i = 0; i < len; i++)
        *--nY = adjustI(*--Y, oHeap, oLimit, nHeap, oBase, oTop, nTop);

      len = envSize(C->cPC);
      nC = nC->C;
      C = C->C;
    }
  }

  /* Adjust the trail entries themselves */
  {
    register trailPo otr = p->proc.trail;
    register uinteger count = otr - (trailPo) oBase;
    register trailPo ntr = (trailPo) adjust((ptrPo) p->proc.trail, oBase, nBase);

    while (count-- > 0) {
      ntr--;
      otr--;

      ntr->var = (ptrPo) adjustI((ptrI) (otr->var), oHeap, oLimit, nHeap, oBase, oTop, nTop);
      ntr->val = adjustI(otr->val, oHeap, oLimit, nHeap, oBase, oTop, nTop);
    }
  }

  p->proc.B = (choicePo) adjust((ptrPo) p->proc.B, oTop, nTop);
  p->proc.SB = (choicePo) adjust((ptrPo) p->proc.SB, oTop, nTop);
  p->proc.cSB = (choicePo) adjust((ptrPo) p->proc.cSB, oTop, nTop);
  p->proc.T = (choicePo) adjust((ptrPo) p->proc.T, oTop, nTop);
  p->proc.trail = (trailPo) adjust((ptrPo) p->proc.trail, oBase, nBase);
  p->proc.C = (callPo) adjust((ptrPo) p->proc.C, oTop, nTop);
  p->proc.PC = (insPo) (p->proc.PC - FirstInstruction(p->proc.PROG));
  p->proc.cPC = (insPo) (p->proc.cPC - FirstInstruction(p->proc.cPROG));
  p->proc.PROG = adjustI(p->proc.PROG, oHeap, oLimit, nHeap, oBase, oTop, nTop);
  p->proc.cPROG = adjustI(p->proc.cPROG, oHeap, oLimit, nHeap, oBase, oTop, nTop);
  p->proc.PC = FirstInstruction(p->proc.PROG) + (long) (p->proc.PC);
  p->proc.cPC = FirstInstruction(p->proc.cPROG) + (long) (p->proc.cPC);
  p->proc.trigger = adjustI(p->proc.trigger, oHeap, oLimit, nHeap, oBase, oTop, nTop);

  /* Adjust the root pointers */
  {
    register int i;

    for (i = 0; i < p->proc.heap.topRoot; i++)
      *p->proc.heap.roots[i] = adjustI(*p->proc.heap.roots[i], oHeap, oLimit, nHeap, oBase, oTop, nTop);
  }

  free(p->proc.heap.base);    /* Dispose of the old evaluation stack */
  p->proc.sBase = nBase;
  p->proc.sTop = nTop;    /* New stack top */
  p->proc.heap.base = (objPo) st;
  p->proc.heap.end = (objPo) nBase;

#ifdef PROCTRACE
  verifyProc(p);

#ifdef MEMTRACE
  extendCount++;
#endif
#endif

  return Ok;
}

static void destroyThread(void *arg) {
  destroyObject(O_OBJECT(arg));
}

// This is called when a new Go thread is started
// Most of the fields are already set up before the fork

void *forkThread(void *arg) {
  processPo P = (processPo) arg;

  pthread_setspecific(((ProcessClassRec *) P->object.class)->proc.processKey, P);
  P->proc.state = runnable;
  pthread_cleanup_push(destroyThread, P);

    runGo(P);        // start the execution of L&O code

  pthread_cleanup_pop(True);
  return NULL;
}

retCode g__fork(processPo P, ptrPo a) {
  ptrI as = deRefI(&a[1]);        /* the code to use in this sub-thread */
  ptrI th = deRefI(&a[2]);

  if (isvar(as))
    return liberror(P, "_fork", eINSUFARG);
  else if (!isvar(th))
    return liberror(P, "_fork", eVARNEEDD);
  else {
    ptrI thread = newThread();
    heapPo H = &P->proc.heap;
    rootPo root = gcAddRoot(H, &thread);

    processPo new = O_PROCESS(newObject(processClass, thread));

    objPo p = objV(as);

    ptrI prog;

    if (isGoObject(p))
      prog = objectCode(loObjV(as));
    else if (isObjct(p))
      prog = ProgramOf(programOfClass(objV(p->class)));
    else {
      assert(isSpecialObject(p));
      prog = ProgramOf(specialProgram(p));
    }

    new->proc.PROG = prog;

    new->proc.A[1] = kstart;    /* start_thread() */
    new->proc.A[2] = as;
    new->proc.A[3] = as;    /* set up a call to the object */

    new->proc.PC = FirstInstruction(prog);

    pthread_attr_t detach;

    if (pthread_attr_init(&detach) != 0)
      syserr("cannot initiate attributes object");
    else if (pthread_attr_setdetachstate(&detach, PTHREAD_CREATE_JOINABLE) != 0)
      syserr("cannot set create-detached");
    if (pthread_create(&new->proc.threadID, &detach, forkThread, new) != 0)
      syserr("cannot fork a thread");

    return equal(P,&thread,&th);
  }
}

retCode g_kill(processPo P, ptrPo a) {
  ptrI x = deRefI(&a[1]);

  if (isvar(x))
    return liberror(P, "kill", eINSUFARG);
  else {
    processPo tgt = getProcessVal(x);

    if (tgt != NULL && tgt != P) {
      ps_kill(tgt);
      return Ok;
    } else
      return liberror(P, "kill", eINVAL);
  }
}

retCode g_thread_state(processPo P, ptrPo a) {
  ptrI x = deRefI(&a[1]);

  if (isvar(x))
    return liberror(P, "thread_state", eINSUFARG);
  else {
    processPo tgt = getProcessVal(x);
    ptrI st;

    switchProcessState(P, in_exclusion);

    if (tgt == NULL)
      st = newEnumSym(state_names[dead]);
    else
      st = newEnumSym(state_names[tgt->proc.state]);
    setProcessRunnable(P);

    return equal(P, &a[2], &st);
  }
}

retCode g_waitfor(processPo P, ptrPo a) {
  ptrI x = deRefI(&a[1]);

  if (isvar(x))
    return liberror(P, "waitfor", eINSUFARG);
  else if (!IsGoObject(x))
    return liberror(P, "waitfor", eINVAL);
  else {
    processPo tgt = getProcessVal(x);

    if (tgt == NULL)
      return Ok;
    else if (tgt != P) {
      pthread_t thread = tgt->proc.threadID;
      void *result;      /* This is ignored */

      switchProcessState(P, wait_term);
      if (pthread_join(thread, &result) == 0) {
        setProcessRunnable(P);
        return Ok;
      } else {
        setProcessRunnable(P);
        switch (errno) {
          case EINVAL:return liberror(P, "waitfor", eINVAL);
          case ESRCH:return liberror(P, "waitfor", eNOTFND);
          case EDEADLK:return liberror(P, "waitfor", eDEAD);
          default:return Ok;
        }
      }
    } else
      return liberror(P, "waitfor", eDEAD);
  }
}

retCode g__assoc(processPo P, ptrPo a) {
  ptrI x = deRefI(&a[1]);
  ptrI p = deRefI(&a[2]);

  if (!isvar(x))
    return liberror(P, "_assoc", eVARNEEDD);
  else if (isvar(p))
    return liberror(P, "_assoc", eINSUFARG);
  else {
    return Ok;
  }
}

retCode g__thread(processPo P, ptrPo a) {
  return equal(P, &P->proc.thread, &a[1]);
}
