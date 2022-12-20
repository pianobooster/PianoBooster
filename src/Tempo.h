/*********************************************************************************/
/*!
@file           Tempo.h

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

#ifndef __TEMPO_H__
#define __TEMPO_H__

#include "MidiEvent.h"
#include "MidiFile.h"
#include "Chord.h"

#define MICRO_SECOND 1000000.0f

// Define a chord
class CTempo
{
public:
    CTempo()
    {
        m_savedWantedChord = 0;
        reset();
    }
    void setSavedWantedChord(CChord * savedWantedChord) { m_savedWantedChord = savedWantedChord; }

    void reset()
    {
        // 120 beats per minute is the default
        setMidiTempo(static_cast<int>(( 60 * MICRO_SECOND ) / 120 ));
        m_jumpAheadDelta = 0;
    }

    // Tempo, microseconds-per-MIDI-quarter-note
    void setMidiTempo(int tempo)
    {
        m_midiTempo = (static_cast<float>(tempo) * DEFAULT_PPQN) / static_cast<float>(CMidiFile::getPulsesPerQuarterNote());
        ppLogWarn("MIDI Tempo %f  ppqn %d %d", static_cast<double>(m_midiTempo), CMidiFile::getPulsesPerQuarterNote(), tempo);
    }

    void setSpeed(float speed)
    {
        // limit the allowed speed
        if (speed > 2.0f)
            speed = 2.0f;
        if (speed < 0.1f)
            speed = 0.1f;
        m_userSpeed = speed;
    }
    float getSpeed() {return m_userSpeed;}

    qint64 mSecToTicks(qint64 mSec)
    {
        return static_cast<qint64>(static_cast<float>(mSec) * m_userSpeed * (100.0f * MICRO_SECOND) / m_midiTempo);
    }

    void insertPlayingTicks(qint64 ticks)
    {
        m_jumpAheadDelta -= ticks;
        if (m_jumpAheadDelta < CMidiFile::ppqnAdjust(-10)*SPEED_ADJUST_FACTOR)
            m_jumpAheadDelta = CMidiFile::ppqnAdjust(-10)*SPEED_ADJUST_FACTOR;
    }

    void removePlayingTicks(qint64 ticks)
    {
        if (m_cfg_maxJumpAhead != 0)
            m_jumpAheadDelta = ticks;
    }

    void clearPlayingTicks()
    {
        m_jumpAheadDelta = 0;
    }

    void adjustTempo(qint64 *ticks);

    static void enableFollowTempo(bool enable);

private:
    float m_userSpeed; // controls the speed of the piece playing
    float m_midiTempo; // controls the speed of the piece playing
    qint64 m_jumpAheadDelta;
    static int m_cfg_maxJumpAhead;
    static int m_cfg_followTempoAmount;
    CChord *m_savedWantedChord; // A copy of the wanted chord complete with both left and right parts

};

#endif  // __TEMPO_H__

