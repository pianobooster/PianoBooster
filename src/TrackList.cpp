/*********************************************************************************/
/*!
@file           TrackList.cpp

@brief          The Design.

@author         L. J. Barman

    Copyright (c)   2008-2013, L. J. Barman, all rights reserved

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

#include <QtGui>

#include "TrackList.h"
#include "Song.h"
#include "Settings.h"

void CTrackList::init(CSong* songObj, CSettings* settings)
{
    m_song = songObj;
    m_settings = settings;
}

void CTrackList::reset(int numberOfTracks)
{
    m_partsList.clear();
    m_midiChannels.clear();
    for (int chan = 0; chan < MAX_MIDI_CHANNELS; chan++) {
        for (int i = 0; i < MAX_MIDI_NOTES; i++) {
            m_noteFrequency[chan][i]=0;
        }
        m_midiChannels.append(AnalyseItem(numberOfTracks));
    }
}

void CTrackList::currentRowChanged(int currentRow)
{
    if (!m_song) return;
    if (currentRow >= m_partsList.size()|| currentRow < 0)
        return;

    m_song->setActiveChannel(m_partsList[currentRow].midiChannel());
}

void CTrackList::examineMidiEvent(CMidiEvent event)
{
    int chan = event.channel();

    assert (chan < MAX_MIDI_CHANNELS && chan >= 0);
    if (chan < MAX_MIDI_CHANNELS && chan >= 0)
    {
        if (event.type() == MIDI_NOTE_ON)
        {
            m_midiChannels[chan].addNoteEvent(event);

            // count each note so we can guess the key signature
            if (event.note() >= 0 && event.note() < MAX_MIDI_NOTES) {
                m_noteFrequency[chan][event.note()]++;
            }
         }

        if (event.type() == MIDI_PROGRAM_CHANGE) {
            m_midiChannels[chan].addPatch(event.programme());
        }
    }
}

// Returns true if there is a piano part on channels 3 & 4
bool CTrackList::pianoPartConvetionTest()
{
    AnalyseItem left = m_midiChannels[CONVENTION_LEFT_HAND_CHANNEL];
    AnalyseItem right = m_midiChannels[CONVENTION_RIGHT_HAND_CHANNEL];
    // Both hands on channels 3 & 4
    if (left.active() && right.active()) {
        if (left.firstPatch() == right.firstPatch() && isPianoOrOrganPatch(right.firstPatch())) {
            CNote::setChannelHands(CONVENTION_LEFT_HAND_CHANNEL, CONVENTION_RIGHT_HAND_CHANNEL);
            return true;
        }
    }
    // Right hand only channel 4 and no left hand on channel 3
    if (right.active() && !left.active()) {
        if (isPianoOrOrganPatch(right.firstPatch())) {
            CNote::setChannelHands(CONVENTION_LEFT_HAND_CHANNEL, CONVENTION_RIGHT_HAND_CHANNEL);
            return true;
        }
    }
    // Left hand only channel 3 and no right hand on channel 4
    if (left.active() && !right.active()) {
        if (isPianoOrOrganPatch(left.firstPatch())) {
            CNote::setChannelHands(CONVENTION_LEFT_HAND_CHANNEL, CONVENTION_RIGHT_HAND_CHANNEL);
            return true;
        }
    }
    return false;
}

bool CTrackList::findLeftAndRightPianoParts()
{
    int patchA = -1;
    int chanA = -1;

    for (int chan = 0 ; chan < MAX_MIDI_CHANNELS; chan++) {
        if (chan == MIDI_DRUM_CHANNEL) {
            continue;
        }
        if (m_midiChannels[chan].active()) {
            int patch = m_midiChannels[chan].firstPatch();
            if (isPianoOrOrganPatch(patch)) {
                if (m_midiChannels[chan].trackCount() > 1) {
                    CNote::setChannelHands(chan, chan);
                    return true;
                }

                if (patchA == -1) {
                    patchA = patch;
                    chanA = chan;
                } else {
                    if (patchA == patch) {
                        if (averageNotePitch(chan) < averageNotePitch(chanA)) {
                            CNote::setChannelHands(chan, chanA);
                        } else {
                            CNote::setChannelHands(chanA, chan);
                        }
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int CTrackList::guessKeySignature(int chanA, int chanB)
{
    int chan;
    int i;
    int keySignature = 0;
    int highScore = 0;
    int scale[MIDI_OCTAVE];
    for (i=0; i < MIDI_OCTAVE; i++)
        scale[i] = 0;
    for (chan = 0 ; chan < MAX_MIDI_CHANNELS; chan++)
    {
        if (chanA == -1 || chan == chanA || chan == chanB)
        {
            for (int note = 0; note < MAX_MIDI_NOTES; note++)
                scale[note % MIDI_OCTAVE] += m_noteFrequency[chan][note];
        }
    }

    for (i = 0; i < MIDI_OCTAVE; i++)
    {
        int score = 0;
        struct {
            int offset;
            int key;
        } keyLookUp[MIDI_OCTAVE] =
            {
                {0,  0}, // 0  C
                {7,  1}, // 1  G  1#
                {5, -1}, // 2  F  1b
                {2,  2}, // 3  D  2#
                {10,-2}, // 4  Bb 2b
                {9,  3}, // 5  A  3#
                {3, -3}, // 6  Eb 3b
                {4,  4}, // 7  E  4#
                {8, -4}, // 8  Ab 4b
                {11, 5}, // 9  B  5#
                {1, -5}, // 10 Db 5b
                {6,  6}, // 11 F# 6#
            };

        int idx = keyLookUp[i].offset;
        score += scale[(idx + 0 )%MIDI_OCTAVE]; // First note in the scale
        score += scale[(idx + 2 )%MIDI_OCTAVE]; // Tone
        score += scale[(idx + 4 )%MIDI_OCTAVE]; // Tone
        score += scale[(idx + 5 )%MIDI_OCTAVE]; // Semi tone
        score += scale[(idx + 7 )%MIDI_OCTAVE]; // Tone
        score += scale[(idx + 9 )%MIDI_OCTAVE]; // Tone
        score += scale[(idx + 11)%MIDI_OCTAVE]; // Tone
                                                // the Last note don't count it

        if (score > highScore)
        {
            highScore = score;
            keySignature = keyLookUp[i].key;
        }
    }
    return keySignature;
}

// Find an unused channel
int CTrackList::findFreeChannel(int startChannel)
{
    int chan;
    for (chan = startChannel; chan < MAX_MIDI_CHANNELS; chan++)
    {
        if (chan == Cfg::keyboardLightsChan)
            continue;
        if (chan == MIDI_DRUM_CHANNEL)
            continue;
        if (!m_midiChannels[chan].active())
            return chan;
    }
    return -1;      // Not found

}

void CTrackList::refresh()
{
    int rowCount = 0;
    m_partsList.clear();

    for (int chan = 0; chan < MAX_MIDI_CHANNELS; chan++)
    {
        if (m_midiChannels[chan].active())
        {
            m_partsList.append(CTrackListItem(chan));
            rowCount++;
        }
    }

    if (CNote::bothHandsChan() != -2   ) { // -2 for not set -1 for not used
        m_song->setActiveChannel(CNote::bothHandsChan());
    } else if (pianoPartConvetionTest()) {
        m_song->setActiveChannel(CNote::bothHandsChan());
        ppLogInfo("Active both");
    }
    else if (findLeftAndRightPianoParts()) {
        m_song->setActiveChannel(CNote::bothHandsChan());
    } else {
        if (m_partsList.count() > 0) {
            // for the case when there is no piano or organ patch
            m_song->setActiveChannel(m_partsList[0].midiChannel());
        }
    }

    for (int chan = 0; chan < MAX_MIDI_CHANNELS; chan++)
    {
        AnalyseItem item  = m_midiChannels[chan];
        if (item.active())
        {
            if (item.firstPatch() == GM_PIANO_PATCH) {
                // For those midi files that do not include ANY patch we want to set it piano
                CMidiEvent midi;
                midi.programChangeEvent(0, chan, GM_PIANO_PATCH);
                m_song->playMidiEvent(midi);
            }
        }
    }

    if (CStavePos::getKeySignature() == NOT_USED)
        CStavePos::setKeySignature(guessKeySignature(CNote::rightHandChan(), CNote::leftHandChan()), 0);

    // Find an unused channel that we can use for the keyboard
    m_song->reset();
    int goodChan = findFreeChannel(0);
    int badChan = findFreeChannel(goodChan + 1);
    int spareChan = findFreeChannel(badChan  +1 );
    if (badChan == -1)
    {
        // As we have not found two we have not found to empty channels to use
        goodChan = 15 -1;
        badChan  = 16-1;
    }
    m_song->setPianistChannels(goodChan, badChan);
    ppLogInfo("Using Pianist Channels %d + %d", goodChan +1, badChan +1);
    if (Cfg::keyboardLightsChan != -1 && spareChan != -1)
        m_song->mapTrack2Channel(Cfg::keyboardLightsChan,  spareChan);
    for (int chan = 0; chan < MAX_MIDI_CHANNELS; chan++) {
        AnalyseItem item  = m_midiChannels[chan];
        CNote::setRightHandTrack(chan, item.rightHandTrack());
    }
}

int CTrackList::getActiveItemIndex()
{
    int chan;
    for (int i = 0; i < m_partsList.size(); ++i)
    {
        chan = m_partsList[i].midiChannel();
        if (chan == CNote::rightHandChan() )
            return i;
    }
    return 0; // Not found so return first item on the list
}

QStringList CTrackList::getAllChannelProgramNames(bool raw)
{
    QStringList items;
    int chan;
    QString text;
    QString hand;

    for (int i = 0; i < m_partsList.size(); ++i)
    {
        hand.clear();
        chan = m_partsList[i].midiChannel();
        if (raw == false)
        {
            if (CNote::leftHandChan() == chan)
                hand += QObject::tr("L");
            if (CNote::rightHandChan() == chan)
                hand += QObject::tr("R");
        }
        text = QString::number(chan+1) + hand + " " + getChannelProgramName(chan);
        items += text;
    }
    return items;
}

int CTrackList::getActiveHandIndex(whichPart_t whichPart)
{
    int index = 0;
     for (int i = 0; i < m_partsList.size(); ++i)
        if (m_partsList[i].midiChannel() == CNote::getHandChannel( whichPart))
            return index;

    return index;
}

void CTrackList::setActiveHandsIndex(int leftIndex, int rightIndex)
{
    int leftChannel = -1;
    int rightChannel = -1;

    if (leftIndex>=0)
        leftChannel = m_partsList.at(leftIndex).midiChannel();
    if (rightIndex>=0)
        rightChannel = m_partsList.at(rightIndex).midiChannel();
    m_settings->setChannelHands(leftChannel, rightChannel);
    refresh();
    m_song->rewind();
}

// get the track index number of the selected hand
int CTrackList::getHandTrackIndex(whichPart_t whichPart)
{
    int index = 0;
    int midiHand = CNote::getHandChannel(whichPart);
    for (int i = 0; i < m_partsList.size(); ++i)
    {

        if (m_partsList[i].midiChannel() == midiHand)
            return index;
        index++;
    }

    return -1;
}

void CTrackList::changeListWidgetItemView( int index, QListWidgetItem* listWidgetItem )
{
    int chan = m_partsList[index].midiChannel();
    if ( CNote::hasPianoPart( chan ))
    {
        QFont font = listWidgetItem->font();
        if (CNote::rightHandChan() >= 0 && CNote::leftHandChan() >= 0 )
            font.setBold(true);
        listWidgetItem->setFont(font);
        listWidgetItem->setForeground(Qt::darkBlue);
    }
    else if ( m_song->hasPianistKeyboardChannel( chan ) )
        listWidgetItem->setForeground(Qt::lightGray);
}

QString CTrackList::getChannelProgramName(int chan)
{
    if(chan<0 || chan>= MAX_MIDI_CHANNELS)
    {
        assert(true);
        return QString();
    }
    int program = m_midiChannels[chan].firstPatch();

    if (chan==10-1)
        return QObject::tr("Drums");
    QString name = getProgramName(program +1); // Skip
    if (name.isEmpty())
        name = QObject::tr("Unknown");

    return name;
}

QString CTrackList::getProgramName(int program)
{
    const char * const gmInstrumentNames[] =
    {
                   QT_TR_NOOP("(None)"),  // Don't use
        /* 1.   */ QT_TR_NOOP("Grand Piano"),
        /* 2.   */ QT_TR_NOOP("Bright Piano"),
        /* 3.   */ QT_TR_NOOP("Electric Grand"),
        /* 4.   */ QT_TR_NOOP("Honky-tonk Piano"),
        /* 5.   */ QT_TR_NOOP("Electric Piano 1"),
        /* 6.   */ QT_TR_NOOP("Electric Piano 2"),
        /* 7.   */ QT_TR_NOOP("Harpsichord"),
        /* 8.   */ QT_TR_NOOP("Clavi"),
        /* 9.   */ QT_TR_NOOP("Celesta"),
        /* 10.  */ QT_TR_NOOP("Glockenspiel"),
        /* 11.  */ QT_TR_NOOP("Music Box"),
        /* 12.  */ QT_TR_NOOP("Vibraphone"),
        /* 13.  */ QT_TR_NOOP("Marimba"),
        /* 14.  */ QT_TR_NOOP("Xylophone"),
        /* 15.  */ QT_TR_NOOP("Tubular Bells"),
        /* 16.  */ QT_TR_NOOP("Dulcimer"),
        /* 17.  */ QT_TR_NOOP("Drawbar Organ"),
        /* 18.  */ QT_TR_NOOP("Percussive Organ"),
        /* 19.  */ QT_TR_NOOP("Rock Organ"),
        /* 20.  */ QT_TR_NOOP("Church Organ"),
        /* 21.  */ QT_TR_NOOP("Reed Organ"),
        /* 22.  */ QT_TR_NOOP("Accordion"),
        /* 23.  */ QT_TR_NOOP("Harmonica"),
        /* 24.  */ QT_TR_NOOP("Tango Accordion"),
        /* 25.  */ QT_TR_NOOP("Acoustic Guitar (nylon)"),
        /* 26.  */ QT_TR_NOOP("Acoustic Guitar (steel)"),
        /* 27.  */ QT_TR_NOOP("Electric Guitar (jazz)"),
        /* 28.  */ QT_TR_NOOP("Electric Guitar (clean)"),
        /* 29.  */ QT_TR_NOOP("Electric Guitar (muted)"),
        /* 30.  */ QT_TR_NOOP("Overdriven Guitar"),
        /* 31.  */ QT_TR_NOOP("Distortion Guitar"),
        /* 32.  */ QT_TR_NOOP("Guitar harmonics"),
        /* 33.  */ QT_TR_NOOP("Acoustic Bass"),
        /* 34.  */ QT_TR_NOOP("Electric Bass (finger)"),
        /* 35.  */ QT_TR_NOOP("Electric Bass (pick)"),
        /* 36.  */ QT_TR_NOOP("Fretless Bass"),
        /* 37.  */ QT_TR_NOOP("Slap Bass 1"),
        /* 38.  */ QT_TR_NOOP("Slap Bass 2"),
        /* 39.  */ QT_TR_NOOP("Synth Bass 1"),
        /* 40.  */ QT_TR_NOOP("Synth Bass 2"),
        /* 41.  */ QT_TR_NOOP("Violin"),
        /* 42.  */ QT_TR_NOOP("Viola"),
        /* 43.  */ QT_TR_NOOP("Cello"),
        /* 44.  */ QT_TR_NOOP("Contrabass"),
        /* 45.  */ QT_TR_NOOP("Tremolo Strings"),
        /* 46.  */ QT_TR_NOOP("Pizzicato Strings"),
        /* 47.  */ QT_TR_NOOP("Orchestral Harp"),
        /* 48.  */ QT_TR_NOOP("Timpani"),
        /* 49.  */ QT_TR_NOOP("String Ensemble 1"),
        /* 50.  */ QT_TR_NOOP("String Ensemble 2"),
        /* 51.  */ QT_TR_NOOP("SynthStrings 1"),
        /* 52.  */ QT_TR_NOOP("SynthStrings 2"),
        /* 53.  */ QT_TR_NOOP("Choir Aahs"),
        /* 54.  */ QT_TR_NOOP("Voice Oohs"),
        /* 55.  */ QT_TR_NOOP("Synth Voice"),
        /* 56.  */ QT_TR_NOOP("Orchestra Hit"),
        /* 57.  */ QT_TR_NOOP("Trumpet"),
        /* 58.  */ QT_TR_NOOP("Trombone"),
        /* 59.  */ QT_TR_NOOP("Tuba"),
        /* 60.  */ QT_TR_NOOP("Muted Trumpet"),
        /* 61.  */ QT_TR_NOOP("French Horn"),
        /* 62.  */ QT_TR_NOOP("Brass Section"),
        /* 63.  */ QT_TR_NOOP("SynthBrass 1"),
        /* 64.  */ QT_TR_NOOP("SynthBrass 2"),
        /* 65.  */ QT_TR_NOOP("Soprano Sax"),
        /* 66.  */ QT_TR_NOOP("Alto Sax"),
        /* 67.  */ QT_TR_NOOP("Tenor Sax"),
        /* 68.  */ QT_TR_NOOP("Baritone Sax"),
        /* 69.  */ QT_TR_NOOP("Oboe"),
        /* 70.  */ QT_TR_NOOP("English Horn"),
        /* 71.  */ QT_TR_NOOP("Bassoon"),
        /* 72.  */ QT_TR_NOOP("Clarinet"),
        /* 73.  */ QT_TR_NOOP("Piccolo"),
        /* 74.  */ QT_TR_NOOP("Flute"),
        /* 75.  */ QT_TR_NOOP("Recorder"),
        /* 76.  */ QT_TR_NOOP("Pan Flute"),
        /* 77.  */ QT_TR_NOOP("Blown Bottle"),
        /* 78.  */ QT_TR_NOOP("Shakuhachi"),
        /* 79.  */ QT_TR_NOOP("Whistle"),
        /* 80.  */ QT_TR_NOOP("Ocarina"),
        /* 81.  */ QT_TR_NOOP("Lead 1 (square)"),
        /* 82.  */ QT_TR_NOOP("Lead 2 (sawtooth)"),
        /* 83.  */ QT_TR_NOOP("Lead 3 (calliope)"),
        /* 84.  */ QT_TR_NOOP("Lead 4 (chiff)"),
        /* 85.  */ QT_TR_NOOP("Lead 5 (charang)"),
        /* 86.  */ QT_TR_NOOP("Lead 6 (voice)"),
        /* 87.  */ QT_TR_NOOP("Lead 7 (fifths)"),
        /* 88.  */ QT_TR_NOOP("Lead 8 (bass + lead)"),
        /* 89.  */ QT_TR_NOOP("Pad 1 (new age)"),
        /* 90.  */ QT_TR_NOOP("Pad 2 (warm)"),
        /* 91.  */ QT_TR_NOOP("Pad 3 (polysynth)"),
        /* 92.  */ QT_TR_NOOP("Pad 4 (choir)"),
        /* 93.  */ QT_TR_NOOP("Pad 5 (bowed)"),
        /* 94.  */ QT_TR_NOOP("Pad 6 (metallic)"),
        /* 95.  */ QT_TR_NOOP("Pad 7 (halo)"),
        /* 96.  */ QT_TR_NOOP("Pad 8 (sweep)"),
        /* 97.  */ QT_TR_NOOP("FX 1 (rain)"),
        /* 98.  */ QT_TR_NOOP("FX 2 (soundtrack)"),
        /* 99.  */ QT_TR_NOOP("FX 3 (crystal)"),
        /* 100. */ QT_TR_NOOP("FX 4 (atmosphere)"),
        /* 101. */ QT_TR_NOOP("FX 5 (brightness)"),
        /* 102. */ QT_TR_NOOP("FX 6 (goblins)"),
        /* 103. */ QT_TR_NOOP("FX 7 (echoes)"),
        /* 104. */ QT_TR_NOOP("FX 8 (sci-fi)"),
        /* 105. */ QT_TR_NOOP("Sitar"),
        /* 106. */ QT_TR_NOOP("Banjo"),
        /* 107. */ QT_TR_NOOP("Shamisen"),
        /* 108. */ QT_TR_NOOP("Koto"),
        /* 109. */ QT_TR_NOOP("Kalimba"),
        /* 110. */ QT_TR_NOOP("Bag pipe"),
        /* 111. */ QT_TR_NOOP("Fiddle"),
        /* 112. */ QT_TR_NOOP("Shanai"),
        /* 113. */ QT_TR_NOOP("Tinkle Bell"),
        /* 114. */ QT_TR_NOOP("Agogo"),
        /* 115. */ QT_TR_NOOP("Steel Drums"),
        /* 116. */ QT_TR_NOOP("Woodblock"),
        /* 117. */ QT_TR_NOOP("Taiko Drum"),
        /* 118. */ QT_TR_NOOP("Melodic Tom"),
        /* 119. */ QT_TR_NOOP("Synth Drum"),
        /* 120. */ QT_TR_NOOP("Reverse Cymbal"),
        /* 121. */ QT_TR_NOOP("Guitar Fret Noise"),
        /* 122. */ QT_TR_NOOP("Breath Noise"),
        /* 123. */ QT_TR_NOOP("Seashore"),
        /* 124. */ QT_TR_NOOP("Bird Tweet"),
        /* 125. */ QT_TR_NOOP("Telephone Ring"),
        /* 126. */ QT_TR_NOOP("Helicopter"),
        /* 127. */ QT_TR_NOOP("Applause"),
        /* 128. */ QT_TR_NOOP("Gunshot"),
    };

    if (program >= 0 && program < arraySize(gmInstrumentNames))
        return tr(gmInstrumentNames[program]);
    else
        return QString();
}
