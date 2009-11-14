/*!
    @file           GuiLoopingPopup.cpp

    @brief          xxx.

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

#include <QtGui>

#include "GuiLoopingPopup.h"
#include "GlView.h"

GuiLoopingPopup::GuiLoopingPopup(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    m_song = 0;
    setWindowTitle(tr("Continuous Looping"));
    setWindowFlags(Qt::Popup);
}


void GuiLoopingPopup::init(CSong* song)
{
    m_song = song;
    loopBarsSpin->setValue(m_song->getLoopingBars());
    updateInfo();
}

void GuiLoopingPopup::updateInfo()
{
    if (m_song->getLoopingBars() > 0.0)
        loopingText->setText(tr("Repeat End Bar: ") + QString().setNum(m_song->getPlayUptoBar()));
    else
        loopingText->setText(tr("Repeat Bar is disabled"));
}


void GuiLoopingPopup::on_loopBarsSpin_valueChanged(double bars)
{
    if (!m_song) return;

    m_song->setLoopingBars( bars);
    updateInfo();
}

void GuiLoopingPopup::closeEvent(QCloseEvent *event)
{
    QPushButton* parent = static_cast<QPushButton*> (parentWidget());
    if (parent)
        parent->setChecked(false);
}

