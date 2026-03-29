#pragma once
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Task_Watchdog
 * Tarea de vigilancia del sistema.
 * 
 * - Recibe latidos (heartbeats) de otras tareas vía xTaskNotify().
 * - Si no recibe un latido en el tiempo configurado, reinicia el sistema.
 * - También reinicia el timer gTimerAppWatchdog para supervisión global.
 */
void Task_Watchdog(void* pvParameters);

#ifdef __cplusplus
}
#endif
