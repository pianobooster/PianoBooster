/*********************************************************************************/
/*!
@file           StavePosition.h

@brief          xxxxxx.

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

#ifndef __STAVE_POS_H__
#define __STAVE_POS_H__

#include "Util.h"
#include "Chord.h"

#include <stdio.h>

typedef  struct {
    int pianoNote;   // 1 is Middle C, 2 is D
    int accidental;
} staveLookup_t;

#define NOT_USED 0x7fffffff

#define MAX_STAVE_INDEX 16
#define MIN_STAVE_INDEX -16

//----------------------------------------------------------------------------
// CStavePos
//----------------------------------------------------------------------------
//! @brief Calculates the position on the stave from the stave index number.
class CStavePos
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    //! @brief The default constructor.
    CStavePos()
    {
        m_staveIndex = 0;
        m_accidental = 0;
        setHand(PB_PART_none);
    };

    ////////////////////////////////////////////////////////////////////////////////
    //! @brief Constructs the Stave position object.
    //! @param hand         the top (right hand) or the bottom the (left hand) see @ref whichPart_t.
    //! @param index        the save index number: 0 central line, 5 = top line, -5 the bottom line.
    //! @param accidental   Indicates an accidental 0 = none, 1=sharp, -1 =flat.
    CStavePos(whichPart_t hand, int index, int accidental = 0)
    {
        m_staveIndex = index;
        m_accidental = accidental;
        setHand(hand);
    };

    ////////////////////////////////////////////////////////////////////////////////
    //! @brief Sets which stave the note will appear on
    //! @param hand         the top (right hand) or the bottom the (left hand) see @ref whichPart_t.
    void setHand(whichPart_t hand)
    {
        m_hand = hand;
        m_offsetY = getStaveCenterY();
        if (m_hand == PB_PART_right)
            m_offsetY += staveCentralOffset();
        else if (m_hand == PB_PART_left)
            m_offsetY -= staveCentralOffset();
    }

    void notePos(whichPart_t hand, int midiNote);


    ////////////////////////////////////////////////////////////////////////////////
    //! @brief Sets which stave the note will appear on
    //! return          The position on the stave.
    float getPosY()
    {
        return verticalNoteSpacing() * m_staveIndex + m_offsetY ;
    }

    float getPosYAccidental() {
        int accidental = m_accidental;
        if (accidental == 2) accidental = 1;
        else if (accidental == -2) accidental = -1;
        return getPosY() + accidental*verticalNoteSpacing()/2;
    }
    float getPosYRelative() { return getPosY() - m_staveCenterY;} // get the Y position relative to the stave centre

    ////////////////////////////////////////////////////////////////////////////////
    //! @brief          The accidental
    //! return          0 = none, 1=sharp, -1 =flat, 2=natural.
    int getAccidental() {return m_accidental;}

    int getStaveIndex() {return m_staveIndex;}
    whichPart_t getHand() {return m_hand;}

    static float getVerticalNoteSpacing(){return verticalNoteSpacing();}
    static float getStaveCenterY(){return m_staveCenterY;}
    static void setStaveCenterY(float y) { m_staveCenterY = y; }
    static void setKeySignature(int key, int majorMinor);
    static int getKeySignature() {return m_KeySignature;}
    static void setStaveCentralOffset(float gap) { m_staveCentralOffset = gap; }
    static float verticalNoteSpacing()      {return 7;}
    static float staveHeight()              {return verticalNoteSpacing() * 8;}
    static float staveCentralOffset()       {return m_staveCentralOffset;}
    // convert the midi note to the note name A B C D E F G
    static staveLookup_t midiNote2Name(int midiNote);
    static const staveLookup_t* getstaveLookupTable(int key);
    
    // do we show a sharp or a flat for this key signature
    // returns 0 = none, 1=sharp, -1 =flat, 2=natural (# Key) , -2=natural (b Key)
    static int getStaveAccidental(int midiNote)
    {
        return m_staveLookUpTable[midiNote%12].accidental;
    }

    // returns 0 = none, 1=above, -1 =below, (a natural is either above or below)
    static int getStaveAccidentalDirection(int midiNote)
    {
        int accidentalDirection = getStaveAccidental(midiNote);
        if (accidentalDirection == 2) // A natural so change to above
            accidentalDirection = 1;
        else if (accidentalDirection == -2) // A natural so change to below
            accidentalDirection = -1;           
            
        return accidentalDirection;
    }


private:
    // fixme TODO This could be improved as the calculations could a done in the constructor
    char m_staveIndex;    // 0 central line, 5 = top line, -5 the bottom line,
    int m_accidental;         // 0 = none, 1=sharp, -1 =flat, 2=natural
    float m_offsetY;
    whichPart_t m_hand;


    static int m_KeySignature;
    static int m_KeySignatureMajorMinor;
    static const staveLookup_t*  m_staveLookUpTable;
    static float m_staveCentralOffset;
    static float m_staveCenterY;
};

#endif //__STAVE_POS_H__
