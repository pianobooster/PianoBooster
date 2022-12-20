/*********************************************************************************/
/*!
@file           Score.h

@brief          xxx.

@author         L. J. Barman

    Copyright (c)   2008-2013, L. J. Barman, all rights reserved

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

#ifndef _SCORE_H_
#define _SCORE_H_

#include "Scroll.h"
#include "Piano.h"
#include "Settings.h"

class CScore : public CDraw
{
public:

    CScore(CSettings* settings);

    ~CScore();

    void init();

    //! add a midi event to be analysed and displayed on the score
    void midiEventInsert(CMidiEvent event)
    {
        for (int i=0; i < arraySize(m_scroll); i++)
        {
            m_scroll[i]->midiEventInsert(event);
        }
    }

    //! first check if there is space to add a midi event
    int midiEventSpace()
    {
        int minSpace = 1000;
        for (auto *const scroll : m_scroll) // this maybe slow
        {
            int space = scroll->midiEventSpace();
            if (space < minSpace)
                minSpace = space;
        }
        return minSpace;
    }

    void transpose(int semitones)
    {
        for (auto *const scroll : m_scroll)
            scroll->transpose(semitones);
    }

    void reset()
    {
        for (auto *const scroll : m_scroll)
            scroll->reset();
    }

    void drawScrollingSymbols(bool show = true)
    {
        for (auto *const scroll : m_scroll)
            scroll->drawScrollingSymbols(show);
    }

    void scrollDeltaTime(qint64 ticks)
    {
        for (auto *const scroll : m_scroll)
            scroll->scrollDeltaTime(ticks);
    }

    void setRatingObject(CRating* rating)
    {
        m_rating = rating;
    }

    CPiano* getPianoObject() { return m_piano;}

    void setPlayedNoteColor(int note, CColor color, qint64 wantedDelta, qint64 pianistTimming = NOT_USED)
    {
        if (m_activeScroll>=0)
            m_scroll[m_activeScroll]->setPlayedNoteColor(note, color, wantedDelta, pianistTimming);
    }

    void setActiveChannel(int channel)
    {
        int newActiveSroll;

        if (channel < 0 || channel >= arraySize(m_scroll))
            return;
        newActiveSroll = channel;

        if (m_activeScroll != newActiveSroll)
        {
            if (m_activeScroll>=0)
                m_scroll[m_activeScroll]->showScroll(false);
            m_activeScroll = newActiveSroll;
            m_scroll[m_activeScroll]->showScroll(true);
        }
    }

    void refreshScroll()
    {
        if (m_activeScroll>=0)
            m_scroll[m_activeScroll]->refresh();
    }

    void setDisplayHand(whichPart_t hand)
    {
        CDraw::setDisplayHand(hand);
        refreshScroll();
    }

    void drawScore();
    void drawScroll(bool refresh);
    void drawPianoKeyboard();

protected:
    CPiano* m_piano;

private:
    CRating* m_rating;
    CScroll* m_scroll[MAX_MIDI_CHANNELS];
    int m_activeScroll;
    GLuint m_scoreDisplayListId;
    GLuint m_stavesDisplayListId;

};

#endif // _SCORE_H_
