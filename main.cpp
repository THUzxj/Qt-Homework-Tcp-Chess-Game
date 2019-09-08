#include "mainwindow.h"
#include "enterwindow.h"
#include "manager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    EnterWindow ew;

    Manager m(&w,&ew);

    ew.show();
    //w.show();

    return a.exec();
}
