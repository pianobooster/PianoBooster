/*********************************************************************************/
/*!
@file           Notation.cpp

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


#include "Notation.h"
#include "Cfg.h"


#define OPTION_DEBUG_NOTATION     0
#if OPTION_DEBUG_NOTATION
#define ppDEBUG_NOTATION(args)     ppLogDebug  args
#else
#define ppDEBUG_NOTATION(args)
#endif


#define MERGESLOT_NOTE_INDEX        0
#define MERGESLOT_BEATMARK_INDEX    1

bool CSlot::addSymbol(CSymbol symbol)
{
    int i;
    if (m_length >= MAX_SYMBOLS)
        return false;

    // Sort the entries low to high
    for (i = m_length - 1; i >= 0; i--)
    {
        if (m_symbols[i].getNote() <= symbol.getNote())
        {
            // don't add duplicates
            if (m_symbols[i].getNote() == symbol.getNote() &&
                m_symbols[i].getType() == symbol.getType() &&
                m_symbols[i].getHand() == symbol.getHand())
                return true;
            break;
        }
        // move the previous entry up one possition
        m_symbols[i+1] = m_symbols[i];
    }
    m_symbols[i+1] = symbol;
    m_length++;
    return true;
}

// find
void CSlot::analyse()
{
    int i;
    int rightIndex = 0;
    int leftIndex = 0;
    int rightTotal = 0;
    int leftTotal = 0;

    for (i = 0; i < m_length; i++)
    {
        if (m_symbols[i].getType() == PB_SYMBOL_note)
        {
            if (m_symbols[i].getHand() == PB_PART_right)
                rightTotal++;
            else if (m_symbols[i].getHand() == PB_PART_left)
                leftTotal++;
        }
    }
    for (i = 0; i < m_length; i++)
    {
        if (m_symbols[i].getType() == PB_SYMBOL_note)
        {
            if (m_symbols[i].getHand() == PB_PART_right)
                m_symbols[i].setIndex(rightIndex++, rightTotal);
            else if (m_symbols[i].getHand() == PB_PART_left)
                m_symbols[i].setIndex(leftIndex++, leftTotal );
        }
    }
}

bool CNotation::m_cfg_displayCourtesyAccidentals = false;

///////////////////////////////////////////////////////////////////////////

CSlot CNotation::nextBeatMarker()
{
    const int cfg_barGap = CMidiFile::ppqnAdjust(30);

    CSlot slot;

    m_beatPerBarCounter++;

    if (m_beatPerBarCounter >= m_bar.getTimeSigTop())
        m_beatPerBarCounter = -1;

    if (m_beatPerBarCounter == -1)    // Sneak in a bar line
        slot.setSymbol( m_bar.getBeatLength() - cfg_barGap, CSymbol( PB_SYMBOL_barLine, PB_PART_both, 0 ));
    else if (m_beatPerBarCounter == 0)
        slot.setSymbol( cfg_barGap, CSymbol( PB_SYMBOL_barMarker, PB_PART_both, 0 ));
    else
        slot.setSymbol( m_bar.getBeatLength(), CSymbol( PB_SYMBOL_beatMarker, PB_PART_both, 0 ));
    return slot;
}

int CNotation::nextMergeSlot()
{
    size_t i;
    CSlot nearestSlot;
    size_t nearestIndex;

    nearestSlot = m_mergeSlots[0];
    nearestIndex = 0;
    for( i = 1; i < arraySize(m_mergeSlots); i++)
    {
        // find the slot with the lowest delta time
        if (m_mergeSlots[i].getDeltaTime() < nearestSlot.getDeltaTime())
        {
            nearestSlot = m_mergeSlots[i];
            nearestIndex = i;
        }
    }

    // Now subtract the delta time from all the others
    for( i = 0; i < arraySize(m_mergeSlots); i++)
    {
        if (i == nearestIndex)
            continue;
        m_mergeSlots[i].addDeltaTime( -nearestSlot.getDeltaTime() );
    }
    return nearestIndex;
}

accidentalModifer_t CNotation::detectSuppressedNatural(int note)
{
    if (note <= 0 || note +1 >= MAX_MIDI_NOTES)
        return PB_ACCIDENTAL_MODIFER_noChange;

    accidentalModifer_t modifer = PB_ACCIDENTAL_MODIFER_noChange;

    while (m_earlyBarChangeDelta >= m_bar.getBarLength())
    {
        m_earlyBarChangeDelta -= m_bar.getBarLength();
        m_earlyBarChangeCounter++;
    }

    CNoteState * pNoteState = &m_noteState[note];
    CNoteState * pBackLink = pNoteState->getBackLink();

    int direction = -CStavePos::getStaveAccidentalDirection(note);
    ppDEBUG_NOTATION(("Note %d %d %d", note, direction, pBackLink));
    // check if this note has occurred in this bar before
    if (pNoteState->getBarChange() == m_earlyBarChangeCounter)
    {
        if (pBackLink)
        {
            ppDEBUG_NOTATION(("Force %d", note));
            modifer = PB_ACCIDENTAL_MODIFER_force;
        }
        else if (direction != 0 && m_cfg_displayCourtesyAccidentals == false)
        {
            ppDEBUG_NOTATION(("Suppress %d %d", note, direction));
            modifer = PB_ACCIDENTAL_MODIFER_suppress;
        }
    }

    if (direction != 0)
    {
        // we are display a accidental so force the note above (or below) to display
        m_noteState[note + direction].setBackLink(pNoteState); // point back to this note
        m_noteState[note + direction].setBarChange(m_earlyBarChangeCounter);
        ppDEBUG_NOTATION(("setting backlink %d %d", note + direction, direction));
    }
    if (pBackLink)
    {
        pNoteState->setBackLink(0);
        pBackLink->setBarChange(-1); // this prevents further suppression on the original note
    }

    pNoteState->setBarChange(m_earlyBarChangeCounter);
    return modifer;
}

void CNotation::findNoteSlots()
{
    CMidiEvent midi;
    CSlot slot;

    while (true)
    {
        // Check that some body has put in some events for us
        if (m_midiInputQueue->length() == 0)
            break;

        midi = m_midiInputQueue->pop();

        m_currentDeltaTime += midi.deltaTime();
        m_earlyBarChangeDelta += midi.deltaTime();
        if (midi.type() == MIDI_PB_chordSeparator || midi.type() == MIDI_PB_EOF)
        {
            if (m_currentSlot.length() > 0)
            {
                // the cord separator arrives very late so we are behind the times
                m_currentSlot.analyse();
                m_slotQueue->push(m_currentSlot);
                m_currentSlot.clear();
            }
            if (midi.type() == MIDI_PB_EOF)
            {
                slot.setSymbol(0, CSymbol( PB_SYMBOL_theEnd, PB_PART_both, 0 ));
                m_slotQueue->push(slot);
            }
            break;
        }

        else if (midi.type() == MIDI_PB_timeSignature)
            m_bar.setTimeSig(midi.data1(), midi.data2());
        else if (midi.type() == MIDI_PB_keySignature)
            CStavePos::setKeySignature(midi.data1(), midi.data2());
        else if (midi.type() == MIDI_NOTE_ON)
        {
            whichPart_t hand = CNote::findHand( midi, m_displayChannel, PB_PART_both );
            if (hand != PB_PART_none)
            {
                musicalSymbol_t symbolType;
                if (midi.channel() == MIDI_DRUM_CHANNEL)
                    symbolType = PB_SYMBOL_drum;
                else
                    symbolType = PB_SYMBOL_note;
                CSymbol symbol(symbolType, hand, midi.note());
                symbol.setColour(Cfg::noteColour());

                // check if this note has occurred in this bar before
                symbol.setAccidentalModifer(detectSuppressedNatural(midi.note()));

                if (m_currentSlot.addSymbol(symbol) == false) {
                    ppLogWarn("[%d] Over the Max symbols limit", m_displayChannel + 1);
                }
                m_currentSlot.addDeltaTime(m_currentDeltaTime);
                m_currentDeltaTime = 0;
                if (hand == PB_PART_left)
                {
                    if (midi.note() < MIDI_BOTTOM_C)
                        m_currentSlot.setAv8Left(MIDI_OCTAVE);
                }
            }
        }
    }
}

CSlot CNotation::nextNoteSlot()
{
    // only if the slot queue is empty should we try to find some more
    if (m_slotQueue->length() == 0)
        findNoteSlots();

    if (m_slotQueue->length() > 0)
        return m_slotQueue->pop();
    else
        return CSlot(); // this is an empty slot which means end of file
}

CSlot CNotation::nextSlot()
{
    int mergeIdx;
    CSlot slot;

    if (m_mergeSlots[MERGESLOT_BEATMARK_INDEX].length() == 0)
    {
        // load up the two slots on start up
        m_mergeSlots[MERGESLOT_NOTE_INDEX] = nextNoteSlot();
        m_mergeSlots[MERGESLOT_BEATMARK_INDEX] = nextBeatMarker();

        // This inserts the beat marksers into the queue early (so they get drawn underneath)
        m_mergeSlots[MERGESLOT_BEATMARK_INDEX].addDeltaTime(
            -CMidiFile::getPulsesPerQuarterNote() * BEAT_MARKER_OFFSET / DEFAULT_PPQN);
    }

    if (m_mergeSlots[0].getSymbolType(0) == PB_SYMBOL_theEnd)
        return m_mergeSlots[0];

    mergeIdx = nextMergeSlot();
    slot = m_mergeSlots[mergeIdx];
    if (mergeIdx == 0)
        m_mergeSlots[mergeIdx] = nextNoteSlot();
    else
        m_mergeSlots[mergeIdx] = nextBeatMarker();
    return slot;
}

void CNotation::midiEventInsert(CMidiEvent event)
{
    if (m_findScrollerChord.findChord(event, m_displayChannel, PB_PART_both ) == true)
    {
        // the Score works differently we just send down a chord separator
        CMidiEvent separator;
        separator.chordSeparator(event);
        m_midiInputQueue->push(separator);
    }

    m_midiInputQueue->push(event);
}

void CNotation::reset()
{
    const int cfg_earlBarLead = CMidiFile::ppqnAdjust(8);

    size_t i;
    m_currentDeltaTime = 0;
    m_midiInputQueue->clear();
    m_slotQueue->clear();
    for( i = 0; i < arraySize(m_mergeSlots); i++)
        m_mergeSlots[i].clear();
    m_currentSlot.clear();
    m_beatPerBarCounter=0;
    m_earlyBarChangeCounter = 0;
    m_earlyBarChangeDelta = cfg_earlBarLead; // We want to detect the bar change early

    m_bar.reset();
    m_findScrollerChord.reset();
    for( i = 0; i < MAX_MIDI_NOTES; i++)
    {
        m_noteState[i].clear();
    }
}
