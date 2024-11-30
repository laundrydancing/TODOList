#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    QIcon windowIcon(":/img/icon/icon5.ico");
    a.setWindowIcon(windowIcon);
    MainWindow w;
    w.show();
    return a.exec();
}
