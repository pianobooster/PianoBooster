/*********************************************************************************/
/*!
@file           Themes.h

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
#ifndef THEMES_H
#define THEMES_H

#include <QColor>
#include <QList>
#include <QSettings>

#include <iostream>

class CTheme
{
private:
    QSettings * settings;
    QString themeName;

public:
    CTheme(QString fileName, QString themeName);
    CTheme(QSettings * settings, QString themeName);
    CTheme(const CTheme &theme) = delete;
    CTheme& operator=(const CTheme & other) = delete;
    ~CTheme();

    QString getThemeName() {return themeName;}

    QColor backgroundColor();
    QColor barMarkerColor();
    QColor beatMarkerColor();
    QColor noteColor();
    QColor noteDimColor();
    QColor noteNameColor();
    QColor pianoBadColor();
    QColor pianoGoodColor() ;
    QColor playedGoodColor();
    QColor playedBadColor();
    QColor playedStoppedColor();
    QColor playZoneAreaColor();
    QColor playZoneEndColor();
    QColor playZoneMiddleColor();
    QColor staveColor();
    QColor staveDimColor();

    QString getBgImageFile();
    int getBgTileNoX();
    int getBgTileNoY();
    QString getBgAlignX();
    QString getBgAlignY();

    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const {
        return settings->value(key, defaultValue);
    }

    QColor getColor(QString name) {
        QString colorName = "ScoreColors/";
        colorName.append(name);
        QString redName = colorName + "Red";
        QString greenName = colorName + "Green";
        QString blueName = colorName + "Blue";
        QString alphaName = colorName + "Alpha";

        QString customColorName = "CustomScoreColors/";
        customColorName.append(name);

        int red = settings->value(customColorName + "Red", settings->value(redName, "0")).toInt();
        int green = settings->value(customColorName + "Green", settings->value(greenName, "0")).toInt();
        int blue = settings->value(customColorName + "Blue", settings->value(blueName, "0")).toInt();
        int alpha = settings->value(customColorName + "Alpha", settings->value(alphaName, "255")).toInt();

        QColor color(red, green, blue, alpha);

        return color;
    }

    void setColor(QString name, QColor & color) {
        settings->setValue("CustomScoreColors/" + name + "Red", color.red());
        settings->setValue("CustomScoreColors/" + name + "Green", color.green());
        settings->setValue("CustomScoreColors/" + name + "Blue", color.blue());
        settings->setValue("CustomScoreColors/" + name + "Alpha", color.alpha());
    }

    QColor getDefaultColor(QString name) {
        QString colorName = "ScoreColors/";
        colorName.append(name);
        QString redName = colorName + "Red";
        QString greenName = colorName + "Green";
        QString blueName = colorName + "Blue";

        int red =settings->value(redName, "0").toInt();
        int green = settings->value(greenName, "0").toInt();
        int blue = settings->value(blueName, "0").toInt();

        QColor color(red, green, blue);

        return color;
    }
};


class CThemeList {
private:
    QList<CTheme*> *themeList;
public:
    CThemeList();
    ~CThemeList();

    QList<QString> getThemeNames() {
        QList<QString> names;
        foreach(CTheme * theme, *themeList) {
            names.append(theme->getThemeName());
        }
        //sort?
        return names;
    }

    CTheme * getTheme(QString name) {
        foreach(CTheme * theme, *themeList) {
            if (name == theme->getThemeName()){
                return theme;
            }
        }
        return NULL;
    }
};


#endif // THEMES_H
