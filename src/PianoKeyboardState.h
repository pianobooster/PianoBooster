/*********************************************************************************/
/*!
@file           PianoKeyboardState.h

@brief          Fixed-size active-note state for the synchronized piano keyboard.

    Copyright (c) 2026 PianoBooster contributors

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

#ifndef __PIANO_KEYBOARD_STATE_H__
#define __PIANO_KEYBOARD_STATE_H__

#include <array>
#include <cstdint>

class CPianoKeyboardState
{
public:
    static const int MIDI_CHANNEL_COUNT = 16;
    static const int MIDI_PITCH_COUNT = 128;
    static const int FIRST_PIANO_PITCH = 21;
    static const int PIANO_KEY_COUNT = 88;

    typedef std::array<bool, PIANO_KEY_COUNT> snapshot_t;

    void noteOn(int channel, int pitch, int velocity = 1);
    void noteOff(int channel, int pitch);
    void controlChange(int channel, int controller, int value);
    void clearChannel(int channel);
    void clearAll();
    snapshot_t snapshot(int channel = -1) const;

private:
    struct noteVoices_t
    {
        std::uint16_t held;
        std::uint16_t sustained;
    };

    static bool validChannel(int channel);
    static bool validPitch(int pitch);
    static void incrementSaturating(std::uint16_t &value);
    static void addSaturating(std::uint16_t &destination,
                              std::uint16_t source);

    void clearNotes(int channel);

    std::array<std::array<noteVoices_t, MIDI_PITCH_COUNT>, MIDI_CHANNEL_COUNT> m_notes{};
    std::array<bool, MIDI_CHANNEL_COUNT> m_sustain{};
};

#endif // __PIANO_KEYBOARD_STATE_H__
