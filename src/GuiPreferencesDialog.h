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

    void on_staveColorSel_clicked();

    void on_staveDimColorSel_clicked();

    void on_noteColorSel_clicked();

    void on_noteDimColorSel_clicked();

    void on_playGoodColorSel_clicked();

    void on_playBadColorSel_clicked();

    void on_playStoppedColorSel_clicked();

    void on_bgColorSel_clicked();

    void on_noteNameColorSel_clicked();

    void on_barMarkerColorSel_clicked();

    void on_beatMarkerColorSel_clicked();

    void on_pianoBadColorSel_clicked();

    void on_playZoneBgColorSel_clicked();

    void on_playZoneMiddleColorSel_clicked();

    void on_playZoneEndLineColorSel_clicked();

    void showColorSelector(QPushButton * btn, QColor & color);

    void on_applyButton_clicked();
    
    void forceGlViewUpdate();

    void updateColorSelBtnBg();

    void setButtonBgColor(QPushButton * btn, CColor color);
    void setButtonBgColor(QPushButton * btn, QColor color);
    void translateColor(const CColor & ccolor, QColor & qcolor);

private:
    void initLanguageCombo();

    void initDisplayColors();
    void saveDisplayColors();

    CSettings* m_settings;
    CSong* m_song;
    CGLView *m_glView;

    QSpinBox * colorSpinBoxes[CSettings::colorCount*3] = {};

    QColor staveColor;
    QColor staveDimColor;
    QColor noteColor;
    QColor noteDimColor;
    QColor playedGoodColor;
    QColor playedBadColor;
    QColor playedStoppedColor;
    QColor backgroundColor;
    QColor barMarkerColor;
    QColor beatMarkerColor;
    QColor pianoGoodColor;
    QColor pianoBadColor;
    QColor noteNameColor;

    QColor playZoneAreaColor;
    QColor playZoneEndColor;
    QColor playZoneMiddleColor;
};

#endif //__GUIPREFERENCESDIALOG_H__
