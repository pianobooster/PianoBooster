/*!
    @file           Settings.h

    @brief          Save all the settings for the programme in the right place.

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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <QSettings>
#include <QDomDocument>
#include "Song.h"

class GuiSidePanel;
class GuiTopBar;

class CSettings : public QSettings
{

public:
    CSettings(QWidget *mainWindow);
    void load();
    void save();
    void openSongFile(QString filename, bool bookListReload = false);

    void init(CSong* song, GuiSidePanel* sidePanel, GuiTopBar* topBar);

    bool isNoteNamesEnabled() { return m_noteNamesEnabled; }
    void setNoteNamesEnabled(bool value);
    void setAdvancedMode(bool value) { m_advancedMode = value;}
    bool showNoteNames(){
        if (m_noteNamesEnabled && !m_advancedMode)
            return true;
        return false;
    }
private:

    QDomDocument m_domDocument;

    QWidget *m_mainWindow;

    CSong* m_song;
    GuiSidePanel* m_guiSidePanel;
    GuiTopBar* m_guiTopBar;
    bool m_noteNamesEnabled;
    bool m_advancedMode;
};

#endif // __SETTINGS_H__
