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
#include <QColorDialog>
#include <QPalette>

#include <QTimer>

#include "GuiPreferencesDialog.h"
#include "GlView.h"

#include "Draw.h"

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

    initDisplayColors();

    initLanguageCombo();
}

void GuiPreferencesDialog::accept()
{
    
    on_applyButton_clicked();
    this->QDialog::accept();
}

void GuiPreferencesDialog::saveDisplayColors(){
    m_settings->setColor("noteDim", noteDimColor);
    m_settings->setColor("barMarker", barMarkerColor);
    m_settings->setColor("noteName", noteNameColor);
    m_settings->setColor("pianoBad", pianoBadColor);
    m_settings->setColor("staveDim", staveDimColor);
    m_settings->setColor("bg", backgroundColor);
    m_settings->setColor("note", noteColor);
    m_settings->setColor("stave", staveColor);
    m_settings->setColor("playStopped", playedStoppedColor);
    m_settings->setColor("beatMarker", beatMarkerColor);
    m_settings->setColor("playZoneBg", playZoneAreaColor);
    m_settings->setColor("playGood", playedGoodColor);
    m_settings->setColor("playBad", playedBadColor);
    m_settings->setColor("playZoneEndLine", playZoneEndColor);
    m_settings->setColor("playZoneMiddle", playZoneMiddleColor);

}

void GuiPreferencesDialog::translateColor(const CColor & ccolor, QColor & qcolor) {
    qcolor.setRgb((int) (ccolor.red * 255),(int)  (ccolor.green *255), (int) (ccolor.blue*255));
}

void GuiPreferencesDialog::initDisplayColors(){
    translateColor(m_settings->staveColor(), staveColor);
    translateColor(m_settings->staveDimColor(), staveDimColor);
    translateColor(m_settings->noteColor(), noteColor);
    translateColor(m_settings->noteDimColor(), noteDimColor);
    translateColor(m_settings->playedGoodColor(), playedGoodColor);
    translateColor(m_settings->playedBadColor(), playedBadColor);
    translateColor(m_settings->playedStoppedColor(), playedStoppedColor);
    translateColor(m_settings->backgroundColor(), backgroundColor);
    translateColor(m_settings->barMarkerColor(), barMarkerColor);
    translateColor(m_settings->beatMarkerColor(), beatMarkerColor);
    translateColor(m_settings->pianoGoodColor(), pianoGoodColor);
    translateColor(m_settings->pianoBadColor(), pianoBadColor);
    translateColor(m_settings->noteNameColor(), noteNameColor);
    translateColor(m_settings->playZoneAreaColor(), playZoneAreaColor);
    translateColor(m_settings->playZoneEndColor(), playZoneEndColor);
    translateColor(m_settings->playZoneMiddleColor(), playZoneMiddleColor);

    updateColorSelBtnBg();
}

void GuiPreferencesDialog::updateColorSelBtnBg() {
    setButtonBgColor(noteDimColorSel, noteDimColor );
    setButtonBgColor(barMarkerColorSel, barMarkerColor );
    setButtonBgColor(noteNameColorSel, noteNameColor );
    setButtonBgColor(pianoBadColorSel, pianoBadColor );
    setButtonBgColor(staveDimColorSel, staveDimColor );
    setButtonBgColor(bgColorSel, backgroundColor );
    setButtonBgColor(noteColorSel, noteColor );
    setButtonBgColor(staveColorSel, staveColor );
    setButtonBgColor(playStoppedColorSel, playedStoppedColor );
    setButtonBgColor(beatMarkerColorSel, beatMarkerColor );
    setButtonBgColor(playZoneBgColorSel, playZoneAreaColor );
    setButtonBgColor(playGoodColorSel, playedGoodColor );
    setButtonBgColor(playBadColorSel, playedBadColor );
    setButtonBgColor(playZoneEndLineColorSel, playZoneEndColor);
    setButtonBgColor(playZoneMiddleColorSel, playZoneMiddleColor );
}

void GuiPreferencesDialog::setButtonBgColor(QPushButton * btn, CColor color) {
    QColor qcolor((int) (color.red * 255),(int)  (color.green *255), (int) (color.blue*255));
    setButtonBgColor(btn, qcolor);
}

void GuiPreferencesDialog::setButtonBgColor(QPushButton * btn, QColor qcolor) {
    QPalette pal = btn->palette();
    pal.setColor(QPalette::Button, qcolor);
    btn->setAutoFillBackground(true);
    btn->setPalette(pal);
    btn->update();
}

void GuiPreferencesDialog::showColorSelector(QPushButton * btn, QColor & qcolor) {
    QColor selColor = QColorDialog::getColor(qcolor, this );
    if( selColor.isValid() )
    {
        qcolor.setRed(selColor.red());
        qcolor.setGreen(selColor.green());
        qcolor.setBlue(selColor.blue());
        setButtonBgColor(btn, qcolor);
    }
}

void GuiPreferencesDialog::on_staveColorSel_clicked()
{
   showColorSelector(staveColorSel, staveColor);
}

void GuiPreferencesDialog::on_staveDimColorSel_clicked()
{
    showColorSelector(staveDimColorSel, staveDimColor);
}

void GuiPreferencesDialog::on_noteColorSel_clicked()
{
    showColorSelector(noteColorSel,
        noteColor);
}

void GuiPreferencesDialog::on_noteDimColorSel_clicked()
{
    showColorSelector(noteDimColorSel,
        noteDimColor);
}

void GuiPreferencesDialog::on_playGoodColorSel_clicked()
{
    showColorSelector(playGoodColorSel,
        playedGoodColor);
}

void GuiPreferencesDialog::on_playBadColorSel_clicked()
{
    showColorSelector(playBadColorSel,
        playedBadColor);
}

void GuiPreferencesDialog::on_playStoppedColorSel_clicked()
{
    showColorSelector(playStoppedColorSel,
        playedStoppedColor);
}

void GuiPreferencesDialog::on_bgColorSel_clicked()
{
    showColorSelector(bgColorSel, backgroundColor);
}

void GuiPreferencesDialog::on_noteNameColorSel_clicked()
{
    showColorSelector(noteNameColorSel,
        noteNameColor);
}

void GuiPreferencesDialog::on_barMarkerColorSel_clicked()
{
    showColorSelector(barMarkerColorSel, barMarkerColor);
}

void GuiPreferencesDialog::on_beatMarkerColorSel_clicked()
{
    showColorSelector(beatMarkerColorSel,
        beatMarkerColor);
}

void GuiPreferencesDialog::on_pianoBadColorSel_clicked()
{
    showColorSelector(pianoBadColorSel,
        pianoBadColor);
}

void GuiPreferencesDialog::on_playZoneBgColorSel_clicked()
{
    showColorSelector(playZoneBgColorSel,
        playZoneAreaColor);
}

void GuiPreferencesDialog::on_playZoneMiddleColorSel_clicked()
{
    showColorSelector(playZoneMiddleColorSel,
        playZoneMiddleColor);        
}

void GuiPreferencesDialog::on_playZoneEndLineColorSel_clicked()
{
    showColorSelector(playZoneEndLineColorSel,
        playZoneEndColor);
}

void GuiPreferencesDialog::on_applyButton_clicked()
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

    saveDisplayColors();
    
    /*
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GuiPreferencesDialog::forceGlViewUpdate);
    timer->start(200);
    */

    QTimer::singleShot(200, this, &GuiPreferencesDialog::forceGlViewUpdate);

    CDraw::forceCompileRedraw();
    
    CSettings::clearCache();
}

void GuiPreferencesDialog::forceGlViewUpdate() {
    //m_glView->updateBackground(true);
    m_glView->updateBackground(true);
    updateColorSelBtnBg();
}