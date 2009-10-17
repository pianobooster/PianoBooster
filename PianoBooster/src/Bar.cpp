/*********************************************************************************/
/*!
@file           Bar.cpp

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

#include "Bar.h"

#define OPTION_DEBUG_BAR     0
#if OPTION_DEBUG_BAR
#define ppDEBUG_BAR(args)     ppLogDebug args
#else
#define ppDEBUG_BAR(args)
#endif


void CBar::setTimeSig(int top, int bottom)
{
    m_currentTimeSigTop = top;
    m_currentTimeSigBottom = bottom;
    if (bottom == 0 || m_startTimeSigBottom == 0) {
        m_startTimeSigTop = top;
        m_startTimeSigBottom = bottom;
        if (bottom == 0 )
        {
            m_currentTimeSigTop = 4;
            m_currentTimeSigBottom = 4;
            m_deltaTime = 0;
            m_beatCounter = 0;
            m_barCounter = 0;
        }
    }
    m_beatLength = (CMidiFile::getPulsesPerQuarterNote() *4)/ m_currentTimeSigBottom;
    m_barLength = m_beatLength * getTimeSigTop();
}

int CBar::addDeltaTime(int ticks)
{
    if (m_flushTicks == true && ticks != 0) // ignore this set of ticks
    {
        ppDEBUG_BAR(("addDeltaTime m_flushTicks ticks %d", ticks));
        m_flushTicks = false;
        return 0;
    }
    m_deltaTime +=ticks;
    if (ticks && m_enablePlayFromBar)
        checkGotoBar();
    while (m_deltaTime > m_beatLength * SPEED_ADJUST_FACTOR)
    {
        m_deltaTime -= m_beatLength * SPEED_ADJUST_FACTOR;
        m_beatCounter++;
        if (m_beatCounter >= m_currentTimeSigTop)
        {
            m_barCounter++;
            m_beatCounter=0;
            ppLogWarn("Bar number %d", m_barCounter);
            m_eventBits |= EVENT_BITS_newBarNumber;
        }
    }

    return ticks;
}

int CBar::goToBarNumer()
{
    int ticks;

    ticks = static_cast<int>((m_playFromBar - getCurrentBarPos()) * m_beatLength * SPEED_ADJUST_FACTOR + 1);

    addDeltaTime(ticks);
    return ticks;

}

void CBar::checkGotoBar()
{
    double currentBar = getCurrentBarPos();
    ppDEBUG_BAR(("checkGotoBar currentBarPos %.1f", currentBar));
    if (currentBar < m_playFromBar )
        m_seekingBarNumber = true;
    else
    {
        if (m_seekingBarNumber == true)
            m_flushTicks = true; // now throw away ticks before we start the music
        m_seekingBarNumber = false;

        if (m_enableLooping && currentBar > m_playUptoBar )
            m_eventBits |= EVENT_BITS_UptoBarReached;
    }
}


void CBar::setPlayFromBar(double bar)
{
    m_playFromBar = bar;
    m_playUptoBar = m_playFromBar + m_loopingBars;
    setupEnableFlags();
    checkGotoBar();
}

void CBar::setPlayFromBar(int bar, int beat, int ticks)
{
    double playFromBar = bar;
    setPlayFromBar( playFromBar);
}

void CBar::setPlayUptoBar(double endBar)
{
    setLoopingBars(endBar - m_playUptoBar);
}

void CBar::setLoopingBars(double bars)
{
    if (bars < 0.0)
        bars = 0;
    m_loopingBars = bars;
    m_playUptoBar = m_playFromBar + m_loopingBars;
    setupEnableFlags();
    checkGotoBar();
}
