/*********************************************************************************/
/*!
@file           Util.h

@brief          xxxx.

@author         L. J. Barman

    Copyright (c)   2008, L. J. Barman, all rights reserved

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
using namespace std;

#define MAX_MIDI_CHANNELS   16
#define MIDDLE_C            60
#define MIDI_OCTAVE         12
#define MIDI_BOTTOM_C       (MIDDLE_C - MIDI_OCTAVE*2)
#define MIDI_TOP_C          (MIDDLE_C + MIDI_OCTAVE*2)

#define MAX_MIDI_NOTES      128

typedef unsigned char byte;

#define arraySize(a) (sizeof(a)/sizeof(a[0]))     /* Returns (at compile time) the number of elelement in an array */

#define ppDEBUG(args)     ppDebug args


class CColour
{
public:
    CColour() { red = green = blue = 0; }


    CColour(double r, double g, double b)
    {
        red = static_cast<float>(r);
        green = static_cast<float>(g);
        blue = static_cast<float>(b);
    }
    float red, green, blue;

    bool operator==(CColour colour)
    {
        if (red == colour.red && green == colour.green && blue == colour.blue)
            return true;
        return false;
    }
};

void fatal(const char *msg, ...);
void ppLog(const char *msg, ...);
void ppTrace(const char *msg, ...);
void ppDebug(const char *msg, ...);
void ppError(const char *msg, ...);

#ifndef HAS_SCORE // Fixme
#define HAS_SCORE  1
#endif

#define SPEED_ADJUST_FACTOR     1000
#define deltaAdjust(delta) ((delta)/SPEED_ADJUST_FACTOR )

class Cfg2
{
public:
    static const int playZoneEarly()     {return 25;} // 20 Was 25
    static const int playZoneLate()      {return 25;}
    static const int silenceTimeOut()    {return 8000;} // the time in msec before everyting goes quiet
    static const int chordNoteGap()      {return 10;} // all notes in a cord must be spaced less than this a gap

};

#endif //__UTIL_H__
