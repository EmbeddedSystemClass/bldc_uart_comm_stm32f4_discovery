/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    osal.c
 * @brief   OSAL module code.
 *
 * @addtogroup OSAL
 * @{
 */

#include "osal.h"

/*===========================================================================*/
/* Module local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Module exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Module local types.                                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Module local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Module exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Dequeues and wakes up one thread from the queue, if any.
 *
 * @param[in] tqp       pointer to the threads queue object
 * @param[in] msg       the message code
 *
 * @iclass
 */
void osalThreadDequeueNextI(threads_queue_t *tqp, msg_t msg) {
  semaphore_t *sp = &tqp->sem;

  if (chSemGetCounterI(&tqp->sem) < (cnt_t)0) {
    thread_reference_t tr = nil.threads;
    while (true) {
      /* Is this thread waiting on this semaphore?*/
      if (tr->u1.semp == sp) {
        sp->cnt++;

        chDbgAssert(NIL_THD_IS_WTSEM(tr), "not waiting");

        (void) chSchReadyI(tr, msg);
        return;
      }
      tr++;

      chDbgAssert(tr < &nil.threads[NIL_CFG_NUM_THREADS],
                  "pointer out of range");
    }
  }
}

/**
 * @brief   Dequeues and wakes up all threads from the queue.
 *
 * @param[in] tqp       pointer to the threads queue object
 * @param[in] msg       the message code
 *
 * @iclass
 */
void osalThreadDequeueAllI(threads_queue_t *tqp, msg_t msg) {
  semaphore_t *sp = &tqp->sem;
  thread_reference_t tr;
  cnt_t cnt;

  cnt = sp->cnt;
  sp->cnt = (cnt_t)0;
  tr = nil.threads;
  while (cnt < (cnt_t)0) {

    chDbgAssert(tr < &nil.threads[NIL_CFG_NUM_THREADS],
                "pointer out of range");

    /* Is this thread waiting on this semaphore?*/
    if (tr->u1.semp == sp) {

      chDbgAssert(NIL_THD_IS_WTSEM(tr), "not waiting");

      cnt++;
      (void) chSchReadyI(tr, msg);
    }
    tr++;
  }
}

/** @} */
