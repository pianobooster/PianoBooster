/*********************************************************************************/
/*!
@file           GuiSongDetailsDialog.h

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

#ifndef __GUISONGDETAILSDIALOG_H__
#define __GUISONGDETAILSDIALOG_H__


#include <QtGui>

#include "Song.h"
#include "Settings.h"


#include "ui_GuiSongDetailsDialog.h"

class CGLView;

class GuiSongDetailsDialog : public QDialog, private Ui::GuiSongDetailsDialog
{
    Q_OBJECT

public:
    GuiSongDetailsDialog(QWidget *parent = 0);
    void init(CSong* song, CSettings* settings);

private slots:
    void accept();
    void on_leftHandChannelCombo_activated (int index);
    void on_rightHandChannelCombo_activated (int index);

private:
    void updateSongInfoText();

    CSettings* m_settings;
    CSong* m_song;
    CTrackList* m_trackList;
};

#endif //__GUISONGDETAILSDIALOG_H__
