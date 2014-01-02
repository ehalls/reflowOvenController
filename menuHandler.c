#include <mchck.h>

#include <stdbool.h>

#include "millis.h"

#include "usb-serial-loopback.desc.h"

#include "menuHandler.h"

static struct cdc_ctx cdc;

static char inputBuffer[80];
static uint8_t bufferSize = 0;

bool stateMonitoring = false;

typedef void (*menuCallback)(void);

static void dataIsSent(size_t amountSent)
{
    gpio_toggle(GPIO_PTB0);
}

static void printRootMenu(void)
{
    cdc_write((uint8_t*)(
        "Reflow Oven Menu\r\n"
        "================\r\n"
        "1. Toggle State Monitoring"), 62,
        &cdc);
    cdc_write_string("\r\n"
        "2. Dump State\r\n", &cdc);
        // "3. Set Reflow Params\r\n"
        // "4. Set Dessicate Temp\r\n"
        // "5. Set Reflow PID Params\r\n",
        // &cdc);
}

static void baseMenuHandler(void)
{
    if(bufferSize != 1 || inputBuffer[0] == '?')
    {
        printRootMenu();
    } else if(inputBuffer[0] == '1')
    {
        if(stateMonitoring == true)
        {
            stateMonitoring = false;
            printf("State Monitoring Disabled.\r\n");
            printRootMenu();
        } else {
            stateMonitoring = true;
            printf("State Monitoring Enabled.\r\n");
            printRootMenu();
        }
    } else if(inputBuffer[0] == '2')
    {
        //Dump state here
    } else if(inputBuffer[0] == '3')
    {
        //Dump current params, print menu, change handler
    } else if(inputBuffer[0] == '4')
    {
        //Prompt, change handler
    } else if(inputBuffer[0] == '5')
    {
        //Dump current params, print menu, change handler
    }
}

menuCallback currentCallback = baseMenuHandler;

static void
new_data(uint8_t *data, size_t len)
{
        cdc_write(data, len, &cdc);
        if((char)*data == '\r')
        {
            currentCallback();
            memset(inputBuffer, 0, bufferSize);
            bufferSize = 0;
        } else
        {
            inputBuffer[bufferSize] = (char)*data;
            bufferSize++;
        }
        // if((char)*data == 't')
        // {
        //     printf("Temp: %k\r\n", getTemperature());
        // }
        // if((char)*data == 'm')
        // {
        //     printf("Millis: %ld\r\n", millis_get());
        // }
        cdc_read_more(&cdc);
}

void
init_vcdc(int config)
{
        cdc_init(new_data, dataIsSent, &cdc);
        cdc_set_stdout(&cdc);
}

void menuInit(void)
{
    usb_init(&cdc_device);
}