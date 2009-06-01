/*********************************************************************************/
/*!
@file           GuiKeyboardSetupDialog.h

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

#ifndef __GUILEYBOARDSETUPDIALOG_H__
#define __GUILEYBOARDSETUPDIALOG_H__

#include <QtGui>

#include "Song.h"
#include "Settings.h"


#include "ui_GuiKeyboardSetupDialog.h"

class GuiKeyboardSetupDialog : public QDialog, private Ui::GuiKeyboardSetupDialog
{
    Q_OBJECT

public:
    GuiKeyboardSetupDialog(QWidget *parent = 0);

    void init(CSong* song, CSettings* settings);

private slots:
    void accept();
    void reject();


    void on_rightTestButton_pressed() {
        m_song->testWrongNoteSound(false);
        m_song->pcKeyPress( 'x', true);
    }
    void on_rightTestButton_released() {
        m_song->pcKeyPress( 'x', false);
    }
    void on_wrongTestButton_pressed() {
        m_song->testWrongNoteSound(true);
        m_song->pcKeyPress( 'x', true);
    }
    void on_wrongTestButton_released() {
        m_song->pcKeyPress( 'x', false);
    }

    void on_resetButton_clicked(bool clicked) {
        lowestNoteEdit->setText("0");
        highestNoteEdit->setText("127");
        updateInfoText();
    }

    void on_rightSoundCombo_activated (int index){ updateSounds(); }
    void on_wrongSoundCombo_activated (int index){ updateSounds(); }
    void on_lowestNoteEdit_editingFinished(){updateInfoText();}
    void on_highestNoteEdit_editingFinished(){updateInfoText();}
private:
    void keyPressEvent ( QKeyEvent * event );
    void keyReleaseEvent ( QKeyEvent * event );

    void updateSounds (){
        m_song->setPianoSoundPatches(rightSoundCombo->currentIndex() -1,
                                     wrongSoundCombo->currentIndex() -1, true);
    }

    void updateInfoText();

    CSettings* m_settings;
    CSong* m_song;
};

#endif //__GUILEYBOARDSETUPDIALOG_H__
