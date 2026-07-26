/*********************************************************************************/
/*!
@file           ScrubPreview.cpp

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

#include "ScrubPreview.h"

#include <limits>

bool CScrubPreviewState::validChannel(int channel)
{
    return channel >= 0 && channel < MIDI_CHANNEL_COUNT;
}

bool CScrubPreviewState::validPitch(int pitch)
{
    return pitch >= 0 && pitch < MIDI_PITCH_COUNT;
}

bool CScrubPreviewState::validNote(const note_t &note)
{
    return validChannel(note.channel) && validPitch(note.pitch) &&
           note.velocity > 0 && note.velocity <= 127 &&
           (note.hand == hand_t::Left || note.hand == hand_t::Right);
}

void CScrubPreviewState::incrementSaturating(std::uint16_t &value)
{
    if (value != std::numeric_limits<std::uint16_t>::max())
        ++value;
}

bool CScrubPreviewState::rateLimitElapsed(std::int64_t nowMs) const
{
    if (!m_hasTriggered)
        return true;

    if (nowMs < m_lastTriggerMs)
        return false;

    return nowMs - m_lastTriggerMs >= RATE_LIMIT_MS;
}

bool CScrubPreviewState::gateElapsed(std::int64_t nowMs) const
{
    if (m_ownedVoiceCount == 0)
        return false;

    // A backwards-moving monotonic clock is treated as expired so ownership
    // cannot become stuck indefinitely after a clock reset.
    if (nowMs < m_gateStartMs)
        return true;

    return nowMs - m_gateStartMs >= GATE_MS;
}

void CScrubPreviewState::addOwnedVoice(const note_t &note)
{
    if (m_ownedVoiceCount >= m_ownedVoices.size())
        return;

    ownedVoice_t &voice = m_ownedVoices[m_ownedVoiceCount];
    voice.channel = note.channel;
    voice.pitch = note.pitch;
    voice.velocity = note.velocity;
    voice.hand = note.hand;
    voice.active = true;
    ++m_ownedVoiceCount;

    pitchOwnership_t &ownership =
            m_ownership[static_cast<std::size_t>(note.channel)]
                       [static_cast<std::size_t>(note.pitch)];
    std::uint8_t &count =
            note.hand == hand_t::Right ? ownership.right : ownership.left;
    if (count != std::numeric_limits<std::uint8_t>::max())
        ++count;
}

void CScrubPreviewState::removeOwnedVoice(const ownedVoice_t &voice)
{
    pitchOwnership_t &ownership =
            m_ownership[static_cast<std::size_t>(voice.channel)]
                       [static_cast<std::size_t>(voice.pitch)];
    std::uint8_t &count =
            voice.hand == hand_t::Right ? ownership.right : ownership.left;
    if (count != 0)
        --count;
}

CScrubPreviewState::command_list_t CScrubPreviewState::releaseOwnedVoices()
{
    command_list_t commands;
    commands.reserve(m_ownedVoiceCount);

    for (std::size_t index = 0; index < m_ownedVoiceCount; ++index) {
        ownedVoice_t &voice = m_ownedVoices[index];
        if (!voice.active)
            continue;

        commands.push_back(command_t{commandType_t::NoteOff,
                                     voice.channel,
                                     voice.pitch,
                                     0,
                                     voice.hand});
        removeOwnedVoice(voice);
        voice.active = false;
    }

    m_ownedVoiceCount = 0;
    return commands;
}

CScrubPreviewState::command_list_t CScrubPreviewState::releaseOwnedVoices(
        int channel, int pitch)
{
    command_list_t commands;
    commands.reserve(m_ownedVoiceCount);

    std::size_t nextActiveIndex = 0;
    for (std::size_t index = 0; index < m_ownedVoiceCount; ++index) {
        ownedVoice_t &voice = m_ownedVoices[index];
        if (!voice.active)
            continue;

        if (voice.channel == channel && voice.pitch == pitch) {
            commands.push_back(command_t{commandType_t::NoteOff,
                                         voice.channel,
                                         voice.pitch,
                                         0,
                                         voice.hand});
            removeOwnedVoice(voice);
            continue;
        }

        if (nextActiveIndex != index)
            m_ownedVoices[nextActiveIndex] = voice;
        ++nextActiveIndex;
    }

    m_ownedVoiceCount = nextActiveIndex;
    return commands;
}

CScrubPreviewState::groupResult_t CScrubPreviewState::startGroup(
        std::int64_t groupId,
        std::int64_t nowMs,
        const std::vector<note_t> &notes)
{
    groupResult_t result;
    if (m_hasGroup && m_groupId == groupId)
        return result;

    if (!rateLimitElapsed(nowMs)) {
        result.rateLimited = true;
        return result;
    }

    result.commands = releaseOwnedVoices();
    m_groupId = groupId;
    m_hasGroup = true;
    m_hasTriggered = true;
    m_lastTriggerMs = nowMs;
    m_gateStartMs = nowMs;
    result.triggered = true;

    const std::size_t remaining =
            MAX_GROUP_NOTES - m_ownedVoiceCount;
    for (std::size_t index = 0;
         index < notes.size() && result.acceptedNotes < remaining;
         ++index) {
        const note_t &note = notes[index];
        if (!validNote(note) || externalActive(note.channel, note.pitch))
            continue;

        addOwnedVoice(note);
        result.commands.push_back(command_t{commandType_t::NoteOn,
                                             note.channel,
                                             note.pitch,
                                             note.velocity,
                                             note.hand});
        ++result.acceptedNotes;
    }

    return result;
}

CScrubPreviewState::command_list_t CScrubPreviewState::expire(
        std::int64_t nowMs)
{
    if (!gateElapsed(nowMs))
        return command_list_t();

    return releaseOwnedVoices();
}

CScrubPreviewState::command_list_t CScrubPreviewState::clear()
{
    command_list_t commands = releaseOwnedVoices();
    m_hasGroup = false;
    return commands;
}

CScrubPreviewState::command_list_t CScrubPreviewState::externalNoteOn(
        int channel, int pitch, int velocity)
{
    if (velocity == 0) {
        externalNoteOff(channel, pitch);
        return command_list_t();
    }

    if (!validChannel(channel) || !validPitch(pitch) || velocity < 0 ||
        velocity > 127)
        return command_list_t();

    command_list_t commands = releaseOwnedVoices(channel, pitch);
    incrementSaturating(
            m_externalVoices[static_cast<std::size_t>(channel)]
                            [static_cast<std::size_t>(pitch)]);
    return commands;
}

void CScrubPreviewState::externalNoteOff(int channel, int pitch)
{
    if (!validChannel(channel) || !validPitch(pitch))
        return;

    std::uint16_t &count =
            m_externalVoices[static_cast<std::size_t>(channel)]
                            [static_cast<std::size_t>(pitch)];
    if (count != 0)
        --count;
}

void CScrubPreviewState::clearExternalChannel(int channel)
{
    if (!validChannel(channel))
        return;

    m_externalVoices[static_cast<std::size_t>(channel)].fill(0);
}

void CScrubPreviewState::clearExternalNotes()
{
    for (std::size_t channel = 0; channel < m_externalVoices.size(); ++channel)
        m_externalVoices[channel].fill(0);
}

CScrubPreviewState::snapshot_t CScrubPreviewState::snapshot() const
{
    snapshot_t result{};

    for (std::size_t key = 0; key < result.left.size(); ++key) {
        const std::size_t pitch =
                static_cast<std::size_t>(FIRST_PIANO_PITCH) + key;
        for (std::size_t channel = 0; channel < m_ownership.size(); ++channel) {
            const pitchOwnership_t &ownership = m_ownership[channel][pitch];
            result.left[key] = result.left[key] || ownership.left != 0;
            result.right[key] = result.right[key] || ownership.right != 0;
        }
    }

    return result;
}

bool CScrubPreviewState::active() const
{
    return m_ownedVoiceCount != 0;
}

bool CScrubPreviewState::owns(int channel, int pitch) const
{
    if (!validChannel(channel) || !validPitch(pitch))
        return false;

    const pitchOwnership_t &ownership =
            m_ownership[static_cast<std::size_t>(channel)]
                       [static_cast<std::size_t>(pitch)];
    return ownership.left != 0 || ownership.right != 0;
}

bool CScrubPreviewState::externalActive(int channel, int pitch) const
{
    if (!validChannel(channel) || !validPitch(pitch))
        return false;

    return m_externalVoices[static_cast<std::size_t>(channel)]
                           [static_cast<std::size_t>(pitch)] != 0;
}

std::size_t CScrubPreviewState::ownedVoiceCount() const
{
    return m_ownedVoiceCount;
}
