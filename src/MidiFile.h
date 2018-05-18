/*********************************************************************************/
/*!
@file           MidiFile.h

@brief          xxxxx.

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

#ifndef __MIDIFILE_H__
#define __MIDIFILE_H__

#include <string>
#include <fstream>
#include "MidiEvent.h"
#include "MidiTrack.h"
#include "Merge.h"

#define DEFAULT_PPQN        96      /* Standard value for pulse per quarter note */

using namespace std;
#define MAX_TRACKS  40

// Reads data from a standard MIDI file
class CMidiFile : public CMerge
{
public:
    CMidiFile()
    {
        size_t i;
        midiError(SMF_NO_ERROR);
        m_ppqn = DEFAULT_PPQN;
        setSize(MAX_TRACKS);
        for (i = 0; i < arraySize(m_tracks); i++)
            m_tracks[i] = 0;
    }

    void openMidiFile(string filename);
    int readWord(void);
    int readHeader(void);
    void rewind();
    static int getPulsesPerQuarterNote(){return m_ppqn;}
    static int ppqnAdjust(float value) {
        return static_cast<int>((value * static_cast<float>(CMidiFile::getPulsesPerQuarterNote()))/DEFAULT_PPQN );
    }
    QString getSongTitle() {return m_songTitle;}

    void setLogLevel(int level){CMidiTrack::setLogLevel(level);}
    midiErrors_t getMidiError() { return m_midiError;}
    
private:
   	bool checkMidiEventFromStream(int streamIdx);
	CMidiEvent fetchMidiEventFromStream(int streamIdx);
    void midiError(midiErrors_t error) {m_midiError = error;}
    fstream m_file;
    static int m_ppqn;
    midiErrors_t m_midiError;
    CMidiTrack* m_tracks[MAX_TRACKS];
    QString m_songTitle;
};

#endif // __MIDIFILE_H__

