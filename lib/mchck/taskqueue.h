#ifndef QUEUE_H
#define QUEUE_H

#include "../../millis.h"
#include "../../scheduleIds.h"

// Settings
#define QUEUE_DEPTH 10

typedef int (*queuedFunction)(millis_t);

typedef struct _queue_item
{
    queuedFunction fPtr;
    millis_t recur;
    millis_t next;
    uint32_t pId;
} queueItem;

//Functions
int scheduleFunction(queuedFunction, uint32_t, millis_t, millis_t);
int scheduleRemoveFunction(uint32_t);
int scheduleChangeFunction(uint32_t, millis_t, millis_t);

int scheduleRun(millis_t);

#endif