/*********************************************************************************/
/*!
@file           GuiTopBar.cpp

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

#include <QtGui>

#include "GuiTopBar.h"
#include "TrackList.h"
#include "GuiLoopingPopup.h"


GuiTopBar::GuiTopBar(QWidget *parent, CSettings* settings)
    : QWidget(parent), m_settings(settings)
{

    m_atTheEndOfTheSong = false;
    m_song = 0;
    setupUi(this);

    parent->installEventFilter(this);

    speedSpin->setMaximum(200);
    speedSpin->setMinimum(20);
    speedSpin->setSuffix(" %");
    speedSpin->setSingleStep(2);
    speedSpin->setValue(100);

    transposeSpin->setMaximum(12);
    transposeSpin->setMinimum(-12);

    majorCombo->addItem(tr("Major"));
    majorCombo->addItem(tr("Minor"));
    setMaximumHeight(30);
    setMaximumSize(QSize(16777215, 30));
}

void GuiTopBar::init(CSong* songObj, CTrackList* trackList)
{
    m_song = songObj;
    reloadKeyCombo(true);
}

void GuiTopBar::refresh(bool reset)
{
    if (reset == true)
    {
        majorCombo->setCurrentIndex(0);
        reloadKeyCombo(true);
        transposeSpin->setValue(0);
        startBarSpin->setValue(0);
    }
    int index = 0;
    if (m_song)
        index = CStavePos::getKeySignature() + 6;
    if (index >= 0 && index < keyCombo->count())
        keyCombo->setCurrentIndex(index);

}

void GuiTopBar::reloadKeyCombo(bool major)
{
    keyCombo->clear();
    if (major)
    {
        keyCombo->addItem("Gb"); // -6
        keyCombo->addItem("Db"); // -5
        keyCombo->addItem("Ab"); // -4
        keyCombo->addItem("Eb"); // -3
        keyCombo->addItem("Bb"); // -2
        keyCombo->addItem("F "); // -1
        keyCombo->addItem("C");  // 0
        keyCombo->addItem("G "); // 1
        keyCombo->addItem("D "); // 2
        keyCombo->addItem("A "); // 3
        keyCombo->addItem("E "); // 4
        keyCombo->addItem("B "); // 5
        keyCombo->addItem("F#"); // 6
    }
    else
    {
        keyCombo->addItem("Eb");
        keyCombo->addItem("Bb");
        keyCombo->addItem("F");
        keyCombo->addItem("C");
        keyCombo->addItem("G");
        keyCombo->addItem("D ");
        keyCombo->addItem("A");
        keyCombo->addItem("E ");
        keyCombo->addItem("B ");
        keyCombo->addItem("F#");
        keyCombo->addItem("G#");
        keyCombo->addItem("C#");
        keyCombo->addItem("D#");
    }
    refresh(false);
}

void GuiTopBar::on_keyCombo_activated(int index)
{
    CStavePos::setKeySignature(index - 6, 0);
    m_song->refreshScroll();
}

void GuiTopBar::on_transposeSpin_valueChanged(int value)
{
    unsigned int i;         //C  Db  D  Eb  E  F   F# G  Ab  A  Bb  B
    const int nextKey[] = {   0, -5, 2, -3, 4, -1, 6, 1, -4, 3, -2, 5};
    if (!m_song) return;
    int diff = value - m_song->getTranspose();
    int oldValue = CStavePos::getKeySignature();
        if (oldValue == -6)
            oldValue = 6; // if key is Eb change to D#

    // Find the old value in the table
    for (i=0; i < arraySize(nextKey); i++)
    {
        if (oldValue == nextKey[i])
            break;
    }

    int newValue = nextKey[(diff  + i + arraySize(nextKey)) % arraySize(nextKey) ];

    CStavePos::setKeySignature( newValue, 0 );

    newValue += 6;
    keyCombo->setCurrentIndex(newValue);

    if (newValue >= 0 && newValue < keyCombo->count())
        keyCombo->setCurrentIndex(newValue);

    m_song->transpose(value);
    m_song->forceScoreRedraw();
}

void GuiTopBar::setPlayButtonState(bool checked, bool atTheEnd)
{
    if (atTheEnd)
        m_atTheEndOfTheSong = true;

    playButton->setChecked(checked);
    if (checked)
    {
        playButton->setIcon(QIcon(":/images/stop.png"));
        playButton->setToolTip("");
        playFromStartButton->setToolTip("");
    }
    else
    {
        playButton->setIcon(QIcon(":/images/play.png"));
        playButton->setToolTip(tr("Start and stop playing music"));
        playFromStartButton->setToolTip(tr("Playing music from the beginning"));
    }
}


void GuiTopBar::on_playButton_clicked(bool clicked)
{
    if (!m_song) return;

    if (m_atTheEndOfTheSong)
        m_song->rewind();
    m_atTheEndOfTheSong = false;

    bool start = !m_song->playingMusic();
    m_song->playMusic(start);
    setPlayButtonState(start);
}

void GuiTopBar::on_playFromStartButton_clicked(bool clicked)
{
    if (!m_song) return;

    m_atTheEndOfTheSong = false;
    m_song->playFromStartBar();
    setPlayButtonState(true);
}

void GuiTopBar::on_speedSpin_valueChanged(int speed)
{
    if (!m_song) return;
    m_song->setSpeed(speed/100.0);
}

void GuiTopBar::on_startBarSpin_valueChanged(double bar)
{
    if (!m_song) return;

    // Stop the muisc playing
    m_song->playMusic(false);
    setPlayButtonState(false);

    m_song->setPlayFromBar( bar);
}



void GuiTopBar::on_saveBarButton_clicked(bool clicked)
{
    if (!m_song) return;
    double barNumber = m_song->getCurrentBarPos();
    startBarSpin->setValue(barNumber);
}


void GuiTopBar::on_loopingBarsPopupButton_clicked(bool clicked)
{
    if (!m_song) return;

    m_song->playMusic(false);
    setPlayButtonState(false);

    QPoint pos = mapToGlobal(loopingBarsPopupButton->pos()) ;

    pos.ry() += loopingBarsPopupButton->height() + 2;
    pos.rx() += -5; // Tweak the position slightly
    GuiLoopingPopup *loopingPopup = new GuiLoopingPopup(loopingBarsPopupButton);
    loopingPopup->init(m_song);
    loopingPopup->move (pos);
    loopingPopup->show();
}

bool GuiTopBar::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key()==Qt::Key_Up) {
            speedSpin->stepUp();
            return true;
        } else if (keyEvent->key()==Qt::Key_Down) {
            speedSpin->stepDown();
            return true;
        } else {
            return false;
        }
    } else {
        // it's not a key event, lets do standard event processing
        return QObject::eventFilter(obj, event);
    }
}
