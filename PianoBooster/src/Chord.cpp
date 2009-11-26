/*********************************************************************************/
/*!
@file           Chord.cpp

@brief          Reads ahead from the songdata and collects chords to be matched.

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

#include "Chord.h"
#include "Cfg.h"

int CNote::m_leftHandChannel = -2;
int CNote::m_rightHandChannel = -2;
whichPart_t CNote::m_activeHand = PB_PART_both;

int CChord::m_cfg_highestPianoNote = 127; // The highest note on the users piano keyboard;
int CChord::m_cfg_lowestPianoNote = 0;

whichPart_t CNote::findHand(int midiNote, int midiChannel, int whichChannel, whichPart_t whichPart)
{
    whichPart_t hand = PB_PART_none;
    // exit if it is not for this channel
    if (midiChannel != whichChannel)
    {
        // return none if this is not being used with the other hand.
        if (CNote::hasPianoPart(whichChannel) == false || CNote::hasPianoPart(midiChannel) == false)
            return PB_PART_none;
    }

    if (midiChannel == CNote::rightHandChan())
        hand  = PB_PART_right;
    else if (midiChannel == CNote::leftHandChan())
        hand  = PB_PART_left;
    else
    {
        if (midiChannel == whichChannel)
        {
            if (midiNote >= MIDDLE_C)
                hand = PB_PART_right;
            else
                hand = PB_PART_left;
        }
    }
    if (whichPart == PB_PART_left && hand == PB_PART_right)
        hand = PB_PART_none;
    if (whichPart == PB_PART_right && hand == PB_PART_left)
        hand = PB_PART_none;
    return hand;
}


void CChord::addNote(whichPart_t part, int note, int duration)
{
    if (m_length >= MAX_CHORD_NOTES)
    {
        ppDEBUG(("Over the chord note limit"));
        return;
    }

    if (searchChord(note)) // don't add duplicates
        return;
    m_notes[m_length] = CNote(part, note, duration);
    m_length++;
}

void CNote::setChannelHands(int left, int right)
{
    m_leftHandChannel = left;
    m_rightHandChannel = right;
}

//////////////// CChord /////////////////////

bool CChord::removeNote(int note)
{
    int i;
    bool noteFound = false;

    for (i = 0; i < MAX_CHORD_NOTES; i++)
    {
        if (i >= m_length)
            break;
        if (noteFound == false)
        {
            if (getNote(i).pitch() == note)
                noteFound = true;
        }
        else
        {
            // shove everything else up to remove the note
            m_notes[i-1] = getNote(i);
        }
    }
    if (noteFound)
        m_length--;
    return noteFound;
}

bool CChord::searchChord(int note, int transpose)
{
    int i;

    for (i = 0; i < m_length; i++)
    {
        if (getNote(i).pitch() + transpose == note)
            return true;
    }
    return false;
}

int CChord::trimOutOfRangeNotes(int transpose)
{
    int i;
    int removedNotes = 0;

    for (i = 0; i < MAX_CHORD_NOTES; i++)
    {
        if (i >= m_length)
            break;

        if (!isNotePlayable(getNote(i).pitch(), transpose) || !isHandPlayable(getNote(i).part()))
            removedNotes++; // remove the note
        else if (removedNotes)
        {
            // shove everything else up to remove the note
            m_notes[i-removedNotes] = getNote(i);
        }
    }
    m_length -= removedNotes;
    return m_length;
}

bool CFindChord::findChord(CMidiEvent midi, int channel, whichPart_t part)
{
    bool foundChord = false;

    if (midi.type() == MIDI_PB_EOF)
    {
        if (m_currentChord.length() > 0)
        {
            m_completeChord = m_currentChord;
            foundChord = true;
        }
        return foundChord;
    }

    m_noteGapTime += midi.deltaTime();


    if ((m_noteGapTime >= m_cfg_ChordNoteGap || m_cordSpanGapTime > m_cfg_ChordMaxLength)
            && m_currentChord.length() > 0 )
    {
        foundChord = true;
        m_completeChord = m_currentChord;
        m_currentChord.clear();
    }

    if (midi.type() == MIDI_NOTE_ON)
    {
        whichPart_t hand = CNote::findHand( midi, channel, part );
        if ( hand != PB_PART_none)
        {
            m_currentChord.addNote(hand, midi.note());
            m_currentChord.setDeltaTime(m_noteGapTime + m_currentChord.getDeltaTime());
            if (m_currentChord.length() <= 1)
                m_cordSpanGapTime = 0;
            else
                m_cordSpanGapTime += m_noteGapTime; // measure the span of the cord
            m_noteGapTime = 0;
        }
    }
    return foundChord;
}

