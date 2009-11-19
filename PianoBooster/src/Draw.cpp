/*********************************************************************************/
/*!
@file           Draw.cpp

@brief          xxxxx.

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

#include "Draw.h"
#include "Cfg.h"
#include "Settings.h"

typedef unsigned int guint;
typedef unsigned char guint8;

whichPart_t CDraw::m_displayHand;
int CDraw::m_forceCompileRedraw;

void CDraw::oneLine(float x1, float y1, float x2, float y2)
{
    glBegin(GL_LINES);
    glVertex2f ((x1),(y1));
    glVertex2f ((x2),(y2));
    glEnd();
    //ppLogTrace("oneLine %f %f %f %f", x1, y1, x2, y2);
}

void  CDraw::drawStaveExtentsion(CSymbol symbol, float x, int noteWidth, bool playable)
{
    int index;
    index = symbol.getStavePos().getStaveIndex();
    if (index < 6 && index > -6)
        return;
    index =  index & ~1; // Force index to be even
    whichPart_t hand = symbol.getStavePos().getHand();

    if (playable)
        drColour(Cfg::staveColour());
    else
        drColour(Cfg::staveColourDim());

    glLineWidth (Cfg::staveThickness());
    glBegin(GL_LINES);
    while (index >= 6 || index <= -6)
    {

        glVertex2f (x - noteWidth/2 - 4, CStavePos(hand, index).getPosYRelative());
        glVertex2f (x + noteWidth/2 + 4, CStavePos(hand, index).getPosYRelative());

        // Move the index closer to the stave centre
        if (index > 0)
            index -= 2;
        else
            index += 2;
    }
    glEnd();
}

#define scaleGlVertex(xa, xb, ya, yb) glVertex2f( ((xa) * 1.2) + (xb), ((ya) * 1.2) + (yb))

void CDraw::drawNoteName(int midiNote, float x, float y, int type)
{
    // Ignore note that are too high
    if (midiNote > MIDI_TOP_C + 6)
        return;

    staveLookup_t item = CStavePos::midiNote2Name(midiNote);

    drColour(Cfg::noteNameColour());

    glLineWidth (1.0);

    if (item.accidental != 0)
    {
        const float accidentalOffset = 10;
        x += accidentalOffset/2;
        if (item.accidental == 1)
        {
            glBegin(GL_LINES);
                //  letterSharp4
                scaleGlVertex( -1.317895, x,   5.794585, y);  //  1
                scaleGlVertex( -1.265845, x,   -6.492455, y);  //  2
                scaleGlVertex( 1.252305, x,   6.492455, y);  //  3
                scaleGlVertex( 1.322655, x,   -5.422335, y);  //  4
                scaleGlVertex( -2.645765, x,   1.967805, y);  //  5
                scaleGlVertex( 2.648325, x,   3.625485, y);  //  6
                scaleGlVertex( -2.648325, x,   -3.306965, y);  //  7
                scaleGlVertex( 2.596205, x,   -1.675765, y);  //  8
            glEnd();

        }
        else
        {
            glBegin(GL_LINE_STRIP);
                //  letterFlat
                scaleGlVertex( -2.52933, x,   6.25291, y);  //  1
                scaleGlVertex( -2.50344, x,   -6.25291, y);  //  2
                scaleGlVertex( 0.76991, x,   -3.63422, y);  //  3
                scaleGlVertex( 2.07925, x,   -1.67021, y);  //  4
                scaleGlVertex( 2.52933, x,   0.25288, y);  //  5
                scaleGlVertex( 1.42458, x,   1.07122, y);  //  6
                scaleGlVertex( -0.53943, x,   0.90755, y);  //  7
                scaleGlVertex( -2.46252, x,   -1.01554, y);  //  8
                scaleGlVertex( -2.50344, x,   -1.67021, y);  //  9
            glEnd();

        }
        x -= accidentalOffset;
    }

    switch(item.pianoNote)
    {
    case 1:
        glBegin(GL_LINE_STRIP);
            //  letterC
            scaleGlVertex( 3.513445, x,   2.17485, y);  //  1
            scaleGlVertex( 1.880175, x,   4.47041, y);  //  2
            scaleGlVertex( -1.598825, x,   4.4321, y);  //  3
            scaleGlVertex( -3.692215, x,   2.26571, y);  //  4
            scaleGlVertex( -3.702055, x,   -1.88958, y);  //  5
            scaleGlVertex( -1.630675, x,   -4.47041, y);  //  6
            scaleGlVertex( 1.932545, x,   -4.44064, y);  //  7
            scaleGlVertex( 3.702055, x,   -1.88554, y);  //  8
        glEnd();
    break;

    case 2:
        glBegin(GL_LINE_STRIP);
            //  letterD
            scaleGlVertex( -3.30696, x,   4.31878, y);  //  1
            scaleGlVertex( -3.3428, x,   -4.31878, y);  //  2
            scaleGlVertex( 0.9425, x,   -4.28164, y);  //  3
            scaleGlVertex( 3.31415, x,   -1.42302, y);  //  4
            scaleGlVertex( 3.3428, x,   1.66626, y);  //  5
            scaleGlVertex( 0.70825, x,   4.31317, y);  //  6
            scaleGlVertex( -3.22083, x,   4.29495, y);  //  7
        glEnd();
    break;

    case 3: // E
        glBegin(GL_LINE_STRIP);
            //  letterE2
            scaleGlVertex( 2.966065, x,   4.416055, y);  //  1
            scaleGlVertex( -3.007275, x,   4.403495, y);  //  2
            scaleGlVertex( -3.037615, x,   -4.416055, y);  //  3
            scaleGlVertex( 3.037615, x,   -4.415435, y);  //  4
        glEnd();
        glBegin(GL_LINES);
            scaleGlVertex( 3.011705, x,   0.197675, y);  //  5
            scaleGlVertex( -2.990845, x,   0.196615, y);  //  6
        glEnd();
    break;

    case 4: // F
        glBegin(GL_LINE_STRIP);
            //  letterF2
            scaleGlVertex( -2.55172, x,   -4.434285, y);  //  1
            scaleGlVertex( -2.51956, x,   4.433665, y);  //  2
            scaleGlVertex( 2.39942, x,   4.434285, y);  //  3
        glEnd();
        glBegin(GL_LINES);
            scaleGlVertex( 2.58143, x,   0.244465, y);  //  4
            scaleGlVertex( -2.58143, x,   0.243405, y);  //  5

        glEnd();
    break;

    case 5:
        glBegin(GL_LINE_STRIP);
            //  letterG
            scaleGlVertex( 0.58123, x,   -0.34005, y);  //  1
            scaleGlVertex( 3.66047, x,   -0.48722, y);  //  2
            scaleGlVertex( 3.70461, x,   -3.23595, y);  //  3
            scaleGlVertex( 1.96234, x,   -4.41694, y);  //  4
            scaleGlVertex( -0.96712, x,   -4.57846, y);  //  5
            scaleGlVertex( -3.70461, x,   -2.29011, y);  //  6
            scaleGlVertex( -3.67245, x,   2.14034, y);  //  7
            scaleGlVertex( -1.25347, x,   4.57846, y);  //  8
            scaleGlVertex( 1.90018, x,   4.55293, y);  //  9
            scaleGlVertex( 3.54236, x,   2.38612, y);  //  10
        glEnd();
    break;

    case 6: // A
        glBegin(GL_LINE_STRIP);
            //  letterA2
            scaleGlVertex( -3.91146, x,   -4.907395, y);  //  1
            scaleGlVertex( 0.06571, x,   4.907395, y);  //  2
            scaleGlVertex( 3.91146, x,   -4.803315, y);  //  3
        glEnd();
        glBegin(GL_LINES);
            scaleGlVertex( 2.60111, x,   -1.400435, y);  //  4
            scaleGlVertex( -2.56175, x,   -1.357305, y);  //  5

        glEnd();
    break;

    case 7:
        glBegin(GL_LINE_STRIP);
            //  letterB
            scaleGlVertex( 1.038555, x,   0.105285, y);  //  1
            scaleGlVertex( -3.001935, x,   0.121925, y);  //  2
            scaleGlVertex( -3.027615, x,   4.417905, y);  //  3
            scaleGlVertex( 1.325925, x,   4.451255, y);  //  4
            scaleGlVertex( 2.737985, x,   3.048615, y);  //  5
            scaleGlVertex( 2.721765, x,   1.366235, y);  //  6
            scaleGlVertex( 1.022635, x,   0.120235, y);  //  7
            scaleGlVertex( 3.026015, x,   -1.282295, y);  //  8
            scaleGlVertex( 3.027615, x,   -3.046285, y);  //  9
            scaleGlVertex( 1.176815, x,   -4.451255, y);  //  10
            scaleGlVertex( -2.981475, x,   -4.445735, y);  //  11
            scaleGlVertex( -3.021355, x,   0.176035, y);  //  12
        glEnd();
    break;


    default:
        glBegin(GL_LINES);
            glVertex2f(  3 + x,   -15  + y);  //  1
            glVertex2f(  3 + x,   8    + y);  //  2

            glVertex2f( -3 + x,   -8   + y);  //  3
            glVertex2f( -3 + x,   15   + y);  //  4

            glVertex2f(  3 + x,   8    + y);  //  5
            glVertex2f( -3 + x,   2    + y);  //  6

            glVertex2f(  3 + x,   -2   + y);  //  7
            glVertex2f( -3 + x,   -8   + y);  //  8
        glEnd();
    break;
    }
}

void CDraw::drawStaveNoteName(CSymbol symbol, float x, float y)
{
    if ( symbol.getNoteIndex() + 1 != symbol.getNoteTotal())
        return;
    if (m_settings->showNoteNames() == false)
        return;
    y += CStavePos::getVerticalNoteSpacing()*2 +3;
    drawNoteName(symbol.getNote(), x, y, true);
}

void CDraw::checkAccidental(CSymbol symbol, float x, float y)
{
    int accidental;
    const int xGap = 16;

    accidental = symbol.getStavePos().getAccidental();


    if (symbol.getAccidentalModifer() == PB_ACCIDENTAL_MODIFER_suppress)
        accidental = 0; // Suppress the accidental if it is the same bar

    if (symbol.getAccidentalModifer() == PB_ACCIDENTAL_MODIFER_force)
    {
        // Force the display of an accidental including naturals if it is the same bar
        accidental = CStavePos::midiNote2Name(symbol.getNote()).accidental;
        if (accidental == 0)
            accidental = 2;
    }

    if (accidental != 0)
    {
        //drColour (Cfg::lineColour());
        if (accidental == 1)
            drawSymbol(CSymbol(PB_SYMBOL_sharp, symbol.getStavePos()), x - xGap, y);
        else if (accidental == -1)
            drawSymbol(CSymbol(PB_SYMBOL_flat, symbol.getStavePos()), x - xGap, y);
        else
            drawSymbol(CSymbol(PB_SYMBOL_natural, symbol.getStavePos()), x - xGap, y);
    }
}

void CDraw::drawSymbol(CSymbol symbol, float x, float y)
{
    CColour colour = symbol.getColour();
    bool playable = true;

    if (m_displayHand != symbol.getHand() && m_displayHand != PB_PART_both)
    {
        if (colour == Cfg::noteColour())
            colour = Cfg::noteColourDim();
        if (colour == Cfg::staveColour())
            colour = Cfg::staveColourDim();
        playable = false;
    }

    switch (symbol.getType())
    {
         case PB_SYMBOL_gClef: // The Treble Clef
            y += 4;
            drColour(colour);
            glLineWidth (3.0);
            glBegin(GL_LINE_STRIP);
            glVertex2f( -0.011922  + x,   -16.11494  + y);  //  1
            glVertex2f( -3.761922  + x,   -12.48994  + y);  //  2
            glVertex2f( -4.859633  + x,   -8.85196  + y);  //  3
            glVertex2f( -4.783288  + x,   -5.42815  + y);  //  4
            glVertex2f( -0.606711  + x,   -1.11108  + y);  //  5
            glVertex2f( 5.355545  + x,   0.48711  + y);  //  6
            glVertex2f( 10.641104  + x,   -1.6473  + y);  //  7
            glVertex2f( 14.293812  + x,   -6.18241  + y);  //  8
            glVertex2f( 14.675578  + x,   -11.42744  + y);  //  9
            glVertex2f( 12.550578  + x,   -17.30244  + y);  //  10
            glVertex2f( 7.912166  + x,   -20.944  + y);  //  11
            glVertex2f( 3.049705  + x,   -21.65755  + y);  //  12
            glVertex2f( -1.711005  + x,   -21.36664  + y);  //  13
            glVertex2f( -6.283661  + x,   -19.66739  + y);  //  14
            glVertex2f( -10.123329  + x,   -16.79162  + y);  //  15
            glVertex2f( -13.363008  + x,   -12.28184  + y);  //  16
            glVertex2f( -14.675578  + x,   -5.79969  + y);  //  17
            glVertex2f( -13.66821  + x,   0.20179  + y);  //  18
            glVertex2f( -10.385341  + x,   6.27562  + y);  //  19
            glVertex2f( 5.539491  + x,   20.32671  + y);  //  20
            glVertex2f( 10.431588  + x,   28.20584  + y);  //  21
            glVertex2f( 11.00141  + x,   34.71585  + y);  //  22
            glVertex2f( 9.204915  + x,   39.62875  + y);  //  23
            glVertex2f( 7.854166  + x,   42.08262  + y);  //  24
            glVertex2f( 5.481415  + x,   42.66649  + y);  //  25
            glVertex2f( 3.57972  + x,   41.4147  + y);  //  26
            glVertex2f( 1.507889  + x,   37.35642  + y);  //  27
            glVertex2f( -0.381338  + x,   31.14317  + y);  //  28
            glVertex2f( -0.664306  + x,   25.51354  + y);  //  29
            glVertex2f( 8.296044  + x,   -32.22694  + y);  //  30
            glVertex2f( 8.050507  + x,   -36.6687  + y);  //  31
            glVertex2f( 6.496615  + x,   -39.52999  + y);  //  32
            glVertex2f( 3.368583  + x,   -41.7968  + y);  //  33
            glVertex2f( 0.253766  + x,   -42.66649  + y);  //  34
            glVertex2f( -3.599633  + x,   -42.23514  + y);  //  35
            glVertex2f( -8.098754  + x,   -39.46637  + y);  //  36
            glVertex2f( -9.463279  + x,   -35.49796  + y);  //  37
            glVertex2f( -7.08037  + x,   -31.36512  + y);  //  38
            glVertex2f( -3.336421  + x,   -31.14057  + y);  //  39
            glVertex2f( -1.360313  + x,   -34.07738  + y);  //  40
            glVertex2f( -1.608342  + x,   -37.11828  + y);  //  41
            glVertex2f( -5.729949  + x,   -39.24759  + y);  //  42
            glVertex2f( -7.480646  + x,   -36.2136  + y);  //  43
            glVertex2f( -6.826918  + x,   -33.36919  + y);  //  44
            glVertex2f( -4.069083  + x,   -32.9226  + y);  //  45
            glVertex2f( -3.040669  + x,   -34.433  + y);  //  46
            glVertex2f( -3.737535  + x,   -36.38759  + y);  //  47
            glVertex2f( -5.496558  + x,   -36.97633  + y);  //  48
            glVertex2f( -5.295932  + x,   -34.01951  + y);  //  49

            glEnd();

            break;

       case PB_SYMBOL_fClef: // The Base Clef
            drColour(colour);
            glLineWidth (3.0);
            glBegin(GL_LINE_STRIP);
                glVertex2f( -15.370325  + x,   -17.42068  + y);  //  1
                glVertex2f( -7.171025  + x,   -13.75432  + y);  //  2
                glVertex2f( -2.867225  + x,   -10.66642  + y);  //  3
                glVertex2f( 0.925165  + x,   -7.03249  + y);  //  4
                glVertex2f( 4.254425  + x,   -0.65527  + y);  //  5
                glVertex2f( 4.762735  + x,   7.77848  + y);  //  6
                glVertex2f( 2.693395  + x,   13.92227  + y);  //  7
                glVertex2f( -1.207935  + x,   16.80317  + y);  //  8
                glVertex2f( -5.526425  + x,   17.42068  + y);  //  9
                glVertex2f( -10.228205  + x,   15.65609  + y);  //  10
                glVertex2f( -13.453995  + x,   10.7128  + y);  //  11
                glVertex2f( -13.133655  + x,   5.43731  + y);  //  12
                glVertex2f( -9.475575  + x,   3.00714  + y);  //  13
                glVertex2f( -5.846445  + x,   4.72159  + y);  //  14
                glVertex2f( -5.395545  + x,   9.72918  + y);  //  15
                glVertex2f( -8.850025  + x,   11.64372  + y);  //  16
                glVertex2f( -11.519385  + x,   10.35816  + y);  //  17
                glVertex2f( -11.706365  + x,   6.8704  + y);  //  18
                glVertex2f( -9.463505  + x,   5.01391  + y);  //  19
                glVertex2f( -7.172075  + x,   5.81649  + y);  //  20
                glVertex2f( -7.189565  + x,   8.62975  + y);  //  21
                glVertex2f( -9.175055  + x,   9.82019  + y);  //  22
                glVertex2f( -10.696425  + x,   8.08395  + y);  //  23
                glVertex2f( -8.843065  + x,   6.66726  + y);  //  24
                glVertex2f( -8.995775  + x,   8.71136  + y);  //  25
            glEnd();

            glBegin(GL_POLYGON);
                glVertex2f( 10  + x,   14  + y);  //  26
                glVertex2f( 14  + x,   14 + y);  //  27
                glVertex2f( 14 + x,    10  + y);  //  28
                glVertex2f( 10  + x,   10  + y);  //  29
                glVertex2f( 10  + x,   14  + y);  //  30
            glEnd();

            glBegin(GL_POLYGON);
                glVertex2f( 10 + x,    4  + y);  //  31
                glVertex2f( 14  + x,   4  + y);  //  32
                glVertex2f( 14  + x,   0  + y);  //  33
                glVertex2f( 10 + x,    0  + y);  //  34
                glVertex2f( 10 + x,    4  + y);  //  35
           glEnd();

          break;

        case PB_SYMBOL_note:
            //ppLogTrace("PB_SYMBOL_note x %f y %f", x, y);
            if (!CChord::isNotePlayable(symbol.getNote(), 0))
            {
                colour = Cfg::noteColourDim();
                playable = false;
            }
            drawStaveExtentsion(symbol, x, 16, playable);
            drColour(colour);
            glBegin(GL_POLYGON);
            glVertex2f(-7.0 + x,  2.0 + y); // 1
            glVertex2f(-5.0 + x,  4.0 + y); // 2
            glVertex2f(-1.0 + x,  6.0 + y); // 3
            glVertex2f( 4.0 + x,  6.0 + y); // 4
            glVertex2f( 7.0 + x,  4.0 + y); // 5
            glVertex2f( 7.0 + x,  1.0 + y); // 6
            glVertex2f( 6.0 + x, -2.0 + y); // 7
            glVertex2f( 4.0 + x, -4.0 + y); // 8
            glVertex2f( 0.0 + x, -6.0 + y); // 9
            glVertex2f(-4.0 + x, -6.0 + y); // 10
            glVertex2f(-8.0 + x, -3.0 + y); // 11
            glVertex2f(-8.0 + x, -0.0 + y); // 12
            glEnd();
            checkAccidental(symbol, x, y);
            break;

        case PB_SYMBOL_drum:
            if (!CChord::isNotePlayable(symbol.getNote(), 0))
                colour = Cfg::noteColourDim();
            drColour(colour);
            glLineWidth (3.0);
            glBegin(GL_LINES);
            glVertex2f( 5.0 + x,-5.0 + y);
            glVertex2f(-5.0 + x, 5.0 + y);
            glVertex2f(-5.0 + x,-5.0 + y);
            glVertex2f( 5.0 + x, 5.0 + y);
            glEnd();
            checkAccidental(symbol, x, y);
            break;

        case PB_SYMBOL_sharp:
            glLineWidth (2.0);
            glBegin(GL_LINES);
            glVertex2f(-2.0 + x, -14.0 + y);
            glVertex2f(-2.0 + x,  14.0 + y);

            glVertex2f( 2.0 + x, -13.0 + y);
            glVertex2f( 2.0 + x,  15.0 + y);

            glVertex2f(-5.0 + x,   4.0 + y);
            glVertex2f( 5.0 + x,   7.0 + y);

            glVertex2f(-5.0 + x,  -6.0 + y);
            glVertex2f( 5.0 + x,  -3.0 + y);
            glEnd();
            break;

         case PB_SYMBOL_flat:
            glLineWidth (2.0);
            glBegin(GL_LINE_STRIP);
            glVertex2f(-4.0 + x, 17.0 + y);  // 1
            glVertex2f(-4.0 + x, -6.0 + y);  // 2
            glVertex2f( 2.0 + x, -2.0 + y);  // 3
            glVertex2f( 5.0 + x,  2.0 + y);  // 4
            glVertex2f( 5.0 + x,  4.0 + y);  // 5
            glVertex2f( 3.0 + x,  5.0 + y);  // 6
            glVertex2f( 0.0 + x,  5.0 + y);  // 7
            glVertex2f(-4.0 + x,  2.0 + y);  // 8
            glEnd();
            break;

         case PB_SYMBOL_natural:
            glLineWidth (2.0);
            glBegin(GL_LINES);
                glVertex2f(  3 + x,   -15  + y);  //  1
                glVertex2f(  3 + x,   8  + y);  //  2

                glVertex2f( -3 + x,   -8  + y);  //  3
                glVertex2f( -3 + x,   15  + y);  //  4

                glVertex2f(  3 + x,   8  + y);  //  5
                glVertex2f( -3 + x,   2  + y);  //  6

                glVertex2f(  3 + x,   -2  + y);  //  7
                glVertex2f( -3 + x,   -8  + y);  //  8
            glEnd();
            break;

        case PB_SYMBOL_barLine:
            x += BEAT_MARKER_OFFSET * HORIZONTAL_SPACING_FACTOR; // the beat markers where entered early so now move them correctly
            glLineWidth (4.0);
            drColour ((m_displayHand == PB_PART_left) ? Cfg::staveColourDim() : Cfg::staveColour());
            oneLine(x, CStavePos(PB_PART_right, 4).getPosYRelative(), x, CStavePos(PB_PART_right, -4).getPosYRelative());
            drColour ((m_displayHand == PB_PART_right) ? Cfg::staveColourDim() : Cfg::staveColour());
            oneLine(x, CStavePos(PB_PART_left, 4).getPosYRelative(), x, CStavePos(PB_PART_left, -4).getPosYRelative());
            break;

        case PB_SYMBOL_barMarker:
            x += BEAT_MARKER_OFFSET * HORIZONTAL_SPACING_FACTOR; // the beat markers where entered early so now move them correctly
            glLineWidth (5.0);
            drColour(Cfg::barMarkerColour());
            oneLine(x, CStavePos(PB_PART_right, m_beatMarkerHeight).getPosYRelative(), x, CStavePos(PB_PART_left, -m_beatMarkerHeight).getPosYRelative());
            glDisable (GL_LINE_STIPPLE);
            break;

        case PB_SYMBOL_beatMarker:
            x += BEAT_MARKER_OFFSET * HORIZONTAL_SPACING_FACTOR; // the beat markers where entered early so now move them correctly
            glLineWidth (4.0);
            drColour(Cfg::beatMarkerColour());
            oneLine(x, CStavePos(PB_PART_right, m_beatMarkerHeight).getPosYRelative(), x, CStavePos(PB_PART_left, -m_beatMarkerHeight).getPosYRelative());
            glDisable (GL_LINE_STIPPLE);
            break;

         case PB_SYMBOL_playingZone:
            {
                float topY = CStavePos(PB_PART_right, m_beatMarkerHeight).getPosY();
                float bottomY = CStavePos(PB_PART_left, -m_beatMarkerHeight).getPosY();
                float early = Cfg::playZoneEarly() * HORIZONTAL_SPACING_FACTOR;
                float late = Cfg::playZoneLate() * HORIZONTAL_SPACING_FACTOR;
                //glColor3f (0.7, 1.0, 0.7);
                glColor3f (0.0, 0.0, 0.3);
                glRectf(x-late, topY, x + early, bottomY);
                glLineWidth (2.0);
                glColor3f (0.0, 0.0, 0.8);
                oneLine(x, topY, x, bottomY );
                glLineWidth (1.0);
                glColor3f (0.0, 0.0, 0.6);
                oneLine(x-late, topY, x-late, bottomY );
                oneLine(x+early, topY, x+early, bottomY );
            }
            break;

       default:
            ppDEBUG(("ERROR drawSymbol unhandled symbol\n"));
            break;
    }

    if (symbol.getType() == PB_SYMBOL_note)
    {
        float pianistX = symbol.getPianistTiming();
        if ( pianistX != NOT_USED)
        {
            pianistX =  x + pianistX * HORIZONTAL_SPACING_FACTOR;
            drColour(CColour(1.0, 1.0, 1.0));
            glLineWidth (2.0);
            glBegin(GL_LINES);
            glVertex2f( 4.0 + pianistX, 4.0 + y);
            glVertex2f(-5.0 + pianistX,-5.0 + y);
            glVertex2f( 4.0 + pianistX,-4.0 + y); // draw pianist note timing markers
            glVertex2f(-5.0 + pianistX, 5.0 + y);
            glEnd();
        }
        if ( playable )
            drawStaveNoteName(symbol, x, y);
    }
}

void CDraw::drawSymbol(CSymbol symbol, float x)
{
    drawSymbol(symbol, x, symbol.getStavePos().getPosY());
}


void CDraw::drawStaves(float startX, float endX)
{
    int i;

    glLineWidth (Cfg::staveThickness());

    /* select colour for all lines  */
    drColour ((m_displayHand != PB_PART_left) ? Cfg::staveColour() : Cfg::staveColourDim());
    glBegin(GL_LINES);

    for (i = -4; i <= 4; i+=2 )
    {
        CStavePos pos = CStavePos(PB_PART_right, i);
        glVertex2f (startX, pos.getPosY());
        glVertex2f (endX, pos.getPosY());
    }
    drColour ((m_displayHand != PB_PART_right) ? Cfg::staveColour() : Cfg::staveColourDim());
    for (i = -4; i <= 4; i+=2 )
    {
        CStavePos pos = CStavePos(PB_PART_left, i);
        glVertex2f (startX, pos.getPosY());
        glVertex2f (endX,   pos.getPosY());
    }
    glEnd();
}


void CDraw::drawKeySignature(int key)
{
    const int sharpLookUpRight[] = { 4, 1, 5, 2,-1, 3, 0};
    const int sharpLookUpLeft[]  = { 2,-1, 3, 0,-3, 1,-2};
    const int flatLookUpRight[]  = { 0, 3,-1, 2,-2, 1,-3};
    const int flatLookUpLeft[]   = {-2, 1,-3, 0,-4,-1,-5};
    const int gapX = 11;
    CStavePos pos;
    size_t i;

    if (key == NOT_USED)
        return;

    i = 0;
    while (key != 0 )
    {
        if (key > 0)
        {
            if (i < arraySize(sharpLookUpRight))
            {
                drColour ((m_displayHand != PB_PART_left) ? Cfg::noteColour() : Cfg::noteColourDim());
                pos = CStavePos(PB_PART_right, sharpLookUpRight[i]);
                drawSymbol( CSymbol(PB_SYMBOL_sharp, pos), Cfg::keySignatureX() + gapX*i );
            }
            if (i < arraySize(sharpLookUpLeft))
            {
                drColour ((m_displayHand != PB_PART_right) ? Cfg::noteColour() : Cfg::noteColourDim());
                pos = CStavePos(PB_PART_left, sharpLookUpLeft[i]);
                drawSymbol( CSymbol(PB_SYMBOL_sharp, pos), Cfg::keySignatureX() + gapX*i );
            }
            key--;
        }
        else
        {
            if (i < arraySize(flatLookUpRight))
            {
                drColour ((m_displayHand != PB_PART_left) ? Cfg::noteColour() : Cfg::noteColourDim());
                pos = CStavePos(PB_PART_right, flatLookUpRight[i]);
                drawSymbol( CSymbol(PB_SYMBOL_flat, pos), Cfg::keySignatureX() + gapX*i );
            }
            if (i < arraySize(flatLookUpLeft))
            {
                drColour ((m_displayHand != PB_PART_right) ? Cfg::noteColour() : Cfg::noteColourDim());
                pos = CStavePos(PB_PART_left, flatLookUpLeft[i]);
                drawSymbol( CSymbol(PB_SYMBOL_flat, pos), Cfg::keySignatureX() + gapX*i );
            }

            key++;
        }
        i++;
    }
}
