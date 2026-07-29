#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define RING_BUFFER_SIZE 64

typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE]; // array which contains data
    uint16_t head;                    // Write Pointer index (write input data)
    uint16_t tail;                    // Read Pointer index (read buffer data to process)
} RingBuffer_t;

void RingBuffer_Init(RingBuffer_t *ring_buffer);
bool RingBuffer_Push(RingBuffer_t *ring_buffer, uint8_t data);
bool RingBuffer_Pop(RingBuffer_t *ring_buffer, uint8_t *data);

#endif
