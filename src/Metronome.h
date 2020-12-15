#ifndef METRONOME_H
#define METRONOME_H

#include <QVector>
#include "MidiEvent.h"
#include "Queue.h"

class CMetronome
{
public:
    CMetronome() {
        generateMetronomeTickPatern();
    }

    void clear() {
        nextMetronomeEvent.clear();
        metronomePatern.clear();
        metronomeIndex = 0;
        nextOtherEvent.clear();
    }

    void resetToStart() {
        nextMetronomeEvent.clear();
        metronomeIndex = 0;
        firstMetronomeEvent = true;
    }


    void generateMetronomeTickPatern();

    CMidiEvent getNextMergedEvent(CQueue<CMidiEvent>* otherQueue);

    void setBarLength(int barLength, int beatLength) {
        m_barLength = barLength;
        m_beatLength = beatLength;
        generateMetronomeTickPatern();
        ppLogInfo("ZZ setBarLength %d %d", barLength, beatLength);
    }

    void flushTicks(int ticks) {

    }

private:
    int drumChanel = MIDI_DRUM_CHANNEL;

    void addDrumNote( int deltaTime, int note, int velocity);

    CMidiEvent nextMetronomeEvent;
    CMidiEvent nextOtherEvent;
    QVector<CMidiEvent> metronomePatern;
    int  metronomeIndex = 0;
    bool firstMetronomeEvent = true;
    CQueue<CMidiEvent>* otherQueue;
    int m_barLength, m_beatLength;

};

#endif // METRONOME_H
