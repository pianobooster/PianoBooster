/*!
    @file           GuiPreferencesDialog.cpp

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

#include "GuiPreferencesDialog.h"
#include "GlView.h"

GuiPreferencesDialog::GuiPreferencesDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    m_song = 0;
    m_settings = 0;
    m_glView = 0;
    setWindowTitle("Preferences");
}


void GuiPreferencesDialog::init(CSong* song, CSettings* settings, CGLView * glView)
{
    m_song = song;
    m_settings = settings;
    m_glView = glView;

    videoOptimiseCheck->setChecked(m_glView->m_cfg_openGlOptimise);
    timingMarkersCheck->setChecked(m_song->cfg_timingMarkersFlag);
    showNoteNamesCheck->setChecked(m_settings->isNoteNamesEnabled());
    courtesyAccidentalsCheck->setChecked(m_settings->displayCourtesyAccidentals());
    followStopPointCombo->addItem("Automatic (Recommended)");
    followStopPointCombo->addItem("On the Beat");
    followStopPointCombo->addItem("After the Beat");
    followStopPointCombo->setCurrentIndex(m_song->cfg_stopPointMode);
}

void GuiPreferencesDialog::accept()
{
    m_glView->m_cfg_openGlOptimise = videoOptimiseCheck->isChecked();
    m_settings->setValue("Display/OpenGlOptimise", m_glView->m_cfg_openGlOptimise );
    m_song->cfg_timingMarkersFlag = timingMarkersCheck->isChecked();
    m_settings->setValue("Score/TimingMarkers", m_song->cfg_timingMarkersFlag );
    m_settings->setNoteNamesEnabled( showNoteNamesCheck->isChecked());
    m_settings->setCourtesyAccidentals( courtesyAccidentalsCheck->isChecked());
    m_song->cfg_stopPointMode = static_cast<stopPointMode_t> (followStopPointCombo->currentIndex());
    m_settings->setValue("Score/StopPointMode", m_song->cfg_stopPointMode );
    m_song->refreshScroll();

    this->QDialog::accept();
}
