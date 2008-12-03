/*********************************************************************************/
/*!
@file           Piano.h

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

#ifndef __PIANO_H__
#define __PIANO_H__

#include "Draw.h"
#include "Chord.h"

class CPiano : protected CDraw
{

public:
    CPiano()
    {
        m_goodChord = 0;
        m_badChord = 0;
    }
    void drawPianoKeyboard();
    void drawPianoInput();

    void setInputChords(CChord* good, CChord* bad)
    {
        m_goodChord = good;
        m_badChord = bad;
    }
private:
    void drawPianoInputChord(CChord* chord, CColour colour);

    CChord* m_goodChord;
    CChord* m_badChord;
};

#endif //__PIANO_H__
