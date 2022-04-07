/*********************************************************************************/
/*!
@file           Themes.cpp

@brief          Encapsulate color themes

@author         Vy Ho

    Copyright (c)   2022, Vy Ho, all rights reserved

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

#include "Themes.h"

#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QSettings>

#include <iostream>

CThemeList::CThemeList() {
    themeList = new QList<CTheme*>();
    auto configFile = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configFile.isEmpty()) qFatal("Cannot determine settings storage location");

    QFileInfo fileInfo(configFile);


    QDir dir = fileInfo.dir();

    if ( !dir.exists() ) {
        qFatal("Cannot determine settings storage location");
    }

    QStringList fileList = dir.entryList(QDir::Files);

    for (int i = 0; i< fileList.count(); i++) {
        if (fileList[i] == "Piano Booster.ini") {
            continue;
        }

        if ( fileList[i].endsWith(".ini")) {
            QString filePath = dir.absolutePath() + "/" + fileList[i];
            QSettings * settings = new QSettings(filePath, QSettings::IniFormat);
            auto themName = settings->value("themeName", "").toString();
            if ( !themName.isEmpty() ) {
                //std::cout << "THem: " << fileList[i].toStdString() << std::endl;
                //themeComboBox->addItem(theme.value("themeName", "").toString());

                CTheme * atheme = new CTheme(settings, themName);
                themeList->append(atheme);
            } else {
                delete settings;
                settings = NULL;
            }
        }
    }
}

CThemeList::~CThemeList(){
    while (!themeList->isEmpty()) {
        delete themeList->takeFirst();
    }
    delete themeList;
    themeList = nullptr;
}

CTheme::CTheme(QString fileName, QString themeName)
{
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "PianoBooster", fileName);
    this->themeName = themeName;
}

CTheme::CTheme(QSettings * settings, QString themeName)
{
    this->settings = settings;
    this->themeName = themeName;
}

CTheme::~CTheme()
{
    delete settings;
    settings = NULL;
}

QColor CTheme::backgroundColor()      {return getColor("bg");}
QColor CTheme::barMarkerColor()       {return getColor("barMarker");}
QColor CTheme::beatMarkerColor()      {return getColor("beatMarker");}
QColor CTheme::noteColor()            {return getColor("note");}
QColor CTheme::noteDimColor()         {return getColor("noteDim");}
QColor CTheme::noteNameColor()        {return getColor("noteName");}
QColor CTheme::pianoBadColor()        {return getColor("pianoBad");}
QColor CTheme::pianoGoodColor()       {return getColor("pianoGood");}
QColor CTheme::playedBadColor()       {return getColor("playBad");}
QColor CTheme::playedGoodColor()      {return getColor("playGood");}
QColor CTheme::playedStoppedColor()   {return getColor("playStopped");}
QColor CTheme::playZoneAreaColor()    {return getColor("playZoneBg");}
QColor CTheme::playZoneEndColor()     {return getColor("playZoneEndLine");}
QColor CTheme::playZoneMiddleColor()  {return getColor("playZoneMiddle");}
QColor CTheme::staveColor()           {return getColor("stave");}
QColor CTheme::staveDimColor()        {return getColor("staveDim");}

QString CTheme::getBgImageFile() { return settings->value("BackgroundImage/fileName", "").toString();}
int CTheme::getBgTileNoX() { return settings->value("BackgroundImage/tileNoX", "1").toInt();}
int CTheme::getBgTileNoY() { return settings->value("BackgroundImage/tileNoY", "1").toInt();}
QString CTheme::getBgAlignX() { return settings->value("BackgroundImage/alignX", "left").toString();}
QString CTheme::getBgAlignY() { return settings->value("BackgroundImage/alignY", "top").toString();}
