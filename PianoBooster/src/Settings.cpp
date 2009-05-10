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

#include <QTextStream>
#include <QFile>
#include "Settings.h"
#include "GuiTopBar.h"
#include "GuiSidePanel.h"

CSettings::CSettings(QWidget *mainWindow) : QSettings(CSettings::IniFormat, CSettings::UserScope, "PianoBooster", "Piano Booster"),
                                 m_mainWindow(mainWindow)
{
    // It is all done in the initialisation list

    m_advancedMode = false;
    load();
    save();
    m_noteNamesEnabled = value("score/noteNames", false ).toBool();
}


void CSettings::setNoteNamesEnabled(bool value) {
    m_noteNamesEnabled = value;
    setValue("score/noteNames", value );
}


void CSettings::init(CSong* song, GuiSidePanel* sidePanel, GuiTopBar* topBar)
{
    m_song = song;
    m_guiSidePanel = sidePanel;
    m_guiTopBar = topBar;
}

void CSettings::load()
{
     QDomElement root = m_domDocument.createElement("pb-config");
     m_domDocument.appendChild(root);

     QDomElement book = m_domDocument.createElement("book");
     root.appendChild(book);
     QDomElement songs = m_domDocument.createElement("songs");
     root.appendChild(songs);

     QDomText t = m_domDocument.createTextNode("Hello World");
     songs.appendChild(t);
}
void CSettings::save()
{
    const int IndentSize = 4;

    QFile file("/home/louis/mydocument.xml");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
         return;

    QTextStream out(&file);
    m_domDocument.save(out, IndentSize);
    file.close();
}

/* fixme work in progress
     QDomDocument doc("mydocument");
     QFile file("mydocument.xml");
     if (!file.open(QIODevice::ReadOnly))
         return;
     if (!doc.setContent(&file)) {
         file.close();
         return;
     }
     file.close();

     // print out the element names of all elements that are direct children
     // of the outermost element.
     QDomElement docElem = doc.documentElement();

     QDomNode n = docElem.firstChild();
     while(!n.isNull()) {
         QDomElement e = n.toElement(); // try to convert the node to an element.
         if(!e.isNull()) {
             cout << qPrintable(e.tagName()) << endl; // the node really is an element.
         }
         n = n.nextSibling();
     }

     // Here we append a new element to the end of the document
     QDomElement elem = doc.createElement("img");
     elem.setAttribute("src", "myimage.png");
     docElem.appendChild(elem);
*/

//Once doc and elem go out of scope, the whole internal tree representing the XML document is deleted.
//To create a document using DOM use code like this:
/*
     QDomDocument doc("MyML");
     QDomElement root = doc.createElement("MyML");
     doc.appendChild(root);

     QDomElement tag = doc.createElement("Greeting");
     root.appendChild(tag);

     QDomText t = doc.createTextNode("Hello World");
     tag.appendChild(t);

     QString xml = doc.toString();
*/

void CSettings::openSongFile(QString filename, bool bookListReload)
{
    if (!QFile::exists(filename))
    {
        ppLogWarn( "File does not exists" + filename.toAscii());
        return;
    }

    setValue("CurrentSong", filename);

    if (bookListReload)
        m_guiSidePanel->loadBookList();

    m_song->loadSong(filename);
    m_guiTopBar->refresh(true);
    m_guiSidePanel->refresh();
    m_mainWindow->setWindowTitle("Piano Booster - " + m_song->getSongTitle());
}

