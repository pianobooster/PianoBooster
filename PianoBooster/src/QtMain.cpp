/****************************************************************************

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

****************************************************************************/

#include <QApplication>

#include <QtOpenGL>
#include "QtWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

     QString locale = QLocale::system().name();
     printf("locale = %s\n", qPrintable(locale));

     QTranslator translator;
     translator.load(QString("pianobooster_") + locale);

     app.installTranslator(&translator);



    if (!QGLFormat::hasOpenGL()) {
        QMessageBox::information(0, tr("OpenGL support"),
                 tr("This system does not support OpenGL which is needed to run Piano Booster."));
        return -1;
    }



    QtWindow window;

    window.show();
    return app.exec();
}

