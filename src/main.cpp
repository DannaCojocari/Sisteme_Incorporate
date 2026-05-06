#include <Arduino.h>

#include "lab_1_1/lab_1_1_app.h"
#include "lab_1_2/lab_1_2_app.h"
#include "lab_2_1/lab_2_1_app.h"
#include "lab_2_2/lab_2_2_app.h"
#include "lab_3_1/lab_3_1_app.h"
#include "lab_3_2/lab_3_2_app.h"
#include "lab_4_1/lab_4_1_app.h"
#include "lab_4_2/lab_4_2_app.h"
#include "lab_5_1/lab_5_1_app.h"
#include "lab_5_2/lab_5_2_app.h"

#define LAB_1_1 11
#define LAB_1_2 12
#define LAB_2_1 21
#define LAB_2_2 22
#define LAB_3_1 31
#define LAB_3_2 32
#define LAB_4_1 41
#define LAB_4_2 42
#define LAB_5_1 51
#define LAB_5_2 52

#define APP_NAME LAB_5_2


void setup() {
    #if APP_NAME == LAB_1_1
        lab1_1Setup();
    #elif APP_NAME == LAB_1_2
        lab1_2Setup();
    #elif APP_NAME == LAB_2_1
        lab2_1Setup();
    #elif APP_NAME == LAB_2_2
        lab2_2Setup();
    #elif APP_NAME == LAB_3_1
        lab3_1Setup();
    #elif APP_NAME == LAB_3_2
        lab3_2Setup();
    #elif APP_NAME == LAB_4_1
        lab4_1Setup();
    #elif APP_NAME == LAB_4_2
        lab4_2Setup();
    #elif APP_NAME == LAB_5_1
        lab5_1Setup();
    #elif APP_NAME == LAB_5_2
        lab5_2Setup();
    #endif
}

void loop() {
    #if APP_NAME == LAB_1_1
        lab1_1Loop();
    #elif APP_NAME == LAB_1_2
        lab1_2Loop();
    #elif APP_NAME == LAB_2_1
        lab2_1Loop();
    #elif APP_NAME == LAB_2_2
        lab2_2Loop();
    #elif APP_NAME == LAB_3_1
        lab3_1Loop();
    #elif APP_NAME == LAB_3_2
        lab3_2Loop();
    #elif APP_NAME == LAB_4_1
        lab4_1Loop();
    #elif APP_NAME == LAB_4_2
        lab4_2Loop();
    #elif APP_NAME == LAB_5_1
        lab5_1Loop();
    #elif APP_NAME == LAB_5_2
        lab5_2Loop();
    #endif
}