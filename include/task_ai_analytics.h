#pragma once
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Task_AI_Analytics
 * 
 * Tarea de IA que:
 *  - Analiza calidad de red (FFT / armónicos).
 *  - Usa un modelo TFLite (pre-entrenado) para identificar electrodomésticos.
 *  - Calcula predicción de consumo mensual (regresión polinómica).
 *  - Genera recomendaciones de ahorro energético.
 * 
 * No es tiempo real duro → prioridad media (2).
 */
void Task_AI_Analytics(void* pvParameters);

#ifdef __cplusplus
}
#endif
