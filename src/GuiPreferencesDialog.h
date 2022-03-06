/*********************************************************************************/
/*!
@file           GuiPreferencesDialog.h

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

#ifndef __GUIPREFERENCESDIALOG_H__
#define __GUIPREFERENCESDIALOG_H__

#include <QtWidgets>

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

    void on_menuClrSel_clicked();

    void on_menuSelectedClrSel_clicked();

    void on_staveClrSel_clicked();

    void on_staveDimClrSel_clicked();

    void on_noteClrSel_clicked();

    void on_noteDimClrSel_clicked();

    void on_playGoodClrSel_clicked();

    void on_playBadClrSel_clicked();

    void on_playStoppedClrSel_clicked();

    void on_bgClrSel_clicked();

    void on_noteNameClrSel_clicked();

    void on_barMarkerClrSel_clicked();

    void on_beatMarkerClrSel_clicked();

    void on_pianoBadClrSel_clicked();

    void on_playZoneBgClrSel_clicked();

    void on_playZoneMiddleClrSel_clicked();

    void on_playZoneEndLineClrSel_clicked();

    void showColorSelector(QSpinBox * red, QSpinBox * green, QSpinBox * blue);

    void on_applyButton_clicked();
    
    void forceGlViewUpdate();

    void updateColorSelBtnBg();
    void addColorChangeListener();
    void colorSpinBoxChanged(int val);

    void setButtonBgColor(QPushButton * btn, CColor color);

private:
    void initLanguageCombo();

    void initDisplayColors();
    void saveDisplayColors();

    CSettings* m_settings;
    CSong* m_song;
    CGLView *m_glView;

    QSpinBox * colorSpinBoxes[CSettings::colorCount*3] = {};
};

#endif //__GUIPREFERENCESDIALOG_H__
