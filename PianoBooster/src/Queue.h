/*********************************************************************************/
/*!
@file           Queue.h

@brief          xxxx.

@author         L. J. Barman

    Copyright (c)   2008-2009, L. J. Barman, all rights reserved

    This file is part of the PianoBooster application

    PianoBooster is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PianoBooster is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PianoBooster.  If not, see <http://www.gnu.org/licenses/>.

*/
/*********************************************************************************/

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <assert.h>

// A Queue or circular buffer also also call a FIFO a First In First Out buffer
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
        delete [] m_buffer;
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

    // this should be atomic operation when two different threads are at each end of the queue
    volatile int m_count;
};

#endif //__QUEUE_H__
