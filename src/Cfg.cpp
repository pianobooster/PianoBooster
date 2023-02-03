/*********************************************************************************/
/*!
@file           Cfg.cpp

@brief          xxx.

@author         L. J. Barman

    Copyright (c)   2008-2013, L. J. Barman, all rights reserved

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

#include "Cfg.h"

float Cfg::m_staveEndX;
int Cfg::logLevel = LOG_LEVEL_INFO;
int Cfg::m_appX;
int Cfg::m_appY;
int Cfg::m_appWidth;
int Cfg::m_appHeight;
bool Cfg::experimentalTempo = false;
bool Cfg::experimentalNoteLength = false;
bool Cfg::useLogFile = false;
bool Cfg::midiInputDump = false;
int Cfg::keyboardLightsChan = -1;

int Cfg::experimentalSwapInterval = -1;
int Cfg::tickRate;

const int Cfg::m_playZoneEarly = 25; // Was 25
const int Cfg::m_playZoneLate = 25;

ColorTheme Cfg::m_colorTheme;

void ColorTheme::load(BuiltInColorTheme builtInColorTheme)
{
    switch (builtInColorTheme) {
    case BuiltInColorTheme::Light:
        menuColor          = CColor(0.1, 0.6, 0.6);
        menuSelectedColor  = CColor(0.7, 0.7, 0.1);
        staveColor         = CColor(0.2, 0.2, 0.2);
        staveColorDim      = CColor(0.25, 0.40, 0.25);  // grey
        noteColor          = CColor(0.0, 0.0, 0.0);     // black
        noteColorDim       = CColor(0.4, 0.4, 0.4);     // grey
        playedGoodColor    = CColor(0.5, 0.6, 1.0);     // purple
        playedBadColor     = CColor(0.8, 0.3, 0.8);     // orange
        playedStoppedColor = CColor(1.0, 0.8, 0.0);     // bright orange
        backgroundColor    = CColor(1.0, 1.0, 1.0);     // white
        barMarkerColor     = CColor(0.5, 0.5, 0.5);     // grey
        beatMarkerColor    = CColor(0.25, 0.25, 0.25);  // grey
        pianoGoodColor     = playedGoodColor;
        pianoBadColor      = CColor(1.0, 0.0, 0.0);
        noteNameColor      = CColor(0.0, 0.0, 0.0);
        playingZoneBg      = CColor(0.75f, 0.75f, 1.0f);
        playingZoneMiddle  = CColor(0.0f, 0.0f, 0.4f);
        playingZoneBorder  = CColor(0.0f, 0.0f, 0.8f);
        textColor          = CColor(0.0, 0.0, 0.0);     // black
        break;
    default:
        *this = ColorTheme();
    }
}
