#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    if (w.connectSQLite()) {
        //On charge les bookmarks existant avant d'afficher la fenêtre
        w.displayBookmarks();
        w.show();
        return a.exec();
    } else {
        return 3;
    }
}
