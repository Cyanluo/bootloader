#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

typedef void(*queue_set)(void* queue, void* data, uint16_t pos);
typedef void*(*queue_get)(void* queue, uint16_t pos);

typedef struct
{
    uint16_t m_front;
    uint16_t m_rear;
    uint16_t m_length;
    uint16_t m_capacity;
    uint8_t m_cover;
    queue_set m_set;
    queue_get m_get;
} Queue, *pQueue;

void static_queue(pQueue queue, uint16_t capacity, uint8_t cover, queue_set set, queue_get get);
uint16_t capacity(pQueue queue);
uint8_t is_cover(pQueue queue);
uint8_t pop(pQueue queue);
uint16_t popn(pQueue queue, uint16_t size);
void* front(pQueue queue);
uint16_t fetch(pQueue queue, uint16_t size, void* data);
uint8_t push(pQueue queue, void* data);
uint16_t pushn(pQueue queue, void* data, uint16_t length);
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
    uint8_t m_cover;
    queue_set m_set;
    queue_get m_get;
    uint8_t m_lock;
    uint8_t* m_buffer;
} CharQueue, *pCharQueue;

void char_queue(pCharQueue queue, uint8_t* data, const uint16_t capacity, uint8_t cover);

#endif //QUEUE_H
