//MCHCK Headers
#include <mchck.h>
#include "lib/mchck/taskqueue.h"
#include "usb-serial-loopback.desc.h"

//Libc Headers
#include <stdbool.h>
#include "stdlib.h" //Note: this isn't in the stdlib, had to do it myself.

//App Headers
#include "millis.h"
#include "paramStorage.h"
#include "temperatureMonitor.h"
#include "armfix.h"

#include "menuHandler.h"

static struct cdc_ctx cdc;

static char inputBuffer[80];
static uint8_t bufferSize = 0;

static int baseMenuHandler(millis_t currentTime);
queuedFunction currentCallback = baseMenuHandler;

bool stateMonitoring = false;

static void handlerCleanup(void)
{
    memset(inputBuffer, 0, bufferSize);
    bufferSize = 0;
}

static void printRootMenu(void)
{
    printf(
        "Reflow Oven Menu\r\n"
        "================\r\n"
        "1. Toggle State Monitoring\r\n"
        "2. Dump State\r\n"
        "3. Set Reflow Params\r\n"
        "4. Set Dessicate Temp\r\n"
        "5. Set Reflow PID Params\r\n");
}

static void dumpState(void)
{
    printf("Temp: %k ", getTemperature());
    printf("State: ");
    printf("\r\n");
}

static void dumpReflowParams(void)
{
    printf("\r\n");
    printf("Soak Min. Temp (C): %k\r\n", params.soak_mintemp);
    printf("Soak Max Temp (C): %k\r\n", params.soak_maxtemp);
    printf("Soak Rate (C/min): %k\r\n", params.soak_rate);
    printf("Reflow Max Temp (C): %k\r\n", params.reflow_maxtemp);
    printf("\r\n");
    printf("Enter new temperatures, or blank to keep.\r\n");
    printf("New Soak Min. Temp (C): "); fflush(stdout);
}

static void dumpPidParams(void)
{
    printf("\r\n");
    printf("Reflow P: %k\r\n", params.reflow_p);
    printf("Reflow I: %k\r\n", params.reflow_i);
    printf("Reflow D: %k\r\n", params.reflow_d);
    printf("Soak P: %k\r\n", params.soak_p);
    printf("Soak I: %k\r\n", params.soak_i);
    printf("Soak D: %k\r\n", params.soak_d);
    printf("\r\n");
    printf("Enter new parameters, or blank to keep.\r\n");
    printf("New Reflow P: "); fflush(stdout);
}

static int reflowParamHandler(millis_t currentTime)
{

    static int step = 0;
    static fix_s15_16_t *pArray[4] = { 
        &(params.soak_mintemp),
        &(params.soak_maxtemp),
        &(params.soak_rate),
        &(params.reflow_maxtemp)
    };

    static const char prompts[4][27] = {
        "New Soak Min. Temp (C): \0",
        "New Soak Max Temp (C): \0",
        "New Soak Rate (C/min): \0",
        "New Reflow Max Temp (C): \0"
    };

    if(bufferSize != 0)
    {
        *(pArray[step]) = atof(inputBuffer);
        printf("\r\nSet %k\r\n", *(pArray[step]));
    }
    step++;
    if(step == 4)
    {
        step = 0;
        printf("\r\n");
        currentCallback = baseMenuHandler;
        paramWrite();
    } else {
        cdc_write_string(prompts[step], &cdc);
    }

    handlerCleanup();

    return 0;
}

static int dessicateEntryHandler(millis_t currentTime)
{
    float newTemp;
    newTemp = atof(inputBuffer);
    params.dessicate_temp = (fix_s15_16_t)newTemp;
    printf("Set: %k\r\n", params.dessicate_temp);
    currentCallback = baseMenuHandler;
    paramWrite();

    handlerCleanup();

    return 0;
}

static int pidParamHandler(millis_t currentTime)
{
    static int step = 0;
    static fix_s15_16_t *pArray[6] = { 
        &(params.reflow_p),
        &(params.reflow_i),
        &(params.reflow_d),
        &(params.soak_p),
        &(params.soak_i),
        &(params.soak_d)
    };

    static const char prompts[6][16] = {
        "New Reflow P: \0",
        "New Reflow I: \0",
        "New Reflow D: \0",
        "New Soak P: \0",
        "New Soak I: \0",
        "New Soak D: \0"
    };

    if(bufferSize != 0)
    {
        *(pArray[step]) = atof(inputBuffer);
        printf("\r\nSet %k\r\n", *(pArray[step]));
    }
    step++;
    if(step == 6)
    {
        step = 0;
        printf("\r\n");
        currentCallback = baseMenuHandler;
        paramWrite();
    } else {
        cdc_write_string(prompts[step], &cdc);
    }

    handlerCleanup();

    return 0;
}

static int baseMenuHandler(millis_t currentTime)
{
    if(bufferSize != 1 || inputBuffer[0] == '?')
    {
        printRootMenu();
    } else if(inputBuffer[0] == '1')
    {
        if(stateMonitoring == true)
        {
            stateMonitoring = false;
            printf("\r\nState Monitoring Disabled.\r\n\r\n");
        } else {
            stateMonitoring = true;
            printf("\r\nState Monitoring Enabled.\r\n\r\n");
        }
    } else if(inputBuffer[0] == '2')
    {
        dumpState();
    } else if(inputBuffer[0] == '3')
    {
        //Dump current params, print menu, change handler
        dumpReflowParams();
        currentCallback = reflowParamHandler;
    } else if(inputBuffer[0] == '4')
    {
        //Prompt, change handler
        printf("Current Dessicate Temp: %k\r\n", params.dessicate_temp);
        printf("New Temp: ");
        fflush(stdout);
        currentCallback = dessicateEntryHandler;
    } else if(inputBuffer[0] == '5')
    {
        //Dump current params, print menu, change handler
        dumpPidParams();
        currentCallback = pidParamHandler;
    }

    handlerCleanup();

    return 0;
}

static void
new_data(uint8_t *data, size_t len)
{
        cdc_write(data, len, &cdc);
        if((char)*data == '\r')
        {
            //We have to schedule the function 
            scheduleFunction(currentCallback, ID_MENU_CALLBACK, 0, 0);

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
        cdc_init(new_data, NULL, &cdc);
        cdc_set_stdout(&cdc);
}

void menuInit(void)
{
    usb_init(&cdc_device);
}