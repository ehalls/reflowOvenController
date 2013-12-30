#include <mchck.h>

#include "usb-serial-loopback.desc.h"

static struct cdc_ctx cdc;
static struct timeout_ctx t;
static struct spi_ctx thermocouple;

uint8_t fakeBuffer[2];
uint8_t rxBuffer[2];

static void
tempCallback(void *data)
{
    uint16_t tempTemp = (uint16_t)rxBuffer[0] << 8 | rxBuffer[1];
    uint16_t temp = (tempTemp >> 5);
    printf("Temp: %d\r\n", temp);
}

static void
new_data(uint8_t *data, size_t len)
{
        cdc_write(data, len, &cdc);
        if((char)*data == 't')
        {
            spi_queue_xfer(&thermocouple, SPI_PCS0, fakeBuffer, 2, rxBuffer, 2, tempCallback, NULL);
        }
        cdc_read_more(&cdc);
}

static void
blink(void *data)
{
    onboard_led(ONBOARD_LED_TOGGLE);
    timeout_add(&t, 1000, blink, NULL);
}

void
init_vcdc(int config)
{
        cdc_init(new_data, NULL, &cdc);
        cdc_set_stdout(&cdc);
}

void
main(void)
{
    pin_mode(PIN_PTC4, PIN_MODE_MUX_ALT2);
    pin_mode(PIN_PTC5, PIN_MODE_MUX_ALT2);
    pin_mode(PIN_PTC6, PIN_MODE_MUX_ALT2);
    pin_mode(PIN_PTC7, PIN_MODE_MUX_ALT2);

    usb_init(&cdc_device);
    spi_init();
    timeout_init();
    blink(NULL);
    sys_yield_for_frogs();
}
