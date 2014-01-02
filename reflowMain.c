#include <mchck.h>

#include "millis.h"
#include "lib/mchck/taskqueue.h"

#include "usb-serial-loopback.desc.h"

#include "buttons.h"
#include "temperatureMonitor.h"

static struct cdc_ctx cdc;

static void
new_data(uint8_t *data, size_t len)
{
        cdc_write(data, len, &cdc);
        if((char)*data == 't')
        {
            printf("Temp: %k\r\n", getTemperature());
        }
        if((char)*data == 'm')
        {
            printf("Millis: %ld\r\n", millis_get());
        }
        cdc_read_more(&cdc);
}

static int
blink(millis_t time)
{
    onboard_led(ONBOARD_LED_TOGGLE);

    //timeout_add(&t, 1000, blink, NULL);

    return 0;
}

static int blinkAnother(millis_t time)
{
    gpio_toggle(GPIO_PTB0);
    //timeout_add(&anotherCtx, 1001, blinkAnother, NULL);

    return 0;
}

void
init_vcdc(int config)
{
        cdc_init(new_data, NULL, &cdc);
        cdc_set_stdout(&cdc);
}

static void aShortCallback(void)
{
    printf("Short Press 1!\r\n");
}

static void aLongCallback(void)
{
    printf("Long Press 1!\r\n");
}

static void aShortCallback1(void)
{
    printf("Short Press 2!\r\n");
}

static void aLongCallback1(void)
{
    printf("Long Press 2!\r\n");
}

static void aShortCallback2(void)
{
    printf("Short Press 3!\r\n");
}

static void aLongCallback2(void)
{
    printf("Long Press 3!\r\n");
}

int
main(void)
{
    pin_mode(PIN_PTC0, PIN_MODE_MUX_ALT2); // Flash CS pin
    pin_mode(PIN_PTC4, PIN_MODE_MUX_ALT2);//SPI Pin mode initialization
    pin_mode(PIN_PTC5, PIN_MODE_MUX_ALT2);
    pin_mode(PIN_PTC6, PIN_MODE_MUX_ALT2);
    pin_mode(PIN_PTC7, PIN_MODE_MUX_ALT2);
   
    gpio_dir(GPIO_PTB0, GPIO_OUTPUT);
    pin_mode(GPIO_PTB0, PIN_MODE_DRIVE_HIGH);

    usb_init(&cdc_device);
    spi_init();

    millis_init();

    initButtons();

    buttonSetCallback(0, aShortCallback, 0);
    buttonSetCallback(0, aLongCallback, 1);

    buttonSetCallback(1, aShortCallback1, 0);
    buttonSetCallback(1, aLongCallback1, 1);

    buttonSetCallback(2, aShortCallback2, 0);
    buttonSetCallback(2, aLongCallback2, 1);

    initializeTemperature();

    scheduleFunction(blink, 0, 0, 1000);
    //scheduleFunction(blinkAnother, 0, 0, 250);

    while(1)
    {
        scheduleRun(millis_get());
    }

    return 0;
}

