/*********************************************************************************/
/*!
@file           Piano.h

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

#ifndef __PIANO_H__
#define __PIANO_H__

#include "Draw.h"
#include "Chord.h"
#include "Settings.h"


typedef struct {
        float posY;
        float posYOriginal;
        int type; // not used
        int pitch;
} noteNameItem_t;


class CPiano : protected CDraw
{

public:
    CPiano(CSettings* settings) : CDraw(settings)
    {
    }

    void drawPianoInput();

    void addPianistNote(whichPart_t part, int note, bool good);
    bool removePianistNote(int note);

    int pianistAllNotesDown(); // Counts the number of notes the pianist has down
    int pianistBadNotesDown();
    void clear();

private:
    void spaceNoteBunch(unsigned int bottomIndex, unsigned int topIndex);
    void drawPianoInputLines(CChord* chord, CColour colour, int lineLength);
    void drawPianoInputNoteNames();
    void spaceNoteNames();
    void addNoteNameItem(float posY, int pitch, int type);
    void removeNoteNameItem(int pitch);
    void noteNameListClear();

    noteNameItem_t  m_noteNameList[20];
    unsigned int m_noteNameListLength;

    CChord m_goodChord;  // The coloured note lines that appear on the score when the pianist plays
    CChord m_badChord;
};

#endif //__PIANO_H__
