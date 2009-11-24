/*********************************************************************************/
/*!
@file           Scroll.h

@brief          The Design.

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

#ifndef __SCROLL_H__
#define __SCROLL_H__


#include "Draw.h"
#include "Song.h"
#include "Queue.h"

#define QUEUE_LENGTH    1000

class CSettings;

class CScroll : public CDraw
{
public:
    CScroll(int id, CSettings* settings) : CDraw(settings)
    {
        m_id = id;
        m_symbolID = 0;

        m_notation = new CNotation();
        m_scrollQueue = new CQueue<CSlotDisplayList>(QUEUE_LENGTH);
        reset();
        m_show = false;
        m_noteSpacingFactor = 1.0;
        m_ppqnFactor = 1.0;
        m_transpose = 0;
    }

    ~CScroll()
    {
        delete m_scrollQueue;
        delete m_notation;
    }
    void reset();
    void scrollDeltaTime(int ticks);
    void transpose(int transpose);
    void refresh();
    void setPlayedNoteColour(int note, CColour colour, int wantedDelta, int pianistTimming);
    void setChannel(int chan)
    {
        m_notation->setChannel( chan );
    }

    //! add a midi event to be analysed and displayed on the score
    void midiEventInsert(CMidiEvent event) { m_notation->midiEventInsert(event);}

    //! first check if there is space to add a midi event
    int midiEventSpace() { return m_notation->midiEventSpace(); }

    void drawScrollingSymbols(bool show);
    void showScroll(bool show);

private:
    class CSlotDisplayList : public CSlot
    {
        public:
        CSlotDisplayList(): m_displayListId(0){};
        CSlotDisplayList(const CSlot &slot, GLuint displayListId, GLuint nextDisplayListId);

        GLuint m_displayListId; // the open GL display list id for this slot
        GLuint m_nextDisplayListId; // and this points to the next one
    };

    void compileSlot(CSlotDisplayList info);
    bool validPianistChord(int index);
    bool insertSlots();
    void removeSlots();
    void removeEarlyTimingMakers();
    int findWantedChord(int note, CColour colour, int wantedDelta);

    int m_id;      // There are lots of these class running but each class has a unique id
    CNotation *m_notation;
    int m_deltaHead;
    int m_deltaTail;

    GLuint m_symbolID; // the next Display List name (or ID) to use
    CSlot m_headSlot;   // The next slot to be put in at the head of the queue;

    int m_transpose;
    int m_wantedIndex;  // The index number of the wanted call in the scrollQueue
    int m_wantedDelta; // The running delta time of the wanted chord

    CQueue<CSlotDisplayList>* m_scrollQueue;  // The current active display list of notes/chords on the screen
    bool m_show; // set to true to show on the screen
    float m_noteSpacingFactor;
    float m_ppqnFactor; // if PulsesPerQuarterNote is 96 then the factor is 1.0
};


#endif //__SCROLL_H__
