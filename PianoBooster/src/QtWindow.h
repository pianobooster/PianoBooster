/*!
    @file           QtWindow.h

    @brief          xxx.

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

#ifndef __QT_WINDOW_H__
#define __QT_WINDOW_H__

#include <QtGui>

#include "Song.h"
#include "Score.h"
#include "GuiMidiSetupDialog.h"
#include "GuiKeyboardSetupDialog.h"
#include "GuiSidePanel.h"
#include "GuiTopBar.h"
#include "GuiPreferencesDialog.h"
#include "GuiSongDetailsDialog.h"
#include "GuiLoopingPopup.h"
#include "Settings.h"



class CGLView;
class QAction;
class QMenu;

class QSlider;
class QPushButton;

class QtWindow : public QMainWindow
{
    Q_OBJECT

public:
    QtWindow();
    ~QtWindow();

    void songEventUpdated(int eventBits)
    {
        if ((eventBits & EVENT_BITS_playingStopped) != 0)
            m_topBar->setPlayButtonState(false, true);
    }

private slots:
    void open();
    void help();
    void website();
    void about();
    void keyboardShortcuts();

    void showMidiSetup()
    {
        GuiMidiSetupDialog midiSetupDialog(this);
        midiSetupDialog.init(m_song, m_settings);
        midiSetupDialog.exec();
    }

    void showPreferencesDialog()
    {
        GuiPreferencesDialog preferencesDialog(this);
        preferencesDialog.init(m_song, m_settings, m_glWidget);
        preferencesDialog.exec();
    }

    void showSongDetailsDialog()
    {
        GuiSongDetailsDialog songDetailsDialog(this);
        songDetailsDialog.init(m_song, m_settings);
        songDetailsDialog.exec();
    }

    void showKeyboardSetup()
    {
        GuiKeyboardSetupDialog keyboardSetup(this);
        keyboardSetup.init(m_song, m_settings);
        keyboardSetup.exec();
    }

    void toggleSidePanel()
    {
        m_sidePanel->setVisible(!m_sidePanel->isVisible());
    }

    void enableFollowTempo()
    {
        CTempo::enableFollowTempo(Cfg::experimentalTempo);
    }
    void disableFollowTempo()
    {
        CTempo::enableFollowTempo(false);
    }

    void on_rightHand()  {  m_sidePanel->setActiveHand(PB_PART_right); }
    void on_bothHands()  {  m_sidePanel->setActiveHand(PB_PART_both); }
    void on_leftHand()   {  m_sidePanel->setActiveHand(PB_PART_left); }
    void on_playFromStart()   {
        if(m_song->playingMusic())
            m_topBar->on_playButton_clicked(true); // Stop the music first if playing
        else
            m_topBar->on_playFromStartButton_clicked(true);
    }

    void on_playPause()   {  m_topBar->on_playButton_clicked(true); }
    void on_faster()   {
        float speed = m_song->getSpeed() + 0.04;
        m_song->setSpeed(speed);
        speed = m_song->getSpeed();
        m_topBar->setSpeed(static_cast<int>(speed*100 + 0.5));
    }
    void on_slower()   {
        float speed = m_song->getSpeed() - 0.04;
        m_song->setSpeed(speed);
        speed = m_song->getSpeed();
        m_topBar->setSpeed(static_cast<int>(speed*100 + 0.5));
    }
    void on_nextSong()   {  m_sidePanel->nextSong(+1); }
    void on_previousSong()   {  m_sidePanel->nextSong(-1); }


protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent ( QKeyEvent * event );
    void keyReleaseEvent ( QKeyEvent * event );

private:
    void decodeCommandLine();
    int decodeIntegerParam(QString arg, int defaultParam);
    void decodeMidiFileArg(QString arg);
    QString displayShortCut(QString code, QString description);
    void addShortcutAction(const QString & key, const char * method);


    void displayUsage();
    void createActions();
    void createMenus();
    void readSettings();
    void writeSettings();

    CSettings* m_settings;

    GuiSidePanel *m_sidePanel;
    GuiTopBar *m_topBar;


    CGLView *m_glWidget;
    QAction *m_openAct;
    QAction *m_exitAct;
    QAction *m_aboutAct;
    QAction *m_shortcutAct;
    QAction *m_songPlayAct;
    QAction *m_setupMidiAct;
    QAction *m_setupKeyboardAct;
    QAction *m_toggleSidePanelAct;
    QAction *m_setupPreferencesAct;
    QAction *m_songDetailsAct;

    QMenu *m_fileMenu;
    QMenu *m_viewMenu;
    QMenu *m_songMenu;
    QMenu *m_setupMenu;
    QMenu *m_helpMenu;

    CSong* m_song;
    CScore* m_score;
};

#endif // __QT_WINDOW_H__
