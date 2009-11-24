/*********************************************************************************/
/*!
@file           MidiTrack.h

@brief          xxxx.

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

#ifndef __MIDITRACK_H__
#define __MIDITRACK_H__
#include <QString>
#include <string>
#include <fstream>
#include "Queue.h"
#include "MidiEvent.h"

using namespace std;



typedef enum
{
    SMF_NO_ERROR,
    SMF_CANNOT_OPEN_FILE,
    SMF_CORRUPTED_MIDI_FILE,
    SMF_UNKNOW_EVENT,
    SMF_ERROR_TOO_MANY_TRACK,
    SMF_END_OF_FILE
} midiErrors_t;



typedef unsigned char byte_t;
typedef unsigned short word_t;
typedef unsigned long dword_t;

// Reads data from a standard MIDI file
class CMidiTrack
{
public:
    CMidiTrack(fstream& file, int no);

    ~CMidiTrack()
    {
        delete m_trackEventQueue;
    }

    int readDelaTime()
    {
        int deltaTime = m_deltaTime;
        m_deltaTime = 0;
        return deltaTime;
    }

    dword_t getTrackLength() {return m_trackLength;}
    void decodeTrack();
    bool failed() { return (m_midiError != SMF_NO_ERROR) ? true : false;}
    midiErrors_t getMidiError() { return m_midiError;}

    int length() {return m_trackEventQueue->length();}
    CMidiEvent pop() {return m_trackEventQueue->pop();}
    QString getTrackName() {return m_trackName;}

    static void setLogLevel(int level){m_logLevel = level;}


private:
    void errorFail(midiErrors_t error)
    {
        if (m_midiError != SMF_NO_ERROR || error != SMF_NO_ERROR)
        {
            m_midiError = error;
            if (m_midiError != SMF_NO_ERROR)
                ppLogError("Midi error %d", m_midiError);
        }
    }
    void midiFailReset() { m_midiError = SMF_NO_ERROR;}


    void ppDebugTrack(int level, const char *msg, ...);

    byte_t readByte(void)
    {
        int c;

        if (m_trackLengthCounter != 0 )
        {
            c = m_file.get();
            if (m_file.fail() == true)
                errorFail(SMF_END_OF_FILE);
            m_trackLengthCounter--;
        }
        else
            c = 0;
        return c;
    }


    word_t readWord(void)
    {
        word_t value;

        value  = (readByte()&0x0ff) <<8 ;
        value |= readByte()&0x0ff;
        return value;
    }

    dword_t readDWord(void)
    {
        dword_t value;

        value  = (readWord()&0x0ffff) <<16 ;
        value |= readWord()&0x0ffff;
        return value;
    }

    void decodeMidiEvent();
    dword_t readVarLen();

    string readTextEvent();
    dword_t readDataEvent(int expectedLength);
    void readMetaEvent(byte_t type);
    void ignoreSysexEvent(byte_t data);

    void readTimeSignatureEvent();
    void readKeySignatureEvent();

    void decodeSystemMessage( byte_t status, byte_t data1 );

    fstream& m_file;
    int m_trackNumber;

    streampos m_filePos;
    dword_t m_trackLength;
    dword_t m_trackLengthCounter;
    CQueue<CMidiEvent>* m_trackEventQueue;
    int m_savedRunningStatus;
    int m_deltaTime;
    midiErrors_t m_midiError;
    QString m_trackName;
    static int m_logLevel;
};

#endif // __MIDITRACK_H__

