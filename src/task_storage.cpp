#include <WiFi.h>
#include <HTTPClient.h>

#include "config.h"
#include "shared_data.h"
#include "task_storage.h"

/**
 * @brief sendToInflux
 * Envío de un punto a InfluxDB usando HTTP.
 * Protegido externamente por gMutexInflux.
 */
static bool sendToInflux(const DspResult_t& res, float energy_Wh_minute) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[Influx] WiFi no conectado");
        return false;
    }

    HTTPClient http;
    http.begin(INFLUX_URL);
    http.addHeader("Authorization", String("Token ") + INFLUX_TOKEN);
    http.addHeader("Content-Type", "text/plain; charset=utf-8");

    String line = "consumo,ubicacion=hogar "
                  "Irms=" + String(res.Irms_A, 4) + ","
                  "Pactiva=" + String(res.P_active_W, 2) + ","
                  "Qreactiva=" + String(res.Q_reactive_var, 2) + ","
                  "THD=" + String(res.THD_percent, 2) + ","
                  "Emin=" + String(energy_Wh_minute, 4) +
                  " " + String(res.timestamp_ms / 1000);

    int code = http.POST(line);
    if (code > 0) {
        Serial.printf("[Influx] HTTP %d\n", code);
    } else {
        Serial.printf("[Influx] Error HTTP: %s\n", http.errorToString(code).c_str());
    }
    http.end();
    return (code >= 200 && code < 300);
}

/**
 * @brief Task_Storage
 * Prioridad 2.
 * Agrega resultados DSP por minuto y los envía a InfluxDB vía HTTP.
 * Usa:
 *  - gQueueDSP_Storage para recibir datos
 *  - gMutexInflux para proteger HTTP
 *  - gTimerInfluxTimeout como timeout de envío
 *  - EventGroup para señalizar fin de ciclo
 *  - xTaskNotify para latido al watchdog
 */
void Task_Storage(void* pvParameters) {
    (void) pvParameters;
    Serial.println("[Task_Storage] Iniciada en Core " + String(xPortGetCoreID()));

    const uint32_t AGG_WINDOW_MS = 60000; // 1 minuto
    uint32_t windowStart = millis();

    float sumP = 0.0f;
    uint32_t count = 0;

    for (;;) {
        DspResult_t res;
        if (xQueueReceive(gQueueDSP_Storage, &res, portMAX_DELAY) == pdTRUE) {
            // Acumula potencia activa
            sumP += res.P_active_W;
            count++;

            uint32_t now = millis();
            if (now - windowStart >= AGG_WINDOW_MS) {
                float avgP = (count > 0) ? (sumP / count) : 0.0f;
                float energy_Wh = avgP * (AGG_WINDOW_MS / 3600000.0f);

                // Timeout de envío a InfluxDB
                xTimerStart(gTimerInfluxTimeout, 0);

                if (xSemaphoreTake(gMutexInflux, pdMS_TO_TICKS(500)) == pdTRUE) {
                    sendToInflux(res, energy_Wh);
                    xSemaphoreGive(gMutexInflux);
                }

                xTimerStop(gTimerInfluxTimeout, 0);

                // Reinicia ventana
                windowStart = now;
                sumP = 0.0f;
                count = 0;

                // Señalizar fin de ciclo de almacenamiento
                xEventGroupSetBits(gEventGroup, BIT_STORAGE_DONE);
            }

            // Latido para watchdog
            xTaskNotify(gTaskHandle_Watchdog, 1, eSetValueWithOverwrite);
        }
    }
}
