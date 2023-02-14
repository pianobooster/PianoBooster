/*********************************************************************************/
/*!
@file           GuiSidePanel.cpp

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

#include <QtWidgets>

#include "GuiSidePanel.h"
#include "GuiTopBar.h"
#include "TrackList.h"
#include "Conductor.h"
#include "Draw.h"

GuiSidePanel::GuiSidePanel(QWidget *parent, CSettings* settings)
    : QWidget(parent), m_parent(parent)
{
    m_song = nullptr;
    m_score = nullptr;
    m_trackList = nullptr;
    m_topBar = nullptr;
    m_settings = settings;
    setupUi(this);
}

void GuiSidePanel::init(CSong* songObj, CTrackList* trackList, GuiTopBar* topBar)
{
    m_song = songObj;
    m_trackList = trackList;
    m_topBar = topBar;
    m_trackList->init(songObj, m_settings);

    // set skill from config
    playMode_t skill = m_settings->value("SidePanel/skill",PB_PLAY_MODE_followYou).toInt();
    switch (skill) {
        case PB_PLAY_MODE_listen:
            listenRadio->setChecked(true);
            on_listenRadio_toggled(true);
            break;
        case PB_PLAY_MODE_rhythmTapping:
            rhythmTapRadio->setChecked(true);
            on_rhythmTapRadio_toggled(true);
            break;
        case PB_PLAY_MODE_followYou:
            followYouRadio->setChecked(true);
            on_followYouRadio_toggled(true);
            break;
        case PB_PLAY_MODE_playAlong:
            playAlongRadio->setChecked(true);
            on_playAlongRadio_toggled(true);
            break;
        default:
            break;
    }

    bothHandsRadio->setChecked(true);

    rhythmTappingCombo->addItem(tr("Drums"));
    rhythmTappingCombo->addItem(tr("Melody"));
    //FIXME rhythmTappingCombo->addItem(tr("Drums+M"));

    on_rhythmTappingCombo_activated(m_settings->value("SidePanel/rhythmTapping",0).toInt());
    rhythmTappingCombo->setCurrentIndex(m_song->cfg_rhythmTapping);
    repeatSong->setChecked(m_settings->value("SidePanel/repeatSong",false).toBool());
    connect(repeatSong,SIGNAL(stateChanged(int)),this,SLOT(on_repeatSong_released()));

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

    m_settings->setValue("SidePanel/clefRight",PB_SYMBOL_gClef);
    m_settings->setValue("SidePanel/clefLeft",PB_SYMBOL_fClef);
    clefRightCombo->addItem(tr("Treble"),PB_SYMBOL_gClef);
    clefRightCombo->addItem(tr("Bass"),PB_SYMBOL_fClef);
    clefLeftCombo->addItem(tr("Treble"),PB_SYMBOL_gClef);
    clefLeftCombo->addItem(tr("Bass"),PB_SYMBOL_fClef);
    clefRightCombo->setCurrentIndex(0);
    clefLeftCombo->setCurrentIndex(1);
    trackListWidget->addAction(act);
    connect(clefRightCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(on_clefRightComboChange(QString)));
    connect(clefLeftCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(on_clefLeftComboChange(QString)));
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
    Q_UNUSED(index)
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

void GuiSidePanel::on_repeatSong_released(){
    m_settings->setValue("SidePanel/repeatSong",repeatSong->isChecked());
}

void GuiSidePanel::autoSetMuteYourPart()
{
    bool checked = true;
    if (m_song->getPlayMode() == PB_PLAY_MODE_rhythmTapping)
    {
        if (m_song->cfg_rhythmTapping == PB_RHYTHM_TAP_drumsOnly)
            checked = false;
    }

    muteYourPartCheck->setChecked(checked);
    m_song->mutePianistPart(checked);
}

void GuiSidePanel::setSongName(const QString &songName)
{
    for (int i = 0; i < songCombo->count(); ++i)
    {
        if (songCombo->itemText(i) == songName)
            songCombo->setCurrentIndex(i);
    }
}

void GuiSidePanel::setBookName(const QString &bookName)
{
    for (int i = 0; i < bookCombo->count(); ++i)
    {
        if (bookCombo->itemText(i) == bookName)
            bookCombo->setCurrentIndex(i);
    }
}

// pass either 'left' 'right' or 'both'
void GuiSidePanel::setCurrentHand(const QString &hand)
{
    if (hand == "left")
        leftHandRadio->setChecked(true);
    else if (hand == "right")
        rightHandRadio->setChecked(true);
    else
        bothHandsRadio->setChecked(true);
}

void GuiSidePanel::updateTranslate(){
    // save original
    if (listWidgetsRetranslateUi.size()==0){
        QList<QWidget*> l2 = this->findChildren<QWidget *>();
        for (auto &w:l2){
            QMap<QString,QString> m;
            m["toolTip"]=w->toolTip();
            m["whatsThis"]=w->whatsThis();
            m["windowTitle"]=w->windowTitle();
            m["statusTip"]=w->statusTip();
            listWidgetsRetranslateUi[w]=m;
        }

        QList<QAction*> l = this->findChildren<QAction *>();
        for (auto &w:l){
            QMap<QString,QString> m;
            m["toolTip"]=w->toolTip();
            m["whatsThis"]=w->whatsThis();
            m["statusTip"]=w->statusTip();
            m["text"]=w->text();
            listActionsRetranslateUi[w]=m;
        }

    }

    // retranslate UI
    QList<QWidget*> l2 = this->findChildren<QWidget *>();
    for (auto &w:l2){
        if (!w->toolTip().isEmpty()) w->setToolTip(tr(listWidgetsRetranslateUi[w]["toolTip"].toStdString().c_str()));
        if (!w->whatsThis().isEmpty()) w->setWhatsThis(tr(listWidgetsRetranslateUi[w]["whatsThis"].toStdString().c_str()));
        if (!w->windowTitle().isEmpty()) w->setWindowTitle(tr(listWidgetsRetranslateUi[w]["windowTitle"].toStdString().c_str()));
        if (!w->statusTip().isEmpty()) w->setStatusTip(tr(listWidgetsRetranslateUi[w]["statusTip"].toStdString().c_str()));
    }

    QList<QAction*> l = this->findChildren<QAction *>();
    for (auto &w:l){
        if (!w->toolTip().isEmpty()) w->setToolTip(tr(listActionsRetranslateUi[w]["toolTip"].toStdString().c_str()));
        if (!w->whatsThis().isEmpty()) w->setWhatsThis(tr(listActionsRetranslateUi[w]["whatsThis"].toStdString().c_str()));
        if (!w->statusTip().isEmpty()) w->setStatusTip(tr(listActionsRetranslateUi[w]["statusTip"].toStdString().c_str()));
        if (!w->text().isEmpty()) w->setText(tr(listActionsRetranslateUi[w]["text"].toStdString().c_str()));
    }

    rhythmTappingCombo->setItemText(0,tr("Drums"));
    rhythmTappingCombo->setItemText(1,tr("Melody"));
    clefRightCombo->setItemText(0,tr("Treble"));
    clefRightCombo->setItemText(1,tr("Bass"));
    clefLeftCombo->setItemText(0,tr("Treble"));
    clefLeftCombo->setItemText(1,tr("Bass"));

    retranslateUi(this);

    // ---  smart resize panel  --- //
    int maxDeltaWidth=0;
    this->setMaximumWidth(300); // default
    QVector<QWidget*> listCheckWidget {label2,listenRadio,rhythmTapRadio,followYouRadio,playAlongRadio,rightHandRadio,bothHandsRadio,leftHandRadio,clefRightCombo,clefLeftCombo};

    for (QWidget* w:listCheckWidget){
        int delta = 0;
        QFontMetrics fm(w->font());

#if (QT_VERSION < QT_VERSION_CHECK(5, 11, 0)) // keep compat with Qt < 5.11
#define horizontalAdvance width
#endif
        auto *const lb = qobject_cast<QLabel*>(w);
        if (lb) delta=fm.horizontalAdvance(lb->text())-lb->width();

        auto *const rb = qobject_cast<QRadioButton*>(w);
        if (rb) delta=fm.horizontalAdvance(rb->text())-(rb->width()-32);

        if (delta>maxDeltaWidth) maxDeltaWidth=delta;
    }
    this->setMaximumWidth(300+maxDeltaWidth);
}

void GuiSidePanel::on_rhythmTappingCombo_activated (int index)
{
    m_settings->setValue("SidePanel/rhythmTapping",index);

    switch (index)
    {
        case 1 :
            m_song->cfg_rhythmTapping = PB_RHYTHM_TAP_mellodyOnly;
            break;
        case 2 :
            m_song->cfg_rhythmTapping = PB_RHYTHM_TAP_drumsAndMellody;
           break;
        default:
            m_song->cfg_rhythmTapping = PB_RHYTHM_TAP_drumsOnly;
    }
    autoSetMuteYourPart();
}

void GuiSidePanel::on_clefComboChange (const QString &name, int value)
{
    m_settings->setValue(name,value);
    CDraw::forceCompileRedraw();
    m_song->refreshScroll();
}
