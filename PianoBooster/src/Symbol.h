/*********************************************************************************/
/*!
@file           Symbol.h

@brief          A single musical symbols (eg note, rest, slur) that can appear on the page.

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
    PB_SYMBOL_barLine,
    PB_SYMBOL_barMarker,
    PB_SYMBOL_beatMarker,
    PB_SYMBOL_playingZone,
    PB_SYMBOL_theEnd
} musicalSymbol_t;


typedef enum {
    PB_ACCIDENTAL_MODIFER_noChange,
    PB_ACCIDENTAL_MODIFER_suppress,
    PB_ACCIDENTAL_MODIFER_force // force a natural/accidental to be played
} accidentalModifer_t;

#define BEAT_MARKER_OFFSET  20 // used to ensure that beat markers are drawn under the note by drawing them early

class CSymbol
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    //@brief constructors
    CSymbol(musicalSymbol_t type, whichPart_t hand, int midiNote, int midiDuration = 0)
    {
        init();
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
    void setPianistTiming(int timing){ m_pianistTiming = timing;}
    int getPianistTiming(){ return m_pianistTiming; }

    void transpose(int amount)
    {
        if (m_symbolType == PB_SYMBOL_note)
        {
            m_midiNote += amount;
            m_stavePos.notePos(m_hand, m_midiNote); // The save position has now moved
        }
    }

    void setIndex(int index, int total)
    {
        m_index = index;
        m_total = total;
    }

    int getNoteIndex() { return m_index; }
    int getNoteTotal() { return m_total; }
    ////////////////////////////////////////////////////////////////////////////////
    //! @brief          The accidental
    //! return          0 = none, 1=sharp, -1 =flat, 2=natural.
    int getAccidental() {
        return getStavePos().getAccidental();
    }


    void setAccidentalModifer(accidentalModifer_t value) {m_accidentalModifer = value;}
    accidentalModifer_t getAccidentalModifer() {return m_accidentalModifer;}


private:
    void init()
    {
        m_symbolType = PB_SYMBOL_none;
        m_midiNote = 0;
        m_hand = PB_PART_none;
        m_midiDuration = 0;
        m_pianistTiming = NOT_USED;
        setIndex(0,0);
        m_accidentalModifer = PB_ACCIDENTAL_MODIFER_noChange;
    }

    CStavePos m_stavePos;
    musicalSymbol_t m_symbolType;
    byte m_midiNote;
    accidentalModifer_t m_accidentalModifer; // Used to suppress the second sharp in the same bar
    unsigned long m_midiDuration;
    whichPart_t m_hand;

    CColour m_colour;
    int m_pianistTiming;
    int m_index;  // the number of the note per hand starting from the bottom.
    int m_total;  // the number of the notes per hand;
};




#endif // _SYMBOL_H_

