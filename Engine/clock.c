/*
  Clock and interval timer management for the L&O system
  Copyright (c) 2016, 2017. Francis G. McCabe

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
  except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the
  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied. See the License for the specific language governing
  permissions and limitations under the License.
 */

#include <signal.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include "lo.h"
#include "clock.h"

long timezone_offset;    // offset in seconds from GMT

struct timeval initial_time;    // Time when the engine started

void init_time(void) {
  time_t tloc;
  struct tm *tmptr;

  gettimeofday(&initial_time, NULL);

  tloc = initial_time.tv_sec;
  tmptr = localtime(&tloc);
  tmptr->tm_hour = 0;
  tmptr->tm_min = 0;
  tmptr->tm_sec = 0;

  timezone_offset = mktime(tmptr) - (tloc - tloc % SECSINDAY);
}

/*
 * reset the interval timer for the new period
 */

retCode g__delay(processPo P, ptrPo a) {
  ptrI x = deRefI(&a[1]);

  if (isvar(x))
    return liberror(P, "delay", eINSUFARG);
  else {
    objPo A1 = objV(x);

    if (!IsFloat(A1))
      return liberror(P, "delay", eNUMNEEDD);
    else {
      struct timespec tm;
      double seconds;
      double fraction = modf(FloatVal(A1), &seconds);

#define NANO (1000000000)

      tm.tv_sec = (long) seconds;
      tm.tv_nsec = (long) (fraction * NANO);  /* Convert microseconds to nanoseconds */
      switchProcessState(P, wait_timer);
      if (nanosleep(&tm, NULL) != 0) {
        setProcessRunnable(P);
        switch (errno) {
          case EINTR:
            return liberror(P, "delay", eINTRUPT);
          case EINVAL:
          case ENOSYS:
          default:
            return liberror(P, "delay", eINVAL);
        }
      } else {
        setProcessRunnable(P);
        return Ok;
      }
    }
  }
}

retCode g__sleep(processPo P, ptrPo a) {
  ptrI x = deRefI(&a[1]);

  if (isvar(x))
    return liberror(P, "sleep", eINSUFARG);
  else {
    objPo A1 = objV(x);

    if (!IsFloat(A1))
      return liberror(P, "sleep", eNUMNEEDD);
    else {
      double f = FloatVal(A1);
      struct timeval now;
      double seconds;
      double fraction = modf(f, &seconds);

      gettimeofday(&now, NULL);

      if (seconds < now.tv_sec ||
          (seconds == now.tv_sec && (fraction * 1000000) < now.tv_usec))
        return Ok;
      else {
        struct timespec tm;

        tm.tv_sec = (long) seconds;
        tm.tv_nsec = (long) (fraction * NANO);  /* Convert microseconds to nanoseconds */

        tm.tv_sec = (long) seconds - now.tv_sec;
        tm.tv_nsec = (long) (fraction * NANO) - now.tv_usec * 1000; /* Convert microseconds to nanoseconds */
        if (tm.tv_nsec > NANO) {
          tm.tv_nsec -= NANO;
          tm.tv_sec++;
        } else if (tm.tv_nsec < 0) {
          tm.tv_nsec += NANO;
          tm.tv_sec--;
        }

        switchProcessState(P, wait_timer);
        if (nanosleep(&tm, NULL) != 0) {
          setProcessRunnable(P);
          switch (errno) {
            case EINTR:
              return liberror(P, "sleep", eINTRUPT);
            case EINVAL:
            case ENOSYS:
            default:
              return liberror(P, "sleep", eINVAL);
          }
        } else {
          setProcessRunnable(P);
          return Ok;
        }
      }
    }
  }
}

/* Return the current time */
retCode g__now(processPo P, ptrPo a) {
  ptrI x = deRefI(&a[1]);

  if (!isvar(x))
    return liberror(P, "now", eVARNEEDD);
  else {
    ptrI now = allocateFloat(&P->proc.heap, get_time());

    return equal(P, &a[1], &now);
  }
}

/* Return the time at midnight */
retCode g__today(processPo P, ptrPo a) {
  ptrI x = deRefI(&a[1]);

  if (!isvar(x))
    return liberror(P, "now", eVARNEEDD);
  else {
    ptrI now = allocateFloat(&P->proc.heap, get_date());

    return equal(P, &a[1], &now);
  }
}

/*
 *  returns the current ticks
 */

double get_ticks(void) {
  return ((double) clock()) / CLOCKS_PER_SEC;
}

retCode g__ticks(processPo p, ptrPo a) {
  ptrI T = allocateFloat(&p->proc.heap, get_ticks());

  return equal(p, &T, &a[1]);
}

/*
 *  returns the current time
 */
double get_time(void) {
  struct timeval t;

  gettimeofday(&t, NULL);

  return t.tv_sec + t.tv_usec / 1.0e6;
}

/*
 *  returns the time at midnight this morning
 */
double get_date(void) {
  struct timeval t;

  gettimeofday(&t, NULL);

  t.tv_sec -= t.tv_sec % SECSINDAY;
  return (t.tv_sec + timezone_offset);
}
