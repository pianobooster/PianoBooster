/*********************************************************************************/
/*!
@file           Symbol.h

@brief          A single musical symbols (eg note, rest, slur) that can appear on the page.

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

#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include "Util.h"
#include "Cfg.h"
#include "StavePosition.h"


typedef enum
{
    PB_SYMBOL_none,
    PB_SYMBOL_note,
    PB_SYMBOL_drum,
    PB_SYMBOL_gClef, // The Treble Clef
    PB_SYMBOL_fClef, // The Base Clef
    PB_SYMBOL_sharp,
    PB_SYMBOL_flat,
    PB_SYMBOL_natural,
    PB_SYMBOL_pianoKeyboard,
    PB_SYMBOL_barLine,
    PB_SYMBOL_barMarker,
    PB_SYMBOL_beatMarker,
    PB_SYMBOL_playingZone,
    PB_SYMBOL_theEnd
} musicalSymbol_t;


class CSymbol
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    //@brief constructors
    CSymbol(musicalSymbol_t type, whichPart_t hand, int midiNote, int midiDuration = 0)
    {
        m_symbolType = type;
        m_midiNote = midiNote;
        m_hand = hand;
        m_midiDuration = midiDuration;
        m_stavePos.notePos(hand, midiNote);
    }

    CSymbol()
    {
        init();
    }

    CSymbol(musicalSymbol_t type,  CStavePos stavePos, CColour colour = Cfg::noteColour())
    {
        init();
        m_symbolType = type;
        m_stavePos = stavePos;
        m_colour = colour;
        m_hand = stavePos.getHand();
    }



    ////////////////////////////////////////////////////////////////////////////////
    //@brief Get the type of symbol
    musicalSymbol_t getType(){return m_symbolType;}

    ////////////////////////////////////////////////////////////////////////////////
    //@brief how long is the note
    int getMidiDuration(){return m_midiDuration;}

    ////////////////////////////////////////////////////////////////////////////////
    //@brief returns the midi note number
    int getNote(){return m_midiNote;}

    ////////////////////////////////////////////////////////////////////////////////
    //@brief returns the midi note number
    whichPart_t getHand(){return m_hand;}

    ////////////////////////////////////////////////////////////////////////////////
    //@brief get the Stave Position
    CStavePos getStavePos(){return m_stavePos;}

    void setColour(CColour colour){ m_colour = colour;}
    CColour getColour(){return m_colour;}

    void transpose(int amount)
    {
        if (m_symbolType == PB_SYMBOL_note)
            m_midiNote += amount;
        m_stavePos.notePos(m_hand, m_midiNote); // fixme remove m_stavePos
    }

private:
    void init()
    {
        m_symbolType = PB_SYMBOL_none;
        m_midiNote = 0;
        m_hand = PB_PART_none;
        m_midiDuration = 0;
    }

    CStavePos m_stavePos;
    musicalSymbol_t m_symbolType;
    byte m_midiNote;
    unsigned long m_midiDuration;
    whichPart_t m_hand;

    CColour m_colour;
};




#endif // _SYMBOL_H_

