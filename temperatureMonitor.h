#ifndef TEMPERATUREMONITOR_H
#define TEMPERATUREMONITOR_H

#include "armfix.h"

void initializeTemperature(void);

fix_s15_16_t getTemperature(void);

#endif