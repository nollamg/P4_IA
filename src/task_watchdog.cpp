#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "shared_data.h"
#include "config.h"
#include "task_watchdog.h"

/**
 * @brief Task_Watchdog
 * Supervisa que todas las tareas envíen un latido (heartbeat)
 * mediante xTaskNotify().
 *
 * Si no recibe un latido en 3 segundos:
 *   → reinicia el sistema con esp_restart()
 *
 * Además reinicia el timer gTimerAppWatchdog para supervisión global.
 */
void Task_Watchdog(void* pvParameters)
{
    (void) pvParameters;
    Serial.println("[Task_Watchdog] Iniciada en Core " + String(xPortGetCoreID()));

    for (;;) {
        uint32_t beat = 0;

        // Espera un latido durante 3 segundos
        BaseType_t notified = xTaskNotifyWait(
            0,          // bits a limpiar al entrar
            0,          // bits a limpiar al salir
            &beat,      // valor recibido
            pdMS_TO_TICKS(3000)
        );

        if (notified == pdFALSE) {
            Serial.println("[WATCHDOG] No se recibió latido → reinicio");
            esp_restart();
        }

        // Reinicia el timer del watchdog de aplicación
        xTimerReset(gTimerAppWatchdog, 0);
    }
}
