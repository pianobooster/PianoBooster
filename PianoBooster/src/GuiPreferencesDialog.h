/*********************************************************************************/
/*!
@file           GuiPreferencesDialog.h

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

#ifndef __GUIPREFERENCESDIALOG_H__
#define __GUIPREFERENCESDIALOG_H__


#include <QtGui>

#include "Song.h"
#include "Settings.h"


#include "ui_GuiPreferencesDialog.h"

class CGLView;

class GuiPreferencesDialog : public QDialog, private Ui::GuiPreferencesDialog
{
    Q_OBJECT

public:
    GuiPreferencesDialog(QWidget *parent = 0);

    void init(CSong* song, CSettings* settings, CGLView* glView);

private slots:
    void accept();

private:
    CSettings* m_settings;
    CSong* m_song;
    CGLView *m_glView;
};

#endif //__GUIPREFERENCESDIALOG_H__
