/*********************************************************************************/
/*!
@file           MidiDevice.h

@brief          xxxxxx.

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

#ifndef __MIDI_DEVICE_H__
#define __MIDI_DEVICE_H__

#include "Util.h"
/*!
 * @brief   xxxxx.
 */

#include "MidiEvent.h"

#include "rtmidi/RtMidi.h"

class CMidiDevice
{
public:
    CMidiDevice();
    ~CMidiDevice();
    void init();
    //! add a midi event to be played imidiataly
    void playMidiEvent(CMidiEvent event);
    int checkMidiInput();
    CMidiEvent readMidiInput();

    string getMidiPortName(int dev, unsigned index);
    bool openMidiPort(int dev, string portName);

private:

    RtMidiOut *m_midiout;
    RtMidiIn *m_midiin;

    // 0 for input, 1 for output
    int m_midiPorts[2];      // select which MIDI output port to open
    std::vector<unsigned char> m_inputMessage;
    unsigned char savedRawBytes[40]; // Raw data is used for used for a SYSTEM_EVENT
    unsigned int rawDataIndex; 
};

#endif //__MIDI_DEVICE_H__
