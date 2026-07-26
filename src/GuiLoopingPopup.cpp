/*!
    @file           GuiLoopingPopup.cpp

    @brief          xxx.

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

#include <QtWidgets>

#include "GuiLoopingPopup.h"
#include "GlView.h"

GuiLoopingPopup::GuiLoopingPopup(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    m_song = nullptr;
    setWindowTitle(tr("Continuous Looping"));
    setWindowFlags(Qt::Popup);
}

void GuiLoopingPopup::init(CSong* song)
{
    m_song = song;
    updateControls();
}

void GuiLoopingPopup::updateControls()
{
    QSignalBlocker loopBarsBlocker(loopBarsSpin);
    QSignalBlocker endBarBlocker(endBarSpin);

    endBarSpin->setMinimum(m_song->getPlayFromBar());
    loopBarsSpin->setValue(m_song->getLoopingBars());
    endBarSpin->setValue(m_song->getPlayUptoBar());
}

void GuiLoopingPopup::on_loopBarsSpin_valueChanged(double bars)
{
    if (!m_song) return;

    m_song->setLoopingBars( bars);
    updateControls();
}

void GuiLoopingPopup::on_endBarSpin_valueChanged(double endBar)
{
    if (!m_song) return;

    m_song->setLoopingBars(endBar - m_song->getPlayFromBar());
    updateControls();
}

void GuiLoopingPopup::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    QPushButton* parent = static_cast<QPushButton*> (parentWidget());
    if (parent)
        parent->setChecked(false);
}
