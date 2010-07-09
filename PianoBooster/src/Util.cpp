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

static  FILE * logInfoFile = 0;
static  FILE * logErrorFile = 0;

static bool logsOpened = false;

static void openLogFile() {
    if (logsOpened == true)
        return;

    if (Cfg::useLogFile)
    {
        logInfoFile = fopen ("pb.log","w");
        logErrorFile = logInfoFile;
        if (logErrorFile == 0)
        {
            fputs("FATAL: cannot open the logfile", stderr);
            exit(EXIT_FAILURE);
        }
        else
        {
            logsOpened = true;
        }
    }
    else
    {
        logInfoFile = stdout;
        logErrorFile = stderr;
    }
}


static void flushLogs()
{
    if (logInfoFile != stdout && logsOpened)
    {
        fflush(logInfoFile);
        // logErrorFile is the same as logInfoFile
    }
}

void closeLogs()
{
    if (logInfoFile != stdout)
    {
        fclose(logInfoFile);
        logInfoFile = stdout;
        logErrorFile = stdout;
    }
}

/* prints an error message to stderr, and dies */
void fatal(const char *msg, ...)
{
    va_list ap;
    openLogFile();
    fputs("FATAL: ", logErrorFile);
    va_start(ap, msg);
    vfprintf(logErrorFile, msg, ap);
    va_end(ap);
    fputc('\n', logErrorFile);
    exit(EXIT_FAILURE);
}

void ppLog(logLevel_t level, const char *msg, ...)
{
    va_list ap;
    if (Cfg::logLevel  < level)
        return;

    openLogFile();
    va_start(ap, msg);
    vfprintf(logInfoFile, msg, ap);
    va_end(ap);
    fputc('\n', logInfoFile);
    flushLogs();
}

void ppLogInfo(const char *msg, ...)
{
    va_list ap;

    //fixme should call ppLog
    if (Cfg::logLevel  <  1)
        return;

    openLogFile();
    fputs("Info: ", logInfoFile);

    va_start(ap, msg);
    vfprintf(logInfoFile, msg, ap);
    va_end(ap);
    fputc('\n', logInfoFile);
    flushLogs();
}

void ppLogWarn(const char *msg, ...)
{
    va_list ap;

    if (Cfg::logLevel  <  2)
        return;

    openLogFile();
    fputs("Warn: ", logInfoFile);

    va_start(ap, msg);
    vfprintf(logInfoFile, msg, ap);
    va_end(ap);
    fputc('\n', logInfoFile);
    flushLogs();
}

void ppLogTrace(const char *msg, ...)
{
    va_list ap;

    openLogFile();
    fputs("Trace: ", logInfoFile);

    va_start(ap, msg);
    vfprintf(logInfoFile, msg, ap);
    va_end(ap);
    fputc('\n', logInfoFile);
    flushLogs();
}


void ppLogDebug( const char *msg, ...)
{
    va_list ap;

    if (Cfg::logLevel  <  2)
        return;

    openLogFile();
    fputs("Debug: ", logInfoFile);
    va_start(ap, msg);
    vfprintf(logInfoFile, msg, ap);
    va_end(ap);
    fputc('\n', logInfoFile);
    flushLogs();
}

void ppLogError(const char *msg, ...)
{
    va_list ap;

    openLogFile();
    fputs("ERROR: ", logErrorFile);
    va_start(ap, msg);
    vfprintf(logErrorFile, msg, ap);
    va_end(ap);
    fputc('\n', logErrorFile);
    flushLogs();
}

void ppTiming(const char *msg, ...)
{
    va_list ap;

    openLogFile();
    va_start(ap, msg);
    fprintf(logInfoFile, "T %4d " , s_realtime.restart() );
    vfprintf(logInfoFile, msg, ap);
    va_end(ap);
    fputc('\n', logInfoFile);
    flushLogs();
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

    openLogFile();
    if (i>=0)
        fprintf(logInfoFile, "Bench%2d: ", i);
    else
        fputs("Bench  : ", logInfoFile);
    fprintf(logInfoFile, "ct %4d, min %2d, avg %4.3f, max %2d frame %4.3f %s\n",  pBench->deltaCount,  pBench->minDelta,
                    static_cast<double>(pBench->deltaTotal)/pBench->deltaCount,
                    pBench->maxDelta,
                    (static_cast<double>(pBench->frameRateCurrent - pBench->frameRatePrevious))/pBench->deltaCount,
                    qPrintable(pBench->msg));
    benchMarkReset(pBench);
    flushLogs();
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
