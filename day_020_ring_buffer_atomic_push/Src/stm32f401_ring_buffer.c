#include "stm32f401_ring_buffer.h"

void RingBuffer_Init(RingBuffer_t *ring_buffer){
    ring_buffer->head = 0;
    ring_buffer->tail = 0;
}

bool RingBuffer_Push(RingBuffer_t *ring_buffer, uint8_t data) {
    // buffer checking, full or not (head == Tail)
    uint16_t next_head = (ring_buffer->head + 1) % RING_BUFFER_SIZE;
    if (next_head == ring_buffer->tail) {
        return false; // buffer is full (overflow prevention)
    }

    // put data in the current head
    ring_buffer->buffer[ring_buffer->head] = data;

    // shift head (atomic update)
    ring_buffer->head = next_head;

    return true;
}
