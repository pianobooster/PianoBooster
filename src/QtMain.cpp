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

#include <QApplication>

#include <cstdlib>

#include "QtWindow.h"
#include "version.h"

int main(int argc, char *argv[]){
    QCoreApplication::setOrganizationName(QStringLiteral("PianoBooster"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("https://github.com/pianobooster/PianoBooster"));
    QCoreApplication::setApplicationName(QStringLiteral("Piano Booster"));
    QCoreApplication::setApplicationVersion(QStringLiteral(PB_VERSION));
    QGuiApplication::setDesktopFileName(QStringLiteral("pianobooster"));

    {
        QCoreApplication app(argc, argv);
        QStringList argList = QCoreApplication::arguments();
        for (const QString &arg : argList){
            if (arg == QLatin1String("--version")) {
                fprintf(stdout, "pianobooster " PB_VERSION "\n");
                return EXIT_SUCCESS;
            }
        }
    }

    QApplication app(argc, argv);
    QtWindow window;
    window.show();

    int value = app.exec();
    closeLogs();
    return value;
}
