#include <mchck.h>

#include "millis.h"

static volatile millis_t milliseconds;

void millis_init()
{
    SIM.scgc5.lptimer = 1; //Enable clock to the timer.

    LPTMR0.psr.raw = ((struct LPTMR_PSR){
                        .prescale = 0, //25 MHz
                        .pbyp = 1, //Bypass the filter
                        .pcs = LPTMR_PCS_LPO //Filter clock 0
                                }).raw;

    LPTMR0.csr.raw = ((struct LPTMR_CSR){
                    .tcf = 1,
                    .tfc = 0, //reset on match
                    .tms = LPTMR_TMS_TIME, //time counter mode
                            }).raw;
    LPTMR0.cmr = 1;  //generate an interrupt every 1 ms

    LPTMR0.csr.raw |= ((struct LPTMR_CSR){
                    .ten = 1,
                    .tie = 1,
                    .tcf = 1
                            }).raw;

    int_enable(IRQ_LPT);

}

millis_t millis_get()
{
    millis_t ms;
    crit_enter();
    ms = milliseconds;
    crit_exit();

    return ms;
}

void millis_resume()
{
    LPTMR0.csr.raw |= ((struct LPTMR_CSR){
                    .ten = 1,
                    .tie = 1,
                    .tcf = 1
                    }).raw;
}

void millis_pause()
{
    LPTMR0.csr.raw |= ((struct LPTMR_CSR){
                    .ten = 0,
                    .tie = 0,
                    .tcf = 1
                    }).raw;
}

void millis_reset()
{
    crit_enter();
    milliseconds = 0;
    crit_exit();
}

void millis_add(millis_t ms)
{
    crit_enter();
    milliseconds += ms;
    crit_exit();

}

void millis_subtract(millis_t ms)
{
    crit_enter();
    milliseconds -= ms;
    crit_exit();
    
}

void LPT_Handler(void)
{
    crit_enter();
    LPTMR0.csr.raw |= ((struct LPTMR_CSR){
                    .ten = 1,
                    .tie = 1,
                    .tcf = 1
                            }).raw;
    milliseconds+=2;  //Unfortunately, the LPTimer interrupt runs at 500 KHz.
    crit_exit();
}

