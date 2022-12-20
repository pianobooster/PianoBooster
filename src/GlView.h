/*********************************************************************************/
/*!
@file           GlView.h

@brief          xxx

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
#ifndef __GLVIEW_H__
#define __GLVIEW_H__
#include <QTime>
#include <QBasicTimer>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include "Song.h"
#include "Score.h"
#include "Settings.h"
//#include "rtmidi/RtTimer.h"

class Window;

class CGLView : public QOpenGLWidget//, RtTimer
{
    Q_OBJECT

public:
    CGLView(QtWindow *parent, CSettings* settings);
    ~CGLView();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    CSong* getSongObject() {return m_song;}
    CScore* getScoreObject() {return m_score;}
    int m_cfg_openGlOptimise;

    void stopTimerEvent();
    void startTimerEvent();

protected:
    void timerEvent(QTimerEvent *event);
    void mediaTimerEvent(int ticks);

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void renderText(double x, double y, double z, const QString &str, const QFont & font);

private:
    void drawDisplayText();
    void drawTimeSignature();
    void drawAccurracyBar();
    void drawBarNumber();
    void updateMidiTask();

    QString accuracyText;
    int accuracyBarStart = 0;

    QColor m_backgroundColor;
    QtWindow* m_qtWindow;  // The parent Window
    CSettings* m_settings;
    CSong* m_song;
    CScore* m_score;
    QBasicTimer m_timer;
    QElapsedTimer m_realtime;
    qint64 m_displayUpdateTicks;
    CRating* m_rating;
    QFont m_timeSigFont;
    QFont m_timeRatingFont;
    // TODO remove these as no longer required
    bool m_fullRedrawFlag;
    int m_forcefullRedraw;
    int m_forceRatingRedraw;
    int m_forceBarRedraw;
    int m_titleHeight;
    eventBits_t m_eventBits;
    bool m_allowedTimerEvent;
};

#endif // __GLVIEW_H__
