/*********************************************************************************/
/*!
@file           StavePos.h

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


#include "StavePosition.h"
#include "Draw.h"

float CStavePos::m_staveCenterY;
int CStavePos::m_KeySignature;
int CStavePos::m_KeySignatureMajorMinor;
const staveLookup_t*  CStavePos::m_staveLookUpTable;
float CStavePos::m_staveCentralOffset = (staveHeight() * 3)/2;

////////////////////////////////////////////////////////////////////////////////
//! @brief Calculates the position of a note on the stave
void CStavePos::notePos(whichPart_t hand, int midiNote)
{
    const int notesInAnOctive = 7; // Don't count middle C twice
    const int semitonesInAnOctive = 12;
    setHand(hand);
    int index = midiNote % semitonesInAnOctive;

    const staveLookup_t* lookUpItem;

    lookUpItem = &m_staveLookUpTable[index];

    if (m_hand == PB_PART_right)
        m_staveIndex =   lookUpItem->pianoNote - 7;
    else if (m_hand == PB_PART_left)
        m_staveIndex = lookUpItem->pianoNote + 5;

    m_staveIndex += (midiNote/semitonesInAnOctive)*notesInAnOctive - notesInAnOctive*5 ;
    m_accidental = lookUpItem->accidental;
    //ppLogTrace("chan %d Note %2d - %2d (%d %d %d),\n", midiNote, m_staveIndex, m_accidental,
        //index%semitonesInAnOctive, lookUpItem->pianoNote, (midiNote/semitonesInAnOctive)*notesInAnOctive);
}

// convert the midi note to the note name C=1, D=2, E=3, F=4, G=5, A=6, B=7
staveLookup_t CStavePos::midiNote2Name(int midiNote)
{
    const staveLookup_t* staffLookupC = getstaveLookupTable(0); // Key of C so we get all the not names

    const int semitonesInAnOctive = 12;
    int index = midiNote % semitonesInAnOctive;

    staveLookup_t item = staffLookupC[index];

    if (item.accidental != 0) // should it be a sharp or a flat
    {
        // Should it be called for example G# or Ab
        if (item.pianoNote != m_staveLookUpTable[index].pianoNote)
        {
            item.pianoNote = m_staveLookUpTable[index].pianoNote;
            if (m_KeySignature > 0)
                item.accidental = 1; // And change to sharp
            else
                item.accidental = -1;  // But use a flat
        }
    }
    return item;
}

void CStavePos::setKeySignature(int key, int majorMinor)
{
    m_KeySignature = key;
    m_KeySignatureMajorMinor = majorMinor;
    if (key == NOT_USED)
        key = 0;
    m_staveLookUpTable = getstaveLookupTable(key);
    CDraw::forceCompileRedraw();
}

const staveLookup_t* CStavePos::getstaveLookupTable(int key)
{
    const staveLookup_t* staveLookup;

    static const staveLookup_t staffLookupFlat6[] = { // Gb [Bb Eb Ab Db Gb Cb]
        {1, 2}, // Cn
        {2, 0}, //.Db
        {2, 2}, // Dn
        {3, 0}, //.Eb
        {3, 2}, // En
        {4, 0}, //.F
        {5, 0}, //-Gb
        {5, 2}, // Gn
        {6, 0}, //.Ab
        {6, 2}, // An
        {7, 0}, //.Bb
        {8, 0}  //.Cb also called Bn
    };

    static const staveLookup_t staffLookupFlat5[] = { // Db [Bb Eb Ab Db Gb]
        {1, 0}, //.C
        {2, 0}, //-Db
        {2, 2}, // Dn
        {3, 0}, //.Eb
        {3, 2}, // En
        {4, 0}, //.F
        {5, 0}, //.Gb
        {5, 2}, // Gn
        {6, 0}, //.Ab
        {6, 2}, // An
        {7, 0}, //.Bb
        {7, 2}  // Bn
    };

    static const staveLookup_t staffLookupFlat4[] = { // Ab [Bb Eb Ab Db]
        {1, 0}, //.C
        {2, 0}, //.Db
        {2, 2}, // Dn
        {3, 0}, //.Eb
        {3, 2}, // En
        {4, 0}, //.F
        {5,-1}, // Gb
        {5, 0}, //.G
        {6, 0}, //-Ab
        {6, 2}, // An
        {7, 0}, //.Bb
        {7, 2}  // Bn
    };

    static const staveLookup_t staffLookupFlat3[] = { // Eb [Bb Eb Ab]
        {1, 0}, //.C
        {2,-1}, // Db
        {2, 0}, //.D
        {3, 0}, //-Eb
        {3, 2}, // En
        {4, 0}, //.F
        {5,-1}, // Gb
        {5, 0}, //.G
        {6, 0}, //.Ab
        {6, 2}, // An
        {7, 0}, //.Bb
        {7, 2}  // Bn
    };

    static const staveLookup_t staffLookupFlat2[] = { // Bb [Bb Eb]
        {1, 0}, //.C
        {2,-1}, // Db
        {2, 0}, //.D
        {3, 0}, //.Eb
        {3, 2}, // En
        {4, 0}, //.F
        {4, 1}, // F# (Gb)
        {5, 0}, //.G
        {6,-1}, // Ab
        {6, 0}, //.A
        {7, 0}, //-Bb
        {7, 2}  // Bn
    };

    static const staveLookup_t staffLookupFlat1[] = { // F [Bb]
        {1, 0}, //.C
        {1, 1}, // C# (Db)
        {2, 0}, //.D
        {3,-1}, // Eb
        {3, 0}, //.E
        {4, 0}, //-F
        {4, 1}, // F#
        {5, 0}, //.G
        {6,-1}, // Ab
        {6, 0}, //.A
        {7, 0}, //.Bb
        {7, 2}  // Bn
    };

//  more flats Cb Gb Db Ab Eb Bb <--> F# C# G# D# A# E# more sharps
    static const staveLookup_t staffLookupC[] = {
        {1, 0}, //-C
        {1, 1}, // C#
        {2, 0}, //.D
        {3,-1}, // Eb
        {3, 0}, //.E
        {4, 0}, //.F
        {4, 1}, // F#
        {5, 0}, //.G
        {5, 1}, // G# (or Ab)
        {6, 0}, //.A
        {7,-1}, // Bb
        {7, 0}  //.B
    };

    static const staveLookup_t staffLookupSharp1[] = { // G [F#]
        {1, 0}, //.C
        {1, 1}, // C#
        {2, 0}, //.D
        {2, 1}, // D# (Eb)
        {3, 0}, //.E
        {4,-2}, // Fn
        {4, 0}, //.F#
        {5, 0}, //-G
        {5, 1}, // G#
        {6, 0}, //.A
        {7,-1}, // Bb
        {7, 0}  //.B
    };

    static const staveLookup_t staffLookupSharp2[] = { // D [F# C#]
        {1,-2}, // Cn
        {1, 0}, //.C#
        {2, 0}, //-D
        {2, 1}, // D#
        {3, 0}, //.E
        {4,-2}, // Fn
        {4, 0}, //.F#
        {5, 0}, //.G
        {5, 1}, // G#
        {6, 0}, //.A
        {6, 1}, // A# (Bb)
        {7, 0}  //.B
    };

    static const staveLookup_t staffLookupSharp3[] = { // A [F# C# G#]
        {1,-2}, // Cn
        {1, 0}, //.C#
        {2, 0}, //.D
        {2, 1}, // D#
        {3, 0}, //.E
        {4,-2}, // Fn
        {4, 0}, //.F#
        {5,-2}, // Gn
        {5, 0}, //.G#
        {6, 0}, //-A
        {6, 1}, // A#
        {7, 0}  //.B
    };

    static const staveLookup_t staffLookupSharp4[] = { // E [F# C# G# D#]
        {1,-2}, // Cn
        {1, 0}, //.C#
        {2,-2}, // Dn
        {2, 0}, //.D#
        {3, 0}, //-E
        {4,-2}, // Fn
        {4, 0}, //.F#
        {5,-2}, // Gn
        {5, 0}, //.G#
        {6, 0}, //.A
        {6, 1}, // A#
        {7, 0}  //.B
    };

    static const staveLookup_t staffLookupSharp5[] = { // B [F# C# G# D# A#]
        {1,-2}, // Cn
        {1, 0}, //.C#
        {2,-2}, // Dn
        {2, 0}, //.D#
        {3, 0}, //.E
        {4,-2}, // Fn
        {4, 0}, //.F#
        {5,-2}, // Gn
        {5, 0}, //.G#
        {6,-2}, // An
        {6, 0}, //.A#
        {7, 0}  //-B
    };

// I think there should be some double sharps here ??
    static const staveLookup_t staffLookupSharp6[] = { // F# [F# C# G# D# A# E#]
        {1,-2}, // Cn
        {1, 0}, //.C#
        {2,-2}, // Dn
        {2, 0}, //.D#
        {3,-2}, // En
        {3, 0}, //.E# (really F natural!!!!)
        {4, 0}, //-F#
        {5,-2}, // Gn
        {5, 0}, //.G#
        {6,-2}, // An
        {6, 0}, //.A#
        {7, 0}  //.B
    };
// Cb Gb Db Ab Eb Bb <> F# C# G# D# A# E#

    switch (key)
    {
        case -6: staveLookup = staffLookupFlat6;  break;
        case -5: staveLookup = staffLookupFlat5;  break;
        case -4: staveLookup = staffLookupFlat4;  break;
        case -3: staveLookup = staffLookupFlat3;  break;
        case -2: staveLookup = staffLookupFlat2;  break;
        case -1: staveLookup = staffLookupFlat1;  break;
        case  0: staveLookup = staffLookupC;      break;
        case  1: staveLookup = staffLookupSharp1; break;
        case  2: staveLookup = staffLookupSharp2; break;
        case  3: staveLookup = staffLookupSharp3; break;
        case  4: staveLookup = staffLookupSharp4; break;
        case  5: staveLookup = staffLookupSharp5; break;
        case  6: staveLookup = staffLookupSharp6; break;
        default: staveLookup = staffLookupC;      break;
    }
    return staveLookup;
}
