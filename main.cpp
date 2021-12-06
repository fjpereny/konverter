/*
Copyright © 2021 Frank Pereny

https://github.com/fjpereny/konverter

This program is free software: you can redistribute it and/or modify it under the terms 
of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.
*/

#include "datatablewindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QString>


// Operating System Selection
#ifdef Q_OS_LINUX
const QString *folder_sep = new QString("/");
#endif

#ifdef Q_OS_MACX
const QString *folder_sep = new QString("/");
#endif

#ifdef Q_OS_WIN32
const QString *folder_sep = new QString("\\");
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "konverter_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    DataTableWindow mainWin(nullptr, folder_sep);
    mainWin.show();
    return a.exec();
}
