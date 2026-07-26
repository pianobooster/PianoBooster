/*********************************************************************************/
/*!
@file           ScrubPreview.h

@brief          Fixed-size ownership and timing state for score scrub previews.

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

#ifndef __SCRUB_PREVIEW_H__
#define __SCRUB_PREVIEW_H__

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

class CScrubPreviewState
{
public:
    static const int MIDI_CHANNEL_COUNT = 16;
    static const int MIDI_PITCH_COUNT = 128;
    static const int FIRST_PIANO_PITCH = 21;
    static const int PIANO_KEY_COUNT = 88;
    static const std::size_t MAX_GROUP_NOTES = 32;
    static const std::int64_t RATE_LIMIT_MS = 50;
    static const std::int64_t GATE_MS = 120;

    enum class hand_t
    {
        Left,
        Right
    };

    enum class commandType_t
    {
        NoteOn,
        NoteOff
    };

    struct note_t
    {
        int channel;
        int pitch;
        int velocity;
        hand_t hand;

        note_t(int noteChannel, int notePitch, int noteVelocity, hand_t noteHand)
            : channel(noteChannel),
              pitch(notePitch),
              velocity(noteVelocity),
              hand(noteHand)
        {
        }
    };

    struct command_t
    {
        commandType_t type;
        int channel;
        int pitch;
        int velocity;
        hand_t hand;
    };

    typedef std::vector<command_t> command_list_t;
    typedef std::array<bool, PIANO_KEY_COUNT> handSnapshot_t;

    struct snapshot_t
    {
        handSnapshot_t left;
        handSnapshot_t right;
    };

    struct groupResult_t
    {
        command_list_t commands;
        std::size_t acceptedNotes;
        bool triggered;
        bool rateLimited;

        groupResult_t()
            : acceptedNotes(0),
              triggered(false),
              rateLimited(false)
        {
        }
    };

    groupResult_t startGroup(std::int64_t groupId,
                             std::int64_t nowMs,
                             const std::vector<note_t> &notes);
    command_list_t expire(std::int64_t nowMs);
    command_list_t clear();

    // Call this before forwarding an externally-owned NOTE_ON. Any conflicting
    // preview voices are released first and returned in dispatch order.
    command_list_t externalNoteOn(int channel, int pitch, int velocity = 1);
    void externalNoteOff(int channel, int pitch);
    void clearExternalChannel(int channel);
    void clearExternalNotes();

    snapshot_t snapshot() const;
    bool active() const;
    bool owns(int channel, int pitch) const;
    bool externalActive(int channel, int pitch) const;
    std::size_t ownedVoiceCount() const;

private:
    struct ownedVoice_t
    {
        int channel;
        int pitch;
        int velocity;
        hand_t hand;
        bool active;
    };

    struct pitchOwnership_t
    {
        std::uint8_t left;
        std::uint8_t right;
    };

    static bool validChannel(int channel);
    static bool validPitch(int pitch);
    static bool validNote(const note_t &note);
    static void incrementSaturating(std::uint16_t &value);

    command_list_t releaseOwnedVoices();
    command_list_t releaseOwnedVoices(int channel, int pitch);
    void addOwnedVoice(const note_t &note);
    void removeOwnedVoice(const ownedVoice_t &voice);
    bool rateLimitElapsed(std::int64_t nowMs) const;
    bool gateElapsed(std::int64_t nowMs) const;

    std::array<ownedVoice_t, MAX_GROUP_NOTES> m_ownedVoices{};
    std::array<std::array<pitchOwnership_t, MIDI_PITCH_COUNT>,
               MIDI_CHANNEL_COUNT> m_ownership{};
    std::array<std::array<std::uint16_t, MIDI_PITCH_COUNT>,
               MIDI_CHANNEL_COUNT> m_externalVoices{};
    std::size_t m_ownedVoiceCount{0};
    std::int64_t m_groupId{0};
    std::int64_t m_lastTriggerMs{0};
    std::int64_t m_gateStartMs{0};
    bool m_hasGroup{false};
    bool m_hasTriggered{false};
};

#endif // __SCRUB_PREVIEW_H__
