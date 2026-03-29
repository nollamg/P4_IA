#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "freertos/task.h"

// =========================
// ESTRUCTURAS
// =========================

typedef struct {
    uint32_t timestamp_ms;
    float    current_A;
} AdcSample_t;

typedef struct {
    uint32_t timestamp_ms;
    float    Irms_A;
    float    P_active_W;
    float    Q_reactive_var;
    float    THD_percent;
} DspResult_t;

// Último resultado DSP global (para Task_Display)
extern DspResult_t gLastResult;

// =========================
// COLAS
// =========================

extern QueueHandle_t gQueueADC;
extern QueueHandle_t gQueueDSP_Storage;
extern QueueHandle_t gQueueDSP_Anomalia;

// =========================
// MUTEX
// =========================

extern SemaphoreHandle_t gMutexInflux;

// =========================
// EVENT GROUP
// =========================

extern EventGroupHandle_t gEventGroup;

#define BIT_DSP_READY      (1 << 0)
#define BIT_STORAGE_DONE   (1 << 1)
#define BIT_ANOMALIA_DONE  (1 << 2)

// =========================
// TIMERS
// =========================

extern TimerHandle_t gTimerInfluxTimeout;
extern TimerHandle_t gTimerAppWatchdog;

// =========================
// TASK HANDLES
// =========================

extern TaskHandle_t gTaskHandle_ADC;
extern TaskHandle_t gTaskHandle_DSP;
extern TaskHandle_t gTaskHandle_Storage;
extern TaskHandle_t gTaskHandle_Anomalia;
extern TaskHandle_t gTaskHandle_Display;
extern TaskHandle_t gTaskHandle_Watchdog;
