/*!
    @file           QtWindow.cpp

    @brief          xxx.

    @author         L. J. Barman

    Copyright (c)   2008, L. J. Barman, all rights reserved

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


class CGLView;
class QAction;
class QMenu;

class QSlider;
class QPushButton;

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window();
    ~Window();

    void songEventUpdated(int eventBits)
    {
        if ((eventBits & EVENT_BITS_playingStopped) != 0)
            m_topBar->setPlayButtonState(false, true);
    }

private slots:
    void open();
    void about();
    void playMusic(bool start)
    {
        if (m_song)
        {
            bool start = !m_song->playingMusic();
            if (start == true)
                m_song->rewind();
            m_song->playMusic(start);
            m_songButton->setChecked(start);
        }
    }

    void setSpeed(int speed)
    {
        if (!m_song) return;
        m_song->setSpeed(speed/100.0);
    }

    void showMidiSetup()
    {
        GuiMidiSetupDialog *midiSetupDialog = new GuiMidiSetupDialog(this);
        midiSetupDialog->init(m_song, m_settings);
        midiSetupDialog->exec();
    }

    void showKeyboardSetup()
    {
        GuiKeyboardSetupDialog *keyboardSetup = new GuiKeyboardSetupDialog(this);
        keyboardSetup->init(m_song, m_settings);
        keyboardSetup->exec();
    }

    void toggleSidePanel()
    {
        m_sidePanel->setVisible(!m_sidePanel->isVisible());
    }

    void enableFollowTempo()
    {
        CTempo::enableFollowTempo(true);
    }
    void disableFollowTempo()
    {
        CTempo::enableFollowTempo(false);
    }


protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent ( QKeyEvent * event );
    void keyReleaseEvent ( QKeyEvent * event );

private:
    void decodeCommandLine();
    void displayUsage();
    void createActions();
    void createMenus();
    void createToolBars();
    void createSongControls();
    void readSettings();
    void writeSettings();

    QSettings* m_settings;

    GuiSidePanel *m_sidePanel;
    GuiTopBar *m_topBar;


    CGLView *m_glWidget;
    QAction *m_openAct;
    QAction *m_exitAct;
    QAction *m_aboutAct;
    QAction *m_songPlayAct;
    QAction *m_setupMidiAct;
    QAction *m_setupKeyboardAct;

    QMenu *m_fileMenu;
    QMenu *m_setupMenu;
    QMenu *m_helpMenu;
    QToolBar *m_songToolBar;

    QPushButton* m_songButton;
    QSpinBox* m_speedSpin;
    CSong* m_song;
    CScore* m_score;
};

#endif // __QT_WINDOW_H__
