/*********************************************************************************/
/*!
@file           Bar.h

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

#ifndef __BAR_H__
#define __BAR_H__


#include "MidiFile.h"


// The event bits can be ORed together
#define EVENT_BITS_playingStopped          0x0001  //set when we reach the end of piece
#define EVENT_BITS_forceFullRedraw         0x0002 // force the whole screen to be redrawn
#define EVENT_BITS_forceRatingRedraw       0x0004 // force the score to be redrawn
#define EVENT_BITS_newBarNumber            0x0008 // force the bar number to be redrawn
#define EVENT_BITS_UptoBarReached          0x0010 // Used for looping when playing between two bars.

typedef unsigned long eventBits_t;



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
    int getBarLength() {return m_barLength;} // in ppqn

    void setPlayFromBar(double bar);
    void setPlayFromBar(int bar, int beat = 0, int ticks = 0);
    void reset() {
        setTimeSig( 0 , 0);
        m_playFromBar = 0.0;
        m_playUptoBar = 0.0;
        m_loopingBars = 0.0;
        m_seekingBarNumber = false;
        m_flushTicks = false;
        m_eventBits = 0;
        setupEnableFlags();
    }

    void setPlayUptoBar(double bar);
    double getPlayUptoBar(){ return m_playUptoBar;}
    void setLoopingBars(double bars);
    double getLoopingBars(){ return m_loopingBars;}

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

    // get and reset the current bar event bits
    eventBits_t readEventBits() {
        eventBits_t bits = m_eventBits;
        m_eventBits = 0;
        return bits;
    }

    int goToBarNumer();

private:
    void checkGotoBar();
    void setupEnableFlags()
    {
        m_enableLooping = (m_loopingBars > 0.0)?true:false;
        m_enablePlayFromBar = (m_enableLooping || m_playFromBar > 0.0)?true:false;
    }



    int m_deltaTime;
    int m_beatLength; //in ppqn ticks
    int m_barLength; // m_beatLength * getTimeSigTop() (also in ppqn ticks)

    int m_startTimeSigTop; // The time Sig at the start of the piece
    int m_startTimeSigBottom;
    int m_currentTimeSigTop; // The current time Sig at the start of the piece
    int m_currentTimeSigBottom;

    int m_barCounter;
    int m_beatCounter;
    double m_playFromBar;
    double m_playUptoBar;
    double m_loopingBars;
    int m_playFromBeat;
    int m_playFromTicks;
    bool m_seekingBarNumber;
    bool m_flushTicks;
    eventBits_t m_eventBits;
    bool m_enableLooping;
    bool m_enablePlayFromBar;

};

#endif  // __BAR_H__

