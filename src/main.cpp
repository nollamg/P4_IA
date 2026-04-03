#include <Arduino.h>
#include <WiFi.h>

#include "config.h"
#include "shared_data.h"

#include "task_adc.h"
#include "task_dsp.h"
#include "task_storage.h"
#include "task_anomalia.h"
#include "task_display.h"
#include "task_watchdog.h"
#include "task_ai_analytics.h"

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("\n[Main] Sistema FreeRTOS avanzado con IA de análisis de consumo");

    // =========================
    // WIFI
    // =========================
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("[WiFi] Conectando");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[WiFi] Conectado. IP: " + WiFi.localIP().toString());

    // =========================
    // COLAS
    // =========================
    gQueueADC          = xQueueCreate(256, sizeof(AdcSample_t));
    gQueueDSP_Storage  = xQueueCreate(64, sizeof(DspResult_t));
    gQueueDSP_Anomalia = xQueueCreate(64, sizeof(DspResult_t));
    gQueueDSP_AI       = xQueueCreate(32, sizeof(DspResult_t));   // NUEVA cola para IA

    if (!gQueueADC || !gQueueDSP_Storage || !gQueueDSP_Anomalia || !gQueueDSP_AI) {
        Serial.println("[Main] ERROR creando colas");
        while (true) vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // =========================
    // MUTEX
    // =========================
    gMutexInflux = xSemaphoreCreateMutex();
    if (!gMutexInflux) {
        Serial.println("[Main] ERROR creando mutex Influx");
        while (true) vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // =========================
    // EVENT GROUP
    // =========================
    gEventGroup = xEventGroupCreate();
    if (!gEventGroup) {
        Serial.println("[Main] ERROR creando EventGroup");
        while (true) vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // =========================
    // TIMERS
    // =========================
    gTimerInfluxTimeout = xTimerCreate(
        "InfluxTimeout",
        pdMS_TO_TICKS(5000),
        pdFALSE,
        nullptr,
        [](TimerHandle_t xTimer){
            Serial.println("[Storage] Timeout InfluxDB");
        }
    );

    gTimerAppWatchdog = xTimerCreate(
        "AppWatchdog",
        pdMS_TO_TICKS(3000),
        pdFALSE,
        nullptr,
        [](TimerHandle_t xTimer){
            Serial.println("[WATCHDOG] Reinicio por falta de latido");
            esp_restart();
        }
    );

    if (!gTimerInfluxTimeout || !gTimerAppWatchdog) {
        Serial.println("[Main] ERROR creando timers");
        while (true) vTaskDelay(pdMS_TO_TICKS(1000));
    }

    xTimerStart(gTimerAppWatchdog, 0);

    // =========================
    // TAREAS
    // =========================

    // Muestreo ADC (crítica)
    xTaskCreatePinnedToCore(
        Task_ADC,
        "Task_ADC",
        4096,
        nullptr,
        4,
        &gTaskHandle_ADC,
        1
    );

    // DSP
    xTaskCreatePinnedToCore(
        Task_DSP,
        "Task_DSP",
        8192,
        nullptr,
        3,
        &gTaskHandle_DSP,
        0
    );

    // Storage → InfluxDB
    xTaskCreate(
        Task_Storage,
        "Task_Storage",
        6144,
        nullptr,
        2,
        &gTaskHandle_Storage
    );

    // Anomalías
    xTaskCreate(
        Task_Anomalia,
        "Task_Anomalia",
        4096,
        nullptr,
        3,
        &gTaskHandle_Anomalia
    );

    // Display / dashboard
    xTaskCreate(
        Task_Display,
        "Task_Display",
        4096,
        nullptr,
        1,
        &gTaskHandle_Display
    );

    // Watchdog
    xTaskCreate(
        Task_Watchdog,
        "Task_Watchdog",
        4096,
        nullptr,
        1,
        &gTaskHandle_Watchdog
    );

    // IA de análisis (FFT, TFLite, predicción, recomendaciones)
    xTaskCreate(
        Task_AI_Analytics,
        "Task_AI_Analytics",
        8192,          // provisional, se ajusta con uxTaskGetStackHighWaterMark
        nullptr,
        2,             // misma prioridad que Storage, por no ser tiempo real duro
        nullptr
    );
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}
