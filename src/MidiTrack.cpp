/*********************************************************************************/
/*!
@file           MidiTrack.cpp

@brief          xxxx.

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

#include "MidiTrack.h"
#include "Util.h"
#include "StavePosition.h"

#include <limits>

#define OPTION_DEBUG_TRACK     1
#if OPTION_DEBUG_TRACK
#define __dt(X)        X
#else
#define __dt(X)
#endif

int CMidiTrack::m_logLevel;

CMidiTrack::CMidiTrack(fstream& file, int no) :m_file(file), m_trackNumber(no)
{
    m_trackEventQueue = nullptr;
    m_savedRunningStatus = 0;
    m_trackLengthCounter = 0;
    m_deltaTime = 0;
    m_currentTime = 0;
    midiFailReset();

    for ( int chan = 0; chan <MAX_MIDI_CHANNELS; chan++ )
    {
        m_noteOnEventPtr[chan] = nullptr;
    }

    int i;

    m_trackName.clear();
    m_trackLengthCounter = 8;
    for ( i=0; i < 4; i++)
    {
        if (m_file.get() !="MTrk"[i] )
        {
            ppLogError("No valid MIDI tracks");
            errorFail(SMF_CORRUPTED_MIDI_FILE);
            return;
        }
    }
    m_trackLengthCounter = readDWord();
    __dt(ppDebugTrack(9, "Track Length %d", m_trackLengthCounter));

    m_filePos = m_file.tellg();
    m_trackLength = m_trackLengthCounter + 8; // 4 bytes for the "MTrk" + 4 bytes for the track length
    if (m_trackLength > static_cast<dword_t>(std::numeric_limits<int>::max())) {
        ppLogError("The track length is too big.");
        errorFail(SMF_ERROR_TOO_MANY_TRACK);
        return;
    }
    m_trackEventQueue = new CQueue<CMidiEvent>(static_cast<int>(m_trackLength/3ul)); // The minimum bytes per event is 3
}

void CMidiTrack::ppDebugTrack(int level, const char *msg, ...)
{
    va_list ap;

    if (level <m_logLevel)
        return;

    fprintf(stdout, "Dbug: Track %d length %5lu: ", m_trackNumber , m_trackLengthCounter);
    va_start(ap, msg);
    vfprintf(stdout, msg, ap);
    va_end(ap);
    fputc('\n', stdout);
}

dword_t CMidiTrack::readVarLen()
{
    dword_t value;
    unsigned char c;
    int i;

    if ((value = readByte()) & 0x80)
    {
        value &= 0x7F;
        for (i =0; i < 4; i++)
        {
            value = ( value << 7 ) + (( c = readByte()) & 0x7F );
            if (failed() == true)
                break;
            if ((c & 0x80) == 0)
                break;
            else if (i>=3)
                errorFail(SMF_END_OF_FILE);
        }
    }
#if OPTION_DEBUG_TRACK
    if (value > 400)
        __dt(ppDebugTrack(2,"Large variable length data %d", value));
#endif
    return ( value );
}

string CMidiTrack::readTextEvent()
{
    dword_t length;

    string text;
    length = readVarLen();
    if (length >= 1000)
    {
        ppLogError("Text Event too large %d", length);
        errorFail(SMF_END_OF_FILE);
        return text;
    }
    while (length--)
    {
        if (failed() == true)
            return text;
        text += static_cast<char>(readByte());
    }
    return text;
}

dword_t CMidiTrack::readDataEvent(int expectedLength)
{
    int length;

    dword_t data;
    length = readByte();
    if (length != expectedLength)
    {
        errorFail(SMF_CORRUPTED_MIDI_FILE);
        return 0;
    }
    data = 0;
    while (length--)
    {
        data <<= 8;
        data |= readByte();
    }
    return data;
}

void  CMidiTrack::ignoreSysexEvent(byte_t data)
{
    word_t length;

    if (data >= 127)
    {
        // this could be a variable length word
        ppLogWarn("SysexEvent, is too long %d", data);
        errorFail(SMF_CORRUPTED_MIDI_FILE);
        return;
    }
    else
        length = data;

    while (length-- > 0 )
    {
        if (failed() == true)
            return;
        readByte();
    }
}

/* Time Signature */
void CMidiTrack::readTimeSignatureEvent()
{
    byte_t timeSigNumerator;
    byte_t timeSigDenominator;
    CMidiEvent event;
    byte_t b3, b4;

    const auto len = readVarLen();
    if (len!=4)
    {
        errorFail(SMF_CORRUPTED_MIDI_FILE);
        return;
    }
    timeSigNumerator = readByte();  // The number on the top
    timeSigDenominator = readByte(); // the number on the bottom
    if (timeSigDenominator >= 5)
    {
        errorFail(SMF_CORRUPTED_MIDI_FILE);
        return;
    }
    if (timeSigNumerator > 20)
    {
        errorFail(SMF_CORRUPTED_MIDI_FILE);
        return;
    }
    //len = (1<<timeSigDenominator);

    b3 = readByte();           /* Ignore the last bytes */
    b4 = readByte();           /* Ignore the last bytes */
    event.metaEvent(readDelaTime(), MIDI_PB_timeSignature, timeSigNumerator, 1<<timeSigDenominator);
    m_trackEventQueue->push(event);
    __dt(ppDebugTrack(4,"Key Signature %d/%d metronome %d quarter %d", timeSigNumerator, 1<<timeSigDenominator, b3, b4));
}

/* Key Signature */
void CMidiTrack::readKeySignatureEvent()
{
    CMidiEvent event;
    int keySig;
    int majorKey;

    const auto len = readVarLen();
    if (len!=2)
    {
        errorFail(SMF_CORRUPTED_MIDI_FILE);
        return;
    }
    keySig = static_cast<char>(readByte());  // force sign conversion The key sig 0=middle C
    majorKey =readByte(); // Major or Minor
    if (keySig > 7 || keySig < -7 )
    {
        errorFail(SMF_CORRUPTED_MIDI_FILE);
        return;
    }

    event.metaEvent(readDelaTime(), MIDI_PB_keySignature, keySig, majorKey);
    m_trackEventQueue->push(event);
    __dt(ppDebugTrack(4,"Key Signature %d maj/min %d", keySig, majorKey));
    if (CStavePos::getKeySignature() == NOT_USED)
        CStavePos::setKeySignature(event.data1(), event.data2());
}

void CMidiTrack::readMetaEvent(byte_t type)
{
    string text;
    __dt(dword_t data);

    if (failed() == true)
        return;

    CMidiEvent event;

    switch (type)
    {
    case METASEQN:                     /* Sequence Number */
        __dt(data = readDataEvent(2));
        __dt(ppDebugTrack(3,"Sequence Number %lu", data));
        break;

    case METACOPYR:                             /* Copyright Notice */
        text = readTextEvent();
        __dt(ppDebugTrack(3,"Copyright %s", text.c_str()));
        break;

    case METATNAME:                          /* Seq/Track Name */
        text = readTextEvent();
        m_trackName = QString(text.c_str());
        __dt(ppDebugTrack(3,"Seq/Track Name %s", text.c_str()));
        break;

    case METAINAME:                          /* Instrument */
        text = readTextEvent();
        __dt(ppDebugTrack(2,"Instrument %s", text.c_str()));
        break;

    case METAEOT:                         /* End of Track */
        readVarLen();
        __dt(ppDebugTrack(2,"End of Track"));
        break;

    case METATEMPO:                        /* Set Tempo */
    {
        auto b1 = readVarLen();
        if (b1 != 3)
        {
            errorFail(SMF_CORRUPTED_MIDI_FILE);
            break;
        }
        b1 = readByte();
        const auto b2 = readByte();
        const auto b3 = readByte();
        const auto tempo = b1 << 16 | b2 << 8 | b3; // microseconds per quarter-note#
        event.metaEvent(readDelaTime(), MIDI_PB_tempo, static_cast<int>(tempo), 0);
        m_trackEventQueue->push(event);
        __dt(ppDebugTrack(2,"Set Tempo %d", tempo));
        break;
    }

    case METATIMESIG:                        /* Time Signature */
        readTimeSignatureEvent();
        break;

    case METAKEYSIG:                        /* Key Signature */
        readKeySignatureEvent();
        break;

    case METATEXT:                      /* Text Event */
        text = readTextEvent();
        __dt(ppDebugTrack(2,"Text %s", text.c_str()));
        break;

    case METALYRIC:                         /* Lyric */
        text = readTextEvent();
        __dt(ppDebugTrack(2,"Lyric %s", text.c_str()));
        break;

    case METAMARKER:
        text = readTextEvent();
        __dt(ppDebugTrack(2,"METAMARKER %s", text.c_str()));
        break;

    case METACUEPT:
        text = readTextEvent();
        __dt(ppDebugTrack(2,"METACUEPT %s", text.c_str()));
        break;

    case METACHANPFX:                         /* MIDI Channel Prefix */
        __dt(data = readDataEvent(1));
        __dt(ppDebugTrack(2,"MIDI Channel Prefix %lu", data));
        break;

    case METASMPTEOFF:                        /* SMPTE Offset */
        text = readTextEvent();
        __dt(ppDebugTrack(2,"SMPTE Offset %s", text.c_str()));
        break;

    case METASEQEVENT:                        /* sequencer Specific */
        text = readTextEvent();
        __dt(ppDebugTrack(2,"sequencer Specific %s", text.c_str()));
        break;

    case 0x21:                         /* MIDI Port */
        data = readDataEvent(1);
        __dt(ppDebugTrack(2,"MIDI Port %lu", data));
        break;

    case 0x09:                          // meta type
        text = readTextEvent();
        __dt(ppDebugTrack(2,"midi meta event 0x%02x %s", type, text.c_str()));
        break;
    default:                          /* Unknown meta type */
        text = readTextEvent();
        ppLogWarn("unknown midi meta event 0x%02x %s", type, text.c_str());
        //errorFail(SMF_UNKNOW_EVENT);
        break;
    }
}

void CMidiTrack::decodeSystemMessage( byte_t status, byte_t data1 )
{
    switch ( status )
    {
    case MIDI_SYSEXEVENT:    /* System exclusive Transmitted */
    case 0xf7:  /* System exclusive Not transmitted */
        __dt(ppDebugTrack(2,"SYSEXEVENT xx"));
        /*ignore_sysex_event();*/
        ignoreSysexEvent(data1);
        break;

    case METAEVENT:
        readMetaEvent(data1);
        break;

    default:
        __dt(ppDebugTrack(99,"UNKNOWN"));
        ignoreSysexEvent(data1);
        errorFail(SMF_UNKNOW_EVENT);
        break;
    }
}

void CMidiTrack::noteOffEvent(CMidiEvent &event, int deltaTime, int channel, int pitch, int velocity)
{
    createNoteEventPtr(channel);

    CMidiEvent* noteOnEventPtr = m_noteOnEventPtr[channel][pitch];

    if (noteOnEventPtr)
    {
        int duration = m_currentTime - noteOnEventPtr->getDuration();
        noteOnEventPtr->setDuration(duration);
        //ppLogDebug ("NOTE OFF chan %d pitch %d  currentTime %d Duration %d", channel + 1, pitch, m_currentTime, duration);
    }
    else
    {
        ppLogWarn("Missing note off duration Chan %d Note off %d", channel + 1, pitch);
    }
    m_noteOnEventPtr[channel][pitch] = nullptr;

    event.noteOffEvent(deltaTime, channel, pitch, velocity);

    m_trackEventQueue->push(event);
    __dt(ppDebugTrack(1,"Chan %d Note off %d", channel + 1, pitch));
}

void CMidiTrack::decodeMidiEvent()
{
    CMidiEvent event;
    byte_t status, data1, data2;
    int channel;

    const auto readDeltaTicks = readVarLen();
    if (readDeltaTicks > std::numeric_limits<int>::max()) {
        errorFail(SMF_CORRUPTED_MIDI_FILE);
        return;
    }
    const auto deltaTicks = static_cast<int>(readDeltaTicks);
    m_deltaTime += deltaTicks;
    m_currentTime += deltaTicks;

    const auto c = readByte();
    if ((c & 0x80) == 0 )
    {
        status = m_savedRunningStatus;
        data1 = c;
    }
    else
    {
        m_savedRunningStatus = status = c;
        data1=readByte();
        if ((data1 & 0x80) != 0) {
            errorFail(SMF_CORRUPTED_MIDI_FILE);
            return;
        }
    }

    channel = status & 0x0f;

    switch ( status & 0xf0 )
    {
    case MIDI_NOTE_OFF:              /* Note off */
        data2 = readByte();
        noteOffEvent(event, readDelaTime(), channel, data1, data2);
        break;

    case MIDI_NOTE_ON:             /* Note on */
        data2 = readByte();
        if (data2 != 0 )
        {
            event.noteOnEvent(readDelaTime(), channel, data1, data2);
            __dt(ppDebugTrack(1,"Chan %d note on %d",channel + 1, data1));

            event.setDuration(m_currentTime); // Set the duration to the current time for now
            //ppLogDebug ("NOTE ON  pitch %d m_currentTime %d event->getDuration() %d", data1, m_currentTime, event.getDuration());

            CMidiEvent* eventPtr = m_trackEventQueue->push(event);

            // Save a reference to the note on event
            createNoteEventPtr(channel);
            m_noteOnEventPtr[channel] [data1]  = eventPtr;
        }
        else
        {
            noteOffEvent(event, readDelaTime(), channel, data1, 0);
        }
        break;

    case MIDI_NOTE_PRESSURE :              /* Key pressure After touch (POLY_AFTERTOUCH)  3 bytes */
        data2 = readByte();
        event.notePressure(readDelaTime(), channel, data1, data2);
        m_trackEventQueue->push(event);
        __dt(ppDebugTrack(2,"Chan %d After touch", channel + 1));
        break;

    case MIDI_PROGRAM_CHANGE :               /* program change */
        event.programChangeEvent(readDelaTime(), channel, data1);
        m_trackEventQueue->push(event);
        __dt(ppDebugTrack(2,"Chan %d Program change %d", channel + 1, data1 + 1));
        break;

    case MIDI_CONTROL_CHANGE :               /* Control Change */
        data2 = readByte();
        event.controlChangeEvent(readDelaTime(), channel, data1, data2);
        m_trackEventQueue->push(event);
        __dt(ppDebugTrack(2,"Chan %d Control Change %d %d", channel + 1, data1, data2));
        break;

    case MIDI_CHANNEL_PRESSURE:            /* Channel Pressure (AFTERTOUCH)*/
        event.channelPressure(readDelaTime(), channel, data1);
        m_trackEventQueue->push(event);
        __dt(ppDebugTrack(2,"Chan %d Channel Pressure", channel + 1));
        break;

    case MIDI_PITCH_BEND:    /* Pitch bend */
        data2 = readByte();
        event.pitchBendEvent(readDelaTime(), channel, data1, data2);
        m_trackEventQueue->push(event);
        __dt(ppDebugTrack(2,"Chan %d Pitch bend",channel + 1));
        break;

    case MIDI_SYSEXEVENT :           /* System EX */
        decodeSystemMessage(status, data1);
        m_savedRunningStatus=0;
        break;
    }
}

void CMidiTrack::decodeTrack()
{
    CMidiEvent event;

    m_file.seekg(m_filePos, ios::beg);
    while (true)
    {
        if (m_trackLengthCounter== 0)
            break;
        if (m_trackEventQueue->space() <= 1)
        {
            ppLogError("Out of Space");
            break;
        }
        decodeMidiEvent();
        if (failed() == true)
            break;
    }
    m_filePos = m_file.tellg();
}
