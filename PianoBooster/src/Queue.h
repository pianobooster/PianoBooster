#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <assert.h>

// A Queue  Circular buffer also also call a FIFO a First In First Out buffer
// different threads could be running each end of the queue

//
template <class TYPE>

class CQueue
{
public:
    explicit CQueue(int size)
    {
        m_size = size;
        m_buffer = new TYPE[size];
        clear();
    }

    ~CQueue()
    {
        delete m_buffer;
    }

    void clear()
    {
        m_count = m_head = m_tail=0;
    }
    void push(TYPE c)
    {
        if (!space())
        {
            assert(false);
            return;
        }
        m_buffer[m_head++] = c;
        if (m_head >= m_size)
            m_head = 0;

        // This must be last if a different thread is using pop()
        m_count++;
    }
    TYPE pop()
    {
        TYPE c;
        if (!length())
        {
            assert(false);
            return m_buffer[m_tail];
        }
        c = m_buffer[m_tail++];
        if (m_tail >= m_size)
            m_tail = 0;

        // This must be last if a different thread is using push()
        m_count--;
        return c;
    }

    TYPE * indexPtr(int index)
    {
        int offset;
        if (index >= length())
        {
            assert(false);
            return &m_buffer[m_head];
        }
        offset = m_tail + index;
        if (offset >= m_size)
            offset -= m_size;
        return &m_buffer[offset];
    }

    TYPE index(int index){ return *indexPtr(index);}

    int length() {return m_count;}
    int space() {return m_size - m_count;}
private:
    TYPE * m_buffer;
    int m_size;
    int m_head;
    int m_tail;

    // this should be atomic operation when two dfferent threads are at each end of the queue
    volatile int m_count;
};

#endif //__QUEUE_H__
