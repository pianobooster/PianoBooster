/*********************************************************************************/
/*!
@file           Song.h

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

#ifndef __SONG_H__
#define __SONG_H__

#include <QString>

#include "Notation.h"
#include "Conductor.h"
#include "TrackList.h"

#define PC_KEY_LOWEST_NOTE    58
#define PC_KEY_HIGHEST_NOTE    75

class CSong : public CConductor
{
public:
    CSong()
    {
        CStavePos::setKeySignature( NOT_USED, 0 );
        m_midiFile = new CMidiFile;
        m_trackList = new CTrackList;

        reset();
    }

    ~CSong()
    {
        delete m_midiFile;
        delete m_trackList;
    }

    void reset()
    {
        m_reachedMidiEof = false;
        m_findChord.reset();
    }

    void init(CScore * scoreWin, CSettings* settings);
    eventBits_t task(int ticks);
    bool pcKeyPress(int key, bool down);
    void loadSong(const QString &filename);
    void regenerateChordQueue();

    void rewind();

    void playFromStartBar()
    {
        rewind();
        playMusic(true);
    }



    void setActiveHand(whichPart_t hand);
    whichPart_t getActiveHand(){return CNote::getActiveHand();}

    void setActiveChannel(int part);
    void setPlayMode(playMode_t mode);
    CTrackList* getTrackList() {return m_trackList;}
    void refreshScroll();


    QString getSongTitle() {return m_songTitle;}

private:
    void midiFileInfo();


    CMidiFile * m_midiFile;
    CFindChord m_findChord;
    bool m_reachedMidiEof;
    CChord m_fakeChord;  // the chord played with the tab key
    CTrackList* m_trackList;
    QString m_songTitle;
};

#endif  // __SONG_H__

