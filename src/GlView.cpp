/*********************************************************************************/
/*!
@file           GlView.cpp

@brief          xxxx

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

#include <QtWidgets>
#include <QtOpenGL>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_1_1>
#include <QPaintEngine>

#include <cmath>

#include "QtWindow.h"
#include "GlView.h"
#include "Cfg.h"
#include "Draw.h"

// This defines the PB Open GL frame per seconds.
// Try to make sure this runs a bit faster than the screen refresh rate of 60z (or 16.6 msec)
#define SCREEN_FRAME_RATE 12 // That 12 msec or 83.3 frames per second

#define REDRAW_COUNT ((m_cfg_openGlOptimise >= 2) ? 1 : 2) // there are two gl buffers but redrawing once is best (set 2 with buggy gl drivers)

#define TEXT_LEFT_MARGIN 30

CGLView::CGLView(QtWindow* parent, CSettings* settings)
    : QOpenGLWidget(parent)
{
    m_qtWindow = parent;
    m_settings = settings;
    m_rating = nullptr;
    m_fullRedrawFlag = true;
    m_themeChange = false;
    m_forcefullRedraw = 0;
    m_forceRatingRedraw = 0;
    m_forceBarRedraw = 0;
    m_allowedTimerEvent = true;

    m_backgroundColor = QColor(0, 0, 0);

    m_song = new CSong();
    m_score = new CScore(m_settings);
    m_displayUpdateTicks = 0;
    m_cfg_openGlOptimise = 0; // zero is no GlOptimise
    m_eventBits = 0;
    BENCHMARK_INIT();
}

CGLView::~CGLView()
{
    delete m_song;
    delete m_score;
    m_titleHeight = 0;
}

QSize CGLView::minimumSizeHint() const
{
    return QSize(200, 200);
}

QSize CGLView::sizeHint() const
{
    return QSize(200, 800); //fixme this does not work
}

void CGLView::stopTimerEvent()
{
    m_allowedTimerEvent=false;
}

void CGLView::startTimerEvent()
{
    m_allowedTimerEvent=true;
}

void CGLView::reportColorThemeChange()
{
    const auto &noteColor = Cfg::colorTheme().noteColor;
    m_themeChange = true;
    m_fullRedrawFlag = true;
    m_score->refreshNoteColor(noteColor);
    m_song->refreshScroll();
    m_score->refreshScroll();
    CDraw::forceCompileRedraw();
    repaint();
}

void CGLView::paintGL()
{
    BENCHMARK(2, "enter");

    if (m_themeChange) {
        auto *const ctx = context();
        auto *const funcs = ctx->functions();
        const auto &colorTheme = Cfg::colorTheme();
        const auto &backgroundColor = colorTheme.backgroundColor;
        funcs->glClearColor(backgroundColor.red, backgroundColor.green, backgroundColor.blue, 0.0);
        funcs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_themeChange = false;
    }

    m_displayUpdateTicks = 0;

    if (m_fullRedrawFlag)
        m_forcefullRedraw = m_forceRatingRedraw = m_forceBarRedraw = REDRAW_COUNT;

    if (m_forcefullRedraw) // clear the screen only if we are doing a full redraw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    glLoadIdentity();
    //BENCHMARK(3, "glLoadIdentity");

    drawDisplayText();
    BENCHMARK(4, "drawDisplayText");

    drawAccurracyBar();
    BENCHMARK(5, "drawAccurracyBar");

    drawBarNumber();
    BENCHMARK(6, "drawBarNumber");

    if (m_forcefullRedraw)
        m_score->drawScore();

    drawTimeSignature();

    updateMidiTask();
    m_score->drawScroll(m_forcefullRedraw);
    BENCHMARK(10, "drawScroll");

    if (m_forcefullRedraw) m_forcefullRedraw--;
    BENCHMARK(11, "exit");
    BENCHMARK_RESULTS();
}

void CGLView::drawTimeSignature()
{
    if (m_forcefullRedraw == 0)
        return;

    float x,y;
    int topNumber, bottomNumber;

    if (m_song == nullptr) return;

    m_song->getTimeSig(&topNumber, &bottomNumber);
    if (topNumber == 0 ) return;

    char bufferTop[10], bufferBottom[10];
    sprintf(bufferTop, "%d", topNumber);
    sprintf(bufferBottom, "%d", bottomNumber);

    x = Cfg::timeSignatureX();

    CDraw::drColor ((CDraw::getDisplayHand() != PB_PART_left) ? Cfg::colorTheme().noteColor : Cfg::colorTheme().noteColorDim);

    y = CStavePos(PB_PART_right,  0).getPosY() + 5;
    renderText(x,y, 0, bufferTop, m_timeSigFont);
    y = CStavePos(PB_PART_right, -3).getPosY() - 2;
    renderText(x,y, 0, bufferBottom, m_timeSigFont);

    CDraw::drColor ((CDraw::getDisplayHand() != PB_PART_right) ? Cfg::colorTheme().noteColor : Cfg::colorTheme().noteColorDim);

    y = CStavePos(PB_PART_left,   0).getPosY() + 5;
    renderText(x,y, 0, bufferTop, m_timeSigFont);
    y = CStavePos(PB_PART_left,  -3).getPosY() - 2;
    renderText(x,y, 0, bufferBottom, m_timeSigFont);
}

void CGLView::drawAccurracyBar()
{
    if (m_song->getPlayMode() == PB_PLAY_MODE_listen || !m_settings->getWarningMessage().isEmpty())
        return;

    if (m_forceRatingRedraw == 0)
        return;
    m_forceRatingRedraw--;

    float accuracy;
    CColor color;

    float y = static_cast<float>(Cfg::getAppHeight() - 14);
    const float x = static_cast<float>(accuracyBarStart);
    const int width = 360;
    const int lineWidth = 8/2;

    m_rating->calculateAccuracy();

    const auto &colorTheme = Cfg::colorTheme();
    accuracy = m_rating->getAccuracyValue();
    color = m_rating->getAccuracyColor();
    CDraw::drColor (color);
    glRectf(x, y - lineWidth, x + width * accuracy, y + lineWidth);
    CDraw::drColor (colorTheme.backgroundColor);
    glRectf(x + width * accuracy, y - lineWidth, x + width, y + lineWidth);

    glLineWidth (1);
    CDraw::drColor(colorTheme.textColor);
    glBegin(GL_LINE_LOOP);
    glVertex2f (x, y + lineWidth);
    glVertex2f (x+ width, y  + lineWidth);
    glVertex2f (x + width, y - lineWidth);
    glVertex2f (x, y - lineWidth);
    glEnd();
}

void CGLView::drawDisplayText()
{
    if (m_rating == nullptr)
    {
        m_rating = m_song->getRating();
        return; // don't run this func the first time it is called
    }

    if (m_forcefullRedraw == 0)
        return;

    int y = Cfg::getAppHeight() - 14;

    if (!m_settings->getWarningMessage().isEmpty())
    {
        glColor3f(1.0f,0.2f,0.0f);
        renderText(TEXT_LEFT_MARGIN, y-4, 0, m_settings->getWarningMessage(), m_timeRatingFont);
        return;
    }

    const auto &colorTheme = Cfg::colorTheme();
    CDraw::drColor(colorTheme.textColor);

    if (m_song->getPlayMode() != PB_PLAY_MODE_listen) {
        if (accuracyBarStart == 0) {
            QFontMetrics fm(m_timeRatingFont);
            accuracyText = tr("Accuracy:");
            accuracyBarStart=fm.boundingRect(accuracyText + "  ").right() + TEXT_LEFT_MARGIN;
       }

        renderText(TEXT_LEFT_MARGIN, y-4,0 ,accuracyText, m_timeRatingFont);
    }

    if (m_titleHeight < 45 )
        return;

    y = Cfg::getAppHeight() - m_titleHeight;

    renderText(TEXT_LEFT_MARGIN, y+6, 0,tr("Song:") + " " + m_song->getSongTitle(), m_timeRatingFont);
    /*
    char buffer[100];
    sprintf(buffer, "Notes %d wrong %d Late %d Score %4.1f%%",
    m_rating->totalNoteCount(), m_rating->wrongNoteCount(),
    m_rating->lateNoteCount(), m_rating->rating());
    renderText(10,20, buffer, m_timeRatingFont);
    */
}

void CGLView::drawBarNumber()
{
    if (m_forceBarRedraw == 0)
        return;
    m_forceBarRedraw--;

    const auto y = static_cast<float>(Cfg::getAppHeight() - m_titleHeight - 34);
    const auto x = static_cast<float>(TEXT_LEFT_MARGIN);

    CDraw::drColor (Cfg::colorTheme().textColor);
    renderText(x, y, 0, tr("Bar:") + " " + QString::number(m_song->getBarNumber()), m_timeRatingFont);
}

void CGLView::resizeGL(int width, int height)
{
    const int maxSoreWidth = 1024;
    const int staveEndGap = 20;
    const int heightAboveStave =  static_cast<int>(CStavePos::verticalNoteSpacing() * MAX_STAVE_INDEX);
    const int heightBelowStave =  static_cast<int>(CStavePos::verticalNoteSpacing() * - MIN_STAVE_INDEX);
    const int minTitleHeight = 20;
    const int minStaveGap = 120;
    int staveGap;
    int maxSoreHeight;

    //int space = height - (heightAboveStave + heightBelowStave + minTitleHeight + minStaveGap);
    //m_titleHeight = qBound(minTitleHeight, minTitleHeight + space/2, 70);
    // staveGap = qBound(minStaveGap, minStaveGap+ space/2, static_cast<int>(CStavePos::staveHeight() * 3));
    if (height < 430)  // So it works on an eeepc 701 (for Trev)
    {
        staveGap = minStaveGap;
        m_titleHeight = minTitleHeight;
    }
    else
    {
        staveGap = static_cast<int>(CStavePos::staveHeight() * 3);
        m_titleHeight = 60;
    }
    maxSoreHeight = heightAboveStave + heightBelowStave + staveGap + m_titleHeight;
    if (height > maxSoreHeight) {
        int scoreExtraTopGap = (height - maxSoreHeight);
        maxSoreHeight += qMin(200, scoreExtraTopGap);
    }
    int sizeX = qMin(width, maxSoreWidth);
    int sizeY = qMin(height, maxSoreHeight);
    int x = 0;
    int y = (height - sizeY) - 5;
    glViewport (x, y, sizeX, sizeY);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, sizeX, 0, sizeY, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    CStavePos::setStaveCenterY(static_cast<float>(sizeY) - static_cast<float>(maxSoreHeight)/2.0f - static_cast<float>(m_titleHeight)/2.0f);
    Cfg::setAppDimentions(x, y, sizeX, sizeY);
    Cfg::setStaveEndX(static_cast<float>(sizeX - staveEndGap));
    CStavePos::setStaveCentralOffset(static_cast<float>(staveGap)/2.0f);
    CDraw::forceCompileRedraw();
}

void CGLView::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void CGLView::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void CGLView::initializeGL()
{
    CColor color = Cfg::colorTheme().backgroundColor;
    glClearColor (color.red, color.green, color.blue, 0.0);
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glShadeModel (GL_FLAT);
    //glEnable(GL_TEXTURE_2D);                        // Enable Texture Mapping

    //from initCheck();
    glShadeModel(GL_FLAT);
    //glEnable(GL_DEPTH_TEST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //glGenTextures(1, &texName);
    //glBindTexture(GL_TEXTURE_2D, texName);

    if (format().samples() > 0) {
        glEnable(GL_MULTISAMPLE);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //enableAntialiasedLines();

    // This is a work around for Windows different display scaling option
    int widgetPointSize = m_qtWindow->font().pointSize();
    m_timeSigFont =  QFont("Arial", widgetPointSize*2 );
    m_timeRatingFont =  QFont("Arial", static_cast<int>(widgetPointSize * 1.2) );

    Cfg::setStaveEndX(400);        //This value get changed by the resizeGL func

    m_song->setActiveHand(PB_PART_both);

    setFocusPolicy(Qt::ClickFocus);
    m_qtWindow->init();

    m_score->init();

    m_song->regenerateChordQueue();

    // increased the tick time for MIDI handling

    m_timer.start(Cfg::tickRate, this );

    m_realtime.start();

    //startMediaTimer(12, this );
}

void CGLView::updateMidiTask()
{
    const auto ticks = m_realtime.restart();
    m_displayUpdateTicks += ticks;
    m_eventBits |= m_song->task(ticks);
}

void CGLView::timerEvent(QTimerEvent *event)
{
    if (!m_allowedTimerEvent) return;

    BENCHMARK(0, "timer enter");
    if (event->timerId() != m_timer.timerId())
    {
         QWidget::timerEvent(event);
         return;
    }

    updateMidiTask();
    BENCHMARK(1, "m_song task");

    if (m_displayUpdateTicks < SCREEN_FRAME_RATE)
        return;

    m_displayUpdateTicks = 0;

    if (m_eventBits != 0)
    {
        if ((m_eventBits & EVENT_BITS_UptoBarReached) != 0)
            m_song->playFromStartBar();
        if ((m_eventBits & EVENT_BITS_forceFullRedraw) != 0)
            m_forcefullRedraw = m_forceRatingRedraw = m_forceBarRedraw = REDRAW_COUNT;
        if ((m_eventBits & EVENT_BITS_forceRatingRedraw) != 0)
            m_forceRatingRedraw = REDRAW_COUNT;
        if ((m_eventBits & EVENT_BITS_newBarNumber) != 0)
            m_forcefullRedraw = m_forceRatingRedraw = m_forceBarRedraw = REDRAW_COUNT; // fixme this did not work so redraw everything

        m_qtWindow->songEventUpdated(m_eventBits);
        m_eventBits = 0;
    }

    if( m_cfg_openGlOptimise == 0 ) // zero is no GlOptimise
        m_fullRedrawFlag = true;
    else
        m_fullRedrawFlag = false;

    //glDraw();
    update();
    m_fullRedrawFlag = true;
    BENCHMARK(19, "timer exit");
}

void CGLView::mediaTimerEvent(int ticks)
{
    Q_UNUSED(ticks)
}

#ifndef QT_OPENGL_ES

static void qt_save_gl_state(QOpenGLFunctions_1_1 *functions1_1)
{
    functions1_1->glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    functions1_1->glPushAttrib(GL_ALL_ATTRIB_BITS);
    functions1_1->glMatrixMode(GL_TEXTURE);
    functions1_1->glPushMatrix();
    functions1_1->glLoadIdentity();
    functions1_1->glMatrixMode(GL_PROJECTION);
    functions1_1->glPushMatrix();
    functions1_1->glMatrixMode(GL_MODELVIEW);
    functions1_1->glPushMatrix();

    functions1_1->glShadeModel(GL_FLAT);
    functions1_1->glDisable(GL_CULL_FACE);
    functions1_1->glDisable(GL_LIGHTING);
    functions1_1->glDisable(GL_STENCIL_TEST);
    functions1_1->glDisable(GL_DEPTH_TEST);
    functions1_1->glEnable(GL_BLEND);
    functions1_1->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

static void qt_restore_gl_state(QOpenGLFunctions_1_1 *functions1_1)
{
    functions1_1->glMatrixMode(GL_TEXTURE);
    functions1_1->glPopMatrix();
    functions1_1->glMatrixMode(GL_PROJECTION);
    functions1_1->glPopMatrix();
    functions1_1->glMatrixMode(GL_MODELVIEW);
    functions1_1->glPopMatrix();
    functions1_1->glPopAttrib();
    functions1_1->glPopClientAttrib();
}

static void qt_gl_draw_text(QOpenGLFunctions *functions, QPainter *p, int x, int y, const QString &str,
                            const QFont &font)
{
    GLfloat color[4];
    functions->glGetFloatv(GL_CURRENT_COLOR, &color[0]);

    const auto col = QColor::fromRgbF(color[0], color[1], color[2],color[3]);
    QPen old_pen = p->pen();
    QFont old_font = p->font();

    p->setPen(col);
    p->setFont(font);
    p->drawText(x, y, str);

    p->setPen(old_pen);
    p->setFont(old_font);
}

static inline void transform_point(GLdouble out[4], const GLdouble m[16], const GLdouble in[4])
{
#define M(row,col)  m[col*4+row]
    out[0] =
        M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
    out[1] =
        M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
    out[2] =
        M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
    out[3] =
        M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
#undef M
}

static inline GLint qgluProject(GLdouble objx, GLdouble objy, GLdouble objz,
           const GLdouble model[16], const GLdouble proj[16],
           const GLint viewport[4],
           GLdouble * winx, GLdouble * winy, GLdouble * winz)
{
   GLdouble in[4], out[4];

   in[0] = objx;
   in[1] = objy;
   in[2] = objz;
   in[3] = 1.0;
   transform_point(out, model, in);
   transform_point(in, proj, out);

   if (in[3] == 0.0)
      return GL_FALSE;

   in[0] /= in[3];
   in[1] /= in[3];
   in[2] /= in[3];

   *winx = viewport[0] + (1 + in[0]) * viewport[2] / 2;
   *winy = viewport[1] + (1 + in[1]) * viewport[3] / 2;

   *winz = (1 + in[2]) / 2;
   return GL_TRUE;
}

#endif // !QT_OPENGL_ES

/*!
 * \brief Renders the string \a str into the GL context of this widget.
 *
 * \a x, \a y and \a z are specified in scene or object coordinates
 * relative to the currently set projection and model matrices. This
 * can be useful if you want to annotate models with text labels and
 * have the labels move with the model as it is rotated etc.
 *
 * \remarks
 * - Taken from Qt 5's QGLWidget implementation (found in
 *   `qtbase/src/opengl/qgl.cpp`). The same counts for the static helper
 *   functions defined above.
 * - Handling the translation for the \a z coordinate has been removed
 *   as it is not used here anyways.
 */
void CGLView::renderText(double x, double y, double z, const QString &str, const QFont & font = QFont()) {
#ifndef QT_OPENGL_ES
    auto *const ctx = context();
    auto *const gl1funcs =
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_1_1>(ctx);
#else
ctx->versionFunctions<QOpenGLFunctions_1_1>();
#endif
    if (!ctx->isOpenGLES() && gl1funcs) {
        if (str.isEmpty() || !isValid())
            return;

        QOpenGLFunctions *const funcs = ctx->functions();

        QPaintEngine *engine = paintEngine();

        QPainter *p;
        bool reuse_painter = false;
        bool use_depth_testing = funcs->glIsEnabled(GL_DEPTH_TEST);
        bool use_scissor_testing = funcs->glIsEnabled(GL_SCISSOR_TEST);

        qt_save_gl_state(gl1funcs);

        if (engine->isActive()) {
            reuse_painter = true;
            p = engine->painter();
        } else {
            p = new QPainter(this);
        }

        const auto *const dev = p->device();
        const auto width = dev ? dev->width() : this->width();
        const auto height = dev ? dev->height() : this->height();
        const auto dpr = dev ? dev->devicePixelRatioF() : 1.0;
        GLdouble model[4 * 4], proj[4 * 4];
        GLint view[4];
        gl1funcs->glGetDoublev(GL_MODELVIEW_MATRIX, &model[0]);
        gl1funcs->glGetDoublev(GL_PROJECTION_MATRIX, &proj[0]);
        funcs->glGetIntegerv(GL_VIEWPORT, &view[0]);
        GLdouble win_x = 0, win_y = 0, win_z = 0;
        qgluProject(x, y, z, &model[0], &proj[0], &view[0],
                    &win_x, &win_y, &win_z);
        win_x /= dpr;
        win_y /= dpr;
        win_y = height + 4 - win_y; // y is inverted
        // note: Not sure why this is rendered 4 pixels to high, just putting "+ 4" there for now.

        QRect viewport(view[0], view[1], view[2], view[3]);
        if (!use_scissor_testing && viewport != rect()) {
            funcs->glScissor(view[0], view[1], view[2], view[3]);
            funcs->glEnable(GL_SCISSOR_TEST);
        } else if (use_scissor_testing) {
            funcs->glEnable(GL_SCISSOR_TEST);
        }
        funcs->glViewport(0, 0, static_cast<GLsizei>(width * dpr), static_cast<GLsizei>(height * dpr));
        gl1funcs->glAlphaFunc(GL_GREATER, 0.0);
        funcs->glEnable(GL_ALPHA_TEST);
        if (use_depth_testing)
            funcs->glEnable(GL_DEPTH_TEST);

        qt_gl_draw_text(funcs, p, qRound(win_x), qRound(win_y), str, font);

        if (!reuse_painter) {
            p->end();
            delete p;
        }

        qt_restore_gl_state(gl1funcs);

        return;
    }
#else // QT_OPENGL_ES
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(z);
    Q_UNUSED(str);
    Q_UNUSED(font);
#endif
    qWarning("QGLWidget::renderText is not supported under OpenGL/ES");
}
