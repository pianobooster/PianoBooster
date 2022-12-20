/*********************************************************************************/
/*!
@file           Tempo.cpp

@brief          Tries to automatically calculate the tempo.

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

#include "Tempo.h"

int CTempo::m_cfg_followTempoAmount = 0;
int CTempo::m_cfg_maxJumpAhead = 0;

void CTempo::enableFollowTempo(bool enable)
{
    if (enable)
    {
        m_cfg_maxJumpAhead = 1;
        m_cfg_followTempoAmount = 1;
    }
    else
    {
        m_cfg_maxJumpAhead = 0;
        m_cfg_followTempoAmount = 0;
    }
}

void CTempo::adjustTempo(qint64 * ticks)
{
    if (m_jumpAheadDelta && m_cfg_maxJumpAhead && m_savedWantedChord)
    {
        if (m_jumpAheadDelta > 0)
            *ticks += m_jumpAheadDelta;

        // Automatically adjust the speed
        m_userSpeed = m_userSpeed + m_userSpeed * static_cast<float>(m_jumpAheadDelta) * 0.00002f;

        if (m_userSpeed> 2.0f) m_userSpeed = 2.0f;
        if (m_userSpeed < 0.2f) m_userSpeed = 0.2f;
        printf("%03.0f  %5lld\r", static_cast<double>(m_userSpeed) * 100.0, m_jumpAheadDelta);
        fflush(stdout);

        m_jumpAheadDelta = 0;
    }
}
