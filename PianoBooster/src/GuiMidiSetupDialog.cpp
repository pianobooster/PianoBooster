/*!
    @file           GuiMidiSetupDialog.cpp

    @brief          xxx.

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

#include <QtGui>

#include "GuiMidiSetupDialog.h"

//#include "rtmidi/RtTimer.h"

GuiMidiSetupDialog::GuiMidiSetupDialog(QWidget *parent)
    : QDialog(parent)
{
    m_song = 0;
    m_settings = 0;
    setupUi(this);
    m_latencyFix = 0;
    m_latencyChanged = false;
    m_midiChanged = false;

}


void GuiMidiSetupDialog::init(CSong* song, QSettings* settings)
{
    m_song = song;
    m_settings = settings;

    // Check inputs.
    QString portName;
    int i = 0;

    m_latencyFix = m_song->getLatencyFix();

    midiInputCombo->addItem(tr("None (PC Keyboard)"));
    while (true)
    {
        portName = song->getMidiPortName(0, i++).c_str();
        if (portName.startsWith("RtMidi"))
            continue;

        if (portName.size() == 0)
            break;
        midiInputCombo->addItem(portName);
    }

    // Check outputs.
    midiOutputCombo->addItem(tr("None"));
    i = 0;
    while (true)
    {
        portName = song->getMidiPortName(1, i++).c_str();
        if (portName.startsWith("RtMidi"))
            continue;
        if (portName.size() == 0)
            break;
        midiOutputCombo->addItem(portName);
    }

    i = midiInputCombo->findText(m_settings->value("midi/input").toString());
    if (i!=-1)
        midiInputCombo->setCurrentIndex(i);
    i = midiOutputCombo->findText(m_settings->value("midi/output").toString());
    if (i!=-1)
        midiOutputCombo->setCurrentIndex(i);

    //latencyFixEdit->hide(); fixme
    //latencyFixLabel->hide();
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

}

void GuiMidiSetupDialog::on_midiInputCombo_activated (int index)
{
    updateMidiInfoText();
    m_midiChanged = true;
}

void GuiMidiSetupDialog::on_midiOutputCombo_activated (int index)
{
    updateMidiInfoText();
    m_midiChanged = true;
    m_latencyFix = 0;
}

void GuiMidiSetupDialog::on_latencyFixButton_clicked ( bool checked )
{
    bool ok;
    int latencyFix = QInputDialog::getInteger(this, tr("Enter a value for the latency fix in milliseconds"),
            tr(
            "The latency fix works by running the music ahead of what you<br>"
            "are playing to conteract the delay within the sound generator.<br>"
            "You will need a piano <b>with speakers</b> that are <b>turned up</b>.<br>"
            "<i>It is not ideal, a low latency sound generator is recommend.</i><br>"
            "Enter the time in milliseconds for the delay (1000 mSec = 1 sec)<br>"
            "(For the Microsoft GS Wavetable SW Synth try a value of 350 msec)<br>"
            "If you are not sure enter a value of zero"


            /*"The value entered here tries to compensate for the latency problems on the sound generator\n"
            "To use this turn up the sound on your piano keyboard\n"
            "(if your piano keyboard does not have it own speakers then this fix will not work)"

            " see the website for more details"*/),
                                      m_latencyFix, 0, 1000, 50, &ok);
    if (ok)
    {
        m_latencyFix = latencyFix;
        updateMidiInfoText();
        m_latencyChanged = true;
    }

}


void GuiMidiSetupDialog::accept()
{
    m_settings->setValue("midi/input", midiInputCombo->currentText());
    m_song->openMidiPort(0,string(midiInputCombo->currentText().toAscii()));
    if (midiInputCombo->currentText().startsWith("None"))
        CChord::setPianoRange(PC_KEY_LOWEST_NOTE, PC_KEY_HIGHEST_NOTE);
    else
        CChord::setPianoRange(m_settings->value("Keyboard/lowestNote", 0).toInt(),
                          m_settings->value("Keyboard/highestNote", 127).toInt());


    if (m_latencyChanged == false || m_midiChanged == true)
    {
        m_settings->setValue("midi/output", midiOutputCombo->currentText());
        m_song->openMidiPort(1,string(midiOutputCombo->currentText().toAscii()));
    }

    m_settings->setValue("midi/latency", m_latencyFix);
    m_song->setLatencyFix(m_latencyFix);
    m_song->regenerateChordQueue();
    this->QDialog::accept();
}
