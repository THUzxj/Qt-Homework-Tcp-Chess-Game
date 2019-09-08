#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    view  = new QGraphicsView(this);
    view->move(0,0);
}

MainWindow::~MainWindow()
{
    delete ui;
}
