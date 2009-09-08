/*********************************************************************************/
/*!
@file           MidiEvent.h

@brief          xxxxx.

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

#ifndef __MIDI_EVENT_H__
#define __MIDI_EVENT_H__

#include "Util.h"

#define MIDI_NOTE_OFF               0x80    /* MIDI voice messages */
#define MIDI_NOTE_ON                0x90
#define MIDI_NOTE_PRESSURE          0xA0   //POLY_AFTERTOUCH: 3 bytes
#define MIDI_CONTROL_CHANGE         0xB0
#define MIDI_PROGRAM_CHANGE         0xC0
#define MIDI_CHANNEL_PRESSURE       0xD0    //AFTERTOUCH: 2 bytes
#define MIDI_PITCH_BEND             0xE0
#define MIDI_SYSTEM_EVENT           0xF0
#define MIDI_SUSTAIN                0x40
#define MIDI_MAIN_VOLUME            0x07
#define MIDI_RESET_ALL_CONTROLLERS  121
#define MIDI_ALL_SOUND_OFF          120
#define MIDI_ALL_NOTES_OFF          123 //0x7B  channel mode message


// now define some of our own events
#define MIDI_NONE                   0x0ff0
#define MIDI_ERROR                  0x0ff1
#define MIDI_PB_EOF                 0x0ff2
#define MIDI_PB_chordSeparator      0x0ff3  // All the notes (note on) between these counts as one chord
#define MIDI_PB_tempo               0x0ff4
#define MIDI_PB_timeSignature       0x0ff5
#define MIDI_PB_keySignature        0x0ff6
#define MIDI_PB_collateRawMidiData  0x0ff7
#define MIDI_PB_outputRawMidiData   0x0ff8  // Raw data is used for used for a SYSTEM_EVENT

/*===================================*/
/*                                   */
/* Standard MIDI file events         */
/*                                   */
/*===================================*/
#define MIDI_SYSEXEVENT         0xF0
#define METAEVENT               0xFF
#define METASEQN                0
#define METATEXT                1
#define METACOPYR               2
#define METATNAME               3
#define METAINAME               4
#define METALYRIC               5
#define METAMARKER              6
#define METACUEPT               7
#define METACHANPFX             0x20
#define METAEOT                 0x2F
#define METATEMPO               0x51
#define METASMPTEOFF            0x54
#define METATIMESIG             0x58
#define METAKEYSIG              0x59
#define METASEQEVENT            0x7F



#define MIDI_DRUM_CHANNEL       (10-1)
#define MAX_MIDI_CHANNELS       16      // There are always at most 16 midi channels

#define GM_PIANO_PATCH        0 // The default grand piano sound


/*!
 * @brief   xxxxx.
 */
class CMidiEvent
{
public:

    CMidiEvent()
    {
        clear();
    }

    void clear()
    {
        m_type = MIDI_NONE;
        m_deltaTime = 0;
        m_channel = 0;
        m_note = 0;
        m_velocity = 0;
    }

    int deltaTime(){return m_deltaTime;}
    void addDeltaTime(int delta){m_deltaTime +=delta;}
    void setDeltaTime(int delta){m_deltaTime = delta;}

    ////////////////////////////////////////////////////////////////////////////////
    //@brief returns the midi note number
    int note() const {return m_note;}
    int programme() const {return m_note;}
    int channel() const {return m_channel;}
    void setChannel(int chan){m_channel = chan;}
    int velocity() const {return m_velocity;}
    int type() const {return m_type;}
    void setType(int type){m_type = type;}
    void transpose(int amount) {m_note += amount;}
    int data1() const {return m_note;} // Meta data is stored here
    int data2() const {return m_velocity;}
    void setDatat2(int value) {m_velocity = value;}

    void noteOffEvent( int deltaTime, int channel, int note, int velocity)
    {
        m_type = MIDI_NOTE_OFF;
        m_deltaTime = deltaTime;
        m_channel = channel;
        m_note = note;
        m_velocity = velocity;
    }

    void noteOnEvent( int deltaTime, int channel, int note, int velocity)
    {
        m_type = MIDI_NOTE_ON;
        m_deltaTime = deltaTime;
        m_channel = channel;
        m_note = note;
        m_velocity = velocity;
    }

    void notePressure( int deltaTime, int channel, int data1, int data2)
    {
        m_type = MIDI_NOTE_PRESSURE; //POLY_AFTERTOUCH: 3 bytes
        m_deltaTime = deltaTime;
        m_channel = channel;
        m_note = data1;
        m_velocity = data2;
    }

    void programChangeEvent( int deltaTime, int channel, int program)
    {
        m_type = MIDI_PROGRAM_CHANGE;
        m_deltaTime = deltaTime;
        m_channel = channel;
        m_note = program;
        m_velocity = 0;
    }

    void controlChangeEvent( int deltaTime, int channel, int data1, int data2)
    {
        m_type = MIDI_CONTROL_CHANGE;
        m_deltaTime = deltaTime;
        m_channel = channel;
        m_note = data1;
        m_velocity = data2;
    }

    void channelPressure( int deltaTime, int channel, int data1)
    {
        m_type = MIDI_CHANNEL_PRESSURE; //AFTERTOUCH: 2 bytes
        m_deltaTime = deltaTime;
        m_channel = channel;
        m_note = data1;
        m_velocity = 0;
    }

    void pitchBendEvent( int deltaTime, int channel, int data1, int data2)
    {
        m_type = MIDI_PITCH_BEND;
        m_deltaTime = deltaTime;
        m_channel = channel;
        m_note = data1;
        m_velocity = data2;
    }

    void chordSeparator(CMidiEvent &event)
    {
        m_type = MIDI_PB_chordSeparator;
        m_note = 0;
        m_channel = event.channel();
        m_deltaTime = 0;
        m_velocity = 0;
    }

    void metaEvent( int deltaTime, int type, int data1, int data2)
    {
        m_type = type;
        m_deltaTime = deltaTime;
        m_channel = 0;
        m_note = data1;
        m_velocity = data2;
    }

    // Raw data is used for used for a SYSTEM_EVENT
    void collateRawByte( int deltaTime, int nextByte)
    {
        m_type = MIDI_PB_collateRawMidiData;
        m_deltaTime = deltaTime;
        m_note = nextByte;
        m_velocity = 0;
    }

    // Raw data is used for used for a SYSTEM_EVENT
    void outputCollatedRawBytes(int deltaTime)
    {
        m_type = MIDI_PB_outputRawMidiData;
        m_deltaTime = deltaTime;
        m_note = 0;
        m_velocity = 0;
    }

    void printDetails()
    {
        ppTiming("chan %2d type %2X note %3d", channel(), type(), note() );
    }

private:
    int m_type;
    int m_deltaTime;
    int m_channel;
    int m_note;
    int m_velocity;
};


#endif //__MIDI_EVENT_H__
