#include <mchck.h>

#include <stdbool.h>

#include "paramStorage.h"

#define FLASH_PAGE 0

config params;

static bool done = false;
struct spiflash_ctx sfctx;

void waitCallback(void *data)
{
    done = true;
}

void paramLoad(void)
{
    spiflash_read_page(&sfctx, (uint8_t*)&params, FLASH_PAGE, sizeof(params), waitCallback, NULL);
    while(!done); //Hopefully this never spinlocks.
    done = false;
}

void paramWrite(void)
{
    spiflash_erase_sector(&sfctx, FLASH_PAGE, waitCallback, NULL);
    while(!done);
    done = false;

    spiflash_program_page(&sfctx, FLASH_PAGE, (uint8_t*)&params, sizeof(params), waitCallback, NULL);
    while(!done);
    done = false;

}
