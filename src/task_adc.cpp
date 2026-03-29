#include "config.h"
#include "shared_data.h"
#include "dsp_utils.h"
#include "task_adc.h"

/**
 * @brief Task_ADC
 * Tarea crítica fijada al Core 1, prioridad 4.
 * Muestrea el ADC a ~2 kHz, convierte a corriente y envía las muestras a gQueueADC.
 */
static float bufferA[DSP_WINDOW_SIZE];
static float bufferB[DSP_WINDOW_SIZE];

static float* activeBuf = bufferA;
static float* procBuf   = bufferB;

static uint16_t idx = 0;

void Task_ADC(void* pv) {
    for (;;) {
        float sample = adcToCurrentA(analogRead(PIN_SCT013));
        activeBuf[idx++] = sample;

        if (idx >= DSP_WINDOW_SIZE) {
            idx = 0;

            // Intercambio ping‑pong
            float* tmp = activeBuf;
            activeBuf = procBuf;
            procBuf   = tmp;

            // Notificar a DSP qué buffer procesar
            xTaskNotify(gTaskHandle_DSP,
                        (uint32_t)procBuf,
                        eSetValueWithOverwrite);
        }
    }
}