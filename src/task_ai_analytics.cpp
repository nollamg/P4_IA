#include "task_ai_analytics.h"
#include "shared_data.h"
#include "config.h"

// Cola de entrada desde DSP (declarada en shared_data.h/.cpp)
extern QueueHandle_t gQueueDSP_AI;

// Resultado IA para display / storage
typedef struct {
    float consumo_pred_mensual_kWh;
    char  dispositivo[32];
    char  recomendacion[96];
} AIAnalytics_t;

AIAnalytics_t gAIAnalytics;

// =========================
// PLACEHOLDERS DE IA
// =========================

static void computeFFTAndHarmonics(const DspResult_t& res) {
    // Aquí iría la FFT real (ej. kissFFT o similar) sobre ventanas de muestras.
    // En este ejemplo, solo mostramos un log.
    (void)res;
    // Serial.println("[IA] FFT y armónicos calculados (placeholder)");
}

static void runTFLiteClassifier(const DspResult_t& res, char* outDevice, size_t len) {
    // Aquí se invocaría a un modelo TFLite Micro pre-entrenado.
    // Placeholder: clasificamos según potencia activa.
    if (res.P_active_W > 1500.0f) {
        strlcpy(outDevice, "Horno", len);
    } else if (res.P_active_W > 800.0f) {
        strlcpy(outDevice, "Lavadora", len);
    } else if (res.P_active_W > 200.0f) {
        strlcpy(outDevice, "Frigorifico", len);
    } else {
        strlcpy(outDevice, "Standby", len);
    }
}

static float predictMonthlyConsumption() {
    // Regresión polinómica simplificada (placeholder).
    // En un caso real, usarías histórico en RAM.
    float a0 = 120.0f;
    float a1 = 0.5f;
    float x  = 10.0f; // días de uso, por ejemplo
    return a0 + a1 * x;
}

static void generateAdvice(const char* device, char* out, size_t len) {
    if (strcmp(device, "Horno") == 0) {
        strlcpy(out, "Usa el horno en franjas valle y evita precalentar más de lo necesario.", len);
    } else if (strcmp(device, "Lavadora") == 0) {
        strlcpy(out, "Agrupa lavados y usa programas de baja temperatura para ahorrar energía.", len);
    } else if (strcmp(device, "Frigorifico") == 0) {
        strlcpy(out, "Revisa el cierre de la puerta y ajusta la temperatura a 4 ºC.", len);
    } else {
        strlcpy(out, "Apaga consumos en standby cuando no estés en casa.", len);
    }
}

// =========================
// TAREA IA
// =========================

void Task_AI_Analytics(void* pvParameters) {
    (void) pvParameters;
    Serial.println("[Task_AI_Analytics] Iniciada en Core " + String(xPortGetCoreID()));

    for (;;) {
        DspResult_t res;
        if (xQueueReceive(gQueueDSP_AI, &res, portMAX_DELAY) == pdTRUE) {

            // 1) Análisis de calidad (FFT / armónicos)
            computeFFTAndHarmonics(res);

            // 2) Clasificación de electrodoméstico (TFLite simplificado)
            runTFLiteClassifier(res, gAIAnalytics.dispositivo, sizeof(gAIAnalytics.dispositivo));

            // 3) Predicción de consumo mensual
            gAIAnalytics.consumo_pred_mensual_kWh = predictMonthlyConsumption();

            // 4) Recomendaciones de ahorro
            generateAdvice(gAIAnalytics.dispositivo,
                           gAIAnalytics.recomendacion,
                           sizeof(gAIAnalytics.recomendacion));

            // Aquí podrías:
            //  - Enviar gAIAnalytics a una cola para Task_Storage (InfluxDB)
            //  - Dejar que Task_Display lea gAIAnalytics para mostrarlo en el dashboard

            // Medición de stack tras las primeras pruebas
            UBaseType_t watermark = uxTaskGetStackHighWaterMark(nullptr);
            Serial.printf("[IA] Stack high water mark: %u words libres\n", watermark);

            // Latido para watchdog (si quieres integrarlo)
            if (gTaskHandle_Watchdog != nullptr) {
                xTaskNotify(gTaskHandle_Watchdog, 1, eSetValueWithOverwrite);
            }
        }
    }
}
