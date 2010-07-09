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

    QString localeDirectory =
#ifdef Q_OS_WIN32
        QApplication::applicationDirPath() + "/translations/";
#endif
#ifdef Q_OS_LINUX
        QApplication::applicationDirPath() + "/../share/" + QSTR_APPNAME + "/translations/";
#endif
#ifdef Q_OS_DARWIN
        QApplication::applicationDirPath() + "/../Resources/translations/";
#endif


     QTranslator translator;
     if (!translator.load(QSTR_APPNAME + QString("_") + locale , localeDirectory))
        if (!translator.load(QSTR_APPNAME + QString("_") + locale, QApplication::applicationDirPath()  + "/translations/"))
            translator.load(QSTR_APPNAME + QString("_") + locale, QApplication::applicationDirPath());

     app.installTranslator(&translator);



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

