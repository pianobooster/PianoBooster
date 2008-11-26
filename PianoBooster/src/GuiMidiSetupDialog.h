#ifndef __GUIMIDISETUPDIALOG_H__
#define __GUIMIDISETUPDIALOG_H__

/*!
    @file           GuiMidiSetupDialog.h

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

#include "Song.h"


#include "ui_GuiMidiSetupDialog.h"

class GuiMidiSetupDialog : public QDialog, private Ui::GuiMidiSettingsDialog
{
    Q_OBJECT

public:
    GuiMidiSetupDialog(QWidget *parent = 0);

    void init(CSong* song, QSettings* settings);

private slots:
    void accept();
    void on_midiInputCombo_activated (int index);
    void on_midiOutputCombo_activated (int index);

private:

    void updateMidiInfoText();
    QSettings* m_settings;
    CSong* m_song;
};

#endif //__GUIMIDISETUPDIALOG_H__
