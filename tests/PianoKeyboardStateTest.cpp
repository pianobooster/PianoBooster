#include "PianoKeyboardState.h"

#include <cstddef>
#include <cstdio>

namespace
{
bool keyIsActive(const CPianoKeyboardState &state, int pitch)
{
    const CPianoKeyboardState::snapshot_t keys = state.snapshot();
    return keys[static_cast<std::size_t>(pitch - CPianoKeyboardState::FIRST_PIANO_PITCH)];
}

bool noKeysAreActive(const CPianoKeyboardState::snapshot_t &keys)
{
    for (std::size_t key = 0; key < keys.size(); ++key) {
        if (keys[key])
            return false;
    }
    return true;
}

bool noKeysAreActive(const CPianoKeyboardState &state)
{
    return noKeysAreActive(state.snapshot());
}

bool require(bool condition, const char *description)
{
    if (condition)
        return true;

    std::fprintf(stderr, "Failed: %s\n", description);
    return false;
}

bool testChordAndRepeatedNotes()
{
    CPianoKeyboardState state;
    state.noteOn(0, 60);
    state.noteOn(0, 64);
    state.noteOn(0, 67);
    if (!require(keyIsActive(state, 60) && keyIsActive(state, 64) && keyIsActive(state, 67),
                 "a chord activates every pitch"))
        return false;

    state.noteOn(0, 60);
    state.noteOff(0, 60);
    if (!require(keyIsActive(state, 60), "one note-off leaves a repeated pitch active"))
        return false;

    state.noteOff(0, 60);
    state.noteOff(0, 64);
    state.noteOff(0, 67);
    return require(noKeysAreActive(state), "the chord releases after its final note-offs");
}

bool testMultichannelAndVelocityZero()
{
    CPianoKeyboardState state;
    state.noteOn(0, 60);
    state.noteOn(1, 60);
    state.noteOn(1, 64);
    if (!require(state.snapshot(0)[60 - CPianoKeyboardState::FIRST_PIANO_PITCH] &&
                 !state.snapshot(0)[64 - CPianoKeyboardState::FIRST_PIANO_PITCH],
                 "a channel snapshot includes only that channel"))
        return false;
    if (!require(state.snapshot(1)[60 - CPianoKeyboardState::FIRST_PIANO_PITCH] &&
                 state.snapshot(1)[64 - CPianoKeyboardState::FIRST_PIANO_PITCH],
                 "another channel snapshot has its own active pitches"))
        return false;
    if (!require(noKeysAreActive(state.snapshot(CPianoKeyboardState::MIDI_CHANNEL_COUNT)),
                 "an invalid channel snapshot is empty"))
        return false;

    state.noteOff(1, 64);
    state.noteOff(0, 60);
    if (!require(keyIsActive(state, 60), "a pitch remains active on another channel"))
        return false;

    state.noteOn(1, 60, 0);
    if (!require(!keyIsActive(state, 60), "velocity zero releases a matching note-on"))
        return false;

    state.noteOn(1, 60, 0);
    return require(noKeysAreActive(state), "orphan velocity-zero note-on is a no-op");
}

bool testSustain()
{
    CPianoKeyboardState state;
    state.controlChange(0, 64, 127);
    state.noteOn(0, 60);
    state.noteOff(0, 60);
    if (!require(keyIsActive(state, 60), "sustain keeps a released note active"))
        return false;

    state.noteOn(0, 64);
    state.controlChange(0, 64, 0);
    if (!require(!keyIsActive(state, 60), "releasing sustain removes only sustained voices"))
        return false;
    if (!require(keyIsActive(state, 64), "releasing sustain preserves held voices"))
        return false;

    state.noteOff(0, 64);
    return require(noKeysAreActive(state), "a held voice releases after sustain is off");
}

bool testControls()
{
    CPianoKeyboardState state;
    state.controlChange(0, 64, 127);
    state.noteOn(0, 60);
    state.controlChange(0, 120, 0);
    if (!require(!keyIsActive(state, 60), "CC 120 clears held and sustained voices"))
        return false;

    state.noteOn(0, 62);
    state.noteOff(0, 62);
    if (!require(keyIsActive(state, 62),
                 "CC 120 preserves the sustain pedal for future notes"))
        return false;
    state.controlChange(0, 64, 0);
    if (!require(!keyIsActive(state, 62), "sustain off clears the future sustained note"))
        return false;

    state.noteOn(0, 60);
    state.controlChange(0, 123, 0);
    if (!require(!keyIsActive(state, 60), "CC 123 clears held voices without sustain"))
        return false;

    state.controlChange(0, 64, 127);
    state.noteOn(0, 60);
    state.controlChange(0, 123, 0);
    if (!require(keyIsActive(state, 60), "CC 123 transfers held voices to sustain"))
        return false;

    state.noteOn(0, 64);
    state.controlChange(0, 121, 0);
    if (!require(!keyIsActive(state, 60), "CC 121 clears sustained voices"))
        return false;
    if (!require(keyIsActive(state, 64), "CC 121 preserves held voices"))
        return false;

    state.noteOff(0, 64);
    return require(noKeysAreActive(state), "CC 121 disables sustain for later note-offs");
}

bool testClearAndPianoRange()
{
    CPianoKeyboardState state;
    state.noteOn(0, 20);
    state.noteOn(0, 109);
    state.noteOn(0, 21);
    state.noteOn(0, 108);
    if (!require(keyIsActive(state, 21) && keyIsActive(state, 108),
                 "the piano endpoints are present in the snapshot"))
        return false;

    state.noteOff(0, 20);
    state.noteOff(0, 109);
    if (!require(keyIsActive(state, 21) && keyIsActive(state, 108),
                 "out-of-range pitches do not clamp into piano keys"))
        return false;

    state.controlChange(0, 64, 127);
    state.noteOff(0, 21);
    state.clearChannel(0);
    if (!require(noKeysAreActive(state), "clearChannel removes all voices from one channel"))
        return false;

    state.noteOn(0, 60);
    state.noteOn(1, 64);
    state.clearAll();
    return require(noKeysAreActive(state), "clearAll removes all channel state");
}

bool testBoundsAndSaturation()
{
    CPianoKeyboardState state;
    state.noteOn(-1, 60);
    state.noteOn(CPianoKeyboardState::MIDI_CHANNEL_COUNT, 60);
    state.noteOn(0, -1);
    state.noteOn(0, CPianoKeyboardState::MIDI_PITCH_COUNT);
    state.noteOff(0, 60);
    state.controlChange(-1, 64, 127);
    state.clearChannel(CPianoKeyboardState::MIDI_CHANNEL_COUNT);
    if (!require(noKeysAreActive(state), "invalid indices and orphan note-offs are no-ops"))
        return false;

    const int saturationCount = 70000;
    for (int count = 0; count < saturationCount; ++count)
        state.noteOn(0, 60);
    for (int count = 0; count < 65534; ++count)
        state.noteOff(0, 60);
    if (!require(keyIsActive(state, 60), "saturated counter remains active before its final release"))
        return false;

    state.noteOff(0, 60);
    state.noteOff(0, 60);
    return require(!keyIsActive(state, 60), "saturated counter does not overflow or underflow");
}
}

int main()
{
    if (!testChordAndRepeatedNotes() ||
        !testMultichannelAndVelocityZero() ||
        !testSustain() ||
        !testControls() ||
        !testClearAndPianoRange() ||
        !testBoundsAndSaturation())
        return 1;

    return 0;
}
