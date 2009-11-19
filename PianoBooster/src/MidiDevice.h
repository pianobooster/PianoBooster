/*********************************************************************************/
/*!
@file           MidiDevice.h

@brief          xxxxxx.

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
    bool validMidiOutput() { return m_validOutput; }

    QStringList getMidiPortList(midiType_t type);
    bool openMidiPort(midiType_t type, QString portName);
    void closeMidiPort(midiType_t type, int index);
    // based on the fluid synth settings
    virtual int     midiSettingsSetStr(QString name, QString str);
    virtual int     midiSettingsSetNum(QString name, double val);
    virtual int     midiSettingsSetInt(QString name, int val);
    virtual QString midiSettingsGetStr(QString name);
    virtual double  midiSettingsGetNum(QString name);
    virtual int     midiSettingsGetInt(QString name);

private:

    CMidiDeviceBase* m_rtMidiDevice;
#if PB_USE_FLUIDSYNTH
    CMidiDeviceBase* m_fluidSynthMidiDevice;
#endif
    CMidiDeviceBase* m_selectedMidiInputDevice;
    CMidiDeviceBase* m_selectedMidiOutputDevice;
    bool m_validOutput;
};

#endif //__MIDI_DEVICE_H__
