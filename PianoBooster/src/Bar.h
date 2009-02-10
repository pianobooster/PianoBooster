/*********************************************************************************/
/*!
@file           Bar.h

@brief          xxx.

@author         L. J. Barman

    Copyright (c)   2008, L. J. Barman, all rights reserved

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

#ifndef __BAR_H__
#define __BAR_H__


#include "MidiFile.h"



// controls the bar numbers
class CBar
{
public:

    CBar() { reset(); }

    // You MUST clear the time sig to 0 first before setting an new start Time Signature
    // at the start of the piece of music
    void setTimeSig(int top, int bottom);

    int getTimeSigTop() {return m_currentTimeSigTop;} // The Numerator
    int getBeatLength() {return m_beatLength;}

    void setPlayFromBar(double bar);
    void setPlayFromBar(int bar, int beat = 0, int ticks = 0);
    void reset() {
        setTimeSig( 0 , 0);
        m_playFromBar = 0.0;
        m_seekingBarNumber = false;
        m_flushTicks = false;
    }

    void rewind() {
        int top = m_startTimeSigTop;
        int bottom = m_startTimeSigBottom;

        setTimeSig( 0 , 0);
        setTimeSig(top, bottom);
        checkGotoBar();
    }
    void getTimeSig(int *top, int *bottom) {
        *top = m_currentTimeSigTop;
        *bottom = m_currentTimeSigBottom;
    }

    //return true if bar number has changed
    int addDeltaTime(int ticks);

    //
    int getBarNumber(){ return m_barCounter;}

    double getCurrentBarPos() { return m_barCounter + static_cast<double>(m_beatCounter)/m_currentTimeSigBottom +
         static_cast<double>(m_deltaTime)/(m_beatLength * m_currentTimeSigBottom * SPEED_ADJUST_FACTOR); }


    bool seekingBarNumber() { return m_seekingBarNumber;}

    bool hasBarNumberChanged() {
        bool flag = m_hasBarNumberChanged;
        m_hasBarNumberChanged = false;
        return flag;
    }

    int goToBarNumer();


private:


    void checkGotoBar();

    int m_deltaTime;
    int m_beatLength;

    int m_startTimeSigTop; // The time Sig at the start of the piece
    int m_startTimeSigBottom;
    int m_currentTimeSigTop; // The current time Sig at the start of the piece
    int m_currentTimeSigBottom;

    int m_barCounter;
    int m_beatCounter;
    double m_playFromBar;
    int m_playFromBeat;
    int m_playFromTicks;
    bool m_seekingBarNumber;
    bool m_flushTicks;
    bool m_hasBarNumberChanged;
};

#endif  // __BAR_H__

