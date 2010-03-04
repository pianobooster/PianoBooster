/*********************************************************************************/
/*!
@file           GuiKeyboardSetupDialog.cpp

@brief          xxxx.

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

#include <QtGui>


#include "GuiKeyboardSetupDialog.h"

#include "rtmidi/RtMidi.h"

GuiKeyboardSetupDialog::GuiKeyboardSetupDialog(QWidget *parent)
    : QDialog(parent)
{
    m_song = 0;
    setupUi(this);
    setWindowTitle(tr("Piano Keyboard Settings"));
}

void GuiKeyboardSetupDialog::init(CSong* song, CSettings* settings)
{
    m_song = song;
    m_settings = settings;

    // Check inputs.
    QString programName;
    int i;

    i = 0;
    while (true)
    {
        programName = CTrackList::getProgramName(i);
        if (programName.isEmpty())
            break;
        rightSoundCombo->addItem(programName);
        wrongSoundCombo->addItem(programName);
        i++;
    }

    int program = m_settings->value("Keyboard/RightSound", Cfg::defaultRightPatch()).toInt();
    rightSoundCombo->setCurrentIndex(program);
    program = m_settings->value("Keyboard/WrongSound", Cfg::defaultWrongPatch()).toInt();
    wrongSoundCombo->setCurrentIndex(program);
    int lowestNote = m_settings->value("Keyboard/LowestNote", "0").toInt();
    int highestNote = m_settings->value("Keyboard/HighestNote", "127").toInt();

    QString midiInputName = m_settings->value("Midi/Input").toString();
    if (midiInputName.startsWith(tr("None"), Qt::CaseInsensitive))
    {
        lowestNote = PC_KEY_LOWEST_NOTE;
        highestNote = PC_KEY_HIGHEST_NOTE;
        lowestNoteEdit->setEnabled(false);
        highestNoteEdit->setEnabled(false);
        resetButton->setEnabled(false);
    }

    lowestNoteEdit->setText(QString().setNum(lowestNote));
    highestNoteEdit->setText(QString().setNum(highestNote));
    updateInfoText();
    rightVolumeLabel->hide(); // fixme Hide for now
    rightVolumeSpin->hide();
    wrongVolumeLabel->hide();
    wrongVolumeSpin->hide();
}

void GuiKeyboardSetupDialog::updateInfoText()
{
    QString str;
    keyboardInfoText->clear();
    int lowestNote = lowestNoteEdit->text().toInt();
    int highestNote = highestNoteEdit->text().toInt();
    lowestNote = qBound(0, lowestNote, 127);
    highestNote = qBound(0, highestNote, 127);
    lowestNoteEdit->setText(QString().setNum(lowestNote));
    highestNoteEdit->setText(QString().setNum(highestNote));
    int noteRange = highestNote - lowestNote;
    keyboardInfoText->append("<span style=\"color:black\">" + tr("Choose the right and wrong sound for your playing.") + "</span>");
    if (!lowestNoteEdit->isEnabled())
        str = "<span style=\"color:black\">" + tr("You can use the PC keyboard instead of a MIDI keyboard; 'x' is middle C.") + "</span>";
    else if (noteRange > 0)
        str = "<span style=\"color:black\">" + QString(tr("Your keyboard range is <b>octaves %1</b> and <b>semitones %2</b>; 60 is middle C.")).arg(noteRange/MIDI_OCTAVE).arg(noteRange%MIDI_OCTAVE) + "</span>";
    else
        str = "<span style=\"color:red\">" + tr("Oops, you have <b>0 notes</b> on your keyboard!") + "</span>";

    keyboardInfoText->append(str);
}

void GuiKeyboardSetupDialog::keyPressEvent ( QKeyEvent * event )
{
    if (event->text().length() == 0)
        return;

    if (event->isAutoRepeat() == true)
        return;

    int c = event->text().toAscii().at(0);
    m_song->pcKeyPress( c, true);
}

void GuiKeyboardSetupDialog::keyReleaseEvent ( QKeyEvent * event )
{
    if (event->isAutoRepeat() == true)
        return;

    if (event->text().length() == 0)
        return;

    int c = event->text().toAscii().at(0);
    m_song->pcKeyPress( c, false);
}


void GuiKeyboardSetupDialog::accept()
{
    m_settings->setValue("Keyboard/RightSound", rightSoundCombo->currentIndex());
    m_settings->setValue("Keyboard/WrongSound", wrongSoundCombo->currentIndex());
    m_settings->setValue("Keyboard/RightSoundPrevious", rightSoundCombo->currentIndex());
    int lowestNote = lowestNoteEdit->text().toInt();
    int highestNote = highestNoteEdit->text().toInt();
    lowestNote = qBound(0, lowestNote, 127);
    highestNote = qBound(0, highestNote, 127);
    CChord::setPianoRange(lowestNote, highestNote);
    if (lowestNoteEdit->isEnabled())
    {
        m_settings->setValue("Keyboard/LowestNote", lowestNote);
        m_settings->setValue("Keyboard/HighestNote", highestNote);
    }
    m_song->testWrongNoteSound(false);
    m_song->regenerateChordQueue();
    this->QDialog::accept();
}


void GuiKeyboardSetupDialog::reject()
{
    m_song->testWrongNoteSound(false);
    m_song->setPianoSoundPatches(m_settings->value("Keyboard/RightSound", Cfg::defaultRightPatch()).toInt() - 1,
                                 m_settings->value("Keyboard/WrongSound", Cfg::defaultWrongPatch()).toInt() - 1, true);

    this->QDialog::reject();
}

