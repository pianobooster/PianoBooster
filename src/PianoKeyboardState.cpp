/*********************************************************************************/
/*!
@file           PianoKeyboardState.cpp

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

#include "PianoKeyboardState.h"

#include <limits>

namespace
{
const int MIDI_SUSTAIN = 64;
const int MIDI_ALL_SOUND_OFF = 120;
const int MIDI_RESET_ALL_CONTROLLERS = 121;
const int MIDI_ALL_NOTES_OFF = 123;
const int MIDI_SUSTAIN_ON_THRESHOLD = 64;
}

bool CPianoKeyboardState::validChannel(int channel)
{
    return channel >= 0 && channel < MIDI_CHANNEL_COUNT;
}

bool CPianoKeyboardState::validPitch(int pitch)
{
    return pitch >= 0 && pitch < MIDI_PITCH_COUNT;
}

void CPianoKeyboardState::incrementSaturating(std::uint16_t &value)
{
    if (value != std::numeric_limits<std::uint16_t>::max())
        ++value;
}

void CPianoKeyboardState::addSaturating(std::uint16_t &destination,
                                        std::uint16_t source)
{
    const std::uint16_t maximum = std::numeric_limits<std::uint16_t>::max();
    if (source > maximum - destination)
        destination = maximum;
    else
        destination = static_cast<std::uint16_t>(destination + source);
}

void CPianoKeyboardState::noteOn(int channel, int pitch, int velocity)
{
    if (!validChannel(channel) || !validPitch(pitch))
        return;

    if (velocity == 0) {
        noteOff(channel, pitch);
        return;
    }

    const std::size_t channelIndex = static_cast<std::size_t>(channel);
    const std::size_t pitchIndex = static_cast<std::size_t>(pitch);
    incrementSaturating(m_notes[channelIndex][pitchIndex].held);
}

void CPianoKeyboardState::noteOff(int channel, int pitch)
{
    if (!validChannel(channel) || !validPitch(pitch))
        return;

    const std::size_t channelIndex = static_cast<std::size_t>(channel);
    const std::size_t pitchIndex = static_cast<std::size_t>(pitch);
    noteVoices_t &voices = m_notes[channelIndex][pitchIndex];
    if (voices.held == 0)
        return;

    --voices.held;
    if (m_sustain[channelIndex])
        incrementSaturating(voices.sustained);
}

void CPianoKeyboardState::controlChange(int channel, int controller, int value)
{
    if (!validChannel(channel))
        return;

    const std::size_t channelIndex = static_cast<std::size_t>(channel);
    switch (controller) {
    case MIDI_SUSTAIN:
        if (value >= MIDI_SUSTAIN_ON_THRESHOLD) {
            m_sustain[channelIndex] = true;
        } else {
            m_sustain[channelIndex] = false;
            for (std::size_t pitch = 0; pitch < m_notes[channelIndex].size(); ++pitch)
                m_notes[channelIndex][pitch].sustained = 0;
        }
        break;

    case MIDI_ALL_SOUND_OFF:
        clearNotes(channel);
        break;

    case MIDI_RESET_ALL_CONTROLLERS:
        m_sustain[channelIndex] = false;
        for (std::size_t pitch = 0; pitch < m_notes[channelIndex].size(); ++pitch)
            m_notes[channelIndex][pitch].sustained = 0;
        break;

    case MIDI_ALL_NOTES_OFF:
        for (std::size_t pitch = 0; pitch < m_notes[channelIndex].size(); ++pitch) {
            noteVoices_t &voices = m_notes[channelIndex][pitch];
            if (m_sustain[channelIndex])
                addSaturating(voices.sustained, voices.held);
            voices.held = 0;
        }
        break;

    default:
        break;
    }
}

void CPianoKeyboardState::clearNotes(int channel)
{
    const std::size_t channelIndex = static_cast<std::size_t>(channel);
    for (std::size_t pitch = 0; pitch < m_notes[channelIndex].size(); ++pitch) {
        m_notes[channelIndex][pitch].held = 0;
        m_notes[channelIndex][pitch].sustained = 0;
    }
}

void CPianoKeyboardState::clearChannel(int channel)
{
    if (!validChannel(channel))
        return;

    clearNotes(channel);
    m_sustain[static_cast<std::size_t>(channel)] = false;
}

void CPianoKeyboardState::clearAll()
{
    for (int channel = 0; channel < MIDI_CHANNEL_COUNT; ++channel)
        clearChannel(channel);
}

CPianoKeyboardState::snapshot_t CPianoKeyboardState::snapshot(int selectedChannel) const
{
    snapshot_t keys{};

    if (selectedChannel < -1)
        return keys;

    if (selectedChannel >= 0)
    {
        if (!validChannel(selectedChannel))
            return keys;

        const std::size_t channel = static_cast<std::size_t>(selectedChannel);
        for (std::size_t key = 0; key < keys.size(); ++key) {
            const std::size_t pitch = static_cast<std::size_t>(FIRST_PIANO_PITCH) + key;
            const noteVoices_t &voices = m_notes[channel][pitch];
            keys[key] = voices.held != 0 || voices.sustained != 0;
        }
        return keys;
    }

    for (std::size_t key = 0; key < keys.size(); ++key) {
        const std::size_t pitch = static_cast<std::size_t>(FIRST_PIANO_PITCH) + key;
        for (std::size_t channel = 0; channel < m_notes.size(); ++channel) {
            const noteVoices_t &voices = m_notes[channel][pitch];
            if (voices.held != 0 || voices.sustained != 0) {
                keys[key] = true;
                break;
            }
        }
    }

    return keys;
}
