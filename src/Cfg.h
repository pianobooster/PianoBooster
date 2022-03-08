/*********************************************************************************/
/*!
@file           Cfg.h

@brief          Contains all the configuration Information.

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

#ifndef __CFG_H__
#define __CFG_H__

#define OPTION_BENCHMARK_TEST     0
#if OPTION_BENCHMARK_TEST
#define BENCHMARK_INIT()        benchMarkInit()
#define BENCHMARK(id,mesg)      benchMark(id,mesg)
#define BENCHMARK_RESULTS()     benchMarkResults()
#else
#define BENCHMARK_INIT()
#define BENCHMARK(id,mesg)
#define BENCHMARK_RESULTS()
#endif

#define LOG_LEVEL_NONE  0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_DEBUG 3

#include <cstddef>
#include <stdexcept>

#include "IColorPreference.h"

/*!
 * @brief   Contains all the configuration Information.
 */
class Cfg
{
public:
    static float staveStartX()         {return 20;}
    static float staveEndX()           {return m_staveEndX;}
    static float playZoneX()           {return scrollStartX() + ( staveEndX() - scrollStartX())* 0.4f;}
    static float clefX()               {return staveStartX() + 20;}
    static float timeSignatureX()       {return clefX() + 25;}
    static float keySignatureX()       {return timeSignatureX() + 25;}
    static float scrollStartX()        {return keySignatureX() + 64;}
    static float pianoX()              {return 25;}

    static float staveThickness()      {return 2;}

    static int playZoneEarly()     {return m_playZoneEarly;}
    static int playZoneLate()      {return m_playZoneLate;}
    static int silenceTimeOut()    {return 8000;} // the time in msec before everything goes quiet
    static int chordNoteGap()      {return 10;} // all notes in a cord must be spaced less than this a gap
    static int chordMaxLength()    {return 20;} // the max time between the start and end of a cord

    static IColorPreference * colorPref;

    static void setColorPreference(IColorPreference * pref) {
        colorPref = pref;
    }

    static CColor staveColor() {
        if ( colorPref != NULL) {
            return colorPref->staveColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }
    static CColor staveDimColor() {
        if ( colorPref != NULL) {
            return colorPref->staveDimColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }
    static CColor noteColor()  {
        if ( colorPref != NULL) {
            return colorPref->noteColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }
    static CColor noteDimColor() {
        if ( colorPref != NULL) {
            return colorPref->noteDimColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }
    //static CColor playedGoodColor()    {return CColor(0.4, 0.4, 0.0);}
    static CColor playedGoodColor() {
        if ( colorPref != NULL) {
            return colorPref->playedGoodColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }
    static CColor playedBadColor() {
        if ( colorPref != NULL) {
            return colorPref->playedBadColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }
    static CColor playedStoppedColor() {
        if ( colorPref != NULL) {
            return colorPref->playedStoppedColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }
    static CColor backgroundColor() {
        if ( colorPref != NULL) {
            return colorPref->backgroundColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }
    static CColor barMarkerColor() {
        if ( colorPref != NULL) {
            return colorPref->barMarkerColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }
    static CColor beatMarkerColor() {
        if ( colorPref != NULL) {
            return colorPref->beatMarkerColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }
    static CColor pianoGoodColor()      {return playedGoodColor();}
    static CColor pianoBadColor() {
        if ( colorPref != NULL) {
            return colorPref->pianoBadColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }
    static CColor noteNameColor() {
        if ( colorPref != NULL) {
            return colorPref->noteNameColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }

    static CColor playZoneAreaColor()  {
        if ( colorPref != NULL) {
            return colorPref->playZoneAreaColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }
    static CColor playZoneEndColor() {
        if ( colorPref != NULL) {
            return colorPref->playZoneEndColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }
    static CColor playZoneMiddleColor() {
        if ( colorPref != NULL) {
            return colorPref->playZoneMiddleColor();
        }
        throw std::runtime_error("Color preference not initialized."); 
    }

    static void setDefaults() {
    #ifdef _WIN32
         tickRate = 12;
    #else
          tickRate = 4; // was 12
    #endif
    }

    static void setStaveEndX(float x)
    {
         m_staveEndX = x;
    }
    static int getAppX(){return m_appX;}
    static int getAppY(){return m_appY;}
    static int getAppWidth(){return m_appWidth;}
    static int getAppHeight(){return m_appHeight;}
    static void setAppDimentions(int x, int y,int width, int height)
    {
        m_appX = x;
        m_appY = y;
        m_appWidth = width;
        m_appHeight = height;
    }

    static int defaultWrongPatch() {return 7;} // Starts at 1
    static int defaultRightPatch() {return 1;} // Starts at 1

    static int logLevel;
    static bool experimentalTempo;
    static bool experimentalNoteLength;
    static int experimentalSwapInterval;
    static int tickRate;
    static bool useLogFile;
    static bool midiInputDump;
    static int keyboardLightsChan;

private:
    static float m_staveEndX;
    static int m_appX, m_appY, m_appWidth, m_appHeight;
    static const int m_playZoneEarly;
    static const int m_playZoneLate;
};

#endif //__CFG_H__
