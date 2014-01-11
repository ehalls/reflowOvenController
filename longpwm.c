#include <mchck.h>

#include "longpwm.h"

#define INTERRUPT_SUBINTERVAL (50000000UL * (2. / 65535.))

static volatile uint16_t pwmCounter = 0;
static volatile uint16_t pwmDuty = 0;

static void callback(enum pit_id id)
{
    crit_enter();
    pwmCounter++;
    if(pwmCounter == pwmDuty)
    {
        gpio_write(GPIO_PTA2, GPIO_LOW);
    } else if(pwmCounter == 0)
    {
        gpio_write(GPIO_PTA2, GPIO_HIGH);
    }
    crit_exit();

}

void longPwmInit(void)
{
    gpio_dir(GPIO_PTA2, GPIO_OUTPUT);
    pin_mode(GPIO_PTA2, PIN_MODE_DRIVE_HIGH);

    pit_init();
    pit_start(PIT_0, INTERRUPT_SUBINTERVAL, callback);
}

int longPwmSetDuty(uint16_t period)
{
    crit_enter();
    pwmDuty = period;
    crit_exit();
}

uint16_t longPwmGetDuty(void)
{
    return pwmDuty;
}
