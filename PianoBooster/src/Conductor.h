/*********************************************************************************/
/*!
@file           Conductor.h

@brief          The realtime midi engine. Send notes to the midi device and responds to the midi keyboard.

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

#ifndef __CONDUCTOR_H__
#define __CONDUCTOR_H__

#include "MidiEvent.h"
#include "Queue.h"
#include "MidiDevice.h"
#include "Util.h"
#include "Chord.h"
#include "Rating.h"

#if HAS_SCORE
class CScore;
#endif


typedef enum {
    PB_FOLLOW_searching,
    PB_FOLLOW_earlyNotes,
    PB_FOLLOW_waiting,
} followState_t;

enum {
    PB_PLAY_MODE_listen,
    PB_PLAY_MODE_followYou,
    PB_PLAY_MODE_playAlong,
};

typedef int playMode_t;

// The event bits can be ORed together
#define EVENT_BITS_playingStopped            1  //set when we reach the end of piece
#define EVENT_BITS_forceFullRredraw          2 // force the score to be redrawn
/*!
 * @brief   xxxxx.
 */
class CConductor : public CMidiDevice
{
public:
    CConductor();
    ~CConductor();


    //! add a midi event to be analysed and played
    void midiEventInsert(CMidiEvent event);

    //! first check if there is space to add a midi event
    int midiEventSpace();

    //! add a chord to be played by the pianist
    void chordEventInsert(CChord chord) {m_wantedChordQueue->push(chord);}

    //! first check if there is space to add a chord event
    int chordEventSpace() { return m_wantedChordQueue->space();}

    void init();
    void rewind();

    void realTimeEngine(int mSecTicks);
    void playMusic(bool start);
    bool playingMusic() {return m_playing;}


    float getSpeed() {return m_userSpeed;}
    void setSpeed(float speed)
    {
        // limit the allowed speed
        if (speed > 2.0f)
            speed = 2.0f;
        if (speed < 0.1f)
            speed = 0.1f;
        m_userSpeed = speed;
    }
    void muteChannel(int channel, bool state);
    void mutePart(int channel, bool state);
    void transpose(int transpose);

    int getTranspose() {return m_transpose;}
    int getSkill() {return m_skill;}
    void setSkill(int skill)
    {
        m_skill = skill;
        if (m_skill >  5)
            m_skill = 5;
        if (m_skill < 0)
            m_skill = 0;
    }

    void pianistInput(CMidiEvent inputNote);
    void setPlayMode(playMode_t mode)
    {
        m_playMode = mode;
        if (m_playMode < 0 ) m_playMode = 0;
        if (m_playMode > 2 ) m_playMode = 2;
        if ( m_playMode == PB_PLAY_MODE_listen )
            resetWantedChord();
        autoMute();
        outputBoostVolume();
    }

    int getBoostVolume() {return m_boostVolume;}
    void boostVolume(int boostVolume)
    {
        m_boostVolume = boostVolume;
        if (m_boostVolume < -100 ) m_boostVolume = -100;
        if (m_boostVolume > 100 ) m_boostVolume = 100;
        outputBoostVolume();
    }

    static playMode_t getPlayMode() {return m_playMode;}

    CChord getWantedChord() {return m_wantedChord;}
    void setActiveHand(whichPart_t hand);

    void setActiveChannel(int channel);
    int getActiveChannel(){return m_activeChannel;}
    void setPianistChannels(int goodChan, int badChan){
        m_pianistGoodChan = goodChan;
        m_pianistBadChan = badChan;
    }
    CRating* getRating(){return &m_rating;}

    // You MUST clear the time sig to 0 first before setting an new start tim Sig
    void setStartTimeSig(int top, int bottom) {
        if (top == 0 || m_startTimeSigTop == 0) {
            m_startTimeSigTop = top; m_startTimeSigBottom = bottom;
        }
    }
    void getCurrentTimeSig(int *top, int *bottom) {*top = m_currentTimeSigTop; *bottom = m_currentTimeSigBottom;}
    void testWrongNoteSound(bool enable) {m_testWrongNoteSound = enable;}
    void setPianoSoundPatches(int rightSound, int wrongSound){
        m_cfg_rightNoteSound = rightSound;
        m_cfg_wrongNoteSound = wrongSound;
    }

#if HAS_SCORE
    void setScore(CScore * scoreWin){m_scoreWin = scoreWin;}
#endif

protected:

#if HAS_SCORE
    CScore* m_scoreWin;
#endif

    CQueue<CMidiEvent>* m_songEventQueue;
    CQueue<CChord>* m_wantedChordQueue;

    int m_realTimeEventBits; //used to signal real time events to the caller of task()

    void outputStavedNotes();
    void autoMute();

    void resetWantedChord();

    bool validatePianistNote(CMidiEvent& inputNote);
    bool validatePianistChord();

    void setEventBits(int bits) { m_realTimeEventBits |= bits; } // dont change the other bits

private:
    void allSoundOff();
    void resetAllChannels();
    void outputBoostVolume();
    void channelSoundOff(int channel);
    void fetchNextChord();
    void playTransposeEvent(CMidiEvent event);
    void outputSavedNotesOff();
    void findImminentNotesOff();

    int pianistNotesDown(); // Counts the number of notes the pianist has down
    void followPlaying();
    void missedNotesColour(CColour colour);

    int calcBoostVolume(int chan, int volume);

    int m_playingDeltaTime;
    int m_chordDeltaTime;
    bool m_playing;

    int m_transpose;     // the number of semitones to transpose the music
    followState_t m_followState;

    followState_t getfollowState()
    {
        if ( m_playMode == PB_PLAY_MODE_listen )
            return PB_FOLLOW_searching;
        return m_followState;
    }

    CRating m_rating;
    CQueue<CMidiEvent>* m_savedNoteQueue;
    CQueue<CMidiEvent>* m_savedNoteOffQueue;
    CMidiEvent m_nextMidiEvent;
    bool m_muteChannels[MAX_MIDI_CHANNELS];
    bool isChannelMuted(int chan)
    {
        if (chan < 0 || chan >= MAX_MIDI_CHANNELS)
            return true;
        return m_muteChannels[chan];
    }

    float m_userSpeed; // controls the speed of the piece playing
    float m_midiTempo; // controls the speed of the piece playing
    int m_startTimeSigTop; // The time Sig at the start of the piece
    int m_startTimeSigBottom;
    int m_currentTimeSigTop; // The current time Sig at the start of the piece
    int m_currentTimeSigBottom;
    int m_leadLagAdjust; // Sycronises the sound the the video
    int m_silenceTimeOut; // used to create silence if the student stops for toooo long
    CChord m_wantedChord;  // The chord the pianist needs to play
    CChord m_savedwantedChord; // A copy of the wanted chord complete with both left and right parts
    CChord m_goodPlayedNotes;  // The good notes the pianist plays
    CChord m_goodNoteLines; // The coloured note lines that appear on the score when the pianist plays
    CChord m_badNoteLines;
    int m_pianistSplitPoint;    // Defines which notes go in the base and treble clef
    int m_lastSound;
    int m_cfg_rightNoteSound;
    int m_cfg_wrongNoteSound;
    int m_pianistGoodChan;
    int m_pianistBadChan;
    int m_cfg_earlyNotesPoint; // don't press the note too early
    int m_cfg_stopPoint;   // Were we stop the music if the pianist is late
    int m_followPlayingTimeOut;  // O dear, the student is too slow
    int m_cfg_imminentNotesOffPoint;
    int m_cfg_playZoneEarly; // when playing along
    int m_cfg_playZoneLate;
    bool m_testWrongNoteSound;
    int m_boostVolume;
    int m_activeChannel; // The current part that is being displayed (used for boost and autoMute)
    int m_savedMainVolume[MAX_MIDI_CHANNELS];
    static playMode_t m_playMode;
    int m_skill;
};

#endif //__CONDUCTOR_H__
