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
