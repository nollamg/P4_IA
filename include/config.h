#pragma once
#include <Arduino.h>

// =========================
// HARDWARE
// =========================

#define PIN_SCT013              4       // GPIO para sensor SCT-013
#define ADC_RESOLUTION          4095.0f
#define ADC_REF_VOLTAGE         3.3f

// =========================
// MUESTREO
// =========================

#define SAMPLE_FREQUENCY_HZ     2000.0f
#define SAMPLE_PERIOD_US        (1000000.0f / SAMPLE_FREQUENCY_HZ)

// Ventana DSP (p.ej. 0,1 s a 2 kHz → 200 muestras)
#define DSP_WINDOW_SIZE         200

// =========================
// RED ELÉCTRICA
// =========================

#define GRID_V_RMS              230.0f  // Tensión RMS típica en Europa
#define DEFAULT_COS_PHI         0.9f

// =========================
// INFLUXDB / RED
// =========================

static const char* WIFI_SSID    = "TU_SSID";
static const char* WIFI_PASS    = "TU_PASSWORD";

static const char* INFLUX_URL   =
    "http://192.168.1.100:8086/api/v2/write?org=home&bucket=energia&precision=s";

static const char* INFLUX_TOKEN = "TU_TOKEN_INFLUX";

// =========================
// WATCHDOG / STACK
// =========================

#define STACK_MIN_SAFE_WORDS    100     // margen mínimo de stack “seguro”
#define WATCHDOG_PERIOD_MS      5000
