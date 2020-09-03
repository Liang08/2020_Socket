#include "mainwindow.h"
#include "clientwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    ClientWindow w_1, w_2;
    w_1.show();
    w_2.show();
    return a.exec();
}
