/*                          oooOOO MidiFile.cpp OOOooo
* Reads a Standard MIDI File

    Copyright (c)   1993, 2008, L. J. Barman, all rights reserved

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
#include <stdio.h>
#include <stdlib.h>
#include "MidiFile.h"

int CMidiFile::m_ppqn = DEFAULT_PPQN;


/* Read 16 bits from the Standard MIDI file */
int CMidiFile::readWord(void)
{
    int value;

    value  = (m_file.get()&0x0ff) <<8 ;
    value |= m_file.get()&0x0ff;
    return value;
}

int CMidiFile::readHeader(void)
{
    size_t i;

    for ( i=0; i < 4; i++)
    {
        if (m_file.get() !="MThd"[i] )
        {
            midiError(SMF_CORRUPTED_MIDI_FILE);
            return 0;
        }
    }
    /* length */
    if ( readWord() != 0)
    {
        midiError(SMF_CORRUPTED_MIDI_FILE);
        return 0;
    }
    if ( readWord() != 6)
    {
        midiError(SMF_CORRUPTED_MIDI_FILE);
        return 0;
    }

    readWord();    /* midi file format */

    i = readWord();          /* ntrks (see Standard MIDI File Spec) */
    m_ppqn=readWord();          /* division */

    ppLogInfo("Tracks %d PPQN %d", i, m_ppqn);

    if (i == 0)
    {
        midiError(SMF_CORRUPTED_MIDI_FILE);
        return 0;
    }

    return i;
}


void CMidiFile::openMidiFile(string filename)
{
    if (m_file.is_open())
        m_file.close();
    m_file.clear();  // clear any errors

    m_file.open(filename.c_str(), ios_base::in | ios_base::binary);
    if (m_file.fail() == true)
    {
        ppError("Cannot open \"%s\"", filename.c_str());
        midiError(SMF_CANNOT_OPEN_FILE);
        return;
    }
    rewind();
}

void CMidiFile::rewind()
{
    size_t ntrks;
    size_t trk;
    dword_t trackLength;
    streampos filePos;

    midiError(SMF_NO_ERROR);
    m_ppqn = DEFAULT_PPQN;

    m_file.seekg (0, ios::beg);

    ntrks = readHeader();
    if (ntrks == 0)
    {
        midiError(SMF_CORRUPTED_MIDI_FILE);
        ppError("Zero tracks in SMF file");
        return;
    }
    if (ntrks > arraySize(m_tracks))
    {
        midiError(SMF_ERROR_TOO_MANY_TRACK);
        ppError("Too many tracks in SMF file");
        return;
    }
    for (trk = 0; trk < arraySize(m_tracks); trk++)
    {
        if (m_tracks[trk]!= 0)
        {
            delete (m_tracks[trk]);
            m_tracks[trk] = 0;
        }
    }
    filePos = m_file.tellg();
    for (trk = 0; trk < ntrks; trk++)
    {
        m_tracks[trk] = new CMidiTrack(m_file, trk);
        trackLength = m_tracks[trk]->init();
        m_tracks[trk]->decodeTrack();
        if (m_tracks[trk]->failed())
            break;

        //now move onto the next track
        filePos += trackLength;
        m_file.seekg (filePos, ios::beg);
    }
    m_songTitle = m_tracks[0]->getTrackName();
    initMergedEvents();
}

int CMidiFile::nextMergedEvent()
{
    size_t i;
    CMidiEvent* nearestEvent;
    size_t nearestIndex;
    int deltaTime;

    nearestEvent = 0;
    // find the first active slot
    for( i = 0; i < arraySize(m_mergeEvents); i++)
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
    for( i = nearestIndex + 1; i < arraySize(m_mergeEvents); i++)
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
    for( i = 0; i < arraySize(m_mergeEvents); i++)
    {
        if (i == nearestIndex)
            continue;
        if (m_mergeEvents[i].type() != MIDI_NONE)
            m_mergeEvents[i].addDeltaTime( deltaTime );
    }
    return nearestIndex;
}

void  CMidiFile::initMergedEvents()
{
    size_t i;
    for( i = 0; i < arraySize(m_mergeEvents); i++)
    {
        m_mergeEvents[i].clear();
        if (m_tracks[i] != 0 && m_tracks[i]->length() > 0)
            m_mergeEvents[i] = m_tracks[i]->pop();
    }

}


CMidiEvent CMidiFile::readMidiEvent()
{
    int mergeIdx;
    CMidiEvent event;

    mergeIdx = nextMergedEvent();
    event = m_mergeEvents[mergeIdx];

    m_mergeEvents[mergeIdx].clear();
    if (m_tracks[mergeIdx] != 0 && m_tracks[mergeIdx]->length() > 0)
        m_mergeEvents[mergeIdx] = m_tracks[mergeIdx]->pop();
    if (event.type() == MIDI_NONE)
        event.setType(MIDI_PB_EOF);
    event.setDeltaTime(event.deltaTime());
    return event;
}
