#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define RING_BUFFER_SIZE 64

typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
    bool is_overflow;       		  // overflow indicator flag (true = has overflowed)
	uint32_t overflow_count;          // count data loss size (byte)
} RingBuffer_t;

void RingBuffer_Init(RingBuffer_t *ring_buffer);
bool RingBuffer_Push(RingBuffer_t *ring_buffer, uint8_t data);
bool RingBuffer_Pop(RingBuffer_t *ring_buffer, uint8_t *data);
bool RingBuffer_HasOverflowed(RingBuffer_t *rb);
uint32_t RingBuffer_GetOverflowCount(RingBuffer_t *rb);
void RingBuffer_ClearOverflow(RingBuffer_t *rb);
bool RingBuffer_IsEmpty(const RingBuffer_t *rb);

#endif
