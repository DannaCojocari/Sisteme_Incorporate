#include <Arduino.h>

#include "lab_1_1/lab_1_1_app.h"
#include "lab_1_2/lab_1_2_app.h"
#include "lab_2_1/lab_2_1_app.h"
    
#define LAB_1_1 11
#define LAB_1_2 12
#define LAB_2_1 21

#define APP_NAME LAB_2_1


void setup() {
    #if APP_NAME == LAB_1_1
        lab1_1Setup();
    #elif APP_NAME == LAB_1_2
        lab1_2Setup();
    #elif APP_NAME == LAB_2_1
        lab2_1Setup();
    #endif
}

void loop() {
    #if APP_NAME == LAB_1_1
        lab1_1Loop();
    #elif APP_NAME == LAB_1_2
        lab1_2Loop();
    #elif APP_NAME == LAB_2_1
        lab2_1Loop();
    #endif
}

