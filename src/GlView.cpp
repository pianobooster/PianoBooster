/*********************************************************************************/
/*!
@file           GlView.cpp

@brief          xxxx

@author         L. J. Barman

    Copyright (c)   2008-2013, L. J. Barman, all rights reserved

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

#include <QtWidgets>
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

#define TEXT_LEFT_MARGIN 30

CGLView::CGLView(QtWindow* parent, CSettings* settings)
    : QGLWidget(parent)
{
    m_qtWindow = parent;
    m_settings = settings;
    m_rating = nullptr;
    m_fullRedrawFlag = true;
    m_forcefullRedraw = 0;
    m_forceRatingRedraw = 0;
    m_forceBarRedraw = 0;
    m_allowedTimerEvent = true;

    m_backgroundColor = QColor(0, 0, 0);

    m_song = new CSong();
    m_score = new CScore(m_settings);
    m_displayUpdateTicks = 0;
    m_cfg_openGlOptimise = 0; // zero is no GlOptimise
    m_eventBits = 0;
    BENCHMARK_INIT();
}

CGLView::~CGLView()
{
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

void CGLView::stopTimerEvent()
{
    m_allowedTimerEvent=false;
}

void CGLView::startTimerEvent()
{
    m_allowedTimerEvent=true;
}

void CGLView::paintGL()
{
    BENCHMARK(2, "enter");

    m_displayUpdateTicks = 0;

    if (m_fullRedrawFlag)
        m_forcefullRedraw = m_forceRatingRedraw = m_forceBarRedraw = REDRAW_COUNT;

    if (m_forcefullRedraw) // clear the screen only if we are doing a full redraw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
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
    if (m_forcefullRedraw == 0)
        return;

    float x,y;
    int topNumber, bottomNumber;

    if (m_song == nullptr) return;

    m_song->getTimeSig(&topNumber, &bottomNumber);
    if (topNumber == 0 ) return;

    char bufferTop[10], bufferBottom[10];
    sprintf(bufferTop, "%d", topNumber);
    sprintf(bufferBottom, "%d", bottomNumber);

    x = Cfg::timeSignatureX();

    CDraw::drColor ((CDraw::getDisplayHand() != PB_PART_left) ? Cfg::noteColor() : Cfg::noteColorDim());

    y = CStavePos(PB_PART_right,  0).getPosY() + 5;
    renderText(x,y, 0, bufferTop, m_timeSigFont);
    y = CStavePos(PB_PART_right, -3).getPosY() - 2;
    renderText(x,y, 0, bufferBottom, m_timeSigFont);

    CDraw::drColor ((CDraw::getDisplayHand() != PB_PART_right) ? Cfg::noteColor() : Cfg::noteColorDim());

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
    CColor color;

    float y = Cfg::getAppHeight() - 14;
    const float x = accuracyBarStart;
    const int width = 360;
    const int lineWidth = 8/2;

    m_rating->calculateAccuracy();

    accuracy = m_rating->getAccuracyValue();
    color = m_rating->getAccuracyColor();
    CDraw::drColor (color);
    glRectf(x, y - lineWidth, x + width * accuracy, y + lineWidth);
    CDraw::drColor (Cfg::backgroundColor());
    glRectf(x + width * accuracy, y - lineWidth, x + width, y + lineWidth);

    glLineWidth (1);
    CDraw::drColor (CColor(1.0, 1.0, 1.0));
    glBegin(GL_LINE_LOOP);
    glVertex2f (x, y + lineWidth);
    glVertex2f (x+ width, y  + lineWidth);
    glVertex2f (x + width, y - lineWidth);
    glVertex2f (x, y - lineWidth);
    glEnd();
}

void CGLView::drawDisplayText()
{
    if (m_rating == nullptr)
    {
        m_rating = m_song->getRating();
        return; // don't run this func the first time it is called
    }

    if (m_forcefullRedraw == 0)
        return;

    int y = Cfg::getAppHeight() - 14;

    if (!m_settings->getWarningMessage().isEmpty())
    {
        glColor3f(1.0,0.2,0.0);
        renderText(TEXT_LEFT_MARGIN, y-4, 0, m_settings->getWarningMessage(), m_timeRatingFont);
        return;
    }

    glColor3f(1.0,1.0,1.0);

    if (m_song->getPlayMode() != PB_PLAY_MODE_listen) {
        if (accuracyBarStart == 0) {
            QFontMetrics fm(m_timeRatingFont);
            accuracyText = tr("Accuracy:");
            accuracyBarStart=fm.boundingRect(accuracyText + "  ").right() + TEXT_LEFT_MARGIN;
       }

        renderText(TEXT_LEFT_MARGIN, y-4,0 ,accuracyText, m_timeRatingFont);
    }

    if (m_titleHeight < 45 )
        return;

    y = Cfg::getAppHeight() - m_titleHeight;

    renderText(TEXT_LEFT_MARGIN, y+6, 0,tr("Song:") + " " + m_song->getSongTitle(), m_timeRatingFont);
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
    if (m_forceBarRedraw == 0)
        return;
    m_forceBarRedraw--;

    float y = Cfg::getAppHeight() - m_titleHeight - 34;
    float x = TEXT_LEFT_MARGIN;

    //CDraw::drColor (Cfg::backgroundColor());
    //CDraw::drColor (Cfg::noteColorDim());
    //glRectf(x+30+10, y-2, x + 80, y + 16);
    glColor3f(1.0,1.0,1.0);
    renderText(x, y, 0, tr("Bar:") + " " + QString::number(m_song->getBarNumber()), m_timeRatingFont);
}

void CGLView::resizeGL(int width, int height)
{
    recalculateGeometry(width, height);
}

void CGLView::recalculateGeometry(int width, int height) {
    float sf = m_settings->scalingFactor();
    CStavePos::setScalingFactor(sf);
    Cfg::setScalingFactor(sf);
    m_score->recalculateGeometry();
    m_score->refreshFontSize();

    const int maxSoreWidth = static_cast<int>(1024 * m_settings->scalingFactor());
    const int staveEndGap = 20;
    const int heightAboveStave =  static_cast<int>(CStavePos::verticalNoteSpacing() * MAX_STAVE_INDEX);
    const int heightBelowStave =  static_cast<int>(CStavePos::verticalNoteSpacing() * - MIN_STAVE_INDEX);
    const int minTitleHeight = 20;
    const int minStaveGap = 120;
    int staveGap;
    int maxSoreHeight;

    //int space = height - (heightAboveStave + heightBelowStave + minTitleHeight + minStaveGap);
    //m_titleHeight = qBound(minTitleHeight, minTitleHeight + space/2, 70);
    // staveGap = qBound(minStaveGap, minStaveGap+ space/2, static_cast<int>(CStavePos::staveHeight() * 3));
    if (height < 430)  // So it works on an eeepc 701 (for Trev)
    {
        staveGap = minStaveGap;
        m_titleHeight = minTitleHeight;
    }
    else
    {
        staveGap = static_cast<int>(CStavePos::staveHeight() * 3 * (1/sf));
        m_titleHeight = 60;
    }
    maxSoreHeight = heightAboveStave + heightBelowStave + staveGap + m_titleHeight;
    if (height > maxSoreHeight) {
        int scoreExtraTopGap = (height - maxSoreHeight);
        maxSoreHeight += qMin(200, scoreExtraTopGap);
    }
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
    refreshFontSize();
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
    CColor color = Cfg::backgroundColor();
    glClearColor (color.red, color.green, color.blue, 0.0);
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glShadeModel (GL_FLAT);
    //glEnable(GL_TEXTURE_2D);                        // Enable Texture Mapping

    //from initCheck();
    glShadeModel(GL_FLAT);
    //glEnable(GL_DEPTH_TEST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //glGenTextures(1, &texName);
    //glBindTexture(GL_TEXTURE_2D, texName);

    if (format().samples() > 0) {
        glEnable(GL_MULTISAMPLE);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //enableAntialiasedLines();

    m_timeSigFont = QFont("Arial");
    m_timeRatingFont = QFont("Arial");
    refreshFontSize();

    Cfg::setStaveEndX(400);        //This value get changed by the resizeGL func

    m_song->setActiveHand(PB_PART_both);

    setFocusPolicy(Qt::ClickFocus);
    m_qtWindow->init();

    m_score->init();

    m_song->regenerateChordQueue();

    // increased the tick time for MIDI handling

    m_timer.start(Cfg::tickRate, this );

    m_realtime.start();

    //startMediaTimer(12, this );
}

void CGLView::refreshFontSize()
{
    // This is a work around for Windows different display scaling option
    int widgetPointSize = m_qtWindow->font().pointSize();
    m_timeSigFont.setPointSize(static_cast<int>(widgetPointSize * 2 * m_settings->scalingFactor()));
    m_timeRatingFont.setPointSize(static_cast<int>(widgetPointSize * 1.2 * m_settings->scalingFactor()));
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
    if (!m_allowedTimerEvent) return;

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
