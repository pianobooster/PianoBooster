/*********************************************************************************/
/*!
@file           TrackList.cpp

@brief          The Design.

@author         L. J. Barman

    Copyright (c)   2008-2009, L. J. Barman, all rights reserved

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

void CTrackList::clear()
{
    int chan;

    for (chan = 0; chan < MAX_MIDI_CHANNELS; chan++)
    {
        m_midiActiveChannels[chan] = false;
        m_midiFirstPatchChannels[chan] = -1;
        for (int i = 0; i < MAX_MIDI_NOTES; i++)
            m_noteFrequency[chan][i]=0;
    }
    m_trackQtList.clear();
}

void CTrackList::currentRowChanged(int currentRow)
{
    if (!m_song) return;
    if (currentRow >= m_trackQtList.size()|| currentRow < 0)
        return;

    m_song->setActiveChannel(m_trackQtList[currentRow].midiChannel);
}

void CTrackList::examineMidiEvent(CMidiEvent event)
{
    int chan;
    chan = event.channel();
    assert (chan < MAX_MIDI_CHANNELS && chan >= 0);
    if (chan < MAX_MIDI_CHANNELS && chan >= 0)
    {
        if (event.type() == MIDI_NOTE_ON)
        {
            m_midiActiveChannels[chan] = true;
            // count each note so we can guess the key signature
            if (event.note() >= 0 && event.note() < MAX_MIDI_NOTES)
                m_noteFrequency[chan][event.note()]++;

            // If we have a note and no patch then default to grand piano patch
            if (m_midiFirstPatchChannels[chan] == -1)
                m_midiFirstPatchChannels[chan] = GM_PIANO_PATCH;

        }

        if (event.type() == MIDI_PROGRAM_CHANGE && m_midiActiveChannels[chan] == false)
            m_midiFirstPatchChannels[chan] = event.programme();
    }
}

// Returns true if there is a piano part on channels 3 & 4
bool CTrackList::pianoPartConvetionTest()
{
    if ((m_midiFirstPatchChannels[CONVENTION_LEFT_HAND_CHANNEL] == GM_PIANO_PATCH &&
         m_midiActiveChannels[CONVENTION_LEFT_HAND_CHANNEL] == true &&
         m_midiFirstPatchChannels[CONVENTION_RIGHT_HAND_CHANNEL]  <= GM_PIANO_PATCH))
            return true;

    if (m_midiFirstPatchChannels[CONVENTION_RIGHT_HAND_CHANNEL] == GM_PIANO_PATCH &&
         m_midiActiveChannels[CONVENTION_RIGHT_HAND_CHANNEL] == true &&
         m_midiFirstPatchChannels[CONVENTION_LEFT_HAND_CHANNEL]  <= GM_PIANO_PATCH)
            return true;

    if (m_midiFirstPatchChannels[CONVENTION_LEFT_HAND_CHANNEL] == GM_PIANO_PATCH &&
        m_midiActiveChannels[CONVENTION_LEFT_HAND_CHANNEL] == true &&
        m_midiFirstPatchChannels[CONVENTION_RIGHT_HAND_CHANNEL] <= GM_PIANO_PATCH)
            return true;
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
        /*
        printf("key %2d score %3d :: ", keyLookUp[i].key, score);
        for (int j=0; j < MIDI_OCTAVE; j++)
            printf(" %d", scale[(keyLookUp[i].offset + j)%MIDI_OCTAVE]);
        printf("\n");
        */
    }
    return keySignature;
}

void CTrackList::refresh()
{
    int chan;
    int rowCount = 0;
    m_trackQtList.clear();

    for (chan = 0; chan < MAX_MIDI_CHANNELS; chan++)
    {
        if (m_midiActiveChannels[chan] == true)
        {
           CTrackListItem trackItem;
            trackItem.midiChannel =  chan;
            m_trackQtList.append(trackItem);
            rowCount++;
        }
    }
    if (pianoPartConvetionTest())
    {
        if (CNote::bothHandsChan() == -2 ) // -2 for not set -1 for not used
            CNote::setChannelHands(CONVENTION_LEFT_HAND_CHANNEL, CONVENTION_RIGHT_HAND_CHANNEL);
        m_song->setActiveChannel(CNote::bothHandsChan());
        ppLogInfo("Active both");
    }
    else
    {
        if (m_trackQtList.count() > 0)
        {
            m_song->setActiveChannel(m_trackQtList[0].midiChannel);
        }
    }

    if (CStavePos::getKeySignature() == NOT_USED)
        CStavePos::setKeySignature(guessKeySignature(CNote::rightHandChan(),CNote::leftHandChan()), 0);

    int goodChan = -1;
    // Find an unused channel that we can use for the keyboard
    for (chan = 0; chan < MAX_MIDI_CHANNELS; chan++)
    {
        if (m_midiActiveChannels[chan] == false)
        {
            if (goodChan != -1)
            {
                m_song->setPianistChannels(goodChan,chan);
                ppLogInfo("Using Pianist Channels %d + %d", goodChan +1, chan +1);
                return;
            }
            goodChan = chan;
        }
    }
    // As we have not returned we have not found to empty channels to use
    if (goodChan == -1)
        goodChan = 15 -1;
    m_song->setPianistChannels(goodChan,16-1);
}

int CTrackList::getActiveItemIndex()
{
    int chan;
    for (int i = 0; i < m_trackQtList.size(); ++i)
    {
        chan = m_trackQtList.at(i).midiChannel;
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

    for (int i = 0; i < m_trackQtList.size(); ++i)
    {
        hand.clear();
        chan = m_trackQtList.at(i).midiChannel;
        if (raw == false)
        {
            if (CNote::leftHandChan() == chan)
                hand += "L";
            if (CNote::rightHandChan() == chan)
                hand += "R";
        }
        text = QString::number(chan+1) + hand + " " + getChannelProgramName(chan);
        items += text;
    }
    return items;
}

int CTrackList::getActiveHandIndex(whichPart_t whichPart)
{
    int index = 0;
     for (int i = 0; i < m_trackQtList.size(); ++i)
        if (m_trackQtList.at(i).midiChannel == CNote::getHandChannel( whichPart))
            return index;

    return index;
}

void CTrackList::setActiveHandsIndex(int leftIndex, int rightIndex)
{
    int leftChannel = -1;
    int rightChannel = -1;

    if (leftIndex>=0)
        leftChannel = m_trackQtList.at(leftIndex).midiChannel;
    if (rightIndex>=0)
        rightChannel = m_trackQtList.at(rightIndex).midiChannel;
    m_settings->setChannelHands(leftChannel, rightChannel);
    refresh();
    m_song->rewind();
}

// get the track index number of the selected hand
int CTrackList::getHandTrackIndex(whichPart_t whichPart)
{
    int index = 0;
    int midiHand = CNote::getHandChannel(whichPart);
    for (int i = 0; i < m_trackQtList.size(); ++i)
    {

        if (m_trackQtList.at(i).midiChannel == midiHand)
            return index;
        index++;
    }

    return -1;
}

void CTrackList::changeListWidgetItemView( unsigned int index, QListWidgetItem* listWidgetItem )
{
    int chan = m_trackQtList[index].midiChannel;
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
    if(chan<0 || chan>= static_cast<int>(arraySize(m_midiFirstPatchChannels)))
    {
        assert(true);
        return QString();
    }
    int program = m_midiFirstPatchChannels[chan];

    if (chan==10-1)
        return "Drums";
    QString name = getProgramName(program +1); // Skip
    if (name.isEmpty())
        name = "Unknown";

    return name;
}

QString CTrackList::getProgramName(int program)
{
    const char * const gmInstrumentNames[] =
    {
                   "(None)",  // Don't use
        /* 1.   */ "Grand Piano",
        /* 2.   */ "Bright Piano",
        /* 3.   */ "Electric Grand",
        /* 4.   */ "Honky-tonk Piano",
        /* 5.   */ "Electric Piano 1",
        /* 6.   */ "Electric Piano 2",
        /* 7.   */ "Harpsichord",
        /* 8.   */ "Clavi",
        /* 9.   */ "Celesta",
        /* 10.  */ "Glockenspiel",
        /* 11.  */ "Music Box",
        /* 12.  */ "Vibraphone",
        /* 13.  */ "Marimba",
        /* 14.  */ "Xylophone",
        /* 15.  */ "Tubular Bells",
        /* 16.  */ "Dulcimer",
        /* 17.  */ "Drawbar Organ",
        /* 18.  */ "Percussive Organ",
        /* 19.  */ "Rock Organ",
        /* 20.  */ "Church Organ",
        /* 21.  */ "Reed Organ",
        /* 22.  */ "Accordion",
        /* 23.  */ "Harmonica",
        /* 24.  */ "Tango Accordion",
        /* 25.  */ "Acoustic Guitar (nylon)",
        /* 26.  */ "Acoustic Guitar (steel)",
        /* 27.  */ "Electric Guitar (jazz)",
        /* 28.  */ "Electric Guitar (clean)",
        /* 29.  */ "Electric Guitar (muted)",
        /* 30.  */ "Overdriven Guitar",
        /* 31.  */ "Distortion Guitar",
        /* 32.  */ "Guitar harmonics",
        /* 33.  */ "Acoustic Bass",
        /* 34.  */ "Electric Bass (finger)",
        /* 35.  */ "Electric Bass (pick)",
        /* 36.  */ "Fretless Bass",
        /* 37.  */ "Slap Bass 1",
        /* 38.  */ "Slap Bass 2",
        /* 39.  */ "Synth Bass 1",
        /* 40.  */ "Synth Bass 2",
        /* 41.  */ "Violin",
        /* 42.  */ "Viola",
        /* 43.  */ "Cello",
        /* 44.  */ "Contrabass",
        /* 45.  */ "Tremolo Strings",
        /* 46.  */ "Pizzicato Strings",
        /* 47.  */ "Orchestral Harp",
        /* 48.  */ "Timpani",
        /* 49.  */ "String Ensemble 1",
        /* 50.  */ "String Ensemble 2",
        /* 51.  */ "SynthStrings 1",
        /* 52.  */ "SynthStrings 2",
        /* 53.  */ "Choir Aahs",
        /* 54.  */ "Voice Oohs",
        /* 55.  */ "Synth Voice",
        /* 56.  */ "Orchestra Hit",
        /* 57.  */ "Trumpet",
        /* 58.  */ "Trombone",
        /* 59.  */ "Tuba",
        /* 60.  */ "Muted Trumpet",
        /* 61.  */ "French Horn",
        /* 62.  */ "Brass Section",
        /* 63.  */ "SynthBrass 1",
        /* 64.  */ "SynthBrass 2",
        /* 65.  */ "Soprano Sax",
        /* 66.  */ "Alto Sax",
        /* 67.  */ "Tenor Sax",
        /* 68.  */ "Baritone Sax",
        /* 69.  */ "Oboe",
        /* 70.  */ "English Horn",
        /* 71.  */ "Bassoon",
        /* 72.  */ "Clarinet",
        /* 73.  */ "Piccolo",
        /* 74.  */ "Flute",
        /* 75.  */ "Recorder",
        /* 76.  */ "Pan Flute",
        /* 77.  */ "Blown Bottle",
        /* 78.  */ "Shakuhachi",
        /* 79.  */ "Whistle",
        /* 80.  */ "Ocarina",
        /* 81.  */ "Lead 1 (square)",
        /* 82.  */ "Lead 2 (sawtooth)",
        /* 83.  */ "Lead 3 (calliope)",
        /* 84.  */ "Lead 4 (chiff)",
        /* 85.  */ "Lead 5 (charang)",
        /* 86.  */ "Lead 6 (voice)",
        /* 87.  */ "Lead 7 (fifths)",
        /* 88.  */ "Lead 8 (bass + lead)",
        /* 89.  */ "Pad 1 (new age)",
        /* 90.  */ "Pad 2 (warm)",
        /* 91.  */ "Pad 3 (polysynth)",
        /* 92.  */ "Pad 4 (choir)",
        /* 93.  */ "Pad 5 (bowed)",
        /* 94.  */ "Pad 6 (metallic)",
        /* 95.  */ "Pad 7 (halo)",
        /* 96.  */ "Pad 8 (sweep)",
        /* 97.  */ "FX 1 (rain)",
        /* 98.  */ "FX 2 (soundtrack)",
        /* 99.  */ "FX 3 (crystal)",
        /* 100. */ "FX 4 (atmosphere)",
        /* 101. */ "FX 5 (brightness)",
        /* 102. */ "FX 6 (goblins)",
        /* 103. */ "FX 7 (echoes)",
        /* 104. */ "FX 8 (sci-fi)",
        /* 105. */ "Sitar",
        /* 106. */ "Banjo",
        /* 107. */ "Shamisen",
        /* 108. */ "Koto",
        /* 109. */ "Kalimba",
        /* 110. */ "Bag pipe",
        /* 111. */ "Fiddle",
        /* 112. */ "Shanai",
        /* 113. */ "Tinkle Bell",
        /* 114. */ "Agogo",
        /* 115. */ "Steel Drums",
        /* 116. */ "Woodblock",
        /* 117. */ "Taiko Drum",
        /* 118. */ "Melodic Tom",
        /* 119. */ "Synth Drum",
        /* 120. */ "Reverse Cymbal",
        /* 121. */ "Guitar Fret Noise",
        /* 122. */ "Breath Noise",
        /* 123. */ "Seashore",
        /* 124. */ "Bird Tweet",
        /* 125. */ "Telephone Ring",
        /* 126. */ "Helicopter",
        /* 127. */ "Applause",
        /* 128. */ "Gunshot",
    };

    if (program >= 0 && program < static_cast<int>(arraySize(gmInstrumentNames)))
        return gmInstrumentNames[program];
    else
        return QString();
}
