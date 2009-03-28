/*********************************************************************************/
/*!
@file           MidiTrack.cpp

@brief          xxxx.

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

#include <stdarg.h>
#include "MidiTrack.h"
#include "Util.h"

#define OPTION_DEBUG_TRACK     1
#if OPTION_DEBUG_TRACK
#define ppDEBUG_TRACK(args)     ppDebugTrack args
#else
#define ppDEBUG_TRACK(args)
#endif

int CMidiTrack::m_logLevel;

void CMidiTrack::ppDebugTrack(int level, const char *msg, ...)
{
    va_list ap;

    if (level <m_logLevel)
        return;

    fprintf(stdout, "Track %d length %5lu: ", m_trackNumber , m_trackLength);
    va_start(ap, msg);
    vfprintf(stdout, msg, ap);
    va_end(ap);
    fputc('\n', stdout);
}


dword_t CMidiTrack::init()
{
    int i;

    m_trackName.clear();
    m_trackLength = 8;
    for ( i=0; i < 4; i++)
    {
        if (m_file.get() !="MTrk"[i] )
        {
            ppError("No valid Midi tracks");
            errorFail(SMF_CORRUPTED_MIDI_FILE);
            return 0;
        }
    }
    m_trackLength = readDWord();
    ppDEBUG_TRACK((9, "Track Length %d", m_trackLength));

    m_filePos = m_file.tellg();
    return m_trackLength + 8; // 4 bytes for the "MTrk" + 4 bytes for the track length
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
        ppDEBUG_TRACK((2,"Large variable length data %d", value));
#endif
    return ( value );
}

string CMidiTrack::readTextEvent()
{
    dword_t length;

    string text;
    length = readVarLen();
    if (length >= 100)
    {
        ppError("Text Event too large %d", length);
        errorFail(SMF_END_OF_FILE);
        return text;
    }
    while (length--)
    {
        if (failed() == true)
            return text;
        text += readByte();
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
    byte_t len;
    byte_t timeSigNumerator;
    byte_t timeSigDenominator;
    CMidiEvent event;
    byte_t b3, b4;

    len = readVarLen();
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
    len = (1<<timeSigDenominator);

    b3 = readByte();           /* Ignore the last bytes */
    b4 = readByte();           /* Ignore the last bytes */
    event.metaEvent(readDelaTime(), MIDI_PB_timeSignature, timeSigNumerator, 1<<timeSigDenominator);
    m_trackEventQueue->push(event);
    ppDEBUG_TRACK((4,"Time Signature %d/%d metronome %d quarter %d", timeSigNumerator, 1<<timeSigDenominator, b3, b4));
}


void CMidiTrack::readMetaEvent(byte_t type)
{
    string text;
    dword_t data;

    if (failed() == true)
        return;

    CMidiEvent event;

    switch (type)
    {
    case METASEQN:                     /* Sequence Number */
        data = readDataEvent(2);
        ppDEBUG_TRACK((3,"Sequence Number %lu", data));
        break;

    case METACOPYR:                             /* Copyright Notice */
        text = readTextEvent();
        ppDEBUG_TRACK((3,"Copyright %s", text.c_str()));
        break;

    case METATNAME:                          /* Seq/Track Name */
        text = readTextEvent();
        m_trackName = QString(text.c_str());
        ppDEBUG_TRACK((3,"Seq/Track Name %s", text.c_str()));
        break;

    case METAINAME:                          /* Instrument */
        text = readTextEvent();
        ppDEBUG_TRACK((2,"Instrument %s", text.c_str()));
        break;

    case METAEOT:                         /* End of Track */
        readVarLen();
        ppDEBUG_TRACK((2,"End of Track"));
        break;

    case METATEMPO:                        /* Set Tempo */
    {
        register byte b1,b2, b3;
        int tempo;

        b1 = readVarLen();
        if (b1 != 3)
        {
            errorFail(SMF_CORRUPTED_MIDI_FILE);
            break;
        }
        b1 = readByte();
        b2 = readByte();
        b3 = readByte();
        tempo = b1 << 16 | b2 << 8 | b3; // microseconds per quarter-note#
        event.metaEvent(readDelaTime(), MIDI_PB_tempo, tempo, 0);
        m_trackEventQueue->push(event);
        ppDEBUG_TRACK((2,"Set Tempo %d", tempo));
        break;
    }

    case METATIMESIG:                        /* Time Signature */
        readTimeSignatureEvent();
        break;

    case METAKEYSIG:                        /* Key Signature */
        {
            data = readDataEvent(2);
            char key = data>>8;
            ppDEBUG_TRACK((4,"Key Signature %d %s", (int)key, ((data&0xff) ==0)?"Major":"Minor"));
            break;
        }

    case METATEXT:                      /* Text Event */
        text = readTextEvent();
        ppDEBUG_TRACK((2,"Text %s", text.c_str()));
        break;

    case METALYRIC:                         /* Lyric */
        text = readTextEvent();
        ppDEBUG_TRACK((2,"Lyric %s", text.c_str()));
        break;

    case METAMARKER:
        text = readTextEvent();
        ppDEBUG_TRACK((2,"METAMARKER %s", text.c_str()));
        break;

    case METACUEPT:
        text = readTextEvent();
        ppDEBUG_TRACK((2,"METACUEPT %s", text.c_str()));
        break;

    case METACHANPFX:                         /* Midi Channel Prefix */
        data = readDataEvent(1);
        ppDEBUG_TRACK((2,"MIDI Channel Prefix %lu", data));
        break;

    case METASMPTEOFF:                        /* SMPTE Offset */
        text = readTextEvent();
        ppDEBUG_TRACK((2,"SMPTE Offset %s", text.c_str()));
        break;

    case METASEQEVENT:                        /* sequencer Specific */
        text = readTextEvent();
        ppDEBUG_TRACK((2,"sequencer Specific %s", text.c_str()));
        break;

    case 0x21:                         /* MIDI Port */
        data = readDataEvent(1);
        ppDEBUG_TRACK((2,"MIDI Port %lu", data));
        break;

    default:                          /* Unknown meta type */
        text = readTextEvent();
        //strict_error("SM9");
        ppDEBUG_TRACK((99,"UNKNOWN meta event 0x%02x %s", type, text.c_str()));
        errorFail(SMF_UNKNOW_EVENT);
        break;
    }
}


void CMidiTrack::decodeSystemMessage( byte_t status, byte_t data1 )
{
    switch ( status )
    {
    case MIDI_SYSEXEVENT:    /* System exclusive Transmitted */
    case 0xf7:  /* System exclusive Not transmitted */
        ppDEBUG_TRACK((2,"SYSEXEVENT xx"));
        //read_sysex_event(); // fixme was
        ignoreSysexEvent(data1);
        /*ignore_sysex_event();*/
        break;

    case METAEVENT:
        readMetaEvent(data1);
        break;

    default:
        ppDEBUG_TRACK((99,"UNKNOWN"));
        ignoreSysexEvent(data1);
        //strict_error("SM8");
        //read_sysex_event();
        errorFail(SMF_UNKNOW_EVENT);
        break;
    }
}


void CMidiTrack::decodeMidiEvent()
{
    CMidiEvent event;
    byte_t c;
    byte_t status, data1, data2;
    int channel;

    m_deltaTime += readVarLen();

    c = readByte();
    if ((c & 0x80) == 0 )
    {
        status = m_savedRunningStatus;
        data1 = c;
    }
    else
    {
        status = c;
        m_savedRunningStatus = status;
        data1=readByte();
    }

    channel = status & 0x0f;

    switch ( status & 0xf0 )
    {
    case MIDI_NOTE_OFF:              /* Note off */
        data2 = readByte();
        event.noteOffEvent(readDelaTime(), channel, data1, data2);
        m_trackEventQueue->push(event);
        ppDEBUG_TRACK((1,"Chan %d Note off", channel + 1));
        break;

    case MIDI_NOTE_ON:             /* Note on */
        data2 = readByte();
        if (data2 != 0 )
        {
            event.noteOnEvent(readDelaTime(), channel, data1, data2);
            ppDEBUG_TRACK((1,"Chan %d note on %d",channel + 1, data1));
        }
        else
        {
            event.noteOffEvent(readDelaTime(),channel, data1, 0);
            ppDEBUG_TRACK((1,"Chan %d note OFF %d",channel + 1, data1));
        }
        m_trackEventQueue->push(event);
        break;

    case MIDI_NOTE_PRESSURE :              /* Key pressure After touch (POLY_AFTERTOUCH)  3 bytes */
        data2 = readByte();
        event.notePressure(readDelaTime(), channel, data1, data2);
        m_trackEventQueue->push(event);
        ppDEBUG_TRACK((2,"Chan %d After touch", channel + 1));
        break;

    case MIDI_PROGRAM_CHANGE :               /* program change */
        event.programChangeEvent(readDelaTime(), channel, data1);
        m_trackEventQueue->push(event);
        ppDEBUG_TRACK((2,"Chan %d Program change %d", channel + 1, data1 + 1));
        break;

    case MIDI_CONTROL_CHANGE :               /* Control Change */
        data2 = readByte();
        event.controlChangeEvent(readDelaTime(), channel, data1, data2);
        m_trackEventQueue->push(event);
        ppDEBUG_TRACK((2,"Chan %d Control Change %d %d", channel + 1, data1, data2));
        break;

    case MIDI_CHANNEL_PRESSURE:            /* Channel Pressure (AFTERTOUCH)*/
        event.channelPressure(readDelaTime(), channel, data1);
        m_trackEventQueue->push(event);
        ppDEBUG_TRACK((2,"Chan %d Channel Pressure", channel + 1));
        break;

    case MIDI_PITCH_BEND:    /* Pitch bend */
        data2 = readByte();
        event.pitchBendEvent(readDelaTime(), channel, data1, data2);
        m_trackEventQueue->push(event);
        ppDEBUG_TRACK((2,"Chan %d Pitch bend",channel + 1));
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
        if (m_trackLength== 0)
            break;
        if (m_trackEventQueue->space() <= 1)
        {
            ppError("Out of Space");
            break;
        }
        decodeMidiEvent();
        if (failed() == true)
            break;
    }
    m_filePos = m_file.tellg();
}
