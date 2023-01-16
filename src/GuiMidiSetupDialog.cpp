/*!
    @file           GuiMidiSetupDialog.cpp

    @brief          xxx.

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

#include <QFileInfo>
#include <QtWidgets>

#include "GuiMidiSetupDialog.h"

#if WITH_INTERNAL_FLUIDSYNTH
#include "MidiDeviceFluidSynth.h"
#endif

GuiMidiSetupDialog::GuiMidiSetupDialog(QWidget *parent)
    : QDialog(parent)
{
    m_song = nullptr;
    m_settings = nullptr;
    setupUi(this);
    m_latencyFix = 0;
    m_latencyChanged = false;
    midiSetupTabWidget->setCurrentIndex(0);

#ifndef WITH_INTERNAL_FLUIDSYNTH
    midiSetupTabWidget->removeTab(midiSetupTabWidget->indexOf(tab_2));
#endif

    setWindowTitle(tr("MIDI Setup"));
}

void GuiMidiSetupDialog::init(CSong* song, CSettings* settings)
{
    m_song = song;
    m_settings = settings;

    // Check inputs.
    QString portName;

    m_latencyFix = m_song->getLatencyFix();

    refreshMidiInputCombo();
    refreshMidiOutputCombo();
#if WITH_INTERNAL_FLUIDSYNTH
    masterGainSpin->setValue(FLUID_DEFAULT_GAIN);
#endif
    reverbCheck->setChecked(false);
    chorusCheck->setChecked(false);

    sampleRateCombo->addItems({"22050", "44100","48000", "88200","96000"});
    sampleRateCombo->setValidator(new QIntValidator(22050, 96000, this));
    bufferSizeCombo->addItems({"64", "128", "512", "1024", "2024", "4096","8192"});
    bufferSizeCombo->setValidator(new QIntValidator(64, 8192, this));
    bufferSizeCombo->setCurrentIndex(1);
    bufferCountCombo->addItems({"2","4", "8","16", "32", "64"});
    bufferCountCombo->setValidator(new QIntValidator(2, 64, this));
    bufferCountCombo->setCurrentIndex(1);

    updateMidiInfoText();

    audioDriverCombo->clear();

#if defined (Q_OS_LINUX)
    audioDriverCombo->addItems({"pulseaudio", "alsa"});
#elif defined (Q_OS_DARWIN)
    audioDriverCombo->addItems({"coreaudio", "portaudio"});
#elif defined (Q_OS_UNIX)
    audioDriverCombo->addItems({"pulseaudio"});
#endif

    if (m_settings->getFluidSoundFontNames().size()>0){
        masterGainSpin->setValue(m_settings->value("FluidSynth/masterGainSpin","40").toInt());
        reverbCheck->setChecked(m_settings->value("FluidSynth/reverbCheck","false").toBool());
        chorusCheck->setChecked(m_settings->value("FluidSynth/chorusCheck","false").toBool());
        setComboFromSetting(audioDriverCombo, "FluidSynth/audioDriverCombo","pulseaudio");
        setComboFromSetting(sampleRateCombo, "FluidSynth/sampleRateCombo","22050");
        setComboFromSetting(bufferSizeCombo, "FluidSynth/bufferSizeCombo","128");
        setComboFromSetting(bufferCountCombo, "FluidSynth/bufferCountCombo","4");
     }

    updateFluidInfoStatus();
}

void GuiMidiSetupDialog::setComboFromSetting(QComboBox *combo, const QString &key, const QVariant &defaultValue) {
    QString value = m_settings->value(key, defaultValue).toString();
    int index = combo->findText(value);

    if ( index != -1 ) { // -1 for not found
        combo->setCurrentIndex(index);
    } else {
        combo->setCurrentText(value);
    }
}

void GuiMidiSetupDialog::refreshMidiInputCombo()
{
    int i = 0;
    midiInputCombo->clear();
    midiInputCombo->addItem(tr("None (PC Keyboard)"));
    midiInputCombo->addItems(m_song->getMidiPortList(CMidiDevice::MIDI_INPUT));
    i = midiInputCombo->findText(m_settings->value("Midi/Input").toString());
    if (i!=-1)
        midiInputCombo->setCurrentIndex(i);
}

void GuiMidiSetupDialog::refreshMidiOutputCombo()
{
    int i = 0;

    // Check outputs.
    midiOutputCombo->clear();
    midiOutputCombo->addItem(tr("None"));
    midiOutputCombo->addItems(m_song->getMidiPortList(CMidiDevice::MIDI_OUTPUT));
    i = midiOutputCombo->findText(m_settings->value("Midi/Output").toString());
    if (i!=-1)
        midiOutputCombo->setCurrentIndex(i);
}

void GuiMidiSetupDialog::updateMidiInfoText()
{
    midiInfoText->clear();

    if (midiInputCombo->currentIndex() == 0)
        midiInfoText->append("<span style=\"color:black\">" + tr("If you don't have a MIDI keyboard you can use the PC keyboard; 'X' is middle C.") + "</span>");
    else if (midiInputCombo->currentText().contains("Midi Through", Qt::CaseInsensitive))
        midiInfoText->append("<span style=\"color:#FF6600\">" + tr("The use of MIDI Through is not recommended!") + "</span>");
    else
        midiInfoText->append("<span style=\"color:gray\">" + tr("MIDI Input Device:") + " " + midiInputCombo->currentText() +"</span>");

    if (midiOutputCombo->currentText() == tr("None"))
        midiInfoText->append("<span style=\"color:red\">" + tr("No Sound Output Device selected; Choose a MIDI Output Device") + "</span>");
    else if (midiOutputCombo->currentText().contains("MIDI Through", Qt::CaseInsensitive))
        midiInfoText->append("<span style=\"color:#FF6600\">" + tr("The use of MIDI Through is not recommended!") + "</span>");
    else if (midiOutputCombo->currentText().contains("Microsoft GS Wavetable", Qt::CaseInsensitive))
        midiInfoText->append("<span style=\"color:#FF6600\">" + tr("Note: the Microsoft GS Wavetable Synth introduces an unwanted delay!") + "\n"
                                + tr("(Try a latency fix of 150msc)") + "</span>");
    else
        midiInfoText->append("<span style=\"color:gray\">" + tr("MIDI Output Device:") + " " + midiOutputCombo->currentText() +"</span>");

    latencyFixLabel->setText(tr("%1 mSec").arg(m_latencyFix));

    updateFluidInfoStatus();
}

void GuiMidiSetupDialog::on_midiInputCombo_activated (int index)
{
    Q_UNUSED(index)
    updateMidiInfoText();
}

void GuiMidiSetupDialog::on_midiOutputCombo_activated (int index)
{
    Q_UNUSED(index)
    updateMidiInfoText();
}

void GuiMidiSetupDialog::on_latencyFixButton_clicked ( bool checked )
{
    Q_UNUSED(checked)
    bool ok;
    int latencyFix = QInputDialog::getInt(this, tr("Enter a value for the latency fix in milliseconds"),
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
    // save FluidSynth settings
    if (m_settings->getFluidSoundFontNames().size()==0){
        m_settings->remove("FluidSynth");
    }else{
        m_settings->setValue("FluidSynth/masterGainSpin",masterGainSpin->value());
        m_settings->setValue("FluidSynth/bufferSizeCombo", bufferSizeCombo->currentText());
        m_settings->setValue("FluidSynth/bufferCountCombo", bufferCountCombo->currentText());
        m_settings->setValue("FluidSynth/reverbCheck",reverbCheck->isChecked());
        m_settings->setValue("FluidSynth/chorusCheck",chorusCheck->isChecked());
        m_settings->setValue("FluidSynth/audioDriverCombo",audioDriverCombo->currentText());
        m_settings->setValue("FluidSynth/sampleRateCombo",sampleRateCombo->currentText());
    }

    m_settings->saveSoundFontSettings();

    m_settings->setValue("Midi/Input", midiInputCombo->currentText());
    m_song->openMidiPort(CMidiDevice::MIDI_INPUT, midiInputCombo->currentText() );
    if (midiInputCombo->currentText().startsWith(tr("None")))
        CChord::setPianoRange(PC_KEY_LOWEST_NOTE, PC_KEY_HIGHEST_NOTE);
    else
        CChord::setPianoRange(m_settings->value("Keyboard/LowestNote", 0).toInt(),
                          m_settings->value("Keyboard/HighestNote", 127).toInt());

    if (midiOutputCombo->currentIndex()==0){
        m_settings->setValue("Midi/Output", "");
        m_song->openMidiPort(CMidiDevice::MIDI_OUTPUT,"");
    }else{
        m_settings->setValue("Midi/Output", midiOutputCombo->currentText());
        m_song->openMidiPort(CMidiDevice::MIDI_OUTPUT, midiOutputCombo->currentText() );
    }
    m_settings->updateWarningMessages();

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
        m_latencyChanged = false;
    }

    this->QDialog::accept();
}

void GuiMidiSetupDialog::updateFluidInfoStatus()
{
    QStringList soundFontNames = m_settings->getFluidSoundFontNames();
    soundFontList->clear();
    if (!m_settings->getFluidSoundFontNames().isEmpty())
    {
        QFileInfo fileInfo = QFileInfo(m_settings->getFluidSoundFontNames().at(0));
        if (fileInfo.exists())
        {
            soundFontList->addItem(fileInfo.fileName());
        }
    }

    bool fontLoaded = (soundFontList->count() > 0) ? true : false;
    fluidClearButton->setEnabled(fontLoaded);

    fluidLoadButton->setEnabled(soundFontList->count() < 2 ? true : false);

    fluidSettingsGroupBox->setEnabled(fontLoaded);
}

void GuiMidiSetupDialog::on_fluidLoadButton_clicked ( bool checked )
{
    Q_UNUSED(checked)
#if WITH_INTERNAL_FLUIDSYNTH
    QString lastSoundFont = m_settings->value("LastSoundFontDir","").toString();

     if (lastSoundFont.isEmpty()) {

        lastSoundFont = QDir::homePath();

        QStringList possibleSoundFontFolders;
#if defined (Q_OS_LINUX) || defined (Q_OS_UNIX)
        possibleSoundFontFolders.push_back("/usr/share/soundfonts");
        possibleSoundFontFolders.push_back("/usr/share/sounds/sf2");
#endif
        for (const QString &soundFontFolder : possibleSoundFontFolders){
            if (QDir(soundFontFolder).exists()){
                lastSoundFont=soundFontFolder;
                break;
            }
        }
    }

    const auto soundFontFile = QFileDialog::getOpenFileName(this, tr("Open SoundFont File for fluidsynth"),
                            lastSoundFont, tr("SoundFont Files (*.sf2 *.sf3)"));
    if (soundFontFile.isEmpty()) return;

    const auto soundFontInfo = QFileInfo(soundFontFile);
    m_settings->setFluidSoundFontNames(soundFontInfo.filePath());
    m_settings->setValue("LastSoundFontDir", soundFontInfo.path());

    if (m_settings->isNewSoundFontEntered())
    {
        int i = midiOutputCombo->findText(CMidiDeviceFluidSynth::getFluidInternalName());
        if (i==-1)
           midiOutputCombo->addItem(CMidiDeviceFluidSynth::getFluidInternalName());
        i = midiOutputCombo->findText(CMidiDeviceFluidSynth::getFluidInternalName());
        if (i!=-1)
            midiOutputCombo->setCurrentIndex(i);
    }
    updateFluidInfoStatus();
    updateMidiInfoText();
#endif
}

void GuiMidiSetupDialog::on_fluidClearButton_clicked( bool checked ){
    Q_UNUSED(checked)
#if WITH_INTERNAL_FLUIDSYNTH
    m_settings->clearFluidSoundFontNames();
    int i = midiOutputCombo->findText(CMidiDeviceFluidSynth::getFluidInternalName());
    if (i>=0)
    {
       midiOutputCombo->removeItem(i);
       midiOutputCombo->setCurrentIndex(0);
    }
    updateFluidInfoStatus();
#endif
}
