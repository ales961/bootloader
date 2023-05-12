#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

typedef struct Buffer Buffer;

Buffer * bufferCreate(uint16_t buffSize);
uint8_t bufferPush(Buffer *buffer, uint8_t value);
uint16_t bufferPushValues(Buffer *buffer, const uint8_t *values, uint16_t size);
uint8_t bufferPop(Buffer *buffer);
uint8_t bufferPeek(const Buffer *buffer);
void bufferClear(Buffer *buffer);
uint16_t bufferGetSize(const Buffer *buffer);
uint16_t bufferPopValues(Buffer *buffer, uint8_t *buf, uint16_t bufSize);
uint8_t bufferHasValues(const Buffer *buffer);
uint16_t bufferGetFreeSize(const Buffer *buffer);
uint16_t bufferPeekValues(const Buffer * buffer, uint8_t **values);
uint8_t bufferToAnother(Buffer * destination, Buffer * source);

#endif
