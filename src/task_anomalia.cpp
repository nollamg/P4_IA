#include "config.h"
#include "shared_data.h"
#include "task_anomalia.h"

/**
 * @brief Task_Anomalia
 * Prioridad 3.
 * Consume resultados DSP desde gQueueDSP_Anomalia y detecta:
 *  - Dispositivo encendido/apagado
 *  - Picos de potencia
 *  - THD elevada
 * Señaliza fin de ciclo con EventGroup y envía latido al watchdog.
 */
void Task_Anomalia(void* pvParameters) {
    (void) pvParameters;
    Serial.println("[Task_Anomalia] Iniciada en Core " + String(xPortGetCoreID()));

    const float THRESHOLD_P_ON      = 50.0f;    // W
    const float THRESHOLD_P_SPIKE   = 2000.0f;  // W
    const float THRESHOLD_THD_HIGH  = 20.0f;    // %

    bool deviceOn = false;

    for (;;) {
        DspResult_t res;
        if (xQueueReceive(gQueueDSP_Anomalia, &res, portMAX_DELAY) == pdTRUE) {
            // Detección de dispositivo encendido
            if (!deviceOn && res.P_active_W > THRESHOLD_P_ON) {
                deviceOn = true;
                Serial.printf("[Anomalia] Dispositivo encendido. P=%.1f W\n", res.P_active_W);
            } else if (deviceOn && res.P_active_W < (THRESHOLD_P_ON * 0.5f)) {
                deviceOn = false;
                Serial.println("[Anomalia] Dispositivo apagado.");
            }

            // Pico de consumo
            if (res.P_active_W > THRESHOLD_P_SPIKE) {
                Serial.printf("[Anomalia] PICO de consumo! P=%.1f W\n", res.P_active_W);
            }

            // THD alta
            if (res.THD_percent > THRESHOLD_THD_HIGH) {
                Serial.printf("[Anomalia] THD elevada: %.1f %%\n", res.THD_percent);
            }

            // Señalizar fin de ciclo de análisis
            xEventGroupSetBits(gEventGroup, BIT_ANOMALIA_DONE);

            // Latido para watchdog
            xTaskNotify(gTaskHandle_Watchdog, 1, eSetValueWithOverwrite);
        }
    }
}
