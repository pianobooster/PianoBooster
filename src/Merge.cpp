/*********************************************************************************/
/*!
@file           Merge.cpp

@brief          Merge Midi Events from multiple streams into a single stream.

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
/////////////////////////////////////////////////////////////////////////////

#include "Merge.h"




void  CMerge::initMergedEvents()
{
    int i;
    for( i = 0; i < m_mergeEvents.size(); i++)
    {
        m_mergeEvents[i].clear();
        if (checkMidiEventFromStream(i) )
            m_mergeEvents[i] = fetchMidiEventFromStream(i);
    }
}


int CMerge::nextMergedEvent()
{
    int nearestIndex = 0;
	
    int i;
    CMidiEvent* nearestEvent;
    int deltaTime;

	nearestEvent = 0;
    // find the first active slot
    for( i = 0; i < m_mergeEvents.size(); i++)
    {
        if (m_mergeEvents[i].type() != MIDI_NONE)
        {
            nearestEvent = &m_mergeEvents[i];
            nearestIndex = i;
            break;
        }
    }
    if (nearestEvent == 0)
        return 0;

    // now search the remaining active slots
    for( i = nearestIndex + 1; i < m_mergeEvents.size(); i++)
    {
        if (m_mergeEvents[i].type() != MIDI_NONE)
        {
            // find the slot with the lowest delta time
            if (m_mergeEvents[i].deltaTime() < nearestEvent->deltaTime())
            {
                nearestEvent = &m_mergeEvents[i];
                nearestIndex = i;
            }
        }
    }

    deltaTime =  -nearestEvent->deltaTime();

    // Now subtract the delta time from all the others
    for( i = 0; i < m_mergeEvents.size(); i++)
    {
        if (i == nearestIndex)
            continue;
        if (m_mergeEvents[i].type() != MIDI_NONE)
            m_mergeEvents[i].addDeltaTime( deltaTime );
    }
  
    return nearestIndex;
}


CMidiEvent CMerge::readMidiEvent()
{
    int mergeIdx;
    CMidiEvent event;

    mergeIdx = nextMergedEvent();
    event = m_mergeEvents[mergeIdx];

    m_mergeEvents[mergeIdx].clear();
	if (checkMidiEventFromStream(mergeIdx) )
		m_mergeEvents[mergeIdx] = fetchMidiEventFromStream(mergeIdx);
    if (event.type() == MIDI_NONE)
        event.setType(MIDI_PB_EOF);
    return event;
}
