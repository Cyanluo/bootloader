#include "queue.h"

void static_queue(pQueue queue, const uint16_t capacity)
{
    queue->m_capacity = capacity;
    queue->m_front = 0;
    queue->m_rear = 0;
    queue->m_length = 0;
}

int capacity(pQueue queue)
{
    return queue->m_capacity;
}

uint8_t pop(pQueue queue)
{
    uint8_t ret = 0;

    if (queue->m_length > 0)
    {
        queue->m_front = (queue->m_front + 1) % queue->m_capacity;
        queue->m_length--;
        ret = 1;
    }

    return ret;
}

uint16_t popn(pQueue queue, const uint16_t size)
{
    uint16_t ret = 0;

    if (queue->m_length > size)
    {
        queue->m_front = (queue->m_front + size) % queue->m_capacity;
        queue->m_length -= size;
        ret = size;
    }
    else
    {
        queue->m_front = queue->m_rear;
        queue->m_length = 0;
        ret = queue->m_length;
    }

    return ret;
}

void clear(pQueue queue)
{
    queue->m_front = 0;
    queue->m_rear = 0;
    queue->m_length = 0;
}

uint16_t length(pQueue queue)
{
    return queue->m_length;
}

// ---------------------------------- CharQueue -------------------------------------

void char_queue(pCharQueue queue, uint8_t data[], const uint16_t capacity)
{
    queue->m_buffer = data;
    static_queue((pQueue)queue, capacity);
}

uint8_t front(pQueue queue)
{
    pCharQueue char_queue = (pCharQueue)queue;

    return char_queue->m_buffer[char_queue->m_front];
}

uint8_t* fetch(pQueue queue, uint16_t size)
{
    uint8_t* ret = (void*)0;

    if (queue->m_length <= size)
    {
        pCharQueue char_queue = (pCharQueue)queue;
        ret = char_queue->m_buffer + char_queue->m_front;
        popn(queue, size);
    }

    return ret;
}

uint8_t push(pQueue queue, uint8_t data)
{
    uint8_t ret = 0;
    pCharQueue char_queue = (pCharQueue)queue;

    if (char_queue->m_length < queue->m_capacity)
    {
        char_queue->m_buffer[char_queue->m_rear] = data;
        char_queue->m_rear = (queue->m_rear + 1) % queue->m_capacity;
        char_queue->m_length++;
        ret = 1;
    }

    return ret;
}

uint16_t pushn(pQueue queue, uint8_t* data, uint16_t length)
{
    uint16_t ret = 0;
    uint8_t has_cap = 1;

    while (length-- && has_cap)
    {
        has_cap = push(queue, *data++);
        ret++;
    }

    return ret;
}
