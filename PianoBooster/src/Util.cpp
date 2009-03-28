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

    va_start(ap, msg);
    vfprintf(stdout, msg, ap);
    va_end(ap);
    fputc('\n', stdout);
}

void ppTrace(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    vfprintf(stdout, msg, ap);
    va_end(ap);
    fputc('\n', stdout);
}


void ppDebug( const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    vfprintf(stdout, msg, ap);
    va_end(ap);
    fputc('\n', stdout);
}

void ppError(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    fputs("Error: ", stdout);
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
