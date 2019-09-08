#include "clientdialog.h"
#include "ui_clientdialog.h"

ClientDialog::ClientDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientDialog)
{
    ui->setupUi(this);
}

ClientDialog::~ClientDialog()
{
    delete ui;
}
