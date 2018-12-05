#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/assets/assets/icon.png"));

    QFile styleSheetFile("://styles/stylesheet.css");
    if(styleSheetFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&styleSheetFile);
        a.setStyleSheet(in.readAll());
    }

    QFontDatabase::addApplicationFont(":/fonts/assets/Raleway-SemiBold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/assets/Raleway-Light.ttf");
    QFontDatabase::addApplicationFont(":/fonts/assets/Montserrat-Regular.otf");
    QFontDatabase::addApplicationFont(":/fonts/assets/Montserrat-Light.otf");

    MainWindow *w = new MainWindow;
    w->show();

    QTranslator qtTranslator;
        if (qtTranslator.load(QLocale::system(),
                    "qt", "_",
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        {
            qDebug() << "qtTranslator ok";
            a.installTranslator(&qtTranslator);
        }

        QTranslator qtBaseTranslator;
        if (qtBaseTranslator.load("qtbase_" + QLocale::system().name(),
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        {
            qDebug() << "qtBaseTranslator ok";
            a.installTranslator(&qtBaseTranslator);
        }

    return a.exec();
}
