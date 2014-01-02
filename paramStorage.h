#ifndef PARAMSTORAGE_H
#define PARAMSTORAGE_H

#include "armfix.h"

typedef struct {
    fix_s15_16_t soak_mintemp;
    fix_s15_16_t soak_maxtemp;
    fix_s15_16_t soak_rate;
    fix_s15_16_t reflow_maxtemp;
    fix_s15_16_t reflow_p;
    fix_s15_16_t reflow_i;
    fix_s15_16_t reflow_d;
    fix_s15_16_t soak_p;
    fix_s15_16_t soak_i;
    fix_s15_16_t soak_d;
} config;

extern config params;

void paramLoad(void);
void paramWrite(void);

#endif