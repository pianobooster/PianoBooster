/*********************************************************************************/
/*!
@file           MidiDeviceRt.cpp

@brief          MidiDeviceRt talks to the MidiRt  Real Time Version.

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

#include "MidiDeviceRt.h"


CMidiDeviceRt::CMidiDeviceRt()
{
    m_midiout = new RtMidiOut();
    m_midiin = new RtMidiIn();
    m_midiPorts[0] = -1;
    m_midiPorts[1] = -1;
    m_rawDataIndex = 0;
}

CMidiDeviceRt::~CMidiDeviceRt()
{
    delete m_midiout;
    delete m_midiin;
}

void CMidiDeviceRt::init()
{
}

QString CMidiDeviceRt::addIndexToString(QString name, int index)
{
    QString ret;
    QString idx;
    idx.setNum(index);
    idx.append(" - ");
    ret = idx + name;
    return ret;
}
QStringList CMidiDeviceRt::getMidiPortList(midiType_t type)
{
    unsigned int nPorts;
    QString name;
    RtMidi* midiDevice;
    QStringList portNameList;

    if (type == MIDI_INPUT)
        midiDevice = m_midiin;
    else
        midiDevice = m_midiout;

    nPorts = midiDevice->getPortCount();

    for(unsigned int i=0; i< nPorts; i++)
    {
        // kotechnology creating indexed string from the post name
        name = addIndexToString(midiDevice->getPortName(i).c_str(),i);
        if (name.contains("RtMidi Output Client"))
            continue;
        if (name.contains("RtMidi Input Client"))
            continue;
         portNameList << name;
    }


    return portNameList;
}

bool CMidiDeviceRt::openMidiPort(midiType_t type, QString portName)
{
    unsigned int nPorts;
    QString name;
    RtMidi* midiDevice;


    if (portName.length() == 0)
        return false;

    int dev;
    if (type == MIDI_INPUT)
    {
        midiDevice = m_midiin;
        dev = 0;
    }
    else
    {
        midiDevice = m_midiout;
        dev = 1;
    }

    nPorts = midiDevice->getPortCount();

    for(unsigned int i=0; i< nPorts; i++)
    {
        // kotechnology creating indexed string from the post name
        name = addIndexToString(midiDevice->getPortName(i).c_str(),i);
        if (name == portName) // Test for a match
        {
            if (m_midiPorts[dev] >= 0)
                midiDevice->closePort();

            m_midiPorts[dev] = i;
            m_rawDataIndex = 0;

            midiDevice->openPort( i );
            return true;
        }
    }
    return false;
}

void CMidiDeviceRt::closeMidiPort(midiType_t type, int index)
{
    if (type == MIDI_INPUT)
        m_midiin->closePort();
    else
        m_midiout->closePort();
}


//! add a midi event to be played immediately
void CMidiDeviceRt::playMidiEvent(const CMidiEvent & event)
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
            if (m_rawDataIndex < arraySize(m_savedRawBytes))
                m_savedRawBytes[m_rawDataIndex++] = event.data1();
            return; // Don't output any thing yet so just return

        case  MIDI_PB_outputRawMidiData: //used for a SYSTEM_EVENT
            for (size_t i = 0; i < m_rawDataIndex; i++)
                message.push_back( m_savedRawBytes[i]);
            m_rawDataIndex = 0;
            break;
    }

    m_midiout->sendMessage( &message );

    //event.printDetails(); // useful for debugging
}


// Return the number of events waiting to be read from the midi device
int CMidiDeviceRt::checkMidiInput()
{
    if (m_midiPorts[0] < 0)
        return 0;
    m_midiin->getMessage( &m_inputMessage );
    return m_inputMessage.size();
}

// reads the real midi event
CMidiEvent CMidiDeviceRt::readMidiInput()
{
    CMidiEvent midiEvent;
    unsigned int channel;


    if (Cfg::midiInputDump)
    {
        QString str;

        for (unsigned int i = 0; i < m_inputMessage.size(); i++)
            str += " 0x" + QString::number(m_inputMessage[i], 16) + ',';
        ppLogInfo("midi input %s", qPrintable(str));
    }

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


int CMidiDeviceRt::midiSettingsSetStr(QString name, QString str)
{
    return 0;
}

int CMidiDeviceRt::midiSettingsSetNum(QString name, double val)
{
    return 0;
}

int CMidiDeviceRt::midiSettingsSetInt(QString name, int val)
{
    return 0;
}

QString CMidiDeviceRt::midiSettingsGetStr(QString name)
{
    return QString();
}

double CMidiDeviceRt::midiSettingsGetNum(QString name)
{
    return 0.0;
}

int CMidiDeviceRt::midiSettingsGetInt(QString namel)
{
    return 0;
}
