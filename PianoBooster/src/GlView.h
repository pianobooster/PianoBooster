/*********************************************************************************/
/*!
@file           GlView.h

@brief          xxx

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
#ifndef __GLVIEW_H__
#define __GLVIEW_H__
#include <QTime>
#include <QBasicTimer>
#include <QGLWidget>
#include "Song.h"
#include "Score.h"
#include "Settings.h"
//#include "rtmidi/RtTimer.h"

class Window;

class CGLView : public QGLWidget//, RtTimer
{
    Q_OBJECT

public:
    CGLView(QtWindow *parent, CSettings* settings);
    ~CGLView();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    CSong* getSongObject() {return m_song;}
    CScore* getScoreObject() {return m_score;}
    void init();
    bool m_cfg_openGlOptimise;

protected:
    void timerEvent(QTimerEvent *event);
    void mediaTimerEvent(int ticks);

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void drawDisplayText();
    void drawTimeSignature();
    void drawAccurracyBar();
    void drawBarNumber();
    void updateMidiTask();


    QColor m_backgroundColour;
    QtWindow* m_qtWindow;  // The parent Window
    CSettings* m_settings;
    CSong* m_song;
    CScore* m_score;
    QBasicTimer m_timer;
    QTime m_realtime;
    int m_displayUpdateTicks;
    CRating* m_rating;
    QFont m_timeSigFont;
    QFont m_timeRatingFont;
    bool m_fullRedrawFlag;
    int m_forcefullRedraw;
    int m_forceRatingRedraw;
    int m_forceBarRedraw;
    int m_titleHeight;
    eventBits_t m_eventBits;
};

#endif // __GLVIEW_H__
