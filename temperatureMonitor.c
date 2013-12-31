#include <mchck.h>

#include "temperatureMonitor.h"

#include "lib/mchck/taskqueue.h"

static struct spi_ctx thermocouple;

static uint8_t temperatureData[2];

static float currentTemperature;

int temperatureCallback(millis_t time);

void initializeTemperature(void)
{
    scheduleFunction(temperatureCallback, 3, 0, 1000);
}

static void updateTemperature(void *data)
{
    if((temperatureData[1] & 0x04) == 0x04)
    {
        currentTemperature = -1.; //Error, open thermocouple.
    } else {
        currentTemperature = (((uint16_t)temperatureData[0] << 5) | (temperatureData[1] >> 3)) / 4.0;
    }
}

int temperatureCallback(millis_t time)
{
    spi_queue_xfer(&thermocouple, SPI_PCS0, temperatureData, 2, temperatureData, 2, updateTemperature, NULL);
    return 0;
}

float getTemperature(void)
{
    return currentTemperature;
}