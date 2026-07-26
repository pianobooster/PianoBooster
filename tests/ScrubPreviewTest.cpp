#include "ScrubPreview.h"

#include <cstddef>
#include <cstdio>
#include <vector>

namespace
{
typedef CScrubPreviewState Preview;

bool require(bool condition, const char *description)
{
    if (condition)
        return true;

    std::fprintf(stderr, "Failed: %s\n", description);
    return false;
}

Preview::note_t note(int channel, int pitch, int velocity,
                     Preview::hand_t hand)
{
    return Preview::note_t(channel, pitch, velocity, hand);
}

bool isCommand(const Preview::command_t &command,
               Preview::commandType_t type,
               int channel,
               int pitch,
               int velocity,
               Preview::hand_t hand)
{
    return command.type == type &&
           command.channel == channel &&
           command.pitch == pitch &&
           command.velocity == velocity &&
           command.hand == hand;
}

bool noKeysAreActive(const Preview::snapshot_t &snapshot)
{
    for (std::size_t key = 0; key < snapshot.left.size(); ++key) {
        if (snapshot.left[key] || snapshot.right[key])
            return false;
    }
    return true;
}

bool testGroupLifecycleAndCleanup()
{
    Preview state;
    std::vector<Preview::note_t> notes;
    notes.push_back(note(2, 60, 91, Preview::hand_t::Left));
    notes.push_back(note(3, 64, 87, Preview::hand_t::Right));

    const Preview::groupResult_t started = state.startGroup(100, 1000, notes);
    if (!require(started.triggered && !started.rateLimited,
                 "the first group triggers immediately"))
        return false;
    if (!require(started.acceptedNotes == 2 && started.commands.size() == 2,
                 "the first group accepts and emits all eligible notes"))
        return false;
    if (!require(isCommand(started.commands[0], Preview::commandType_t::NoteOn,
                           2, 60, 91, Preview::hand_t::Left) &&
                 isCommand(started.commands[1], Preview::commandType_t::NoteOn,
                           3, 64, 87, Preview::hand_t::Right),
                 "note-on commands preserve group order and note data"))
        return false;
    if (!require(state.active() && state.ownedVoiceCount() == 2 &&
                 state.owns(2, 60) && state.owns(3, 64),
                 "a triggered group owns every emitted voice"))
        return false;

    const Preview::groupResult_t repeated =
            state.startGroup(100, 1100, notes);
    if (!require(!repeated.triggered && !repeated.rateLimited &&
                 repeated.acceptedNotes == 0 && repeated.commands.empty(),
                 "the same group never retriggers"))
        return false;

    if (!require(state.expire(1119).empty() && state.active(),
                 "the gate remains active before 120 milliseconds"))
        return false;

    const Preview::command_list_t expired = state.expire(1120);
    if (!require(expired.size() == 2 && !state.active() &&
                 state.ownedVoiceCount() == 0,
                 "the gate releases all voices at 120 milliseconds"))
        return false;
    if (!require(isCommand(expired[0], Preview::commandType_t::NoteOff,
                           2, 60, 0, Preview::hand_t::Left) &&
                 isCommand(expired[1], Preview::commandType_t::NoteOff,
                           3, 64, 0, Preview::hand_t::Right),
                 "gate cleanup emits individual note-offs in ownership order"))
        return false;
    if (!require(state.expire(2000).empty(),
                 "an expired group cannot emit duplicate note-offs"))
        return false;

    const Preview::groupResult_t stillRepeated =
            state.startGroup(100, 2100, notes);
    if (!require(!stillRepeated.triggered && stillRepeated.commands.empty(),
                 "gate expiry does not make a stationary attack retrigger"))
        return false;

    if (!require(state.clear().empty(),
                 "clearing an already released group emits nothing"))
        return false;
    const Preview::groupResult_t afterClear =
            state.startGroup(100, 2200, notes);
    if (!require(afterClear.triggered && afterClear.acceptedNotes == 2,
                 "explicit cleanup releases the group identity"))
        return false;

    const Preview::command_list_t cleared = state.clear();
    return require(cleared.size() == 2 && !state.active() &&
                   !state.owns(2, 60) && !state.owns(3, 64) &&
                   noKeysAreActive(state.snapshot()),
                   "explicit cleanup releases ownership and the snapshot");
}

bool testRateLimitAndGroupReplacement()
{
    Preview state;
    std::vector<Preview::note_t> firstNotes;
    firstNotes.push_back(note(0, 60, 80, Preview::hand_t::Left));
    std::vector<Preview::note_t> secondNotes;
    secondNotes.push_back(note(0, 62, 81, Preview::hand_t::Right));

    if (!require(state.startGroup(1, 500, firstNotes).triggered,
                 "rate-limit test starts its first group"))
        return false;

    const Preview::groupResult_t limited =
            state.startGroup(2, 549, secondNotes);
    if (!require(!limited.triggered && limited.rateLimited &&
                 limited.commands.empty() && state.owns(0, 60) &&
                 !state.owns(0, 62),
                 "a new group is rejected before the 50 millisecond boundary"))
        return false;

    const Preview::groupResult_t replacement =
            state.startGroup(2, 550, secondNotes);
    if (!require(replacement.triggered && !replacement.rateLimited &&
                 replacement.acceptedNotes == 1 &&
                 replacement.commands.size() == 2,
                 "a new group replaces the old group at the rate boundary"))
        return false;
    if (!require(isCommand(replacement.commands[0],
                           Preview::commandType_t::NoteOff,
                           0, 60, 0, Preview::hand_t::Left) &&
                 isCommand(replacement.commands[1],
                           Preview::commandType_t::NoteOn,
                           0, 62, 81, Preview::hand_t::Right),
                 "replacement sends old note-off before new note-on"))
        return false;

    const Preview::groupResult_t clockRollback =
            state.startGroup(3, 540, firstNotes);
    if (!require(clockRollback.rateLimited && state.owns(0, 62),
                 "a backwards rate clock cannot bypass throttling"))
        return false;

    const Preview::command_list_t rollbackExpiry = state.expire(540);
    if (!require(rollbackExpiry.size() == 1 && !state.active(),
                 "a backwards gate clock releases ownership defensively"))
        return false;

    Preview clearedState;
    clearedState.startGroup(10, 1000, firstNotes);
    if (!require(clearedState.clear().size() == 1,
                 "direction cleanup releases the current group"))
        return false;
    const Preview::groupResult_t limitedAfterClear =
            clearedState.startGroup(11, 1049, secondNotes);
    if (!require(limitedAfterClear.rateLimited && !clearedState.active(),
                 "cleanup cannot bypass the global preview rate limit"))
        return false;

    return require(clearedState.startGroup(11, 1050, secondNotes).triggered,
                   "a cleaned state can trigger again at the rate boundary");
}

bool testGroupLimitValidationAndStableOrder()
{
    Preview state;
    state.externalNoteOn(0, 10);

    std::vector<Preview::note_t> notes;
    notes.push_back(note(-1, 60, 80, Preview::hand_t::Left));
    notes.push_back(note(0, -1, 80, Preview::hand_t::Left));
    notes.push_back(note(0, 60, 0, Preview::hand_t::Left));
    notes.push_back(note(0, 61, 128, Preview::hand_t::Right));
    notes.push_back(note(0, 10, 80, Preview::hand_t::Left));
    for (int pitch = 20; pitch < 60; ++pitch)
        notes.push_back(note(1, pitch, pitch, Preview::hand_t::Right));

    const Preview::groupResult_t result = state.startGroup(1, 0, notes);
    if (!require(result.triggered &&
                 result.acceptedNotes == Preview::MAX_GROUP_NOTES &&
                 result.commands.size() == Preview::MAX_GROUP_NOTES &&
                 state.ownedVoiceCount() == Preview::MAX_GROUP_NOTES,
                 "a group accepts at most 32 eligible, non-conflicting notes"))
        return false;
    if (!require(isCommand(result.commands.front(),
                           Preview::commandType_t::NoteOn,
                           1, 20, 20, Preview::hand_t::Right) &&
                 isCommand(result.commands.back(),
                           Preview::commandType_t::NoteOn,
                           1, 51, 51, Preview::hand_t::Right),
                 "the cap retains the first 32 eligible notes in stable order"))
        return false;
    if (!require(!state.owns(1, 52) && state.externalActive(0, 10),
                 "notes beyond the cap and externally active notes are excluded"))
        return false;

    const Preview::command_list_t cleanup = state.clear();
    if (!require(cleanup.size() == Preview::MAX_GROUP_NOTES,
                 "cleanup emits one note-off for every capped voice"))
        return false;
    for (std::size_t index = 0; index < cleanup.size(); ++index) {
        if (!require(cleanup[index].type == Preview::commandType_t::NoteOff &&
                     cleanup[index].pitch == static_cast<int>(20 + index),
                     "capped cleanup preserves individual ownership order"))
            return false;
    }

    const std::vector<Preview::note_t> invalidOnly(
            1, note(Preview::MIDI_CHANNEL_COUNT,
                    Preview::MIDI_PITCH_COUNT,
                    -1,
                    Preview::hand_t::Left));
    const Preview::groupResult_t empty =
            state.startGroup(2, 100, invalidOnly);
    return require(empty.triggered && empty.acceptedNotes == 0 &&
                   empty.commands.empty() && !state.active(),
                   "an attack with no eligible notes is a safe empty group");
}

bool testSnapshotsByHandAndVoice()
{
    Preview state;
    std::vector<Preview::note_t> notes;
    notes.push_back(note(0, 20, 70, Preview::hand_t::Left));
    notes.push_back(note(0, 21, 71, Preview::hand_t::Left));
    notes.push_back(note(1, 60, 72, Preview::hand_t::Left));
    notes.push_back(note(2, 60, 73, Preview::hand_t::Right));
    notes.push_back(note(3, 108, 74, Preview::hand_t::Right));
    notes.push_back(note(3, 109, 75, Preview::hand_t::Right));
    notes.push_back(note(4, 64, 76, Preview::hand_t::Right));
    notes.push_back(note(4, 64, 77, Preview::hand_t::Right));

    const Preview::groupResult_t started = state.startGroup(8, 100, notes);
    if (!require(started.acceptedNotes == notes.size() &&
                 state.ownedVoiceCount() == notes.size(),
                 "snapshots retain multichannel and duplicate voices"))
        return false;

    const Preview::snapshot_t snapshot = state.snapshot();
    const std::size_t firstKey = 0;
    const std::size_t middleC =
            static_cast<std::size_t>(60 - Preview::FIRST_PIANO_PITCH);
    const std::size_t pitch64 =
            static_cast<std::size_t>(64 - Preview::FIRST_PIANO_PITCH);
    const std::size_t lastKey =
            static_cast<std::size_t>(108 - Preview::FIRST_PIANO_PITCH);

    if (!require(snapshot.left[firstKey] && !snapshot.right[firstKey] &&
                 snapshot.left[middleC] && snapshot.right[middleC] &&
                 !snapshot.left[pitch64] && snapshot.right[pitch64] &&
                 !snapshot.left[lastKey] && snapshot.right[lastKey],
                 "snapshots separate hands and preserve a same-pitch tie"))
        return false;

    if (!require(state.owns(0, 20) && state.owns(3, 109),
                 "ownership includes valid MIDI notes outside the piano range"))
        return false;

    const Preview::command_list_t cleanup = state.clear();
    return require(cleanup.size() == notes.size() &&
                   noKeysAreActive(state.snapshot()) &&
                   state.ownedVoiceCount() == 0,
                   "snapshot cleanup includes duplicate and off-keyboard voices");
}

bool testExternalInputAndCollision()
{
    Preview state;
    std::vector<Preview::note_t> notes;
    notes.push_back(note(0, 60, 90, Preview::hand_t::Left));
    notes.push_back(note(0, 60, 91, Preview::hand_t::Right));
    notes.push_back(note(0, 64, 92, Preview::hand_t::Right));
    state.startGroup(1, 100, notes);

    const Preview::command_list_t collision =
            state.externalNoteOn(0, 60, 100);
    if (!require(collision.size() == 2 &&
                 isCommand(collision[0], Preview::commandType_t::NoteOff,
                           0, 60, 0, Preview::hand_t::Left) &&
                 isCommand(collision[1], Preview::commandType_t::NoteOff,
                           0, 60, 0, Preview::hand_t::Right),
                 "external note-on releases every conflicting preview voice first"))
        return false;
    if (!require(state.externalActive(0, 60) &&
                 !state.owns(0, 60) && state.owns(0, 64) &&
                 state.ownedVoiceCount() == 1,
                 "collision preserves non-conflicting preview ownership"))
        return false;

    const Preview::command_list_t cleanup = state.clear();
    if (!require(cleanup.size() == 1 && cleanup[0].pitch == 64 &&
                 state.externalActive(0, 60),
                 "later cleanup cannot emit an off for external ownership"))
        return false;

    state.externalNoteOn(0, 60, 110);
    state.externalNoteOff(0, 60);
    if (!require(state.externalActive(0, 60),
                 "external ownership counts repeated voices"))
        return false;
    state.externalNoteOn(0, 60, 0);
    if (!require(!state.externalActive(0, 60),
                 "velocity-zero external input releases one voice"))
        return false;

    state.externalNoteOn(1, 62);
    state.externalNoteOn(2, 67);
    std::vector<Preview::note_t> blocked;
    blocked.push_back(note(1, 62, 70, Preview::hand_t::Left));
    blocked.push_back(note(1, 65, 71, Preview::hand_t::Right));
    const Preview::groupResult_t filtered =
            state.startGroup(2, 200, blocked);
    if (!require(filtered.acceptedNotes == 1 &&
                 filtered.commands.size() == 1 &&
                 filtered.commands[0].pitch == 65 &&
                 !state.owns(1, 62),
                 "a group skips pitches already owned externally"))
        return false;

    if (!require(state.externalNoteOn(-1, 60).empty() &&
                 state.externalNoteOn(Preview::MIDI_CHANNEL_COUNT, 60).empty() &&
                 state.externalNoteOn(0, Preview::MIDI_PITCH_COUNT).empty() &&
                 state.externalNoteOn(0, 61, -1).empty() &&
                 state.externalNoteOn(0, 61, 128).empty() &&
                 !state.owns(-1, 60) &&
                 !state.owns(0, Preview::MIDI_PITCH_COUNT) &&
                 !state.externalActive(-1, 60),
                 "invalid external notes are ignored"))
        return false;
    state.externalNoteOff(-1, -1);
    state.clearExternalChannel(1);
    state.clearExternalChannel(-1);
    if (!require(!state.externalActive(1, 62) &&
                 state.externalActive(2, 67),
                 "external channel cleanup is selective"))
        return false;
    state.clearExternalNotes();
    if (!require(!state.externalActive(0, 60) &&
                 !state.externalActive(1, 62) &&
                 !state.externalActive(2, 67),
                 "external ownership can be reset on load or reconnect"))
        return false;

    const Preview::command_list_t finalCleanup = state.clear();
    return require(finalCleanup.size() == 1 &&
                   finalCleanup[0].pitch == 65 &&
                   !state.active(),
                   "external reset does not disturb unrelated preview cleanup");
}
}

int main()
{
    if (!testGroupLifecycleAndCleanup() ||
        !testRateLimitAndGroupReplacement() ||
        !testGroupLimitValidationAndStableOrder() ||
        !testSnapshotsByHandAndVoice() ||
        !testExternalInputAndCollision())
        return 1;

    return 0;
}
