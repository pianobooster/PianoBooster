/*********************************************************************************/
/*!
@file           Merge.h

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

#ifndef __MERGE_H__
#define __MERGE_H__

#include <QVector>
#include "MidiEvent.h"

// Reads data from a standard MIDI file
class CMerge
{
public:
    CMerge()
    {
    }
	CMidiEvent readMidiEvent();
	    //you should always have a virtual destructor when using virtual functions
    virtual ~CMerge() {};

protected:
	void setSize(int size) {m_mergeEvents.resize(size);}
    void initMergedEvents();
    int nextMergedEvent();
	virtual bool checkMidiEventFromStream(int streamIdx) = 0;
	virtual CMidiEvent fetchMidiEventFromStream(int streamIdx)  = 0;

private:
    QVector<CMidiEvent> m_mergeEvents;
};

#endif // __MERGE_H__

