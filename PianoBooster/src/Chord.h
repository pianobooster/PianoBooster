/*********************************************************************************/
/*!
@file           Chord.h

@brief          Find the all the chords in a piece of music.

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

#ifndef __CHORD_H__
#define __CHORD_H__

#include <assert.h>

#include "Cfg.h"
#include "MidiFile.h"
#include "Queue.h"

////////////////////////////////////////////////////////////////////////////////
//! @brief The part for each hand uses a different midi channel
typedef enum
{
    PB_PART_both    = 200, // keep well clear of real midi channels
    PB_PART_right,
    PB_PART_left,
    PB_PART_accump,
    PB_PART_all,
    PB_PART_none,
} whichPart_t;


#define MAX_CHORD_NOTES    20  // The maximum notes in a chord well we only have 10 fingers

class CNote
{
public:
    CNote()
    {
        m_part = PB_PART_none;
        m_pitch = 0;
        m_duration = 0;
    }

    CNote(whichPart_t part, int note, int duration = 0)
    {
        m_part = part;
        m_pitch = note;
        m_duration = duration;
    }
    void transpose(int amount)
    {
        m_pitch += amount;
    }
    int pitch() {return m_pitch;}
    whichPart_t part() {return m_part;}
    void setPart(whichPart_t part) {m_part = part;}

    static whichPart_t findHand(int midiNote, int midiChannel, int whichChannel, whichPart_t whichPart);

    static whichPart_t findHand(CMidiEvent midi, int channel, whichPart_t part)
    {
        return findHand(midi.note(), midi.channel(), channel, part);
    }

    static void setChannelHands(int left, int right);
    static void setActiveHand(whichPart_t hand){m_activeHand = hand;}
    static whichPart_t getActiveHand(){return m_activeHand;}

    static int rightHandChan()       {return m_rightHandChannel;}
    static int leftHandChan()        {return m_leftHandChannel;}
    static int bothHandsChan()       {return m_leftHandChannel;}
    static int getHandChannel(whichPart_t whichPart)   { return (whichPart == PB_PART_right) ? m_rightHandChannel : m_leftHandChannel;}
    static bool hasPianoPart(int chan)   { return (m_leftHandChannel == chan || m_rightHandChannel == chan ) ? true : false;}

private:
    whichPart_t m_part;
    int m_pitch;
    int m_duration;
    static int m_leftHandChannel;
    static int m_rightHandChannel;
    static whichPart_t m_activeHand;

};

class CNoteRange
{
public:

    int highestNote; // The highest note in the range (often on the users piano keyboard)
    int lowestNote;
};

class CChord
{
public:
    CChord()
    {
        clear();
    }

    CNote getNote(int index) {return m_notes[index];}
    int length() {return m_length;}
    void setDeltaTime(int delta) {m_deltaTime = delta;}
    int getDeltaTime() {return m_deltaTime;}
    void clear() { m_length = 0; m_deltaTime = 0;}
    void addNote(whichPart_t part, int note, int duration = 0);
    bool removeNote(int note);
    bool searchChord(int note, int transpose = 0);
    int trimOutOfRangeNotes(int transpose);


    void transpose(int amount)
    {
        for (int i = 0; i < m_length; i++)
        {
            m_notes[i].transpose(amount);
        }
    }

    static void setPianoRange(int lowestNote, int highestNote ){
        m_cfg_highestPianoNote = highestNote; // The highest note on the users piano keyboard;
        m_cfg_lowestPianoNote = lowestNote;
    }

    static bool isNotePlayable(int note, int transpose )
    {
        note += transpose;
        if (note >=  m_cfg_lowestPianoNote && note <= m_cfg_highestPianoNote)
            return true;
        return false;
    }

    static bool isHandPlayable(whichPart_t hand)
    {
        if (CNote::getActiveHand() == PB_PART_both)
            return true;
        if (CNote::getActiveHand() == hand)
            return true;
        return false;
    }

private:
    int m_deltaTime;

    CNote m_notes[MAX_CHORD_NOTES];
    int m_length;
    static int m_cfg_highestPianoNote; // The highest note on the users piano keyboard;
    static int m_cfg_lowestPianoNote;

};


// Define a chord
class CFindChord
{
public:
    CFindChord()
    {
        reset();
    }

    void reset()
    {
        m_noteGapTime = 0;
        m_cordSpanGapTime = 0;
        m_completeChord.clear();
        m_currentChord.clear();
        m_cfg_ChordNoteGap = CMidiFile::ppqnAdjust(Cfg::chordNoteGap());
        m_cfg_ChordMaxLength = CMidiFile::ppqnAdjust(Cfg::chordMaxLength());
    }


    CChord getChord()
    {
        CChord chord;
        chord = m_completeChord;
        m_completeChord.clear();
        return chord;
    }

    bool findChord(CMidiEvent midi, int channel, whichPart_t part);
private:
    int m_noteGapTime;
    int m_cordSpanGapTime;
    CChord m_currentChord;
    CChord m_completeChord;
    int m_cfg_ChordNoteGap;
    int m_cfg_ChordMaxLength;
};

#endif  // __CHORD_H__

