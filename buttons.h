#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h>

typedef void (*buttonCallback)(void);

void initButtons(void);

int buttonSetCallback(int buttonNumber, buttonCallback callback, bool longPress);


#endif