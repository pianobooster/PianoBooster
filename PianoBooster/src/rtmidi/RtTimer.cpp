/**********************************************************************/
/*! \class RtMidiTimer
    \brief ToDo
*/
/**********************************************************************/

// RtMidi: Version 1.0.7

#include "RtTimer.h"
#include <QBasicTimer>
#include <QTime>
#include <QTimerEvent>
#include <QObject>

class QtTimerHandler : public QObject
{
public:
    QtTimerHandler ()  {
        m_userTimerObject = 0;
    }

    ~QtTimerHandler () {
    }

    void timerEvent(QTimerEvent *event);
    void startTimer(int msec, RtTimer * userObject)
    {
        m_userTimerObject = userObject;
        m_time.restart();
        m_timer.start(msec, this);
    }

private:
    QBasicTimer m_timer;
    QTime m_time;
    RtTimer * m_userTimerObject;
};

void QtTimerHandler::timerEvent(QTimerEvent *event)
{
    int ticks;
    if (event->timerId() == m_timer.timerId())
    {
//printf("xxx\n");
        ticks = m_time.restart();
        if (m_userTimerObject)
            m_userTimerObject->mediaTimerEvent(ticks);
    }
    else
        QObject::timerEvent(event);
}

RtTimer::RtTimer()
{
    m_qtTimerHandler = 0;
    m_userObject = 0;
}


RtTimer::~RtTimer()
{
    delete m_qtTimerHandler;
}

void RtTimer::startMediaTimer(int msec, RtTimer *object)
{
    if (m_qtTimerHandler == 0)
        m_qtTimerHandler = new QtTimerHandler();
    m_userObject = object; // Save the user object
    m_qtTimerHandler->startTimer(msec, object);
}


