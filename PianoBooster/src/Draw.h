/*********************************************************************************/
/*!
@file           Draw.h

@brief          .

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
#ifndef __DRAW_H__
#define __DRAW_H__

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#define HORIZONTAL_SPACING_FACTOR   (0.75) // defines the speed of the scrolling


#include "StavePosition.h"

#include "Symbol.h"

#define REDRAW_COUNT 2 // There are two buffers so redraw twice


class CDraw
{
public:
    CDraw()
    {
        m_displayHand = PB_PART_both;
        m_forceCompileRedraw = REDRAW_COUNT;

    }

    void drawSymbol(CSymbol symbol, float x, float y);
    void drawSymbol(CSymbol symbol, float x);
    static void setDisplayHand(whichPart_t hand)
    {
        m_displayHand = hand;
        m_forceCompileRedraw = REDRAW_COUNT;
    }
    static whichPart_t getDisplayHand()    {return m_displayHand;}
    static void drColour(CColour colour) { glColor3f(colour.red, colour.green, colour.blue);}
    static void forceCompileRedraw(int value = REDRAW_COUNT) {    m_forceCompileRedraw = value; }

protected:
    static whichPart_t m_displayHand;
    static int getCompileRedrawCount() {  return m_forceCompileRedraw; }

    void oneLine(float x1, float y1, float x2, float y2);
    void drawStaves(float startX, float endX);
    void drawKeySignature(int key);

private:
    void checkAccidental(CSymbol symbol, float x, float y);
    void drawStaveExtentsion(CSymbol symbol, float x, int noteWidth);
    static int m_forceCompileRedraw;

};

#endif //__DRAW_H__
