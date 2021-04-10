/*!
    @file           GuiSongDetailsDialog.cpp

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

#include "GuiTopBar.h"
#include "GuiMetronomeDialog.h"
#include "GlView.h"


GuiMetronomeDialog::GuiMetronomeDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi(this);
    setWindowTitle(tr("Metronome Settings"));
}

void GuiMetronomeDialog::init(CSong* song, CSettings* settings, GuiTopBar* topBar)
{
    m_song = song;
    m_settings = settings;
    m_trackList = m_song->getTrackList();
    m_topBar = topBar;

    prevMetronomeActive = m_song->metronomeActive();
    prevMetronomeVolume = m_song->getMetronomeVolume();
    prevMetronomeBarSound = m_song->getMetronomeBarSound();
    prevMetronomeBeatSound = m_song->getMetronomeBeatSound();
    prevMetronomeBarVelocity = m_song->getMetronomeBarVelocity();
    prevMetronomeBeatVelocity = m_song->getMetronomeBeatVelocity();

    metronomeVolume->setMinimum(-100);
    metronomeVolume->setMaximum(100);

    barSpin->setMaximum(MAX_VELOCITY);
    beatSpin->setMaximum(MAX_VELOCITY);

    barCombo->addItem(tr("No note assigned"));
    barCombo->addItems(m_trackList->getAllPercussionProgramNames());

    beatCombo->addItem(tr("No note assigned"));
    beatCombo->addItems(m_trackList->getAllPercussionProgramNames());

    subBeatCombo->addItem(tr("No note assigned"));
    subBeatCombo->addItems(m_trackList->getAllPercussionProgramNames());

    if (prevMetronomeBarSound == -1) {
        barCombo->setCurrentIndex(0);
    }
    else {
        barCombo->setCurrentIndex(prevMetronomeBarSound - 33 + 1);
    }

    if (prevMetronomeBeatSound == -1) {
        beatCombo->setCurrentIndex(0);
    }
    else {
        beatCombo->setCurrentIndex(prevMetronomeBeatSound - 33 + 1);
    }

    if (prevMetronomeBarVelocity == -1) {
        barSpin->setValue(barSpin->maximum());
    }
    else {
        barSpin->setValue(prevMetronomeBarVelocity);
    }

    if (prevMetronomeBeatVelocity == -1) {
        beatSpin->setValue(beatSpin->maximum());
    }
    else {
        beatSpin->setValue(prevMetronomeBeatVelocity);
    }

    if (prevMetronomeActive) {
        metronomeCheck->setCheckState(Qt::Checked);
    }
    else {
        metronomeCheck->setCheckState(Qt::Unchecked);
    }

    if (m_song) {
        int sliderValue = getNormalizedValue(prevMetronomeVolume, MIN_MIDI_VOLUME, MAX_MIDI_VOLUME, -100, 100); //reverse normalization process

        metronomeVolume->setSliderPosition(sliderValue);
    }

}

void GuiMetronomeDialog::on_barCombo_activated(int index)
{

}

void GuiMetronomeDialog::on_beatCombo_activated(int index)
{

}

void GuiMetronomeDialog::on_subBeatCombo_activated(int index)
{

}

void GuiMetronomeDialog::on_barSpin_valueChanged(int i)
{

}

void GuiMetronomeDialog::on_beatSpin_valueChanged(int i)
{

}

void GuiMetronomeDialog::on_subBeatSpin_valueChanged(int i)
{

}

void GuiMetronomeDialog::on_metronomeCheck_stateChanged(int state)
{
    bool metronomeActive;

    if (state == 0) { //unchecked 
        metronomeActive = false;
    }
    if (state == 2) { //checked 
        metronomeActive = true;
    }

    if (metronomeActive != prevMetronomeActive) {
        m_topBar->metronomeClicked();
    }
    m_song->setMetronomeState(metronomeActive);
}

void GuiMetronomeDialog::on_previewSoundPush_clicked(bool clicked)
{
    if (barCombo->currentIndex() != 0) {
        m_song->setMetronomeBarSound(barCombo->currentIndex() + 33 - 1);
    }

    if (beatCombo->currentIndex() != 0) {
        m_song->setMetronomeBeatSound(beatCombo->currentIndex() + 33 - 1);
    }

    if (barSpin->value() != 0) {
        m_song->setMetronomeBarVelocity(barSpin->value());
    }

    if (beatSpin->value() != 0) {
        m_song->setMetronomeBeatVelocity(beatSpin->value());
    }
}

void GuiMetronomeDialog::on_metronomeVolume_valueChanged(int value)
{
    int normalizedVolume = getNormalizedValue(value, -100, 100, MIN_MIDI_VOLUME, MAX_MIDI_VOLUME);
    m_song->setMetronomeVolume(normalizedVolume);
}

void GuiMetronomeDialog::accept()
{
    if (barCombo->currentIndex() != 0) {
        m_song->setMetronomeBarSound(barCombo->currentIndex() + 33 - 1);
    }

    if (beatCombo->currentIndex() != 0) {
        m_song->setMetronomeBeatSound(beatCombo->currentIndex() + 33 - 1);
    }

    if (barSpin->value() != 0) {
        m_song->setMetronomeBarVelocity(barSpin->value());
    }

    if (beatSpin->value() != 0) {
        m_song->setMetronomeBeatVelocity(beatSpin->value());
    }

    this->QDialog::accept();
}

void GuiMetronomeDialog::reject()
{
    if (m_song->metronomeActive() != prevMetronomeActive) {
        m_topBar->metronomeClicked();
        m_song->setMetronomeState(prevMetronomeActive);
    }
    m_song->setMetronomeVolume(prevMetronomeVolume);
    m_song->setMetronomeBarSound(prevMetronomeBarSound);
    m_song->setMetronomeBeatSound(prevMetronomeBeatSound);
    m_song->setMetronomeBarVelocity(prevMetronomeBarVelocity);
    m_song->setMetronomeBeatVelocity(prevMetronomeBeatVelocity);

    this->QDialog::reject();
}
