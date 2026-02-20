#include <Arduino.h>

#include "lab_1_1/lab_1_1_app.h"
#include "lab_1_2/lab_1_2_app.h"
    
#define LAB_1_1 11
#define LAB_1_2 12

#define APP_NAME LAB_1_2


void setup() {
    #if APP_NAME == LAB_1_1
        lab1_1Setup();
    #elif APP_NAME == LAB_1_2
        lab1_2Setup();
    #endif
}

void loop() {
    #if APP_NAME == LAB_1_1
        lab1_1Loop();
    #elif APP_NAME == LAB_1_2
        lab1_2Loop();
    #endif
}

