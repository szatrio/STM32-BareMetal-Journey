#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

// Ring buffer size
#define RING_BUFFER_SIZE 64

// Ring Buffer data structure
typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE]; // array which contains data
    uint16_t head;                    // Write Pointer index (write input data)
    uint16_t tail;                    // Read Pointer index (read buffer data to process)
} RingBuffer_t;

// init function
void RingBuffer_Init(RingBuffer_t *ring_buffer);

#endif
