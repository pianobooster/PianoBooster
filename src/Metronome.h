#ifndef METRONOME_H
#define METRONOME_H

#define MAX_VELOCITY 127

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

    void skipMetronomeEvent() {
        metronomeIndex++;
    }

    CMidiEvent getNextMergedEvent(CQueue<CMidiEvent>* otherQueue, bool metronomeActive = true);

    void setBarLength(int barLength, int timeSigBottom) {
        m_barLength = barLength;
        m_timeSigBottom = timeSigBottom;
        resetTicks();
    }

    int getBarSound() {
        return barSound;
    }

    int getBeatSound() {
        return beatSound;
    }

    int getBarVelocity() {
        return barVelocity;
    }

    int getBeatVelocity() {
        return beatVelocity;
    }
    
    int getMetronomeVolume() {
        return metronomeVolume;
    }

    void setBarSound(int key) {
        barSound = key;
    }

    void setBeatSound(int key) {
        beatSound = key;
    }

    void setBarVelocity(int velocity) {
        barVelocity = velocity;
    }

    void setBeatVelocity(int velocity) {
        beatVelocity = velocity;
    }
    
    void setMetronomeVolume(int volume) {
        metronomeVolume = volume; 
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
    int barSound = -1;
    int beatSound = -1;
    int barVelocity = -1;
    int beatVelocity = -1;
    int metronomeVolume = MAX_MIDI_VOLUME / 2;
    
};

#endif // METRONOME_H
