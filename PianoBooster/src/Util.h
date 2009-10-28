/*********************************************************************************/
/*!
@file           Util.h

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

#ifndef __UTIL_H__
#define __UTIL_H__

#include <assert.h>
#include <string>
#include <QString>

using namespace std;

#define MAX_MIDI_CHANNELS   16
#define MIDDLE_C            60
#define MIDI_OCTAVE         12
#define MIDI_BOTTOM_C       (MIDDLE_C - MIDI_OCTAVE*2)
#define MIDI_TOP_C          (MIDDLE_C + MIDI_OCTAVE*2)

#define MAX_MIDI_NOTES      128

typedef unsigned char byte;

#define arraySize(a) (sizeof(a)/sizeof(a[0]))     /* Returns (at compile time) the number of elements in an array */

#define ppDEBUG(args)     ppLogDebug args



typedef enum
{
    PB_LOG_error,
    PB_LOG_warn,
    PB_LOG_info,
    PB_LOG_verbose,
} logLevel_t;

void fatal(const char *msg, ...);
void ppLogTrace(const char *msg, ...);
void ppLogDebug(const char *msg, ...);
void ppLog(logLevel_t level, const char *msg, ...);
void ppLogInfo(const char *msg, ...);
void ppLogWarn(const char *msg, ...);
void ppLogError(const char *msg, ...);
void ppTiming(const char *msg, ...);

#define SPEED_ADJUST_FACTOR     1000
#define deltaAdjust(delta) ((delta)/SPEED_ADJUST_FACTOR )

void benchMarkInit();
void benchMark(unsigned int id, QString message);
void benchMarkResults();


#endif //__UTIL_H__
