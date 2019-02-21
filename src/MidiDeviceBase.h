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

#ifndef __MIDI_DEVICE_BASE_H__
#define __MIDI_DEVICE_BASE_H__
#include <QStringList>

#include "Util.h"
#include "Cfg.h"

/*!
 * @brief   xxxxx.
 */

#include "MidiEvent.h"


class CMidiDeviceBase
{
public:
    virtual void init() = 0;
    //! add a midi event to be played immediately
    virtual void playMidiEvent(const CMidiEvent & event) = 0;
    virtual int checkMidiInput() = 0;
    virtual CMidiEvent readMidiInput() = 0;

    typedef enum {MIDI_INPUT, MIDI_OUTPUT} midiType_t;
    virtual QStringList getMidiPortList(midiType_t type) = 0;

    virtual bool openMidiPort(midiType_t type, QString portName) = 0;
    virtual void closeMidiPort(midiType_t type, int index) = 0;

    // based on the fluid synth settings
    virtual int     midiSettingsSetStr(QString name, QString str) = 0;
    virtual int     midiSettingsSetNum(QString name, double val) = 0;
    virtual int     midiSettingsSetInt(QString name, int val) = 0;
    virtual QString midiSettingsGetStr(QString name) = 0;
    virtual double  midiSettingsGetNum(QString name) = 0;
    virtual int     midiSettingsGetInt(QString name) = 0;

    //you should always have a virtual destructor when using virtual functions
    virtual ~CMidiDeviceBase() {};


private:
};

#endif //__MIDI_DEVICE_H__
