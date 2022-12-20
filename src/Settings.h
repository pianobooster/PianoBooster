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

#include <QSettings>
#include <QDomDocument>
#include "Song.h"
#include "Notation.h"

#define QSTR_APPNAME "pianobooster"

class GuiSidePanel;
class GuiTopBar;
class QtWindow;

/// Save all the settings for the programme in the right place.
class CSettings : public QSettings
{

public:
    CSettings(QtWindow *mainWindow);

    void init(CSong* song, GuiSidePanel* sidePanel, GuiTopBar* topBar);

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
    void setFluidSoundFontNames(const QStringList &names)
    {
        m_fluidSoundFontNames = names;
    }
    void setFluidSoundFontNames(const QString &names)
    {
        m_fluidSoundFontNames = QStringList(names);
    }
    void addFluidSoundFontName(const QString &sfName)
    {
        m_fluidSoundFontNames.append(sfName);
    }
    void removeFluidSoundFontName(const QString &sfName)
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
