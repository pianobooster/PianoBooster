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

#ifdef __linux__
#ifndef USE_REALTIME_PRIORITY
#define USE_REALTIME_PRIORITY 0
#endif
#endif

#if USE_REALTIME_PRIORITY
#include <sched.h>

/* sets the process to "policy" policy at given priority */
static int set_realtime_priority(int policy, int prio)
{
    struct sched_param schp;
    memset(&schp, 0, sizeof(schp));

    schp.sched_priority = prio;
    if (sched_setscheduler(0, policy, &schp) != 0) {
        perror("sched_setscheduler");
        return -1;
    }
    return 0;
}
#endif


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


#if USE_REALTIME_PRIORITY
    int rt_prio = sched_get_priority_max(SCHED_FIFO);
    set_realtime_priority(SCHED_FIFO, rt_prio);
#endif

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

    QString midiInputName = m_settings->value("Midi/Input").toString();
    if (midiInputName.startsWith("None"))
        CChord::setPianoRange(PC_KEY_LOWEST_NOTE, PC_KEY_HIGHEST_NOTE);
    else
        CChord::setPianoRange(m_settings->value("Keyboard/LowestNote", 0).toInt(),
                          m_settings->value("Keyboard/HighestNote", 127).toInt());

    m_song->setLatencyFix(m_settings->value("Midi/Latency", 0).toInt());


#ifdef _WIN32
    m_glWidget->m_cfg_openGlOptimise = true; // don't default to true on windows
#else
    m_glWidget->m_cfg_openGlOptimise = true; // changed to default to false on platforms
#endif

    m_glWidget->m_cfg_openGlOptimise = m_settings->value("Display/OpenGlOptimise", m_glWidget->m_cfg_openGlOptimise ).toBool();
    m_song->cfg_timingMarkersFlag = m_settings->value("Score/TimingMarkers", m_song->cfg_timingMarkersFlag ).toBool();
    m_song->cfg_stopPointMode = static_cast<stopPointMode_t> (m_settings->value("Score/StopPointMode", m_song->cfg_stopPointMode ).toInt());

    m_song->openMidiPort(CMidiDevice::MIDI_INPUT, midiInputName);
    m_song->openMidiPort(CMidiDevice::MIDI_OUTPUT,m_settings->value("midi/output").toString());

    m_settings->loadSettings();

    show();
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

void QtWindow::addShortcutAction(const QString & key, const char * method)
{
    QAction* act = new QAction(this);
    act->setShortcut(m_settings->value(key).toString());
    connect(act, SIGNAL(triggered()), this, method);
    addAction(act);
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

    m_shortcutAct = new QAction(tr("&PC Shortcut Keys"), this);
    m_shortcutAct->setStatusTip(tr("The PC Keyboard shortcut keys"));
    connect(m_shortcutAct, SIGNAL(triggered()), this, SLOT(keyboardShortcuts()));

    m_setupMidiAct = new QAction(tr("&Midi Setup ..."), this);
    m_setupMidiAct->setShortcut(tr("Ctrl+S"));
    m_setupMidiAct->setStatusTip(tr("Setup the Midi input an output"));
    connect(m_setupMidiAct, SIGNAL(triggered()), this, SLOT(showMidiSetup()));

    m_setupKeyboardAct = new QAction(tr("Piano &Keyboard Setting ..."), this);
    m_setupKeyboardAct->setShortcut(tr("Ctrl+K"));
    m_setupKeyboardAct->setStatusTip(tr("Change the piano keybaord settings"));
    connect(m_setupKeyboardAct, SIGNAL(triggered()), this, SLOT(showKeyboardSetup()));

    m_toggleSidePanelAct = new QAction(tr("&Show/Hide the Side Panel"), this);
    m_toggleSidePanelAct->setShortcut(tr("F11"));
    connect(m_toggleSidePanelAct, SIGNAL(triggered()), this, SLOT(toggleSidePanel()));

    m_setupPreferencesAct = new QAction(tr("&Preferences ..."), this);
    m_setupPreferencesAct->setShortcut(tr("Ctrl+P"));
    connect(m_setupPreferencesAct, SIGNAL(triggered()), this, SLOT(showPreferencesDialog()));

    m_songDetailsAct = new QAction(tr("&Song Details ..."), this);
    m_songDetailsAct->setShortcut(tr("Ctrl+S"));
    connect(m_songDetailsAct, SIGNAL(triggered()), this, SLOT(showSongDetailsDialog()));

    QAction* act = new QAction(this);
    act->setShortcut(tr("Shift+F1"));
    connect(act, SIGNAL(triggered()), this, SLOT(enableFollowTempo()));
    addAction(act);

    act = new QAction(this);
    act->setShortcut(tr("Alt+F1"));
    connect(act, SIGNAL(triggered()), this, SLOT(disableFollowTempo()));
    addAction(act);

    addShortcutAction("ShortCuts/RightHand",        SLOT(on_rightHand()));
    addShortcutAction("ShortCuts/BothHands",        SLOT(on_bothHands()));
    addShortcutAction("ShortCuts/LeftHand",         SLOT(on_leftHand()));
    addShortcutAction("ShortCuts/PlayFromStart",    SLOT(on_playFromStart()));
    addShortcutAction("ShortCuts/PlayPause",        SLOT(on_playPause()));
    addShortcutAction("ShortCuts/Faster",           SLOT(on_faster()));
    addShortcutAction("ShortCuts/Slower",           SLOT(on_slower()));
    addShortcutAction("ShortCuts/NextSong",         SLOT(on_nextSong()));
    addShortcutAction("ShortCuts/PreviousSong",     SLOT(on_previousSong()));
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

    QAction* act;
    act = new QAction(tr("&Help"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(help()));
    m_helpMenu->addAction(act);

    act = new QAction(tr("&Website"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(website()));
    m_helpMenu->addAction(act);

    m_helpMenu->addAction(m_shortcutAct);
    m_helpMenu->addAction(m_aboutAct);
}
void QtWindow::website()
{
    QDesktopServices::openUrl(QUrl("http://pianobooster.sourceforge.net"));
}

void QtWindow::help()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle (tr("Piano Booster Help"));
    msgBox.setText(
            tr(
   "<h3>Getting Started</h3>"


   "<p>You need a <b>MIDI Piano Keyboard </b> and a <b>MIDI interface</b> for the PC. If you "
   "don't have a MIDI keyboard you can still try out PianoBooster using the PC keyboard, 'X' is "
   "middle C.</p>"

   "<p>To hear the music you will need a <b>General Midi sound synthesizer</b>. "
   "The \"Microsoft GS Wavetable software synthesizer\" that comes with Windows can be used "
   "but it introduces an unacceptable delay (latency).</p>"


   "<p>PianoBooster does not come with any <b>MIDI Files</b> and so you will need to get them from the net. "
   "See the <a href=\"http://pianobooster.sourceforge.net/faq.html\" ><b>PianoBooster FAQ</b></a> "
   "for where to get MIDI files. "
   "PianoBooster works best with MIDI files that have separate left and right piano parts "
   "using MIDI channels 3 and 4."

   "<h3>Setting Up</h3>"


   "<p>First use the <i>Setup/Midi Setup</i> menu and in the dialog box select the MIDI input and MIDI "
   "output interfaces that match your hardware. "

   "Next use <i>File/Open</i> to open the MIDI file \".mid\" or a karaoke \".kar\" file. "
   "Now select whether you want to just <i>listen</i> to the music or "
   "<i>play along</i> on the piano keyboard by setting the <i>skill</i> level on the side panel. Finally when "
   "you are ready click the <i>play icon</i> (or press the <i>space bar</i>) to roll the music."

   "<h3>Hints on Playing the Piano</h3>"
   "<p>For hints on how to play the piano see: "
   "<a href=\"http://pianobooster.sourceforge.net/pianohints.html\" ><b>Piano Hints</b></a></p>"

   "<h3>More Information</h3>"
   "<p>For more help please visit the PianoBooster "
   "<a href=\"http://pianobooster.sourceforge.net/\" ><b>website</b></a>, "
   "the PianoBooster <a href=\"http://pianobooster.sourceforge.net/faq.html\" ><b>FAQ</b></a> "
   "and the <a href=\"http://n2.nabble.com/Piano-Booster-Users-f1591936.html\" ><b>user forum</b></a>. "

                ));
    msgBox.setMinimumWidth(600);
    msgBox.exec();
}

void QtWindow::about()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle (tr("About Piano Booster"));
    msgBox.setText(
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
    msgBox.setMinimumWidth(600);
    msgBox.exec();
}

QString QtWindow::displayShortCut(QString key, QString description)
{
    QString str = QString("<tr>"
                "<td>%1</td>"
                "<td>%2</td>"
                "</tr>").arg( description ).arg( m_settings->value(key).toString());
    return str;

}

void QtWindow::keyboardShortcuts()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle (tr("PC Keyboard Short Cuts"));
    QString msg =
            tr(
                "<h2><center>Keyboard short cuts</center></h2>"
                "<p>The following PC keyboard short cuts have been defined.</p>"
                "<center><table  border='1' cellspacing='0' cellpadding='4' >"
                );

    msg += tr(
                "<tr>"
                "<th>Action</th>"
                "<th>Key</th>"
                "</tr>"
            );
    msg += displayShortCut("ShortCuts/RightHand","Choose the right hand");
    msg += displayShortCut("ShortCuts/BothHands","Choose both hands");
    msg += displayShortCut("ShortCuts/LeftHand","Choose the left Hand");
    msg += displayShortCut("ShortCuts/PlayFromStart","Play from start toggle");
    msg += displayShortCut("ShortCuts/PlayPause","Play Pause Toggle");
    msg += displayShortCut("ShortCuts/Faster","Increase the speed by 5%");
    msg += displayShortCut("ShortCuts/Slower","Increase the speed by 5%");
    msg += displayShortCut("ShortCuts/NextSong","Change to the Next Song");
    msg += displayShortCut("ShortCuts/PreviousSong","Change to the Previous Song");

    msg += tr(
                "<tr><td>Fake Piano keys</td><td>X is middle C</td></tr>"
                "</table> </center><br>"
                );
    msgBox.setText(msg);

    msgBox.setMinimumWidth(600);
    msgBox.exec();
}


void QtWindow::open()
{
    QString currentSong = m_settings->getCurrentSongLongFileName();

    if (currentSong.isEmpty())
        currentSong = QDir::homePath();

    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Midi File"),
                            currentSong, tr("Midi Files (*.mid *.MID *.midi *.kar *.KAR)"));
    if (!fileName.isEmpty())
        m_settings->openSongFile(fileName);
}

void QtWindow::readSettings()
{
    QPoint pos = m_settings->value("Window/Pos", QPoint(25, 25)).toPoint();
    QSize size = m_settings->value("Window/Size", QSize(800, 600)).toSize();
    resize(size);
    move(pos);
}

void QtWindow::writeSettings()
{
    m_settings->setValue("Window/Pos", pos());
    m_settings->setValue("Window/Size", size());
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

