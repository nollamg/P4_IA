#include "config.h"
#include "shared_data.h"
#include "dsp_utils.h"
#include "task_dsp.h"

static float bufferA[DSP_WINDOW_SIZE];
static float bufferB[DSP_WINDOW_SIZE];

static float* activeBuf = bufferA;
static float* procBuf   = bufferB;

static uint16_t idx = 0;

void Task_DSP(void* pv) {
    Serial.println("[Task_DSP] Iniciada");

    for (;;) {
        uint32_t bufPtr;
        xTaskNotifyWait(0, 0, &bufPtr, portMAX_DELAY);

        float* buf = (float*)bufPtr;

        float Irms = computeRMS(buf, DSP_WINDOW_SIZE);
        float mean = computeMean(buf, DSP_WINDOW_SIZE);
        float stddev = computeStdDev(buf, DSP_WINDOW_SIZE, mean);
        float THD = estimateTHDPercent(Irms, stddev);

        DspResult_t res = {
            millis(),
            Irms,
            computeActivePower(Irms),
            computeReactivePower(Irms),
            THD
        };

        // Guardar último resultado global
        gLastResult = res;

        // Enviar a Storage y Anomalia
        xQueueSend(gQueueDSP_Storage,  &res, 0);
        xQueueSend(gQueueDSP_Anomalia, &res, 0);

        // Señalizar fin de ciclo DSP
        xEventGroupSetBits(gEventGroup, BIT_DSP_READY);

        // Latido watchdog
        xTaskNotify(gTaskHandle_Watchdog, 1, eSetValueWithOverwrite);
    }
}
