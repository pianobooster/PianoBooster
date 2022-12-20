/*********************************************************************************/
/*!
@file           MidiDeviceRt.cpp

@brief          MidiDeviceRt talks to the MidiRt  Real Time Version.

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

#include "MidiDeviceRt.h"

#include <limits>

CMidiDeviceRt::CMidiDeviceRt()
{
    m_validConnection = false;
    m_midiout = nullptr;
    m_midiin = nullptr;
    m_midiPorts[0] = -1;
    m_midiPorts[1] = -1;
    m_rawDataIndex = 0;
    init();
}

CMidiDeviceRt::~CMidiDeviceRt()
{
    if (m_midiout!=nullptr) { delete m_midiout; }
    if (m_midiin!=nullptr) {delete m_midiin; }
}

void CMidiDeviceRt::init()
{
    if (m_midiin == nullptr || m_midiout == nullptr) {
        m_midiPorts[0] = -1;
        m_midiPorts[1] = -1;
        m_rawDataIndex = 0;
        if (m_midiout!=nullptr) {
            delete m_midiout;
            m_midiout = nullptr;
        }
        try {
            m_midiout = new RtMidiOut();
        }
        catch(RtMidiError &error){
            error.printMessage();
            return;
        }

        if (m_midiin!=nullptr) {
            delete m_midiin;
            m_midiin = nullptr;
        }
        try {
            m_midiin = new RtMidiIn();
        }
        catch(RtMidiError &error){
            error.printMessage();
            return;
        }
    }
}

QString CMidiDeviceRt::addIndexToString(const QString &name, int index)
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
    init();
    QStringList portNameList;
    if (m_midiin == nullptr || m_midiout == nullptr) {
        return portNameList;
    }

    unsigned int nPorts;
    QString name;
    RtMidi* midiDevice;

    if (type == MIDI_INPUT)
        midiDevice = m_midiin;
    else
        midiDevice = m_midiout;

    nPorts = midiDevice->getPortCount();

    for(unsigned int i=0; i< nPorts && i < std::numeric_limits<int>::max(); ++i)
    {
        // kotechnology creating indexed string from the post name
        name = addIndexToString(midiDevice->getPortName(i).c_str(), static_cast<int>(i));
        if (name.contains("RtMidi Output Client"))
            continue;
        if (name.contains("RtMidi Input Client"))
            continue;
         portNameList << name;
    }

    return portNameList;
}

bool CMidiDeviceRt::openMidiPort(midiType_t type, const QString &portName)
{
    init();
    if (m_midiin == nullptr || m_midiout == nullptr) {
        return false;
    }

    unsigned int nPorts;
    QString name;
    RtMidi* midiDevice;

    if (portName.isEmpty())
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

    for(unsigned int i=0; i< nPorts && i <= std::numeric_limits<int>::max(); i++)
    {
        // kotechnology creating indexed string from the post name
        name = addIndexToString(midiDevice->getPortName(i).c_str(), static_cast<int>(i));
        if (name == portName) // Test for a match
        {
            if (m_midiPorts[dev] >= 0)
                midiDevice->closePort();

            m_midiPorts[dev] = static_cast<int>(i);
            m_rawDataIndex = 0;

            midiDevice->openPort( i );
            m_validConnection = true;
            return true;
        }
    }
    return false;
}

void CMidiDeviceRt::closeMidiPort(midiType_t type, int index)
{
    Q_UNUSED(index)
    m_validConnection = false;
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
            message.push_back(static_cast<unsigned char>(channel | MIDI_NOTE_OFF));
            message.push_back(static_cast<unsigned char>(event.note()));
            message.push_back(static_cast<unsigned char>(event.velocity()));
            break;
        case MIDI_NOTE_ON:      // NOTE_ON
            message.push_back(static_cast<unsigned char>(channel | MIDI_NOTE_ON));
            message.push_back(static_cast<unsigned char>(event.note()));
            message.push_back(static_cast<unsigned char>(event.velocity()));
            break;

        case MIDI_NOTE_PRESSURE: //POLY_AFTERTOUCH: 3 bytes
            message.push_back(static_cast<unsigned char>(channel | MIDI_NOTE_PRESSURE));
            message.push_back(static_cast<unsigned char>(event.data1()));
            message.push_back(static_cast<unsigned char>(event.data2()));
            break;

        case MIDI_CONTROL_CHANGE: //CONTROL_CHANGE:
            message.push_back(static_cast<unsigned char>(channel | MIDI_CONTROL_CHANGE));
            message.push_back(static_cast<unsigned char>(event.data1()));
            message.push_back(static_cast<unsigned char>(event.data2()));
            break;

        case MIDI_PROGRAM_CHANGE: //PROGRAM_CHANGE:
            message.push_back(static_cast<unsigned char>(channel | MIDI_PROGRAM_CHANGE));
            message.push_back(static_cast<unsigned char>(event.programme()));
            break;

        case MIDI_CHANNEL_PRESSURE: //AFTERTOUCH: 2 bytes only
            message.push_back(static_cast<unsigned char>(channel | MIDI_CHANNEL_PRESSURE));
            message.push_back(static_cast<unsigned char>(event.data1()));
            break;

        case MIDI_PITCH_BEND: //PITCH_BEND:
            message.push_back(static_cast<unsigned char>(channel | MIDI_PITCH_BEND));
            message.push_back(static_cast<unsigned char>(event.data1()));
            message.push_back(static_cast<unsigned char>(event.data2()));
            break;

        case  MIDI_PB_collateRawMidiData: //used for a SYSTEM_EVENT
            if (m_rawDataIndex < arraySizeAs<unsigned int>(m_savedRawBytes))
                m_savedRawBytes[m_rawDataIndex++] = static_cast<unsigned char>(event.data1());
            return; // Don't output any thing yet so just return

        case  MIDI_PB_outputRawMidiData: //used for a SYSTEM_EVENT
            for (size_t i = 0; i < m_rawDataIndex; i++)
                message.push_back( m_savedRawBytes[i]);
            m_rawDataIndex = 0;
            break;

        default:
            return;

    }
    try {
        m_midiout->sendMessage( &message );
    }
    catch(RtMidiError &error){
        error.printMessage();
        m_validConnection = false;
    }

    //event.printDetails(); // useful for debugging
}

// Return the number of events waiting to be read from the midi device
int CMidiDeviceRt::checkMidiInput()
{
    if (m_midiPorts[0] < 0)
        return 0;

    try {
        m_stamp = m_midiin->getMessage( &m_inputMessage );
    }
    catch(RtMidiError &error){
        error.printMessage();
        m_validConnection = false;
        return 0;
    }

    return m_inputMessage.size() > std::numeric_limits<int>::max() ? std::numeric_limits<int>::max() : static_cast<int>(m_inputMessage.size());
}

// reads the real midi event
CMidiEvent CMidiDeviceRt::readMidiInput()
{
    CMidiEvent midiEvent;

    if (Cfg::midiInputDump)
    {
        QString str;

        for (unsigned int i = 0; i < m_inputMessage.size(); i++)
            str += " 0x" + QString::number(m_inputMessage[i], 16) + ',';
        ppLogInfo("midi input %f : %s", m_stamp, qPrintable(str));
    }

    int channel = m_inputMessage[0] & 0x0f;
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
        midiEvent.notePressure(0, channel, m_inputMessage[1], m_inputMessage[2]);
        break;

    case MIDI_CONTROL_CHANGE:  //CONTROL_CHANGE:
        midiEvent.controlChangeEvent(0, channel, m_inputMessage[1], m_inputMessage[2]);
        break;

    case MIDI_PROGRAM_CHANGE: //PROGRAM_CHANGE:
        midiEvent.programChangeEvent(0, channel, m_inputMessage[1]);
        break;

    case MIDI_CHANNEL_PRESSURE: //AFTERTOUCH:
        midiEvent.channelPressure(0, channel, m_inputMessage[1]);
        break;

    case MIDI_PITCH_BEND: //PITCH_BEND:
        midiEvent.pitchBendEvent(0, channel, m_inputMessage[1], m_inputMessage[2]);
        break;
    }

    m_inputMessage.clear();
    return midiEvent;
}

int CMidiDeviceRt::midiSettingsSetStr(const QString &name, const QString &str)
{
    Q_UNUSED(name)
    Q_UNUSED(str)
    return 0;
}

int CMidiDeviceRt::midiSettingsSetNum(const QString &name, double val)
{
    Q_UNUSED(name)
    Q_UNUSED(val)
    return 0;
}

int CMidiDeviceRt::midiSettingsSetInt(const QString &name, int val)
{
    Q_UNUSED(name)
    Q_UNUSED(val)
    return 0;
}

QString CMidiDeviceRt::midiSettingsGetStr(const QString &name)
{
    Q_UNUSED(name)
    return QString();
}

double CMidiDeviceRt::midiSettingsGetNum(const QString &name)
{
    Q_UNUSED(name)
    return 0.0;
}

int CMidiDeviceRt::midiSettingsGetInt(const QString &name)
{
    Q_UNUSED(name)
    return 0;
}
