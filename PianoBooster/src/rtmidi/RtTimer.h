/**********************************************************************/
/*! \class RtTimer
    \brief ToDo
*/
/**********************************************************************/

// RtMidi: Version 1.0.7

#ifndef RTTIMER_H
#define RTTIMER_H

#include "RtMidi.h"
#include "RtError.h"
#include <string>

class QtTimerHandler;
class RtTimer
{
public:
    RtTimer();

    virtual ~RtTimer();        //fixme

    // object will receive the timer events
    void startMediaTimer(int msec, RtTimer *object);

public:
    virtual void mediaTimerEvent(int deltaTime)=0; // make pure to force derived class to have one of these
private:

    QtTimerHandler* m_qtTimerHandler;  // The timer using the QBasicTimer for now
    RtTimer *m_userObject;   // saved copy of the object that will recieve the timer events

};

#endif // RTTIMER_H

