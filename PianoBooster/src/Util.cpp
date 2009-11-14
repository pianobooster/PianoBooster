/*********************************************************************************/
/*!
@file           Util.cpp

@brief          xxxx.

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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fstream>
#include <sstream>
#include "Util.h"
#include "Cfg.h"
#include <QTime>



static QTime s_realtime;

/* prints an error message to stderr, and dies */
void fatal(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}

void ppLog(logLevel_t level, const char *msg, ...)
{
    va_list ap;
    if (Cfg::logLevel  < level)
        return;

    va_start(ap, msg);
    vfprintf(stdout, msg, ap);
    va_end(ap);
    fputc('\n', stdout);
}
void ppLogInfo(const char *msg, ...)
{
    va_list ap;

    //fixme should call ppLog
    if (Cfg::logLevel  <  1)
        return;

    fputs("Info: ", stdout);

    va_start(ap, msg);
    vfprintf(stdout, msg, ap);
    va_end(ap);
    fputc('\n', stdout);
}

void ppLogWarn(const char *msg, ...)
{
    va_list ap;

    if (Cfg::logLevel  <  2)
        return;

    fputs("Warn: ", stdout);

    va_start(ap, msg);
    vfprintf(stdout, msg, ap);
    va_end(ap);
    fputc('\n', stdout);
}

void ppLogTrace(const char *msg, ...)
{
    va_list ap;

    fputs("Trace: ", stdout);

    va_start(ap, msg);
    vfprintf(stdout, msg, ap);
    va_end(ap);
    fputc('\n', stdout);
}


void ppLogDebug( const char *msg, ...)
{
    va_list ap;

    fputs("Debug: ", stdout);
    va_start(ap, msg);
    vfprintf(stdout, msg, ap);
    va_end(ap);
    fputc('\n', stdout);
}

void ppLogError(const char *msg, ...)
{
    va_list ap;

    fputs("ERROR: ", stdout);
    va_start(ap, msg);
    vfprintf(stdout, msg, ap);
    va_end(ap);
    fputc('\n', stdout);
}

void ppTiming(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    fprintf(stdout, "T %4d " , s_realtime.restart() );
    vfprintf(stdout, msg, ap);
    va_end(ap);
    fputc('\n', stdout);
}

////////////////////// BENCH MARK //////////////////////


static QTime s_benchMarkTime;
static int s_previousTime;
static int s_previousFrameTime;

typedef struct
{
    int time;
    QString msg;
    int deltaTotal;
    int deltaCount;
    int maxDelta;
    int minDelta;
    int frameRatePrevious;
    int frameRateCurrent;

} benchData_t;

benchData_t s_benchData[20];
benchData_t s_frameRate;

void benchMarkReset(benchData_t *pBench)
{
    pBench->deltaTotal = 0;
    pBench->deltaCount = 0;
    pBench->maxDelta = 0;
    pBench->minDelta = 9999999;
    pBench->frameRatePrevious = pBench->frameRateCurrent;
}


void benchMarkInit()
{
    s_benchMarkTime.start();
    s_previousTime = 0;
    s_previousFrameTime = 0;

    for (unsigned int i=0; i <  arraySize( s_benchData ); i++)
        benchMarkReset(&s_benchData[i]);
    benchMarkReset(&s_frameRate);
    s_frameRate.msg = " *** Frame Rate ***";
}

int benchMarkUpdate(benchData_t *pBench,  int previousTime)
{
    int time = s_benchMarkTime.elapsed();
    int delta = time - previousTime;
    pBench->deltaTotal += delta;
    pBench->deltaCount++;
    pBench->frameRateCurrent = time;
    pBench->maxDelta = qMax(pBench->maxDelta, delta);
    pBench->minDelta = qMin(pBench->minDelta, delta);
    return time;
}

void benchMark(unsigned int id, QString message)
{
    if (id >= arraySize(s_benchData))
        return;
    if (s_benchData[id].msg.size() == 0 )
        s_benchData[id].msg = message;
    s_previousTime = benchMarkUpdate(&s_benchData[id], s_previousTime);

}

void printResult(int i, benchData_t *pBench)
{
    if (pBench->deltaCount == 0)
        return;
    if (i>=0)
        fprintf(stdout, "Bench%2d: ", i);
    else
        fputs("Bench  : ", stdout);
    fprintf(stdout, "ct %4d, min %2d, avg %4.3f, max %2d frame %4.3f %s\n",  pBench->deltaCount,  pBench->minDelta,
                    static_cast<double>(pBench->deltaTotal)/pBench->deltaCount,
                    pBench->maxDelta,
                    (static_cast<double>(pBench->frameRateCurrent - pBench->frameRatePrevious))/pBench->deltaCount,
                    qPrintable(pBench->msg));
    benchMarkReset(pBench);
}

void benchMarkResults()
{
    int ticks;
    ticks = s_benchMarkTime.elapsed();

    if ( (ticks - s_previousFrameTime) < 5000)
        return;
    s_previousFrameTime = ticks;
    for (unsigned int i=0; i <  arraySize( s_benchData ); i++)
    {
        printResult(i, &s_benchData[i]);
    }
}
