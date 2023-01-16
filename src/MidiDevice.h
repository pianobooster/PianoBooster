/*********************************************************************************/
/*!
@file           MidiDevice.h

@brief          xxxxxx.

@author         L. J. Barman

    Copyright (c)   2008-2020, L. J. Barman and others, all rights reserved

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

#include "MidiDeviceBase.h"

class CMidiDevice : public CMidiDeviceBase
{
public:
    CMidiDevice();
    ~CMidiDevice();
    void init();
    //! add a midi event to be played immediately
    void playMidiEvent(const CMidiEvent & event);
    int checkMidiInput();
    CMidiEvent readMidiInput();
    bool validMidiOutput();
    virtual bool validMidiConnection() {return validMidiOutput();}

    QStringList getMidiPortList(midiType_t type);
    bool openMidiPort(midiType_t type, const QString &portName);
    void closeMidiPort(midiType_t type, int index);
    // based on the fluid synth settings
    virtual int     midiSettingsSetStr(const QString &name, const QString &str);
    virtual int     midiSettingsSetNum(const QString &name, double val);
    virtual int     midiSettingsSetInt(const QString &name, int val);
    virtual QString midiSettingsGetStr(const QString &name);
    virtual double  midiSettingsGetNum(const QString &name);
    virtual int     midiSettingsGetInt(const QString &name);

    void flushMidiInput()
    {
        while (checkMidiInput() > 0) {
            readMidiInput();
        }
    }

private:
    CMidiDeviceBase* m_rtMidiDevice;
#if WITH_INTERNAL_FLUIDSYNTH
    CMidiDeviceBase* m_fluidSynthMidiDevice;
#endif
    CMidiDeviceBase* m_selectedMidiInputDevice;
    CMidiDeviceBase* m_selectedMidiOutputDevice;
    bool m_validOutput;
};

#endif //__MIDI_DEVICE_H__
