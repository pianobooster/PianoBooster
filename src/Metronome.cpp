#include "Metronome.h"
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>


void  CMetronome::writeStandardCsv(QDir dir) {
    QFile metroFile(dir.filePath(DEFAULT_FILE_NAME));

    if (!metroFile.exists()) {

        if (!metroFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            ppLogError("Cannot create metronome file %s", qPrintable(metroFile.fileName()));
            return;
        }

        QTextStream out(&metroFile);
        out << "0, 0, Header, 1, 1, 480" << endl;
        out << "1, 0, Start_track" << endl;
        out << "1, 0, Cue_point_t, \"Metronome Clicks\"" << endl;
        out << "1, 0, Note_on_c, 9, 34, 127" << endl;
        out << "1, 0, Note_off_c, 9, 34, 0" << endl;
        out << "1, 480, Note_on_c, 9, 33, 127" << endl;
        out << "1, 480, Note_off_c, 9, 33, 0" << endl;
        out << "1, 960, Note_on_c, 9, 33, 127" << endl;
        out << "1, 960, Note_off_c, 9, 33, 0" << endl;
        out << "1, 1440, Note_on_c, 9, 33, 127" << endl;
        out << "1, 1440, Note_off_c, 9, 33, 0" << endl;
        out << "1, 1920, End_track" << endl;
        metroFile.close();
    }
}

void CMetronome::addRepeatPattern (CMidiEvent event, long timeCode){
    if (timeCode >= ticksPerBeat*2 && timeCode < ticksPerBeat*4 ) {
        if (repeatPattern.empty()) {
            event.setDeltaTime(timeCode - ticksPerBeat*2);
        }
        repeatPattern.append(event);
    }
}

void  CMetronome::loadMetronomeCsv(QDir dir, QString fileName) {
    QFile metroFile(dir.filePath(fileName));
    if (!metroFile.open(QIODevice::ReadOnly| QIODevice::Text)) {
       ppLogError("Failed to open file %s", qPrintable(fileName));
       return;
    }

    int deltaTime = 0;
    long previousTime = 0;
    int metronomePpqn = DEFAULT_PPQN;
    double timeScaling = CMidiFile::getPulsesPerQuarterNote() / (double)metronomePpqn;

    long timeCode = 0;
    QTextStream in(&metroFile);

    while (!in.atEnd()){
    QString line=in.readLine().trimmed(); // reads line from file
        if (line.startsWith('#')) {
            continue;
        }
        QStringList elements = line.split(",");
        if (elements.size() >= 6) {
            timeCode = elements[1].trimmed().toLong() * timeScaling;
            deltaTime = timeCode - previousTime;
            previousTime = timeCode;
            QString cmd = elements[2].trimmed();
            int channel = elements[3].trimmed().toInt();
            QString noteStr = elements[4].trimmed();
            int note = noteStr.toInt();
            QString velocityStr = elements[5].trimmed();
            int velocity = velocityStr.toInt();

            CMidiEvent midiEvent;
            if (cmd == "Header") {
                metronomePpqn = velocity;
                timeScaling = CMidiFile::getPulsesPerQuarterNote() / (double)metronomePpqn;
                ticksPerBeat = metronomePpqn * timeScaling;

            } else if (cmd == "Note_on_c") {
                midiEvent.noteOnEvent(deltaTime, channel, note, velocity);
                metronomePattern.append(midiEvent);
                addRepeatPattern(midiEvent, timeCode );
            } else if (cmd == "Note_off_c") {
                midiEvent.noteOffEvent(deltaTime, channel, note, velocity);
                metronomePattern.append(midiEvent);
                addRepeatPattern(midiEvent, timeCode );
            }
        }
    }

    if (timeCode < ticksPerBeat*4 && !repeatPattern.empty()) {
        // Extend the metronome pattern by repeating in the pattern in the last beats twice to make four extra beats.
        repeatPattern[0].addDeltaTime(ticksPerBeat*4 - timeCode);
        metronomePattern.append(repeatPattern);
        metronomePattern.append(repeatPattern);
    }
    metroFile.close();
}

void  CMetronome::generateMetronomeTickPattern() {
    clear();

    QDir metronomeDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));

    metronomeDir.mkpath("PianoBooster/metronome-patterns");
    metronomeDir.cd("PianoBooster/metronome-patterns");

    writeStandardCsv(metronomeDir);
    loadMetronomeCsv(metronomeDir, DEFAULT_FILE_NAME);
}

CMidiEvent CMetronome::getNextMergedEvent(CQueue<CMidiEvent>* otherQueue) {

    if (otherQueue->length() == 0) {
        return CMidiEvent();
    }

    if (metronomePattern.size() == 0) {
        return otherQueue->pop();
    }


    if (nextOtherEvent.type() == MIDI_NONE) {
        nextOtherEvent = otherQueue->pop();
    }
    if (nextMetronomeEvent.type() == MIDI_NONE )
    {
        bool rewindMetronome = false;

        if (metronomeIndex >= metronomePattern.size()) {
            rewindMetronome = true;
            metronomeIndex = 0;
        }

        CMidiEvent metronomeEvent = getMetronomePattern(metronomeIndex);

        if (m_barLength - (metronomeBarTicks + metronomeEvent.deltaTime()) <= 0) {
            metronomeIndex = 0;
            metronomeEvent = getMetronomePattern(metronomeIndex);
            rewindMetronome = true;
        }

        if (rewindMetronome) {
            metronomeEvent.addDeltaTime(m_barLength - metronomeBarTicks);
            metronomeBarTicks = 0;
        } else {
            metronomeBarTicks += metronomeEvent.deltaTime();
        }

        nextMetronomeEvent = metronomeEvent;
        metronomeIndex++;
    }

    return nextMetronomeEvent.getNextMergedEvent(nextOtherEvent);
}
