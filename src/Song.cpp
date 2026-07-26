/*********************************************************************************/
/*!
@file           Song.cpp

@brief          xxxxx.

@author         L. J. Barman

    Copyright (c)   2008-2020, L. J. Barman and others, all rights reserved

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

#include "Song.h"
#include "Score.h"

#include <cmath>
#include <limits>
#include <vector>

void CSong::init2(CScore * scoreWin, CSettings* settings)
{

    CNote::reset();

    this->CConductor::init2(scoreWin, settings);

    setActiveHand(PB_PART_both);
    setPlayMode(PB_PLAY_MODE_followYou);
    setSpeed(1.0);
    setSkill(3);
}

void CSong::loadSong(const QString & filename)
{
    cancelScrub();
    CNote::reset();

    m_songTitle = filename;
    int index = m_songTitle.lastIndexOf("/");
    if (index >= 0)
        m_songTitle = m_songTitle.right( m_songTitle.length() - index - 1);

    QString fn = filename;
#ifdef _WIN32
     fn = fn.replace('/','\\');
#endif
    m_midiFile->setLogLevel(3);
    m_midiFile->openMidiFile(string(fn.toLocal8Bit().data()));
    ppLogInfo("Opening song %s",  fn.toLocal8Bit().data());
    transpose(0);
    midiFileInfo();
    m_midiFile->setLogLevel(99);
    playMusic(false);
    rewind();
    setPlayFromBar(0.0);
    setLoopingBars(0.0);
    setEventBits(EVENT_BITS_loadSong);
    if (!m_midiFile->getSongTitle().isEmpty())
        m_songTitle = m_midiFile->getSongTitle();

}

// read the file ahead to collect info about the song first
void CSong::midiFileInfo()
{
    m_trackList->reset(m_midiFile->numberOfTracks());
    setTimeSig(0,0);
    CStavePos::setKeySignature( NOT_USED, 0 );

    // Read the next events to find the active channels
    CMidiEvent event;
    std::vector<CMidiEvent> timelineEvents;
    while ( true )
    {
        event = m_midiFile->readMidiEvent();
        timelineEvents.push_back(event);
        m_trackList->examineMidiEvent(event);

        if (event.type() == MIDI_PB_timeSignature)
        {
            setTimeSig(event.data1(),event.data2());
        }

        if (event.type() == MIDI_PB_EOF)
            break;
    }
    if (!m_timeline.build(timelineEvents, CMidiFile::getPulsesPerQuarterNote()))
        m_timeline.clear();
}

void CSong::rewind()
{
    clearScrubPreview();
    this->CConductor::rewind();
    m_scoreWin->reset();
    reset();
    m_timelineCursor = 0;
    m_timelineFeedTick = 0;
    m_scoreTimelineCursor = 0;
    m_scoreTimelineFeedTick = 0;
    m_scoreReachedMidiEof = false;
    m_scrubbing = false;
    m_scrubAnchorTick = 0;
    m_scrubTargetTick = 0;
    m_scrubPreviewTick = 0;
    m_scrubDirection = 0;
    m_scrubVisualBaseTick = 0;
    m_pendingScrubRebase = false;
    m_pendingScrubRebaseTick = 0;
    forceScoreRedraw();
}

CMidiEvent CSong::nextTimelineEvent(std::size_t &cursor, qint64 &feedTick)
{
    CMidiEvent event;
    const CTimelineEvent *timelineEvent = m_timeline.eventAt(cursor);
    if (timelineEvent == nullptr) {
        event.setType(MIDI_PB_EOF);
        event.setDeltaTime(0);
        return event;
    }

    event = timelineEvent->midi;
    const qint64 delta = qMax<qint64>(0, timelineEvent->tick - feedTick);
    event.setDeltaTime(delta > std::numeric_limits<int>::max()
                           ? std::numeric_limits<int>::max()
                           : static_cast<int>(delta));
    feedTick = timelineEvent->tick;
    ++cursor;
    return event;
}

bool CSong::beginScrub()
{
    if (m_scrubbing || m_timeline.empty())
        return false;

    if (playingMusic())
        playMusic(false);
    else
        clearScrubPreview();

    m_scrubbing = true;
    m_scrubAnchorTick = qBound<qint64>(0, currentRawTick(), m_timeline.eofTick());
    m_scrubTargetTick = m_scrubAnchorTick;
    m_scrubPreviewTick = m_scrubAnchorTick;
    m_scrubDirection = 0;
    m_scrubVisualBaseTick = m_scrubAnchorTick;
    m_pendingScrubRebase = false;
    m_scoreWin->setScrubVisualOffset(0.0f);
    return true;
}

qint64 CSong::updateScrub(int horizontalPixels)
{
    if (!m_scrubbing)
        return currentRawTick();

    const float pixelsPerTick = qMax(0.001f, m_scoreWin->pixelsPerTick());
    const qint64 tickDelta = static_cast<qint64>(
            std::llround(static_cast<double>(horizontalPixels) /
                         static_cast<double>(pixelsPerTick)));
    qint64 minimumTick = 0;
    qint64 maximumTick = m_timeline.eofTick();
    if (getLoopingBars() > 0.0) {
        minimumTick = m_timeline.tickAtBarPosition(getPlayFromBar());
        maximumTick = m_timeline.tickAtBarPosition(
                getPlayFromBar() + getLoopingBars());
        if (maximumTick < minimumTick)
            maximumTick = minimumTick;
    }
    const qint64 target = qBound<qint64>(
            minimumTick, m_scrubAnchorTick - tickDelta, maximumTick);

    const int direction = target > m_scrubPreviewTick
            ? 1
            : (target < m_scrubPreviewTick ? -1 : 0);
    if (direction != 0 && m_scrubDirection != 0 &&
        direction != m_scrubDirection)
        clearScrubPreview();
    if (direction != 0)
        m_scrubDirection = direction;

    const whichPart_t activeHand = CNote::getActiveHand();
    const CAttackGroup *group = nullptr;
    for (int channel = 0; channel < MAX_MIDI_CHANNELS; ++channel) {
        if (channel == MIDI_DRUM_CHANNEL ||
            channel == Cfg::keyboardLightsChan)
            continue;

        const CAttackGroup *candidate = m_timeline.attackGroupCrossed(
                m_scrubPreviewTick, target, channel);
        if (candidate == nullptr)
            continue;

        bool hasDisplayedNote = false;
        for (std::size_t eventIndex : candidate->eventIndices) {
            const CTimelineEvent *timelineEvent = m_timeline.eventAt(eventIndex);
            if (timelineEvent != nullptr &&
                CNote::findHand(timelineEvent->midi, getActiveChannel(),
                                activeHand) != PB_PART_none) {
                hasDisplayedNote = true;
                break;
            }
        }
        if (!hasDisplayedNote)
            continue;

        if (group == nullptr ||
            (target > m_scrubPreviewTick && candidate->tick > group->tick) ||
            (target < m_scrubPreviewTick && candidate->tick < group->tick))
            group = candidate;
    }

    if (group != nullptr) {
        std::vector<CScrubPreviewState::note_t> notes;
        notes.reserve(CScrubPreviewState::MAX_GROUP_NOTES);
        std::size_t eventIndex = m_timeline.eventIndexAt(group->tick);
        while (const CTimelineEvent *timelineEvent =
                       m_timeline.eventAt(eventIndex)) {
            if (timelineEvent->tick != group->tick)
                break;
            ++eventIndex;
            if (timelineEvent->midi.type() != MIDI_NOTE_ON ||
                timelineEvent->midi.velocity() <= 0)
                continue;

            CMidiEvent event = timelineEvent->midi;
            if (event.channel() == MIDI_DRUM_CHANNEL ||
                event.channel() == Cfg::keyboardLightsChan)
                continue;
            const whichPart_t hand =
                    CNote::findHand(event, getActiveChannel(), activeHand);
            if (hand == PB_PART_none)
                continue;
            const int outputChannel = outputChannelForTrack(event.channel());
            const int pitch = event.note() + getTranspose();
            if (outputChannel < 0 || outputChannel == MIDI_DRUM_CHANNEL ||
                pitch < 0 || pitch >= MAX_MIDI_NOTES)
                continue;
            notes.push_back(CScrubPreviewState::note_t(
                    outputChannel,
                    pitch,
                    event.velocity(),
                        hand == PB_PART_right
                        ? CScrubPreviewState::hand_t::Right
                        : CScrubPreviewState::hand_t::Left));
            if (notes.size() >= CScrubPreviewState::MAX_GROUP_NOTES)
                break;
        }
        previewScrubGroup(group->tick, notes);
    }

    m_scrubPreviewTick = target;
    m_scrubTargetTick = target;
    setScrubPosition(target, m_timeline.barPositionAt(target));
    const float boundedOffset = static_cast<float>(
            static_cast<double>(m_scrubVisualBaseTick - target) *
            static_cast<double>(pixelsPerTick));
    m_scoreWin->setScrubVisualOffset(boundedOffset);
    const float rebaseDistance =
            (Cfg::staveEndX() - Cfg::scrollStartX()) * 0.45f;
    if (std::fabs(boundedOffset) >= rebaseDistance) {
        m_pendingScrubRebase = true;
        m_pendingScrubRebaseTick = target;
    }
    forceScoreRedraw();
    return target;
}

void CSong::seekTimeline(qint64 targetTick)
{
    targetTick = qBound<qint64>(0, targetTick, m_timeline.eofTick());
    clearScrubPreview();
    m_findChord.reset();
    m_timelineCursor = m_timeline.eventIndexAt(targetTick);
    m_timelineFeedTick = targetTick;
    m_reachedMidiEof = false;
    m_pendingScrubRebase = true;
    m_pendingScrubRebaseTick = targetTick;
    prepareTimelineSeek(targetTick, m_timeline.stateBefore(targetTick));
}

bool CSong::commitScrub()
{
    if (!m_scrubbing)
        return currentRawTick() >= m_timeline.eofTick();

    const qint64 target = m_scrubTargetTick;
    m_scrubbing = false;
    m_scoreWin->setScrubVisualOffset(0.0f);
    seekTimeline(target);
    return target >= m_timeline.eofTick();
}

void CSong::cancelScrub()
{
    if (!m_scrubbing)
        return;

    clearScrubPreview();
    m_scrubbing = false;
    m_scrubTargetTick = m_scrubAnchorTick;
    m_scrubPreviewTick = m_scrubAnchorTick;
    m_scrubDirection = 0;
    setScrubPosition(m_scrubAnchorTick,
                     m_timeline.barPositionAt(m_scrubAnchorTick));
    m_pendingScrubRebase = true;
    m_pendingScrubRebaseTick = m_scrubAnchorTick;
    if (m_scoreWin)
        m_scoreWin->setScrubVisualOffset(0.0f);
    forceScoreRedraw();
}

void CSong::applyPendingScrubRebase()
{
    if (!m_pendingScrubRebase || m_scoreWin == nullptr)
        return;

    // Once the drag transaction has finished playback may resume before the
    // next paint.  Rebase at the live playhead so those elapsed ticks are not
    // lost from the independently-fed score cursor.
    const qint64 requestedTick = m_scrubbing
            ? m_pendingScrubRebaseTick
            : currentRawTick();
    const qint64 targetTick = qBound<qint64>(
            0, requestedTick, m_timeline.eofTick());
    const qint64 preRollTicks =
            static_cast<qint64>(CMidiFile::getPulsesPerQuarterNote()) * 2;
    const qint64 aheadTicks =
            static_cast<qint64>(CMidiFile::getPulsesPerQuarterNote()) * 8;
    qint64 startTick = qMax<qint64>(0, targetTick - preRollTicks);
    const CBarPosition startPosition = m_timeline.barPositionAt(startTick);
    const qint64 beatLength = qMax<qint64>(
            1, (static_cast<qint64>(CMidiFile::getPulsesPerQuarterNote()) * 4) /
                       qMax(1, startPosition.denominator));
    const qint64 ticksIntoBar =
            static_cast<qint64>(startPosition.beat) * beatLength +
            startPosition.tickInBeat;
    startTick = qMax<qint64>(0, startTick - ticksIntoBar);
    const qint64 endTick = qMin<qint64>(
            m_timeline.eofTick(), targetTick + aheadTicks);

    m_scoreWin->reset();
    const CPlaybackState startState = m_timeline.stateBefore(startTick);
    CMidiEvent contextEvent;
    contextEvent.setTrack(0);
    contextEvent.metaEvent(0, MIDI_PB_timeSignature,
                           startState.timeSignatureNumerator,
                           startState.timeSignatureDenominator);
    m_scoreWin->midiEventInsert(contextEvent);
    contextEvent.metaEvent(0, MIDI_PB_keySignature,
                           startState.keySignature,
                           startState.minorKey ? 1 : 0);
    m_scoreWin->midiEventInsert(contextEvent);

    qint64 previousTick = startTick;
    qint64 preparedThrough = startTick;
    std::size_t index = m_timeline.eventIndexAt(startTick);
    while (const CTimelineEvent *timelineEvent = m_timeline.eventAt(index)) {
        if (timelineEvent->tick > endTick || m_scoreWin->midiEventSpace() <= 100)
            break;
        CMidiEvent event = timelineEvent->midi;
        const qint64 delta = qMax<qint64>(0, timelineEvent->tick - previousTick);
        event.setDeltaTime(delta > std::numeric_limits<int>::max()
                               ? std::numeric_limits<int>::max()
                               : static_cast<int>(delta));
        m_scoreWin->midiEventInsert(event);
        previousTick = timelineEvent->tick;
        preparedThrough = timelineEvent->tick;
        ++index;
        if (event.type() == MIDI_PB_EOF)
            break;
    }

    m_scoreWin->scrollDeltaTime(
            (targetTick - startTick) * SPEED_ADJUST_FACTOR);
    m_scoreWin->setScrubVisualOffset(0.0f);
    m_scrubVisualBaseTick = targetTick;
    m_scoreTimelineCursor = index;
    m_scoreTimelineFeedTick = preparedThrough;
    m_scoreReachedMidiEof =
            m_scoreTimelineCursor >= m_timeline.eventCount();
    m_pendingScrubRebase = false;
    forceScoreRedraw();
}

void CSong::setActiveHand(whichPart_t hand)
{
    if (hand < PB_PART_both)
        hand = PB_PART_both;
    if (hand > PB_PART_left)
        hand = PB_PART_left;

    this->CConductor::setActiveHand(hand);
    regenerateChordQueue();

    m_scoreWin->setDisplayHand(hand);
}

void CSong::setActiveChannel(int chan)
{
    this->CConductor::setActiveChannel(chan);
    m_scoreWin->setActiveChannel(chan);
    regenerateChordQueue();
}

void  CSong::setPlayMode(playMode_t mode)
{
    regenerateChordQueue();
    this->CConductor::setPlayMode(mode);
    forceScoreRedraw();
}

void CSong::regenerateChordQueue()
{
    int i;
    int length;
    CMidiEvent event;

    m_wantedChordQueue->clear();
    m_findChord.reset();

    length = m_songEventQueue->length();

    for (i = 0; i < length; i++)
    {
        event = m_songEventQueue->index(i);
        // Find the next chord
        if (m_findChord.findChord(event, getActiveChannel(), PB_PART_both) == true)
            chordEventInsert( m_findChord.getChord() ); // give the Conductor the chord event

    }
    resetWantedChord();
}

void CSong::refreshScroll()
{
    m_scoreWin->refreshScroll();
    forceScoreRedraw();
}

eventBits_t CSong::task(qint64 ticks)
{
    realTimeEngine(ticks);

    while (true)
    {
        while (!m_reachedMidiEof)
        {
            // Check that there is space
            if (midiEventSpace() <= 10 || chordEventSpace() <= 10)
                break;

            // Read the next events
            CMidiEvent event =
                    nextTimelineEvent(m_timelineCursor, m_timelineFeedTick);

            //ppLogTrace("Song event delta %d type 0x%x chan %d Note %d", event.deltaTime(), event.type(), event.channel(), event.note());

            // Find the next chord
            if (m_findChord.findChord(event, getActiveChannel(), PB_PART_both) == true)
                chordEventInsert( m_findChord.getChord() ); // give the Conductor the chord event

            // send the events to the other end
            midiEventInsert(event);

            if (event.type() == MIDI_PB_EOF)
            {
                m_reachedMidiEof = true;
                break;
            }
        }

        while (!m_pendingScrubRebase && !m_scoreReachedMidiEof) {
            if (m_scoreWin->midiEventSpace() <= 100)
                break;
            CMidiEvent event = nextTimelineEvent(
                    m_scoreTimelineCursor, m_scoreTimelineFeedTick);
            m_scoreWin->midiEventInsert(event);
            if (event.type() == MIDI_PB_EOF)
                m_scoreReachedMidiEof = true;
        }

        // carry on with the data until we reach the bar we want
        if (seekingBarNumber() && m_reachedMidiEof == false && playingMusic())
        {
            realTimeEngine(0);
            m_scoreWin->drawScrollingSymbols(false); // don't display any thing just  remove from the queue
        }
        else
            break;
    }

    eventBits_t eventBits = m_realTimeEventBits;
    m_realTimeEventBits = 0;
    return eventBits;
}

static const struct pcNote_s
{
    int key;
    int note;
} pcNoteLookup[] =
{
    { 'a', PC_KEY_LOWEST_NOTE },
    { 'z', 59 }, // B
    { 'x', 60 }, // Middle C
    { 'd', 61 },
    { 'c', 62 }, // D
    { 'f', 63 },
    { 'v', 64 }, // E
    { 'b', 65 }, // F
    { 'h', 66 },
    { 'n', 67 }, // G
    { 'j', 68 },
    { 'm', 69 }, // A
    { 'k', 70 },
    { ',', 71 }, // B
    { '.', 72 }, // C
    { ';', 73 },
    { '/', 74 }, // D
    { '\'', PC_KEY_HIGHEST_NOTE },
};

// Fakes a midi piano keyboard using the PC keyboard
bool CSong::pcKeyPress(int key, bool down)
{
    int i;
    int j;
    CMidiEvent midi;
    const int cfg_pcKeyVolume = 64;
    const int cfg_pcKeyChannel = 1-1;

    if (key == 't') // the tab key on the PC fakes good notes
    {
        if (down)
            m_fakeChord = getWantedChord();
        for (i = 0; i < m_fakeChord.length(); i++)
        {
            if (down)
                midi.noteOnEvent(0, cfg_pcKeyChannel, m_fakeChord.getNote(i).pitch() + getTranspose(), cfg_pcKeyVolume);
            else
                midi.noteOffEvent(0, cfg_pcKeyChannel, m_fakeChord.getNote(i).pitch() + getTranspose(), cfg_pcKeyVolume);
            expandPianistInput(midi);
        }
        return true;
    }

    for (j = 0; j < arraySize(pcNoteLookup); j++)
    {
        if ( key==pcNoteLookup[j].key)
        {
            if (down)
                midi.noteOnEvent(0, cfg_pcKeyChannel, pcNoteLookup[j].note, cfg_pcKeyVolume);
            else
                midi.noteOffEvent(0, cfg_pcKeyChannel, pcNoteLookup[j].note, cfg_pcKeyVolume);

            expandPianistInput(midi);
            return true;
        }
    }
    //printf("pcKeyPress %d %d\n", m_pcNote, key);
    return false;
}
