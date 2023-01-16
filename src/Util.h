/*********************************************************************************/
/*!
@file           Util.h

@brief          xxxx.

@author         L. J. Barman

    Copyright (c)   2008-2020, L. J. Barman and others, all rights reserved

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

#include <cassert>
#include <string>
#include <limits>

#include <QString>
#include <QApplication>

using namespace std;

#define MAX_MIDI_CHANNELS       16      // There are always at most 16 midi channels
#define MIDDLE_C                60
#define MIDI_OCTAVE             12
#define MIDI_BOTTOM_C           (MIDDLE_C - MIDI_OCTAVE*2)
#define MIDI_TOP_C              (MIDDLE_C + MIDI_OCTAVE*2)
#define MIDI_DRUM_CHANNEL       (10-1)

#define MAX_MIDI_NOTES          128

#define MAX_MIDI_TRACKS         32      // This will allow us to map midi track on to midi channels
typedef unsigned char byte_t;

template <typename As, typename T, std::size_t N>
[[nodiscard]] constexpr As arraySizeAs(const T (&)[N]) noexcept
{
    static_assert(N >= 0 && N <= std::numeric_limits<As>::max());
    return static_cast<As>(N);
}

template <typename T, std::size_t N>
[[nodiscard]] constexpr int arraySize(const T (&a)[N]) noexcept
{
    return arraySizeAs<int>(a);
}

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
void closeLogs();

constexpr int SPEED_ADJUST_FACTOR = 1000;
constexpr int deltaAdjust (int delta) { return delta / SPEED_ADJUST_FACTOR; }
constexpr qint64 deltaAdjustL (qint64 delta) { return delta / SPEED_ADJUST_FACTOR; }
constexpr float deltaAdjustF (qint64 delta) { return static_cast<float>(delta) / static_cast<float>(SPEED_ADJUST_FACTOR); }

void benchMarkInit();
void benchMark(unsigned int id, const QString &message);
void benchMarkResults();

class Util {
public:
    static QString dataDir();
};

#endif //__UTIL_H__
