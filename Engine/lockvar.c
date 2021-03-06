/* 
  Lock synchronization functions
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

#include <errno.h>

#include "lo.h"
#include "clock.h"

#define NANO (1000000000)

ptrI lockStrct;

static long lckSizeFun(specialClassPo class, objPo o);
static comparison lckCompFun(specialClassPo class, objPo o1, objPo o2);
static retCode lckOutFun(specialClassPo class, ioPo out, objPo o);
static retCode lckScanFun(specialClassPo class, specialHelperFun helper, void *c, objPo o);
static objPo lckCopyFun(specialClassPo class, objPo dst, objPo src);
static uinteger lckHashFun(specialClassPo class, objPo o);

void initLockStrct(void) {
  lockStrct = newSpecialClass("lo.thread*lock", lckSizeFun, lckCompFun,
                              lckOutFun, lckCopyFun, lckScanFun, lckHashFun);
}

static long lckSizeFun(specialClassPo class, objPo o) {
  assert(o->class == lockStrct);

  return CellCount(sizeof(LockRec));
}

static comparison lckCompFun(specialClassPo class, objPo o1, objPo o2) {
  if (o1 == o2)
    return same;
  else
    return incomparible;
}

static retCode lckOutFun(specialClassPo class, ioPo out, objPo o) {
  lockPo s = (lockPo) o;

  return outMsg(out, "lock[0x%x]", s);
}

static retCode lckScanFun(specialClassPo class, specialHelperFun helper, void *c, objPo o) {
  return Ok;
}

static objPo lckCopyFun(specialClassPo class, objPo dst, objPo src) {
  long size = lckSizeFun(class, src);
  memmove((void *) dst, (void *) src, size * sizeof(ptrI));

  return (objPo) (((ptrPo) dst) + size);
}

static uinteger lckHashFun(specialClassPo class, objPo o) {
  assert(o->class == lockStrct);

  return (uinteger) o;
}


// We are having to do this somewhat clumsily because not all systems 
// support pthread_mutex_timedlock (posix 1.d) including mac os x :(
// So we use a condition variable to signal that a lock is available

retCode acquireLock(lockPo l, double tmOut) {
#ifdef LOCKTRACE
  if (traceLock)
    outMsg(logFile, RED_ESC_ON "acquire lock (%f) [%d] on 0x%x"RED_ESC_OFF"\n%_", tmOut, l->count, l);
#endif

  if (pthread_mutex_lock(&l->mutex))  /* This will be transitory */
    syserr("problem in acquiring lock mutex");

  again:
  if (l->count == 0) {
    l->owner = pthread_self();
    l->count = 1;
    if (pthread_mutex_unlock(&l->mutex))
      syserr("problem in releasing lock mutex");
    return Ok;
  } else if (pthread_equal(l->owner, pthread_self())) {
    l->count++;

    if (pthread_mutex_unlock(&l->mutex))
      syserr("problem in releasing lock mutex");
    return Ok;
  } else if (tmOut == 0.0) {      /* treat as a no-timeout lock */
#ifdef LOCKTRACE
    if (traceLock)
      outMsg(logFile, RED_ESC_ON "getLock cond_wait on 0x%x"RED_ESC_OFF"\n%_", l);
#endif

    if (pthread_cond_wait(&l->cond, &l->mutex))
      syserr("problem in waiting");
    goto again;        /* try to lock again */
  } else {
    struct timespec tm;
    double seconds;
    double fraction = modf(tmOut, &seconds);

    tm.tv_sec = (long) seconds;
    tm.tv_nsec = (long) (fraction * NANO);  // Convert microseconds to nanoseconds
    switch (pthread_cond_timedwait(&l->cond, &l->mutex, &tm)) {
      case 0:goto again;
      case ETIMEDOUT:pthread_mutex_unlock(&l->mutex);
        return Fail;
      default:pthread_mutex_unlock(&l->mutex);
        return Error;
    }
  }
}

retCode releaseLock(lockPo l) {
  if (pthread_mutex_lock(&l->mutex))  /* This will be transitory */
    syserr("problem in acquiring lock mutex");
  retCode ret = Ok;

#ifdef LOCKTRACE
  if (traceLock)
    outMsg(logFile, RED_ESC_ON "releaseLock [%d] on 0x%x"RED_ESC_OFF"\n%_", l->count, l);
#endif

  if (pthread_equal(l->owner, pthread_self())) {
    if (l->count > 0) {
      l->count--;

      if (l->count <= 0) {
        l->count = 0;
        l->owner = NULL;
        pthread_cond_broadcast(&l->cond);
      }
    }
  } else if (l->count != 0) {
#ifdef LOCKTRACE
    if (traceLock)
      outMsg(logFile, RED_ESC_ON "tried to release non-owned lock 0x%x"RED_ESC_OFF"\n%_", l);
#endif

    ret = Fail;
  }

  if (pthread_mutex_unlock(&l->mutex))
    syserr("problem in releasing lock");
  return ret;
}

// waitLock releases a lock and puts the client on the lock's wait queue
retCode waitLock(lockPo l, double tmOut) {
#ifdef LOCKTRACE
  if (traceLock) {
    outMsg(logFile, RED_ESC_ON "waitLock on 0x%x"RED_ESC_OFF"\n%_", l);
  }
#endif
  retCode ret;

  if (pthread_mutex_lock(&l->mutex))  /* This will be transitory */
    syserr("problem in acquiring lock mutex");

  if ((pthread_equal(l->owner, pthread_self()) && l->count == 1) ||
      l->count == 0) {
    l->count = 0;
    l->owner = NULL;    /* the equivalent of unlocking */

    if (tmOut == 0.0) {      /* treat as a no-timeout wait */
#ifdef LOCKTRACE
      if (traceLock) {
        outMsg(logFile, RED_ESC_ON "waiting on 0x%x"RED_ESC_OFF"\n%_", l);
      }
#endif

      if (pthread_cond_broadcast(&l->cond))
        syserr("problem in broadcast");

      if (pthread_cond_wait(&l->cond, &l->mutex))
        syserr("problem in condwait");
      ret = Ok;
    } else {
      struct timespec tm;
      double seconds;
      double fraction = modf(tmOut, &seconds);

#ifdef LOCKTRACE
      if (traceLock)
        outMsg(logFile, RED_ESC_ON "waiting for %f seconds"RED_ESC_OFF"\n%_", seconds);
#endif
      tm.tv_sec = (long) seconds;
      tm.tv_nsec = (long) (fraction * NANO);  /* convert fractions to nanoseconds */

      int unixRet = 0;
      switch (unixRet = pthread_cond_timedwait(&l->cond, &l->mutex, &tm)) {
        case 0:ret = Ok;
          break;            /* somewhere else we will try to relock */
        case ETIMEDOUT:ret = Fail;
          break;
        default:
#ifdef LOCKTRACE
          if (traceLock)
            outMsg(logFile, RED_ESC_ON "error on wait %d"RED_ESC_OFF"\n%_", unixRet);
#endif

          ret = Error;
          break;
      }
    }
  } else {
#ifdef LOCKTRACE
    if (traceLock)
      outMsg(logFile, RED_ESC_ON "0x%x locked more than once"RED_ESC_OFF"\n%_", l);
#endif

    ret = Error; /* not allowed to wait on a lock that is locked more than once */
  }

  pthread_mutex_unlock(&l->mutex);
  return ret;
}

static lockPo getLock(ptrI S) {
  dynPo s = loObjV(S);
  if (s->lock == NULL)
    s->lock = newLock();
  return s->lock;
}

lockPo newLock(void) {
  lockPo l = (lockPo) permAllocate(LockCellCount);

  l->class = lockStrct;
  l->owner = NULL;
  l->count = 0;

  pthread_mutexattr_t attr;

  pthread_mutexattr_init(&attr);
#ifdef TRACELOCK
  pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_ERRORCHECK);
#else
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
#endif

  mtxInit:
  switch (pthread_mutex_init(&l->mutex, &attr)) {
    case 0:break;
    case EINVAL:syserr("cannot init");
    case ENOMEM:syserr("no memory");
    case EAGAIN:goto mtxInit;
    default:syserr("!!mutex init");
  }

  pthread_mutexattr_destroy(&attr);

  again:
  switch (pthread_cond_init(&l->cond, NULL)) {
    case 0:return l;
    case EAGAIN:goto again;
    default:syserr("cannot init lock");
      return l;
  }
}

retCode g__newLock(processPo P, ptrPo a) {
  ptrI A1 = deRefI(&a[1]);
  if (isvar(A1))
    return liberror(P, "_newLock", eVARNEEDD);
  else {
    ptrI l = objP(newLock());
    return equal(P, &a[1], &l);
  }
}

retCode g__acquireLock(processPo P, ptrPo a) {
  ptrI A1 = deRefI(&a[1]);
  ptrI A2 = deRefI(&a[2]);              // Second argument is a timeout value
  objPo o2 = objV(A2);

  if (isvar(A1))
    return liberror(P, "__acquireLock", eINSUFARG);
  else if (!IsLock(A1))
    return liberror(P, "_acquireLock", eINVAL);
  else if (isvar(A2))
    return liberror(P, "__acquireLock", eINSUFARG);
  else if (!IsFloat(o2))
    return liberror(P, "__acquireLock", eNUMNEEDD);
  else {
    double tmOut = FloatVal(o2);

    if (isvar(A1))
      return liberror(P, "__acquireLock", eINSUFARG);

    rootPo root = gcAddRoot(&P->proc.heap, &A1);
    gcAddRoot(&P->proc.heap, &A2);

    lockPo lk = lockV(A1);    // Pick up the lock

    switchProcessState(P, wait_lock);

#ifdef LOCKTRACE
    if (traceLock)
      outMsg(logFile, RED_ESC_ON "%w: getting lock 0x%x on %w"RED_ESC_OFF"\n%_", &P->proc.thread, lk, &a[1]);
#endif
    retCode ret = acquireLock(lk, tmOut);
    setProcessRunnable(P);

    gcRemoveRoot(&P->proc.heap, root);

    switch (ret) {
      case Ok:
#ifdef LOCKTRACE
        if (traceLock)
          outMsg(logFile, RED_ESC_ON "%w: Locked %w"RED_ESC_OFF"\n%_", &P->proc.thread, &a[1]);
#endif
        return Ok;
      case Fail:    // Could not acquire the lock and timeout is not established
#ifdef LOCKTRACE
        if (traceLock)
          outMsg(logFile, RED_ESC_ON "%w: Lock on %w failed"RED_ESC_OFF"\n%_", &P->proc.thread, &a[1]);
#endif
        return Fail;
      case Error:
      default:
#ifdef LOCKTRACE
        if (traceLock)
          outMsg(logFile, RED_ESC_ON "%w: problem with waitlock on %w"RED_ESC_OFF"\n%_", &P->proc.thread, &a[1]);
#endif
        return liberror(P, "__acquireLock", eDEAD);
    }
  }
}

retCode g__waitLock(processPo P, ptrPo a) {
  ptrI A1 = deRefI(&a[1]);
  ptrI A2 = deRefI(&a[2]);              // Second argument is a timeout value
  objPo o2 = objV(A2);

  if (isvar(A1))
    return liberror(P, "_waitLock", eINSUFARG);
  else if (!IsLock(A1))
    return liberror(P, "_waitLock", eINVAL);
  else if (isvar(A2))
    return liberror(P, "__waitLock", eINSUFARG);
  else if (!IsFloat(o2))
    return liberror(P, "__waitLock", eNUMNEEDD);
  else {
    double tmOut = FloatVal(o2);

    rootPo root = gcAddRoot(&P->proc.heap, &A1);
    gcAddRoot(&P->proc.heap, &A2);

    lockPo lk = lockV(A1);    // Pick up the lock

    switchProcessState(P, wait_lock);

#ifdef LOCKTRACE
    if (traceLock)
      outMsg(logFile, RED_ESC_ON "%w: waiting on lock 0x%x on %w"RED_ESC_OFF"\n%_", &P->proc.thread, lk, &a[1]);
#endif

    retCode ret = waitLock(lk, tmOut);
    setProcessRunnable(P);

    gcRemoveRoot(&P->proc.heap, root);

    switch (ret) {
      case Ok:
#ifdef LOCKTRACE
        if (traceLock)
          outMsg(logFile, RED_ESC_ON "%w: come back on %w "RED_ESC_OFF"\n%_", &P->proc.thread, &a[1]);
#endif
        return Ok;
      case Fail:
#ifdef LOCKTRACE
        if (traceLock)
          outMsg(logFile, RED_ESC_ON "%w: Timed out, waiting on %w "RED_ESC_OFF"\n%_", &P->proc.thread, &a[1]);
#endif
        return Fail;
      case Error:
#ifdef LOCKTRACE
        if (traceLock)
          outMsg(logFile, RED_ESC_ON "%w: problem with waitlock on %w"RED_ESC_OFF"\n%_", &P->proc.thread, &a[1]);
#endif
        return liberror(P, "_waitLock", eDEAD);
      default:return liberror(P, "_waitLock", eINVAL);
    }
  }
}

retCode g__releaseLock(processPo P, ptrPo a) {
  ptrI A1 = deRefI(&a[1]);

  if (isvar(A1))
    return liberror(P, "_releaseLock", eINSUFARG);
  else if (!IsLock(A1))
    return liberror(P, "_releaseLock", eINVAL);

  lockPo lk = lockV(A1);    // Pick up the lock

#ifdef LOCKTRACE
  if (traceLock)
    outMsg(logFile, RED_ESC_ON "%w: releasing lock 0x%x on %w"RED_ESC_OFF"\n%_", &P->proc.thread, lk, &a[1]);
#endif

  retCode ret = releaseLock(lk);

  switch (ret) {
    case Ok:
#ifdef LOCKTRACE
      if (traceLock)
        outMsg(logFile, RED_ESC_ON "%w: Lock released on %w"RED_ESC_OFF"\n%_", &P->proc.thread, &a[1]);
#endif

      return Ok;
    case Fail:
#ifdef LOCKTRACE
      if (traceLock)
        outMsg(logFile, RED_ESC_ON "%w: Could not release %w "RED_ESC_OFF"\n%_", &P->proc.thread, &a[1]);
#endif
      return Fail;
    case Error:
#ifdef LOCKTRACE
      if (traceLock)
        outMsg(logFile, RED_ESC_ON "%w: did not own %w "RED_ESC_OFF"\n%_", &P->proc.thread, &a[1]);
#endif
      return liberror(P, "__releaseLock", eFAIL);
    default:return liberror(P, "__releaseLock", eINVAL);
  }
}

