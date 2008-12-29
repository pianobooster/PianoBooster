/*********************************************************************************/
/*!
@file           GlView.cpp

@brief          xxxx

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
/*********************************************************************************/

#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "QtWindow.h"
#include "GlView.h"
#include "Cfg.h"
#include "Draw.h"

int nextListId; //FIXME

CGLView::CGLView(Window *parent)
    : QGLWidget(parent)
{
    m_qtWindow = parent;
    m_rating = 0;
    m_fullRedrawFlag = true;

    m_backgroundColour = QColor(0, 0, 0);

    m_song = new CSong();
    m_score = new CScore();
    m_midiTicks = 0;
    m_scrollTicks = 0;
}

CGLView::~CGLView()
{
    makeCurrent();
    delete m_song;
    delete m_score;
    m_titleHeight = 0;
    if (nextListId) { glDeleteLists(nextListId,1); nextListId = 0;}

}

QSize CGLView::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize CGLView::sizeHint() const
{
    return QSize(200, 400); //Fixme this does not work
}

void CGLView::initializeGL()
{
    qglClearColor(m_backgroundColour.dark());
}

void CGLView::paintGL()
{
    if (m_fullRedrawFlag) // clear the screen only if we are doing a full redraw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (m_fullRedrawFlag)
        drawDisplayText();

    m_score->drawScore(m_fullRedrawFlag);
    if (m_fullRedrawFlag)
        drawTimeSignature();

    m_scrollTicks = 0;
    m_midiTicks += m_realtime.restart();
}

void CGLView::drawTimeSignature()
{
    if (Cfg::quickStart)
        return;

    float x,y;
    int topNumber, bottomNumber;

    if (m_song == 0) return;

    m_song->getCurrentTimeSig(&topNumber, &bottomNumber);
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
    if (m_song->getPlayMode() == PB_PLAY_MODE_listen)
        return;

    float y;
    float accuracy;
    CColour colour;

    y = Cfg::getAppHeight() - 14;
    const float x = 120;
    const int width = 360;
    const int lineWidth = 8/2;

    if (!Cfg::quickStart)
    {
        glColor3f(1.0,1.0,1.0);
        renderText(30, y-4,0 ,"Accuracy:", m_timeRatingFont);
    }

    m_rating->getAccuracy(&colour, &accuracy);
    CDraw::drColour (colour);
    glRectf(x, y - lineWidth, x + width * accuracy, y + lineWidth);

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

    drawAccurracyBar();

    if (m_titleHeight < 45 )
        return;
    float y;
    glColor3f(1.0,1.0,1.0);
    y = Cfg::getAppHeight() - m_titleHeight;
    renderText(30, y+6, 0,"Song: " + m_song->getSongTitle(), m_timeRatingFont);
    /*
    char buffer[100];
    sprintf(buffer, "Notes %d wrong %d Late %d Score %4.1f%%",
    m_rating->totalNoteCount(), m_rating->wrongNoteCount(),
    m_rating->lateNoteCount(), m_rating->rating());
    renderText(10,20, buffer, m_timeRatingFont);
    */
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
    if (Cfg::smallScreen)
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
}

void CGLView::mousePressEvent(QMouseEvent *event)
{
}

void CGLView::mouseMoveEvent(QMouseEvent *event)
{
}

void CGLView::init()
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
        renderText(10,10,"x",m_timeRatingFont); //FIXME
/*
    nextListId = glGenLists (1);

    glNewList (nextListId, GL_COMPILE);
    //renderText(10.0,10.0,0.0,"xxxxx");
    //renderText(10,10,"xxxxx");
    glEndList ();
// */

    setFocusPolicy(Qt::ClickFocus);

    // todo increase the tick time for Midi handling
    m_timer.start(12, this ); // was 5
    m_realtime.start();

    //startMediaTimer(12, this );
}

void CGLView::timerEvent(QTimerEvent *event)
{
    int eventBits;
    if (event->timerId() == m_timer.timerId())
    {
        int ticks;
        ticks = m_realtime.restart();
        m_midiTicks += ticks;
        m_scrollTicks += ticks;
        eventBits = m_song->task(m_midiTicks);
        //if (m_midiTicks >= 20) ppTrace("m_midiTicks = %d", m_midiTicks); //fixme
        m_midiTicks = 0;

        m_fullRedrawFlag = false;
        if (eventBits != 0)
        {
            if ((eventBits & EVENT_BITS_forceFullRredraw) != 0)
                m_fullRedrawFlag = true;
            m_qtWindow->songEventUpdated(eventBits);
        }
#ifdef _WIN32
m_fullRedrawFlag = true; //fixme
#endif
        // if m_fullRedrawFlag is true it will redraw the entire GL window
        //if (m_scrollTicks>= 12)
            glDraw();
        m_fullRedrawFlag = true;
    }
    else
    {
        QWidget::timerEvent(event);
    }
}

void CGLView::mediaTimerEvent(int deltaTime)
{
/*
    int eventBits;
    eventBits = m_song->task(deltaTime);

    m_fullRedrawFlag = false;
    if (eventBits != 0)
    {
        if ((eventBits & EVENT_BITS_forceFullRredraw) != 0)
            m_fullRedrawFlag = true;
        m_qtWindow->songEventUpdated(eventBits);
    }
#ifdef _WIN32
m_fullRedrawFlag = true; //fixme
#endif
    // if m_fullRedrawFlag is true it will redraw the entire GL window
    glDraw();
    m_fullRedrawFlag = true;
*/
}
