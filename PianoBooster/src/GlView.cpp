/*********************************************************************************/
/*!
@file           GlView.cpp

@brief          xxxx

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
/*********************************************************************************/

#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "QtWindow.h"
#include "GlView.h"
#include "Cfg.h"
#include "Draw.h"

// This defines the PB Open GL frame per seconds.
// Try to make sure this runs a bit faster than the screen refresh rate of 60z (or 16.6 msec)
#define SCREEN_FRAME_RATE 12 // That 12 msec or 83.3 frames per second

#define REDRAW_COUNT ((m_cfg_openGlOptimise >= 2) ? 1 : 2) // there are two gl buffers but redrawing once is best (set 2 with buggy gl drivers)


CGLView::CGLView(QtWindow* parent, CSettings* settings)
    : QGLWidget(parent)
{
    m_qtWindow = parent;
    m_settings = settings;
    m_rating = 0;
    m_fullRedrawFlag = true;
    m_forcefullRedraw = 0;
    m_forceRatingRedraw = 0;
    m_forceBarRedraw = 0;

    m_backgroundColour = QColor(0, 0, 0);

    m_song = new CSong();
    m_score = new CScore(m_settings);
    m_displayUpdateTicks = 0;
    m_cfg_openGlOptimise = 0; // zero is no GlOptimise
    m_eventBits = 0;
    BENCHMARK_INIT();
}

CGLView::~CGLView()
{
    makeCurrent();
    delete m_song;
    delete m_score;
    m_titleHeight = 0;

}

QSize CGLView::minimumSizeHint() const
{
    return QSize(200, 200);
}

QSize CGLView::sizeHint() const
{
    return QSize(200, 800); //fixme this does not work
}

void CGLView::paintGL()
{
    BENCHMARK(2, "enter");

    m_displayUpdateTicks = 0;

    if (m_fullRedrawFlag)
        m_forcefullRedraw = m_forceRatingRedraw = m_forceBarRedraw = REDRAW_COUNT;

    if (m_forcefullRedraw) // clear the screen only if we are doing a full redraw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    //BENCHMARK(3, "glLoadIdentity");

    drawDisplayText();
    BENCHMARK(4, "drawDisplayText");

    drawAccurracyBar();
    BENCHMARK(5, "drawAccurracyBar");

    drawBarNumber();
    BENCHMARK(6, "drawBarNumber");

    if (m_forcefullRedraw)
        m_score->drawScore();

    drawTimeSignature();

    updateMidiTask();
    m_score->drawScroll(m_forcefullRedraw);
    BENCHMARK(10, "drawScroll");


    if (m_forcefullRedraw) m_forcefullRedraw--;
    BENCHMARK(11, "exit");
    BENCHMARK_RESULTS();
}

void CGLView::drawTimeSignature()
{
    if (Cfg::quickStart)
        return;

    if (m_forcefullRedraw == 0)
        return;

    float x,y;
    int topNumber, bottomNumber;

    if (m_song == 0) return;

    m_song->getTimeSig(&topNumber, &bottomNumber);
    if (topNumber == 0 ) return;

    char bufferTop[10], bufferBottom[10];
    sprintf(bufferTop, "%d", topNumber);
    sprintf(bufferBottom, "%d", bottomNumber);

    x = Cfg::timeSignatureX();

    CDraw::drColour ((CDraw::getDisplayHand() != PB_PART_left) ? Cfg::noteColour() : Cfg::noteColourDim());

    y = CStavePos(PB_PART_right,  0).getPosY() + 5;
    renderText(x,y, 0, bufferTop, m_timeSigFont);
    y = CStavePos(PB_PART_right, -3).getPosY() - 2;
    renderText(x,y, 0, bufferBottom, m_timeSigFont);

    CDraw::drColour ((CDraw::getDisplayHand() != PB_PART_right) ? Cfg::noteColour() : Cfg::noteColourDim());

    y = CStavePos(PB_PART_left,   0).getPosY() + 5;
    renderText(x,y, 0, bufferTop, m_timeSigFont);
    y = CStavePos(PB_PART_left,  -3).getPosY() - 2;
    renderText(x,y, 0, bufferBottom, m_timeSigFont);
}

void CGLView::drawAccurracyBar()
{
    if (m_song->getPlayMode() == PB_PLAY_MODE_listen || !m_settings->getWarningMessage().isEmpty())
        return;

    if (m_forceRatingRedraw == 0)
        return;
    m_forceRatingRedraw--;


    float accuracy;
    CColour colour;

    float y = Cfg::getAppHeight() - 14;
    const float x = 120;
    const int width = 360;
    const int lineWidth = 8/2;

    m_rating->calculateAccuracy();

    accuracy = m_rating->getAccuracyValue();
    colour = m_rating->getAccuracyColour();
    CDraw::drColour (colour);
    glRectf(x, y - lineWidth, x + width * accuracy, y + lineWidth);
    CDraw::drColour (Cfg::backgroundColour());
    glRectf(x + width * accuracy, y - lineWidth, x + width, y + lineWidth);


    glLineWidth (1);
    CDraw::drColour (CColour(1.0, 1.0, 1.0));
    glBegin(GL_LINE_LOOP);
    glVertex2f (x, y + lineWidth);
    glVertex2f (x+ width, y  + lineWidth);
    glVertex2f (x + width, y - lineWidth);
    glVertex2f (x, y - lineWidth);
    glEnd();
}

void CGLView::drawDisplayText()
{
    if (m_rating == 0)
    {
        m_rating = m_song->getRating();
        return; // don't run this func the first time it is called
    }

    if (Cfg::quickStart)
        return;

    if (m_forcefullRedraw == 0)
        return;

    int y = Cfg::getAppHeight() - 14;

    if (!m_settings->getWarningMessage().isEmpty())
    {
        glColor3f(1.0,0.2,0.0);
        renderText(30, y-4, 0, m_settings->getWarningMessage(), m_timeRatingFont);
        return;
    }

    glColor3f(1.0,1.0,1.0);

    if (m_song->getPlayMode() != PB_PLAY_MODE_listen)
        renderText(30, y-4,0 ,tr("Accuracy:"), m_timeRatingFont);

    if (m_titleHeight < 45 )
        return;

    y = Cfg::getAppHeight() - m_titleHeight;

    renderText(30, y+6, 0,tr("Song: ") + m_song->getSongTitle(), m_timeRatingFont);
    /*
    char buffer[100];
    sprintf(buffer, "Notes %d wrong %d Late %d Score %4.1f%%",
    m_rating->totalNoteCount(), m_rating->wrongNoteCount(),
    m_rating->lateNoteCount(), m_rating->rating());
    renderText(10,20, buffer, m_timeRatingFont);
    */
}

void CGLView::drawBarNumber()
{
    if (m_forceBarRedraw == 0 || Cfg::quickStart)
        return;
    m_forceBarRedraw--;

    float y = Cfg::getAppHeight() - m_titleHeight - 34;
    float x = 30;

    //CDraw::drColour (Cfg::backgroundColour());
    //CDraw::drColour (Cfg::noteColourDim());
    //glRectf(x+30+10, y-2, x + 80, y + 16);
    glColor3f(1.0,1.0,1.0);
    renderText(x, y, 0, tr("Bar: ") + QString::number(m_song->getBarNumber()), m_timeRatingFont);
}

void CGLView::resizeGL(int width, int height)
{
    const int maxSoreWidth = 1024;
    const int staveEndGap = 20;
    const int heightAboveStave =  static_cast<int>(CStavePos::verticalNoteSpacing() * MAX_STAVE_INDEX);
    const int heightBelowStave =  static_cast<int>(CStavePos::verticalNoteSpacing() * - MIN_STAVE_INDEX);
    const int minTitleHeight = 20;
    const int minStaveGap = 120;
    int staveGap;
    int maxSoreHeight;

    int space = height - (heightAboveStave + heightBelowStave + minTitleHeight + minStaveGap);
    //m_titleHeight = qBound(minTitleHeight, minTitleHeight + space/2, 70);
    // staveGap = qBound(minStaveGap, minStaveGap+ space/2, static_cast<int>(CStavePos::staveHeight() * 3));
    if (height < 430)  // So it works on an eeepc 701 (for Trev)
    {
        staveGap = minStaveGap;
        m_titleHeight = minTitleHeight;
    }
    else
    {
        staveGap = static_cast<int>(CStavePos::staveHeight() * 3);
        m_titleHeight = 60;
    }
    maxSoreHeight = heightAboveStave + heightBelowStave + staveGap + m_titleHeight;
    int sizeX = qMin(width, maxSoreWidth);
    int sizeY = qMin(height, maxSoreHeight);
    int x = (width - sizeX)/2;
    int y = (height - sizeY)/2;
    y = (height - sizeY) - 5;
    x = 0;

    glViewport (x, y, sizeX, sizeY);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, sizeX, 0, sizeY, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    CStavePos::setStaveCenterY(sizeY - maxSoreHeight/2 - m_titleHeight/2);
    Cfg::setAppDimentions(x, y, sizeX, sizeY);
    Cfg::setStaveEndX(sizeX - staveEndGap);
    CStavePos::setStaveCentralOffset(staveGap/2);
    CDraw::forceCompileRedraw();
}

void CGLView::mousePressEvent(QMouseEvent *event)
{
}

void CGLView::mouseMoveEvent(QMouseEvent *event)
{
}

void CGLView::initializeGL()
{
    CColour colour = Cfg::backgroundColour();
    glClearColor (colour.red, colour.green, colour.blue, 0.0);
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glShadeModel (GL_FLAT);
    //glEnable(GL_TEXTURE_2D);                        // Enable Texture Mapping


    //from initCheck();
    glShadeModel(GL_FLAT);
    //glEnable(GL_DEPTH_TEST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //glGenTextures(1, &texName);
    //glBindTexture(GL_TEXTURE_2D, texName);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //enableAntialiasedLines();

    m_timeSigFont =  QFont("Arial", 22 );
    m_timeRatingFont =  QFont("Arial", 12 );

    Cfg::setStaveEndX(400);        //This value get changed by the resizeGL func

    m_song->setActiveHand(PB_PART_both);

    if (!Cfg::quickStart)
    {
        renderText(10,10,QString("~"), m_timeRatingFont); //fixme this is a work around for a QT bug.
        renderText(10,10,QString("~"), m_timeSigFont); //this is a work around for a QT bug.
    }

    setFocusPolicy(Qt::ClickFocus);
    m_qtWindow->init();

    m_score->init();

    m_song->regenerateChordQueue();


    // increased the tick time for Midi handling

    m_timer.start(Cfg::tickRate, this );

    m_realtime.start();

    //startMediaTimer(12, this );
}

void CGLView::updateMidiTask()
{
    int ticks;
    ticks = m_realtime.restart();
    m_displayUpdateTicks += ticks;
    m_eventBits |= m_song->task(ticks);
}

void CGLView::timerEvent(QTimerEvent *event)
{
    BENCHMARK(0, "timer enter");
    if (event->timerId() != m_timer.timerId())
    {
         QWidget::timerEvent(event);
         return;
    }


    updateMidiTask();
    BENCHMARK(1, "m_song task");

    if (m_displayUpdateTicks < SCREEN_FRAME_RATE)
        return;

    m_displayUpdateTicks = 0;

    if (m_eventBits != 0)
    {
        if ((m_eventBits & EVENT_BITS_UptoBarReached) != 0)
            m_song->playFromStartBar();
        if ((m_eventBits & EVENT_BITS_forceFullRedraw) != 0)
            m_forcefullRedraw = m_forceRatingRedraw = m_forceBarRedraw = REDRAW_COUNT;
        if ((m_eventBits & EVENT_BITS_forceRatingRedraw) != 0)
            m_forceRatingRedraw = REDRAW_COUNT;
        if ((m_eventBits & EVENT_BITS_newBarNumber) != 0)
            m_forcefullRedraw = m_forceRatingRedraw = m_forceBarRedraw = REDRAW_COUNT; // fixme this did not work so redraw everything

        m_qtWindow->songEventUpdated(m_eventBits);
        m_eventBits = 0;
    }

    if( m_cfg_openGlOptimise == 0 ) // zero is no GlOptimise
        m_fullRedrawFlag = true;
    else
        m_fullRedrawFlag = false;

    glDraw();
    //update();
    m_fullRedrawFlag = true;
    BENCHMARK(19, "timer exit");
}

void CGLView::mediaTimerEvent(int ticks)
{
}
