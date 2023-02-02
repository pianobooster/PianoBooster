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
    font->FaceSize(FONT_SIZE, FONT_SIZE);
#endif
    m_settings = settings;
    m_displayHand = PB_PART_both;
    m_forceCompileRedraw = 1;
    m_scrollProperties = &m_scrollPropertiesHorizontal;
}

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
        drColor(Cfg::colorTheme().staveColor);
    else
        drColor(Cfg::colorTheme().staveColorDim);

    glLineWidth (Cfg::staveThickness());
    glBegin(GL_LINES);
    while (index >= 6 || index <= -6)
    {

        glVertex2f (x - static_cast<float>(noteWidth)/2.0f - 4.0f, CStavePos(hand, index).getPosYRelative());
        glVertex2f (x + static_cast<float>(noteWidth)/2.0f + 4.0f, CStavePos(hand, index).getPosYRelative());

        // Move the index closer to the stave centre
        if (index > 0)
            index -= 2;
        else
            index += 2;
    }
    glEnd();
}

#define scaleGlVertex(xa, xb, ya, yb) glVertex2f( ((xa) * 1.2f) + (xb), ((ya) * 1.2f) + (yb))

#ifndef NO_USE_FTGL
void CDraw::renderText(float x, float y, const char* s)
{
  const auto w = font->Advance(s);
  const auto h = font->Descender();
  glRasterPos2f(x - w/2, y - h);
  font->Render(s);
}
#endif

void CDraw::drawNoteName(int midiNote, float x, float y, int type)
{
    Q_UNUSED(type)
    // Ignore note that are too high
    if (midiNote > MIDI_TOP_C + 6)
        return;

    staveLookup_t item = CStavePos::midiNote2Name(midiNote);

    drColor(Cfg::colorTheme().noteNameColor);

    glLineWidth (1.0);

#ifdef NO_USE_FTGL
    if (item.accidental != 0)
      {
          const float accidentalOffset = 10;
          x += accidentalOffset/2;
          if (item.accidental == 1)
          {
              glBegin(GL_LINES);
                  //  letterSharp4
                  scaleGlVertex( -1.317895f, x,   5.794585f, y);  //  1
                  scaleGlVertex( -1.265845f, x,   -6.492455f, y);  //  2
                  scaleGlVertex( 1.252305f, x,   6.492455f, y);  //  3
                  scaleGlVertex( 1.322655f, x,   -5.422335f, y);  //  4
                  scaleGlVertex( -2.645765f, x,   1.967805f, y);  //  5
                  scaleGlVertex( 2.648325f, x,   3.625485f, y);  //  6
                  scaleGlVertex( -2.648325f, x,   -3.306965f, y);  //  7
                  scaleGlVertex( 2.596205f, x,   -1.675765f, y);  //  8
              glEnd();

          }
          else
          {
              glBegin(GL_LINE_STRIP);
                  //  letterFlat
                  scaleGlVertex( -2.52933f, x,   6.25291f, y);  //  1
                  scaleGlVertex( -2.50344f, x,   -6.25291f, y);  //  2
                  scaleGlVertex( 0.76991f, x,   -3.63422f, y);  //  3
                  scaleGlVertex( 2.07925ff, x,   -1.67021f, y);  //  4
                  scaleGlVertex( 2.52933f, x,   0.25288f, y);  //  5
                  scaleGlVertex( 1.42458f, x,   1.07122f, y);  //  6
                  scaleGlVertex( -0.53943f, x,   0.90755f, y);  //  7
                  scaleGlVertex( -2.46252f, x,   -1.01554f, y);  //  8
                  scaleGlVertex( -2.50344f, x,   -1.67021f, y);  //  9
              glEnd();

          }
          x -= accidentalOffset;
      }

      switch(item.pianoNote)
      {
      case 1:
          glBegin(GL_LINE_STRIP);
              //  letterC
              scaleGlVertex( 3.513445f, x,   2.17485f, y);  //  1
              scaleGlVertex( 1.880175f, x,   4.47041f, y);  //  2
              scaleGlVertex( -1.598825f, x,   4.4321f, y);  //  3
              scaleGlVertex( -3.692215f, x,   2.26571f, y);  //  4
              scaleGlVertex( -3.702055f, x,   -1.88958f, y);  //  5
              scaleGlVertex( -1.630675f, x,   -4.47041f, y);  //  6
              scaleGlVertex( 1.932545f, x,   -4.44064f, y);  //  7
              scaleGlVertex( 3.702055f, x,   -1.88554f, y);  //  8
          glEnd();
      break;

      case 2:
          glBegin(GL_LINE_STRIP);
              //  letterD
              scaleGlVertex( -3.30696f, x,   4.31878f, y);  //  1
              scaleGlVertex( -3.3428f, x,   -4.31878f, y);  //  2
              scaleGlVertex( 0.9425f, x,   -4.28164f, y);  //  3
              scaleGlVertex( 3.31415f, x,   -1.42302f, y);  //  4
              scaleGlVertex( 3.3428f, x,   1.66626f, y);  //  5
              scaleGlVertex( 0.70825f, x,   4.31317f, y);  //  6
              scaleGlVertex( -3.22083f, x,   4.29495f, y);  //  7
          glEnd();
      break;

      case 3: // E
          glBegin(GL_LINE_STRIP);
              //  letterE2
              scaleGlVertex( 2.966065f, x,   4.416055f, y);  //  1
              scaleGlVertex( -3.007275f, x,   4.403495f, y);  //  2
              scaleGlVertex( -3.037615f, x,   -4.416055f, y);  //  3
              scaleGlVertex( 3.037615f, x,   -4.415435f, y);  //  4
          glEnd();
          glBegin(GL_LINES);
              scaleGlVertex( 3.011705f, x,   0.197675f, y);  //  5
              scaleGlVertex( -2.990845f, x,   0.196615f, y);  //  6
          glEnd();
      break;

      case 4: // F
          glBegin(GL_LINE_STRIP);
              //  letterF2
              scaleGlVertex( -2.55172f, x,   -4.434285f, y);  //  1
              scaleGlVertex( -2.51956f, x,   4.433665ff, y);  //  2
              scaleGlVertex( 2.39942f, x,   4.434285f, y);  //  3
          glEnd();
          glBegin(GL_LINES);
              scaleGlVertex( 2.58143f, x,   0.244465f, y);  //  4
              scaleGlVertex( -2.58143f, x,   0.243405f, y);  //  5

          glEnd();
      break;

      case 5:
          glBegin(GL_LINE_STRIP);
              //  letterG
              scaleGlVertex( 0.58123f, x,   -0.34005f, y);  //  1
              scaleGlVertex( 3.66047f, x,   -0.48722f, y);  //  2
              scaleGlVertex( 3.70461f, x,   -3.23595f, y);  //  3
              scaleGlVertex( 1.96234f, x,   -4.41694f, y);  //  4
              scaleGlVertex( -0.96712f, x,   -4.57846f, y);  //  5
              scaleGlVertex( -3.70461f, x,   -2.29011f, y);  //  6
              scaleGlVertex( -3.67245f, x,   2.14034f, y);  //  7
              scaleGlVertex( -1.25347f, x,   4.57846f, y);  //  8
              scaleGlVertex( 1.90018f, x,   4.55293f, y);  //  9
              scaleGlVertex( 3.54236f, x,   2.38612f, y);  //  10
          glEnd();
      break;

      case 6: // A
          glBegin(GL_LINE_STRIP);
              //  letterA2
              scaleGlVertex( -3.91146f, x,   -4.907395f, y);  //  1
              scaleGlVertex( 0.06571f, x,   4.907395f, y);  //  2
              scaleGlVertex( 3.91146f, x,   -4.803315f, y);  //  3
          glEnd();
          glBegin(GL_LINES);
              scaleGlVertex( 2.60111f, x,   -1.400435f, y);  //  4
              scaleGlVertex( -2.56175f, x,   -1.357305f, y);  //  5

          glEnd();
      break;

      case 7:
          glBegin(GL_LINE_STRIP);
              //  letterB
              scaleGlVertex( 1.038555f, x,   0.105285f, y);  //  1
              scaleGlVertex( -3.001935f, x,   0.121925f, y);  //  2
              scaleGlVertex( -3.027615f, x,   4.417905f, y);  //  3
              scaleGlVertex( 1.325925f, x,   4.451255f, y);  //  4
              scaleGlVertex( 2.737985f, x,   3.048615f, y);  //  5
              scaleGlVertex( 2.721765f, x,   1.366235f, y);  //  6
              scaleGlVertex( 1.022635f, x,   0.120235f, y);  //  7
              scaleGlVertex( 3.026015f, x,   -1.282295f, y);  //  8
              scaleGlVertex( 3.027615f, x,   -3.046285f, y);  //  9
              scaleGlVertex( 1.176815f, x,   -4.451255f, y);  //  10
              scaleGlVertex( -2.981475f, x,   -4.445735f, y);  //  11
              scaleGlVertex( -3.021355f, x,   0.176035f, y);  //  12
          glEnd();
      break;

      default:
          glBegin(GL_LINES);
              glVertex2f(  3.0f + x,   -15.0f  + y);  //  1
              glVertex2f(  3.0f + x,   8.0f    + y);  //  2

              glVertex2f( -3.0f + x,   -8.0f   + y);  //  3
              glVertex2f( -3.0f + x,   15.0f   + y);  //  4

              glVertex2f(  3.0f + x,   8.0f    + y);  //  5
              glVertex2f( -3.0f + x,   2.0f    + y);  //  6

              glVertex2f(  3.0f + x,   -2.0f   + y);  //  7
              glVertex2f( -3.0f + x,   -8.0f   + y);  //  8
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
        //drColor (Cfg::colorTheme().lineColor);
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
    Q_UNUSED(slot)
    const float stemLength  = 34.0f;
    float noteWidth  = 6.0f;

    //ppLogTrace("PB_SYMBOL_noteHead x %f y %f", x, y);
    if (!CChord::isNotePlayable(symbol->getNote(), 0))
    {
        color = Cfg::colorTheme().noteColorDim;
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
            noteWidth += 1.0f;
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(noteWidth + x,  0.0f + y); // 1
            glVertex2f(noteWidth + x, stemLength + y); // 2
        glEnd();
    }

    float offset = stemLength;
    while (stemFlagCount>0)
    {

        glLineWidth(2.0);
        glBegin(GL_LINE_STRIP);
            glVertex2f(noteWidth + x, offset  + y); // 1
            glVertex2f(noteWidth + 8.0f + x, offset - 16.0f + y); // 2
        glEnd();
        offset -= 8;
        stemFlagCount--;
    }

    if (solidNoteHead)
    {
        glBegin(GL_POLYGON);
            glVertex2f(-7.0f + x,  2.0f + y); // 1
            glVertex2f(-5.0f + x,  4.0f + y); // 2
            glVertex2f(-1.0f + x,  6.0f + y); // 3
            glVertex2f( 4.0f + x,  6.0f + y); // 4
            glVertex2f( 7.0f + x,  4.0f + y); // 5
            glVertex2f( 7.0f + x,  1.0f + y); // 6
            glVertex2f( 6.0f + x, -2.0f + y); // 7
            glVertex2f( 4.0f + x, -4.0f + y); // 8
            glVertex2f( 0.0f + x, -6.0f + y); // 9
            glVertex2f(-4.0f + x, -6.0f + y); // 10
            glVertex2f(-8.0f + x, -3.0f + y); // 11
            glVertex2f(-8.0f + x, -0.0f + y); // 12
        glEnd();
    }
    else
    {
        glLineWidth(2.0);
        glBegin(GL_LINE_STRIP);
            glVertex2f(-7.0f + x,  2.0f + y); // 1
            glVertex2f(-5.0f + x,  4.0f + y); // 2
            glVertex2f(-1.0f + x,  6.0f + y); // 3
            glVertex2f( 4.0f + x,  6.0f + y); // 4
            glVertex2f( 7.0f + x,  4.0f + y); // 5
            glVertex2f( 7.0f + x,  1.0f + y); // 6
            glVertex2f( 6.0f + x, -2.0f + y); // 7
            glVertex2f( 4.0f + x, -4.0f + y); // 8
            glVertex2f( 0.0f + x, -6.0f + y); // 9
            glVertex2f(-4.0f + x, -6.0f + y); // 10
            glVertex2f(-8.0f + x, -3.0f + y); // 11
            glVertex2f(-8.0f + x, -0.0f + y); // 12
        glEnd();
    }

    checkAccidental(*symbol, x, y);

    return playable;
}

void CDraw::drawSymbol(CSymbol symbol, float x, float y, CSlot* slot)
{
    const auto &colorTheme = Cfg::colorTheme();
    CColor color = symbol.getColor();
    bool playable = true;

    if (m_displayHand != symbol.getHand() && m_displayHand != PB_PART_both)
    {
        if (color == colorTheme.noteColor)
            color = colorTheme.noteColorDim;
        if (color == colorTheme.staveColor)
            color = colorTheme.staveColorDim;
        playable = false;
    }

    switch (symbol.getType())
    {
         case PB_SYMBOL_gClef: // The Treble Clef
            y += 4;
            drColor(color);
            glLineWidth (3.0f);
            glBegin(GL_LINE_STRIP);
            glVertex2f( -0.011922f  + x,   -16.11494f  + y);  //  1
            glVertex2f( -3.761922f  + x,   -12.48994f  + y);  //  2
            glVertex2f( -4.859633f  + x,   -8.85196f  + y);  //  3
            glVertex2f( -4.783288f  + x,   -5.42815f  + y);  //  4
            glVertex2f( -0.606711f  + x,   -1.11108f  + y);  //  5
            glVertex2f( 5.355545f  + x,   0.48711f  + y);  //  6
            glVertex2f( 10.641104f  + x,   -1.6473f  + y);  //  7
            glVertex2f( 14.293812f  + x,   -6.18241f  + y);  //  8
            glVertex2f( 14.675578f  + x,   -11.42744f  + y);  //  9
            glVertex2f( 12.550578f  + x,   -17.30244f  + y);  //  10
            glVertex2f( 7.912166f  + x,   -20.944f  + y);  //  11
            glVertex2f( 3.049705f  + x,   -21.65755f  + y);  //  12
            glVertex2f( -1.711005f  + x,   -21.36664f  + y);  //  13
            glVertex2f( -6.283661f  + x,   -19.66739f  + y);  //  14
            glVertex2f( -10.123329f  + x,   -16.79162f  + y);  //  15
            glVertex2f( -13.363008f  + x,   -12.28184f  + y);  //  16
            glVertex2f( -14.675578f  + x,   -5.79969f  + y);  //  17
            glVertex2f( -13.66821f  + x,   0.20179f  + y);  //  18
            glVertex2f( -10.385341f  + x,   6.27562f  + y);  //  19
            glVertex2f( 5.539491f  + x,   20.32671f  + y);  //  20
            glVertex2f( 10.431588f  + x,   28.20584f  + y);  //  21
            glVertex2f( 11.00141f  + x,   34.71585f  + y);  //  22
            glVertex2f( 9.204915f  + x,   39.62875f  + y);  //  23
            glVertex2f( 7.854166f  + x,   42.08262f  + y);  //  24
            glVertex2f( 5.481415f  + x,   42.66649f  + y);  //  25
            glVertex2f( 3.57972f  + x,   41.4147f  + y);  //  26
            glVertex2f( 1.507889f  + x,   37.35642f  + y);  //  27
            glVertex2f( -0.381338f  + x,   31.14317f  + y);  //  28
            glVertex2f( -0.664306f  + x,   25.51354f  + y);  //  29
            glVertex2f( 8.296044f  + x,   -32.22694f  + y);  //  30
            glVertex2f( 8.050507f  + x,   -36.6687f  + y);  //  31
            glVertex2f( 6.496615f  + x,   -39.52999f  + y);  //  32
            glVertex2f( 3.368583f  + x,   -41.7968f  + y);  //  33
            glVertex2f( 0.253766f  + x,   -42.66649f  + y);  //  34
            glVertex2f( -3.599633f  + x,   -42.23514f  + y);  //  35
            glVertex2f( -8.098754f  + x,   -39.46637f  + y);  //  36
            glVertex2f( -9.463279f  + x,   -35.49796f  + y);  //  37
            glVertex2f( -7.08037f  + x,   -31.36512f  + y);  //  38
            glVertex2f( -3.336421f  + x,   -31.14057f  + y);  //  39
            glVertex2f( -1.360313f  + x,   -34.07738f  + y);  //  40
            glVertex2f( -1.608342f  + x,   -37.11828f  + y);  //  41
            glVertex2f( -5.729949f  + x,   -39.24759f  + y);  //  42
            glVertex2f( -7.480646f  + x,   -36.2136f  + y);  //  43
            glVertex2f( -6.826918f  + x,   -33.36919f  + y);  //  44
            glVertex2f( -4.069083f  + x,   -32.9226f  + y);  //  45
            glVertex2f( -3.040669f  + x,   -34.433f  + y);  //  46
            glVertex2f( -3.737535f  + x,   -36.38759f  + y);  //  47
            glVertex2f( -5.496558f  + x,   -36.97633f  + y);  //  48
            glVertex2f( -5.295932f  + x,   -34.01951f  + y);  //  49

            glEnd();

            break;

       case PB_SYMBOL_fClef: // The Base Clef
            drColor(color);
            glLineWidth (3.0f);
            glBegin(GL_LINE_STRIP);
                glVertex2f( -15.370325f  + x,   -17.42068f  + y);  //  1
                glVertex2f( -7.171025f  + x,   -13.75432f  + y);  //  2
                glVertex2f( -2.867225f  + x,   -10.66642f  + y);  //  3
                glVertex2f( 0.925165f  + x,   -7.03249f  + y);  //  4
                glVertex2f( 4.254425f  + x,   -0.65527f  + y);  //  5
                glVertex2f( 4.762735f  + x,   7.77848f  + y);  //  6
                glVertex2f( 2.693395f  + x,   13.92227f  + y);  //  7
                glVertex2f( -1.207935f  + x,   16.80317f  + y);  //  8
                glVertex2f( -5.526425f  + x,   17.42068f  + y);  //  9
                glVertex2f( -10.228205f  + x,   15.65609f  + y);  //  10
                glVertex2f( -13.453995f  + x,   10.7128f  + y);  //  11
                glVertex2f( -13.133655f  + x,   5.43731f  + y);  //  12
                glVertex2f( -9.475575f  + x,   3.00714f  + y);  //  13
                glVertex2f( -5.846445f  + x,   4.72159f  + y);  //  14
                glVertex2f( -5.395545f  + x,   9.72918f  + y);  //  15
                glVertex2f( -8.850025f  + x,   11.64372f  + y);  //  16
                glVertex2f( -11.519385f  + x,   10.35816f  + y);  //  17
                glVertex2f( -11.706365f  + x,   6.8704f  + y);  //  18
                glVertex2f( -9.463505f  + x,   5.01391f  + y);  //  19
                glVertex2f( -7.172075f  + x,   5.81649f  + y);  //  20
                glVertex2f( -7.189565f  + x,   8.62975f  + y);  //  21
                glVertex2f( -9.175055f  + x,   9.82019f  + y);  //  22
                glVertex2f( -10.696425f  + x,   8.08395f  + y);  //  23
                glVertex2f( -8.843065f  + x,   6.66726f  + y);  //  24
                glVertex2f( -8.995775f  + x,   8.71136f  + y);  //  25
            glEnd();

            glBegin(GL_POLYGON);
                glVertex2f( 10.0f  + x,   14.0f  + y);  //  26
                glVertex2f( 14.0f  + x,   14.0f + y);  //  27
                glVertex2f( 14.0f + x,    10.0f  + y);  //  28
                glVertex2f( 10.0f  + x,   10.0f  + y);  //  29
                glVertex2f( 10.0f  + x,   14.0f  + y);  //  30
            glEnd();

            glBegin(GL_POLYGON);
                glVertex2f( 10.0f + x,    4.0f  + y);  //  31
                glVertex2f( 14.0f  + x,   4.0f  + y);  //  32
                glVertex2f( 14.0f  + x,   0.0f  + y);  //  33
                glVertex2f( 10.0f + x,    0.0f  + y);  //  34
                glVertex2f( 10.0f + x,    4.0f  + y);  //  35
           glEnd();

          break;

        case PB_SYMBOL_noteHead:
            //ppLogTrace("PB_SYMBOL_noteHead x %f y %f", x, y);
            if (!CChord::isNotePlayable(symbol.getNote(), 0))
            {
                color = Cfg::colorTheme().noteColorDim;
                playable = false;
            }
            drawStaveExtentsion(symbol, x, 16, playable);

            // See forum post at link below from PianoBooster forum user Kory.
            // http://piano-booster.2625608.n2.nabble.com/Pianobooster-port-to-arm-linux-or-Android-td7572459.html
            // http://piano-booster.2625608.n2.nabble.com/Pianobooster-port-to-arm-linux-or-Android-td7572459.html#a7572676
            if (m_settings->coloredNotes() && color == colorTheme.noteColor) //KORY added
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
                glVertex2f(-7.0f + x,  2.0f + y); // 1
                glVertex2f(-5.0f + x,  4.0f + y); // 2
                glVertex2f(-1.0f + x,  6.0f + y); // 3
                glVertex2f( 4.0f + x,  6.0f + y); // 4
                glVertex2f( 7.0f + x,  4.0f + y); // 5
                glVertex2f( 7.0f + x,  1.0f + y); // 6
                glVertex2f( 6.0f + x, -2.0f + y); // 7
                glVertex2f( 4.0f + x, -4.0f + y); // 8
                glVertex2f( 0.0f + x, -6.0f + y); // 9
                glVertex2f(-4.0f + x, -6.0f + y); // 10
                glVertex2f(-8.0f + x, -3.0f + y); // 11
                glVertex2f(-8.0f + x, -0.0f + y); // 12
            glEnd();

            /*
            // shows the MIDI Duration (but not very useful)
            glLineWidth(4.0f);
            drColor(CColor(0.3, 0.4, 0.4));
            glBegin(GL_LINE_STRIP);
                glVertex2f(x,  y);
                glVertex2f(x + CMidiFile::ppqnAdjust(symbol.getMidiDuration()) * HORIZONTAL_SPACING_FACTOR, y);
            glEnd();
            drColor(color);
            */

            checkAccidental(symbol, x, y);
            break;

        case PB_SYMBOL_drum:
            if (!CChord::isNotePlayable(symbol.getNote(), 0))
                color = colorTheme.noteColorDim;
            drColor(color);
            glLineWidth (3.0f);
            glBegin(GL_LINES);
                glVertex2f( 5.0f + x,-5.0f + y);
                glVertex2f(-5.0f + x, 5.0f + y);
                glVertex2f(-5.0f + x,-5.0f + y);
                glVertex2f( 5.0f + x, 5.0f + y);
            glEnd();
            checkAccidental(symbol, x, y);
            break;

        case PB_SYMBOL_sharp:
            glLineWidth (2.0f);
            glBegin(GL_LINES);
                glVertex2f(-2.0f + x, -14.0f + y);
                glVertex2f(-2.0f + x,  14.0f + y);

                glVertex2f( 2.0f + x, -13.0f + y);
                glVertex2f( 2.0f + x,  15.0f + y);

                glVertex2f(-5.0f + x,   4.0f + y);
                glVertex2f( 5.0f + x,   7.0f + y);

                glVertex2f(-5.0f + x,  -6.0f + y);
                glVertex2f( 5.0f + x,  -3.0f + y);
            glEnd();
            break;

         case PB_SYMBOL_flat:
            glLineWidth (2.0f);
            glBegin(GL_LINE_STRIP);
                glVertex2f(-4.0f + x, 17.0f + y);  // 1
                glVertex2f(-4.0f + x, -6.0f + y);  // 2
                glVertex2f( 2.0f + x, -2.0f + y);  // 3
                glVertex2f( 5.0f + x,  2.0f + y);  // 4
                glVertex2f( 5.0f + x,  4.0f + y);  // 5
                glVertex2f( 3.0f + x,  5.0f + y);  // 6
                glVertex2f( 0.0f + x,  5.0f + y);  // 7
                glVertex2f(-4.0f + x,  2.0f + y);  // 8
            glEnd();
            break;

         case PB_SYMBOL_natural:
            glLineWidth (2.0f);
            glBegin(GL_LINES);
                glVertex2f(  3.0f + x,   -15.0f  + y);  //  1
                glVertex2f(  3.0f + x,   8.0f  + y);  //  2

                glVertex2f( -3.0f + x,   -8.0f  + y);  //  3
                glVertex2f( -3.0f + x,   15.0f  + y);  //  4

                glVertex2f(  3.0f + x,   8.0f  + y);  //  5
                glVertex2f( -3.0f + x,   2.0f  + y);  //  6

                glVertex2f(  3.0f + x,   -2.0f  + y);  //  7
                glVertex2f( -3.0f + x,   -8.0f  + y);  //  8
            glEnd();
            break;

        case PB_SYMBOL_barLine:
            x += BEAT_MARKER_OFFSET * HORIZONTAL_SPACING_FACTOR; // the beat markers where entered early so now move them correctly
            glLineWidth (4.0f);
            drColor ((m_displayHand == PB_PART_left) ? colorTheme.staveColorDim : colorTheme.staveColor);
            oneLine(x, CStavePos(PB_PART_right, 4).getPosYRelative(), x, CStavePos(PB_PART_right, -4).getPosYRelative());
            drColor ((m_displayHand == PB_PART_right) ? colorTheme.staveColorDim : colorTheme.staveColor);
            oneLine(x, CStavePos(PB_PART_left, 4).getPosYRelative(), x, CStavePos(PB_PART_left, -4).getPosYRelative());
            break;

        case PB_SYMBOL_barMarker:
            x += BEAT_MARKER_OFFSET * HORIZONTAL_SPACING_FACTOR; // the beat markers where entered early so now move them correctly
            glLineWidth (5.0f);
            drColor(colorTheme.barMarkerColor);
            oneLine(x, CStavePos(PB_PART_right, m_beatMarkerHeight).getPosYRelative(), x, CStavePos(PB_PART_left, -m_beatMarkerHeight).getPosYRelative());
            glDisable (GL_LINE_STIPPLE);
            break;

        case PB_SYMBOL_beatMarker:
            x += BEAT_MARKER_OFFSET * HORIZONTAL_SPACING_FACTOR; // the beat markers where entered early so now move them correctly
            glLineWidth (4.0);
            drColor(colorTheme.beatMarkerColor);
            oneLine(x, CStavePos(PB_PART_right, m_beatMarkerHeight).getPosYRelative(), x, CStavePos(PB_PART_left, -m_beatMarkerHeight).getPosYRelative());
            glDisable (GL_LINE_STIPPLE);
            break;

         case PB_SYMBOL_playingZone:
            {
                float topY = CStavePos(PB_PART_right, m_beatMarkerHeight).getPosY();
                float bottomY = CStavePos(PB_PART_left, -m_beatMarkerHeight).getPosY();
                float early = static_cast<float>(Cfg::playZoneEarly()) * HORIZONTAL_SPACING_FACTOR;
                float late = static_cast<float>(Cfg::playZoneLate()) * HORIZONTAL_SPACING_FACTOR;
                drColor(colorTheme.playingZoneBg);
                glRectf(x-late, topY, x + early, bottomY);
                glLineWidth (2.0f);
                drColor(colorTheme.playingZoneMiddle);
                oneLine(x, topY, x, bottomY );
                glLineWidth (1.0f);
                glColor3f (1.0f, 0.0f, 0.6f);
                drColor(colorTheme.playingZoneBorder);
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
        if ( symbol.getPianistTiming() != NOT_USED)
        {
            auto pianistX = static_cast<float>(symbol.getPianistTiming());
            pianistX =  x + pianistX * HORIZONTAL_SPACING_FACTOR;
            drColor(CColor(1.0, 1.0, 1.0));
            glLineWidth (2.0f);
            glBegin(GL_LINES);
            glVertex2f( 4.0f + pianistX, 4.0f + y);
            glVertex2f(-5.0f + pianistX,-5.0f + y);
            glVertex2f( 4.0f + pianistX,-4.0f + y); // draw pianist note timing markers
            glVertex2f(-5.0f + pianistX, 5.0f + y);
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
    const auto &colorTheme = Cfg::colorTheme();
    drColor ((m_displayHand != PB_PART_left) ? colorTheme.staveColor : colorTheme.staveColorDim);
    glBegin(GL_LINES);

    for (i = -4; i <= 4; i+=2 )
    {
        CStavePos pos = CStavePos(PB_PART_right, i);
        glVertex2f (startX, pos.getPosY());
        glVertex2f (endX, pos.getPosY());
    }
    drColor ((m_displayHand != PB_PART_right) ? colorTheme.staveColor : colorTheme.staveColorDim);
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
    static constexpr int sharpLookUpRight[] = { 4, 1, 5, 2,-1, 3, 0};
    static constexpr int sharpLookUpLeft[]  = { 2,-1, 3, 0,-3, 1,-2};
    static constexpr int flatLookUpRight[]  = { 0, 3,-1, 2,-2, 1,-3};
    static constexpr int flatLookUpLeft[]   = {-2, 1,-3, 0,-4,-1,-5};
    static constexpr float gapX = 11.0f;
    const auto &colorTheme = Cfg::colorTheme();
    CStavePos pos;

    if (key == NOT_USED)
        return;

    int i = 0;
    while (key != 0 )
    {
        if (key > 0)
        {
            if (i < arraySize(sharpLookUpRight))
            {
                drColor ((m_displayHand != PB_PART_left) ? colorTheme.noteColor : colorTheme.noteColorDim);
                pos = CStavePos(PB_PART_right, sharpLookUpRight[i]);
                drawSymbol( CSymbol(PB_SYMBOL_sharp, pos), Cfg::keySignatureX() + gapX * static_cast<float>(i) );
            }
            if (i < arraySize(sharpLookUpLeft))
            {
                drColor ((m_displayHand != PB_PART_right) ? colorTheme.noteColor : colorTheme.noteColorDim);
                pos = CStavePos(PB_PART_left, sharpLookUpLeft[i]);
                drawSymbol( CSymbol(PB_SYMBOL_sharp, pos), Cfg::keySignatureX() + gapX * static_cast<float>(i) );
            }
            key--;
        }
        else
        {
            if (i < arraySize(flatLookUpRight))
            {
                drColor ((m_displayHand != PB_PART_left) ? colorTheme.noteColor : colorTheme.noteColorDim);
                pos = CStavePos(PB_PART_right, flatLookUpRight[i]);
                drawSymbol( CSymbol(PB_SYMBOL_flat, pos), Cfg::keySignatureX() + gapX * static_cast<float>(i) );
            }
            if (i < arraySize(flatLookUpLeft))
            {
                drColor ((m_displayHand != PB_PART_right) ? colorTheme.noteColor : colorTheme.noteColorDim);
                pos = CStavePos(PB_PART_left, flatLookUpLeft[i]);
                drawSymbol( CSymbol(PB_SYMBOL_flat, pos), Cfg::keySignatureX() + gapX * static_cast<float>(i) );
            }

            key++;
        }
        i++;
    }
}
