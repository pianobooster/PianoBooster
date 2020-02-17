/*!
    @file           GuiMidiSetupDialog.cpp

    @brief          xxx.

    @author         L. J. Barman

    Copyright (c)   2008-2013, L. J. Barman, all rights reserved

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

#include <QtWidgets>

#include "GuiMidiSetupDialog.h"


GuiMidiSetupDialog::GuiMidiSetupDialog(QWidget *parent)
    : QDialog(parent)
{
    m_song = 0;
    m_settings = 0;
    setupUi(this);
    m_latencyFix = 0;
    m_latencyChanged = false;
    midiSetupTabWidget->setCurrentIndex(0);

#ifndef EXPERIMENTAL_USE_FLUIDSYNTH
    midiSetupTabWidget->removeTab(midiSetupTabWidget->indexOf(tab_2));
#endif

    setWindowTitle(tr("Midi Setup"));
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
    sampleRateCombo->setValidator(new QIntValidator(0, 999999, this));

    updateMidiInfoText();

    audioDriverCombo->clear();
    audioDriverCombo->addItem("");
    audioDriverCombo->addItem("alsa");
    audioDriverCombo->addItem("file");
    audioDriverCombo->addItem("jack");
    audioDriverCombo->addItem("oss");
    audioDriverCombo->addItem("portaudio");
    audioDriverCombo->addItem("pulseaudio");

    setDefaultFluidSynth();

    connect(audioDriverCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(on_audioDriverCombo_currentIndexChanged(int)));

    if (m_settings->getFluidSoundFontNames().size()!=0){
        masterGainSpin->setValue(m_settings->value("FluidSynth/masterGainSpin","0.2").toDouble());
        bufferSizeSpin->setValue(m_settings->value("FluidSynth/bufferSizeSpin","").toInt());
        bufferCountsSpin->setValue(m_settings->value("FluidSynth/bufferCountsSpin","").toInt());
        reverbCheck->setChecked(m_settings->value("FluidSynth/reverbCheck","false").toBool());
        chorusCheck->setChecked(m_settings->value("FluidSynth/chorusCheck","false").toBool());

        audioDeviceLineEdit->setText(m_settings->value("FluidSynth/audioDeviceLineEdit","").toString());
        for (int i=0;i<audioDriverCombo->count();i++){
            if (audioDriverCombo->itemText(i)==m_settings->value("FluidSynth/audioDriverCombo","").toString()){
                audioDriverCombo->setCurrentIndex(i);
                break;
            }
        }
        sampleRateCombo->setCurrentText(m_settings->value("FluidSynth/sampleRateCombo").toString());
    }

    updateFluidInfoText();
}

void GuiMidiSetupDialog::updateMidiInfoText()
{

    midiInfoText->clear();

    if (midiInputCombo->currentIndex() == 0)
        midiInfoText->append("<span style=\"color:black\">" + tr("If you don't have a MIDI keyboard you can use the PC keyboard; 'X' is middle C.") + "</span>");
    else if (midiInputCombo->currentText().contains("Midi Through", Qt::CaseInsensitive))
        midiInfoText->append("<span style=\"color:#FF6600\">" + tr("The use of Midi Through is not recommended!") + "</span>");
    else
        midiInfoText->append("<span style=\"color:gray\">" + tr("Midi Input Device:") + " " + midiInputCombo->currentText() +"</span>");

    if (midiOutputCombo->currentText() == tr("None"))
        midiInfoText->append("<span style=\"color:red\">" + tr("No Sound Output Device selected; Choose a Midi Output Device") + "</span>");
    else if (midiOutputCombo->currentText().contains("Midi Through", Qt::CaseInsensitive))
        midiInfoText->append("<span style=\"color:#FF6600\">" + tr("The use of Midi Through is not recommended!") + "</span>");
    else if (midiOutputCombo->currentText().contains("Microsoft GS Wavetable", Qt::CaseInsensitive))
        midiInfoText->append("<span style=\"color:#FF6600\">" + tr("Note: the Microsoft GS Wavetable Synth introduces an unwanted delay!.") + "\n"
                                + tr("(Try a latency fix of 150msc)") + "</span>");
    else
        midiInfoText->append("<span style=\"color:gray\">" + tr("Midi Output Device:") + " " + midiOutputCombo->currentText() +"</span>");

    latencyFixLabel->setText(tr("%1 mSec").arg(m_latencyFix));

    updateFluidInfoText();
}

void GuiMidiSetupDialog::on_midiInputCombo_activated (int index)
{
    updateMidiInfoText();
}

void GuiMidiSetupDialog::on_midiOutputCombo_activated (int index)
{
    updateMidiInfoText();
}

void GuiMidiSetupDialog::on_latencyFixButton_clicked ( bool checked )
{
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



    // save FluidSynth settings
    if (m_settings->getFluidSoundFontNames().size()==0){
        m_settings->remove("FluidSynth");
    }else{
        m_settings->setValue("FluidSynth/masterGainSpin",QString::number(masterGainSpin->value(),'f',2));
        m_settings->setValue("FluidSynth/bufferSizeSpin",bufferSizeSpin->value());
        m_settings->setValue("FluidSynth/bufferCountsSpin",bufferCountsSpin->value());
        m_settings->setValue("FluidSynth/reverbCheck",reverbCheck->isChecked());
        m_settings->setValue("FluidSynth/chorusCheck",chorusCheck->isChecked());
        m_settings->setValue("FluidSynth/audioDriverCombo",audioDriverCombo->currentText());
        if (audioDriverCombo->currentText()=="alsa"){
            m_settings->setValue("FluidSynth/audioDeviceLineEdit",audioDeviceLineEdit->text());
        }else{
            m_settings->setValue("FluidSynth/audioDeviceLineEdit","");
        }
        m_settings->setValue("FluidSynth/sampleRateCombo",sampleRateCombo->currentText());
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

void GuiMidiSetupDialog::setDefaultFluidSynth(){
    masterGainSpin->setValue(0.4);
    bufferSizeSpin->setValue(128);
    bufferCountsSpin->setValue(6);
    reverbCheck->setChecked(false);
    chorusCheck->setChecked(false);
    #if defined (Q_OS_UNIX) || defined (Q_OS_DARWIN)
    audioDriverCombo->setCurrentIndex(3);
    audioDeviceLineEdit->setText("");
    #endif
    #if defined (Q_OS_LINUX)
    audioDriverCombo->setCurrentIndex(1);
    audioDeviceLineEdit->setText("plughw:0");
    #endif
    sampleRateCombo->setCurrentIndex(0);

}


void GuiMidiSetupDialog::on_fluidAddButton_clicked ( bool checked )
{
    QStringList possibleSoundFontFolders;
#if defined (Q_OS_LINUX) || defined (Q_OS_UNIX)
    possibleSoundFontFolders.push_back("/usr/share/soundfonts");
    possibleSoundFontFolders.push_back("/usr/share/sounds/sf2");
#endif

    QString lastSoundFont = QDir::homePath();

    for (QString soundFontFolder:possibleSoundFontFolders){
        QDir dir(soundFontFolder);
        if (dir.exists()){
            lastSoundFont=soundFontFolder;
            break;
        }
    }


    QString soundFontName = QFileDialog::getOpenFileName(this,tr("Open SoundFont2 File for fluidsynth"),
                            lastSoundFont, tr("SoundFont2 Files (*.sf2)"));
    if (soundFontName.isEmpty()) return;

    m_settings->addFluidSoundFontName(soundFontName);

    updateFluidInfoText();

    m_settings->setValue("FluidSynth/SoundFont2_1","");
    m_settings->setValue("FluidSynth/SoundFont2_2","");
    for (int i=0;i<m_settings->getFluidSoundFontNames().size();i++){
        m_settings->setValue("FluidSynth/SoundFont2_"+QString::number(1+i),m_settings->getFluidSoundFontNames().at(i));
    }}

void GuiMidiSetupDialog::on_fluidRemoveButton_clicked ( bool checked ){
    if (soundFontList->currentRow()==-1) return;

    QStringList soundFontNames = m_settings->getFluidSoundFontNames();

    m_settings->removeFluidSoundFontName(soundFontNames.at(soundFontList->currentRow()));
    soundFontList->removeItemWidget(soundFontList->currentItem());

    updateFluidInfoText();

    m_settings->setValue("FluidSynth/SoundFont2_1","");
    m_settings->setValue("FluidSynth/SoundFont2_2","");
    for (int i=0;i<m_settings->getFluidSoundFontNames().size();i++){
        m_settings->setValue("FluidSynth/SoundFont2_"+QString::number(1+i),m_settings->getFluidSoundFontNames().at(i));
    }

    if (m_settings->getFluidSoundFontNames().size()==0){
        setDefaultFluidSynth();
        m_settings->remove("Fluid");
    }

}

void GuiMidiSetupDialog::on_audioDriverCombo_currentIndexChanged(int index){
    if (audioDriverCombo->currentText()=="alsa"){
        audioDeviceLineEdit->setEnabled(true);
        if (audioDeviceLineEdit->text().isEmpty()){
            audioDeviceLineEdit->setText("plughw:0");
        }
    }else{
        audioDeviceLineEdit->setEnabled(false);
        audioDeviceLineEdit->setText("");
    }
}
