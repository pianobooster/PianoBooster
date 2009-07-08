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
#include "MidiDeviceFluidSynth.h"



CMidiDevice::CMidiDevice()
{
    m_rtMidiDevice = new CMidiDeviceRt();
    m_fluidSynthMidiDevice = new CMidiDeviceFluidSynth();
    m_selectedMidiInputDevice = m_rtMidiDevice;
    //m_selectedMidiOutputDevice = m_rtMidiDevice;
    m_selectedMidiOutputDevice = m_fluidSynthMidiDevice;
}

CMidiDevice::~CMidiDevice()
{
    delete m_rtMidiDevice;
    delete m_fluidSynthMidiDevice;
}



void CMidiDevice::init()
{
}

QStringList CMidiDevice::getMidiPortList(midiType_t type)
{
    QStringList list;

    list <<  m_fluidSynthMidiDevice->getMidiPortList(type);
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

        //m_selectedMidiOutputDevice->closeMidiPort(type, portName);
        if ( m_rtMidiDevice->openMidiPort(type, portName) )
        {
            m_selectedMidiOutputDevice = m_rtMidiDevice;
            return true;
        }
        if ( m_fluidSynthMidiDevice->openMidiPort(type, portName) )
        {

            m_selectedMidiOutputDevice = m_fluidSynthMidiDevice;
            return true;
        }
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
    //event.printDetails(); // usefull for debuging
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


