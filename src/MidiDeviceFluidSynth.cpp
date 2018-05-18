/*********************************************************************************/
/*!
@file           MidiDeviceFluidSynth.cpp

@brief          MidiDeviceFluidSynth talks to the MidiRt  Real Time Version.

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

#include "MidiDeviceFluidSynth.h"

#include <QString>
#include <QDir>
#include <string>

CMidiDeviceFluidSynth::CMidiDeviceFluidSynth()
{
    m_synth = 0;
    m_fluidSettings = 0;
    m_audioDriver = 0;
    m_rawDataIndex = 0;
}

CMidiDeviceFluidSynth::~CMidiDeviceFluidSynth()
{
    closeMidiPort(MIDI_OUTPUT, -1);
}



void CMidiDeviceFluidSynth::init()
{
}


QStringList CMidiDeviceFluidSynth::getMidiPortList(midiType_t type)
{

    if (type != MIDI_OUTPUT) // Only has an output
        return QStringList();

    //debugSettings(("getSongList %s + %d", qPrintable(getCurrentBookName()), qPrintable(m_bookPath)));
    QDir dirSoundFont("soundfont");
    dirSoundFont.setFilter(QDir::Files);
    QStringList fileNames = dirSoundFont.entryList();

    QStringList portNames;

    for (int i = 0; i < fileNames.size(); i++)
    {
        if ( fileNames.at(i).endsWith(".sf2", Qt::CaseInsensitive ) )
        {
            portNames  +=  fileNames.at(i);
        }
    }

    return portNames;
}

bool CMidiDeviceFluidSynth::openMidiPort(midiType_t type, QString portName)
{
    closeMidiPort(MIDI_OUTPUT, -1);

    if (portName.length() == 0)
        return false;

    if (type == MIDI_INPUT)
        return false;

    /* Create the settings. */
    m_fluidSettings = new_fluid_settings();

    /* Change the settings if necessary*/

    fluid_settings_setnum(m_fluidSettings, (char *)"synth.sample-rate", 22050.0);
    fluid_settings_setint(m_fluidSettings, "audio.periods", 5);
    fluid_settings_setint(m_fluidSettings, "audio.period-size", 128);

    fluid_settings_setstr(m_fluidSettings, "audio.alsa.device",        "plughw:0");





    /* Create the synthesizer. */
    m_synth = new_fluid_synth(m_fluidSettings);

    fluid_synth_set_reverb_on(m_synth, 0);
    fluid_synth_set_chorus_on(m_synth, 0);


    /* Create the audio driver. The synthesizer starts playing as soon
    as the driver is created. */
    m_audioDriver = new_fluid_audio_driver(m_fluidSettings, m_synth);

    /* Load a SoundFont*/
    m_soundFontId = fluid_synth_sfload(m_synth, "FluidR3_GM.sf2", 0);
    //m_soundFontId = fluid_synth_sfload(m_synth, "VintageDreamsWaves-v2.sf2", 0);

    /* Select bank 0 and preset 0 in the SoundFont we just loaded on
    channel 0 */
    //fluid_synth_program_select(m_synth, 0, m_soundFontId, 0, 0);


    for (int channel = 0; channel < MAX_MIDI_CHANNELS ; channel++)
    {
        //fluid_synth_program_select(m_synth, channel, m_soundFontId, 0, GM_PIANO_PATCH);
        fluid_synth_program_change(m_synth, channel, GM_PIANO_PATCH);
    }
    fluid_synth_set_gain(m_synth, 0.4);

    return true;
}

void CMidiDeviceFluidSynth::closeMidiPort(midiType_t type, int index)
{
    if (type != MIDI_OUTPUT)
        return;

    if (m_fluidSettings == 0)
        return;

    /* Clean up */
    delete_fluid_audio_driver(m_audioDriver);
    delete_fluid_synth(m_synth);
    delete_fluid_settings(m_fluidSettings);
    m_fluidSettings = 0;
    m_rawDataIndex = 0;

}

//! add a midi event to be played immediately
void CMidiDeviceFluidSynth::playMidiEvent(const CMidiEvent & event)
{

    if (m_synth == 0)
        return;

    unsigned int channel;

    channel = event.channel() & 0x0f;

    switch(event.type())
    {
        case MIDI_NOTE_OFF: // NOTE_OFF
            fluid_synth_noteoff(m_synth, channel, event.note());
            break;
        case MIDI_NOTE_ON:      // NOTE_ON
            fluid_synth_noteon(m_synth, channel, event.note(), event.velocity());
            break;

        case MIDI_NOTE_PRESSURE: //POLY_AFTERTOUCH: 3 bytes
            //message.push_back( channel | MIDI_NOTE_PRESSURE);
            //message.push_back( event.data1());
            //message.push_back( event.data2());
            break;

        case MIDI_CONTROL_CHANGE: //CONTROL_CHANGE:
            fluid_synth_cc(m_synth, channel, event.data1(), event.data2());
            //ppLogTrace("MIDI_CONTROL_CHANGE %d %d %d", channel, event.data1(), event.data2());
            break;

        case MIDI_PROGRAM_CHANGE: //PROGRAM_CHANGE:
            fluid_synth_program_change(m_synth, channel, event.programme());
            break;

        case MIDI_CHANNEL_PRESSURE: //AFTERTOUCH: 2 bytes only
            //message.push_back( channel | MIDI_CHANNEL_PRESSURE);
            //message.push_back( event.data1());
            // FIXME missing
            break;

        case MIDI_PITCH_BEND: //PITCH_BEND:
            // a 14 bit number LSB first 0x4000 is the off posintions
            fluid_synth_pitch_bend(m_synth, channel, (event.data2() << 7) | event.data1());
                break;

        case  MIDI_PB_collateRawMidiData: //used for a SYSTEM_EVENT
            if (m_rawDataIndex < arraySize(m_savedRawBytes))
                m_savedRawBytes[m_rawDataIndex++] = event.data1();
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



int CMidiDeviceFluidSynth::midiSettingsSetStr(QString name, QString str)
{
    if (!m_fluidSettings)
        return 0;

    return fluid_settings_setstr(m_fluidSettings, (char *)qPrintable(name), (char *)qPrintable(str));
}

int CMidiDeviceFluidSynth::midiSettingsSetNum(QString name, double val)
{
    if (!m_fluidSettings)
        return 0;
    return fluid_settings_setnum(m_fluidSettings, (char *)qPrintable(name), val);
}

int CMidiDeviceFluidSynth::midiSettingsSetInt(QString name, int val)
{
    if (!m_fluidSettings)
        return 0;

    return fluid_settings_setint(m_fluidSettings, (char *)qPrintable(name), val);
}

QString CMidiDeviceFluidSynth::midiSettingsGetStr(QString name)
{
    char buffer[200];
    if (!m_fluidSettings)
        return QString();
    //fluid_settings_getstr(m_fluidSettings, (char *)qPrintable(name), buffer );
    return QString( buffer );
}

double CMidiDeviceFluidSynth::midiSettingsGetNum(QString name)
{
    if (!m_fluidSettings)
        return 0.0;
    double val;
    fluid_settings_getnum(m_fluidSettings, (char *)qPrintable(name), &val);
    return val;
}

int CMidiDeviceFluidSynth::midiSettingsGetInt(QString name)
{
    if (!m_fluidSettings)
        return 0;
    int val = 0;
    fluid_settings_getint(m_fluidSettings, (char *)qPrintable(name),&val);
    return val;
}
