#include "stm32f401_ring_buffer.h"

void RingBuffer_Init(RingBuffer_t *ring_buffer){
    ring_buffer->head = 0;
    ring_buffer->tail = 0;
}
