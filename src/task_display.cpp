#include "config.h"
#include "shared_data.h"
#include "task_display.h"

/**
 * @brief updateWebDashboard
 * Simula la actualización de un dashboard web embebido.
 * En un proyecto real, aquí iría la lógica WebSocket.
 */
static void updateWebDashboard(const DspResult_t& res) {
    Serial.printf("[Display] Irms=%.3f A, P=%.1f W, THD=%.1f %%\n",
                  res.Irms_A, res.P_active_W, res.THD_percent);
}

/**
 * @brief Task_Display
 * Prioridad 1.
 * Espera a que DSP, Storage y Anomalia completen un ciclo
 * usando gEventGroup, y luego actualiza el dashboard con gLastResult.
 */
void Task_Display(void* pvParameters) {
    (void) pvParameters;
    Serial.println("[Task_Display] Iniciada en Core " + String(xPortGetCoreID()));

    const EventBits_t ALL_BITS =
        BIT_DSP_READY | BIT_STORAGE_DONE | BIT_ANOMALIA_DONE;

    for (;;) {
        // Espera a que todas las tareas hayan marcado su bit
        xEventGroupWaitBits(
            gEventGroup,
            ALL_BITS,
            pdTRUE,     // limpiar bits al salir
            pdTRUE,     // esperar a todos los bits
            portMAX_DELAY
        );

        // Usa el último resultado DSP global
        updateWebDashboard(gLastResult);

        // Latido para watchdog
        xTaskNotify(gTaskHandle_Watchdog, 1, eSetValueWithOverwrite);
    }
}
