#include "enterwindow.h"
#include "ui_enterwindow.h"
#include "network.h"

EnterWindow::EnterWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EnterWindow)
{
    ui->setupUi(this);
}

EnterWindow::~EnterWindow()
{
    delete ui;
}

void EnterWindow::on_pushButton_2_clicked()//server
{
    Server* s = new Server;
    s->showDialog();
    emit getServer(s);
}

void EnterWindow::on_pushButton_3_clicked()//client
{
    Client* c = new Client;
    c->showDialog();
    emit getClient(c);
}

void EnterWindow::on_pushButton_clicked()//
{

}
