#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTranslator>
#include <QLocale>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QTranslator myTranslator;
    myTranslator.load("hydrochat_"+QLocale::system().name(), ":/translations");
    a.installTranslator(&myTranslator);

    QTranslator qtTranslator;
    qtTranslator.load("qt_"+QLocale::system().name(), ":/translations");
    a.installTranslator(&qtTranslator);

    settings.emoticonspath = QDir::toNativeSeparators(QApplication::applicationDirPath());

#ifdef Q_OS_UNIX
    QDir dir(settings.emoticonspath);
    dir.cdUp();
    if (dir.cd("share/hydrochat"))
        settings.emoticonspath = dir.canonicalPath();
#endif
    settings.emoticonspath+=QDir::separator();
    settings.emoticonspath+="emoticons";

    MainWindow w;
    w.show();
    w.checkAutoConnect();
    return a.exec();
}
