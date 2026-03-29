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

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("\n[Main] Sistema FreeRTOS avanzado iniciado");

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

    if (!gQueueADC || !gQueueDSP_Storage || !gQueueDSP_Anomalia) {
        Serial.println("[Main] ERROR creando colas");
        while (true) vTaskDelay(1000);
    }

    // =========================
    // MUTEX
    // =========================
    gMutexInflux = xSemaphoreCreateMutex();

    // =========================
    // EVENT GROUP
    // =========================
    gEventGroup = xEventGroupCreate();

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

    xTimerStart(gTimerAppWatchdog, 0);

    // =========================
    // TAREAS
    // =========================

    xTaskCreatePinnedToCore(Task_ADC, "Task_ADC", 4096, nullptr, 4, &gTaskHandle_ADC, 1);
    xTaskCreatePinnedToCore(Task_DSP, "Task_DSP", 8192, nullptr, 3, &gTaskHandle_DSP, 0);

    xTaskCreate(Task_Storage, "Task_Storage", 6144, nullptr, 2, &gTaskHandle_Storage);
    xTaskCreate(Task_Anomalia, "Task_Anomalia", 4096, nullptr, 3, &gTaskHandle_Anomalia);
    xTaskCreate(Task_Display, "Task_Display", 4096, nullptr, 1, &gTaskHandle_Display);
    xTaskCreate(Task_Watchdog, "Task_Watchdog", 4096, nullptr, 1, &gTaskHandle_Watchdog);
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}
