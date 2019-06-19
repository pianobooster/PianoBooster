/*********************************************************************************/
/*!
@file           GuiTopBar.cpp

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
#ifndef __GUITOPBAR_H__
#define __GUITOPBAR_H__

#include <QtWidgets>

#include "Song.h"
#include "Score.h"
#include "TrackList.h"
#include "Settings.h"

#include "ui_GuiTopBar.h"

class GuiTopBar : public QWidget, private Ui::GuiTopBar
{
    Q_OBJECT

public:
    GuiTopBar(QWidget *parent, CSettings* settings);

    void init(CSong* songObj);

    void refresh(bool reset);

    void setPlayButtonState(bool checked, bool atTheEnd = false);

    void setSpeed(int value){ speedSpin->setValue(value); }
    int getSpeed(){return speedSpin->value();}

    void updateTranslate();

public slots:
    void on_playFromStartButton_clicked(bool clicked);
    void on_playButton_clicked(bool clicked);

private slots:
    void on_speedSpin_valueChanged(int speed);
    void on_startBarSpin_valueChanged(double bar);

    void on_transposeSpin_valueChanged(int value);
    void on_keyCombo_activated(int index);

    void on_majorCombo_activated(int index)
    {
        reloadKeyCombo((index == 0)?true:false);
    }

    void on_saveBarButton_clicked(bool clicked);
    void on_loopingBarsPopupButton_clicked(bool clicked);


private:
    bool eventFilter(QObject *obj, QEvent *event);
    void reloadKeyCombo(bool major);

    CSong* m_song;
    CSettings* m_settings;

    QMap<QWidget*,QMap<QString,QString>> listWidgetsRetranslateUi;
    QMap<QAction*,QMap<QString,QString>> listActionsRetranslateUi;

    bool m_atTheEndOfTheSong;
};

#endif //__GUITOPBAR_H__
