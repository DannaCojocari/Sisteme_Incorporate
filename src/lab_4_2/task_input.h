#ifndef LAB_4_2_TASK_INPUT_H
#define LAB_4_2_TASK_INPUT_H

// Task 3 – UART Input (100 ms)
// Citeste comenzi de viteza prin Serial (scanf).
// Formatul asteptat: un numar intreg 0–100 urmat de Enter.
// Exemple: "50" → 50%, "0" → stop, "100" → viteza maxima.
// Valori invalide sunt ignorate (raw_cmd ramane neschimbat).
void l42_taskInput(void *pvParameters);

#endif // LAB_4_2_TASK_INPUT_H