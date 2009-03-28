/*********************************************************************************/
/*!
@file           GuiLoopingPopup.h

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

#ifndef __GUILOOPINGPOPUP_H__
#define __GUILOOPINGPOPUP_H__


#include <QtGui>

#include "Song.h"


#include "ui_GuiLoopingPopup.h"

class CGLView;

class GuiLoopingPopup : public QWidget, private Ui::GuiLoopingPopup
{
    Q_OBJECT

public:
    GuiLoopingPopup(QWidget *parent = 0);
    void init(CSong* song);

private slots:
    void on_loopBarsSpin_valueChanged(double bars);
    void closeEvent(QCloseEvent *event);

private:
    void updateInfo();
    CSong* m_song;
};

#endif //__GUILOOPINGPOPUP_H__
