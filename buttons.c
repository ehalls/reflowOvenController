#include <mchck.h>

#include "millis.h"
#include "lib/mchck/taskqueue.h"

#include "buttons.h"

#define longWaitTime 1000

static buttonCallback shortCallbacks[3] = { 0 };
static buttonCallback longCallbacks[3] = { 0 };

static int waiting = 0;
static millis_t lastButton = 0;

void initButtons(void)
{

    //Set as inputs. I beleive this digital filter is the highspeed lowpass.
    //This filter will do nothing to improve bouncing
    gpio_dir(GPIO_PTD2, GPIO_INPUT);
    pin_mode(PIN_PTD2, PIN_MODE_FILTER_ON);
    gpio_dir(GPIO_PTD1, GPIO_INPUT);
    pin_mode(PIN_PTD1, PIN_MODE_FILTER_ON);
    gpio_dir(GPIO_PTD0, GPIO_INPUT);
    pin_mode(PIN_PTD0, PIN_MODE_FILTER_ON);

    //Special digital filter for de bouncing
    PORTD_DFWR |= 0b11111; //Set the length at 31 clock pulses
    PORTD_DFCR |= 0b1;     //Clock source is the 1KHz clock, should filter out bouncing on the order of 31 ms
    PORTD_DFER |= 0b111;   //Enable the filter on PD0, 1 and 2

    PORTD_PCR(2) |= PORT_PCR_IRQC(0b1011);
    PORTD_PCR(1) |= PORT_PCR_IRQC(0b1011);
    PORTD_PCR(0) |= PORT_PCR_IRQC(0b1011);

    int_enable(IRQ_PORTD);

}

int buttonSetCallback(int buttonNumber, buttonCallback callback, bool longPress)
{
    if (buttonNumber > 2)
    {
        return -1;
    }
    if(longPress)
    {
        longCallbacks[buttonNumber] = callback;
    } else {
        shortCallbacks[buttonNumber] = callback;
    }

    return 0;

}

void PORTD_Handler(void)
{
    crit_enter();
    PORTD_ISFR = 0xFFFFFFFF;
    crit_exit();
    if(waiting != 0)
    {
        uint32_t bitmask = GPIOD_PDIR & (1<< (waiting - 1));
        if(bitmask != 0)
        {
            int oldWaiting = waiting;
            waiting = 0;
            if((millis_get() - lastButton) > longWaitTime)
                longCallbacks[oldWaiting - 1]();
            else
                shortCallbacks[oldWaiting - 1]();
        }

    } else {
        if(gpio_read(GPIO_PTD0) == GPIO_LOW)
            waiting = 1;
        else if(gpio_read(GPIO_PTD1) == GPIO_LOW)
            waiting = 2;
        else if(gpio_read(GPIO_PTD2) == GPIO_LOW)
            waiting = 3;

        if(waiting != 0)
        {
            lastButton = millis_get();
        }
    }


}