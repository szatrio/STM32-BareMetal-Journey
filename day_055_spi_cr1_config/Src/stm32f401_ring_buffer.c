#include "stm32f401_ring_buffer.h"

void RingBuffer_Init(RingBuffer_t *ring_buffer){
    ring_buffer->head = 0;
    ring_buffer->tail = 0;
    ring_buffer->is_overflow = false;
    ring_buffer->overflow_count = 0;
}

bool RingBuffer_Push(RingBuffer_t *ring_buffer, uint8_t data) {
    // buffer checking, full or not (head == Tail)
    uint16_t next_head = (ring_buffer->head + 1) % RING_BUFFER_SIZE;
    if (next_head == ring_buffer->tail) {
    	ring_buffer->is_overflow = true;   // turn on flag error
    	ring_buffer->overflow_count++;
        return false; // buffer is full (overflow prevention)
    }

    // put data in the current head
    ring_buffer->buffer[ring_buffer->head] = data;

    // shift head (atomic update)
    ring_buffer->head = next_head;

    return true;
}

bool RingBuffer_Pop(RingBuffer_t *ring_buffer, uint8_t *data) {
	// checking if bufffer is empty
	if (ring_buffer->head == ring_buffer->tail) {
        return false; // empty buffer
    }

	// getting data from the current tail
	*data = ring_buffer->buffer[ring_buffer->tail];

	// shift tail to next index
	ring_buffer->tail = (ring_buffer->tail +1) % RING_BUFFER_SIZE;

	return true;
}

bool RingBuffer_HasOverflowed(RingBuffer_t *rb) {
    return rb->is_overflow;
}

uint32_t RingBuffer_GetOverflowCount(RingBuffer_t *rb) {
    return rb->overflow_count;
}

void RingBuffer_ClearOverflow(RingBuffer_t *rb) {
    rb->is_overflow = false;
    rb->overflow_count = 0;
}
