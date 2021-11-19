#include "datatablewindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QString>


QString folder_sep = "/";
#ifdef Q_OS_WIN32
QString folder_sep = "\\";
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

    // Application File System Settings
    #ifndef Q_OS_WIN32
    QString *folder_seperator = new QString("/");
    #endif
    #ifdef Q_OS_WIN32
    QString *folder_seperator = new QString("\\");
    #endif

    DataTableWindow mainWin(nullptr, folder_seperator);
    mainWin.show();
    return a.exec();
}
