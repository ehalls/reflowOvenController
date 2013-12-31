#include <mchck.h>

#include <stdint.h>

#include "taskqueue.h"

static uint8_t queueStart = 0;
static uint8_t queueEnd = 0;
static uint8_t itemsInQueue = 0;

static queueItem scheduleQueue[QUEUE_DEPTH];

static int scheduleQueueGetTop(queueItem *returnItem);
static int internalScheduleFunction(queueItem pItem);

int scheduleFunction(queuedFunction pFunction, uint32_t pId, millis_t pInitialRun, millis_t pRecur)
{
    int rv = 0;

    queueItem newItem;
    newItem.fPtr = pFunction;
    newItem.pId = pId;
    newItem.recur = pRecur;
    newItem.next = pInitialRun;

    rv = internalScheduleFunction(newItem);

    return rv;
}

int scheduleRemoveFunction(uint32_t pId)
{
    queueItem target;
    int rv = 0;
    for (int i = 0; i < itemsInQueue; ++i)
    {
        if(scheduleQueueGetTop(&target) == 0)
        {
            if(target.pId != pId)
            {
                internalScheduleFunction(target);
            }
        } else {
            rv = -1;
            break;
        }
    }

    return rv;
}

int scheduleChangeFunction(uint32_t pId, millis_t pNewNext, millis_t pNewRecur)
{
    queueItem target;
    int rv = 0;
    for (int i = 0; i < itemsInQueue; ++i)
    {
        if(scheduleQueueGetTop(&target) == 0)
        {
            if(target.pId == pId)
            {
                target.next = pNewNext;
                target.recur = pNewRecur;
            }
            internalScheduleFunction(target);
        } else {
            rv = -1;
            break;
        }
    }

    return rv;
}

int scheduleRun(millis_t pNow)
{
    queueItem target;
    int rv = 0;
    for (int i = 0; i < itemsInQueue; ++i)
    {
        if(scheduleQueueGetTop(&target)==0)
        {
            if(target.next <= pNow)
            {
                int tRv;
                tRv = (target.fPtr)(pNow);
                if(tRv == 0)
                {
                    rv++;
                }
                if(target.recur != 0)
                {
                    target.next = pNow + target.recur;
                    internalScheduleFunction(target);
                }
            } else {
                internalScheduleFunction(target);
            }
        } else {
            rv = -1;
            break;
        }
    }

    return rv;
}

static int scheduleQueueGetTop(queueItem *returnItem)
{
    int rv = 0;
    //Remove the top item, stuff it into returnItem
    if (queueEnd != queueStart) {
            queueItem tempQueueItem = scheduleQueue[queueStart];
            //This Algorithm also from Wikipedia.
            queueStart = (queueStart + 1) % QUEUE_DEPTH;
            *returnItem = tempQueueItem;
            itemsInQueue--;
    } else {
    //if the buffer is empty, return an error code
        rv = -1;
    }

    return rv;   
}

static int internalScheduleFunction(queueItem pItem)
{
    int rv = 0;
    if ((queueEnd + 1) % QUEUE_DEPTH != queueStart) {
        scheduleQueue[queueEnd] = pItem;
        queueEnd = (queueEnd + 1) % QUEUE_DEPTH;
        itemsInQueue++;
    } else {
        rv = -1;
    }
    return rv;
}