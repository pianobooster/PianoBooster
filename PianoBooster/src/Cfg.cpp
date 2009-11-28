/*********************************************************************************/
/*!
@file           Cfg.cpp

@brief          xxx.

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




#include "Cfg.h"

float Cfg::m_staveEndX;
int Cfg::logLevel = 1;
bool Cfg::quickStart = false;
int Cfg::m_appX;
int Cfg::m_appY;
int Cfg::m_appWidth;
int Cfg::m_appHeight;
bool Cfg::experimentalTempo = false;
int Cfg::experimentalSwapInterval = -1;

const int Cfg::m_playZoneEarly = 25; // Was 25
const int Cfg::m_playZoneLate = 25;

