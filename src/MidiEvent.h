/*********************************************************************************/
/*!
@file           MidiEvent.h

@brief          xxxxx.

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

#ifndef __MIDI_EVENT_H__
#define __MIDI_EVENT_H__

#include <string>
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

#define GM_PIANO_PATCH        0 // The default grand piano sound
#define isPianoOrOrganPatch(p)   (((p) >= 0 && (p) <=7) || ((p) >= 16 && (p) <= 23 ))

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
        m_duration = 0;
    }

    int deltaTime(){return m_deltaTime;}
    void addDeltaTime(int delta){m_deltaTime +=delta;}
    void subtractDeltaTime(int delta){m_deltaTime -=delta;}
    void setDeltaTime(int delta){m_deltaTime = delta;}

    ////////////////////////////////////////////////////////////////////////////////
    //@brief returns the midi note number
    int note() const {return m_note;}
    void setNote(int note){m_note = note;}
    int programme() const {return m_note;}
    int channel() const {return m_channel;} // can also contain the track number
    void setChannel(int chan){m_channel = chan;}
    int velocity() const {return m_velocity;}
    void setVelocity(int value) {m_velocity = value;}
    int type() const {return m_type;}
    void setType(int type){m_type = type;}
    void transpose(int amount) {m_note += amount;}
    int data1() const {return m_note;} // Meta data is stored here
    int data2() const {return m_velocity;}
    void setDatat2(int value) {m_velocity = value;}
    void setTrack(int track) {m_track = track;}
    int track() {return m_track;}

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

    ////////////////////////////////////////////////////////////////////////////////
    //@brief set the midi note duration
    void setDuration(int duration) {m_duration = duration;}

    ////////////////////////////////////////////////////////////////////////////////
    //@brief how long the midi note was played for
    int getDuration(){return m_duration;}

    /**
     * This merges two MidiEvents (this and the other MidiEvent)
     * The MidiEvent that happens first (has the lowest delta time)
     * then a COPY of that MidiEvent is returned.
     * The original is then cleared so that  it type is set to MIDI_NONE
     * The MidiEvent that happens second will have it delta time reduced
     * using the delta time from the first MidiEvent.
     * NOTE Both this and the otherMidiEvent will be altered when call this method.
     */
    CMidiEvent getNextMergedEvent(CMidiEvent &otherMidiEvent)
    {
        CMidiEvent result;
        int thisDelta = deltaTime();
        int otherDelta = otherMidiEvent.deltaTime();
        if ( thisDelta <= otherDelta ) {
            result = *this;
            otherMidiEvent.subtractDeltaTime(thisDelta);
            clear();
        } else {
            result = otherMidiEvent;
            subtractDeltaTime(otherDelta);
            otherMidiEvent.clear();
        }
        return result;
    }

    std::string event_type_str(int atype)
       {
         std::string r;
         switch (atype) {
         case MIDI_NOTE_OFF:             r = "MIDI_NOTE_OFF";break;
         case MIDI_NOTE_ON:              r = "MIDI_NOTE_ON";break;
         case MIDI_NOTE_PRESSURE:        r = "MIDI_NOTE_PRESSURE";break;
         case MIDI_CONTROL_CHANGE:       r = "MIDI_CONTROL_CHANGE";break;
         case MIDI_PROGRAM_CHANGE:       r = "MIDI_PROGRAM_CHANGE";break;
         case MIDI_CHANNEL_PRESSURE:     r = "MIDI_CHANNEL_PRESSURE";break;
         case MIDI_PITCH_BEND:           r = "MIDI_PITCH_BEND";break;
         case MIDI_SYSTEM_EVENT:         r = "MIDI_SYSTEM_EVENT";break;
         case MIDI_SUSTAIN:              r = "MIDI_SUSTAIN";break;
         case MIDI_MAIN_VOLUME:          r = "MIDI_MAIN_VOLUME";break;
         case MIDI_RESET_ALL_CONTROLLERS:r = "MIDI_RESET_ALL_CONTROLLERS";break;
         case MIDI_ALL_SOUND_OFF:        r = "MIDI_ALL_SOUND_OFF";break;
         case MIDI_ALL_NOTES_OFF:        r = "MIDI_ALL_NOTES_OFF";break;

         case MIDI_NONE:                 r = "MIDI_NONE";break;
         case MIDI_ERROR:                r = "MIDI_ERROR";break;
         case MIDI_PB_EOF:               r = "MIDI_PB_EOF";break;
         case MIDI_PB_chordSeparator:    r = "MIDI_PB_chordSeparator";break;
         case MIDI_PB_tempo:             r = "MIDI_PB_tempo";break;
         case MIDI_PB_timeSignature:     r = "MIDI_PB_timeSignature";break;
         case MIDI_PB_keySignature:      r = "MIDI_PB_keySignature";break;
         case MIDI_PB_collateRawMidiData:r = "MIDI_PB_collateRawMidiData";break;
         case MIDI_PB_outputRawMidiData: r = "MIDI_PB_outputRawMidiData";break;

           //      case MIDI_SYSEXEVENT:           r = "MIDI_SYSEXEVENT";break;
         case METAEVENT:                 r = "METAEVENT";break;
         case METASEQN:                  r = "METASEQN";break;
         case METATEXT:                  r = "METATEXT";break;
         case METACOPYR:                 r = "METACOPYR";break;
         case METATNAME:                 r = "METATNAME";break;
         case METAINAME:                 r = "METAINAME";break;
         case METALYRIC:                 r = "METALYRIC";break;
         case METAMARKER:                r = "METAMARKER";break;
           //      case METACUEPT:                 r = "METACUEPT";break;
         case METACHANPFX:               r = "METACHANPFX";break;
         case METAEOT:                   r = "METAEOT";break;
         case METATEMPO:                 r = "METATEMPO";break;
         case METASMPTEOFF:              r = "METASMPTEOFF";break;
         case METATIMESIG:               r = "METATIMESIG";break;
         case METAKEYSIG:                r = "METAKEYSIG";break;
         case METASEQEVENT:              r = "METASEQEVENT";break;

         default:
           {
             char s[16];
             sprintf(s, "%2x", atype);
             r = s;
           }
         }
         return r;
       }

    void printDetails()
    {
        if (type() == MIDI_NOTE_ON) {
            ppTiming("chan %2d NOTE ON  note %3d vel %3d", channel(), note(), velocity() );
        }
        else if (type() == MIDI_NOTE_OFF) {
            ppTiming("chan %2d NOTE OFF note %3d vel %3d", channel(), note(), velocity() );
        }
        else {
            ppTiming("chan %2d type %-20s note %3d velocity %3d deltaTime %3d",
                      channel(), event_type_str(type()).c_str(), note(), velocity(), deltaTime());
        }
   }

private:
    int m_type;
    int m_deltaTime;
    int m_channel;
    int m_note;
    int m_velocity;
    int m_duration;
    int m_track;
};

#endif //__MIDI_EVENT_H__
