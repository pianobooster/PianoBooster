/*!
    @file           Settings.cpp

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

/*!
 * The following attributes are save in the pb.cfg xml file
 * BOOK:
 *   name       the name of the book
 *   lastsong   the name of the last song played so it can be restored
 *
 * SONG:
 *   name
 *   hand
 *   leftHandMidiChannel
 *   rightHandMidiChannel
 *
 * HAND:
 *   speed
 *
*/

#include <QTextStream>
#include <QFile>
#include "Settings.h"
#include "GuiTopBar.h"
#include "GuiSidePanel.h"
#include "QtWindow.h"
#include "ReleaseNote.txt"


#define OPTION_DEBUG_SETTINGS     0
#if OPTION_DEBUG_SETTINGS
#define debugSettings(args)     qDebug args
#else
#define debugSettings(args)
#endif


CSettings::CSettings(QtWindow *mainWindow) : QSettings(CSettings::IniFormat, CSettings::UserScope, "PianoBooster", "Piano Booster"),
                                 m_mainWindow(mainWindow)
{
    // It is all done in the initialisation list

    m_advancedMode = false;
    m_pianistActive = false;
    m_noteNamesEnabled = value("Score/NoteNames", true ).toBool();
    m_tutorPagesEnabled = value("Tutor/TutorPages", true ).toBool();
    CNotation::setCourtesyAccidentals(value("Score/CourtesyAccidentals", false ).toBool());
}

void CSettings::setDefaultValue(const QString & key, const QVariant & value )
{
    if (contains(key)) // Do not change the value if it already set
        return;

    setValue(key, value);
}


void CSettings::init(CSong* song, GuiSidePanel* sidePanel, GuiTopBar* topBar)
{
    m_song = song;
    m_guiSidePanel = sidePanel;
    m_guiTopBar = topBar;
}


void CSettings::setNoteNamesEnabled(bool value) {
    m_noteNamesEnabled = value;
    setValue("Score/NoteNames", value );
}

void CSettings::setTutorPagesEnabled(bool value) {
    m_tutorPagesEnabled = value;
    setValue("Tutor/TutorPages", value );
    updateTutorPage();
}


void CSettings::setCourtesyAccidentals(bool value) {
    CNotation::setCourtesyAccidentals(value);
    setValue("Score/CourtesyAccidentals", value );
}

// Open a document if it exists or else create it (also delete an duplicates
QDomElement CSettings::openDomElement(QDomElement parent, const QString & elementName, const QString & attributeName)
{
    QDomElement wantedElement;

    debugSettings(("openDomElement1 %s %s %s", qPrintable(parent.tagName()), qPrintable(elementName), qPrintable(elementName)));
    // There should be only a single element without a name
    // there should be lots of elemens but only one with this tag name

    QDomNode n = parent.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        debugSettings(("openDomElement2 tagName %s %s", qPrintable(e.tagName()), qPrintable(elementName)));
        if(!e.isNull() && e.tagName() == elementName)
        {
            if (attributeName.isEmpty() || e.attribute("name") == attributeName)
            {
                debugSettings(("openDomElement3 ragName %s %s", qPrintable(e.attribute("name")), qPrintable(attributeName)));

                if (wantedElement.isNull())
                    wantedElement = e;   // we have found a match
                else
                    parent.removeChild(e); // remove unwanted duplicates
            }
        }
        n = n.nextSibling();
    }

    if (wantedElement.isNull())
    {
        // Create the element because it does not exsist
        wantedElement = m_domDocument.createElement(elementName);
        if (!attributeName.isEmpty() )
            wantedElement.setAttribute("name", attributeName);
        parent.appendChild(wantedElement);
    }

    return wantedElement;
}

void CSettings::loadHandSettings()
{
    if (m_domSong.isNull())
        return;
    m_domHand = openDomElement(m_domSong, "hand", partToHandString(m_song->getActiveHand()));
    //m_guiTopBar->setSpeed(m_domHand.attribute("speed", "100" ).toInt());
}

void CSettings::saveHandSettings()
{
    //m_domHand.setAttribute("speed", m_guiTopBar->getSpeed());
}

void CSettings::loadSongSettings()
{
    m_domSong = openDomElement(m_domBook, "song", m_currentSongName);
    m_guiSidePanel->setCurrentHand(m_domSong.attribute("hand", "both" ));
    m_guiTopBar->setSpeed(m_domSong.attribute("speed", "100" ).toInt());


    // -1 means none and -2 means not set
    int left = m_domSong.attribute("leftHandMidiChannel", "-2").toInt();
    int right = m_domSong.attribute("rightHandMidiChannel", "-2").toInt();
    CNote::setChannelHands(left, right);

    loadHandSettings();
}


void CSettings::saveSongSettings()
{
    m_domSong.setAttribute("hand", partToHandString(m_song->getActiveHand()));
    m_domSong.setAttribute("speed", m_guiTopBar->getSpeed());

    saveHandSettings();
}

void CSettings::loadBookSettings()
{
    QDomElement root = m_domDocument.documentElement();
    m_domBook = openDomElement(root, "book");
    QString lastSong = m_domBook.attribute("lastsong");
    if (!lastSong.isEmpty() && m_currentSongName.isEmpty())
        m_currentSongName = lastSong;
}


void CSettings::saveBookSettings()
{
    if (!m_currentBookName.isEmpty())
        m_domBook.setAttribute("name", m_currentBookName);
    if (!m_currentSongName.isEmpty())
        m_domBook.setAttribute("lastsong", m_currentSongName);

    saveSongSettings();
}


void CSettings::loadXmlFile()
{
    m_domDocument.documentElement().clear();
    m_domDocument.clear();
    m_domBook.clear();
    m_domSong.clear();
    m_domHand.clear();

    QFile file(m_bookPath + getCurrentBookName() + '/' + "pb.cfg");
    if (file.open(QIODevice::ReadOnly))
    {
        if (!m_domDocument.setContent(&file)) {
            ppLogError("Cannot setContent on XLM file");
        }
        file.close();
    }

    QDomElement root = m_domDocument.documentElement();
    if (root.tagName() != "pianobooster")
    {
        m_domDocument.clear();
        QDomComment comment =    m_domDocument.createComment("Piano Booster Configuration file");
        m_domDocument.appendChild(comment);
        root = m_domDocument.createElement("pianobooster");
        m_domDocument.appendChild(root);
    }

    loadBookSettings();
}

// save the xml
void CSettings::saveXmlFile()
{
    saveBookSettings();

    const int IndentSize = 4;

    QFile file(m_bookPath + getCurrentBookName() + '/' + "pb.cfg");

    // don't save the config file unless the user really is using the system
    if (m_pianistActive == false && file.exists() == false)
        return;

    m_pianistActive = false;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        ppLogError("Cannot save xml file %s", qPrintable(file.fileName()));
        return;
    }

    QTextStream out(&file);
    m_domDocument.save(out, IndentSize);
    file.close();
}

void CSettings::updateTutorPage()
{
    QFileInfo fileInfo(getCurrentSongLongFileName());
    const char* EXTN = ".html";

    QString fileBase = fileInfo.absolutePath() + "/InfoPages/" + fileInfo.completeBaseName() + "_";

    QString locale = QLocale::system().name();

    if (m_tutorPagesEnabled)
    {
		QFileInfo tutorFile;
 		tutorFile.setFile(fileBase + locale + EXTN);
        if (tutorFile.exists())
        {
            m_mainWindow->loadTutorHtml(tutorFile.absoluteFilePath());
            return;
        }
        int n = locale.indexOf("_");

        if (n > 0)
        {
            locale = locale.left(n);
 			tutorFile.setFile(fileBase + locale + EXTN);
            if (tutorFile.exists())
            {
                m_mainWindow->loadTutorHtml(tutorFile.absoluteFilePath());
                return;
            }
        }

        locale = "en";
 		tutorFile.setFile(fileBase + locale + EXTN);
        if (tutorFile.exists())
        {
            m_mainWindow->loadTutorHtml(tutorFile.absoluteFilePath());
            return;
        }
    }
    m_mainWindow->loadTutorHtml(QString());

}

void CSettings::openSongFile(const QString & filename)
{
    if (!QFile::exists(filename))
    {
        ppLogError( "File does not exists %s", qPrintable(filename));
        return;
    }
    QDir dirBooks;
    QString  currentSongName;
    if (filename.isEmpty())
    {
        dirBooks.setPath( QDir::homePath());
    }
    else
    {
        dirBooks.setPath(filename);
        currentSongName = dirBooks.dirName();
        dirBooks.cdUp();
        m_currentBookName = dirBooks.dirName();
        dirBooks.cdUp();
    }
    m_bookPath =  dirBooks.path() + '/';

    m_currentSongName = currentSongName;
    m_guiSidePanel->loadBookList();
    updateWarningMessages();
}

void CSettings::setActiveHand(whichPart_t hand)
{
    saveHandSettings();
    m_song->setActiveHand(hand);
    loadHandSettings();
}

QStringList CSettings::getSongList()
{
    debugSettings(("getSongList %s + %s", qPrintable(getCurrentBookName()), qPrintable(m_bookPath)));
    QDir dirSongs = QDir(m_bookPath + getCurrentBookName());
    dirSongs.setFilter(QDir::Files);
    QStringList fileNames = dirSongs.entryList();


    QStringList songNames;
    for (int i = 0; i < fileNames.size(); i++)
    {
        if ( fileNames.at(i).endsWith(".mid", Qt::CaseInsensitive ) ||
             fileNames.at(i).endsWith(".midi", Qt::CaseInsensitive ) ||
             fileNames.at(i).endsWith(".kar", Qt::CaseInsensitive ) )
        {
            songNames  +=  fileNames.at(i);
        }
    }

    return songNames;
}

QStringList CSettings::getBookList()
{
    QDir dirBooks( m_bookPath);
    dirBooks.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    return dirBooks.entryList();
}


void CSettings::writeSettings()
{

    if (QFile::exists(getCurrentSongLongFileName() ))
        setValue("CurrentSong", getCurrentSongLongFileName());
    saveXmlFile();
}


void CSettings::loadSettings()
{
    unzipBootserMusicBooks();
    // Set default values
    setValue("PianoBooster/Version", PB_VERSION);
    setDefaultValue("ShortCuts/LeftHand", "F2");
    setDefaultValue("ShortCuts/BothHands","F3");
    setDefaultValue("ShortCuts/RightHand","F4");
    setDefaultValue("ShortCuts/PlayFromStart","space");
    setDefaultValue("ShortCuts/PlayPause","P");
    setDefaultValue("ShortCuts/Faster","=");
    setDefaultValue("ShortCuts/Slower","-");
    setDefaultValue("ShortCuts/NextSong","]");
    setDefaultValue("ShortCuts/PreviousSong","[");
    setDefaultValue("ShortCuts/NextBook","{");
    setDefaultValue("ShortCuts/PreviousBook","}");
    QString songName = value("CurrentSong").toString();
    if (!songName.isEmpty())
        openSongFile( songName );

    updateWarningMessages();
    updateTutorPage();

}

void CSettings::unzipBootserMusicBooks()
{
    // Set default values

    const int MUSIC_RELEASE = 1;
    const QString ZIPFILENAME("BoosterMusicBooks.zip");


    if (value("PianoBooster/MusicRelease", 0).toInt() < MUSIC_RELEASE)
    {
        QString resourceDir = QApplication::applicationDirPath() + "/../music/";

        ppLogTrace("resourceDir1 %s", qPrintable(resourceDir));

        if (!QFile::exists(resourceDir + ZIPFILENAME))
            resourceDir = QApplication::applicationDirPath() + "/../../music/";
        ppLogTrace("resourceDir2 %s", qPrintable(resourceDir));

        if (!QFile::exists(resourceDir + ZIPFILENAME))
        {
#ifdef Q_OS_LINUX
            resourceDir = QApplication::applicationDirPath() + "/../share/" + QSTR_APPNAME + "/music/";
#endif
#ifdef Q_OS_DARWIN
            resourceDir = QApplication::applicationDirPath() + "/../Resources/music/";
#endif
        }

        ppLogInfo(qPrintable("applicationDirPath=" + QApplication::applicationDirPath()));
        ppLogTrace("resourceDir3 %s", qPrintable(resourceDir));


        QFileInfo zipFile(resourceDir +  ZIPFILENAME);
        ppLogTrace("xx %s", qPrintable(zipFile.filePath()));
        
		QDir destMusicDir;
		
#ifdef _WIN32
        const QString MUSIC_DIR_NAME("My Music");
		QSettings settings(QSettings::UserScope, "Microsoft", "Windows");
		settings.beginGroup("CurrentVersion/Explorer/Shell Folders");
		destMusicDir.setPath(QDir::fromNativeSeparators(settings.value("Personal").toString()));
#else
        const QString MUSIC_DIR_NAME("Music");
		destMusicDir.setPath(QDir::homePath() );
#endif
         
        if (!QDir(destMusicDir.absolutePath() + "/" + MUSIC_DIR_NAME).exists())
        {
            destMusicDir.mkdir(MUSIC_DIR_NAME);
        }
        destMusicDir.setPath(destMusicDir.absolutePath() + "/" + MUSIC_DIR_NAME);

#ifndef _WIN32

		// on windows the the installer does the unzipping
        if (!zipFile.exists() )
        {
            ppLogError(qPrintable("Cannot find " + ZIPFILENAME));
            return;
        }



        QProcess unzip;
        unzip.start("unzip", QStringList() << "-o" << zipFile.filePath() << "-d" << destMusicDir.path() );
        ppLogInfo(qPrintable("running unzip -o " + zipFile.filePath() + " -d " + destMusicDir.path()) );
        char buf[1024];
        while (true)
        {
            qint64 lineLength = unzip.readLine(buf, sizeof(buf));
            if (lineLength <= 0)
                break;
            ppLogInfo(buf);
                 // the line is available in buf
        }


        if (!unzip.waitForFinished())
        {
             ppLogError("unzip failed");
             return;
        }
#endif       
		QString fileName(destMusicDir.absolutePath() + "/BoosterMusicBooks" + QString::number(MUSIC_RELEASE) + "/Booster Music/01-ClairDeLaLune.mid");
		openSongFile(fileName);
		m_mainWindow->setCurrentFile(fileName);
		setValue("PianoBooster/MusicRelease", MUSIC_RELEASE);
    }
}

void CSettings::setCurrentSongName(const QString & name)
{
    if (name.isEmpty())
        return;
    saveSongSettings();
    m_currentSongName = name;
    debugSettings(("setCurrentSongName %s -- %s", qPrintable(name), qPrintable(getCurrentSongLongFileName())));
    setValue("CurrentSong", getCurrentSongLongFileName());

    m_song->loadSong(getCurrentSongLongFileName());
    loadSongSettings();

    m_guiSidePanel->refresh();
    m_guiTopBar->refresh(true);
    m_mainWindow->setWindowTitle("Piano Booster - " + m_song->getSongTitle());
    updateTutorPage();
}

void CSettings::setCurrentBookName(const QString & name, bool clearSongName)
{
    if (name.isEmpty())
        return;
    if (!m_currentBookName.isEmpty() && m_currentBookName != name)
        saveXmlFile();

    m_currentBookName = name;
    if (clearSongName)
        m_currentSongName.clear();
    debugSettings(("setCurrentBookName %s --- %s ", qPrintable(name), qPrintable(getCurrentSongLongFileName())));
    loadXmlFile();
}

void CSettings::setChannelHands(int left, int right)
{
    CNote::setChannelHands(left, right);
    m_domSong.setAttribute("leftHandMidiChannel", left);
    m_domSong.setAttribute("rightHandMidiChannel", right);
    m_guiSidePanel->refresh();
}

void CSettings::updateWarningMessages()
{
    if (!m_song->validMidiOutput())
        m_warningMessage = tr("ERROR NO SOUND: To fix this use menu Setup/Midi Setup ...");
    else if (m_currentSongName.isEmpty())
        m_warningMessage = tr("ERROR NO MIDI FILE: To fix this use menu File/Open ...");
    else
        m_warningMessage.clear();
}

