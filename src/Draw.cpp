/*********************************************************************************/
/*!
@file           Draw.cpp



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

#include "Draw.h"
#include "Cfg.h"
#include "Settings.h"
#include "Notation.h"

typedef unsigned int guint;
typedef unsigned char guint8;

whichPart_t CDraw::m_displayHand;
int CDraw::m_forceCompileRedraw;

CDraw::CDraw(CSettings* settings)
#ifndef NO_USE_FTGL
    :font(nullptr)
#endif
{
    m_settings = settings;
    m_displayHand = PB_PART_both;
    m_forceCompileRedraw = 1;
    m_scrollProperties = &m_scrollPropertiesHorizontal;
#ifndef NO_USE_FTGL
    QStringList listPathFonts;

    listPathFonts.append(Util::dataDir()+"/fonts/DejaVuSans.ttf");
    listPathFonts.append(QApplication::applicationDirPath() + "/fonts/DejaVuSans.ttf");
    listPathFonts.append(QApplication::applicationDirPath() + "/../Resources/fonts/DejaVuSans.ttf");
    listPathFonts.append("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    listPathFonts.append("/usr/share/fonts/dejavu/DejaVuSans.ttf");
    listPathFonts.append("/usr/share/fonts/TTF/dejavu/DejaVuSans.ttf");
    listPathFonts.append("/usr/share/fonts/TTF/DejaVuSans.ttf");
    listPathFonts.append("/usr/share/fonts/truetype/DejaVuSans.ttf");
    listPathFonts.append("/usr/local/share/fonts/dejavu/DejaVuSans.ttf");

    for (int i=0;i<listPathFonts.size();i++){
        QFile file(listPathFonts.at(i));
        if (file.exists()){
            font = new FTBitmapFont(listPathFonts.at(i).toStdString().c_str());
            break;
        }
    }
    if (font==nullptr){
        ppLogError("Font DejaVuSans.ttf was not found !");
        exit(0);
    }
    refreshFontSize();
#endif
}

#ifndef NO_USE_FTGL
void CDraw::refreshFontSize() {
    int fontSize = static_cast<int>(FONT_SIZE * m_settings->scalingFactor());
    font->FaceSize(fontSize);
}
#endif

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
        drColor(Cfg::staveColor());
    else
        drColor(Cfg::staveColorDim());

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

#ifndef NO_USE_FTGL
void CDraw::renderText(float x, float y, const char* s)
{
  double w = font->Advance(s);
  double h = font->Descender();
  glRasterPos2f(x - w/2, y - h);
  font->Render(s);
}
#endif

void CDraw::drawNoteName(int midiNote, float x, float y, int type)
{
    // Ignore note that are too high
    if (midiNote > MIDI_TOP_C + 6)
        return;

    staveLookup_t item = CStavePos::midiNote2Name(midiNote);

    drColor(Cfg::noteNameColor());

    glLineWidth (1.0);

#ifdef NO_USE_FTGL
    float sf = m_settings->scalingFactor();
    if (item.accidental != 0)
      {
          const float accidentalOffset = 10;
          x += accidentalOffset/2;
          if (item.accidental == 1)
          {
              glBegin(GL_LINES);
                  //  letterSharp4
                  scaleGlVertex( -1.317895, x * sf,   5.794585, y * sf);  //  1
                  scaleGlVertex( -1.265845, x * sf,   -6.492455, y * sf);  //  2
                  scaleGlVertex( 1.252305, x * sf,   6.492455, y * sf);  //  3
                  scaleGlVertex( 1.322655, x * sf,   -5.422335, y * sf);  //  4
                  scaleGlVertex( -2.645765, x * sf,   1.967805, y * sf);  //  5
                  scaleGlVertex( 2.648325, x * sf,   3.625485, y * sf);  //  6
                  scaleGlVertex( -2.648325, x * sf,   -3.306965, y * sf);  //  7
                  scaleGlVertex( 2.596205, x * sf,   -1.675765, y * sf);  //  8
              glEnd();

          }
          else
          {
              glBegin(GL_LINE_STRIP);
                  //  letterFlat
                  scaleGlVertex( -2.52933, x * sf,   6.25291, y * sf);  //  1
                  scaleGlVertex( -2.50344, x * sf,   -6.25291, y * sf);  //  2
                  scaleGlVertex( 0.76991, x * sf,   -3.63422, y * sf);  //  3
                  scaleGlVertex( 2.07925, x * sf,   -1.67021, y * sf);  //  4
                  scaleGlVertex( 2.52933, x * sf,   0.25288, y * sf);  //  5
                  scaleGlVertex( 1.42458, x * sf,   1.07122, y * sf);  //  6
                  scaleGlVertex( -0.53943, x * sf,   0.90755, y * sf);  //  7
                  scaleGlVertex( -2.46252, x * sf,   -1.01554, y * sf);  //  8
                  scaleGlVertex( -2.50344, x * sf,   -1.67021, y * sf);  //  9
              glEnd();

          }
          x -= accidentalOffset;
      }

      switch(item.pianoNote)
      {
      case 1:
          glBegin(GL_LINE_STRIP);
              //  letterC
              scaleGlVertex( 3.513445, x * sf,   2.17485, y * sf);  //  1
              scaleGlVertex( 1.880175, x * sf,   4.47041, y * sf);  //  2
              scaleGlVertex( -1.598825, x * sf,   4.4321, y * sf);  //  3
              scaleGlVertex( -3.692215, x * sf,   2.26571, y * sf);  //  4
              scaleGlVertex( -3.702055, x * sf,   -1.88958, y * sf);  //  5
              scaleGlVertex( -1.630675, x * sf,   -4.47041, y * sf);  //  6
              scaleGlVertex( 1.932545, x * sf,   -4.44064, y * sf);  //  7
              scaleGlVertex( 3.702055, x * sf,   -1.88554, y * sf);  //  8
          glEnd();
      break;

      case 2:
          glBegin(GL_LINE_STRIP);
              //  letterD
              scaleGlVertex( -3.30696, x * sf,   4.31878, y * sf);  //  1
              scaleGlVertex( -3.3428, x * sf,   -4.31878, y * sf);  //  2
              scaleGlVertex( 0.9425, x * sf,   -4.28164, y * sf);  //  3
              scaleGlVertex( 3.31415, x * sf,   -1.42302, y * sf);  //  4
              scaleGlVertex( 3.3428, x * sf,   1.66626, y * sf);  //  5
              scaleGlVertex( 0.70825, x * sf,   4.31317, y * sf);  //  6
              scaleGlVertex( -3.22083, x * sf,   4.29495, y * sf);  //  7
          glEnd();
      break;

      case 3: // E
          glBegin(GL_LINE_STRIP);
              //  letterE2
              scaleGlVertex( 2.966065, x * sf,   4.416055, y * sf);  //  1
              scaleGlVertex( -3.007275, x * sf,   4.403495, y * sf);  //  2
              scaleGlVertex( -3.037615, x * sf,   -4.416055, y * sf);  //  3
              scaleGlVertex( 3.037615, x * sf,   -4.415435, y * sf);  //  4
          glEnd();
          glBegin(GL_LINES);
              scaleGlVertex( 3.011705, x * sf,   0.197675, y * sf);  //  5
              scaleGlVertex( -2.990845, x * sf,   0.196615, y * sf);  //  6
          glEnd();
      break;

      case 4: // F
          glBegin(GL_LINE_STRIP);
              //  letterF2
              scaleGlVertex( -2.55172, x * sf,   -4.434285, y * sf);  //  1
              scaleGlVertex( -2.51956, x * sf,   4.433665, y * sf);  //  2
              scaleGlVertex( 2.39942, x * sf,   4.434285, y * sf);  //  3
          glEnd();
          glBegin(GL_LINES);
              scaleGlVertex( 2.58143, x * sf,   0.244465, y * sf);  //  4
              scaleGlVertex( -2.58143, x * sf,   0.243405, y * sf);  //  5

          glEnd();
      break;

      case 5:
          glBegin(GL_LINE_STRIP);
              //  letterG
              scaleGlVertex( 0.58123, x * sf,   -0.34005, y * sf);  //  1
              scaleGlVertex( 3.66047, x * sf,   -0.48722, y * sf);  //  2
              scaleGlVertex( 3.70461, x * sf,   -3.23595, y * sf);  //  3
              scaleGlVertex( 1.96234, x * sf,   -4.41694, y * sf);  //  4
              scaleGlVertex( -0.96712, x * sf,   -4.57846, y * sf);  //  5
              scaleGlVertex( -3.70461, x * sf,   -2.29011, y * sf);  //  6
              scaleGlVertex( -3.67245, x * sf,   2.14034, y * sf);  //  7
              scaleGlVertex( -1.25347, x * sf,   4.57846, y * sf);  //  8
              scaleGlVertex( 1.90018, x * sf,   4.55293, y * sf);  //  9
              scaleGlVertex( 3.54236, x * sf,   2.38612, y * sf);  //  10
          glEnd();
      break;

      case 6: // A
          glBegin(GL_LINE_STRIP);
              //  letterA2
              scaleGlVertex( -3.91146, x * sf,   -4.907395, y * sf);  //  1
              scaleGlVertex( 0.06571, x * sf,   4.907395, y * sf);  //  2
              scaleGlVertex( 3.91146, x * sf,   -4.803315, y * sf);  //  3
          glEnd();
          glBegin(GL_LINES);
              scaleGlVertex( 2.60111, x * sf,   -1.400435, y * sf);  //  4
              scaleGlVertex( -2.56175, x * sf,   -1.357305, y * sf);  //  5

          glEnd();
      break;

      case 7:
          glBegin(GL_LINE_STRIP);
              //  letterB
              scaleGlVertex( 1.038555, x * sf,   0.105285, y * sf);  //  1
              scaleGlVertex( -3.001935, x * sf,   0.121925, y * sf);  //  2
              scaleGlVertex( -3.027615, x * sf,   4.417905, y * sf);  //  3
              scaleGlVertex( 1.325925, x * sf,   4.451255, y * sf);  //  4
              scaleGlVertex( 2.737985, x * sf,   3.048615, y * sf);  //  5
              scaleGlVertex( 2.721765, x * sf,   1.366235, y * sf);  //  6
              scaleGlVertex( 1.022635, x * sf,   0.120235, y * sf);  //  7
              scaleGlVertex( 3.026015, x * sf,   -1.282295, y * sf);  //  8
              scaleGlVertex( 3.027615, x * sf,   -3.046285, y * sf);  //  9
              scaleGlVertex( 1.176815, x * sf,   -4.451255, y * sf);  //  10
              scaleGlVertex( -2.981475, x * sf,   -4.445735, y * sf);  //  11
              scaleGlVertex( -3.021355, x * sf,   0.176035, y * sf);  //  12
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
#else
    const QChar flat = QChar(0x266D);
    const QChar natural = QChar(0x266E);
    const QChar sharp = QChar(0x266F);
    const QString n[7] =
     {
      tr("C"),
      tr("D"),
      tr("E"),
      tr("F"),
      tr("G"),
      tr("A"),
      tr("B")
     };

    if(0<item.pianoNote && item.pianoNote < 8)
     {
      QString accident = QString("");
      switch(item.accidental)
       {
        case -1:
          accident = QString(flat);
          break;
        case 1:
          accident = QString(sharp);
          break;
        case 2:
          accident = QString(natural);
          break;
       }
      QString note = n[item.pianoNote-1] + accident;
      renderText(x, y, note.toUtf8().data());
     }
#endif
}

void CDraw::drawStaveNoteName(CSymbol symbol, float x, float y)
{
    if ( symbol.getNoteIndex() + 1 != symbol.getNoteTotal())
        return;
    if (m_settings->showNoteNames() == false)
        return;
    y += CStavePos::verticalNoteSpacing()*2 +3;
    drawNoteName(symbol.getNote(), x, y, true);
}

void CDraw::checkAccidental(CSymbol symbol, float x, float y)
{
    int accidental;
    const int xGap = 16 * m_settings->scalingFactor();

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
        //drColor (Cfg::lineColor());
        if (accidental == 1)
            drawSymbol(CSymbol(PB_SYMBOL_sharp, symbol.getStavePos()), x - xGap, y);
        else if (accidental == -1)
            drawSymbol(CSymbol(PB_SYMBOL_flat, symbol.getStavePos()), x - xGap, y);
        else
            drawSymbol(CSymbol(PB_SYMBOL_natural, symbol.getStavePos()), x - xGap, y);
    }
}

bool CDraw::drawNote(CSymbol* symbol, float x, float y, CSlot* slot, CColor color, bool playable)
{
    const float stemLength  = 34.0 * m_settings->scalingFactor();
    float noteWidth  = 6.0 * m_settings->scalingFactor();

    //ppLogTrace("PB_SYMBOL_noteHead x %f y %f", x, y);
    if (!CChord::isNotePlayable(symbol->getNote(), 0))
    {
        color = Cfg::noteColorDim();
        playable = false;
    }
    drawStaveExtentsion(*symbol, x, 16, playable);
    drColor(color);
    bool solidNoteHead = false;
    bool showNoteStem = false;
    int stemFlagCount = 0;

    if (symbol->getType() <= PB_SYMBOL_semiquaver)
        stemFlagCount = 2;
    else if (symbol->getType() <= PB_SYMBOL_quaver)
        stemFlagCount = 1;

    if (symbol->getType() <= PB_SYMBOL_crotchet)
        solidNoteHead = true;

    if (symbol->getType() <= PB_SYMBOL_minim)
        showNoteStem = true;

    if (showNoteStem)
    {
        if (!solidNoteHead)
            noteWidth += 1.0;
        glLineWidth(2.0);
        glBegin(GL_LINE_STRIP);
            glVertex2f(noteWidth + x,  0.0 + y); // 1
            glVertex2f(noteWidth + x, stemLength + y); // 2
        glEnd();
    }

    int offset = stemLength;
    while (stemFlagCount>0)
    {

        glLineWidth(2.0);
        glBegin(GL_LINE_STRIP);
            glVertex2f(noteWidth + x, offset  + y); // 1
            glVertex2f(noteWidth + 8 + x, offset - 16 + y); // 2
        glEnd();
        offset -= 8;
        stemFlagCount--;
    }

    if (solidNoteHead)
    {
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
    }
    else
    {
        glLineWidth(2.0);
        glBegin(GL_LINE_STRIP);
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
    }

    checkAccidental(*symbol, x, y);

    return playable;
}

void CDraw::drawSymbol(CSymbol symbol, float x, float y, CSlot* slot)
{
    float sf = m_settings->scalingFactor();
    CColor color = symbol.getColor();
    bool playable = true;

    if (m_displayHand != symbol.getHand() && m_displayHand != PB_PART_both)
    {
        if (color == Cfg::noteColor())
            color = Cfg::noteColorDim();
        if (color == Cfg::staveColor())
            color = Cfg::staveColorDim();
        playable = false;
    }

    switch (symbol.getType())
    {
         case PB_SYMBOL_gClef: // The Treble Clef
            y += 4;
            drColor(color);
            glLineWidth (3.0);
            glBegin(GL_LINE_STRIP);
            glVertex2f( -0.011922  * sf + x,   -16.11494  * sf + y);  //  1
            glVertex2f( -3.761922  * sf + x,   -12.48994  * sf + y);  //  2
            glVertex2f( -4.859633  * sf + x,   -8.85196  * sf + y);  //  3
            glVertex2f( -4.783288  * sf + x,   -5.42815  * sf + y);  //  4
            glVertex2f( -0.606711  * sf + x,   -1.11108  * sf + y);  //  5
            glVertex2f( 5.355545  * sf + x,   0.48711  * sf + y);  //  6
            glVertex2f( 10.641104  * sf + x,   -1.6473  * sf + y);  //  7
            glVertex2f( 14.293812  * sf + x,   -6.18241  * sf + y);  //  8
            glVertex2f( 14.675578  * sf + x,   -11.42744  * sf + y);  //  9
            glVertex2f( 12.550578  * sf + x,   -17.30244  * sf + y);  //  10
            glVertex2f( 7.912166  * sf + x,   -20.944  * sf + y);  //  11
            glVertex2f( 3.049705  * sf + x,   -21.65755  * sf + y);  //  12
            glVertex2f( -1.711005  * sf + x,   -21.36664  * sf + y);  //  13
            glVertex2f( -6.283661  * sf + x,   -19.66739  * sf + y);  //  14
            glVertex2f( -10.123329  * sf + x,   -16.79162  * sf + y);  //  15
            glVertex2f( -13.363008  * sf + x,   -12.28184  * sf + y);  //  16
            glVertex2f( -14.675578  * sf + x,   -5.79969  * sf + y);  //  17
            glVertex2f( -13.66821  * sf + x,   0.20179  * sf + y);  //  18
            glVertex2f( -10.385341  * sf + x,   6.27562  * sf + y);  //  19
            glVertex2f( 5.539491  * sf + x,   20.32671  * sf + y);  //  20
            glVertex2f( 10.431588  * sf + x,   28.20584  * sf + y);  //  21
            glVertex2f( 11.00141  * sf + x,   34.71585  * sf + y);  //  22
            glVertex2f( 9.204915  * sf + x,   39.62875  * sf + y);  //  23
            glVertex2f( 7.854166  * sf + x,   42.08262  * sf + y);  //  24
            glVertex2f( 5.481415  * sf + x,   42.66649  * sf + y);  //  25
            glVertex2f( 3.57972  * sf + x,   41.4147  * sf + y);  //  26
            glVertex2f( 1.507889  * sf + x,   37.35642  * sf + y);  //  27
            glVertex2f( -0.381338  * sf + x,   31.14317  * sf + y);  //  28
            glVertex2f( -0.664306  * sf + x,   25.51354  * sf + y);  //  29
            glVertex2f( 8.296044  * sf + x,   -32.22694  * sf + y);  //  30
            glVertex2f( 8.050507  * sf + x,   -36.6687  * sf + y);  //  31
            glVertex2f( 6.496615  * sf + x,   -39.52999  * sf + y);  //  32
            glVertex2f( 3.368583  * sf + x,   -41.7968  * sf + y);  //  33
            glVertex2f( 0.253766  * sf + x,   -42.66649  * sf + y);  //  34
            glVertex2f( -3.599633  * sf + x,   -42.23514  * sf + y);  //  35
            glVertex2f( -8.098754  * sf + x,   -39.46637  * sf + y);  //  36
            glVertex2f( -9.463279  * sf + x,   -35.49796  * sf + y);  //  37
            glVertex2f( -7.08037  * sf + x,   -31.36512  * sf + y);  //  38
            glVertex2f( -3.336421  * sf + x,   -31.14057  * sf + y);  //  39
            glVertex2f( -1.360313  * sf + x,   -34.07738  * sf + y);  //  40
            glVertex2f( -1.608342  * sf + x,   -37.11828  * sf + y);  //  41
            glVertex2f( -5.729949  * sf + x,   -39.24759  * sf + y);  //  42
            glVertex2f( -7.480646  * sf + x,   -36.2136  * sf + y);  //  43
            glVertex2f( -6.826918  * sf + x,   -33.36919  * sf + y);  //  44
            glVertex2f( -4.069083  * sf + x,   -32.9226  * sf + y);  //  45
            glVertex2f( -3.040669  * sf + x,   -34.433  * sf + y);  //  46
            glVertex2f( -3.737535  * sf + x,   -36.38759  * sf + y);  //  47
            glVertex2f( -5.496558  * sf + x,   -36.97633  * sf + y);  //  48
            glVertex2f( -5.295932  * sf + x,   -34.01951  * sf + y);  //  49

            glEnd();

            break;

       case PB_SYMBOL_fClef: // The Base Clef
            drColor(color);
            glLineWidth (3.0);
            glBegin(GL_LINE_STRIP);
                glVertex2f( -15.370325  * sf + x,   -17.42068  * sf + y);  //  1
                glVertex2f( -7.171025  * sf + x,   -13.75432  * sf + y);  //  2
                glVertex2f( -2.867225  * sf + x,   -10.66642  * sf + y);  //  3
                glVertex2f( 0.925165  * sf + x,   -7.03249  * sf + y);  //  4
                glVertex2f( 4.254425  * sf + x,   -0.65527  * sf + y);  //  5
                glVertex2f( 4.762735  * sf + x,   7.77848  * sf + y);  //  6
                glVertex2f( 2.693395  * sf + x,   13.92227  * sf + y);  //  7
                glVertex2f( -1.207935  * sf + x,   16.80317  * sf + y);  //  8
                glVertex2f( -5.526425  * sf + x,   17.42068  * sf + y);  //  9
                glVertex2f( -10.228205  * sf + x,   15.65609  * sf + y);  //  10
                glVertex2f( -13.453995  * sf + x,   10.7128  * sf + y);  //  11
                glVertex2f( -13.133655  * sf + x,   5.43731  * sf + y);  //  12
                glVertex2f( -9.475575  * sf + x,   3.00714  * sf + y);  //  13
                glVertex2f( -5.846445  * sf + x,   4.72159  * sf + y);  //  14
                glVertex2f( -5.395545  * sf + x,   9.72918  * sf + y);  //  15
                glVertex2f( -8.850025  * sf + x,   11.64372  * sf + y);  //  16
                glVertex2f( -11.519385  * sf + x,   10.35816  * sf + y);  //  17
                glVertex2f( -11.706365  * sf + x,   6.8704  * sf + y);  //  18
                glVertex2f( -9.463505  * sf + x,   5.01391  * sf + y);  //  19
                glVertex2f( -7.172075  * sf + x,   5.81649  * sf + y);  //  20
                glVertex2f( -7.189565  * sf + x,   8.62975  * sf + y);  //  21
                glVertex2f( -9.175055  * sf + x,   9.82019  * sf + y);  //  22
                glVertex2f( -10.696425  * sf + x,   8.08395  * sf + y);  //  23
                glVertex2f( -8.843065  * sf + x,   6.66726  * sf + y);  //  24
                glVertex2f( -8.995775  * sf + x,   8.71136  * sf + y);  //  25
            glEnd();

            glBegin(GL_POLYGON);
                glVertex2f( 10  * sf + x,   14  * sf + y);  //  26
                glVertex2f( 14  * sf + x,   14 * sf + y);  //  27
                glVertex2f( 14 * sf + x,    10  * sf + y);  //  28
                glVertex2f( 10  * sf + x,   10  * sf + y);  //  29
                glVertex2f( 10  * sf + x,   14  * sf + y);  //  30
            glEnd();

            glBegin(GL_POLYGON);
                glVertex2f( 10 * sf + x,    4  * sf + y);  //  31
                glVertex2f( 14  * sf + x,   4  * sf + y);  //  32
                glVertex2f( 14  * sf + x,   0  * sf + y);  //  33
                glVertex2f( 10 * sf + x,    0  * sf + y);  //  34
                glVertex2f( 10 * sf + x,    4  * sf + y);  //  35
           glEnd();

          break;

        case PB_SYMBOL_noteHead:
            //ppLogTrace("PB_SYMBOL_noteHead x %f y %f", x, y);
            if (!CChord::isNotePlayable(symbol.getNote(), 0))
            {
                color = Cfg::noteColorDim();
                playable = false;
            }
            drawStaveExtentsion(symbol, x, 16, playable);

            // See forum post at link below from PianoBooster forum user Kory.
            // http://piano-booster.2625608.n2.nabble.com/Pianobooster-port-to-arm-linux-or-Android-td7572459.html
            // http://piano-booster.2625608.n2.nabble.com/Pianobooster-port-to-arm-linux-or-Android-td7572459.html#a7572676
            if (m_settings->coloredNotes() && color == Cfg::noteColor()) //KORY added
            {
                int note = symbol.getNote() % MIDI_OCTAVE;
                switch (note)
                {
                    case 0: //note::PitchLabel::C:
                        color = CColor(1.0, 0.0, 0.0); //Red
                      break;
                    case 1: //note::PitchLabel::C♯:
                        color = CColor(1.0, 0.25, 0.0); //Red
                      break;
                    case 2: //note::PitchLabel::D:
                        color = CColor(1.0, 0.5, 0.0); //Orange
                      break;
                    case 3: //note::PitchLabel::D♯:
                        color = CColor(1.0, 0.75, 0.0); //Orange
                      break;
                    case 4: //note::PitchLabel::E:
                        color = CColor(1.0, 1.0, 0.0); //Yellow
                      break;
                    case 5: //note::PitchLabel::F:
                        color = CColor(0.0, 1.0, 0.0); //Green
                      break;
                    case 6: //note::PitchLabel::F♯:
                        color = CColor(0.0, 0.5, 0.5); //Green
                      break;
                    case 7: //note::PitchLabel::G:
                        color = CColor(0.0, 0.0, 1.0); //Blue
                      break;
                    case 8: //note::PitchLabel::G♯:
                        color = CColor(0.290, 0.0, 0.903); //Blue
                      break;
                    case 9: //note::PitchLabel::A:
                        color = CColor(0.580, 0.0, 0.827); //Dark Violet #9400D3
                      break;
                    case 10: //note::PitchLabel::A♯:
                        color = CColor(0.790, 0.0, 0.903); //Dark Violet #9400D3
                      break;
                    case 11: //note::PitchLabel::B:
                        color = CColor(1.0, 0.0, 1.0); //Magenta
                      break;
                }
            }

            drColor(color);
            glBegin(GL_POLYGON);
                glVertex2f(-7.0 * sf + x,  2.0 * sf + y); // 1
                glVertex2f(-5.0 * sf + x,  4.0 * sf + y); // 2
                glVertex2f(-1.0 * sf + x,  6.0 * sf + y); // 3
                glVertex2f( 4.0 * sf + x,  6.0 * sf + y); // 4
                glVertex2f( 7.0 * sf + x,  4.0 * sf + y); // 5
                glVertex2f( 7.0 * sf + x,  1.0 * sf + y); // 6
                glVertex2f( 6.0 * sf + x, -2.0 * sf + y); // 7
                glVertex2f( 4.0 * sf + x, -4.0 * sf + y); // 8
                glVertex2f( 0.0 * sf + x, -6.0 * sf + y); // 9
                glVertex2f(-4.0 * sf + x, -6.0 * sf + y); // 10
                glVertex2f(-8.0 * sf + x, -3.0 * sf + y); // 11
                glVertex2f(-8.0 * sf + x, -0.0 * sf + y); // 12
            glEnd();

            /*
            // shows the MIDI Duration (but not very useful)
            glLineWidth(4.0);
            drColor(CColor(0.3, 0.4, 0.4));
            glBegin(GL_LINE_STRIP);
                glVertex2f(x,  y);
                glVertex2f(x + CMidiFile::ppqnAdjust(symbol.getMidiDuration()) * HORIZONTAL_SPACING_FACTOR * sf, y);
            glEnd();
            drColor(color);
            */

            checkAccidental(symbol, x, y);
            break;

        case PB_SYMBOL_drum:
            if (!CChord::isNotePlayable(symbol.getNote(), 0))
                color = Cfg::noteColorDim();
            drColor(color);
            glLineWidth (3.0);
            glBegin(GL_LINES);
                glVertex2f( 5.0 * sf + x,-5.0 * sf + y);
                glVertex2f(-5.0 * sf + x, 5.0 * sf + y);
                glVertex2f(-5.0 * sf + x,-5.0 * sf + y);
                glVertex2f( 5.0 * sf + x, 5.0 * sf + y);
            glEnd();
            checkAccidental(symbol, x, y);
            break;

        case PB_SYMBOL_sharp:
            glLineWidth (2.0);
            glBegin(GL_LINES);
                glVertex2f(-2.0 * sf + x, -14.0 * sf + y);
                glVertex2f(-2.0 * sf + x,  14.0 * sf + y);

                glVertex2f( 2.0 * sf + x, -13.0 * sf + y);
                glVertex2f( 2.0 * sf + x,  15.0 * sf + y);

                glVertex2f(-5.0 * sf + x,   4.0 * sf + y);
                glVertex2f( 5.0 * sf + x,   7.0 * sf + y);

                glVertex2f(-5.0 * sf + x,  -6.0 * sf + y);
                glVertex2f( 5.0 * sf + x,  -3.0 * sf + y);
            glEnd();
            break;

         case PB_SYMBOL_flat:
            glLineWidth (2.0);
            glBegin(GL_LINE_STRIP);
                glVertex2f(-4.0 * sf + x, 17.0 * sf + y);  // 1
                glVertex2f(-4.0 * sf + x, -6.0 * sf + y);  // 2
                glVertex2f( 2.0 * sf + x, -2.0 * sf + y);  // 3
                glVertex2f( 5.0 * sf + x,  2.0 * sf + y);  // 4
                glVertex2f( 5.0 * sf + x,  4.0 * sf + y);  // 5
                glVertex2f( 3.0 * sf + x,  5.0 * sf + y);  // 6
                glVertex2f( 0.0 * sf + x,  5.0 * sf + y);  // 7
                glVertex2f(-4.0 * sf + x,  2.0 * sf + y);  // 8
            glEnd();
            break;

         case PB_SYMBOL_natural:
            glLineWidth (2.0);
            glBegin(GL_LINES);
                glVertex2f(  3 * sf + x,   -15  * sf + y);  //  1
                glVertex2f(  3 * sf + x,   8  * sf + y);  //  2

                glVertex2f( -3 * sf + x,   -8  * sf + y);  //  3
                glVertex2f( -3 * sf + x,   15  * sf + y);  //  4

                glVertex2f(  3 * sf + x,   8  * sf + y);  //  5
                glVertex2f( -3 * sf + x,   2  * sf + y);  //  6

                glVertex2f(  3 * sf + x,   -2  * sf + y);  //  7
                glVertex2f( -3 * sf + x,   -8  * sf + y);  //  8
            glEnd();
            break;

        case PB_SYMBOL_barLine:
            x += BEAT_MARKER_OFFSET * HORIZONTAL_SPACING_FACTOR * sf; // the beat markers where entered early so now move them correctly
            glLineWidth (4.0);
            drColor ((m_displayHand == PB_PART_left) ? Cfg::staveColorDim() : Cfg::staveColor());
            oneLine(x, CStavePos(PB_PART_right, 4).getPosYRelative(), x, CStavePos(PB_PART_right, -4).getPosYRelative());
            drColor ((m_displayHand == PB_PART_right) ? Cfg::staveColorDim() : Cfg::staveColor());
            oneLine(x, CStavePos(PB_PART_left, 4).getPosYRelative(), x, CStavePos(PB_PART_left, -4).getPosYRelative());
            break;

        case PB_SYMBOL_barMarker:
            x += BEAT_MARKER_OFFSET * HORIZONTAL_SPACING_FACTOR * sf; // the beat markers where entered early so now move them correctly
            glLineWidth (5.0);
            drColor(Cfg::barMarkerColor());
            oneLine(x, CStavePos(PB_PART_right, m_beatMarkerHeight).getPosYRelative(), x, CStavePos(PB_PART_left, -m_beatMarkerHeight).getPosYRelative());
            glDisable (GL_LINE_STIPPLE);
            break;

        case PB_SYMBOL_beatMarker:
            x += BEAT_MARKER_OFFSET * HORIZONTAL_SPACING_FACTOR * sf; // the beat markers where entered early so now move them correctly
            glLineWidth (4.0);
            drColor(Cfg::beatMarkerColor());
            oneLine(x, CStavePos(PB_PART_right, m_beatMarkerHeight).getPosYRelative(), x, CStavePos(PB_PART_left, -m_beatMarkerHeight).getPosYRelative());
            glDisable (GL_LINE_STIPPLE);
            break;

         case PB_SYMBOL_playingZone:
            {
                float topY = CStavePos(PB_PART_right, m_beatMarkerHeight).getPosY();
                float bottomY = CStavePos(PB_PART_left, -m_beatMarkerHeight).getPosY();
                float early = Cfg::playZoneEarly() * HORIZONTAL_SPACING_FACTOR * sf;

                float late = Cfg::playZoneLate() * HORIZONTAL_SPACING_FACTOR * sf;
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

            if (symbol.getType() >= PB_SYMBOL_noteHead)
                playable = drawNote(&symbol, x, y, slot, color, playable);
            else
                ppDEBUG(("ERROR drawSymbol unhandled symbol\n"));
            break;
    }

    if (symbol.getType() >= PB_SYMBOL_noteHead)
    {
        float pianistX = symbol.getPianistTiming();
        if ( pianistX != NOT_USED)
        {
            pianistX =  x + pianistX * HORIZONTAL_SPACING_FACTOR * sf;
            drColor(CColor(1.0, 1.0, 1.0));
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

void CDraw::drawSlot(CSlot* slot)
{
    CStavePos stavePos;
    //int av8Left = slot->getAv8Left();
    for (int i=0; i < slot->length(); i++)
    {
        stavePos.notePos(slot->getSymbol(i).getHand(), slot->getSymbol(i).getNote());
        //ppLogTrace ("compileSlot len %d id %2d next %2d time %2d type %2d note %2d", slot->length(), slot->m_displayListId,
        //slot->m_nextDisplayListId, slot->getDeltaTime(), slot->getSymbol(i).getType(), slot->getSymbol(i).getNote());

        drawSymbol(slot->getSymbol(i), 0.0, stavePos.getPosYRelative()); // we add this  back when drawing this symbol
    }
}

void CDraw::drawStaves(float startX, float endX)
{
    int i;

    glLineWidth (Cfg::staveThickness());

    /* select color for all lines  */
    drColor ((m_displayHand != PB_PART_left) ? Cfg::staveColor() : Cfg::staveColorDim());
    glBegin(GL_LINES);

    for (i = -4; i <= 4; i+=2 )
    {
        CStavePos pos = CStavePos(PB_PART_right, i);
        glVertex2f (startX, pos.getPosY());
        glVertex2f (endX, pos.getPosY());
    }
    drColor ((m_displayHand != PB_PART_right) ? Cfg::staveColor() : Cfg::staveColorDim());
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
                drColor ((m_displayHand != PB_PART_left) ? Cfg::noteColor() : Cfg::noteColorDim());
                pos = CStavePos(PB_PART_right, sharpLookUpRight[i]);
                drawSymbol( CSymbol(PB_SYMBOL_sharp, pos), Cfg::keySignatureX() + gapX*i );
            }
            if (i < arraySize(sharpLookUpLeft))
            {
                drColor ((m_displayHand != PB_PART_right) ? Cfg::noteColor() : Cfg::noteColorDim());
                pos = CStavePos(PB_PART_left, sharpLookUpLeft[i]);
                drawSymbol( CSymbol(PB_SYMBOL_sharp, pos), Cfg::keySignatureX() + gapX*i );
            }
            key--;
        }
        else
        {
            if (i < arraySize(flatLookUpRight))
            {
                drColor ((m_displayHand != PB_PART_left) ? Cfg::noteColor() : Cfg::noteColorDim());
                pos = CStavePos(PB_PART_right, flatLookUpRight[i]);
                drawSymbol( CSymbol(PB_SYMBOL_flat, pos), Cfg::keySignatureX() + gapX*i );
            }
            if (i < arraySize(flatLookUpLeft))
            {
                drColor ((m_displayHand != PB_PART_right) ? Cfg::noteColor() : Cfg::noteColorDim());
                pos = CStavePos(PB_PART_left, flatLookUpLeft[i]);
                drawSymbol( CSymbol(PB_SYMBOL_flat, pos), Cfg::keySignatureX() + gapX*i );
            }

            key++;
        }
        i++;
    }
}
