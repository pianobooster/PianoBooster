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


CMidiDevice::CMidiDevice()
{
    m_rtMidiDevice = new CMidiDeviceRt();
    m_selectedMidiDevice = m_rtMidiDevice;
}

CMidiDevice::~CMidiDevice()
{
    delete m_rtMidiDevice;
}



void CMidiDevice::init()
{
}

// dev = 0 for midi input, dev = 1 for output
string CMidiDevice::getMidiPortName(int dev, unsigned int index)
{
    return m_rtMidiDevice->getMidiPortName(dev, index);
}

bool CMidiDevice::openMidiPort(int dev, string portName)
{
    return m_rtMidiDevice->openMidiPort(dev, portName);
}


//! add a midi event to be played immediately
void CMidiDevice::playMidiEvent(const CMidiEvent & event)
{
    m_selectedMidiDevice->playMidiEvent(event);
    //event.printDetails(); // usefull for debuging
}


// Return the number of events waiting to be read from the midi device
int CMidiDevice::checkMidiInput()
{
    return m_rtMidiDevice->checkMidiInput();
}

// reads the real midi event
CMidiEvent CMidiDevice::readMidiInput()
{
    return m_rtMidiDevice->readMidiInput();
}


