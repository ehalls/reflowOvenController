#ifndef MILLIS_H
#define MILLIS_H

typedef unsigned long millis_t;  //Overflows after about 50 days.

#ifdef __cplusplus
extern "C" {
#endif

void millis_init(void);

millis_t millis_get(void);

void millis_resume(void);

//Power Saving.
void millis_pause(void);

void millis_reset(void);

void millis_add(millis_t ms);

void millis_subtract(millis_t ms);

#ifdef __cplusplus
}
#endif

#endif