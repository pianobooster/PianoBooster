/*********************************************************************************/
/*!
@file           MidiDeviceFluidSynth.cpp

@brief          MidiDeviceFluidSynth talks to the MidiRt  Real Time Version.

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

#include "MidiDeviceFluidSynth.h"

static fluid_settings_t* s_debug_fluid_settings;

static void debug_settings_foreach_func (void *data,
#if FLUIDSYNTH_VERSION_MAJOR >= 2
    const char *name,
#else
    char *name,
#endif
    int type)
{
    Q_UNUSED(data)
    Q_UNUSED(type)
    char buffer[300];
    fluid_settings_copystr(s_debug_fluid_settings, name, buffer, sizeof (buffer));
    ppLogDebug("settings_foreach_func %s : %s", name , buffer );
}

CMidiDeviceFluidSynth::CMidiDeviceFluidSynth()
{
    m_synth = nullptr;
    m_fluidSettings = nullptr;
    m_audioDriver = nullptr;
    m_rawDataIndex = 0;
    m_validConnection = false;
}

CMidiDeviceFluidSynth::~CMidiDeviceFluidSynth()
{
    CMidiDeviceFluidSynth::closeMidiPort(MIDI_OUTPUT, -1); // not possible to make virtual call in d'tor
}

void CMidiDeviceFluidSynth::init()
{
}

QStringList CMidiDeviceFluidSynth::getMidiPortList(midiType_t type)
{
    if (type != MIDI_OUTPUT) // Only has an output
        return QStringList();

    if (qsettings==nullptr)
        return QStringList();

    QStringList fontList = qsettings->value("FluidSynth/SoundFont").toStringList();

    if (fontList.size() > 0){
        return QStringList(getFluidInternalName());
    }
    return fontList;
}

bool CMidiDeviceFluidSynth::openMidiPort(midiType_t type, const QString &portName)
{
    closeMidiPort(MIDI_OUTPUT, -1);

    if (portName.isEmpty())
        return false;

    if (type == MIDI_INPUT)
        return false;

    if (!portName.endsWith(FLUID_NAME)) {return false;}

    if (getMidiPortList(type).size()==0) {return false;}

    // Load a SoundFont
    QStringList fontList = qsettings->value("FluidSynth/SoundFont").toStringList();
    if (fontList.size() == 0) {return false;}

    // Create the settings.
    m_fluidSettings = new_fluid_settings();

    // Change the settings if necessary
    fluid_settings_setnum(m_fluidSettings, "synth.sample-rate", qsettings->value("FluidSynth/sampleRateCombo",22050).toInt());
    fluid_settings_setint(m_fluidSettings, "audio.period-size", qsettings->value("FluidSynth/bufferSizeCombo", 128).toInt());
    fluid_settings_setint(m_fluidSettings, "audio.periods", qsettings->value("FluidSynth/bufferCountCombo", 4).toInt());

#if !defined (Q_OS_WINDOWS)
    fluid_settings_setstr(m_fluidSettings, "audio.driver", qsettings->value("FluidSynth/audioDriverCombo", "pulseaudio").toString().toStdString().c_str());
#endif

    // Create the synthesizer.
    m_synth = new_fluid_synth(m_fluidSettings);
#if (FLUIDSYNTH_VERSION_MAJOR >= 2) && (FLUIDSYNTH_VERSION_MINOR >= 2)
    fluid_synth_reverb_on(m_synth, -1, 0);
    fluid_synth_chorus_on(m_synth, -1, 0);
#else
    fluid_synth_set_reverb_on(m_synth, 0);
    fluid_synth_set_chorus_on(m_synth, 0);
#endif

    // Create the audio driver.
    m_audioDriver = new_fluid_audio_driver(m_fluidSettings, m_synth);

    QString pathName = fontList.at(0);
    ppLogDebug("Sound font %s", qPrintable(pathName));
    m_soundFontId = fluid_synth_sfload(m_synth, qPrintable(pathName), 0);
    if (m_soundFontId == -1)
        return false;

    for (int channel = 0; channel < MAX_MIDI_CHANNELS ; channel++)
    {
         fluid_synth_program_change(m_synth, channel, GM_PIANO_PATCH);
    }
    fluid_synth_set_gain(m_synth, qsettings->value("FluidSynth/masterGainSpin", FLUID_DEFAULT_GAIN).toFloat()/100.0f );
    m_validConnection = true;
    if (Cfg::logLevel >= LOG_LEVEL_DEBUG) {
        s_debug_fluid_settings = m_fluidSettings;
        fluid_settings_foreach(m_fluidSettings, 0, debug_settings_foreach_func);
    }
    return true;
}

void CMidiDeviceFluidSynth::closeMidiPort(midiType_t type, int index)
{
    Q_UNUSED(index)
    m_validConnection = false;

    if (type != MIDI_OUTPUT)
        return;

    if (m_fluidSettings == nullptr)
        return;

    /* Clean up */
    delete_fluid_audio_driver(m_audioDriver);
    delete_fluid_synth(m_synth);
    delete_fluid_settings(m_fluidSettings);
    m_fluidSettings = nullptr;
    m_rawDataIndex = 0;

}

//! add a midi event to be played immediately
void CMidiDeviceFluidSynth::playMidiEvent(const CMidiEvent & event)
{
    if (m_synth == nullptr)
        return;

    int channel = event.channel() & 0x0f;
    switch(event.type())
    {
        case MIDI_NOTE_OFF: // NOTE_OFF
            fluid_synth_noteoff(m_synth, channel, event.note());
            break;
        case MIDI_NOTE_ON:      // NOTE_ON
            fluid_synth_noteon(m_synth, channel, event.note(), event.velocity());
            break;

        case MIDI_NOTE_PRESSURE: //POLY_AFTERTOUCH: 3 bytes
            //fluid_synth_key_pressure(m_synth, channel, event.data1(), event.data2());
            break;

        case MIDI_CONTROL_CHANGE: //CONTROL_CHANGE:
            fluid_synth_cc(m_synth, channel, event.data1(), event.data2());
            //ppLogTrace("MIDI_CONTROL_CHANGE %d %d %d", channel, event.data1(), event.data2());
            break;

        case MIDI_PROGRAM_CHANGE: //PROGRAM_CHANGE:
            fluid_synth_program_change(m_synth, channel, event.programme());
            break;

        case MIDI_CHANNEL_PRESSURE: //AFTERTOUCH: 2 bytes only
            fluid_synth_channel_pressure(m_synth, channel, event.programme());
            break;

        case MIDI_PITCH_BEND: //PITCH_BEND:
            // a 14 bit number LSB first 0x4000 is the off positions
            fluid_synth_pitch_bend(m_synth, channel, (event.data2() << 7) | event.data1());
            break;

        case  MIDI_PB_collateRawMidiData: //used for a SYSTEM_EVENT
            if (m_rawDataIndex < arraySizeAs<unsigned int>(m_savedRawBytes))
                m_savedRawBytes[m_rawDataIndex++] = static_cast<unsigned char>(event.data1());
            return; // Don't output any thing yet so just return

        case  MIDI_PB_outputRawMidiData: //used for a SYSTEM_EVENT
            //for (size_t i = 0; i < m_rawDataIndex; i++)
                //message.push_back( m_savedRawBytes[i]);
            m_rawDataIndex = 0;
            // FIXME missing
            break;
    }
    //event.printDetails(); // useful for debugging
}

// Return the number of events waiting to be read from the midi device
int CMidiDeviceFluidSynth::checkMidiInput()
{
    return 0;
}

// reads the real midi event
CMidiEvent CMidiDeviceFluidSynth::readMidiInput()
{
    CMidiEvent midiEvent;
    return midiEvent;
}

int CMidiDeviceFluidSynth::midiSettingsSetStr(const QString &name, const QString &str)
{
    if (!m_fluidSettings)
        return 0;

    return fluid_settings_setstr(m_fluidSettings, (char *)qPrintable(name), (char *)qPrintable(str));
}

int CMidiDeviceFluidSynth::midiSettingsSetNum(const QString &name, double val)
{
    if (!m_fluidSettings)
        return 0;
    return fluid_settings_setnum(m_fluidSettings, (char *)qPrintable(name), val);
}

int CMidiDeviceFluidSynth::midiSettingsSetInt(const QString &name, int val)
{
    if (!m_fluidSettings)
        return 0;

    return fluid_settings_setint(m_fluidSettings, (char *)qPrintable(name), val);
}

QString CMidiDeviceFluidSynth::midiSettingsGetStr(const QString &name)
{
    Q_UNUSED(name)
    //char buffer[200];
    //if (!m_fluidSettings)
    //    return QString();
    //fluid_settings_getstr(m_fluidSettings, (char *)qPrintable(name), buffer );
    //return QString( buffer );
    return QString(); // FIXME: buffer is never initialized here, let's just return QString() for now
}

double CMidiDeviceFluidSynth::midiSettingsGetNum(const QString &name)
{
    if (!m_fluidSettings)
        return 0.0;
    double val;
    fluid_settings_getnum(m_fluidSettings, (char *)qPrintable(name), &val);
    return val;
}

int CMidiDeviceFluidSynth::midiSettingsGetInt(const QString &name)
{
    if (!m_fluidSettings)
        return 0;
    int val = 0;
    fluid_settings_getint(m_fluidSettings, (char *)qPrintable(name),&val);
    return val;
}
