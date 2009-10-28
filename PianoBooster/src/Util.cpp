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

//////////////////////   BENCH MARK //////////


static QTime s_benchMarkTime;
//static int frameTime;
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
	
} benchData_t;

benchData_t s_benchData[10];
benchData_t s_frameRate;

void benchMarkReset(benchData_t *pBench)
{
	pBench->deltaTotal = 0;
	pBench->deltaCount = 0;
	pBench->maxDelta = 0;
	pBench->minDelta = 9999999;
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

void printResult(benchData_t *pBench)
{
	if (pBench->deltaCount == 0)
		return;
	fputs("Bench: ", stdout);
	fprintf(stdout, "min %2d, avg %2d, max %2d %s\n",   pBench->minDelta,
													pBench->deltaTotal/pBench->deltaCount,
													pBench->maxDelta,
													qPrintable(pBench->msg));
	benchMarkReset(pBench);
}

void benchMarkResults()
{
	s_previousFrameTime = benchMarkUpdate(&s_frameRate, s_previousFrameTime);
	
	static int s_counter;
	s_counter++;
	if (s_counter < 50)
		return;
	s_counter = 0;
	for (unsigned int i=0; i <  arraySize( s_benchData ); i++) 
	{
		printResult(&s_benchData[i]);
	}
	printResult(&s_frameRate);
}
