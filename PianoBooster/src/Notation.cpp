/*********************************************************************************/
/*!
@file           Notation.cpp

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


#include "Notation.h"
#include "Cfg.h"

CSlot CNotation::nextBeatMarker()
{
    const int cfg_barGap = CMidiFile::getPulsesPerQuarterNote() * 30 / DEFAULT_PPQN;

    CSlot slot;

    m_beatPerBarCounter++;

    if (m_beatPerBarCounter >= m_timeSigNumerator)
        m_beatPerBarCounter = -1;

    if (m_beatPerBarCounter == -1)    // Sneak in a bar line
        slot.setSymbol( m_beatLength - cfg_barGap, CSymbol( PB_SYMBOL_barLine, PB_PART_both, 0 ));
    else if (m_beatPerBarCounter == 0)
        slot.setSymbol( cfg_barGap, CSymbol( PB_SYMBOL_barMarker, PB_PART_both, 0 ));
    else
        slot.setSymbol( m_beatLength, CSymbol( PB_SYMBOL_beatMarker, PB_PART_both, 0 ));
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
        if (midi.type() == MIDI_PB_chordSeparator || midi.type() == MIDI_PB_EOF)
        {
            if (m_currentSlot.length() > 0)
            {
                // the cord separator arrives very late so we are behind the times
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
            setTimeSignature(midi.data1(), midi.data2());
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
                if (m_currentSlot.addSymbol(symbol) == false) {
                    ppLog("[%d] Over the Max symbols limit", m_displayChannel + 1); //fix me
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

    if (m_mergeSlots[1].length() == 0)
    {
        m_mergeSlots[0] = nextNoteSlot();
        m_mergeSlots[1] = nextBeatMarker();
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
    size_t i;
    m_currentDeltaTime = 0;
    m_midiInputQueue->clear();
    m_slotQueue->clear();
    for( i = 0; i < arraySize(m_mergeSlots); i++)
        m_mergeSlots[i].clear();
    m_currentSlot.clear();
    m_beatPerBarCounter=0;
    setTimeSignature( 4 , 4);
    m_findScrollerChord.reset();
}
