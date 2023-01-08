#include <stdint.h>
#include <stdlib.h>

#include "buffer.h"

#include <cmsis_gcc.h>

static uint32_t pmask;

static void disableInt();
static void enableInt();

struct Buffer {
    uint16_t ptrIn;
    uint16_t ptrOut;
    uint16_t size;
    uint16_t capacity;
    uint8_t *data;
};

Buffer * bufferCreate(const uint16_t capacity) {
    Buffer * const buffer = (Buffer *) malloc(sizeof(Buffer));
    buffer->data = (uint8_t *) malloc(sizeof(uint8_t) * capacity);
    buffer->capacity = capacity;
    bufferClear(buffer);
    return buffer;
}

uint8_t bufferPush(Buffer * const buffer, const uint8_t value) {
    if (buffer->size < buffer->capacity) {
        disableInt();

        buffer->data[buffer->ptrIn] = value;
        buffer->ptrIn = (buffer->ptrIn + 1) % buffer->capacity;
        buffer->size++;

        enableInt();
        return 1;
    } else
        return 0;
}

uint16_t bufferPushValues(Buffer * const buffer, const uint8_t * const values, const uint16_t size) {
    uint16_t pushed;
    for (pushed = 0; pushed < size; pushed++) {
        if (!bufferPush(buffer, values[pushed]))
            return pushed;
    }
    return pushed;
}

uint8_t bufferPop(Buffer * const buffer) {
    if (buffer->size == 0)
        return 0;

    disableInt();

    buffer->size--;
    uint8_t value = buffer->data[buffer->ptrOut];
    buffer->ptrOut = (buffer->ptrOut + 1) % buffer->capacity;

    enableInt();

    return value;
}

uint8_t bufferPeek(const Buffer * const buffer) {
    return buffer->data[buffer->ptrOut];
}

void bufferClear(Buffer * const buffer) {
    disableInt();
    buffer->size = 0;
    buffer->ptrIn = 0;
    buffer->ptrOut = 0;
    enableInt();
}

uint8_t bufferHasValues(const Buffer * const buffer) {
    return buffer->size > 0;
}

uint16_t bufferPopValues(Buffer * const buffer, uint8_t * const buf, const uint16_t bufSize) {
    if (!bufferHasValues(buffer))
        return 0;
    uint16_t popSize = bufferGetSize(buffer);
    if (popSize > bufSize)
        popSize = bufSize;
    for (uint16_t i = 0; i < popSize; i++) {
        buf[i] = bufferPop(buffer);
    }
    return popSize;
}

uint16_t bufferGetSize(const Buffer * const buffer) {
    return buffer->size;
}

uint16_t bufferGetFreeSize(const Buffer * const buffer) {
    return buffer->capacity - buffer->size;
}

uint16_t bufferPeekValues(const Buffer * const buffer, uint8_t **values) {
    *values = buffer->data + buffer->ptrOut;
    return buffer->size;
}

uint8_t bufferToAnother(Buffer * destination, Buffer * source) {
	if (!bufferHasValues(source) || destination->capacity < source->size)
	    return 0;
	uint16_t size = source->size;
	for (uint16_t i = 0; i < size; i++) {
		uint8_t var = bufferPop(source);
		bufferPush(destination, var);
	}
	return 1;
}

static void disableInt() {
	pmask = __get_PRIMASK();
    __disable_irq();
}

static void enableInt() {
    __set_PRIMASK(pmask);
}
