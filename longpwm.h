#ifndef LONGPWM_H
#define LONGPWM_H

void longPwmInit(void);
int longPwmSetDuty(uint16_t period);
uint16_t longPwmGetDuty(void);

#endif