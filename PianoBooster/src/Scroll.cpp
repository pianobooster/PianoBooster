/*********************************************************************************/
/*!
@file           Scroll.c

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

#include "Cfg.h"
#include "Scroll.h"

//#define NOTE_AHEAD_GAP          50
//#define NOTE_BEHIND_GAP          14

#define NOTE_AHEAD_GAP          22 // the notes on the left hand side of the score
#define NOTE_BEHIND_GAP         14

void CScroll::compileSlot(CSlotDisplayList info)
{
    int i;

    if (m_show == false || info.m_displayListId == 0)
        return;

    glNewList (info.m_displayListId, GL_COMPILE);
    glTranslatef (info.getDeltaTime() * m_noteSpacingFactor, 0.0, 0.0); /*  move position  */

    info.transpose(m_transpose);
    CStavePos stavePos;
    int av8Left = info.getAv8Left();
    for (i=0; i < info.length(); i++)
    {
        stavePos.notePos(info.getSymbol(i).getHand(), info.getSymbol(i).getNote());
        //ppLogTrace ("compileSlot len %d id %2d next %2d time %2d type %2d note %2d", info.length(), info.m_displayListId,
        //info.m_nextDisplayListId, info.getDeltaTime(), info.getSymbol(i).getType(), info.getSymbol(i).getNote());

        drawSymbol(info.getSymbol(i), 0.0, stavePos.getPosYRelative()); // we add this  back when drawing this symbol
    }
    glCallList (info.m_nextDisplayListId);    /* Automatically draw the next slot even if it is not there yet */
    glEndList ();
}

/*! Insert a symbol into the display list
 * @return  false when we have run out of symbols
 */
bool CScroll::insertSlots()
{
    GLuint nextListId = 0;

    if (m_headSlot.length() == 0)
        m_headSlot = m_notation->nextSlot();
    if (m_headSlot.length() == 0 || m_headSlot.getSymbolType(0) == PB_SYMBOL_theEnd) // this means we have reached the end of the file
        return false;

    while (true)
    {
        float headDelta = deltaAdjust(m_deltaHead) * m_noteSpacingFactor;
        float slotDetlta = Cfg::staveEndX() - Cfg::playZoneX() - m_headSlot.getDeltaTime() * m_noteSpacingFactor - NOTE_BEHIND_GAP;

        if (headDelta > slotDetlta)
            break;

        if (m_show)
        {
            if (m_symbolID == 0)
                m_symbolID = glGenLists (1);

            nextListId = glGenLists (1);
        }
        else
        {
            nextListId = m_symbolID = 0;
        }

        CSlotDisplayList info(m_headSlot, m_symbolID, nextListId);

        m_deltaHead += info.getDeltaTime() * SPEED_ADJUST_FACTOR;

        compileSlot(info);

        m_scrollQueue->push(info);
        m_symbolID  = nextListId;

        m_headSlot = m_notation->nextSlot();
        if (m_headSlot.length() == 0 || m_headSlot.getSymbolType(0) == PB_SYMBOL_theEnd) // this means we have reached the end of the file
            return false;
    }
    return true;
}

void CScroll::removeEarlyTimingMakers()
{
    float delta = deltaAdjust(m_deltaTail) * m_noteSpacingFactor  + Cfg::playZoneX() - Cfg::scrollStartX() - NOTE_AHEAD_GAP;
    // only look a few steps (10) into the scroll queue
    for (int i = 0; i < 10 && i < m_scrollQueue->length(); i++ )
    {
        if (delta < -(m_scrollQueue->index(i).getLeftSideDeltaTime() * m_noteSpacingFactor))
        {
            m_scrollQueue->indexPtr(i)->clearAllNoteTimmings();
            compileSlot(m_scrollQueue->index(i));
        }
        delta += m_scrollQueue->index(i).getDeltaTime() * m_noteSpacingFactor;
    }
}

void CScroll::removeSlots()
{
    while (m_scrollQueue->length() > 0)
    {
        if (deltaAdjust(m_deltaTail) * m_noteSpacingFactor > -Cfg::playZoneX() + Cfg::scrollStartX() + NOTE_AHEAD_GAP -(m_scrollQueue->index(0).getLeftSideDeltaTime() * m_noteSpacingFactor) )
            break;

        CSlotDisplayList info = m_scrollQueue->pop();

        m_deltaTail += info.getDeltaTime() * SPEED_ADJUST_FACTOR;

        //ppLogTrace("Remove slot id %2d time %2d type %2d note %2d", info.m_displayListId, info.getDeltaTime(), info.getSymbol(0).getType(), info.getSymbol(0).getNote());

        if (info.m_displayListId)
            glDeleteLists( info.m_displayListId, 1);
        if (m_wantedIndex > 0)
            m_wantedIndex--;  // also the Chord has moved down one place
        else
        {
            m_wantedIndex = 0;
            m_wantedDelta = m_deltaTail;
        }
    }
}


//! Draw all the symbols that we have in the list
void CScroll::drawScrollingSymbols(bool show)
{
    insertSlots();  // new symbols at the end of the score
    removeSlots();  // delete old symbols no longer required
    removeEarlyTimingMakers();

    if (show == false)   // Just update the queue only
        return;

    if (m_scrollQueue->length() == 0 || m_scrollQueue->indexPtr(0)->m_displayListId == 0)
        return;

    glPushMatrix();
    glTranslatef (Cfg::playZoneX() + deltaAdjust(m_deltaTail) * m_noteSpacingFactor, CStavePos::getStaveCenterY(), 0.0);

    BENCHMARK(8, "glTranslatef");

    if (m_scrollQueue->length() > 0)
        glCallList (m_scrollQueue->indexPtr(0)->m_displayListId);
    BENCHMARK(9, "glCallList");

    glPopMatrix();
}


void CScroll::scrollDeltaTime(int ticks)
{
    m_deltaHead -= ticks;
    m_deltaTail -= ticks;
    m_wantedDelta -= ticks;
}

bool CScroll::validPianistChord(int index)
{
    CSlot* pSlot = m_scrollQueue->indexPtr(index);

    assert(pSlot->length()!=0);
    if (pSlot->getSymbol(0).getType() == PB_SYMBOL_note)
    {
        if (m_displayHand == PB_PART_both)
            return true;

        //eventually we need two slot queues one for each hand
        for (int i = 0; i < pSlot->length(); i++)
        {

            if (pSlot->getSymbol(i).getHand() ==  m_displayHand)
                return true;
        }
    }
    return false;
}

int CScroll::findWantedChord(int note, CColour colour, int wantedDelta)
{
    if (colour == Cfg::playedBadColour()) // fixme should be an enum
        return m_wantedIndex;
    {
        while ( m_wantedIndex + 1 < m_scrollQueue->length())
        {
            if ((m_wantedDelta + m_scrollQueue->indexPtr(m_wantedIndex)->getDeltaTime() * SPEED_ADJUST_FACTOR) >= -wantedDelta)
            {
                if (validPianistChord(m_wantedIndex) == true)
                    break;
            }
            m_wantedDelta += m_scrollQueue->indexPtr(m_wantedIndex)->getDeltaTime() * SPEED_ADJUST_FACTOR;
            m_wantedIndex++;
        }
    }
    return m_wantedIndex;
}

void CScroll::setPlayedNoteColour(int note, CColour colour, int wantedDelta, int pianistTimming)
{
    int index;
    if (m_wantedIndex >= m_scrollQueue->length())
        return;
    index = findWantedChord(note, colour, wantedDelta);
    note -= m_transpose;
    m_scrollQueue->indexPtr(index)->setNoteColour(note, colour);
    if (pianistTimming != NOT_USED)
    {
        pianistTimming = deltaAdjust(pianistTimming) * DEFAULT_PPQN / CMidiFile::getPulsesPerQuarterNote();

        m_scrollQueue->indexPtr(index)->setNoteTimming(note, pianistTimming);
    }
    compileSlot(m_scrollQueue->index(index));
}


void CScroll::refresh()
{
    int i;
    if (m_show == false)
        return;

    for ( i = 0; i < m_scrollQueue->length(); i++)
        compileSlot(m_scrollQueue->index(i));
}


void CScroll::transpose(int transpose)
{
    if (m_transpose == transpose)
        return;

    m_transpose = transpose;
    refresh();
}

void CScroll::showScroll(bool show)
{
    int i;
    GLuint nextListId = 0;

    m_show = show;
    if (show == true)
    {

        if (m_symbolID == 0)
            m_symbolID = glGenLists (1);


        // add in the missing GL display list
        for ( i = 0; i < m_scrollQueue->length(); i++)
        {
            //assert (m_scrollQueue->indexPtr(i)->m_displayListId == 0);
            nextListId = glGenLists (1);
            m_scrollQueue->indexPtr(i)->m_displayListId = m_symbolID;
            m_scrollQueue->indexPtr(i)->m_nextDisplayListId = nextListId;
            m_symbolID  = nextListId;

        }
        // And now compile the slot (remember that each slot points to the next one)
        for ( i = 0; i < m_scrollQueue->length(); i++)
        {
            compileSlot(m_scrollQueue->index(i));
        }
    }
    else
    {
        // Remove all the gl items
        for ( i = 0; i < m_scrollQueue->length(); i++)
        {
            if (m_scrollQueue->indexPtr(i)->m_displayListId != 0 && m_scrollQueue->indexPtr(i)->m_displayListId != nextListId)
                glDeleteLists(m_scrollQueue->index(i).m_displayListId, 1);
            m_scrollQueue->indexPtr(i)->m_displayListId = 0;

            nextListId = m_scrollQueue->indexPtr(i)->m_nextDisplayListId;
            m_scrollQueue->indexPtr(i)->m_nextDisplayListId = 0;

            if (nextListId != 0)
                glDeleteLists(nextListId, 1);

        }
        if (m_symbolID != 0)
           glDeleteLists(m_symbolID, 1);
        m_symbolID = 0;
    }
}


CScroll::CSlotDisplayList::CSlotDisplayList(const CSlot& slot, GLuint displayListId, GLuint nextDisplayListId) : CSlot(slot),
    m_displayListId(displayListId), m_nextDisplayListId(nextDisplayListId)
{
    // It is all done in the initialisation list
}

void CScroll::reset()
{
    int i;
    m_wantedIndex = 0;
    m_wantedDelta = 0;
    m_deltaHead = m_deltaTail = 0;
    m_notation->reset();
    m_headSlot.clear();
    for ( i = 0; i < m_scrollQueue->length(); i++)
    {
        if (m_scrollQueue->index(i).m_displayListId)
            glDeleteLists(m_scrollQueue->index(i).m_displayListId, 1);
    }
    if (m_symbolID != 0)
       glDeleteLists(m_symbolID, 1);
    m_symbolID = 0;

    m_scrollQueue->clear();
    m_ppqnFactor = static_cast<float>(DEFAULT_PPQN) / CMidiFile::getPulsesPerQuarterNote();
    m_noteSpacingFactor = m_ppqnFactor * HORIZONTAL_SPACING_FACTOR;
}
