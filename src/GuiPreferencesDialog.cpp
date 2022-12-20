/*!
    @file           GuiPreferencesDialog.cpp

    @brief          xxx.

    @author         L. J. Barman

    Copyright (c)   2008-2020, L. J. Barman and others, all rights reserved

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
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>

#include <QRegExp> // deprecated, from compat module in Qt 6

#include "GuiPreferencesDialog.h"
#include "GlView.h"

GuiPreferencesDialog::GuiPreferencesDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    m_song = nullptr;
    m_settings = nullptr;
    m_glView = nullptr;
    setWindowTitle(tr("Preferences"));
    followStopPointCombo->addItem(tr("Automatic (Recommended)"));
    followStopPointCombo->addItem(tr("On the Beat"));
    followStopPointCombo->addItem(tr("After the Beat"));
}

void GuiPreferencesDialog::initLanguageCombo(){
#ifndef NO_LANGS

    QString localeDirectory = QApplication::applicationDirPath() + "/translations/";

    QFile fileTestLocale(localeDirectory);
    if (!fileTestLocale.exists()){
        localeDirectory=Util::dataDir()+"/translations/";
 #ifdef Q_OS_DARWIN
        localeDirectory=QApplication::applicationDirPath() + "/../Resources/translations/";
 #endif
    }

    // read langs.json
    QJsonObject rootLangs;
    QFile file;
    file.setFileName(localeDirectory+"/langs.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        ppLogError("Error while opening langs.json");
        return;
    }else{
        QByteArray val = file.readAll();
        file.close();

        QJsonDocument document = QJsonDocument::fromJson(val);
        if (document.isEmpty()){
            ppLogError("langs.json is not valid");
            return;
        }else{
            rootLangs = document.object();
        }
    }

    // loading languages
    languageCombo->clear();
    languageCombo->addItem("<"+tr("System Language")+">","");
    languageCombo->addItem("English","en");
    if (m_settings->value("General/lang","").toString()=="en"){
        languageCombo->setCurrentIndex(languageCombo->count()-1);
    }

    QDir dirLang(localeDirectory);
    dirLang.setFilter(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);
    QFileInfoList listLang = dirLang.entryInfoList();
    for (QFileInfo fileInfo : listLang) {

        QRegExp rx("(pianobooster_)(.*)(.qm)");
        if (rx.indexIn(fileInfo.fileName())!=-1){
            QString lang_code = rx.cap(2);

            if (lang_code=="blank") continue;

            QString lang_code_loc = lang_code;
            if (lang_code_loc.indexOf("_")==-1) lang_code_loc+="_"+lang_code_loc.toUpper();

            QString languageName = "";

            if (rootLangs.value(lang_code).toObject().value("nativeName").isString()){
                languageName = rootLangs.value(lang_code).toObject().value("nativeName").toString();
            }

            if (languageName.isEmpty()){
                QLocale loc(lang_code_loc);
                languageName = loc.nativeLanguageName();

                if (languageName.isEmpty()){
                    languageName=QLocale::languageToString(loc.language());
                }

            }

            languageName[0]=languageName[0].toUpper();

            if (languageName.isEmpty() || languageName=="C"){
                    languageName=lang_code;
            }

            languageCombo->addItem(languageName,lang_code);
            if (m_settings->value("General/lang","").toString()==lang_code){
                languageCombo->setCurrentIndex(languageCombo->count()-1);
            }
        }
    }

#else
    // loading languages
    languageCombo->clear();
    languageCombo->addItem("English","en");
    languageCombo->setCurrentIndex(0);
#endif
}

void GuiPreferencesDialog::init(CSong* song, CSettings* settings, CGLView * glView)
{
    m_song = song;
    m_settings = settings;
    m_glView = glView;

    timingMarkersCheck->setChecked(m_song->cfg_timingMarkersFlag);
    showNoteNamesCheck->setChecked(m_settings->isNoteNamesEnabled());
    courtesyAccidentalsCheck->setChecked(m_settings->displayCourtesyAccidentals());
    showTutorPagesCheck->setChecked(m_settings->isTutorPagesEnabled());
    followThroughErrorsCheck->setChecked(m_settings->isFollowThroughErrorsEnabled());
    showColoredNotesCheck->setChecked(m_settings->isColoredNotesEnabled());

    followStopPointCombo->setCurrentIndex(m_song->cfg_stopPointMode);

    initLanguageCombo();
}

void GuiPreferencesDialog::accept()
{
    m_song->cfg_timingMarkersFlag = timingMarkersCheck->isChecked();
    m_settings->setValue("Score/TimingMarkers", m_song->cfg_timingMarkersFlag );
    m_settings->setNoteNamesEnabled( showNoteNamesCheck->isChecked());
    m_settings->setCourtesyAccidentals( courtesyAccidentalsCheck->isChecked());
    m_settings->setTutorPagesEnabled( showTutorPagesCheck->isChecked());
    m_settings->setFollowThroughErrorsEnabled( followThroughErrorsCheck->isChecked());
    m_settings->setColoredNotes( showColoredNotesCheck->isChecked());
    m_song->cfg_stopPointMode = static_cast<stopPointMode_t> (followStopPointCombo->currentIndex());
    m_settings->setValue("Score/StopPointMode", m_song->cfg_stopPointMode );

    m_settings->setValue("General/lang", languageCombo->currentData().toString());

    m_song->refreshScroll();

    this->QDialog::accept();
}
