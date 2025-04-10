#include "queue.h"
#include <memory.h>

void static_queue(pQueue queue, uint16_t capacity, uint8_t cover, queue_set set, queue_get get)
{
    queue->m_capacity = capacity;
    queue->m_front = 0;
    queue->m_rear = 0;
    queue->m_length = 0;
    queue->m_set = set;
    queue->m_get = get;
    queue->m_cover = cover;
}

uint16_t capacity(pQueue queue)
{
    return queue->m_capacity;
}

uint8_t is_cover(pQueue queue)
{
    return queue->m_cover;
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

void* front(pQueue queue)
{
    return queue->m_get(queue, queue->m_front);
}

uint16_t fetch(pQueue queue, uint16_t size, void* data)
{
    if (size == 0 || data == NULL)
        return size;

    if (queue->m_length < size)
    {
        size = queue->m_length;
    }

    if (queue->m_front < queue->m_rear)
    {
        memcpy(data, queue->m_get(queue, queue->m_front), size);
    }
    else
    {
        const int rest = queue->m_capacity - queue->m_front;
        memcpy(data, queue->m_get(queue, queue->m_front), rest);

        if (size > rest)
            memcpy(data + rest, queue->m_get(queue, 0), size-rest);
    }

    popn(queue, size);

    return size;
}

uint8_t push(pQueue queue, void* data)
{
    uint8_t ret = 0;

    if (data == NULL)
        return ret;

    if (queue->m_length < queue->m_capacity || queue->m_cover)
    {
        queue->m_set(queue, data, queue->m_rear);
        queue->m_rear = (queue->m_rear + 1) % queue->m_capacity;

        if (queue->m_length < queue->m_capacity)
            queue->m_length++;

        ret = 1;
    }

    return ret;
}

uint16_t pushn(pQueue queue, void* data, uint16_t length)
{
    uint16_t ret = 0;

    if (data == NULL)
        return ret;

    uint8_t has_cap = queue->m_length < queue->m_capacity || queue->m_cover;

    while (length-- && has_cap)
    {
        if (queue->m_length < queue->m_capacity)
            ret++;

        has_cap = push(queue, data++);
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

static void char_queue_set(void* queue, void* data, uint16_t pos)
{
    ((pCharQueue)queue)->m_buffer[pos] = *(uint8_t*)data;
}

static void* char_queue_get(void* queue, uint16_t pos)
{
    return ((pCharQueue)queue)->m_buffer + pos;
}

void char_queue(pCharQueue queue, uint8_t data[], const uint16_t capacity, uint8_t cover)
{
    queue->m_buffer = data;
    static_queue((pQueue)queue, capacity, cover, char_queue_set, char_queue_get);
}
