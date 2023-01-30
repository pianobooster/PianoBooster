/*********************************************************************************/
/*!
@file           Score.cpp

@brief          xxxx.

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

#include "Cfg.h"
#include "Draw.h"
#include "Score.h"

CScore::CScore(CSettings* settings) : CDraw(settings)
{
    m_piano = new CPiano(settings);
    m_rating = nullptr;
    for (int i=0; i< arraySize(m_scroll); i++)
    {
        m_scroll[i] = new CScroll(i, settings);
        m_scroll[i]->setChannel(i);
    }

    m_activeScroll = -1;
    m_stavesDisplayListId = 0;
    m_scoreDisplayListId = 0;//glGenLists (1);
}

CScore::~CScore()
{
    delete m_piano;
    for (auto *const scroll : m_scroll)
        delete scroll;

    if (m_scoreDisplayListId != 0)
        glDeleteLists(m_scoreDisplayListId, 1);
    m_scoreDisplayListId = 0;

    if (m_stavesDisplayListId != 0)
        glDeleteLists(m_stavesDisplayListId, 1);
    m_stavesDisplayListId = 0;
}

void CScore::init()
{
}

void CScore::drawScroll(bool refresh)
{
    if (refresh == false)
    {
        float topY = CStavePos(PB_PART_right, MAX_STAVE_INDEX).getPosY();
        float bottomY = CStavePos(PB_PART_left, MIN_STAVE_INDEX).getPosY();
        drColor (Cfg::colorTheme().backgroundColor);
        glRectf(Cfg::scrollStartX(), topY, static_cast<float>(Cfg::getAppWidth()), bottomY);
    }

    if (m_stavesDisplayListId == 0)
        m_stavesDisplayListId = glGenLists (1);

    if (getCompileRedrawCount())
    {

        glNewList (m_stavesDisplayListId, GL_COMPILE_AND_EXECUTE);
            drawSymbol(CSymbol(PB_SYMBOL_playingZone,  CStavePos(PB_PART_both, 0)), Cfg::playZoneX());
            drawStaves(Cfg::scrollStartX(), Cfg::staveEndX());
        glEndList ();
                // decrement the compile count until is reaches zero
        forceCompileRedraw(0);

    }
    else
        glCallList(m_stavesDisplayListId);

    if (m_settings->value("View/PianoKeyboard").toString()=="on"){
        drawPianoKeyboard();
    }
    drawScrollingSymbols(true);
    m_piano->drawPianoInput();
}

void CScore::drawPianoKeyboard(){
    const static int keysCount = 88;
    struct PianoKeyboard {
        int i, k;
        float yStart;
        float xSize;
        float ySize;

        float xPlaceSize;
        float xKeySize;
        char state[keysCount];
        bool stopped;

        PianoKeyboard() {
            i = 0; k = 0;
            yStart = 0.0f;
            xSize = Cfg::staveEndX() - Cfg::staveStartX();
            ySize = 30;

            xPlaceSize = xSize / 52.0f;
            xKeySize = xPlaceSize - xPlaceSize * 0.1f;
            stopped = false;
        }

        void drawBlackKey(int i, int k) {
            glPushMatrix();
            float yBlackShift = ySize / 2.5f;
            float yBlackSize = ySize - yBlackShift;
            glScalef(1.0f, 1.4f, 1.0f);
            glTranslatef(Cfg::staveStartX() + xPlaceSize * static_cast<float>(i) - xPlaceSize / 3.0f,
                yStart + yBlackShift, 0.0f);

            float xKeySize = this->xKeySize / 1.5f;

            CDraw::drColor (CColor(0.0, 0.0, 0.0));
            if(state[k]==1) CDraw::drColor(stopped ? Cfg::colorTheme().playedStoppedColor : Cfg::colorTheme().noteColor);
            if(state[k]==2) CDraw::drColor(Cfg::colorTheme().playedBadColor);
            glBegin(GL_QUADS);
            glVertex2f(0, yBlackSize);
            glVertex2f(xKeySize, yBlackSize);
            glVertex2f(xKeySize, 0);
            glVertex2f(0, 0);
            glEnd();

            glPopMatrix();
            state[k] = 0;
        }

        void drawWhiteKey() {
            glPushMatrix();
            glScalef(1.0f, 1.4f, 1.0f);
            glTranslatef(Cfg::staveStartX() + xPlaceSize * static_cast<float>(i++), yStart, 0.0f);

            CDraw::drColor (CColor(1.0, 1.0, 1.0));
            if(state[k]==1) CDraw::drColor(stopped ? Cfg::colorTheme().playedStoppedColor : Cfg::colorTheme().noteColor);
            if(state[k]==2) CDraw::drColor(Cfg::colorTheme().playedBadColor);
            glBegin(GL_QUADS);
            glVertex2f(0, ySize);
            glVertex2f(xKeySize, ySize);
            glVertex2f(xKeySize, 0);
            glVertex2f(0, 0);
            glEnd();

            glPopMatrix();
            state[k++] = 0;
        }

        void drawOctave() {
            drawWhiteKey();
            int b1 = i, k1 = k++;
            drawWhiteKey();
            int b2 = i, k2 = k++;
            drawWhiteKey();
            drawWhiteKey();
            int b3 = i, k3 = k++;
            drawWhiteKey();
            int b4 = i, k4 = k++;
            drawWhiteKey();
            int b5 = i, k5 = k++;
            drawWhiteKey();
            drawBlackKey(b1, k1);
            drawBlackKey(b2, k2);
            drawBlackKey(b3, k3);
            drawBlackKey(b4, k4);
            drawBlackKey(b5, k5);
        }

        void drawKeyboard() {
            i = k = 0;
            drawWhiteKey();
            int b1 = i, k1 = k++;
            drawWhiteKey();
            drawBlackKey(b1, k1);
            for(int i=0; i<7; ++i) drawOctave();
            drawWhiteKey();
        }
    };
    static PianoKeyboard pianoKeyboard;

    CChord chord = m_piano->getBadChord();
    for(int n=0; n<chord.length(); ++n) {
        int pitch = chord.getNote(n).pitch();
        int k = pitch - 21;
        k = k < 0 ? 0 : (k >= keysCount ? (keysCount-1) : k);
        pianoKeyboard.state[k] = 2;
    }

    for (auto *const scroll : m_scroll) {
        int notes[64];
        memset(notes, 0, sizeof(notes));
        bool stopped = scroll->getKeyboardInfo(notes);
        for(int *note=notes; *note; ++note) {
            pianoKeyboard.stopped = stopped;
            int k = *note - 21;
            k = k < 0 ? 0 : (k >= keysCount ? (keysCount-1) : k);
            pianoKeyboard.state[k] = 1;
        }
    }

    pianoKeyboard.drawKeyboard();
}

void CScore::drawScore()
{
    if (getCompileRedrawCount())
    {
        if (m_scoreDisplayListId == 0)
            m_scoreDisplayListId = glGenLists (1);

        glNewList (m_scoreDisplayListId, GL_COMPILE_AND_EXECUTE);
            drColor (Cfg::colorTheme().staveColor);

            drawSymbol(CSymbol(PB_SYMBOL_gClef, CStavePos(PB_PART_right, -1)), Cfg::clefX()); // The Treble Clef
            drawSymbol(CSymbol(PB_SYMBOL_fClef, CStavePos(PB_PART_left, 1)), Cfg::clefX());
            drawKeySignature(CStavePos::getKeySignature());
            drawStaves(Cfg::staveStartX(), Cfg::scrollStartX());
        glEndList ();
    }
    else
        glCallList(m_scoreDisplayListId);
}
