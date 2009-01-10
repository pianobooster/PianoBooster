/*********************************************************************************/
/*!
@file           Bar.h

@brief          xxx.

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

#ifndef __BAR_H__
#define __BAR_H__


#include "MidiFile.h"



// controls the bar numbers
class CBar
{
public:

    CBar() { reset(); }

    // You MUST clear the time sig to 0 first before setting an new start Time Signature
    // at the start of the piece of music
    void setTimeSig(int top, int bottom)
    {
        m_currentTimeSigTop = top;
        m_currentTimeSigBottom = bottom;
        if (top == 0 || m_startTimeSigTop == 0) {
            m_startTimeSigTop = top;
            m_startTimeSigBottom = bottom;
            if (top == 0 )
            {
                m_currentTimeSigTop = 4;
                m_currentTimeSigBottom = 4;
            }
        }

        m_beatLength =  (CMidiFile::getPulsesPerQuarterNote() *4)/ m_currentTimeSigBottom;
    }


    int getTimeSigTop() {return m_currentTimeSigTop;} // The Numerator
    int getBeatLength() {return m_beatLength;}

    void reset() {   setTimeSig( 0 , 0);}
    void rewind() {  setTimeSig(m_startTimeSigTop, m_startTimeSigBottom);}
    void getTimeSig(int *top, int *bottom) {
        *top = m_currentTimeSigTop;
        *bottom = m_currentTimeSigBottom;
    }

private:
    //int m_currentDeltaTime;
    int m_beatLength;
    //int m_beatPerBarCounter;

    int m_startTimeSigTop; // The time Sig at the start of the piece
    int m_startTimeSigBottom;
    int m_currentTimeSigTop; // The current time Sig at the start of the piece
    int m_currentTimeSigBottom;

};

#endif  // __BAR_H__

