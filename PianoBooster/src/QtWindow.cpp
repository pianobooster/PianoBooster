/*!
    @file           QtWindow.cpp

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

#include "GlView.h"
#include "QtWindow.h"
#include "ReleaseNote.txt"

QtWindow::QtWindow()
{


    QCoreApplication::setOrganizationName("PianoBooster");
    QCoreApplication::setOrganizationDomain("pianobooster.sourceforge.net/");
    QCoreApplication::setApplicationName("Piano Booster");
    m_settings = new CSettings(this);
    setWindowIcon(QIcon(":/images/Logo32x32.png"));
    setWindowTitle(tr("Piano Booster"));

    decodeCommandLine();

    if (Cfg::experimentalSwapInterval != -1)
    {
        QGLFormat fmt;
        fmt.setSwapInterval(Cfg::experimentalSwapInterval);
        int value = fmt.swapInterval();
        ppLogInfo("Open GL Swap Interval %d", value);
        QGLFormat::setDefaultFormat(fmt);
    }


    m_glWidget = new CGLView(this, m_settings);

    m_song = m_glWidget->getSongObject();
    m_score = m_glWidget->getScoreObject();


    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *columnLayout = new QVBoxLayout;

    m_sidePanel = new GuiSidePanel(this, m_settings);
    m_topBar = new GuiTopBar(this, m_settings);

    m_settings->init(m_song, m_sidePanel, m_topBar);

    mainLayout->addWidget(m_sidePanel);
    columnLayout->addWidget(m_topBar);
    columnLayout->addWidget(m_glWidget);
    mainLayout->addLayout(columnLayout);

    m_song->init(m_score, m_settings);
    m_glWidget->init();

    m_sidePanel->init(m_song, m_song->getTrackList(), m_topBar);
    m_topBar->init(m_song, m_song->getTrackList());
    createActions();
    createMenus();
    readSettings();

    QWidget *centralWin = new QWidget();
    centralWin->setLayout(mainLayout);

    setCentralWidget(centralWin);

    m_glWidget->setFocus(Qt::ActiveWindowFocusReason);

    m_song->setPianoSoundPatches(m_settings->value("Keyboard/RightSound", Cfg::defaultRightPatch()).toInt() - 1,
                                 m_settings->value("Keyboard/WrongSound", Cfg::defaultWrongPatch()).toInt() - 1, true);

    QString midiInputName = m_settings->value("midi/input").toString();
    if (midiInputName.startsWith("None"))
        CChord::setPianoRange(PC_KEY_LOWEST_NOTE, PC_KEY_HIGHEST_NOTE);
    else
        CChord::setPianoRange(m_settings->value("Keyboard/lowestNote", 0).toInt(),
                          m_settings->value("Keyboard/highestNote", 127).toInt());

    m_song->setLatencyFix(m_settings->value("midi/latency", 0).toInt());


#ifdef _WIN32
    m_glWidget->m_cfg_openGlOptimise = true; // don't default to true on windows
#else
    m_glWidget->m_cfg_openGlOptimise = true; // changed to default to false on platforms
#endif

    m_glWidget->m_cfg_openGlOptimise = m_settings->value("display/openGlOptimise", m_glWidget->m_cfg_openGlOptimise ).toBool();
    m_song->cfg_timingMarkersFlag = m_settings->value("score/timingMarkers", m_song->cfg_timingMarkersFlag ).toBool();
    m_song->cfg_stopPointMode = static_cast<stopPointMode_t> (m_settings->value("score/stopPointMode", m_song->cfg_stopPointMode ).toInt());

    m_song->openMidiPort(CMidiDevice::MIDI_INPUT, midiInputName);

    m_settings->loadSettings();

    show();

    if (m_song->openMidiPort(CMidiDevice::MIDI_OUTPUT,m_settings->value("midi/output").toString())==false)
    {
        showMidiSetup();
    }
}

QtWindow::~QtWindow()
{
    delete m_settings;
}

///////////////////////////////////////////////////////////////////////////////
//! @brief               Displays the usage
void QtWindow::displayUsage()
{
    fprintf(stderr, "Usage: pianobooster [flags] [midifile]\n");
    fprintf(stderr, "       -d: Increase the debug level\n");
    fprintf(stderr, "       -s: Small screen display\n");
    fprintf(stderr, "       -q: Quick start\n");
    fprintf(stderr, "       -h: --help: Displays this help message\n");
    fprintf(stderr, "       -v: Displays version number and then exits\n");
}

int QtWindow::decodeIntegerParam(QString arg, int defaultParam)
{
    int n = arg.lastIndexOf('=');
    if (n == -1 || (n + 1) >= arg.size())
        return defaultParam;
    bool ok;
    int value = arg.mid(n+1).toInt(&ok);
    if (ok)
        return value;
    return defaultParam;
}

void QtWindow::decodeMidiFileArg(QString arg)
{

    QFileInfo fileInfo(arg);

    if (!fileInfo.exists() )
    {
        QMessageBox::warning(0, "PianoBooster Midi File Error",
                 "Cannot Open\"" + fileInfo.absoluteFilePath() + "\"");
        exit(1);
    }
        else if ( !(fileInfo.fileName().endsWith(".mid", Qt::CaseInsensitive ) ||
             fileInfo.fileName().endsWith(".midi", Qt::CaseInsensitive ) ||
             fileInfo.fileName().endsWith(".kar", Qt::CaseInsensitive )) )

	{
        QMessageBox::warning(0, "PianoBooster Midi File Error",
                 "Not a Midi File \"" + fileInfo.fileName() + "\"");
        exit(1);
	}
    else
    {
        bool vaildMidiFile = true;
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly))
            vaildMidiFile = false;
        else
        {
            QByteArray bytes = file.read(4);
            for (int i = 0; i < 4; i++)
            {
                if (bytes[i] !="MThd"[i] )
                    vaildMidiFile = false;
            }
            file.close();
        }
        if (vaildMidiFile ==  true)
            m_settings->setValue("CurrentSong", fileInfo.absoluteFilePath());
        else
        {
            QMessageBox::warning(0, "PianoBooster Midi File Error",
                 "Not a valid MIDI file \"" + fileInfo.absoluteFilePath() + "\"");
			exit(1);
		}
    }
}

void QtWindow::decodeCommandLine()
{
    bool hasMidiFile = false;
    QStringList argList = QCoreApplication::arguments();
    QString arg;
    for (int i = 0; i < argList.size(); ++i)
    {
        arg = argList[i];
        if (arg.startsWith("-"))
        {
            if (arg.startsWith("-d"))
                Cfg::logLevel++;
            else if (arg.startsWith("-s"))
                Cfg::smallScreen = true;
            else if (arg.startsWith("-q"))
                Cfg::quickStart = true;
            else if (arg.startsWith("-X1"))
                Cfg::experimentalTempo = true;
            else if (arg.startsWith("-Xswap"))
                Cfg::experimentalSwapInterval = decodeIntegerParam(arg, 100);

            else if (arg.startsWith("-h") || arg.startsWith("-?") ||arg.startsWith("--help"))
            {
                displayUsage();
                exit(0);
            }
            else if (arg.startsWith("-v"))
            {
                fprintf(stderr, "pianobooster Version " PB_VERSION"\n");
                exit(0);
            }
            else
            {
                fprintf(stderr, "ERROR: Unknown arguments \n");
                displayUsage();
                exit(0);
            }
        }
        else {
            if ( hasMidiFile == false && i > 0)
            {
                hasMidiFile = true;
                decodeMidiFileArg(arg);

            }
        }
    }
}

void QtWindow::createActions()
{
    m_openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    m_openAct->setShortcut(tr("Ctrl+O"));
    m_openAct->setStatusTip(tr("Open an existing file"));
    connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));

    m_exitAct = new QAction(tr("E&xit"), this);
    m_exitAct->setShortcut(tr("Ctrl+Q"));
    m_exitAct->setStatusTip(tr("Exit the application"));
    connect(m_exitAct, SIGNAL(triggered()), this, SLOT(close()));

    m_aboutAct = new QAction(tr("&About"), this);
    m_aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    m_setupMidiAct = new QAction(tr("&Midi Setup ..."), this);
    m_setupMidiAct->setShortcut(tr("Ctrl+S"));
    m_setupMidiAct->setStatusTip(tr("Setup the Midi input an output"));
    connect(m_setupMidiAct, SIGNAL(triggered()), this, SLOT(showMidiSetup()));

    m_setupKeyboardAct = new QAction(tr("&Keyboard setting ..."), this);
    m_setupKeyboardAct->setShortcut(tr("Ctrl+K"));
    m_setupKeyboardAct->setStatusTip(tr("Setup the Midi input an output"));
    connect(m_setupKeyboardAct, SIGNAL(triggered()), this, SLOT(showKeyboardSetup()));

    m_toggleSidePanelAct = new QAction(tr("&Show/Hide the side panel"), this);
    m_toggleSidePanelAct->setShortcut(tr("F11"));
    connect(m_toggleSidePanelAct, SIGNAL(triggered()), this, SLOT(toggleSidePanel()));

    m_setupPreferencesAct = new QAction(tr("&Preferences ..."), this);
    m_setupPreferencesAct->setShortcut(tr("Ctrl+P"));
    connect(m_setupPreferencesAct, SIGNAL(triggered()), this, SLOT(showPreferencesDialog()));

    m_songDetailsAct = new QAction(tr("&Song Details ..."), this);
    m_songDetailsAct->setShortcut(tr("Ctrl+S"));
    connect(m_songDetailsAct, SIGNAL(triggered()), this, SLOT(showSongDetailsDialog()));

    QAction* enableFollowTempoAct = new QAction(this);
    enableFollowTempoAct->setShortcut(tr("Shift+F1"));
    connect(enableFollowTempoAct, SIGNAL(triggered()), this, SLOT(enableFollowTempo()));
    addAction(enableFollowTempoAct);

    QAction* disableFollowTempoAct = new QAction(this);
    disableFollowTempoAct->setShortcut(tr("Alt+F1"));
    connect(disableFollowTempoAct, SIGNAL(triggered()), this, SLOT(disableFollowTempo()));
    addAction(disableFollowTempoAct);
}

void QtWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_openAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAct);

    m_viewMenu = menuBar()->addMenu(tr("&View"));
    m_viewMenu->addAction(m_toggleSidePanelAct);

    m_songMenu = menuBar()->addMenu(tr("&Song"));
    m_songMenu->addAction(m_songDetailsAct);

    m_setupMenu = menuBar()->addMenu(tr("Set&up"));
    m_setupMenu->addAction(m_setupMidiAct);
    m_setupMenu->addAction(m_setupKeyboardAct);
    m_setupMenu->addAction(m_setupPreferencesAct);

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_aboutAct);
}

void QtWindow::about()
{
    QMessageBox about(this);
    about.setWindowTitle (tr("About"));
    about.setText(
            tr(
                "<b>PainoBooster - Version " PB_VERSION "</b> <br><br>"
                "<b>Boost</b> your <b>Piano</b> playing skills!<br><br>"
                "<a href=\"http://pianobooster.sourceforge.net/\" ><b>http://pianobooster.sourceforge.net</b></a><br><br>"
                "Copyright(c) L. J. Barman, 2008-2009; All rights reserved.<br><br>"
                "This program is made available "
                "under the terms of the GNU General Public License version 3 as published by "
                "the Free Software Foundation.<br><br>"
                "This program also contains RtMIDI: realtime MIDI i/o C++ classes<br>"
                "Copyright(c) 2003-2007 Gary P. Scavone"
                ));
    about.setMinimumWidth(600);
    about.exec();
}

void QtWindow::open()
{
	m_glWidget->fastUpdateRate(false);
    QString currentSong = m_settings->getCurrentSongLongFileName();
	
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Midi File"),
                            currentSong, tr("Midi Files (*.mid *.MID *.midi *.kar *.KAR)"));
    if (!fileName.isEmpty())
        m_settings->openSongFile(fileName);
	m_glWidget->fastUpdateRate(true);
}

void QtWindow::readSettings()
{
    QPoint pos = m_settings->value("window/pos", QPoint(25, 25)).toPoint();
    QSize size = m_settings->value("window/size", QSize(800, 600)).toSize();
    resize(size);
    move(pos);
}

void QtWindow::writeSettings()
{
    m_settings->setValue("window/pos", pos());
    m_settings->setValue("window/size", size());
    m_settings->writeSettings();
}

void QtWindow::closeEvent(QCloseEvent *event)
{
    if (m_song->playingMusic())
    {
        m_song->playMusic(false);
    }

    writeSettings();
}


void QtWindow::keyPressEvent ( QKeyEvent * event )
{
    if (event->text().length() == 0)
        return;

    if (event->isAutoRepeat() == true)
        return;

    if (event->key() == Qt::Key_F1)
        return;

    int c = event->text().toAscii().at(0);
    m_song->pcKeyPress( c, true);
}

void QtWindow::keyReleaseEvent ( QKeyEvent * event )
{
    if (event->isAutoRepeat() == true)
        return;

    if (event->text().length() == 0)
        return;

    int c = event->text().toAscii().at(0);
    m_song->pcKeyPress( c, false);
}

void QtWindow::fastUpdateRate(bool fullSpeed)
{
	if (m_glWidget)
		m_glWidget->fastUpdateRate(fullSpeed);
}
