#include <Arduino.h>

#include "lab_1_1/lab_1_1_app.h"

#define APP_NAME LAB_1_1

void setup() {
    #if APP_NAME == LAB_1_1
        lab1_1Setup();
    #endif
}

void loop() {
    #if APP_NAME == LAB_1_1
        lab1_1Loop();
    #endif
}

