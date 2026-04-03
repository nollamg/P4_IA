#include "shared_data.h"

// =========================
// VARIABLES GLOBALES
// =========================

DspResult_t gLastResult = {0};

// =========================
// COLAS
// =========================

QueueHandle_t gQueueADC          = nullptr;
QueueHandle_t gQueueDSP_Storage  = nullptr;
QueueHandle_t gQueueDSP_Anomalia = nullptr;
QueueHandle_t gQueueDSP_AI = nullptr;

// =========================
// MUTEX
// =========================

SemaphoreHandle_t gMutexInflux = nullptr;

// =========================
// EVENT GROUP
// =========================

EventGroupHandle_t gEventGroup = nullptr;

// =========================
// TIMERS
// =========================

TimerHandle_t gTimerInfluxTimeout = nullptr;
TimerHandle_t gTimerAppWatchdog   = nullptr;

// =========================
// TASK HANDLES
// =========================

TaskHandle_t gTaskHandle_ADC      = nullptr;
TaskHandle_t gTaskHandle_DSP      = nullptr;
TaskHandle_t gTaskHandle_Storage  = nullptr;
TaskHandle_t gTaskHandle_Anomalia = nullptr;
TaskHandle_t gTaskHandle_Display  = nullptr;
TaskHandle_t gTaskHandle_Watchdog = nullptr;
