/*********************************************************************************/
/*!
@file           MidiDevice.cpp

@brief          xxxx.

@author         L. J. Barman

    Copyright (c)   2008, L. J. Barman, all rights reserved

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

#include "MidiDevice.h"




CMidiDevice::CMidiDevice()
{
    m_midiout = new RtMidiOut();
    m_midiin = new RtMidiIn();
    m_midiPorts[0] = -1;
    m_midiPorts[1] = -1;
    rawDataIndex = 0;
}

CMidiDevice::~CMidiDevice()
{
    delete m_midiout;
    delete m_midiin;
}



void CMidiDevice::init()
{
}

// dev = 0 for midi input, dev = 1 for output
string CMidiDevice::getMidiPortName(int dev, unsigned int index)
{
    unsigned int nPorts;
    string name;
    RtMidi* midiDevice;

    if (dev < 0 || dev >= 2)
        return string();

    if (dev == 0)
        midiDevice = m_midiin;
    else
        midiDevice = m_midiout;

    nPorts = midiDevice->getPortCount();

    for(unsigned int i=0; i< nPorts; i++)
    {
        name = midiDevice->getPortName(i);
        if (i == index)
            return name;
    }
    return string();
}

bool CMidiDevice::openMidiPort(int dev, string portName)
{
    unsigned int nPorts;
    string name;
    RtMidi* midiDevice;

    if (dev < 0 || dev >= 2)
        return false;

    if (portName.length() == 0)
        return false;

    if (dev == 0)
        midiDevice = m_midiin;
    else
        midiDevice = m_midiout;

    nPorts = midiDevice->getPortCount();

    for(unsigned int i=0; i< nPorts; i++)
    {
        name = midiDevice->getPortName(i);
        if (name.compare(0,portName.length(), portName) == 0) // Test for a match
        {
            if (m_midiPorts[dev] >= 0)
                midiDevice->closePort();

            m_midiPorts[dev] = i;
            midiDevice->openPort( i );
            return true;
        }
    }
    return false;
}


//! add a midi event to be played immediately
void CMidiDevice::playMidiEvent(CMidiEvent event)
{
    if (m_midiPorts[1] < 0)
        return;

    unsigned int channel;
    std::vector<unsigned char> message;

    channel = event.channel() & 0x0f;

    switch(event.type())
    {
        case MIDI_NOTE_OFF: // NOTE_OFF
            message.push_back( channel | MIDI_NOTE_OFF );
            message.push_back( event.note());
            message.push_back( event.velocity());
            break;
        case MIDI_NOTE_ON:      // NOTE_ON
            message.push_back( channel | MIDI_NOTE_ON );
            message.push_back( event.note());
            message.push_back( event.velocity());
            break;

        case MIDI_NOTE_PRESSURE: //POLY_AFTERTOUCH: 3 bytes
            message.push_back( channel | MIDI_NOTE_PRESSURE);
            message.push_back( event.data1());
            message.push_back( event.data2());
            break;

        case MIDI_CONTROL_CHANGE: //CONTROL_CHANGE:
            message.push_back( channel | MIDI_CONTROL_CHANGE);
            message.push_back( event.data1());
            message.push_back( event.data2());
            break;

        case MIDI_PROGRAM_CHANGE: //PROGRAM_CHANGE:
            message.push_back( channel | MIDI_PROGRAM_CHANGE);
            message.push_back( event.programme());
            break;

        case MIDI_CHANNEL_PRESSURE: //AFTERTOUCH: 2 bytes only
            message.push_back( channel | MIDI_CHANNEL_PRESSURE);
            message.push_back( event.data1());
            break;

        case MIDI_PITCH_BEND: //PITCH_BEND:
            message.push_back( channel | MIDI_PITCH_BEND);
            message.push_back( event.data1());
            message.push_back( event.data2());
            break;

        case  MIDI_PB_collateRawMidiData: //used for a SYSTEM_EVENT
            if (rawDataIndex < arraySize(savedRawBytes))
                savedRawBytes[rawDataIndex++] = event.data1();
            return; // Don't output any thing yet so just return

        case  MIDI_PB_outputRawMidiData: //used for a SYSTEM_EVENT
            for (size_t i = 0; i < rawDataIndex; i++)
                message.push_back( savedRawBytes[i]);
            rawDataIndex = 0;
            break;
    }

    m_midiout->sendMessage( &message );

    //event.printDetails(); // usefull for debuging
}


// Return the number of events waiting to be read from the midi device
int CMidiDevice::checkMidiInput()
{
    if (m_midiPorts[0] < 0)
        return 0;
    m_midiin->getMessage( &m_inputMessage );
    return m_inputMessage.size();
}

// reads the real midi event
CMidiEvent CMidiDevice::readMidiInput()
{
    CMidiEvent midiEvent;
    unsigned int channel;


    channel = m_inputMessage[0] & 0x0f;
    switch (m_inputMessage[0] & 0xf0 )
    {
    case MIDI_NOTE_ON:
        if (m_inputMessage[2] != 0 )
            midiEvent.noteOnEvent(0, channel, m_inputMessage[1], m_inputMessage[2]);
        else
            midiEvent.noteOffEvent(0,channel, m_inputMessage[1], m_inputMessage[2]);
        break;

    case MIDI_NOTE_OFF:
        midiEvent.noteOffEvent(0, channel, m_inputMessage[1], m_inputMessage[2]);
        break;

    case MIDI_NOTE_PRESSURE: //MIDI_CMD_NOTE_PRESSURE: //POLY_AFTERTOUCH:
        // fixme fill in the blanks
        //midi_input_bytes[midi_input_length++] = channel | MIDI_CMD_NOTE_PRESSURE;
        //midi_input_bytes[midi_input_length++] = ev->data.note.note;
        //midi_input_bytes[midi_input_length++] = ev->data.note.velocity;
        break;

    case MIDI_CONTROL_CHANGE:  //CONTROL_CHANGE:
        midiEvent.controlChangeEvent(0, channel, m_inputMessage[1], m_inputMessage[2]);
        break;

    case MIDI_PROGRAM_CHANGE: //PROGRAM_CHANGE:
        //midiEvent.programChangeEvent(0, ev->data.control.channel, ev->data.control.value);
        break;

    case MIDI_CHANNEL_PRESSURE: //AFTERTOUCH:
        // fixme fill in the blanks
        //midi_input_bytes[midi_input_length++] = ev->data.control.channel | MIDI_CMD_CHANNEL_PRESSURE;
        //midi_input_bytes[midi_input_length++] = ev->data.control.value;
        break;

    case MIDI_PITCH_BEND: //PITCH_BEND:
        // fixme fill in the blanks
        //midi_input_bytes[midi_input_length++] = ev->data.control.channel | MIDI_CMD_CHANNEL_PRESSURE;
        //midi_input_bytes[midi_input_length++] = ev->data.control.value;
        break;
    }

    m_inputMessage.clear();
    return midiEvent;
}


