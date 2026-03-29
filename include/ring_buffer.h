#pragma once
#include "freertos/FreeRTOS.h"

typedef struct {
    float* data;
    size_t size;
    size_t head;
    size_t count;
} RingBuffer;

RingBuffer* rbCreate(size_t size);
void rbPush(RingBuffer* rb, float v);
float rbGet(RingBuffer* rb, size_t index);
