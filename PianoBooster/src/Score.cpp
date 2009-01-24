/*********************************************************************************/
/*!
@file           Score.cpp

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

#include <stdarg.h>
#include "Cfg.h"
#include "Draw.h"
#include "Score.h"

CScore::CScore()
{
    size_t i;
    m_piano = new CPiano();
    m_rating = 0;
    m_activeScroll = -1;
    for (i=0; i< arraySize(m_scroll); i++)
    {
        m_scroll[i] = new CScroll(i);
        m_scroll[i]->setChannel(i);
    }

    m_activeScroll = 0;
    m_scroll[m_activeScroll]->showScroll(true);
}

CScore::~CScore()
{
    delete m_piano;
    size_t i;
    for (i=0; i< arraySize(m_scroll); i++)
        delete m_scroll[i];
}

void CScore::drawScroll(bool clearBackground)
{
    if (clearBackground == true)
    {
        float topY = CStavePos(PB_PART_right, MAX_STAVE_INDEX).getPosY();
        float bottomY = CStavePos(PB_PART_left, MIN_STAVE_INDEX).getPosY();
        drColour (Cfg::backgroundColour());
        glRectf(Cfg::scrollStartX(), topY, Cfg::getAppWidth(), bottomY);
    }
    drawSymbol(CSymbol(PB_SYMBOL_playingZone,  CStavePos(PB_PART_both, 0)), Cfg::playZoneX());
    drawScrollingSymbols();
    drawStaves(Cfg::staveStartX(), Cfg::scrollStartX());
    m_piano->drawPianoInput();

}

void CScore::drawScore(bool refresh)
{
    if (refresh)
    {
        drColour (Cfg::staveColour());

        drawSymbol(CSymbol(PB_SYMBOL_gClef, CStavePos(PB_PART_right, -1)), Cfg::clefX()); // The Treble Clef
        drawSymbol(CSymbol(PB_SYMBOL_fClef, CStavePos(PB_PART_left, 1)), Cfg::clefX());
        //m_piano->drawPianoKeyboard();
        drawKeySignature(CStavePos::getKeySignature());
    }
    drawStaves(Cfg::scrollStartX(), Cfg::staveEndX());

    drawScroll(!refresh);
}



