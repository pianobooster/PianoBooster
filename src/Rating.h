/*********************************************************************************/
/*!
@file           Rating.h

@brief          The generates a score on how well the pianist is doing.

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

#ifndef __RATING_H__
#define __RATING_H__

#include "Util.h"
#include "Cfg.h"

class CRating
{
public:
    CRating()
    {
        reset();
    }

    void reset();
    void totalNotes(int count) { m_totalNotesCount += count;}
    void wrongNotes(int count) { m_wrongNoteCount += count;}
    void lateNotes(int count) { m_lateNoteCount += count;}
    int totalNoteCount() {return m_totalNotesCount;}
    int wrongNoteCount() {return m_wrongNoteCount;}
    int lateNoteCount() {return m_lateNoteCount;}

    double rating()
    {
        double percent = 100;
        if (m_totalNotesCount > 0)
            percent = ((m_totalNotesCount - m_lateNoteCount) * 100.0) /m_totalNotesCount;
        return percent;
    }

    void calculateAccuracy();

    float getAccuracyValue(){ return m_currentAccuracy; }
    CColour getAccuracyColour() { return m_currentColour; }
    bool isAccuracyGood() { return m_goodAccuracyFlag; }

private:
    int m_totalNotesCount;
    int m_previousNoteCount;
    int m_lateNoteCount;
    int m_previousLateNoteCount;
    int m_wrongNoteCount;
    float m_currentAccuracy;
    float m_factor;
    CColour m_currentColour;
    bool m_goodAccuracyFlag;

};


#endif //__RATING_H__
