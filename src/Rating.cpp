/*********************************************************************************/
/*!
@file           Rating.cpp

@brief          The generates a score on how well the pianist is doing.

@author         L. J. Barman

    Copyright (c)   2008-2013, L. J. Barman, all rights reserved

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

#include "Rating.h"
#include "Conductor.h"

void CRating::reset()
{
    m_totalNotesCount = 0;
    m_wrongNoteCount = 0;
    m_lateNoteCount = 0;
    m_currentAccuracy = 0.5;
    m_previousNoteCount = -1;
    m_previousLateNoteCount = 0;
    m_factor = 2.0;
    m_goodAccuracyFlag = false;
}

void CRating::calculateAccuracy()
{
    int direction = 0;

    if (m_previousNoteCount != m_totalNotesCount)
    {
        m_previousNoteCount = m_totalNotesCount;
        direction = 1;
    }

    if (m_previousLateNoteCount != m_lateNoteCount)
    {
        m_previousLateNoteCount = m_lateNoteCount;
        direction = -1;
    }

    if (direction != 0)
    {
        typedef struct
        {
            float value;
            float stepUp;
            float stepDown;
            CColor color;
        } accuracyItem_t;
        const accuracyItem_t accuracyTable[] =
        {
            {1.00f, 0.01f, -0.08f, CColor(0.5, 0.5, 1.0)}, // Only the color is used on the top score
            {0.75f, 0.01f, -0.07f, CColor(0.7, 0.3, 1.0)},
            {0.50f, 0.01f, -0.05f, CColor(1.0, 0.6, 0.4)},
            {0.25f, 0.01f, -0.03f, CColor(1.0, 0.3, 1.0)},
            {0.00f, 0.015f, -0.02f, CColor(1.0, 0.4, 0.2)}
        };

        for (auto i = 0; i< arraySize(accuracyTable); i++)
        {
            if (m_currentAccuracy >= accuracyTable[i].value  || i+1 == arraySize(accuracyTable))
            {
                float stepAmount = (direction > 0) ? accuracyTable[i].stepUp : accuracyTable[i].stepDown;
                if (stepAmount < 0 && CConductor::getPlayMode() == PB_PLAY_MODE_playAlong)
                    stepAmount = accuracyTable[i].stepUp + accuracyTable[i].stepDown;
                m_currentAccuracy += stepAmount *m_factor;
                break;
            }
        }

        if (m_currentAccuracy < 0) m_currentAccuracy = 0;
        if (m_currentAccuracy > 1) m_currentAccuracy = 1;

        m_goodAccuracyFlag = false;

        for (auto i = 0; i< arraySize(accuracyTable); i++)
        {
            if (m_currentAccuracy >= accuracyTable[i].value || i+1 == arraySize(accuracyTable))
            {
                m_currentColor = accuracyTable[i].color;
                if (m_currentAccuracy >= accuracyTable[ 1 ].value) // the second row in the table
                    m_goodAccuracyFlag = true;
                break;
            }
        }
    }
}

