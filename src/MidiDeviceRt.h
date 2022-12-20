/*********************************************************************************/
/*!
@file           MidiDeviceRt.h

@author         L. J. Barman

    Copyright (c)   2008-2020, L. J. Barman, all rights reserved

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

#ifndef __MIDI_DEVICE_RT_H__
#define __MIDI_DEVICE_RT_H__

#include "MidiDeviceBase.h"
#include "rtmidi/RtMidi.h"

class CMidiDeviceRt : public CMidiDeviceBase
{
    virtual void init();
    //! add a midi event to be played immediately
    virtual void playMidiEvent(const CMidiEvent & event);
    virtual int checkMidiInput();
    virtual CMidiEvent readMidiInput();

    virtual QStringList getMidiPortList(midiType_t type);

    virtual bool openMidiPort(midiType_t type, const QString &portName);
    virtual void closeMidiPort(midiType_t type, int index);

    virtual bool validMidiConnection() {return m_validConnection;}

    // based on the fluid synth settings
    virtual int     midiSettingsSetStr(const QString &name, const QString &str);
    virtual int     midiSettingsSetNum(const QString &name, double val);
    virtual int     midiSettingsSetInt(const QString &name, int val);
    virtual QString midiSettingsGetStr(const QString &name);
    virtual double  midiSettingsGetNum(const QString &name);
    virtual int     midiSettingsGetInt(const QString &name);

public:
    CMidiDeviceRt();
    ~CMidiDeviceRt();


private:

    RtMidiOut *m_midiout;
    RtMidiIn *m_midiin;

    double m_stamp;

    // 0 for input, 1 for output
    int m_midiPorts[2];      // select which MIDI output port to open
    std::vector<unsigned char> m_inputMessage;
    unsigned char m_savedRawBytes[40]; // Raw data is used for used for a SYSTEM_EVENT
    unsigned int m_rawDataIndex;

    // kotechnology added function to create indexed string. Format: "1 - Example"
    QString addIndexToString(const QString &name, int index);

    bool m_validConnection;
};

#endif //__MIDI_DEVICE_RT_H__
