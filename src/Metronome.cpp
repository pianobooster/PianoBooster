#include "Metronome.h"

/*
CMetronome::CMetronome()
{
    generateMetronomeTickPatern();
}*/


void  CMetronome::generateMetronomeTickPatern() {
    ppLogDebug("ZZ generateMetronomeTickPatern %d/%d", m_barLength,m_beatLength);

    // 76 E4 Hi Wood Block
    // 77 F4 Low Wood Block
    //33 Metronome Click (GM2)
    // 34 Metronome Bell (GM2)
    metronomePatern.clear();
    resetToStart();
    //addDrumNote(1.0f * m_beatLength, 34, 127); //  34 Metronome Bell (GM2)
    addDrumNote(1.0f * m_beatLength, 76, 127); // 76 E4 Hi Wood Block
    //addDrumNote(0,                   77, 127); // 77 F4 Low Wood Block
   for (int ticks = m_beatLength;  ticks < m_barLength; ticks+=  m_beatLength) {
        //addDrumNote(1.0f* m_beatLength, 33, 127); //33 Metronome Click (GM2)
        addDrumNote(1.0f* m_beatLength, 77, 127); // 77 F4 Low Wood Block

    }
    //addDrumNote(1.0f, 34, 127);
    //addDrumNote(1.0f, 33, 127);
    //addDrumNote(1.0f, 33, 127);
    //addDrumNote(96, 33, 127);




    /*
    //82 Shaker (GM2)

    addDrumNote(96/2, 82, 75);
    addDrumNote(96/2, 34, 127);
    addDrumNote(0, 82, 75);
    addDrumNote(96/2, 82, 75);
    addDrumNote(96/2, 33, 127);
    addDrumNote(0, 82, 75);
    addDrumNote(96/2, 82, 75);
    addDrumNote(96/2, 33, 127);
    addDrumNote(0, 82, 75);
    metronomeIndex = 5;
    // */

}

void CMetronome::addDrumNote( int deltaTime, int note, int velocity)
{
    CMidiEvent midiEventOn;
    midiEventOn.noteOnEvent(deltaTime, drumChanel, note, velocity);
    metronomePatern.append(midiEventOn);

    CMidiEvent midiEventoff;
    midiEventoff.noteOffEvent(0, drumChanel, note, velocity);
    metronomePatern.append(midiEventoff);

}

CMidiEvent CMetronome::getNextMergedEvent(CQueue<CMidiEvent>* otherQueue) {

    if (otherQueue->length() == 0 || metronomePatern.size() == 0)
        return CMidiEvent();

    if (nextOtherEvent.type() == MIDI_NONE) {
        nextOtherEvent = otherQueue->pop();
       // ppLogInfo("ZZ CMetronome::nextOtherEvent %04x %03d", nextOtherEvent.type(), nextOtherEvent.deltaTime());
    }
    if (nextMetronomeEvent.type() == MIDI_NONE)
    {
       // ppLogInfo("ZZ CMetronome::nextMetronomeEvent %d of %d", metronomeIndex, metronomePatern.size());

        nextMetronomeEvent = metronomePatern[metronomeIndex++];
        if (metronomeIndex >= metronomePatern.size()) {
            metronomeIndex = 0;
        }
        if (firstMetronomeEvent) {
            firstMetronomeEvent = false;
            nextMetronomeEvent.setDeltaTime(0);
        }
    }
    //return nextMetronomeEvent.getNextMergedEvent(nextOtherEvent);
    CMidiEvent result = nextMetronomeEvent.getNextMergedEvent(nextOtherEvent);
    //ppLogInfo("ZZ CMetronome::result  %04x  %d  %d", result.type(), result.channel(), result.note());
    return result;
}
