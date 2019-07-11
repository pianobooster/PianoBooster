/****************************************************************************

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

****************************************************************************/

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#include <QApplication>
#include <QtOpenGL>
#include "QtWindow.h"
#include "version.txt"

int main(int argc, char *argv[]){
    QCoreApplication::setOrganizationName("PianoBooster");
    QCoreApplication::setOrganizationDomain("https://github.com/captnfab/PianoBooster");
    QCoreApplication::setApplicationName("Piano Booster");
    QCoreApplication::setApplicationVersion(PB_VERSION);

    {
        QCoreApplication app(argc, argv);
        QStringList argList = QCoreApplication::arguments();
        for (QString arg:argList){
            if (arg=="--version"){
                fprintf(stdout, "pianobooster " PB_VERSION "\n");
                exit(0);
            }
        }
    }

    QApplication app(argc, argv);

    if (!QGLFormat::hasOpenGL()) {
        QMessageBox::information(0, QMessageBox::tr("OpenGL support"),
                 QMessageBox::tr("This system does not support OpenGL which is needed to run Piano Booster."));
        return -1;
    }



    QtWindow window;

    window.show();

    int value = app.exec();
    closeLogs();
    return value;
}

