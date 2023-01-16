/*********************************************************************************/
/*!
@file           Draw.h

@brief          .

@author         L. J. Barman

    Copyright (c)   2008-2020, L. J. Barman and others, all rights reserved

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
#ifndef __DRAW_H__
#define __DRAW_H__

#ifdef _WIN32
  #include <windows.h>
#endif

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
#ifndef NO_USE_FTGL
#include <FTGL/ftgl.h>
#endif
#include <QObject>
#include <QFile>
#include <QApplication>

#define HORIZONTAL_SPACING_FACTOR   (0.75f) // defines the speed of the scrolling
#define FONT_SIZE 16

#include "StavePosition.h"

#include "Symbol.h"

class CSettings;
class CSlot;

class CScrollProperties
{
public:
    CScrollProperties()
    {
        m_horizontal = false;
    }
    bool horizontal() { return m_horizontal; }
private:
    bool m_horizontal;

};

class CDraw : public QObject
{
public:

    CDraw(CSettings* settings);

    ~CDraw(){
#ifndef NO_USE_FTGL
        if (font!=nullptr) delete font;
#endif
    }

    void scrollVertex(float x, float y)
    {
        if (m_scrollProperties->horizontal())
            glVertex2f (x,y);
        else
            glVertex2f (y,x);
    }

    void drawSymbol(CSymbol symbol, float x, float y, CSlot* slot = 0);
    void drawSymbol(CSymbol symbol, float x);
    void drawSlot(CSlot* slot);

    static void setDisplayHand(whichPart_t hand)
    {
        m_displayHand = hand;
        m_forceCompileRedraw = 1;
    }
    static whichPart_t getDisplayHand()    {return m_displayHand;}
    static void drColor(CColor color) { glColor3f(color.red, color.green, color.blue);}
    static void forceCompileRedraw(int value = 1) {    m_forceCompileRedraw = value; }

protected:
    static whichPart_t m_displayHand;
    static int getCompileRedrawCount() {  return m_forceCompileRedraw; }

    void oneLine(float x1, float y1, float x2, float y2);
    void drawStaves(float startX, float endX);
    void drawKeySignature(int key);
    void drawNoteName(int midiNote, float x, float y, int type);
#ifndef NO_USE_FTGL
    void renderText(float x, float y, const char* s);
#endif
    CSettings* m_settings;

private:
    Q_OBJECT
    void drawStaveNoteName(CSymbol symbol, float x, float y);
    bool drawNote(CSymbol* symbol, float x, float y, CSlot* slot, CColor color, bool playable);

    void checkAccidental(CSymbol symbol, float x, float y);
    void drawStaveExtentsion(CSymbol symbol, float x, int noteWidth, bool playable);
    static int m_forceCompileRedraw;
    const static int m_beatMarkerHeight = 10; // The height of the beat markers in the stave positions

    CScrollProperties *m_scrollProperties;
    CScrollProperties m_scrollPropertiesHorizontal;
    CScrollProperties m_scrollPropertiesVertical;
#ifndef NO_USE_FTGL
    FTFont *font;
#endif
};

#endif //__DRAW_H__
