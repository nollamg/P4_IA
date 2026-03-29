#pragma once
#include <Arduino.h>
#include "config.h"

// Funciones DSP sencillas, inline para eficiencia

// =========================
// ADC → Corriente
// =========================

inline float adcToCurrentA(int adcRaw) {
    float voltage = (adcRaw / ADC_RESOLUTION) * ADC_REF_VOLTAGE;
    // Factor de conversión simplificado (ajustar en calibración real)
    const float CONVERSION_FACTOR = 30.0f; // A/V (ejemplo)
    return voltage * CONVERSION_FACTOR;
}

// =========================
// RMS
// =========================

inline float computeRMS(const float* buf, size_t n) {
    if (n == 0) return 0.0f;
    float sumSq = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        sumSq += buf[i] * buf[i];
    }
    return sqrtf(sumSq / n);
}

// =========================
// MEDIA / VARIANZA / THD
// =========================

inline float computeMean(const float* buf, size_t n) {
    if (n == 0) return 0.0f;
    float sum = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        sum += buf[i];
    }
    return sum / n;
}

inline float computeStdDev(const float* buf, size_t n, float mean) {
    if (n == 0) return 0.0f;
    float var = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        float d = buf[i] - mean;
        var += d * d;
    }
    var /= n;
    return sqrtf(var);
}

inline float estimateTHDPercent(float Irms, float stddev) {
    if (Irms <= 0.0f) return 0.0f;
    return (stddev / Irms) * 100.0f;
}

// =========================
// POTENCIA ACTIVA / REACTIVA
// =========================

inline float computeActivePower(float Irms, float cosPhi = DEFAULT_COS_PHI) {
    return GRID_V_RMS * Irms * cosPhi;
}

inline float computeReactivePower(float Irms, float cosPhi = DEFAULT_COS_PHI) {
    float sinPhi = sqrtf(1.0f - cosPhi * cosPhi);
    return GRID_V_RMS * Irms * sinPhi;
}
