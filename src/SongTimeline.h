/*********************************************************************************/
/*!
@file           SongTimeline.h

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

#ifndef __SONG_TIMELINE_H__
#define __SONG_TIMELINE_H__

#include "MidiEvent.h"

#include <QtGlobal>

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

struct CTimelineEvent
{
    qint64 tick;
    int sourceDeltaTicks;
    std::size_t stableOrder;
    int track;
    int channel;
    CMidiEvent midi;
};

struct CNotePlaybackState
{
    CNotePlaybackState();

    std::uint16_t heldVoices;
    std::uint16_t sustainedVoices;
    int heldVelocity;
    int sustainedVelocity;

    bool active() const { return heldVoices != 0 || sustainedVoices != 0; }
};

struct CChannelPlaybackState
{
    CChannelPlaybackState();

    int program;
    int pitchBendLsb;
    int pitchBendMsb;
    int channelPressure;
    bool sustain;
    std::array<int, MAX_MIDI_NOTES> controls;
    std::array<CNotePlaybackState, MAX_MIDI_NOTES> notes;
};

struct CBarPosition
{
    CBarPosition();

    qint64 tick;
    int bar;
    int beat;
    qint64 tickInBeat;
    int numerator;
    int denominator;
};

struct CPlaybackState
{
    CPlaybackState();

    int tempoMicrosecondsPerQuarter;
    int timeSignatureNumerator;
    int timeSignatureDenominator;
    int keySignature;
    bool minorKey;
    CBarPosition barPosition;
    std::array<CChannelPlaybackState, MAX_MIDI_CHANNELS> channels;
};

struct CPlaybackCheckpoint
{
    qint64 tick;
    std::size_t eventIndex;
    CPlaybackState state;
};

struct CAttackGroup
{
    qint64 tick;
    int channel;
    std::vector<std::size_t> eventIndices;
};

class CSongTimeline
{
public:
    static const std::size_t MAX_EVENTS_BETWEEN_CHECKPOINTS = 4096;

    CSongTimeline();

    void clear();
    bool build(const std::vector<CMidiEvent> &mergedDeltaEvents, int ppqn);

    bool empty() const { return m_events.empty(); }
    std::size_t eventCount() const { return m_events.size(); }
    const std::vector<CTimelineEvent> &events() const { return m_events; }
    const CTimelineEvent *eventAt(std::size_t index) const;

    int pulsesPerQuarterNote() const { return m_ppqn; }
    qint64 durationTicks() const { return m_durationTicks; }
    qint64 eofTick() const { return m_eofTick; }
    bool hasExplicitEof() const { return m_hasExplicitEof; }

    // Index of the first event at or after tick.
    std::size_t eventIndexAt(qint64 tick) const;

    // State after all events before eventIndex, or strictly before tick.
    CPlaybackState stateAtEventIndex(std::size_t eventIndex) const;
    CPlaybackState stateBefore(qint64 tick) const;
    const CPlaybackCheckpoint *checkpointBefore(qint64 tick) const;
    const std::vector<CPlaybackCheckpoint> &checkpoints() const { return m_checkpoints; }

    CBarPosition barPositionAt(qint64 tick) const;
    qint64 tickAtBarPosition(double barPosition) const;

    const std::vector<CAttackGroup> &attackGroups(int channel) const;
    const CAttackGroup *attackGroupCrossed(qint64 fromTick, qint64 toTick,
                                           int channel) const;

private:
    struct CMeterSegment
    {
        qint64 tick;
        int firstBar;
        int numerator;
        int denominator;
    };

    static bool isStructuralEvent(const CMidiEvent &event);
    static void applyEvent(CPlaybackState &state, const CMidiEvent &event);
    static std::uint16_t incrementSaturated(std::uint16_t value);
    static void releaseHeldVoice(CChannelPlaybackState &channel, int pitch);

    void addCheckpoint(std::size_t eventIndex, const CPlaybackState &state);
    void buildMeterMap();
    CBarPosition positionInSegment(const CMeterSegment &segment, qint64 tick) const;
    std::size_t checkpointIndexForEvent(std::size_t eventIndex) const;

    int m_ppqn;
    qint64 m_durationTicks;
    qint64 m_eofTick;
    bool m_hasExplicitEof;
    std::vector<CTimelineEvent> m_events;
    std::vector<CPlaybackCheckpoint> m_checkpoints;
    std::array<std::vector<CAttackGroup>, MAX_MIDI_CHANNELS> m_attackGroups;
    std::vector<CMeterSegment> m_meterSegments;
};

#endif // __SONG_TIMELINE_H__
