/*********************************************************************************/
/*!
@file           GuiSidePanel.cpp

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

#include "GuiSidePanel.h"
#include "GuiTopBar.h"
#include "TrackList.h"


GuiSidePanel::GuiSidePanel(QWidget *parent, CSettings* settings)
    : QWidget(parent), m_parent(parent)
{
    m_song = 0;
    m_score = 0;
    m_trackList = 0;
    m_topBar = 0;
    m_settings = settings;
    setupUi(this);
}

void GuiSidePanel::init(CSong* songObj, CTrackList* trackList, GuiTopBar* topBar)
{
    m_song = songObj;
    m_trackList = trackList;
    m_topBar = topBar;
    m_trackList->init(songObj, m_settings);

    followYouRadio->setChecked(true);
    bothHandsRadio->setChecked(true);

    boostSlider->setMinimum(-100);
    boostSlider->setMaximum(100);
    pianoSlider->setMinimum(-100);
    pianoSlider->setMaximum(100);

    QAction* act;
    act = new QAction(tr("Set as Right Hand Part"), this);
    trackListWidget->addAction(act);
    connect(act, SIGNAL(triggered()), this, SLOT(setTrackRightHandPart()));

    act = new QAction(tr("Set as Left Hand Part"), this);
    trackListWidget->addAction(act);
    connect(act, SIGNAL(triggered()), this, SLOT(setTrackLeftHandPart()));

    act = new QAction(tr("Reset Both Parts"), this);
    trackListWidget->addAction(act);
    connect(act, SIGNAL(triggered()), this, SLOT(clearTrackPart()));

    trackListWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void GuiSidePanel::refresh() {
    if (m_trackList)
    {
        m_trackList->refresh();

        trackListWidget->clear();

        trackListWidget->addItems(m_trackList->getAllChannelProgramNames());


        trackListWidget->setCurrentRow(m_trackList->getActiveItemIndex());

        for (int i = 0; i < trackListWidget->count(); i++)
            m_trackList->changeListWidgetItemView(i, trackListWidget->item(i));

    }
    autoSetMuteYourPart();
}

void GuiSidePanel::loadBookList()
{
    QStringList bookNames = m_settings->getBookList();

    bookCombo->clear();

    for (int i = 0; i < bookNames.size(); i++)
    {
        bookCombo->addItem( bookNames.at(i));
        if (bookNames.at(i) == m_settings->getCurrentBookName())
            bookCombo->setCurrentIndex(i);
    }
    on_bookCombo_activated(-1);
}

void GuiSidePanel::on_bookCombo_activated (int index)
{
    QString currentSong;

    m_settings->setCurrentBookName(bookCombo->currentText(), (index >= 0)? true : false);

    currentSong = m_settings->getCurrentSongName();

    songCombo->clear();
    QStringList songNames = m_settings->getSongList();

    for (int i = 0; i < songNames.size(); ++i)
    {
        songCombo->addItem( songNames.at(i));
        if (songNames.at(i) == currentSong)
            songCombo->setCurrentIndex(i);
    }
    on_songCombo_activated(0); // Now load the selected song
}

void GuiSidePanel::on_songCombo_activated(int index)
{
    m_settings->setCurrentSongName(songCombo->currentText());
}

void GuiSidePanel::on_rightHandRadio_toggled (bool checked)
{
    if (checked)
        m_settings->setActiveHand(PB_PART_right);
}

void GuiSidePanel::on_bothHandsRadio_toggled (bool checked)
{
    if (checked)
        m_settings->setActiveHand(PB_PART_both);
}

void GuiSidePanel::on_leftHandRadio_toggled (bool checked)
{
    if (checked)
        m_settings->setActiveHand(PB_PART_left);
}

void GuiSidePanel::autoSetMuteYourPart()
{
    bool checked = false;
    if ( CNote::hasPianoPart(m_song->getActiveChannel()))
        checked =  true;
    muteYourPartCheck->setChecked(checked);
    m_song->mutePianistPart(checked);
}

void GuiSidePanel::setSongName(QString songName)
{
    for (int i = 0; i < songCombo->count(); ++i)
    {
        if (songCombo->itemText(i) == songName)
            songCombo->setCurrentIndex(i);
    }
}

void GuiSidePanel::setBookName(QString bookName)
{
    for (int i = 0; i < bookCombo->count(); ++i)
    {
        if (bookCombo->itemText(i) == bookName)
            bookCombo->setCurrentIndex(i);
    }
}

// pass either 'left' 'right' or 'both'
void GuiSidePanel::setCurrentHand(QString hand)
{
    if (hand == "left")
        leftHandRadio->setChecked(true);
    else if (hand == "right")
        rightHandRadio->setChecked(true);
    else
        bothHandsRadio->setChecked(true);
    //on_bothHandsRadio_toggled
}

