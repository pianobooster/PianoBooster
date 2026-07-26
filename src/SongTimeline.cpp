/*********************************************************************************/
/*!
@file           SongTimeline.cpp

@brief          Immutable, seekable view of a merged MIDI event stream.

@author         PianoBooster contributors

    Copyright (c) 2026, PianoBooster contributors

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

#include "SongTimeline.h"

#include <algorithm>
#include <limits>

namespace
{
const int DEFAULT_TEMPO_MICROSECONDS = 500000;

bool validChannel(int channel)
{
    return channel >= 0 && channel < MAX_MIDI_CHANNELS;
}

bool validPitch(int pitch)
{
    return pitch >= 0 && pitch < MAX_MIDI_NOTES;
}

qint64 nonNegativeTick(qint64 tick)
{
    return tick < 0 ? 0 : tick;
}
}

CNotePlaybackState::CNotePlaybackState()
    : heldVoices(0),
      sustainedVoices(0),
      heldVelocity(0),
      sustainedVelocity(0)
{
}

CChannelPlaybackState::CChannelPlaybackState()
    : program(GM_PIANO_PATCH),
      pitchBendLsb(0),
      pitchBendMsb(64),
      channelPressure(0),
      sustain(false)
{
    controls.fill(-1);
}

CBarPosition::CBarPosition()
    : tick(0),
      bar(0),
      beat(0),
      tickInBeat(0),
      numerator(4),
      denominator(4)
{
}

CPlaybackState::CPlaybackState()
    : tempoMicrosecondsPerQuarter(DEFAULT_TEMPO_MICROSECONDS),
      timeSignatureNumerator(4),
      timeSignatureDenominator(4),
      keySignature(0),
      minorKey(false)
{
}

CSongTimeline::CSongTimeline()
{
    clear();
}

void CSongTimeline::clear()
{
    m_ppqn = 96;
    m_durationTicks = 0;
    m_eofTick = 0;
    m_hasExplicitEof = false;
    std::vector<CTimelineEvent>().swap(m_events);
    std::vector<CPlaybackCheckpoint>().swap(m_checkpoints);
    std::vector<CMeterSegment>().swap(m_meterSegments);
    for (std::size_t channel = 0; channel < m_attackGroups.size(); ++channel)
        std::vector<CAttackGroup>().swap(m_attackGroups[channel]);
}

bool CSongTimeline::build(const std::vector<CMidiEvent> &mergedDeltaEvents, int ppqn)
{
    clear();
    if (ppqn <= 0)
        return false;

    m_ppqn = ppqn;
    m_events.reserve(mergedDeltaEvents.size());

    qint64 absoluteTick = 0;
    for (std::size_t index = 0; index < mergedDeltaEvents.size(); ++index) {
        // CMidiEvent has legacy non-const accessors for these two fields.
        CMidiEvent midi = mergedDeltaEvents[index];
        const int delta = midi.deltaTime();
        if (delta < 0) {
            clear();
            return false;
        }
        if (absoluteTick > std::numeric_limits<qint64>::max() - delta) {
            clear();
            return false;
        }
        absoluteTick += delta;
        if (midi.type() == MIDI_PB_EOF || midi.type() == MIDI_NONE)
            midi.setTrack(-1);

        CTimelineEvent event;
        event.tick = absoluteTick;
        event.sourceDeltaTicks = delta;
        event.stableOrder = index;
        event.track = midi.track();
        event.channel = midi.channel();
        event.midi = midi;
        m_events.push_back(event);

        if (midi.type() == MIDI_PB_EOF) {
            m_hasExplicitEof = true;
            m_eofTick = absoluteTick;
        }

        if (midi.type() == MIDI_NOTE_ON && midi.velocity() > 0 &&
            validChannel(midi.channel()) && validPitch(midi.note())) {
            std::vector<CAttackGroup> &groups =
                m_attackGroups[static_cast<std::size_t>(midi.channel())];
            if (groups.empty() || groups.back().tick != absoluteTick) {
                CAttackGroup group;
                group.tick = absoluteTick;
                group.channel = midi.channel();
                groups.push_back(group);
            }
            groups.back().eventIndices.push_back(index);
        }
    }

    m_durationTicks = absoluteTick;
    if (!m_hasExplicitEof)
        m_eofTick = m_durationTicks;

    buildMeterMap();

    CPlaybackState state;
    state.barPosition = barPositionAt(0);
    addCheckpoint(0, state);
    for (std::size_t index = 0; index < m_events.size(); ++index) {
        applyEvent(state, m_events[index].midi);
        const std::size_t nextIndex = index + 1;
        if (nextIndex % MAX_EVENTS_BETWEEN_CHECKPOINTS == 0 ||
            isStructuralEvent(m_events[index].midi))
            addCheckpoint(nextIndex, state);
    }
    addCheckpoint(m_events.size(), state);
    return true;
}

const CTimelineEvent *CSongTimeline::eventAt(std::size_t index) const
{
    return index < m_events.size() ? &m_events[index] : nullptr;
}

std::size_t CSongTimeline::eventIndexAt(qint64 tick) const
{
    tick = nonNegativeTick(tick);
    const std::vector<CTimelineEvent>::const_iterator found =
        std::lower_bound(m_events.begin(), m_events.end(), tick,
                         [](const CTimelineEvent &event, qint64 value) {
                             return event.tick < value;
                         });
    return static_cast<std::size_t>(found - m_events.begin());
}

CPlaybackState CSongTimeline::stateAtEventIndex(std::size_t eventIndex) const
{
    if (eventIndex > m_events.size())
        eventIndex = m_events.size();

    CPlaybackState state;
    std::size_t replayIndex = 0;
    if (!m_checkpoints.empty()) {
        const std::size_t checkpointIndex = checkpointIndexForEvent(eventIndex);
        state = m_checkpoints[checkpointIndex].state;
        replayIndex = m_checkpoints[checkpointIndex].eventIndex;
    }

    while (replayIndex < eventIndex) {
        applyEvent(state, m_events[replayIndex].midi);
        ++replayIndex;
    }

    const qint64 tick = eventIndex < m_events.size()
        ? m_events[eventIndex].tick
        : m_durationTicks;
    state.barPosition = barPositionAt(tick);
    return state;
}

CPlaybackState CSongTimeline::stateBefore(qint64 tick) const
{
    tick = nonNegativeTick(tick);
    CPlaybackState state = stateAtEventIndex(eventIndexAt(tick));
    state.barPosition = barPositionAt(tick);
    return state;
}

const CPlaybackCheckpoint *CSongTimeline::checkpointBefore(qint64 tick) const
{
    if (m_checkpoints.empty())
        return nullptr;

    const std::size_t targetIndex = eventIndexAt(tick);
    return &m_checkpoints[checkpointIndexForEvent(targetIndex)];
}

CBarPosition CSongTimeline::barPositionAt(qint64 tick) const
{
    tick = nonNegativeTick(tick);
    if (m_meterSegments.empty()) {
        CMeterSegment fallback;
        fallback.tick = 0;
        fallback.firstBar = 0;
        fallback.numerator = 4;
        fallback.denominator = 4;
        return positionInSegment(fallback, tick);
    }

    const std::vector<CMeterSegment>::const_iterator upper =
        std::upper_bound(m_meterSegments.begin(), m_meterSegments.end(), tick,
                         [](qint64 value, const CMeterSegment &segment) {
                             return value < segment.tick;
                         });
    const CMeterSegment &segment =
        upper == m_meterSegments.begin() ? m_meterSegments.front() : *(upper - 1);
    return positionInSegment(segment, tick);
}

qint64 CSongTimeline::tickAtBarPosition(double barPosition) const
{
    if (barPosition <= 0.0 || m_eofTick <= 0)
        return 0;

    const auto valueAtTick = [this](qint64 tick) {
        const CBarPosition position = barPositionAt(tick);
        const qint64 beatLength =
                (static_cast<qint64>(m_ppqn) * 4) /
                qMax(1, position.denominator);
        return static_cast<double>(position.bar) +
               static_cast<double>(position.beat) /
                       static_cast<double>(position.denominator) +
               static_cast<double>(position.tickInBeat) /
                       static_cast<double>(
                               qMax<qint64>(1, beatLength) *
                               position.denominator);
    };

    if (barPosition >= valueAtTick(m_eofTick))
        return m_eofTick;

    qint64 low = 0;
    qint64 high = m_eofTick;
    while (low < high) {
        const qint64 middle = low + (high - low) / 2;
        if (valueAtTick(middle) < barPosition)
            low = middle + 1;
        else
            high = middle;
    }
    return low;
}

const std::vector<CAttackGroup> &CSongTimeline::attackGroups(int channel) const
{
    static const std::vector<CAttackGroup> emptyGroups;
    if (!validChannel(channel))
        return emptyGroups;
    return m_attackGroups[static_cast<std::size_t>(channel)];
}

const CAttackGroup *CSongTimeline::attackGroupCrossed(qint64 fromTick, qint64 toTick,
                                                       int channel) const
{
    const std::vector<CAttackGroup> &groups = attackGroups(channel);
    if (groups.empty() || fromTick == toTick)
        return nullptr;

    if (toTick > fromTick) {
        // Select the crossed group closest to the final target.
        const std::vector<CAttackGroup>::const_iterator upper =
            std::upper_bound(groups.begin(), groups.end(), toTick,
                             [](qint64 value, const CAttackGroup &group) {
                                 return value < group.tick;
                             });
        if (upper == groups.begin())
            return nullptr;
        const CAttackGroup &candidate = *(upper - 1);
        return candidate.tick > fromTick ? &candidate : nullptr;
    }

    const std::vector<CAttackGroup>::const_iterator lower =
        std::lower_bound(groups.begin(), groups.end(), toTick,
                         [](const CAttackGroup &group, qint64 value) {
                             return group.tick < value;
                         });
    if (lower == groups.end() || lower->tick >= fromTick)
        return nullptr;
    return &*lower;
}

bool CSongTimeline::isStructuralEvent(const CMidiEvent &event)
{
    return event.type() == MIDI_PB_tempo ||
           event.type() == MIDI_PB_timeSignature ||
           event.type() == MIDI_PB_keySignature;
}

void CSongTimeline::applyEvent(CPlaybackState &state, const CMidiEvent &event)
{
    const int type = event.type();
    if (type == MIDI_PB_tempo) {
        if (event.data1() > 0)
            state.tempoMicrosecondsPerQuarter = event.data1();
        return;
    }
    if (type == MIDI_PB_timeSignature) {
        if (event.data1() > 0 && event.data2() > 0) {
            state.timeSignatureNumerator = event.data1();
            state.timeSignatureDenominator = event.data2();
        }
        return;
    }
    if (type == MIDI_PB_keySignature) {
        state.keySignature = event.data1();
        state.minorKey = event.data2() != 0;
        return;
    }

    const int channelNumber = event.channel();
    if (!validChannel(channelNumber))
        return;
    CChannelPlaybackState &channel =
        state.channels[static_cast<std::size_t>(channelNumber)];

    if (type == MIDI_PROGRAM_CHANGE) {
        channel.program = event.programme();
    } else if (type == MIDI_PITCH_BEND) {
        channel.pitchBendLsb = event.data1();
        channel.pitchBendMsb = event.data2();
    } else if (type == MIDI_CHANNEL_PRESSURE) {
        channel.channelPressure = event.data1();
    } else if (type == MIDI_CONTROL_CHANGE) {
        const int controller = event.data1();
        const int value = event.data2();
        if (controller >= 0 && controller < MAX_MIDI_NOTES)
            channel.controls[static_cast<std::size_t>(controller)] = value;

        if (controller == MIDI_SUSTAIN) {
            const bool sustain = value >= 64;
            if (channel.sustain && !sustain) {
                for (std::size_t pitch = 0; pitch < channel.notes.size(); ++pitch) {
                    channel.notes[pitch].sustainedVoices = 0;
                    channel.notes[pitch].sustainedVelocity = 0;
                }
            }
            channel.sustain = sustain;
        } else if (controller == MIDI_ALL_SOUND_OFF) {
            for (std::size_t pitch = 0; pitch < channel.notes.size(); ++pitch)
                channel.notes[pitch] = CNotePlaybackState();
        } else if (controller == MIDI_ALL_NOTES_OFF) {
            for (std::size_t pitch = 0; pitch < channel.notes.size(); ++pitch) {
                CNotePlaybackState &note = channel.notes[pitch];
                if (channel.sustain && note.heldVoices != 0) {
                    const unsigned int total =
                        static_cast<unsigned int>(note.sustainedVoices) + note.heldVoices;
                    note.sustainedVoices = total > std::numeric_limits<std::uint16_t>::max()
                        ? std::numeric_limits<std::uint16_t>::max()
                        : static_cast<std::uint16_t>(total);
                    note.sustainedVelocity = note.heldVelocity;
                }
                note.heldVoices = 0;
                note.heldVelocity = 0;
                if (!channel.sustain) {
                    note.sustainedVoices = 0;
                    note.sustainedVelocity = 0;
                }
            }
        } else if (controller == MIDI_RESET_ALL_CONTROLLERS) {
            channel.controls.fill(-1);
            channel.sustain = false;
            for (std::size_t pitch = 0; pitch < channel.notes.size(); ++pitch) {
                channel.notes[pitch].sustainedVoices = 0;
                channel.notes[pitch].sustainedVelocity = 0;
            }
        }
        return;
    }

    const int pitch = event.note();
    if (!validPitch(pitch))
        return;
    CNotePlaybackState &note = channel.notes[static_cast<std::size_t>(pitch)];
    if (type == MIDI_NOTE_ON && event.velocity() > 0) {
        note.heldVoices = incrementSaturated(note.heldVoices);
        note.heldVelocity = event.velocity();
    } else if (type == MIDI_NOTE_OFF ||
               (type == MIDI_NOTE_ON && event.velocity() == 0)) {
        releaseHeldVoice(channel, pitch);
    }
}

std::uint16_t CSongTimeline::incrementSaturated(std::uint16_t value)
{
    return value == std::numeric_limits<std::uint16_t>::max()
        ? value
        : static_cast<std::uint16_t>(value + 1);
}

void CSongTimeline::releaseHeldVoice(CChannelPlaybackState &channel, int pitch)
{
    CNotePlaybackState &note = channel.notes[static_cast<std::size_t>(pitch)];
    if (note.heldVoices == 0)
        return;

    --note.heldVoices;
    if (channel.sustain) {
        note.sustainedVoices = incrementSaturated(note.sustainedVoices);
        note.sustainedVelocity = note.heldVelocity;
    }
    if (note.heldVoices == 0)
        note.heldVelocity = 0;
}

void CSongTimeline::addCheckpoint(std::size_t eventIndex, const CPlaybackState &state)
{
    if (!m_checkpoints.empty() && m_checkpoints.back().eventIndex == eventIndex) {
        m_checkpoints.back().state = state;
        return;
    }

    CPlaybackCheckpoint checkpoint;
    checkpoint.eventIndex = eventIndex;
    checkpoint.tick = eventIndex < m_events.size()
        ? m_events[eventIndex].tick
        : m_durationTicks;
    checkpoint.state = state;
    checkpoint.state.barPosition = barPositionAt(checkpoint.tick);
    m_checkpoints.push_back(checkpoint);
}

void CSongTimeline::buildMeterMap()
{
    CMeterSegment initial;
    initial.tick = 0;
    initial.firstBar = 0;
    initial.numerator = 4;
    initial.denominator = 4;
    m_meterSegments.push_back(initial);

    for (std::size_t index = 0; index < m_events.size(); ++index) {
        const CTimelineEvent &timelineEvent = m_events[index];
        const CMidiEvent &midi = timelineEvent.midi;
        if (midi.type() != MIDI_PB_timeSignature ||
            midi.data1() <= 0 || midi.data2() <= 0)
            continue;

        CMeterSegment segment;
        segment.tick = timelineEvent.tick;
        segment.numerator = midi.data1();
        segment.denominator = midi.data2();

        if (segment.tick == m_meterSegments.back().tick) {
            segment.firstBar = m_meterSegments.back().firstBar;
            m_meterSegments.back() = segment;
            continue;
        }

        const CBarPosition previous = positionInSegment(m_meterSegments.back(), segment.tick);
        segment.firstBar = previous.bar;
        if (previous.beat != 0 || previous.tickInBeat != 0)
            ++segment.firstBar;
        m_meterSegments.push_back(segment);
    }
}

CBarPosition CSongTimeline::positionInSegment(const CMeterSegment &segment, qint64 tick) const
{
    CBarPosition position;
    position.tick = nonNegativeTick(tick);
    position.numerator = segment.numerator > 0 ? segment.numerator : 4;
    position.denominator = segment.denominator > 0 ? segment.denominator : 4;

    const qint64 beatLength =
        std::max<qint64>(1, (static_cast<qint64>(m_ppqn) * 4) / position.denominator);
    const qint64 barLength = beatLength * position.numerator;
    const qint64 elapsed = position.tick > segment.tick ? position.tick - segment.tick : 0;
    position.bar = segment.firstBar + static_cast<int>(elapsed / barLength);
    const qint64 tickInBar = elapsed % barLength;
    position.beat = static_cast<int>(tickInBar / beatLength);
    position.tickInBeat = tickInBar % beatLength;
    return position;
}

std::size_t CSongTimeline::checkpointIndexForEvent(std::size_t eventIndex) const
{
    const std::vector<CPlaybackCheckpoint>::const_iterator upper =
        std::upper_bound(m_checkpoints.begin(), m_checkpoints.end(), eventIndex,
                         [](std::size_t value, const CPlaybackCheckpoint &checkpoint) {
                             return value < checkpoint.eventIndex;
                         });
    return upper == m_checkpoints.begin()
        ? 0
        : static_cast<std::size_t>((upper - m_checkpoints.begin()) - 1);
}
