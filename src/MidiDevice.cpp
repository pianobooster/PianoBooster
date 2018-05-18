/*********************************************************************************/
/*!
@file           MidiDevice.cpp

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

#include "MidiDevice.h"
#include "MidiDeviceRt.h"
#if PB_USE_FLUIDSYNTH
    #include "MidiDeviceFluidSynth.h"
#endif




CMidiDevice::CMidiDevice()
{
    m_rtMidiDevice = new CMidiDeviceRt();
#if PB_USE_FLUIDSYNTH
    m_fluidSynthMidiDevice = new CMidiDeviceFluidSynth();
#endif
    m_selectedMidiInputDevice = m_rtMidiDevice;
    m_selectedMidiOutputDevice = m_rtMidiDevice;
    m_validOutput = false;
}

CMidiDevice::~CMidiDevice()
{
    delete m_rtMidiDevice;
#if PB_USE_FLUIDSYNTH
    delete m_fluidSynthMidiDevice;
#endif
}



void CMidiDevice::init()
{
}

QStringList CMidiDevice::getMidiPortList(midiType_t type)
{
    QStringList list;
#if PB_USE_FLUIDSYNTH
    list <<  m_fluidSynthMidiDevice->getMidiPortList(type);
#endif
    list <<  m_rtMidiDevice->getMidiPortList(type);

    return list;
}

bool CMidiDevice::openMidiPort(midiType_t type, QString portName)
{

    closeMidiPort(type, -1);
    if (type == MIDI_INPUT)
    {
        if (m_rtMidiDevice->openMidiPort(type, portName))
        {
            m_selectedMidiOutputDevice = m_rtMidiDevice;
            return true;
        }
    }
    else
    {
        m_validOutput = false;

        //m_selectedMidiOutputDevice->closeMidiPort(type, portName);
        if ( m_rtMidiDevice->openMidiPort(type, portName) )
        {
            m_selectedMidiOutputDevice = m_rtMidiDevice;
            m_validOutput = true;
            return true;
        }
#if PB_USE_FLUIDSYNTH
        if ( m_fluidSynthMidiDevice->openMidiPort(type, portName) )
        {
            m_selectedMidiOutputDevice = m_fluidSynthMidiDevice;
            m_validOutput = true;
            return true;
        }
#endif
    }
    return false;
}

void CMidiDevice::closeMidiPort(midiType_t type, int index)
{
    if (m_selectedMidiOutputDevice == 0)
        return;

    m_selectedMidiOutputDevice->closeMidiPort(type, index);

    m_selectedMidiOutputDevice = 0;
}

//! add a midi event to be played immediately
void CMidiDevice::playMidiEvent(const CMidiEvent & event)
{
    if (m_selectedMidiOutputDevice == 0)
        return;

    m_selectedMidiOutputDevice->playMidiEvent(event);
    //event.printDetails(); // useful for debugging
}


// Return the number of events waiting to be read from the midi device
int CMidiDevice::checkMidiInput()
{
    if (m_selectedMidiOutputDevice == 0)
        return 0;

    return m_selectedMidiInputDevice->checkMidiInput();
}

// reads the real midi event
CMidiEvent CMidiDevice::readMidiInput()
{
    return m_selectedMidiInputDevice->readMidiInput();
}


int CMidiDevice::midiSettingsSetStr(QString name, QString str)
{
    if (m_selectedMidiOutputDevice)
        return m_selectedMidiOutputDevice->midiSettingsSetStr(name, str);
    return 0;
}

int CMidiDevice::midiSettingsSetNum(QString name, double val)
{
    if (m_selectedMidiOutputDevice)
        return m_selectedMidiOutputDevice->midiSettingsSetNum(name, val);
    return 0;
}

int CMidiDevice::midiSettingsSetInt(QString name, int val)
{
    if (m_selectedMidiOutputDevice)
        return m_selectedMidiOutputDevice->midiSettingsSetInt(name, val);
    return 0;
}

QString CMidiDevice::midiSettingsGetStr(QString name)
{
    if (m_selectedMidiOutputDevice)
        return m_selectedMidiOutputDevice->midiSettingsGetStr(name);
    return QString();
}

double CMidiDevice::midiSettingsGetNum(QString name)
{
    if (m_selectedMidiOutputDevice)
        return m_selectedMidiOutputDevice->midiSettingsGetNum(name);
    return 0.0;
}

int CMidiDevice::midiSettingsGetInt(QString name)
{
    if (m_selectedMidiOutputDevice)
        return m_selectedMidiOutputDevice->midiSettingsGetInt(name);
    return 0;
}
