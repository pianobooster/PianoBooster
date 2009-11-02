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
    m_midiTicks = 0;
    m_scrollTicks = 0;
    m_cfg_openGlOptimise = true;
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
    return QSize(50, 50);
}

QSize CGLView::sizeHint() const
{
    return QSize(200, 400); //fixme this does not work
}

void CGLView::initializeGL()
{
    qglClearColor(m_backgroundColour.dark());
}

void CGLView::paintGL()
{
	BENCHMARK(2, "enter");

    if (m_fullRedrawFlag)
        m_forcefullRedraw = m_forceRatingRedraw = m_forceBarRedraw = REDRAW_COUNT;

    if (m_forcefullRedraw) // clear the screen only if we are doing a full redraw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
	BENCHMARK(8, "glLoadIdentity");
	
    drawDisplayText();
	BENCHMARK(9, "drawDisplayText");

    drawAccurracyBar();
	BENCHMARK(10, "drawAccurracyBar");

    drawBarNumber();
	BENCHMARK(3, "drawBarNumber");

    m_score->drawScore();
	BENCHMARK(4, "drawScore");
    m_score->drawScroll(m_forcefullRedraw);
	//BENCHMARK(5, "drawScroll");

    drawTimeSignature();

    m_scrollTicks = 0;
    m_midiTicks += m_realtime.restart();

    if (m_forcefullRedraw) m_forcefullRedraw--;
	BENCHMARK(7, "exit");
    BENCHMARK_RESULTS();
}

void CGLView::drawTimeSignature()
{
    if (Cfg::quickStart)
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
    if (m_song->getPlayMode() == PB_PLAY_MODE_listen)
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

    glColor3f(1.0,1.0,1.0);
    int y = Cfg::getAppHeight() - 14;

    if (m_song->getPlayMode() != PB_PLAY_MODE_listen)
        renderText(30, y-4,0 ,"Accuracy:", m_timeRatingFont);

    if (m_titleHeight < 45 )
        return;

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
    renderText(x, y, 0,"Bar: " + QString::number(m_song->getBarNumber()), m_timeRatingFont);
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
    CDraw::forceCompileRedraw();

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
    {
        renderText(10,10,"~", m_timeRatingFont); //fixme this is a work around for a QT bug.
        renderText(10,10,"~", m_timeSigFont); //this is a work around for a QT bug.
    }

    setFocusPolicy(Qt::ClickFocus);

    // todo increase the tick time for Midi handling
    m_timer.start(12, this ); // fixme was 12 was 5
    m_realtime.start();
    
    fastUpdateRate(true);

    //startMediaTimer(12, this );
}

static bool s_fullSpeed;
static int s_holdOff;
void CGLView::fastUpdateRate(bool fullSpeed)
{
	s_fullSpeed = fullSpeed;
}

void CGLView::timerEvent(QTimerEvent *event)
{
	if ( s_fullSpeed == false)
	{
		s_holdOff++;
		if ( s_holdOff < 4)
			return;
		s_holdOff = 0;
	}
	
	BENCHMARK(0, "timer enter");
    eventBits_t eventBits;
    if (event->timerId() == m_timer.timerId())
    {
        int ticks;
        ticks = m_realtime.restart();
        m_midiTicks += ticks;
        m_scrollTicks += ticks;
        eventBits = m_song->task(m_midiTicks);
        m_midiTicks = 0;

        if (eventBits != 0)
        {
            if ((eventBits & EVENT_BITS_UptoBarReached) != 0)
                m_song->playFromStartBar();
            if ((eventBits & EVENT_BITS_forceFullRedraw) != 0)
                m_forcefullRedraw = m_forceRatingRedraw = m_forceBarRedraw = REDRAW_COUNT;
            if ((eventBits & EVENT_BITS_forceRatingRedraw) != 0)
                m_forceRatingRedraw = REDRAW_COUNT;
            if ((eventBits & EVENT_BITS_newBarNumber) != 0)
                m_forcefullRedraw = m_forceRatingRedraw = m_forceBarRedraw = REDRAW_COUNT; // fixme this did not work so redraw everything

            m_qtWindow->songEventUpdated(eventBits);
        }
        if(m_cfg_openGlOptimise)
            m_fullRedrawFlag = false;
        else
            m_fullRedrawFlag = true;

        // if m_fullRedrawFlag is true it will redraw the entire GL window
        //if (m_scrollTicks>= 12)
         glDraw();
		//update();
		m_fullRedrawFlag = true;
    }
    else
    {
        QWidget::timerEvent(event);
    }
    BENCHMARK(1, "timer exit");
}

void CGLView::mediaTimerEvent(int ticks)
{
}
