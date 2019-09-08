#include "serverdialog.h"
#include "ui_serverdialog.h"

ServerDialog::ServerDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerDialog)
{
    ui->setupUi(this);
}

ServerDialog::~ServerDialog()
{
    delete ui;
}
