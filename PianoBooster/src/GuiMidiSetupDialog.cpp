/*!
    @file           GuiMidiSetupDialog.cpp

    @brief          xxx.

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

#include <QtGui>

#include "GuiMidiSetupDialog.h"


GuiMidiSetupDialog::GuiMidiSetupDialog(QWidget *parent)
    : QDialog(parent)
{
    m_song = 0;
    m_settings = 0;
    setupUi(this);
    m_latencyFix = 0;
    m_latencyChanged = false;
    m_midiChanged = false;
    midiSetupTabWidget->setCurrentIndex(0);
#if !PB_USE_FLUIDSYNTH
    midiSetupTabWidget->removeTab(1);
#endif
    setWindowTitle("Midi Setup");
}

void GuiMidiSetupDialog::init(CSong* song, CSettings* settings)
{
    m_song = song;
    m_settings = settings;

    // Check inputs.
    QString portName;
    int i = 0;

    m_latencyFix = m_song->getLatencyFix();

    midiInputCombo->addItem(tr("None (PC Keyboard)"));

    midiInputCombo->addItems(song->getMidiPortList(CMidiDevice::MIDI_INPUT));


    // Check outputs.
    midiOutputCombo->addItem(tr("None"));
    midiOutputCombo->addItems(song->getMidiPortList(CMidiDevice::MIDI_OUTPUT));
    i = midiInputCombo->findText(m_settings->value("Midi/Input").toString());
    if (i!=-1)
        midiInputCombo->setCurrentIndex(i);
    i = midiOutputCombo->findText(m_settings->value("Midi/Output").toString());
    if (i!=-1)
        midiOutputCombo->setCurrentIndex(i);

    sampleRateCombo->addItem("44100");
    sampleRateCombo->addItem("22050");
    i = sampleRateCombo->findText(m_settings->value("Fliudsynth/SampleRate").toString());
    if (i!=-1)
        sampleRateCombo->setCurrentIndex(i);

    //midiSettingsSetnum

    updateMidiInfoText();
}

void GuiMidiSetupDialog::updateMidiInfoText()
{
    QString str;
    midiInfoText->clear();

    if (midiInputCombo->currentIndex() == 0)
        midiInfoText->append("<span style=\"color:black\">If you don't have a MIDI keyboard you can use the PC keyboard; 'X' is middle C.</span>");
    else if (midiInputCombo->currentText().startsWith("Midi Through", Qt::CaseInsensitive))
        midiInfoText->append("<span style=\"color:#FF6600\">The use of Midi Through is not recommended!</span>");
    else
        midiInfoText->append("<span style=\"color:gray\">Midi Input Device: " + midiInputCombo->currentText() +"</span>");

    if (midiOutputCombo->currentText() == "None")
        midiInfoText->append("<span style=\"color:red\">No Sound Output Device selected; Choose a Midi Output Device </span>");
    else if (midiOutputCombo->currentText().startsWith("Midi Through", Qt::CaseInsensitive))
        midiInfoText->append("<span style=\"color:#FF6600\">The use of Midi Through is not recommended!</span>");
    else if (midiOutputCombo->currentText().startsWith("Microsoft GS Wavetable SW Synth", Qt::CaseInsensitive))
        midiInfoText->append("<span style=\"color:black\">Note: the Microsoft SW Synth introduces an unwanted delay!</span>");
    else
        midiInfoText->append("<span style=\"color:gray\">Midi Output Device: " + midiOutputCombo->currentText() +"</span>");

    latencyFixLabel->setText(tr("%1 mSec").arg(m_latencyFix));

    updateFluidInfoText();
}

void GuiMidiSetupDialog::on_midiInputCombo_activated (int index)
{
    m_midiChanged = true;
    updateMidiInfoText();
}

void GuiMidiSetupDialog::on_midiOutputCombo_activated (int index)
{
    m_midiChanged = true;
    updateMidiInfoText();
}

void GuiMidiSetupDialog::on_latencyFixButton_clicked ( bool checked )
{
    bool ok;
    int latencyFix = QInputDialog::getInteger(this, tr("Enter a value for the latency fix in milliseconds"),
            tr(
            "The latency fix works by running the music ahead of what you<br>"
            "are playing to counteract the delay within the sound generator.<br><br>"
            "You will need a piano <b>with speakers</b> that are <b>turned up</b>.<br><br>"
            "Enter the time in milliseconds for the delay (1000 mSec = 1 sec)<br>"
            "(For the Microsoft GS Wavetable SW Synth try a value of 150)<br>"
            "If you are not sure enter a value of zero."),
                                      m_latencyFix, 0, 1000, 50, &ok);
    if (ok)
    {
        m_latencyFix = latencyFix;
        m_latencyChanged = true;
        updateMidiInfoText();
    }
}


void GuiMidiSetupDialog::accept()
{
    m_settings->setValue("Midi/Input", midiInputCombo->currentText());
    m_song->openMidiPort(CMidiDevice::MIDI_INPUT, midiInputCombo->currentText() );
    if (midiInputCombo->currentText().startsWith("None"))
        CChord::setPianoRange(PC_KEY_LOWEST_NOTE, PC_KEY_HIGHEST_NOTE);
    else
        CChord::setPianoRange(m_settings->value("Keyboard/LowestNote", 0).toInt(),
                          m_settings->value("Keyboard/HighestNote", 127).toInt());


    if (m_latencyChanged == false || m_midiChanged == true)
    {
        m_settings->setValue("Midi/Output", midiOutputCombo->currentText());
        m_song->openMidiPort(CMidiDevice::MIDI_OUTPUT, midiOutputCombo->currentText() );
        m_settings->updateWarningMessages();
    }

    m_settings->setValue("Midi/Latency", m_latencyFix);
    m_song->setLatencyFix(m_latencyFix);
    m_song->regenerateChordQueue();
    if (m_latencyChanged)
    {
        if( m_latencyFix> 0)
        {
            int rightSound = m_settings->value("Keyboard/RightSound", Cfg::defaultRightPatch()).toInt();
            m_settings->setValue("Keyboard/RightSoundPrevious", rightSound); // Save the current right sound
            // Mute the Piano if we are using the latency fix;
            m_settings->setValue("Keyboard/RightSound", 0);
            m_song->setPianoSoundPatches( -1, -2); // -1 means no sound and -2 means ignore this parameter
        }
        else
        {
            int previousRightSound = m_settings->value("Keyboard/RightSoundPrevious", Cfg::defaultRightPatch()).toInt();
            m_settings->setValue("Keyboard/RightSound", previousRightSound);
            m_song->setPianoSoundPatches(previousRightSound, -2); // -2 means ignore this parameter
        }
    }


    this->QDialog::accept();
}


void GuiMidiSetupDialog::updateFluidInfoText()
{
    QStringList soundFontNames = m_settings->getFluidSoundFontNames();
    soundFontList->clear();
    for (int i=0; i < soundFontNames.count(); i++)
    {
        int n = soundFontNames.at(i).lastIndexOf("/");
        soundFontList->addItem(soundFontNames.at(i).mid(n+1));
    }


    bool fontLoaded = (soundFontList->count() > 0) ? true : false;
    fluidRemoveButton->setEnabled(fontLoaded);

    fluidAddButton->setEnabled(soundFontList->count() < 2 ? true : false);
    fluidSettingsGroupBox->setEnabled(fontLoaded);

}


void GuiMidiSetupDialog::on_fluidAddButton_clicked ( bool checked )
{
    QString lastSoundFont;
    QStringList sfList = m_settings->getFluidSoundFontNames();
    if (sfList.size() > 0)
        lastSoundFont = sfList.last();

    QString soundFontName = QFileDialog::getOpenFileName(this,tr("Open SoundFont2 File for fluid synth"),
                            lastSoundFont, tr("SoundFont2 Files (*.sf2)"));
    if (!soundFontName.isEmpty())
        m_settings->addFluidSoundFontName(soundFontName);
    updateFluidInfoText();
}

void GuiMidiSetupDialog::on_fluidRemoveButton_clicked ( bool checked )
{
}
