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

#include <iostream>

#include "GuiPreferencesDialog.h"
#include "GlView.h"

#include "Draw.h"

#include "Themes.h"

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
    QTimer::singleShot(200, this, &QDialog::close);
    //this->QDialog::accept();  //Call directly causes some of the apply operation not working
}

void GuiPreferencesDialog::saveDisplayColors(){
    //get the
    CThemeList themeList;

    QString name = themeName->text();

    CTheme * theme = themeList.getTheme(name);

    theme->setColor("noteDim", noteDimColor);
    theme->setColor("barMarker", barMarkerColor);
    theme->setColor("noteName", noteNameColor);
    theme->setColor("pianoBad", pianoBadColor);
    theme->setColor("staveDim", staveDimColor);
    theme->setColor("bg", backgroundColor);
    theme->setColor("note", noteColor);
    theme->setColor("stave", staveColor);
    theme->setColor("playStopped", playedStoppedColor);
    theme->setColor("beatMarker", beatMarkerColor);
    theme->setColor("playZoneBg", playZoneAreaColor);
    theme->setColor("playGood", playedGoodColor);
    theme->setColor("playBad", playedBadColor);
    theme->setColor("playZoneEndLine", playZoneEndColor);
    theme->setColor("playZoneMiddle", playZoneMiddleColor);

    m_settings->clearCache();

}

void GuiPreferencesDialog::translateColor(const CColor & ccolor, QColor & qcolor) {
    qcolor.setRgb((int) (ccolor.red * 255),(int)  (ccolor.green *255), (int) (ccolor.blue*255));
}

void GuiPreferencesDialog::initDisplayColors(){
    updateColorSelBtnBg();
}

void GuiPreferencesDialog::updateColorSelBtnBg() {
    CThemeList themeList;

    QString name = m_settings->value("themeName", "Default Theme").toString();
    themeName->setText(name);

    CTheme * theme = themeList.getTheme(name);

    staveColor = theme->staveColor();
    staveDimColor = theme->staveDimColor();
    noteColor = theme->noteColor();
    noteDimColor = theme->noteDimColor();
    playedGoodColor = theme->playedGoodColor();
    playedBadColor = theme->playedBadColor();
    playedStoppedColor = theme->playedStoppedColor();
    backgroundColor = theme->backgroundColor();
    barMarkerColor = theme->barMarkerColor();
    beatMarkerColor = theme->beatMarkerColor();
    pianoGoodColor = theme->pianoGoodColor();
    pianoBadColor = theme->pianoBadColor();
    noteNameColor = theme->noteNameColor();
    playZoneAreaColor = theme->playZoneAreaColor();
    playZoneEndColor = theme->playZoneEndColor();
    playZoneMiddleColor = theme->playZoneMiddleColor();

    setButtonBgColor(barMarkerColorSel, theme->barMarkerColor() );
    setButtonBgColor(beatMarkerColorSel, theme->beatMarkerColor() );
    setButtonBgColor(bgColorSel, theme->backgroundColor() );
    setButtonBgColor(noteColorSel, theme->noteColor() );
    setButtonBgColor(noteDimColorSel, theme->noteDimColor() );
    setButtonBgColor(noteNameColorSel, theme->noteNameColor() );
    setButtonBgColor(pianoBadColorSel, theme->pianoBadColor() );
    setButtonBgColor(playBadColorSel, theme->playedBadColor() );
    setButtonBgColor(playStoppedColorSel, theme->playedStoppedColor() );
    setButtonBgColor(playZoneBgColorSel, theme->playZoneAreaColor() );
    setButtonBgColor(playGoodColorSel, theme->playedGoodColor() );
    setButtonBgColor(playZoneEndLineColorSel, theme->playZoneEndColor());
    setButtonBgColor(playZoneMiddleColorSel, theme->playZoneMiddleColor() );
    setButtonBgColor(staveDimColorSel, theme->staveDimColor() );
    setButtonBgColor(staveColorSel, theme->staveColor() );

}

void GuiPreferencesDialog::setButtonBgColor(QPushButton * btn, CColor color) {
    QColor qcolor((int) (color.red * 255),(int)  (color.green *255), (int) (color.blue*255));
    setButtonBgColor(btn, qcolor);
}

void GuiPreferencesDialog::setButtonBgColor(QPushButton * btn, QColor qcolor) {
    btn->setStyleSheet(" background-repeat:no-repeat; background-position: center center; background-image: url(:/images/color-picker-icon.png);");

    QPalette pal = btn->palette();
    pal.setColor(QPalette::Button, qcolor);
    btn->setAutoFillBackground(true);
    btn->setPalette(pal);
    btn->update();
}

void GuiPreferencesDialog::showColorSelector(QPushButton * btn, QColor & qcolor) {
    QColor selColor = QColorDialog::getColor(qcolor, this, "", QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
    if( selColor.isValid() ) {
        qcolor.setRed(selColor.red());
        qcolor.setGreen(selColor.green());
        qcolor.setBlue(selColor.blue());
        qcolor.setAlpha(selColor.alpha());
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

    CDraw::forceCompileRedraw();
    CSettings::clearCache();
    QTimer::singleShot(150, this, &GuiPreferencesDialog::forceGlViewUpdate);
}

void GuiPreferencesDialog::forceGlViewUpdate() {
    //m_glView->updateBackground(true);
    m_glView->updateBackground(true);
    m_song->forceScoreRedraw();
    m_song->rewind();
    updateColorSelBtnBg();
}

void GuiPreferencesDialog::on_resetBtn_clicked()
{
    CThemeList themeList;

    CTheme * theme = themeList.getTheme(themeName->text());

    noteDimColor = theme->getDefaultColor("noteDim");
    barMarkerColor = theme->getDefaultColor("barMarker");
    noteNameColor = theme->getDefaultColor("noteName");
    pianoBadColor = theme->getDefaultColor("pianoBad");
    staveDimColor = theme->getDefaultColor("staveDim");
    backgroundColor = theme->getDefaultColor("bg");
    noteColor = theme->getDefaultColor("note");
    staveColor = theme->getDefaultColor("stave");
    playedStoppedColor = theme->getDefaultColor("playStopped");
    beatMarkerColor = theme->getDefaultColor("beatMarker");
    playZoneAreaColor = theme->getDefaultColor("playZoneBg");
    playedGoodColor = theme->getDefaultColor("playGood");
    playedBadColor = theme->getDefaultColor("playBad");
    playZoneEndColor = theme->getDefaultColor("playZoneEndLine");
    playZoneMiddleColor = theme->getDefaultColor("playZoneMiddle");

    setButtonBgColor(barMarkerColorSel, barMarkerColor );
    setButtonBgColor(beatMarkerColorSel, beatMarkerColor );
    setButtonBgColor(bgColorSel, backgroundColor );
    setButtonBgColor(noteColorSel, noteColor);
    setButtonBgColor(noteDimColorSel, noteDimColor );
    setButtonBgColor(noteNameColorSel, noteNameColor );
    setButtonBgColor(pianoBadColorSel, pianoBadColor );
    setButtonBgColor(playBadColorSel, playedBadColor );
    setButtonBgColor(playStoppedColorSel, playedStoppedColor );
    setButtonBgColor(playZoneBgColorSel, playZoneAreaColor );
    setButtonBgColor(playGoodColorSel, playedGoodColor );
    setButtonBgColor(playZoneEndLineColorSel, playZoneEndColor);
    setButtonBgColor(playZoneMiddleColorSel, playZoneMiddleColor );
    setButtonBgColor(staveDimColorSel, staveDimColor );
    setButtonBgColor(staveColorSel, staveColor );
}
