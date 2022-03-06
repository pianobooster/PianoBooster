/*!
    @file           Settings.h

    @brief          Save all the settings for the programme in the right place.

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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <unordered_map>

#include <QSettings>
#include <QDomDocument>

#include "IColorPreference.h"

#include "Song.h"
#include "Notation.h"

#define QSTR_APPNAME "pianobooster"

class GuiSidePanel;
class GuiTopBar;
class QtWindow;

/// Save all the settings for the programme in the right place.
class CSettings : public QSettings, public IColorPreference
{

public:
    CSettings(QtWindow *mainWindow);

    static std::unordered_map<std::string, CColor *> colorCache;

    void init(CSong* song, GuiSidePanel* sidePanel, GuiTopBar* topBar);
    void setDefaultColors();
    /// returns true if the user wants to see the note names
    bool isNoteNamesEnabled() { return m_noteNamesEnabled; }
    bool displayCourtesyAccidentals() { return CNotation::displayCourtesyAccidentals(); }

    bool isTutorPagesEnabled() { return m_tutorPagesEnabled; }
    bool isFollowThroughErrorsEnabled() { return m_followThroughErrorsEnabled; }
    bool isColoredNotesEnabled() { return m_coloredNotes; }

    /// Saves in the .ini file whether the user wants to show the note names
    void setNoteNamesEnabled(bool value);
    void setColoredNotes(bool value);
    void setTutorPagesEnabled(bool value);
    void setFollowThroughErrorsEnabled(bool value);

    void setCourtesyAccidentals(bool value);
    void setAdvancedMode(bool value) { m_advancedMode = value;}

    /// returns true if the user wants to see the note names
    bool showNoteNames(){
        return m_noteNamesEnabled;
    }

    /// returns true if the user wants to see color-coded notes
    bool coloredNotes(){
        return m_coloredNotes;
    }

    void coloredNotes(bool b){
        m_coloredNotes = b;
    }

    /// returns true if the user wants Follow Skill to ignore errors
    bool followThroughErrors(){
        return m_followThroughErrorsEnabled;
    }

    void updateTutorPage();
    void openSongFile(const QString & filename);
    QString getCurrentSongName() { return m_currentSongName; }
    void setCurrentSongName(const QString & name);

    QString getCurrentBookName() { return m_currentBookName; }
    void setCurrentBookName(const QString & name, bool clearSongName);
    QStringList getBookList();
    QStringList getSongList();
    void writeSettings();
    void loadSettings();
    void unzipBoosterMusicBooks();
    QString getCurrentSongLongFileName()
    {
        if (getCurrentSongName().isEmpty())
            return QString();
        return m_bookPath + getCurrentBookName() + '/' + getCurrentSongName();
    }
    QStringList getFluidSoundFontNames()
    {
        return m_fluidSoundFontNames;
    }
    void setFluidSoundFontNames(QStringList names)
    {
        m_fluidSoundFontNames = names;
    }
    void setFluidSoundFontNames(QString names)
    {
        m_fluidSoundFontNames = QStringList(names);
    }
    void addFluidSoundFontName(QString sfName)
    {
        m_fluidSoundFontNames.append(sfName);
    }
    void removeFluidSoundFontName(QString sfName)
    {
        m_fluidSoundFontNames.removeAll(sfName);
    }
    void clearFluidSoundFontNames()
    {
        m_fluidSoundFontNames.clear();
    }
    void saveSoundFontSettings()
    {
        setValue("FluidSynth/SoundFont", getFluidSoundFontNames());
    }

    // has a new sound font been entered that is not the same as the old sound font
    bool isNewSoundFontEntered()
    {
        if (getFluidSoundFontNames().isEmpty())
            return false;

        return getFluidSoundFontNames() != value("FluidSynth/SoundFont").toStringList();
    }

    void pianistActive() { m_pianistActive = true;}
    void setActiveHand(whichPart_t hand);

    void setChannelHands(int left, int right);

    void fastUpdateRate(bool fullSpeed);
    QString getWarningMessage() {return m_warningMessage;}
    void updateWarningMessages();

    CColor menuColor() ;
    CColor menuSelectedColor() ;

    CColor staveColor() ;
    CColor staveColorDim() ;
    CColor noteColor() ;
    CColor noteColorDim() ;
    //static CColor playedGoodColor()    {return CColor(0.4, 0.4, 0.0);}
    CColor playedGoodColor() ;
    CColor playedBadColor()  ;
    CColor playedStoppedColor() ;
    CColor backgroundColor()  ;
    CColor barMarkerColor() ;
    CColor beatMarkerColor() ;
    CColor pianoGoodColor() ;
    CColor pianoBadColor() ;
    CColor noteNameColor() ;

    CColor playZoneAreaColor() ;
    CColor playZoneEndColor() ;
    CColor playZoneMiddleColor() ;

    static const int colorCount = 17;

    const char * colorNames[colorCount*3][2] = {
        {"noteDimRed", "191"},
        {"noteDimGreen", "140"},
        {"noteDimBlue", "191"},
        {"barMarkerRed", "179"},
        {"barMarkerGreen", "191"},
        {"barMarkerBlue", "191"},
        {"noteNameRed", "0"},
        {"noteNameGreen", "0"},
        {"noteNameBlue", "0"},
        {"pianoBadRed", "0"},
        {"pianoBadGreen", "255"},
        {"pianoBadBlue", "255"},
        {"staveDimRed", "191"},
        {"staveDimGreen", "179"},
        {"staveDimBlue", "191"},
        {"bgRed", "230"},
        {"bgGreen", "230"},
        {"bgBlue", "230"},
        {"menuSelectedRed", "77"},
        {"menuSelectedGreen", "77"},
        {"menuSelectedBlue", "230"},
        {"noteRed", "140"},
        {"noteGreen", "115"},
        {"noteBlue", "140"},
        {"staveRed", "153"},
        {"staveGreen", "143"},
        {"staveBlue", "148"},
        {"menuRed", "230"},
        {"menuGreen", "102"},
        {"menuBlue", "102"},
        {"playStoppedRed", "0"},
        {"playStoppedGreen", "51"},
        {"playStoppedBlue", "0"},
        {"beatMarkerRed", "196"},
        {"beatMarkerGreen", "199"},
        {"beatMarkerBlue", "199"},
        {"playZoneBgRed", "219"},
        {"playZoneBgGreen", "222"},
        {"playZoneBgBlue", "214"},
        {"playGoodRed", "128"},
        {"playGoodGreen", "102"},
        {"playGoodBlue", "0"},
        {"playBadRed", "51"},
        {"playBadGreen", "179"},
        {"playBadBlue", "51"},
        {"playZoneEndLineRed", "189"},
        {"playZoneEndLineGreen", "194"},
        {"playZoneEndLineBlue", "179"},
        {"playZoneMiddleRed", "158"},
        {"playZoneMiddleGreen", "163"},
        {"playZoneMiddleBlue", "179"}
    };

    static void clearCache();

    QColor getQColor(string name) {
        CColor color = getColor(name);
        QColor qColor((int) (color.red * 255), (int) (color.green * 255), (int) (color.blue * 255), 255);
        return qColor;
    }

    CColor getColor(string name) {
        std::unordered_map<std::string, CColor* >::iterator colorIter = CSettings::colorCache.find(name);
        if ( colorIter != CSettings::colorCache.end()) {
            return *(colorIter->second);
        }
        
        string colorName = "ScoreColors/";
        colorName.append(name);
        string redName = colorName + "Red";
        string greenName = colorName + "Green";
        string blueName = colorName + "Blue";

        int red = value(QString::fromUtf8(redName.c_str()), "0").toInt();
        int green = value(QString::fromUtf8(greenName.c_str()), "0").toInt();
        int blue = value(QString::fromUtf8(blueName.c_str()), "0").toInt();

        CColor * color = new CColor(red/255.0f, green/255.0f, blue/255.0f);

        CSettings::colorCache[name] = color;

        return *color;
        
    };

    QString selectedLangauge() {
        QString locale = value("General/lang","").toString();
        if (locale.isEmpty()) {
            locale = QLocale::system().bcp47Name();
            int n = locale.indexOf("_");
            if ((n > 0)) {
                locale = locale.left(n);
            }
        }
        return locale;
    }

private:

    Q_OBJECT
    QDomElement openDomElement(QDomElement parent, const QString & elementName, const QString & attributeName = QString());
    void loadHandSettings();
    void saveHandSettings();
    void loadPartSettings();
    void savePartSettings();
    void loadSongSettings();
    void saveSongSettings();
    void loadBookSettings();
    void saveBookSettings();
    void loadXmlFile();
    void saveXmlFile();
    void setDefaultValue(const QString & key, const QVariant & value );
    void setupDefaultSoundFont();

    // returns either 'left' 'right' or 'both'
    const QString partToHandString(whichPart_t part)
    {
        if (part == PB_PART_left)
            return "left";
        else if (part == PB_PART_right)
            return "right";
        return "both";
    }

    QDomDocument m_domDocument; //  The Complete XML DOM document for one book
    QDomElement m_domBook;      // only one book
    QDomElement m_domSong;      // The Elements for each song
    QDomElement m_domHand;      // The saved settings for each hand

    QtWindow *m_mainWindow;

    CSong* m_song;
    GuiSidePanel* m_guiSidePanel;
    GuiTopBar* m_guiTopBar;
    bool m_noteNamesEnabled;
    bool m_coloredNotes;
    bool m_tutorPagesEnabled;
    bool m_advancedMode;
    bool m_followThroughErrorsEnabled;
    QString m_bookPath;
    QString m_currentBookName;
    QString m_currentSongName;
    QString m_warningMessage;
    QStringList m_fluidSoundFontNames = QStringList();
    bool m_pianistActive;
};

#endif // __SETTINGS_H__
