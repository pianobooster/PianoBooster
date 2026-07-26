#include "SongTimeline.h"

#include <cstddef>
#include <cstdio>
#include <vector>

namespace
{
bool require(bool condition, const char *description)
{
    if (condition)
        return true;

    std::fprintf(stderr, "Failed: %s\n", description);
    return false;
}

CMidiEvent noteOn(int delta, int channel, int pitch, int velocity, int track = 0)
{
    CMidiEvent event;
    event.noteOnEvent(delta, channel, pitch, velocity);
    event.setTrack(track);
    return event;
}

CMidiEvent noteOff(int delta, int channel, int pitch, int track = 0)
{
    CMidiEvent event;
    event.noteOffEvent(delta, channel, pitch, 0);
    event.setTrack(track);
    return event;
}

CMidiEvent control(int delta, int channel, int controller, int value, int track = 0)
{
    CMidiEvent event;
    event.controlChangeEvent(delta, channel, controller, value);
    event.setTrack(track);
    return event;
}

CMidiEvent program(int delta, int channel, int value, int track = 0)
{
    CMidiEvent event;
    event.programChangeEvent(delta, channel, value);
    event.setTrack(track);
    return event;
}

CMidiEvent pitchBend(int delta, int channel, int lsb, int msb, int track = 0)
{
    CMidiEvent event;
    event.pitchBendEvent(delta, channel, lsb, msb);
    event.setTrack(track);
    return event;
}

CMidiEvent channelPressure(int delta, int channel, int value, int track = 0)
{
    CMidiEvent event;
    event.channelPressure(delta, channel, value);
    event.setTrack(track);
    return event;
}

CMidiEvent meta(int delta, int type, int data1, int data2, int track = 0)
{
    CMidiEvent event;
    event.metaEvent(delta, type, data1, data2);
    event.setTrack(track);
    return event;
}

CMidiEvent eof(int delta)
{
    CMidiEvent event;
    event.setType(MIDI_PB_EOF);
    event.setDeltaTime(delta);
    event.setTrack(0);
    return event;
}

bool testAbsoluteTicksStableOrderAndSearch()
{
    std::vector<CMidiEvent> source;
    source.push_back(noteOn(12, 0, 60, 100, 2));
    source.push_back(noteOn(0, 1, 64, 90, 1));
    source.push_back(noteOff(8, 0, 60, 2));
    source.push_back(eof(5));

    CSongTimeline timeline;
    if (!require(timeline.build(source, 96), "a valid merged stream builds"))
        return false;
    if (!require(timeline.eventCount() == source.size(), "every source event is retained once"))
        return false;
    if (!require(timeline.eventAt(0)->tick == 12 &&
                 timeline.eventAt(1)->tick == 12 &&
                 timeline.eventAt(2)->tick == 20 &&
                 timeline.eventAt(3)->tick == 25,
                 "relative deltas become absolute ticks"))
        return false;
    if (!require(timeline.eventAt(0)->stableOrder == 0 &&
                 timeline.eventAt(1)->stableOrder == 1 &&
                 timeline.eventAt(0)->track == 2 &&
                 timeline.eventAt(1)->track == 1,
                 "simultaneous events retain stream order and track"))
        return false;
    if (!require(timeline.eventAt(0)->sourceDeltaTicks == 12 &&
                 timeline.eventAt(1)->sourceDeltaTicks == 0,
                 "the source event copy retains its original delta"))
        return false;
    if (!require(timeline.eventIndexAt(-10) == 0 &&
                 timeline.eventIndexAt(0) == 0 &&
                 timeline.eventIndexAt(12) == 0 &&
                 timeline.eventIndexAt(13) == 2 &&
                 timeline.eventIndexAt(25) == 3 &&
                 timeline.eventIndexAt(26) == 4,
                 "binary search returns the first event at or after the target"))
        return false;
    if (!require(timeline.durationTicks() == 25 && timeline.eofTick() == 25 &&
                 timeline.hasExplicitEof(),
                 "duration and explicit EOF use absolute ticks"))
        return false;

    return require(timeline.eventAt(timeline.eventCount()) == nullptr,
                   "out-of-range event access is safe");
}

bool testContextAndStrictlyBefore()
{
    std::vector<CMidiEvent> source;
    source.push_back(meta(0, MIDI_PB_tempo, 600000, 0));
    source.push_back(meta(0, MIDI_PB_timeSignature, 3, 4));
    source.push_back(meta(0, MIDI_PB_keySignature, -3, 1));
    source.push_back(program(0, 1, 40));
    source.push_back(control(0, 1, MIDI_MAIN_VOLUME, 91));
    source.push_back(pitchBend(0, 1, 4, 70));
    source.push_back(channelPressure(0, 1, 45));
    source.push_back(noteOn(24, 1, 60, 80));

    CSongTimeline timeline;
    if (!require(timeline.build(source, 96), "context stream builds"))
        return false;

    const CPlaybackState atZero = timeline.stateBefore(0);
    if (!require(atZero.tempoMicrosecondsPerQuarter == 500000 &&
                 atZero.timeSignatureNumerator == 4 &&
                 atZero.keySignature == 0 &&
                 atZero.channels[1].program == GM_PIANO_PATCH,
                 "stateBefore excludes all events exactly at the target"))
        return false;

    const CPlaybackState afterZero = timeline.stateBefore(1);
    if (!require(afterZero.tempoMicrosecondsPerQuarter == 600000 &&
                 afterZero.timeSignatureNumerator == 3 &&
                 afterZero.timeSignatureDenominator == 4 &&
                 afterZero.keySignature == -3 && afterZero.minorKey,
                 "tempo, meter and key signature restore from prior events"))
        return false;
    if (!require(afterZero.channels[1].program == 40 &&
                 afterZero.channels[1].controls[MIDI_MAIN_VOLUME] == 91 &&
                 afterZero.channels[1].pitchBendLsb == 4 &&
                 afterZero.channels[1].pitchBendMsb == 70 &&
                 afterZero.channels[1].channelPressure == 45,
                 "program, controls, bend and pressure restore independently per channel"))
        return false;

    const CPlaybackState atAttack = timeline.stateBefore(24);
    return require(!atAttack.channels[1].notes[60].active(),
                   "an attack exactly at the target is not included in restored state");
}

bool testMeterMap()
{
    std::vector<CMidiEvent> source;
    source.push_back(meta(0, MIDI_PB_timeSignature, 4, 4));
    source.push_back(noteOn(384, 0, 60, 90));
    source.push_back(meta(0, MIDI_PB_timeSignature, 3, 4));
    source.push_back(eof(576));

    CSongTimeline timeline;
    if (!require(timeline.build(source, 96), "meter stream builds"))
        return false;

    const CBarPosition first = timeline.barPositionAt(383);
    const CBarPosition changed = timeline.barPositionAt(384);
    const CBarPosition later = timeline.barPositionAt(384 + 288 + 96 + 12);
    if (!require(first.bar == 0 && first.beat == 3 && first.tickInBeat == 95,
                 "the initial 4/4 segment maps tick to bar and beat"))
        return false;
    if (!require(changed.bar == 1 && changed.beat == 0 &&
                 changed.numerator == 3 && changed.denominator == 4,
                 "a meter change at a boundary begins the next bar"))
        return false;
    if (!require(later.bar == 2 && later.beat == 1 && later.tickInBeat == 12,
                 "bars after a 3/4 change use the new bar length"))
        return false;
    if (!require(timeline.tickAtBarPosition(-1.0) == 0 &&
                 timeline.tickAtBarPosition(0.25) == 96 &&
                 timeline.tickAtBarPosition(1.0) == 384 &&
                 timeline.tickAtBarPosition(1.5) == 576 &&
                 timeline.tickAtBarPosition(1.9) == 672 &&
                 timeline.tickAtBarPosition(2.25) == 768 &&
                 timeline.tickAtBarPosition(100.0) == timeline.eofTick(),
                 "bar-position lookup inverts the meter map, rounds meter gaps "
                 "forward and clamps to the timeline"))
        return false;

    qint64 previousTick = 0;
    for (int tenth = 0; tenth <= 30; ++tenth) {
        const qint64 tick =
                timeline.tickAtBarPosition(static_cast<double>(tenth) / 10.0);
        if (!require(tick >= previousTick,
                     "bar-position lookup remains monotonic across a meter change"))
            return false;
        previousTick = tick;
    }
    return true;
}

bool testVoicesSustainAndChannelControls()
{
    std::vector<CMidiEvent> source;
    source.push_back(noteOn(0, 2, 60, 90));
    source.push_back(noteOn(0, 2, 60, 70));
    source.push_back(control(10, 2, MIDI_SUSTAIN, 127));
    source.push_back(noteOff(0, 2, 60));
    source.push_back(noteOn(0, 2, 60, 0));
    source.push_back(noteOn(0, 3, 65, 88));
    source.push_back(control(10, 2, MIDI_SUSTAIN, 0));
    source.push_back(control(0, 3, MIDI_ALL_SOUND_OFF, 0));

    CSongTimeline timeline;
    if (!require(timeline.build(source, 96), "voice stream builds"))
        return false;

    const CPlaybackState sustained = timeline.stateBefore(11);
    const CNotePlaybackState &voice = sustained.channels[2].notes[60];
    if (!require(voice.heldVoices == 0 && voice.sustainedVoices == 2 &&
                 voice.sustainedVelocity == 70,
                 "repeated voices and velocity-zero note-off move under sustain"))
        return false;
    if (!require(sustained.channels[3].notes[65].heldVoices == 1,
                 "voice state is independent across channels"))
        return false;

    const CPlaybackState released = timeline.stateBefore(21);
    return require(!released.channels[2].notes[60].active() &&
                   !released.channels[3].notes[65].active(),
                   "sustain-off and all-sound-off clear their owned voices");
}

bool testAllNotesAndResetControllers()
{
    std::vector<CMidiEvent> source;
    source.push_back(control(0, 0, MIDI_SUSTAIN, 127));
    source.push_back(noteOn(0, 0, 60, 90));
    source.push_back(control(1, 0, MIDI_ALL_NOTES_OFF, 0));
    source.push_back(noteOn(1, 0, 64, 80));
    source.push_back(control(1, 0, MIDI_RESET_ALL_CONTROLLERS, 0));

    CSongTimeline timeline;
    if (!require(timeline.build(source, 96), "controller stream builds"))
        return false;

    const CPlaybackState allNotes = timeline.stateBefore(2);
    if (!require(allNotes.channels[0].notes[60].heldVoices == 0 &&
                 allNotes.channels[0].notes[60].sustainedVoices == 1,
                 "all-notes-off transfers held voices to active sustain"))
        return false;

    const CPlaybackState reset = timeline.stateBefore(4);
    return require(!reset.channels[0].sustain &&
                   !reset.channels[0].notes[60].active() &&
                   reset.channels[0].notes[64].heldVoices == 1 &&
                   reset.channels[0].controls[MIDI_SUSTAIN] == -1,
                   "reset-all-controllers drops sustain but preserves held voices");
}

bool testAttacks()
{
    std::vector<CMidiEvent> source;
    source.push_back(noteOn(10, 0, 60, 80));
    source.push_back(noteOn(0, 0, 64, 90));
    source.push_back(noteOn(5, 1, 67, 100));
    source.push_back(noteOn(5, 0, 69, 0));
    source.push_back(noteOn(5, 0, 72, 100));

    CSongTimeline timeline;
    if (!require(timeline.build(source, 96), "attack stream builds"))
        return false;
    const std::vector<CAttackGroup> &channelZero = timeline.attackGroups(0);
    if (!require(channelZero.size() == 2 &&
                 channelZero[0].tick == 10 &&
                 channelZero[0].eventIndices.size() == 2 &&
                 channelZero[1].tick == 25,
                 "positive-velocity attacks group by tick and channel"))
        return false;
    if (!require(timeline.attackGroups(1).size() == 1 &&
                 timeline.attackGroups(-1).empty(),
                 "attack indexes are channel-specific and bounds-safe"))
        return false;

    const CAttackGroup *forward = timeline.attackGroupCrossed(0, 24, 0);
    const CAttackGroup *fastForward = timeline.attackGroupCrossed(0, 30, 0);
    const CAttackGroup *backward = timeline.attackGroupCrossed(30, 9, 0);
    if (!require(forward && forward->tick == 10 &&
                 fastForward && fastForward->tick == 25 &&
                 backward && backward->tick == 10,
                 "crossing chooses the eligible attack nearest the drag target"))
        return false;
    return require(timeline.attackGroupCrossed(10, 24, 0) == nullptr &&
                   timeline.attackGroupCrossed(0, 30, 15) == nullptr,
                   "the anchor attack is not replayed and empty channels are safe");
}

bool testCheckpointBoundAndStateReplay()
{
    std::vector<CMidiEvent> source;
    const std::size_t eventCount = 9003;
    source.reserve(eventCount);
    for (std::size_t index = 0; index < eventCount; ++index)
        source.push_back(program(1, 0, static_cast<int>(index % 128)));

    CSongTimeline timeline;
    if (!require(timeline.build(source, 480), "a long stream builds"))
        return false;

    const std::vector<CPlaybackCheckpoint> &checkpoints = timeline.checkpoints();
    if (!require(!checkpoints.empty() &&
                 checkpoints.front().eventIndex == 0 &&
                 checkpoints.back().eventIndex == eventCount,
                 "checkpoints cover both timeline endpoints"))
        return false;
    for (std::size_t index = 1; index < checkpoints.size(); ++index) {
        if (!require(checkpoints[index].eventIndex -
                         checkpoints[index - 1].eventIndex <=
                     CSongTimeline::MAX_EVENTS_BETWEEN_CHECKPOINTS,
                     "no checkpoint gap exceeds the replay bound"))
            return false;
    }

    const CPlaybackCheckpoint *checkpoint = timeline.checkpointBefore(8500);
    if (!require(checkpoint && checkpoint->eventIndex <= timeline.eventIndexAt(8500) &&
                 timeline.eventIndexAt(8500) - checkpoint->eventIndex <=
                     CSongTimeline::MAX_EVENTS_BETWEEN_CHECKPOINTS,
                 "checkpoint lookup bounds replay before an arbitrary target"))
        return false;

    const CPlaybackState replayed = timeline.stateBefore(8501);
    return require(replayed.channels[0].program == static_cast<int>(8499 % 128),
                   "state reconstruction replays the bounded checkpoint suffix");
}

bool testEmptyAndInvalidInput()
{
    CSongTimeline timeline;
    const std::vector<CMidiEvent> empty;
    if (!require(timeline.build(empty, 96) && timeline.empty() &&
                 timeline.durationTicks() == 0 && timeline.eofTick() == 0 &&
                 timeline.eventIndexAt(50) == 0 &&
                 timeline.checkpointBefore(0) != nullptr,
                 "an empty MIDI stream has a valid zero-length timeline"))
        return false;

    std::vector<CMidiEvent> invalid;
    invalid.push_back(noteOn(-1, 0, 60, 90));
    return require(!timeline.build(invalid, 96) && timeline.empty() &&
                   !timeline.build(empty, 0),
                   "negative deltas and invalid PPQN fail without partial state");
}
}

int main()
{
    if (!testAbsoluteTicksStableOrderAndSearch() ||
        !testContextAndStrictlyBefore() ||
        !testMeterMap() ||
        !testVoicesSustainAndChannelControls() ||
        !testAllNotesAndResetControllers() ||
        !testAttacks() ||
        !testCheckpointBoundAndStateReplay() ||
        !testEmptyAndInvalidInput())
        return 1;

    return 0;
}
