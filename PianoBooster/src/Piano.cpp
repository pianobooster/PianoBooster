/*********************************************************************************/
/*!
@file           Piano.cpp

@brief          The Design.

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

#include <cstring>
#include "Util.h"
#include "Piano.h"
#include "Cfg.h"
#include "StavePosition.h"

void CPiano::drawPianoInputChord(CChord* chord, CColour colour)
{
    int i;

    drColour(colour);

    CStavePos stavePos;

    for ( i = 0; i < chord->length(); i++)
    {
        stavePos.notePos(chord->getNote(i).part(), chord->getNote(i).pitch());
        if (stavePos.getStaveIndex() >= MAX_STAVE_INDEX || stavePos.getStaveIndex() <= MIN_STAVE_INDEX )
            continue;

        glLineWidth (3.0);
        if (stavePos.getAccidental() != 0)
        {
            glEnable (GL_LINE_STIPPLE);
            glLineStipple (1, 0x0f0f);  /*  dashed  */
            glLineWidth (3.0);
        }

        oneLine(Cfg::playZoneX() - 64, stavePos.getPosYAccidental(), Cfg::playZoneX(), stavePos.getPosYAccidental());
        glDisable (GL_LINE_STIPPLE);
    }
}


void CPiano::drawPianoInput()
{
    if (m_goodChord == 0 || m_badChord == 0)
        return;

    if (m_goodChord->length() > 0)
        drawPianoInputChord(m_goodChord, Cfg::playedGoodColour());//CColour(0.0, 0.0, 1.0));

    if (m_badChord->length() > 0)
        drawPianoInputChord(m_badChord, CColour(1.0, 0.0, 0.0));

    //printf("drawPianoInput %d \n", m_pcNote);
}

void CPiano::drawPianoKeyboard()
{
    drawSymbol(CSymbol(PB_SYMBOL_pianoKeyboard, CStavePos(PB_PART_both, 0)), Cfg::pianoX());
}

