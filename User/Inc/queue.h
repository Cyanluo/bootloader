#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

typedef struct
{
    uint16_t m_front;
    uint16_t m_rear;
    uint16_t m_length;
    uint16_t m_capacity;
} Queue, *pQueue;

void static_queue(pQueue queue, const uint16_t capacity);
int capacity(pQueue queue);
uint8_t pop(pQueue queue);
uint16_t popn(pQueue queue, const uint16_t size);
void clear(pQueue queue);
uint16_t length(pQueue queue);

// ---------------------------------- CharQueue -------------------------------------

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))

typedef struct
{
    uint16_t m_front;
    uint16_t m_rear;
    uint16_t m_length;
    uint16_t m_capacity;
    uint8_t* m_buffer;
} CharQueue, *pCharQueue;

void char_queue(pCharQueue queue, uint8_t* data, const uint16_t capacity);
uint8_t front(pQueue queue);
uint8_t* fetch(pQueue queue, uint16_t size);
uint8_t push(pQueue queue, uint8_t data);
uint16_t pushn(pQueue queue, uint8_t* data, uint16_t length);

#endif //QUEUE_H
