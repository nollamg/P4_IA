#include "ring_buffer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portable.h"

RingBuffer* rbCreate(size_t size) {
    RingBuffer* rb = (RingBuffer*)pvPortMalloc(sizeof(RingBuffer));
    rb->data = (float*)pvPortMalloc(sizeof(float) * size);
    rb->size = size;
    rb->head = 0;
    rb->count = 0;
    return rb;
}

void rbPush(RingBuffer* rb, float v) {
    rb->data[rb->head] = v;
    rb->head = (rb->head + 1) % rb->size;
    if (rb->count < rb->size) rb->count++;
}

float rbGet(RingBuffer* rb, size_t index) {
    if (index >= rb->count) return 0;
    size_t pos = (rb->head + rb->size - rb->count + index) % rb->size;
    return rb->data[pos];
}
