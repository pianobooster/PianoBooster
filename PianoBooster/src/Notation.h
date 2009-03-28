/*********************************************************************************/
/*!
@file           Notation.h

@brief          xxx.

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

#ifndef __NOTATION_H__
#define __NOTATION_H__

#include <assert.h>

#include "MidiFile.h"
#include "Queue.h"
#include "Symbol.h"
#include "Chord.h"
#include "Bar.h"


#define MAX_SYMBOLS    20  // The maximum number of symbols that can be stored in one slot

class CSlot
{
public:
    CSlot()
    {
        clear();
    }
    void clear()
    {
        m_length = 0;
        m_deltaTime = 0;
        m_av8Left = 0;
        m_av8Right = 0;
        m_maxLeftEdge = 0;
    }

    CSymbol getSymbol(int index) {return m_symbols[index];}
    musicalSymbol_t getSymbolType(int index) {return m_symbols[index].getType();}
    int length() {return m_length;}
    void setDeltaTime(int delta) {m_deltaTime = delta;}
    void addDeltaTime(int delta) {m_deltaTime += delta;}
    int getDeltaTime() {return m_deltaTime;}
    int getLeftSideDeltaTime() {return m_deltaTime + m_maxLeftEdge;}

    void setAv8Left(int val) {m_av8Left = val;}
    int getAv8Left() {return m_av8Left;}

    // return false on error
    bool addSymbol(CSymbol symbol)
    {
        if (m_length >= MAX_SYMBOLS)
            return false;
        for (int i=0; i < m_length; i++)
        {
            // don't add duplicates
            if (m_symbols[i].getNote() == symbol.getNote() &&
                m_symbols[i].getType() == symbol.getType() &&
                m_symbols[i].getHand() == symbol.getHand())
                return true;
        }
        m_symbols[m_length++] = symbol;
        return true;
    }

    void setSymbol(int delta, CSymbol symbol)
    {
        clear();
        setDeltaTime(delta);
        addSymbol(symbol);
    }


    void transpose(int amount)
    {
        for (int i = 0; i < m_length; i++)
        {
            m_symbols[i].transpose(amount);
        }
    }

    void setNoteColour(int note, CColour colour )
    {
        for (int i = 0; i < m_length; i++)
        {
            if (note == m_symbols[i].getNote() || note == 0)
                m_symbols[i].setColour(colour);
        }
    }

    void setNoteTimming(int note, int timing)
    {
        for (int i = 0; i < m_length; i++)
        {
            if (note == m_symbols[i].getNote() || note == 0)
                m_symbols[i].setPianistTiming(timing);
        }
        if (timing < m_maxLeftEdge)
            m_maxLeftEdge = timing;
    }

    void clearAllNoteTimmings()
    {
        for (int i = 0; i < m_length; i++)
        {
            m_symbols[i].setPianistTiming(NOT_USED);
        }
        m_maxLeftEdge = 0;
    }

protected:

private:
    int m_deltaTime;

    CSymbol m_symbols[MAX_SYMBOLS];
    int m_length;
    int m_av8Left;
    int m_av8Right;
    int m_maxLeftEdge; // the furthest the note will appear on the left hand edge (used when removing the note)
};


// Define a chord
class CNotation
{
public:
    CNotation()
    {
        m_midiInputQueue = new CQueue<CMidiEvent>(1000);
        m_slotQueue = new CQueue<CSlot>(200);
        reset();
        m_displayChannel = 0;
    }
    ~CNotation()
    {
        delete m_midiInputQueue;
        delete m_slotQueue;
    }
    void reset();

    void setChannel(int channel) {m_displayChannel = channel;}


    CSlot nextSlot();
    void midiEventInsert(CMidiEvent event);

    int midiEventSpace() { return m_midiInputQueue->space();};

private:
    CSlot nextBeatMarker();
    int nextMergeSlot();
    void findNoteSlots();
    CSlot nextNoteSlot();

    CQueue<CSlot>* m_slotQueue;             // Queue of symbol slots that have not been read yet
    CQueue<CMidiEvent>* m_midiInputQueue;   // A Queue of midi events
    CSlot m_currentSlot;
    int m_currentDeltaTime;
    int m_beatPerBarCounter;
    CSlot m_mergeSlots[2];
    int m_displayChannel;
    CFindChord m_findScrollerChord;
    CBar m_bar;
};

#endif  // __NOTATION_H__

