#ifndef METRONOME_H
#define METRONOME_H

#include <QVector>
#include <QDir>
#include "MidiEvent.h"
#include "Queue.h"
#include "MidiFile.h"

class CMetronome
{
public:

    void clear() {
        nextMetronomeEvent.clear();
        metronomePattern.clear();
        repeatPattern.clear();
        metronomeIndex = 0;
        nextOtherEvent.clear();
    }

    void rewindToStart() {
        generateMetronomeTickPattern();
    }

    CMidiEvent getNextMergedEvent(CQueue<CMidiEvent>* otherQueue);

    void setBarLength(int barLength, int timeSigBottom) {
        m_barLength = barLength;
        m_timeSigBottom = timeSigBottom;
        resetTicks();
    }

private:
    void generateMetronomeTickPattern();
    void writeStandardCsv(QDir dir);
    void loadMetronomeCsv(QDir dir, QString fileName);
    void resetTicks() {
        nextMetronomeEvent.clear();
        metronomeIndex = 0;
        metronomeBarTicks = 0;
    }

    CMidiEvent getMetronomePattern(int index) {
        CMidiEvent event = metronomePattern[index];
        if (m_timeSigBottom >= 8) {
            event.setDeltaTime(event.deltaTime()/2);
        }
        else if (m_timeSigBottom == 2) {
            event.setDeltaTime(event.deltaTime() * 2);
        }
        return event;
    }

    void addRepeatPattern (CMidiEvent event, long timeCode);

    const int drumChanel = MIDI_DRUM_CHANNEL;
    const char * DEFAULT_FILE_NAME = "Default.txt";

    CMidiEvent nextMetronomeEvent;
    CMidiEvent nextOtherEvent;
    QVector<CMidiEvent> metronomePattern;
    QVector<CMidiEvent> repeatPattern;
    int metronomeIndex = 0;
    int metronomeBarTicks = 0;
    int ticksPerBeat = DEFAULT_PPQN;
    CQueue<CMidiEvent>* otherQueue;
    int m_barLength, m_timeSigBottom;
};

#endif // METRONOME_H
