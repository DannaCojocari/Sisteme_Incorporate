#include <Arduino.h>

#include "lab_1/lab_1_app.h"

#define APP_NAME LAB_1

void setup() {
    #if APP_NAME == LAB_1
        lab1Setup();
    #endif
}

void loop() {
    #if APP_NAME == LAB_1
        lab1Loop();
    #endif
}

