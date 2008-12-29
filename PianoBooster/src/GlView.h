/*********************************************************************************/
/*!
@file           GlView.h

@brief          xxx

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
#ifndef __GLVIEW_H__
#define __GLVIEW_H__
#include <QTime>
#include <QBasicTimer>
#include <QGLWidget>
#include "Song.h"
#include "Score.h"
//#include "rtmidi/RtTimer.h"

class Window;

class CGLView : public QGLWidget//, RtTimer
{
    Q_OBJECT

public:
    CGLView(Window *parent);
    ~CGLView();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    CSong* getSongObject() {return m_song;}
    CScore* getScoreObject() {return m_score;}
    void init();

protected:
    void timerEvent(QTimerEvent *event);
    void mediaTimerEvent(int deltaTime);

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void drawDisplayText();
    void drawTimeSignature();
    void drawAccurracyBar();

    QColor m_backgroundColour;
    Window* m_qtWindow;  // The parent Window
    CSong* m_song;
    CScore* m_score;
    QBasicTimer m_timer;
    QTime m_realtime;
    int m_midiTicks;
    int m_scrollTicks;
    CRating* m_rating;
    QFont m_timeSigFont;
    QFont m_timeRatingFont;
    bool m_fullRedrawFlag;
    int m_titleHeight;
};

#endif // __GLVIEW_H__
