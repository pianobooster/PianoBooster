
#ifndef __GLVIEW_H__
#define __GLVIEW_H__
#include <QTime>
#include <QBasicTimer>
#include <QGLWidget>
#include "Song.h"
#include "Score.h"
#include "rtmidi/RtTimer.h"

class Window;

class CGLView : public QGLWidget, RtTimer
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
    CRating* m_rating;
    QFont m_timeSigFont;
    QFont m_timeRatingFont;
    bool m_fullRedrawFlag;
    int m_titleHeight;
};

#endif // __GLVIEW_H__
