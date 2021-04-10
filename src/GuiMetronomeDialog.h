/*********************************************************************************/
/*!
@file           GuiSongDetailsDialog.h

@brief          xxxx.

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
/*********************************************************************************/

#ifndef __GUIMETRONOMEDIALOG_H__
#define __GUIMETRONOMEDIALOG_H__

#include <QtWidgets>

#include "Metronome.h"
#include "Settings.h"

#include "ui_GuiMetronomeDialog.h"

class GuiTopBar;
class CGLView;

class GuiMetronomeDialog : public QDialog, private Ui::GuiMetronomeDialog {
    Q_OBJECT

public:
    GuiMetronomeDialog(QWidget *parent = 0);
    void init(CSong* song, CSettings* settings, GuiTopBar* topBar);

private slots:
    void accept();
    void reject();
    void on_barCombo_activated (int index);
    void on_beatCombo_activated (int index);
    void on_subBeatCombo_activated (int index);
    
    void on_barSpin_valueChanged (int i);
    void on_beatSpin_valueChanged (int i);
    void on_subBeatSpin_valueChanged (int i);
    
    void on_metronomeCheck_stateChanged (int state);
    void on_previewSoundPush_clicked (bool clicked);
    void on_metronomeVolume_valueChanged(int value);

private:
    CSettings* m_settings;
    CSong* m_song;
    CTrackList* m_trackList;
    GuiTopBar* m_topBar;

    bool prevMetronomeActive;
    int prevMetronomeVolume;
    int prevMetronomeBarSound;
    int prevMetronomeBeatSound;
    int prevMetronomeBarVelocity;
    int prevMetronomeBeatVelocity;
};

#endif //__GUIMETRONOMEDIALOG_H__

